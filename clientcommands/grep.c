/*
    This file is part of lightweight.

    lightweight is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    lightweight is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with lightweight; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <lightweight.h>
#include <globalexterns.h>
#include <channelsdb.h>
#include <accountsdb.h>
#include <channels.h>
#include <pcre.h>
#include <time.h>
#include <errno.h>

/* local */
typedef int BOOL;

#define FALSE 0
#define TRUE 1

#define	HITS		20
#define	MAX_HITS	1000

/* globals */
static pcre *cpattern;
static pcre_extra *hints;
static BOOL invert = FALSE;
static int count;
static int max_hits;

/*
 * grep: search thru L's logfiles w. PCRE pattern matching
 * 
 * Parameters:
 *  -i        - ignore case
 *  -v        - reverse matching
 *  pattern   - search for this PCRE pattern
 *
 * Requires oper powah
 */

void Usage(struct user *user, int grep)
{
  if (grep) {
    NoticeToUser(user, "Usage: grep [-ivl num] pattern");
    NoticeToUser(user, "       i       - ignore upper/lower case distinctions during comparisons");
    NoticeToUser(user, "       v       - invert the sense of matching, to select non-matching lines");
    NoticeToUser(user, "       l num   - list max <num> amounts of lines (20 by default)");
    NoticeToUser(user, "       pattern - a perl compatible regular expression (PCRE)");
    NoticeToUser(user, "             ie: #channel2|user9|Cleanup or [a-z]{7}\\s[0-9]{2}");
  } else {
    NoticeToUser(user, "Usage: find [-l num] pattern");
    NoticeToUser(user, "       l num   - list max <num> amounts of lines (20 by default)");
    NoticeToUser(user, "       pattern - pattern to search for");
  }
  return;
}

void pcregrep(FILE * in, char *name, struct user *user)
{
  time_t ts;
  BOOL match;
  int c, length, offsets[99];
  char buf[4096], line[4096], time[256];

  while (fgets(buf, sizeof(buf), in) != NULL && count < max_hits) {
    length = (int) strlen(buf);
    if (length > 0 && buf[length - 1] == '\n')
      buf[--length] = 0;

    if (strstr(buf, "SendLine:")) /* skip debug lines */
      continue;

    match = pcre_exec(cpattern, hints, buf, length, 0, 0, offsets, 99) >= 0;
    if (match != invert) {
      count++;
      if ((c = (sscanf(buf, "%s %[^\n]", time, line))) == 2) {
        time[strlen(time) - 1] = '\0';
        ts = strtol(time, NULL, 10);
        strftime(time, 100, "%A, %Y.%m.%d %T", localtime(&ts));
        NoticeToUser(user, "%s: %s", time, line);
      } else {
        NoticeToUser(user, "%s", buf);
      }
    }
  }
}

void dogrep(struct user *user, char *tail)
{
  FILE *fp;
  int i = 1, errptr, opt = 0;
  const char *error;
  char *options, *pattern, *ep;
  char logfile[256];

  /* Check auth level.. */
  if (!CheckAuthLevel(user, 250))
    return;

  options = tail;
  pattern = SeperateWord(tail);

  if (NULL == options) {
    Usage(user, 1);
    return;
  }

  invert = FALSE;
  max_hits = HITS;
  if ('-' == options[0]) {
    while (options[i]) {
      switch (options[i++]) {
      case 'i':
        opt |= PCRE_CASELESS;
        break;
      case 'v':
        invert = TRUE;
        break;
      case 'l':
        if (NULL != pattern) {
          if ((max_hits = (int) strtol(pattern, &ep, 10)) > MAX_HITS)
            max_hits = MAX_HITS;
          if (0 == max_hits)
            max_hits = HITS;
          pattern = SeperateWord(pattern);
        }
        break;
      case 'V':
        NoticeToUser(user, "PCRE version %s", pcre_version());
        break;
      default:
        NoticeToUser(user, "unknown option \"%c\"", options[i - 1]);
        return;
        break;
      }
    }
  } else {
    pattern = options;
  }

  SeperateWord(pattern);

  if (NULL == pattern) {
    Usage(user, 1);
    return;
  }

  if (NULL == (cpattern = pcre_compile(pattern, opt, &error, &errptr, NULL))) {
    NoticeToUser(user, "error in regex at offset %d: %s", errptr, error);
    return;
  }

  hints = pcre_study(cpattern, 0, &error);
  if (NULL != error) {
    NoticeToUser(user, "error while studying regex: %s", error);
    return;
  }

  NoticeToUser(user, "Log entries%smatching \"%s\"", (TRUE == invert) ? " NOT " : " ", pattern);

  count = 0;
  for (i = NUMBEROFLOGFILES; (i >= 0 && count < max_hits); i--) {
    snprintf(logfile, 256, "%s.%i", LOGFILE, i);
    if (NULL == (fp = fopen(logfile, "r"))) {
      NoticeToUser(user, "failed to open: %s (%s)", logfile, strerror(errno));
    } else {
      pcregrep(fp, logfile, user);
      fclose(fp);
    }
  }

  if (cpattern != NULL)
    free(cpattern);
  if (hints != NULL)
    free(hints);

  if (count >= max_hits)
    NoticeToUser(user, "--- More than %d hits, list truncated", max_hits);

  NoticeToUser(user, "--- End of list - %d matc%s", count, (1 == count) ? "h" : "hes");
  NoticeToUser(user, "Done.");
}

void dofind(struct user *user, char *tail)
{
  char s[2048], *p;

  /* Check auth level.. */
  if (!CheckAuthLevel(user, 250))
    return;

  if (NULL == tail) {
    Usage(user, 0);
    return;
  }

  strcpy(s, "-i");
  if ('-' == tail[0] && 'l' == tail[1]) {
    tail++;
  } else {
    strcat(s, " ");
  }
  p = &s[strlen(s)];

  while (*tail != '\0' && *tail != '\n') {
    switch (*tail) {
    case '\\':
    case '^':
    case '$':
    case '.':
    case '[':
    case ']':
    case '{':
    case '}':
    case '|':
    case '(':
    case ')':
    case '?':
    case '+':
      *p++ = '\\';
      *p++ = *tail++;
      break;
    case '%':
    case '*':
      *p++ = '.';
      *p++ = '*';
      tail++;
      break;
/*
      case ' ':
        *p++ = '\\';
        *p++ = 's';
        tail++;
        break;
*/
    default:
      *p++ = *tail++;
      break;
    }
  }
  *p = '\0';
  NoticeToUser(user, "(Executing \"grep %s\")", s);
  dogrep(user, s);
}

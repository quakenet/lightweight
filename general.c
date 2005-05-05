/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: general.c,v 1.20 2003/09/08 01:19:25 zarjazz Exp $
 *
 *******************************************************************************/

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
#include <ctype.h>
#include <stdarg.h>

/* general functions */

/* This table is used for converting numerics to longs. */
unsigned int numerictab[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 62, 0, 63, 0, 0, 0, 26, 27,
  28,
  29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
  47, 48,
  49, 50, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* This table is used for converting longs to numerics. */
char tokens[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789[]";

/* NumericToLong: converts a numeric into a long */

long NumericToLong(const char *numeric, int numericlen)
{
  /* Converts a numeric to a long. The numeric looked at starts at 'numeric' and is numericlen long. */

  long mynumeric = 0; /* Return value. */
  int i;  /* Loopcounter. */

  for (i = 0; i < numericlen; i++) {
    /* Run through the chars in numeric. */
    /* Multiply the result of the numerics looked at so far by 64 and add the value of the current char. */
    mynumeric = (mynumeric << 6) + numerictab[(int) *(numeric++)];
  }

  return (mynumeric);
}

/* LongToNumeric: converts a long value into a numeric */

char *LongToNumeric(long param, int len)
{
  /* Converts a long to a numeric. 'param' is the long, and len
   * is the desired length of the resulting numeric.
   */

  static char mynum[6]; /*The result is stored here. */
  int i;  /* Loopcounter. */

  /* Safecatch. We only need numerics of length 5 when dealing with ircu. */
  if (len > 5)
    len = 5;

  for (i = len - 1; i >= 0; i--) {
    /* We look at the rest when divided by 64 and put this in the right end of the string.
     * (The right end represents the lowest valued ciphers, just like in a number.
     */
    mynum[i] = tokens[(param % 64)];
    param /= 64;
  }

  mynum[len + 1] = '\0';

  /* Lets return what we made, shall we? ;) */
  return (mynum);
}

int IsCharInString(char chr, char *string)
{
  /* Returns wether a char (chr) is in a string (string). */
  for (; *string; string++)
    if (chr == *string)
      return 1;

  return 0;
}

char *SeperateWord(char *sentence)
{
  /* Returns the pointer to the second word in the sentence. */

  /* Sanity check: make we're not given a NULL pointer to start with,
   * return NULL if we are.
   *
   * This allows:
   * word2=SeperateWord(word1);
   * word3=SeperateWord(word2);
   * ...
   * with only one NULL check */

  if (sentence == NULL)
    return NULL;

  /* All sentences end with a zero. It is padded in GetLineFromChunk. */
  while (*sentence != ' ' && *sentence != '\0' && *sentence != '\n') {
    sentence++;
  }

  /* If this was the last word, return NULL. */
  if (*sentence == '\0' || *sentence == '\n') {
    *sentence = '\0';
    return (NULL);
  }

  /* We'll insert a zero after the first word, and return the pointer to the second word. */
  if (*sentence == ' ') {       /* This check is superflous, but improves readability. */
    *sentence = '\0';
    sentence++;
  }

  return (StripBlanks(sentence));
}

char *StripBlanks(char *paddedstring)
{
  /* Returns the pointer to the first nonblank char. */

  /* This check can be removed to improve performance, we might coredump then though. */
  if (paddedstring == NULL) {
    return (NULL);
  }

  /* Skip all the spaces in the start. */
  while (*paddedstring == ' ') {
    paddedstring++;
  }

  return (paddedstring);
}

int CheckAuthLevel(struct user *user_ptr, unsigned char level)
{
  if (user_ptr->authedas == NULL) {
    Error(ERR_WARNING | ERR_INTERNAL, "CheckAuthLevel() with non-authed user");
    return 0;
  }

  if (user_ptr->authedas->authlevel < level) {
    MessageToUser(user_ptr, "Sorry, that command requires authlevel %d (your level: %d).", level,
                  user_ptr->authedas->authlevel);
    return 0;
  }

  return 1;
}

void MessageToUser(struct user *user, char *message, ...)
{
  char buf[512];
  char buf2[512];
  va_list val;

  va_start(val, message);
  vsnprintf(buf, 511, message, val);
  va_end(val);

  snprintf(buf2, 511, "%sAAA P %s :%s\r\n", my_numeric, user->numeric, buf);
  SendLine(buf2);
}

void NoticeToUser(struct user *user, char *message, ...)
{
  char buf[512];
  char buf2[512];
  va_list val;
  int buflen;

  va_start(val, message);
  vsnprintf(buf, 511, message, val);
  va_end(val);

  buflen = snprintf(buf2, 509, "%sAAA %s %s :%s", my_numeric, (user->oper > 1) ? "P" : "O", user->numeric, buf);
  if (buflen > 509 || buflen < 0)
    buflen = 509;

  sprintf(buf2 + buflen, "\r\n");

  SendLine(buf2);
}

/* simple case insensitive regex match, collapses multiple * chars for speed */
int match2strings(const char *reg, const char *str)
{
  return a_strmatch(str, reg, 0);
}

/*
 * Author : Zarjazz
 * Email  : zarjazz@quakenet.org
 * ----
 * $Id: a_string.c,v 1.1 2002/07/02 16:41:22 zarjazz Exp $
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/*
 * ================
 * a_strstrip
 * ----------------
 * remove initial and trailing white-space char's from a string
 * ================
 */

char *a_strstrip(char *str)
{
  int l;

  assert(str);
  /* skip pre-non-valid chars */
  while (*str && isspace(*str))
    str++;
  /* remove trailing white space chars */
  l = strlen(str);
  while (l > 0 && isspace(str[--l]))
    str[l] = '\0';
  /* return the stripped string */
  return str;
}

/*
 * ================
 * a_strcpy
 * ----------------
 * buffer safe string copy
 * ================
 */

char *a_strcpy(char *dst, const char *src, int dst_len)
{
  char *save = dst;

  assert(src);
  assert(dst);
  /* copy string and ensure null terminated */
  while (*src && (--dst_len > 0))
    *dst++ = *src++;
  *dst = '\0';

  return save;
}

/*
 * ================
 * a_strextract
 * ----------------
 * extract the first non-space token from 'src' and
 * ensure string is null terminated. returns the
 * string postition after extracted token
 * ================
 */

char *a_strextract(char *dst, const char *src, int dst_len)
{
  assert(src);
  assert(dst);
  /* skip pre-non-valid chars */
  while (*src && isspace(*src))
    src++;
  /* copy string and ensure null terminated */
  while (*src && (--dst_len > 0) && !isspace(*src))
    *dst++ = *src++;
  /* skip post-non-valid chars */
  while (*src && isspace(*src))
    src++;
  *dst = '\0';

  return (char *) src;
}

/*
 * ================
 * a_strmatch
 * ----------------
 * does a string match using a very simple "regex" (ie only special characters are: "*?\")
 * ================
 * */

int a_strmatch(const char *str, const char *reg, int case_sensitive)
{
  assert(str);
  assert(reg);
  /* compare strings */
  while (*str && *reg) {
    switch (*reg) {
    case '*':
      /* FIXME: '..*?..' support broken */
      while (*reg == '*' || *reg == '?')
        reg++;

      switch (*reg) {
      case '\0':
        return 1;
      case '\\':
        if (*++reg == '\0')
          return 0;
      default:
        if (case_sensitive) {
          while ((str = strchr(str, *reg)))
            if (a_strmatch(str++, reg, case_sensitive))
              return 1;
        } else {
          char *save = (char *) str;

          while ((str = strchr(str, tolower(*reg))))
            if (a_strmatch(str++, reg, case_sensitive))
              return 1;
          str = save;
          while ((str = strchr(str, toupper(*reg))))
            if (a_strmatch(str++, reg, case_sensitive))
              return 1;
        }
        return 0;
      }
      break;
    case '?':
      str++, reg++;
      break;
    case '\\':
      if (*++reg == '\0')
        return 0;
    default:
      if (case_sensitive) {
        if (*str++ != *reg++)
          return 0;
      } else {
        if (tolower(*str++) != tolower(*reg++))
          return 0;
      }
      break;
    }
  }

  if (*str || *reg)
    return 0;

  return 1;
}

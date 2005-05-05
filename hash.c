/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: hash.c,v 1.8 2002/03/31 11:29:15 bigfoot Exp $
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
#include <ctype.h>
#include <zlib.h>               /* For adler32(). */
#include <strcmp.h>             /* For ToLower(). */

int hash(char *buffer)
{
  /* This hashfunction takes a string and returns a value x, 0 <= x < HASHMAX. */

  uLong adlerhash;
  int modhash;
  char tmp[50];
  int i;

  /* Uhm, this seems like a terrible hack, why do we do it? -Bigfoot */
  for (i = 0; i < 50; i++) {    /* change the string to tolower with ircd
                                   specialities */
    tmp[i] = ToLower(buffer[i]);
    if ('\0' == tmp[i])
      break;
  }
  /* Use zlib's adler32 - this should be faster than the crc32 function used by O. */
  adlerhash = adler32(1, tmp, strlen(tmp));

  modhash = adlerhash % HASHMAX;

  return (modhash);

}

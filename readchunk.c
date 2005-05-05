/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: readchunk.c,v 1.8 2002/03/26 00:47:25 zarjazz Exp $
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

int ReadChunk(void)
{

  int oldbytes; /* Number of bytes left from last read. */
  int readbytes;  /* Number of bytes read this time. */
  char *dest; /* Temporary variable to point into chunkbuf. */

  /* Reads whatever is available from the serversocket. */

  /* In case we got half a line the last time, copy it to the start
   * and recieve the ready data after the remains.
   */

  if (*nextline != '\0') {
    oldbytes = strlen(nextline);
    dest = chunkbuf;
    while (*nextline) {
      *dest++ = *nextline++;
    }
    chunkbufentry = chunkbuf + oldbytes;
  } else {
    oldbytes = 0;
    chunkbufentry = chunkbuf;
  }

  /* Now read the ready ]data[. */
  readbytes = read(serverfd, chunkbufentry, (CHUNKSIZE - oldbytes) - 10);
  if (readbytes == 0 || readbytes == -1) {
    /* TODO: Do some errorhandling. */
    return (0);
  }

  /* Make sure the data are terminated properly. */
  *(chunkbufentry + readbytes) = '\0';
  nextline = chunkbuf;

  /* The chunnkbuf should now look as follows:
   * olddata + newdata + zero
   */

  return (1);

}

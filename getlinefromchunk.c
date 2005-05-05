/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: getlinefromchunk.c,v 1.6 2002/03/26 00:47:25 zarjazz Exp $
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

int GetLineFromChunk(void)
{
  /* currentline is the variable where we store a pointer to the next line in line. */
  currentline = nextline;

  /* Search through the line we have here. */
  while (*nextline != '\n' && *nextline != '\r' && *nextline != '\0') {
    nextline++;                 /* A safecheck could be used here. */
  }

  /* In case we didnt get a proper last line, it wont be terminated by \r or \n (or \r\n) like it
   * usually is but instead by a zero. In that case we'll save the line and try to read some more
   * data to get a complete line.
   */
  if (*nextline == '\0') {
    nextline = currentline;
    return (0);
  }

  /* We replace the trailing \r and \n with a zero to terminate the line. */
  if (*nextline == '\n' || *nextline == '\r') { /* This check is not necessary, but improves readability. */
    *nextline = '\0';
    nextline++;
  }

  /* There may be two of these, a \r\n combo (this is the normal iirc). */
  if (*nextline == '\n' || *nextline == '\r') {
    *nextline = '\0';
    nextline++;
  }

  return (1);

}

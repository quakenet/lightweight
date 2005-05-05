/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: strcmp.c,v 1.5 2002/03/31 11:29:15 bigfoot Exp $
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

#include <ctype.h>
#include <stdlib.h>
#include <strcmp.h>
#include <limits.h>
#include <sys/types.h>

/*
 * The following code is stolen from ircu.
 * Copyright (C) 1998 Andrea Cocito and Thomas Helvey.
 * Released under the GPL.
 */

int Strcmp(const char *a, const char *b)
{
  const char *ra = a;
  const char *rb = b;

  while (ToLower(*ra) == ToLower(*rb)) {
    if (!*ra++)
      return 0;
    else
      ++rb;
  }
  return (*ra - *rb);
}

int Strncmp(const char *a, const char *b, int n)
{
  const char *ra = a;
  const char *rb = b;
  int left = n;

  if (!left--)
    return 0;
  while (ToLower(*ra) == ToLower(*rb)) {
    if (!*ra++ || !left--)
      return 0;
    else
      ++rb;
  }
  return (*ra - *rb);
}

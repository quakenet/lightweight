/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: base64.c,v 1.5 2002/03/18 09:29:54 zarjazz Exp $
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

#include <base64.h>
#include <lightweight.h>
extern struct server *servertree[4096];

/* internal functions */
char base64toint(char);
char inttobase64(char);

/* returns the server index for numeric id char[5] */
short int getserverindex(char *numeric)
{

  return base64toint(numeric[0]) * 64 + base64toint(numeric[1]);
}

/* returns the client index for numeric id char[5] */
long int getclientindex(char *numeric)
{
  return (base64toint(numeric[2]) * 4096 + base64toint(numeric[3]) * 64 +
          base64toint(numeric[4])) % servertree[getserverindex(numeric)]->maxusers;
}

/* the server and client indexes are divided into base 64 using / as integer
division and some modulus */
void createnumeric(char *ptr, short int serverindex, long int clientindex)
{
  ptr[0] = inttobase64((char) (serverindex / 64));
  ptr[1] = inttobase64((char) (serverindex % 64));
  ptr[2] = inttobase64((char) (clientindex / 4096));
  ptr[3] = inttobase64((char) ((clientindex % 4096) / 64));
  ptr[4] = inttobase64((char) (clientindex % 64));
}

/* A little internal ascii play, nothing fancy but fast */
char base64toint(char base64)
{
  if ('9' >= base64)
    return base64 + 4;
  if ('Z' >= base64)
    return base64 - 65;
  if ('a' <= base64)
    return base64 - 71;
  if (']' == base64)
    return 63;
  else
    return 62;
}

char inttobase64(char integer)
{
  if (25 >= integer)
    return integer + 65;
  if (51 >= integer)
    return integer + 71;
  if (61 >= integer)
    return integer - 4;
  if (62 == integer)
    return 91;
  else
    return 93;
}

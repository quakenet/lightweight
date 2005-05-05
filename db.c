/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: db.c,v 1.7 2002/03/19 02:33:49 splidge Exp $
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
#include <base64.h>

/* See datastructures.txt for explanations about how the dbs are structured. */

/* required access functions */

struct user *FindUserByNick(char *nick)
{
  /* Finds the user with nick 'nick'. */
  struct user *tmp_ptr;

  tmp_ptr = nickhashtable[hash(nick)];  /* start of list */
  while (NULL != tmp_ptr) {     /* go through the list */
    if (!Strcmp(tmp_ptr->nick, nick)) /* case insensitive */
      return tmp_ptr;           /* if found, return */
    tmp_ptr = tmp_ptr->nextbynick;  /* else go to next in list */
  }
  return NULL;                  /* not found */
}

struct user *FindUserByNumeric(char *numeric)
{
  /* Finds the user with numeric 'numeric'. */
  return usertablepointer[getserverindex(numeric)][getclientindex(numeric)];
}

struct user *FindUserByAuthname(char *authname)
{
  /* Finds the user whom is authe as authname. */

  return (NULL);
}

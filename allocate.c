/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by David Mansell & Raimo Nikkilä & Rasmus Have
 *
 * allocate.c
 *
 * $Id: allocate.c,v 1.9 2002/03/25 10:02:11 bigfoot Exp $
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

/* allocate.c: handle allocation and freeing of objects */

#include <stdlib.h>

#include <lightweight.h>
#include <globalexterns.h>

/* Easy improvement:
 * - let these functions buffer allocations so some are reused.
 * Harder improvement:
 * - limit the calls to malloc() and free() by allocating larger
 *   blocks and split them out yourself (makes free'ing harder,
 *   perhaps ditch the free'ing part?).
 */

struct server *GetServer()
{
  memoryused += sizeof(struct server);
  return (struct server *) malloc(sizeof(struct server));
}

void FreeServer(struct server *sp)
{
  memoryused -= sizeof(struct server);
  free(sp);
}

struct user *GetUser()
{
  memoryused += sizeof(struct user);
  return (struct user *) malloc(sizeof(struct user));
}

void FreeUser(struct user *up)
{
  memoryused -= sizeof(struct user);
  free(up);
}

struct user *DBUserMalloc(void)
{
  memoryused += sizeof(struct user);
  return (struct user *) malloc(sizeof(struct user));
}

void DBUserFree(struct user *user_pointer)
{
  memoryused -= sizeof(struct user);
  free(user_pointer);
}

struct account *DBAccountMalloc(void)
{
  memoryused += sizeof(struct account);
  return (struct account *) malloc(sizeof(struct account));
}

void DBAccountFree(struct account *account_pointer)
{
  memoryused -= sizeof(struct account);
  free(account_pointer);
}

struct reggedchannel *DBMallocChannel(void)
{
  memoryused += sizeof(struct reggedchannel);
  return (struct reggedchannel *) malloc(sizeof(struct reggedchannel));
}

void DBFreeChannel(struct reggedchannel *chn_ptr)
{
  memoryused -= sizeof(struct reggedchannel);
  free(chn_ptr);
}

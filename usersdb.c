/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: usersdb.c,v 1.10 2002/03/25 10:02:11 bigfoot Exp $
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
#include <accountsdb.h>
#include <channelsdb.h>
#include <usersdb.h>
#include <base64.h>
#include <globalexterns.h>

/* internal function pointer */
struct user *DBUserMalloc(void);
void DBUserFree(struct user *);
void AddUserNumeric(struct user *, char *);
void AddUserToHash(struct user *, char *);
void addauthtoaccount(struct account *, struct user *);
void DeleteUserFromHash(struct user *);

void SetOperUser(char *numericid, char isoper)
{                               /* just set account->oper to 0 */
  usertablepointer[getserverindex(numericid)][getclientindex(numericid)]->oper = isoper;
}

struct account *AuthUser(char *numericid, char *authname)
{
  struct account *act_ptr;
  struct user *usr_ptr;

  act_ptr = GetAccountByAuthName(authname); /* get the account for auth name */
  if (NULL == act_ptr)
    act_ptr = AddAccount(authname); /* no such account -> add it */

/*if (Strcmp(act_ptr->authpass, password)) return NULL; *//* the password
   was incorrect */
  /* auth succesfull */
  usr_ptr = usertablepointer[getserverindex(numericid)][getclientindex(numericid)]; /* corresponding user struct */

  if (NULL == usr_ptr) {
    Error(ERR_ERROR | ERR_INTERNAL, "Trying to auth non-existent user %s!", numericid);
    return NULL;
  }

  if (NULL != usr_ptr->authedas) {

    /* They were already authed.. BAD */
    /* However, for now, do what Q does (which is, forget the old auth and apply the new one) */
    Error(ERR_ERROR | ERR_PROTOCOL, "User was already authed: %s", numericid);
    RemoveAuthFromUser(usr_ptr);
  }

  addauthtoaccount(act_ptr, usr_ptr); /* and add the auth to the account */
  usr_ptr->authedas = act_ptr;  /* set the user as authed */
  act_ptr->lastauth = time(NULL); /* he authed now */

  /* call anything else you need upon authing */

  DoAllAutoModes(usr_ptr);

  return act_ptr;
}

void addauthtoaccount(struct account *act_ptr, struct user *usr_ptr)
{
  struct usersauthed *tmp_ptr;

  tmp_ptr = act_ptr->currentuser; /* start of the authed list by storing it */
  act_ptr->currentuser = (struct usersauthed *) malloc(sizeof(struct usersauthed)); /* add a list element */
  act_ptr->currentuser->nextinlist = tmp_ptr; /* set the next item in list as the original list */
  act_ptr->currentuser->autheduser = usr_ptr; /* and store the user */
  memoryused += sizeof(struct usersauthed);
}

void RemoveAuthFromUser(struct user *usr_ptr)
{
  struct account *act_ptr = usr_ptr->authedas;
  struct usersauthed **tmp_ptr;

  if (act_ptr == NULL) {        /* Sanity check */
    return;
  }

  tmp_ptr = &act_ptr->currentuser;  /* start of the auths list */
  while (NULL != *tmp_ptr) {    /* go throught the list */
    if ((*tmp_ptr)->autheduser == usr_ptr) {  /* if found */
      struct usersauthed *tmp_ptr2;

      tmp_ptr2 = (*tmp_ptr)->nextinlist;  /* store the list */
      free(*tmp_ptr);           /* free the current item */
      memoryused -= sizeof(struct usersauthed);
      *tmp_ptr = tmp_ptr2;      /* restore the list */
      return;
    }
    tmp_ptr = &(*tmp_ptr)->nextinlist;  /* move along in the list */
  }

  Error(ERR_ERROR & ERR_INTERNAL, "Couldn't find user in RemoveAuthFromAccount()");
}

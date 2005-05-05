/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: nick.c,v 1.18 2003/09/08 01:19:25 zarjazz Exp $
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
#include <accountsdb.h>
#include <channelsdb.h>

/* When we recieves a NICK command from the server, it can either be a nickchange
 * or an introduction of a new nick. We use two functions to handle the two
 * different cases.
 */

void UserNickCreation(char *numeric, char *nick, char isoper)
{
  /* This function creates a new user in the userdb. */

  struct user *newuser;
  int uhash;
  int maskednum;
  int servernumeric;
  char buf[512];

  /* Calculate the relevant numerics. Please note the hack used by ircu to
   * avoid using the same numeric twice within short time.
   */
  servernumeric = NumericToLong(numeric, 2);
  maskednum = NumericToLong(numeric + 2, 3) % (servertree[servernumeric]->maxusers);

  if (usertablepointer[servernumeric][maskednum] != NULL) {
    /* Erk. Duplicate numeric... */
    /* We're going to end up with these due to not handling nick collisions :) - splidge */
    /* I cant see why a nick collision should lead to a numericcollision, elaborate please. - Bigfoot */
    Error(ERR_WARNING | ERR_PROTOCOL, "Numeric clash: %s (%s) vs. %s (%s)",
          usertablepointer[servernumeric][maskednum]->numeric, usertablepointer[servernumeric][maskednum]->nick,
          numeric, nick);
    DeleteUser(numeric);
  }

  /* user number i connects to server number n
   * - allocate user struct & fill in ]data[
   */
  newuser = GetUser();

  a_strcpy(newuser->nick, nick, sizeof(newuser->nick));
  a_strcpy(newuser->numeric, numeric, sizeof(newuser->numeric));

  newuser->authedas = NULL;
  newuser->oper = isoper;
  newuser->nextbynick = NULL;

#ifndef HORRIBLE_DEOPALL_HACK
  if (!strncmp(newuser->nick, o_nick, NICKLEN)) {
    Optr = newuser;
    sprintf(buf, "%sAAA P %s :auth %s %s\r\n", my_numeric, o_userserver, o_login, o_pass);
    SendLine(buf);
  }
#endif

  /* - make pointer to struct in (*(usertablepointer[n]) + i*sizeof(void *)) */

  usertablepointer[servernumeric][maskednum] = newuser;

  /* - hash nick 
   * - newuser->next = nickhashtable[hash]
   * - nickhashtable[hash] = newuser      */

  uhash = hash(nick);
  newuser->nextbynick = nickhashtable[uhash];
  nickhashtable[uhash] = newuser;
}

void UserNickChange(char *numeric, char *newnick)
{
  /* This function changes a users nick in the userdb. */

  /*
     - FindUserByNumeric
     - Hash previous nick (is still in the userstruct)
     - find the previous nick in nickhashtable[oldhash] linked list and remember the user_before
     - if user_before==NULL
     then
     nickhashtable[oldhash] = user->next
     else
     user_before->next = user->next
     - hash newnick and copy nexnick into user struct
     - user->next = nickhashtable[newhash]
     - nickhashtable[newhash] = user
   */

  int servernum;  /* servernumeric. */
  int maskednum;  /* masked usernumeric. */

  /* tempuserpointer is the pointer we use to run through the linked list. 
   * previoususerpointer is the pointer to the previous user.
   * wanteduserpointer is the pointer to the userstruct we want to find.
   */
  struct user *wanteduserpointer;
  struct user *tempuserpointer;
  struct user *previoususerpointer;
  int uhash;  /* variable to store the hash of the nick in. */
  int bfound = 0; /* indicates when our search (the while-loop) ends. */

  /* Calculate the relevant numerics. Please note the hack used by ircu to
   * avoid using the same numeric twice within short time.
   */
  servernum = NumericToLong(numeric, 2);
  maskednum = NumericToLong(numeric + 2, 3) % servertree[servernum]->maxusers;

  /* Have the pointer to the user we want to find handy. */
  wanteduserpointer = usertablepointer[servernum][maskednum];

  /* start from the start of the linked list. */
  uhash = hash(wanteduserpointer->nick);
  tempuserpointer = nickhashtable[uhash];
  previoususerpointer = NULL;

  while (tempuserpointer != NULL && !bfound) {
    if (tempuserpointer == wanteduserpointer) {
      /* We found the userpointer we where looking for, remove it from the linked list. */
      if (previoususerpointer != NULL) {
        previoususerpointer->nextbynick = tempuserpointer->nextbynick;
      } else {
        nickhashtable[uhash] = tempuserpointer->nextbynick;
      }
      bfound = 1;
    } else {
      /* else, go to the next user in the linked list. */
      previoususerpointer = tempuserpointer;
      tempuserpointer = tempuserpointer->nextbynick;
    }
  }

  if (!bfound) {
    /* Bad, we have a usertablestruct corruption. */
    Error(ERR_ERROR | ERR_INTERNAL, "usertablestruct corruption - nick not found: %s (%s).\n", wanteduserpointer->nick,
          wanteduserpointer->numeric);
  }

  a_strcpy(wanteduserpointer->nick, newnick, sizeof(wanteduserpointer->nick));
  uhash = hash(newnick);

  wanteduserpointer->nextbynick = nickhashtable[uhash];
  nickhashtable[uhash] = wanteduserpointer;
}

void UserHashStats(void)
{
  /* Print some cheesy hash stats */
  int numused = 0;
  int maxchain = 0;
  int thischain = 0;
  int totalusers = 0;
  struct user *up;
  int i;

  for (i = 0; i < HASHMAX; i++)
    if (nickhashtable[i]) {
      numused++;
      for (thischain = 0, up = nickhashtable[i]; up; up = up->nextbynick, thischain++, totalusers++);
      if (thischain > maxchain)
        maxchain = thischain;
    }

  Error(ERR_DEBUG | ERR_STATS, "User hash stats: %d users, %d/%d buckets, max chain %d", totalusers, numused, HASHMAX,
        maxchain);
}

void DeleteUser(char *numeric)
{
  int servernum;
  int maskednum;
  int uhash;
  struct user *up, *lup, *theup;

  servernum = NumericToLong(numeric, 2);
  maskednum = NumericToLong(numeric + 2, 3) % servertree[servernum]->maxusers;

  if (usertablepointer[servernum][maskednum] == NULL) {
    Error(ERR_WARNING | ERR_INTERNAL, "Trying to delete non-existent user %s", numeric);
    return;
  } else {
    theup = usertablepointer[servernum][maskednum];
    usertablepointer[servernum][maskednum] = NULL;
  }

  /* theup is the user we want to delete */
  /* De-auth it now if it is authed */
  if (theup->authedas != NULL) {
    RemoveAuthFromUser(theup);
  }
#ifndef HORRIBLE_DEOPALL_HACK
  if (!strncmp(theup->nick, "O", NICKLEN))
    Optr = NULL;
#endif

  lup = NULL;
  uhash = hash(theup->nick);
  up = nickhashtable[uhash];
  while (up)
    if (!strncmp(up->nick, theup->nick, NICKLEN)) {
      if (lup)
        lup->nextbynick = up->nextbynick;
      else
        nickhashtable[uhash] = up->nextbynick;

      FreeUser(theup);
      return;
    } else {
      lup = up;
      up = up->nextbynick;
    }

  Error(ERR_INTERNAL | ERR_ERROR, "DeleteUser: Couldn't find entry in nicklist...");
  FreeUser(theup);
}

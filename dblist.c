/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: dblist.c,v 1.36 2004/09/20 20:24:22 froo Exp $
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

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <accountsdb.h>
/* TEMP */
#include <channelsdb.h>         /* TEMP */
#include <usersdb.h>
#include <lightweight.h>
#include <dblist.h>

                                                                  /* TEMP */ extern void NoticeToUser(struct user *, char *, ...);

                                                                  /* TEMP */

/* TEMP */
/* Internal function definitions */

/* TEMP */

/* TEMP */
/*
struct server* servertree[4096];
struct user *nickhashtable[HASHMAX];
struct user **usertablepointer[4096];
struct account* accounthashtable[HASHMAX];
struct reggedchannel* channelhashtable[HASHMAX];
long int hash(char* a) {return 200;}
*/

void WhoAmI(struct user *);
void WhoIsUser(struct user *, char *);
void WhoIsAccount(struct user *, char *);
void PrintUInfo(struct user *, struct account *);
void PrintUInfoRestricted(struct user *, struct account *);

int CanRemoveOwner(struct reggedchannel *);

int CanRemoveOwner(struct reggedchannel *chn_ptr)
{
  int ucount = 0, ncount = 0, i;

  for (i = 0; i < USERSPERCHANNEL; i++) {
    if (NULL != chn_ptr->channeluser[i]) {
      ucount++;
      if (GetChannelFlags(chn_ptr->channeluser[i], chn_ptr) & CFLAG_OWNER)
        ncount++;
    }
  }
  return (!(1 == ncount && 1 != ucount));
}

void GetWhois(struct user *usr_ptr, char *target)
{
  if (NULL == target)
    WhoAmI(usr_ptr);
  else if ('\0' == *target)
    return;
  else if ('#' == *target)
    WhoIsAccount(usr_ptr, target + sizeof(char));
  else
    WhoIsUser(usr_ptr, target);
}

void WhoAmI(struct user *usr_ptr)
{
  NoticeToUser(usr_ptr, "You are authed as %s", usr_ptr->authedas->authname);
  if (1 < usr_ptr->authedas->authlevel)
    NoticeToUser(usr_ptr, "Global auth level: %d", usr_ptr->authedas->authlevel);
  PrintUInfo(usr_ptr, usr_ptr->authedas);
}

void WhoIsUser(struct user *usr_ptr, char *target)
{
  struct user *tmp_ptr;

  tmp_ptr = FindUserByNick(target);
  if (NULL == tmp_ptr)
    NoticeToUser(usr_ptr, "%s is not online right now.", target);
  else if (NULL == tmp_ptr->authedas)
    NoticeToUser(usr_ptr, "User %s is not authed", tmp_ptr->nick);
  else {
    NoticeToUser(usr_ptr, "%s is authed as %s.", tmp_ptr->nick, tmp_ptr->authedas->authname);
    if (10 <= tmp_ptr->authedas->authlevel)
      NoticeToUser(usr_ptr, "%s is QuakeNet Staff.", tmp_ptr->nick);
    if (250 <= tmp_ptr->authedas->authlevel)
      NoticeToUser(usr_ptr, "%s is an IRC Operator.", tmp_ptr->nick);
    if (10 < usr_ptr->authedas->authlevel)
      NoticeToUser(usr_ptr, "%s has global auth level %d.", tmp_ptr->nick, tmp_ptr->authedas->authlevel);   
    NoticeToUser(usr_ptr, "Last auth: %s", ctime(&tmp_ptr->authedas->lastauth));
    if (10 <= usr_ptr->authedas->authlevel)
      PrintUInfo(usr_ptr, tmp_ptr->authedas);
    else
      PrintUInfoRestricted(usr_ptr, tmp_ptr->authedas);
  }
}

void WhoIsAccount(struct user *usr_ptr, char *target)
{
  struct account *tmp_ptr;

  tmp_ptr = GetAccountByAuthName(target);
  if (NULL == tmp_ptr)
    NoticeToUser(usr_ptr, "There is no user named %s.", target);
  else {
    if (NULL == tmp_ptr->currentuser)
      NoticeToUser(usr_ptr, "User %s is not currently online.", tmp_ptr->authname);
    else {
      int i = 0;
      char tmp[(10 * (NICKLEN + 1)) + 1]; /* 10 NICKLEN nicks, with spaces and terminating NULL */
      struct usersauthed *wrk_ptr;

      wrk_ptr = tmp_ptr->currentuser;
      *tmp = '\0';
      while (NULL != wrk_ptr && i < 10) {
        strcat(tmp, wrk_ptr->autheduser->nick);
        strcat(tmp, " ");
        i++;
        wrk_ptr = wrk_ptr->nextinlist;
      }
      NoticeToUser(usr_ptr, "%s currently authed as %s: %s", (i > 1 ? "These users are" : "This user is"),
                   tmp_ptr->authname, tmp);
    }
    if (10 <= tmp_ptr->authlevel)
      NoticeToUser(usr_ptr, "%s is QuakeNet Staff.", tmp_ptr);
    if (250 <= tmp_ptr->authlevel)
      NoticeToUser(usr_ptr, "%s is an IRC Operator.", tmp_ptr);
    if (10 < usr_ptr->authedas->authlevel)
      NoticeToUser(usr_ptr, "%s has global authlevel %d", tmp_ptr->authname, tmp_ptr->authlevel);   
    NoticeToUser(usr_ptr, "Last auth: %s", ctime(&tmp_ptr->lastauth));
    if (usr_ptr->authedas == tmp_ptr)
      PrintUInfo(usr_ptr, tmp_ptr);
    else if (10 <= usr_ptr->authedas->authlevel)
      PrintUInfo(usr_ptr, tmp_ptr);
    else
      PrintUInfoRestricted(usr_ptr, tmp_ptr);
  }
}

void PrintUInfo(struct user *usr_ptr, struct account *act_ptr)
{
  char tmp[40];
  int i;
  int shown = 0;
  int uonchans = 0;

  tmp[39] = '\0';
  for (i = 0; i < CHANNELSPERUSER; i++) {
    if (NULL != act_ptr->channel[i]) {
      uonchans++;
      memset(tmp, ' ', 39);
      if (shown == 0) {
        shown = 1;
        NoticeToUser(usr_ptr, "Known on the following channels:");
        NoticeToUser(usr_ptr, "channel                    access flags");
        NoticeToUser(usr_ptr, "---------------------------------------");
      }
      sprintf(tmp, "%s", act_ptr->channel[i]->channelname);
      tmp[strlen(tmp)] = ' ';
      PrintCFlags(tmp + 31 * sizeof(char), act_ptr->channelflags[i]);
      NoticeToUser(usr_ptr, "%s", tmp);
    }
  }
  if (shown == 0)
    NoticeToUser(usr_ptr, "%s is not known on any channels.", act_ptr->authname);
  else {
    NoticeToUser(usr_ptr, "%s is known on %d channel/s (max %d channels).",
      act_ptr->authname, uonchans, CHANNELSPERUSER);
    NoticeToUser(usr_ptr, "End of list.");
  }
}

void PrintUInfoRestricted(struct user *usr_ptr, struct account *act_ptr)
{
  char tmp[40];
  int i;
  int shown = 0;
  int uonchans = 0;

  tmp[39] = '\0';
  for (i = 0; i < CHANNELSPERUSER; i++) {
    if (NULL != act_ptr->channel[i]
        && (-1 != GetUserChannelIndex(act_ptr->channel[i], usr_ptr->authedas))) {
      uonchans++;
      memset(tmp, ' ', 39);
      if (shown == 0) {
        shown = 1;
        NoticeToUser(usr_ptr, "Known on the following channels:");
        NoticeToUser(usr_ptr, "channel                    access flags");
        NoticeToUser(usr_ptr, "---------------------------------------");
      }
      sprintf(tmp, "%s", act_ptr->channel[i]->channelname);
      tmp[strlen(tmp)] = ' ';
      PrintCFlags(tmp + 31, act_ptr->channelflags[i]);
      NoticeToUser(usr_ptr, "%s", tmp);
    }
  }
  if (shown == 0)
    NoticeToUser(usr_ptr, "%s is not known on any channels.", act_ptr->authname);
  else {
    NoticeToUser(usr_ptr, "%s is known to you on %d channel/s (max %d channels).",
      act_ptr->authname, uonchans, CHANNELSPERUSER);
    NoticeToUser(usr_ptr, "End of list.");
  }
}

void PrintChanlev(struct user *usr_ptr, char *channame, char *target)
{
  int i;
  int shown = 0;
  char tmp[30];
  unsigned char currentflags;
  struct reggedchannel *chn_ptr;
  struct account *act_ptr;
  int owners=0,masters=0,ops=0,voices=0;

  tmp[29] = '\0';               /* we want the string to end */
  chn_ptr = GetChannelPointer(channame);
  if (NULL == chn_ptr)
    NoticeToUser(usr_ptr, "Can't find channel %s.", channame);
  if (-1 == GetUserChannelIndex(chn_ptr, usr_ptr->authedas) && (10 > usr_ptr->authedas->authlevel))
    NoticeToUser(usr_ptr, "Sorry, you're not known on %s.", chn_ptr->channelname);
  else {
    /* Check if they specified a user.  If not, it's just a complete listing */
    if (NULL == target) {
      if (10 <= usr_ptr->authedas->authlevel) 
        NoticeToUser(usr_ptr, "Founder           : %s", (chn_ptr->founder == NULL ? "???" : chn_ptr->founder->authname));
      NoticeToUser(usr_ptr, "Users for channel : %s", chn_ptr->channelname);
      NoticeToUser(usr_ptr, "Authname         Access flags");
      NoticeToUser(usr_ptr, "-----------------------------");
      for (i = 0; i < USERSPERCHANNEL; i++) {
        if (NULL != chn_ptr->channeluser[i]) {
          currentflags = chn_ptr->channeluser[i]->channelflags[GetUserChannelIndex(chn_ptr, chn_ptr->channeluser[i])];
          if (currentflags & CFLAG_OWNER) {
            memset(tmp, ' ', 29);
            sprintf(tmp, "%s", chn_ptr->channeluser[i]->authname);
            tmp[strlen(tmp)] = ' ';
            PrintCFlags(tmp + 21, currentflags);
            NoticeToUser(usr_ptr, "%s", tmp);
            owners++;
          }
        }
      }
      for (i = 0; i < USERSPERCHANNEL; i++) {
        if (NULL != chn_ptr->channeluser[i]) {
          currentflags = chn_ptr->channeluser[i]->channelflags[GetUserChannelIndex(chn_ptr, chn_ptr->channeluser[i])];
          if (!(currentflags & CFLAG_OWNER) && (currentflags & CFLAG_MASTER)) {
            memset(tmp, ' ', 29);
            sprintf(tmp, "%s", chn_ptr->channeluser[i]->authname);
            tmp[strlen(tmp)] = ' ';
            PrintCFlags(tmp + 21, currentflags);
            NoticeToUser(usr_ptr, "%s", tmp);
            masters++;
          }
        }
      }
      for (i = 0; i < USERSPERCHANNEL; i++) {
        if (NULL != chn_ptr->channeluser[i]) {
          currentflags = chn_ptr->channeluser[i]->channelflags[GetUserChannelIndex(chn_ptr, chn_ptr->channeluser[i])];
          if (!(currentflags & CFLAG_OWNER) && !(currentflags & CFLAG_MASTER)
              && (currentflags & CFLAG_OP)) {
            memset(tmp, ' ', 29);
            sprintf(tmp, "%s", chn_ptr->channeluser[i]->authname);
            tmp[strlen(tmp)] = ' ';
            PrintCFlags(tmp + 21, currentflags);
            NoticeToUser(usr_ptr, "%s", tmp);
            ops++;
          }
        }
      }
      for (i = 0; i < USERSPERCHANNEL; i++) {
        if (NULL != chn_ptr->channeluser[i]) {
          currentflags = chn_ptr->channeluser[i]->channelflags[GetUserChannelIndex(chn_ptr, chn_ptr->channeluser[i])];
          if (!(currentflags & CFLAG_OWNER) && !(currentflags & CFLAG_MASTER)
              && !(currentflags & CFLAG_OP)) {
            memset(tmp, ' ', 29);
            sprintf(tmp, "%s", chn_ptr->channeluser[i]->authname);
            tmp[strlen(tmp)] = ' ';
            PrintCFlags(tmp + 21, currentflags);
            NoticeToUser(usr_ptr, "%s", tmp);
            voices++;
          }
        }
      }
      NoticeToUser(usr_ptr, "Total: %d (max %d users) (owner: %d, master: %d, op: %d, voice: %d).",
                   (owners+masters+ops+voices),USERSPERCHANNEL,owners,masters,ops,voices);
      NoticeToUser(usr_ptr, "End of chanlev for %s.", chn_ptr->channelname);
    } else {
      /* They specified a single user to list. */
      act_ptr = FindAccount(target);
      if (NULL == act_ptr) {
        NoticeToUser(usr_ptr, "Unknown user %s.", target);
        return;
      }
      for (i = 0; i < CHANNELSPERUSER; i++) {
        if (NULL != act_ptr->channel[i]) {
          if (act_ptr->channel[i] == chn_ptr) {
            memset(tmp, ' ', 29);
            if (shown == 0) {
              shown = 1;
              NoticeToUser(usr_ptr, "Users for channel %s", chn_ptr->channelname);
              NoticeToUser(usr_ptr, "Authname         Access flags");
              NoticeToUser(usr_ptr, "-----------------------------");
            }
            sprintf(tmp, "%s", act_ptr->authname);
            tmp[strlen(tmp)] = ' ';
            PrintCFlags(tmp + 21, act_ptr->channelflags[i]);
            NoticeToUser(usr_ptr, "%s", tmp);
            break;
          }
        }
      }
      if (shown == 0)
        NoticeToUser(usr_ptr, "%s is not known on %s.", act_ptr->authname, chn_ptr->channelname);
      else
        NoticeToUser(usr_ptr, "End of chanlev for %s.", chn_ptr->channelname);
    }
  }
}

void ChangeChanlev(struct user *usr_ptr, char *channame, char *target, char *flags)
{
  struct reggedchannel *chn_ptr;
  struct account *act_ptr;
  int isnewuser = 0;

  chn_ptr = GetChannelPointer(channame);
  if (NULL == chn_ptr) {
    NoticeToUser(usr_ptr, "Unknown channel %s.", channame);
    return;
  }
  if (!IsAdmin(usr_ptr) && (-1 == GetUserChannelIndex(chn_ptr, usr_ptr->authedas))) {
    NoticeToUser(usr_ptr, "You're not known on %s.", chn_ptr->channelname);
    return;
  }
  act_ptr = FindAccount(target);
  if (act_ptr == usr_ptr->authedas)
    if (strlen(flags))
      if ('-' == *flags && (NULL == strchr(flags, '+')))
        goto UserSelfSkip;

  if (!IsAdmin(usr_ptr)
      && (!(GetChannelFlags(usr_ptr->authedas, chn_ptr) & (CFLAG_MASTER | CFLAG_OWNER)))) {
    NoticeToUser(usr_ptr, "You're not the owner or a master of %s.", chn_ptr->channelname);
    return;
  }
  act_ptr = FindAccount(target);
  if (NULL == act_ptr) {
    NoticeToUser(usr_ptr, "Unknown user %s.", target);
    return;
  }

  if (strlen(act_ptr->authname) < 2) {
    NoticeToUser(usr_ptr, "You cannot give QuakeNet services a chanlev.");
    return;
  }

  if (-1 == GetUserChannelIndex(chn_ptr, act_ptr)) {
    if (NULL == AddChannelToAccount(act_ptr, chn_ptr)) {
      /* User not known and couldn't add either... */
      NoticeToUser(usr_ptr, "Couldn't add %s to %s.", act_ptr->authname, chn_ptr->channelname);
      NoticeToUser(usr_ptr, "Either there is no room for more users on %s", channame);
      NoticeToUser(usr_ptr, "or there is no room for more channels on %s's account.", target);
      return;
    } else {
      isnewuser = 1;            /* Set the "isnewuser" flag here; if we abort we need to delete the user again */
    }
  }
UserSelfSkip:                  /* if the user wants to change his own modes, only - works */
  {
    char *UCflags;
    char *TCflags;
    char tempflags; /* Make a copy so we either do all changes or none */
    char modetoset = 1;
    int i;

    UCflags = &usr_ptr->authedas->channelflags[GetUserChannelIndex(chn_ptr, usr_ptr->authedas)];
    TCflags = &act_ptr->channelflags[GetUserChannelIndex(chn_ptr, act_ptr)];
    tempflags = *TCflags;
    /* You're allowed to change your own flags, but otherwise you cannot change 
     * *ANY* flags on a master unless you are owner */
    if (!(*UCflags & CFLAG_OWNER) && (*TCflags & (CFLAG_OWNER | CFLAG_MASTER))
        && (TCflags != UCflags) && !IsAdmin(usr_ptr)) {
      NoticeToUser(usr_ptr, "Only owners can change a master's flags.");
      return;                   /* No need to do the 'isnewuser' check here -- the target user was +m so can't be new */
    }
    for (i = 0; '\0' != flags[i]; i++) {
      if ('+' == flags[i])
        modetoset = 1;
      else if ('-' == flags[i])
        modetoset = 0;
      else if ('a' == flags[i] || 'v' == flags[i] || 'g' == flags[i] || 'o' == flags[i])
        SetChanFlag(modetoset, flags[i], &tempflags);
      else if ('m' == flags[i]) {
        /* Again, you can manipulate the m flag of yourself if you are a master */
        if (!(*UCflags & CFLAG_OWNER) && !IsAdmin(usr_ptr) && (TCflags != UCflags)) {
          NoticeToUser(usr_ptr, "Only owners can add or remove the master flag.");
          if (isnewuser) {
            RemoveChannelFromAccount(act_ptr, chn_ptr);
          }
          return;
        }
        SetChanFlag(modetoset, flags[i], &tempflags);
      } else if ('n' == flags[i]) {
        if (!(*UCflags & CFLAG_OWNER) && !IsAdmin(usr_ptr)) {
          NoticeToUser(usr_ptr, "Only owners can add or remove the owner flag.");
          if (isnewuser) {
            RemoveChannelFromAccount(act_ptr, chn_ptr);
          }
          return;
        }
        /* "only owner" warning: Only trigger if:
         *  - Mode is being removed
         *  - The channel has exactly one +n and more than one user
         *  - The user making the change isn't an oper
         *  - The target user is actually owner on the channel (use SAVED flags not working copy 
         *    so that +n-n works).
         */
        if (!modetoset && !CanRemoveOwner(chn_ptr) && !IsAdmin(usr_ptr)
            && ((*TCflags) & CFLAG_OWNER)) {
          NoticeToUser(usr_ptr,
                       "You're the only owner left for channel %s, you may not remove your +n flag at this point. Either remove all other users and then remove your owner flag, or give the owner flag to someone else before removing yours.",
                       chn_ptr->channelname);
          return;
        }
        SetChanFlag(modetoset, flags[i], &tempflags);
      } else {
        /* invalid flag -- abort the operation */
        /* This prevents people being tricked into +av/only etc. */
        NoticeToUser(usr_ptr, "Invalid flag specified.");
        NoticeToUser(usr_ptr, "Valid flags are: agmnov");
        if (isnewuser) {
          RemoveChannelFromAccount(act_ptr, chn_ptr);
        }
        return;
      }
    }
    /* We've got to the end */
    /* If the user has no flags left, delete */
    if (!(tempflags & ~(CFLAG_AUTO | CFLAG_GIVE))) {
      RemoveChannelFromAccount(act_ptr, chn_ptr);
    } else {
      *TCflags = tempflags;
    }
  }

  Log("ChangeChanlev: %s (%s) changed flags (%s) for %s on %s", usr_ptr->nick, usr_ptr->authedas->authname, flags,
      target, channame);
  NoticeToUser(usr_ptr, "Done.");
}

/*
void AddToChan(struct user* usr_ptr,char* channelname, char* name) {
	struct account* act_ptr;
	struct reggedchannel* chn_ptr;
	act_ptr = FindAccount(name);
	if (NULL == act_ptr) {
		printf("%s no such user or account",name);
		return;
	}
	chn_ptr = GetChannelPointer(channelname);
	if (NULL == chn_ptr) printf("%s no such channel or you're not known on the channel",channelname);
	else if (-1 == GetUserChannelIndex(usr_ptr->authedas,chn_ptr)) printf("%s no such channel or you're not known on the channel",channelname);
	if (GetChanFlag(char,char);
	

}
void RemoveFromChan(struct user* usr_ptr, char* channelname, char* name) {
}
*/

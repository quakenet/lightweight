/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: accountsdb.c,v 1.32 2003/09/08 01:19:25 zarjazz Exp $
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
#include <channelsdb.h>
#include <accountsdb.h>
#include <globalexterns.h>
#include <usersdb.h>
#include <base64.h>
#include <channels.h>

/* the user channelflags bitmasks */
#define NFLAGS 8
static unsigned char cflagmasks[NFLAGS] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
static char cflags[NFLAGS] = { 'a', 'b', 'm', 'n', 'o', 't', 'v', 'g' };

/* x is reserved for future use */

/* internal function headers */
void CreateValidAuthName(char *, char *);
void AddAccountToHash(struct account *);
void DeleteAccount(struct account *);
char getcfbmask(char);

extern struct account *DBAccountMalloc(void);
extern void DBAccountFree(struct account *);

struct account *AddAccount(char *user_nick)
{
  struct account *new_act;

  new_act = DBAccountMalloc();  /* get space */
  if (NULL == new_act) {
    /* out of memory */
    return NULL;
  }
  new_act->currentuser = NULL;  /* new account, no auths */
  new_act->authlevel = 1;       /* default authlevel */
  new_act->lastauth = (time_t) 0; /* hasn't outhed yet */
  CreateValidAuthName(new_act->authname, user_nick);  /* get an authname */
  memset(new_act->channel, '\0', sizeof(void *) * CHANNELSPERUSER); /* clear the channels */
  memset(new_act->channelflags, '\0', CHANNELSPERUSER); /* and channelflags */
  AddAccountToHash(new_act);    /* and add the new account to db */
  return new_act;
}

struct account *RemoveAccount(struct account *tmp_ptr)
{
/*	struct account* tmp_ptr;*/
  int i;

/*	tmp_ptr = GetAccountByAuthName(authname);*//* get the account struct */
  if (NULL == tmp_ptr)
    return NULL;                /* make sure there's something to delete */
  {                             /* let's remove the auth from all users authed on this account */
    struct usersauthed *wrk_ptr1, *wrk_ptr2;  /* something to work with */

    wrk_ptr1 = tmp_ptr->currentuser;  /* start the auth list */
    while (NULL != wrk_ptr1) {  /* go through it */
      wrk_ptr1->autheduser->authedas = NULL;  /* remove auth from user */
      wrk_ptr2 = wrk_ptr1->nextinlist;  /* swap & next in list */
      free(wrk_ptr1);           /* free the list element */
      wrk_ptr1 = wrk_ptr2;      /* swap back */
    }
  }
  for (i = 0; i < CHANNELSPERUSER; i++) /* remove user from all channels */
    if (NULL != tmp_ptr->channel[i])
      RemoveChannelFromAccount(tmp_ptr, tmp_ptr->channel[i]);
  CleanUserReferences(tmp_ptr);
  DeleteAccount(tmp_ptr);       /* and delete it */
  return tmp_ptr;
}

struct account *FindAccount(char *authornick)
{
  struct user *tmp_ptr;

  if (NULL == authornick)
    return NULL;                /* no NULL pointers please */
  if ('\0' == *authornick)
    return NULL;                /* or empty strings */
  if ('#' == *authornick)
    return GetAccountByAuthName(authornick + sizeof(char)); /* it was an authname */
  tmp_ptr = FindUserByNick(authornick); /* get the user for the nick */
  if (NULL == tmp_ptr)
    return NULL;                /* no such nick ? */
  return tmp_ptr->authedas;     /* return the account the user is authed as, can be NULL */
}

void AddAccountToHash(struct account *new_act)
{
  long int hash_value;

  hash_value = hash(new_act->authname); /* hash the authname */
  new_act->nextbyauthname = accounthashtable[hash_value]; /* add the rest of the list */
  accounthashtable[hash_value] = new_act; /* add new channel to start of list */
}

void DeleteAccount(struct account *act_ptr)
{
  struct account **tmp_ptr;

  tmp_ptr = &accounthashtable[hash(act_ptr->authname)]; /* get the list start */
  while (NULL != *tmp_ptr) {    /* go through entire list */
    if (*tmp_ptr == act_ptr) {  /* did we find the correct account ? */
      /* found it! */
      struct account *tmp_ptr2; /* small temporary pointer */

      tmp_ptr2 = act_ptr->nextbyauthname; /* store the next in list */
      DBAccountFree(act_ptr);   /* remove the account */
      *tmp_ptr = tmp_ptr2;      /* restore the list */
      return;                   /* happy */
    }
    tmp_ptr = &(*tmp_ptr)->nextbyauthname;  /*´advance in list */
  }
}

char *ChangeAuthName(struct account *act_ptr, char *newauthname)
{
  short int authname_len = strlen(newauthname); /* get the new name length */

  if (authname_len <= 0 || authname_len > NICKLEN)
    return NULL;                /* is the name good ? */
  if (NULL != GetAccountByAuthName(newauthname))
    return NULL;                /* is it reserved ? */
  memcpy(act_ptr->authname, newauthname, authname_len + 1); /* no, let's use it */
  return newauthname;           /* success */
}

void SetAuthLevel(struct account *act_ptr, unsigned char newauthlevel)
{
  act_ptr->authlevel = newauthlevel;  /* just set the level, all values do */
}

void CreateValidAuthName(char *target, char *user_nick)
{
  short int nick_len;

  nick_len = strlen(user_nick); /* get the length of the nick */
  if (nick_len <= 0 || nick_len > NICKLEN)
    Error(ERR_INTERNAL | ERR_FATAL, "CreateValidAuthName() invalid nick");
  memcpy(target, user_nick, nick_len + 1);  /* copy the nick to the authname field */

  if (NULL != GetAccountByAuthName(target)) { /* check if the authname is used */
    short int i = 0;  /* it is */

    if (nick_len > 8)
      nick_len = 8;             /* decrease the nick to first 8 characters */
    do {
      sprintf(target + nick_len, "%d%c", i, '\0');  /* add an integer to end of authname */
      i++;
    } while (NULL != GetAccountByAuthName(target)); /* do until we have a free authname */
  }
}

struct account *GetAccountByAuthName(char *authname)
{
  struct account *tmp_ptr;  /* working pointer */

  tmp_ptr = accounthashtable[hash(authname)]; /* get the start of list */
  while (NULL != tmp_ptr) {     /* go through all */
    if (!Strcmp(tmp_ptr->authname, authname)) /* Case insensitive */
      return tmp_ptr;           /* if name matches return pointer */
    tmp_ptr = tmp_ptr->nextbyauthname;  /* if not, go to next */
  }
  return NULL;                  /* not found */
}

struct reggedchannel **GetChannelUserEntry(struct reggedchannel *chn_ptr, struct account *act_ptr)
{
  int i;

  for (i = 0; i < CHANNELSPERUSER; i++) /* go through the array - linear search */
    if (act_ptr->channel[i] == chn_ptr)
      return (&(act_ptr->channel[i]));  /* if found, return */

  return NULL;                  /* not found */
}

short int IsAdmin(struct user *usr_ptr)
{
  if (NULL == usr_ptr)
    return 0;
  if (usr_ptr->oper && usr_ptr->authedas->authlevel > 200)
    return 1;
  return 0;
}

short int GetUserChannelIndex(struct reggedchannel *chn_ptr, struct account *act_ptr)
{
  int i;

  for (i = 0; i < CHANNELSPERUSER; i++) { /* see if the user is on the channels list */
    /* RISK */
    /* if (chn_ptr->channeluser[i] == act_ptr) return i; */
/* RISK *//*printf("%ld %ld\n",act_ptr->channel[i],chn_ptr */
    if (act_ptr->channel[i] == chn_ptr)
      return i;
  }
  return -1;                    /* not found */
}

struct reggedchannel *RemoveChannelFromAccount(struct account *act_ptr, struct reggedchannel *chn_ptr)
{
  struct reggedchannel **tmp_chn_ptr;
  struct account **tmp_act_ptr;
  int i;

  tmp_act_ptr = GetUserChannelEntry(chn_ptr, act_ptr);  /* find the user on the channel */
  if (NULL == tmp_act_ptr)
    return NULL;                /* problem */
  tmp_chn_ptr = GetChannelUserEntry(chn_ptr, act_ptr);  /* find the channel on the user */
  if (NULL == tmp_chn_ptr)
    return NULL;                /* problem^2 */
  *tmp_act_ptr = NULL;          /* remove user from channel */
  *tmp_chn_ptr = NULL;          /* remove channel from user */
  for (i = 0; i < USERSPERCHANNEL; i++)
    if (NULL != chn_ptr->channeluser[i])
      return chn_ptr;
  RemoveChannel(chn_ptr);
  return chn_ptr;               /* success */
}

struct reggedchannel *AddChannelToAccount(struct account *act_ptr, struct reggedchannel *chn_ptr)
{
  struct reggedchannel **tmp_chn_ptr;
  struct account **tmp_act_ptr;

  if (NULL != GetUserChannelEntry(chn_ptr, act_ptr))
    return NULL;                /* user already known on channel */
  if (NULL != GetChannelUserEntry(chn_ptr, act_ptr))
    return NULL;                /* user already known on channel */
  tmp_act_ptr = GetUserChannelEntry(chn_ptr, NULL); /* find an empty space on channel */
  if (NULL == tmp_act_ptr)
    return NULL;                /* channel full */
  tmp_chn_ptr = GetChannelUserEntry(NULL, act_ptr); /* find an empty space on user */
  if (NULL == tmp_chn_ptr)
    return NULL;                /* user has max channels */
  *tmp_act_ptr = act_ptr;       /* register the user */
  *tmp_chn_ptr = chn_ptr;       /* to the channel */
  act_ptr->channelflags[GetUserChannelIndex(chn_ptr, act_ptr)] = '\0';  /* reset chanlev.. don't ask */
  return chn_ptr;
}

short int AutoModes(char *numericid, char *channame)
{
  struct account *act_ptr;  /* the account for numeric id */
  struct reggedchannel *chn_ptr;  /* channel for channel name */
  int uchanindex;
  char uchanflag;

  chn_ptr = GetChannelPointer(channame);  /* get the channel */
  if (NULL == chn_ptr)
    return AUTOMODE_UNREG;      /* make sure it exists */
  if (IsSuspended(chn_ptr))
    return AUTOMODE_UNREG;      /* make sure it isn't suspended */
  act_ptr = usertablepointer[getserverindex(numericid)][getclientindex(numericid)]->authedas; /* get the account */
  if (NULL == act_ptr)
    return AUTOMODE_NOTHING;    /* it has to exist aswell */
  uchanindex = GetUserChannelIndex(chn_ptr, act_ptr); /* get the users entry on channel */
  if (-1 == uchanindex)
    return AUTOMODE_NOTHING;    /* and it should exist too */
  /* User is known on the channel after this. */
  uchanflag = act_ptr->channelflags[uchanindex];  /* get the channelflags */
  if ((uchanflag & CFLAG_AUTO) && (uchanflag & CFLAG_OP)) /* check for ao */
    return AUTOMODE_OP;
  if ((uchanflag & (CFLAG_AUTO | CFLAG_GIVE)) && (uchanflag & CFLAG_VOICE)) /* check for av */
    return AUTOMODE_VOICE;
  /* If the user doesnt have any autoflags, return that. */
  return AUTOMODE_NOAUTOFLAGS;
}

void DoAllAutoModes(struct user *usr_ptr)
{
  int i;
  char cflags;  /* copy of the current channel flags */
  struct reggedchannel *chn_ptr;

  if (usr_ptr == NULL || usr_ptr->authedas == NULL) {
    Error(ERR_INTERNAL | ERR_WARNING, "DoAllAutoModes() called on non-authed user");
    return;
  }

  /* Note that we don't join channels just for this, so if we're sitting
   * on channels and we're not on a particular channel the user has flags on,
   * assume the user isn't there either */
  for (i = 0; i < CHANNELSPERUSER; i++) {
    chn_ptr = usr_ptr->authedas->channel[i];
#ifdef SIT_ON_CHANNELS
    if (chn_ptr != NULL && !IsSuspended(chn_ptr) && IsJoined(chn_ptr)) {
#else
    if (chn_ptr != NULL && !IsSuspended(chn_ptr)) {
#endif
      cflags = usr_ptr->authedas->channelflags[i];
#ifndef AUTOMODES_IN_BURST
      if (burst_done) {
#endif
        if ((cflags & CFLAG_AUTO) && (cflags & CFLAG_OP)) {
          /* AUTO and OP makes AUTOOP! */
          OpUser(usr_ptr->authedas->channel[i], usr_ptr->numeric);
        } else if ((cflags & (CFLAG_AUTO | CFLAG_GIVE)) && (cflags & CFLAG_VOICE)) {
          /* AUTO and VOICE makes AUTOVOICE! */
          /* (or give voice too ;) */
          VoiceUser(usr_ptr->authedas->channel[i], usr_ptr->numeric);
        }
#ifndef AUTOMODES_IN_BURST
      }
#endif
    }
  }
}

char GetChannelFlag(struct account *act_ptr, struct reggedchannel *chn_ptr, char cflagid)
{
  int uindex;

  uindex = GetUserChannelIndex(chn_ptr, act_ptr);
  if (-1 == uindex)
    return '\0';
  return getchanflag(act_ptr->channelflags[uindex], cflagid);

}

char GetChannelFlags(struct account *act_ptr, struct reggedchannel *chn_ptr)
{
  int uindex;

  uindex = GetUserChannelIndex(chn_ptr, act_ptr);
  if (-1 == uindex)
    return '\0';
  return act_ptr->channelflags[uindex];
}

char getcfbmask(char chanflag)
{
  int i;

  for (i = 0; i < NFLAGS; i++)  /* find the correct character - mask pair */
    if (cflags[i] == chanflag)
      return cflagmasks[i];     /* return it */
  return 0;                     /* not found */
}

char getchanflag(char chanflag, char flag_ptr)
{
  return flag_ptr & getcfbmask(chanflag); /* check if the chanflag bit is on */
}

char SetChanFlag(char settype, char chanflag, char *flag_ptr)
{
  char bmask;

  bmask = getcfbmask(chanflag); /* get the mask for character */
  if (0 == bmask)
    return 0;                   /* let's not do anything stupid */
  if (settype)
    *flag_ptr |= bmask;         /* if were're putting it on just | it */
  else
    *flag_ptr &= ~bmask;        /* taking of, & with inverse */
  return chanflag;
}

void PrintCFlags(char *print_ptr /* char[8] */ , char flag_ptr)
{
  int i, j; /* the variables, i is chanflag, j is the position in string */

  memset(print_ptr, ' ', NFLAGS); /* set the target to space (default) */
  for (i = j = NFLAGS - 1; i >= 0; i--) { /* go through all flags */
    if (cflagmasks[i] & flag_ptr) { /* if the flag is set */
      print_ptr[j] = cflags[i]; /* print the corresponding character to the end of target */
      j--;                      /* move closer to start of target */
    }
  }
}

void SetUserChannelFlags(struct account *act_ptr, struct reggedchannel *chn_ptr, char *flags)
{
  char setmode = 1;
  char *tmp_ptr;
  int i = 0;
  int idx = 0;

  idx = GetUserChannelIndex(chn_ptr, act_ptr);
  if (idx == -1) {
    Error(ERR_WARNING | ERR_INTERNAL, "SetUserChannelFlags called where account %s not on channel %s",
          act_ptr->authname, chn_ptr->channelname);
    return;
  }

  tmp_ptr = &(act_ptr->channelflags[idx]);
  /* ^- the channelflags we're modifying */
  while ('\0' != flags[i]) {    /* go through the string */
    if ('+' == flags[i])
      setmode = 1;              /* are we setting or */
    else if ('-' == flags[i])
      setmode = 0;              /* unsetting flags */
    else if (getcfbmask(flags[i]))
      SetChanFlag(setmode, flags[i], tmp_ptr);  /* set it */
    i++;                        /* next */
  }
}

/* This function removes all references to the specified user pointer from
 * suspendedby and founder fields */

void CleanUserReferences(struct account *act_ptr)
{
  int i;
  struct reggedchannel *chan;

  for (i = 0; i < HASHMAX; i++)
    for (chan = channelhashtable[i]; chan; chan = chan->nextbychannelname) {
      if (chan->founder == act_ptr)
        chan->founder = NULL;

      if (chan->suspendby == act_ptr)
        chan->suspendby = NULL;
    }
}

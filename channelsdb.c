/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: channelsdb.c,v 1.27 2004/08/29 19:06:18 froo Exp $
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
#include <channels.h>
#include <globalexterns.h>

/* internal function definitions */
void AddChannelToHash(struct reggedchannel *);

extern struct reggedchannel *DBMallocChannel(void);
extern void DBFreeChannel(struct reggedchannel *);

void AddChannelToHash(struct reggedchannel *chn_ptr)
{
  long int hash_value;

  hash_value = hash(chn_ptr->channelname);  /* hash the channel name */
  chn_ptr->nextbychannelname = channelhashtable[hash_value];  /* save the rest of the list */
  channelhashtable[hash_value] = chn_ptr; /* and add new to the start of it */
}

struct reggedchannel *AddChannel(char *chn_name, struct account *owner_act, char *addedby, int lastused,
                                 struct user *user)
{
  struct reggedchannel *tmp_ptr;
  short int chname_len;

  if (NULL != GetChannelPointer(chn_name)) {
    if (user)
      NoticeToUser(user, "Error adding channel: The channel %s seems to have L already.", chn_name);
    return NULL;                /* make sure the channels isn't there already */
  }
  chname_len = strlen(chn_name);
  if (29 < chname_len) {
    if (user)
      NoticeToUser(user, "Error adding channel: channel name too long.");
    return NULL;                /* is the channel name too long ? */
  }
  tmp_ptr = DBMallocChannel();  /* get memory for the channel */
  if (NULL == tmp_ptr)
    return NULL;                /* was there memory */
  memset(tmp_ptr->channeluser, '\0', sizeof(void *) * USERSPERCHANNEL); /* initialise users to NULL */
  memcpy(tmp_ptr->channelname, chn_name, chname_len + 1);
  if (NULL != owner_act) {      /* was the channel owner defined - doublecheck, owner is verified in addchan.c */
    if (NULL == AddChannelToAccount(owner_act, tmp_ptr)) {
      DBFreeChannel(tmp_ptr);   /* this happens ONLY if the users is already on maxchannels */
      if (user)
        NoticeToUser(user, "Error adding channel: user is on the maximum number of channels.");
      return NULL;
    }
    owner_act->channelflags[GetUserChannelIndex(tmp_ptr, owner_act)] = (char) 0x1D; /* give the owner +aonm */
  }
  a_strcpy(tmp_ptr->addedby, addedby, sizeof(tmp_ptr->addedby));
  tmp_ptr->founder = owner_act;
  tmp_ptr->suspendby = NULL;
  tmp_ptr->welcome = NULL;
  tmp_ptr->suspendreason = NULL;
  tmp_ptr->flags = 0;           /* 0 is default value */
  tmp_ptr->dateadded = 0;       /* This should be filled in later, but just in case.. */
  if (lastused) {
    /* Loaded channel. */
    tmp_ptr->lastused = lastused;
  } else {
    /* Fresh channel. */
    tmp_ptr->lastused = time(NULL);
#ifdef SIT_ON_CHANNELS
    /* If we're creating a new channel with addchan we make it join the channel */
    /* This is a bad idea if we're loading the channel from disk, so moved this into here */
    CheckJoined(tmp_ptr);
#endif
  }
  AddChannelToHash(tmp_ptr);    /* add the channel to hash */
  return tmp_ptr;
}

void SetChannelSuspend(struct account *act_ptr, struct reggedchannel *chn_ptr, int settype, char *reason)
{
  if (!settype) {
    if (NULL == chn_ptr->suspendby)
      return;
    ClearSuspended(chn_ptr);
    chn_ptr->suspendby = NULL;
    if (NULL == chn_ptr->suspendreason)
      return;
    free(chn_ptr->suspendreason);
    chn_ptr->suspendreason = NULL;
    return;
  }
  if (NULL != chn_ptr->suspendby)
    SetChannelSuspend(act_ptr, chn_ptr, 0, NULL);
  chn_ptr->suspendby = act_ptr;
  SetSuspended(chn_ptr);
  if (NULL == reason)
    return;
  chn_ptr->suspendreason = strdup(reason);
}

struct reggedchannel *RemoveChannel(struct reggedchannel *chn_ptr)
{
  int i;
  struct reggedchannel **tmp_ptr;

  if (NULL == chn_ptr)
    return NULL;                /* let's not delete something that isn't there to start with */
  for (i = 0; i < USERSPERCHANNEL; i++) /* remove all users from channel */
    if (NULL != chn_ptr->channeluser[i])
      RemoveChannelFromAccount(chn_ptr->channeluser[i], chn_ptr);
  tmp_ptr = &channelhashtable[hash(chn_ptr->channelname)];  /* get the start of list */
  while (*tmp_ptr != NULL) {    /* go through all */
    if (*tmp_ptr == chn_ptr) {
      /* found it */
      struct reggedchannel *tmp_ptr2; /* just remove the list element */

      PartChannel(chn_ptr, NULL);     /* Deleting chan? get off it first */
      tmp_ptr2 = chn_ptr->nextbychannelname;
      free(*tmp_ptr);
      *tmp_ptr = tmp_ptr2;
      return chn_ptr;
    }
    tmp_ptr = &((*tmp_ptr)->nextbychannelname); /* next in list */
  }
  return NULL;                  /* very, very bad */

}

struct reggedchannel *GetChannelPointer(char *chan_name)
{
  struct reggedchannel *tmp_ptr;

  tmp_ptr = channelhashtable[hash(chan_name)];  /* get the start of list */
  while (NULL != tmp_ptr)       /* go through the list */
    if (!Strcmp(tmp_ptr->channelname, chan_name)) /* compare names (case insensitive) */
      return tmp_ptr;           /* if match, return the pointer */
    else
      tmp_ptr = tmp_ptr->nextbychannelname; /* next in list */
  return NULL;                  /* not found */
}

struct account **GetUserChannelEntry(struct reggedchannel *chn_ptr, struct account *act_ptr)
{
  int i;

  for (i = 0; i < USERSPERCHANNEL; i++)
    /* see if the user is on the channels list */
    if (chn_ptr->channeluser[i] == act_ptr)
      return &(chn_ptr->channeluser[i]);
  return NULL;                  /* not found */
}

#ifdef TOPICSAVE
void SetChannelTopic(struct reggedchannel *chn_ptr, char *new_topic)
{
  a_strcpy(chn_ptr->topic, new_topic, sizeof(chn_ptr->topic));  /* copy the new topic */
}

char *GetChannelTopic(struct reggedchannel *chn_ptr)
{
  return chn_ptr->topic;
}
#endif

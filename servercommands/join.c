/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: join.c,v 1.18 2003/09/08 01:19:25 zarjazz Exp $
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
#include <channels.h>
#include <channelsdb.h>

#include <base64.h>

/*
  - FindUserByNumeric
  - if isauthed
    then
      For each channel which account has flags on
      do
        if ChannelJoined == channel
          applyflags on channel

   I've changed this slightly -- splidge
   We now rely on strutsi's AutoModes() function to tell us what to do...
   It's slightly inefficient in the case of a user joining multiple channels,
   but few users do that (and I mean _slightly_ -- numeric lookups are cheap).
*/

/*
  Problem, in case we wants welcome messages, we need to find the channel first,
  but this can not be done in O(1) as the above can.
  Ditch the welcome message idea?
*/

void DoJoins(char *numeric, char *channellist, time_t timestamp)
{
  char *currentchan;
  char *nextchan = channellist;
  struct reggedchannel *chanptr;
  struct user *user_ptr;
  int automodes;

  /* Right then.  First up, let's see if it's 0 being joined here */
  /* If so, we need to do all the work of a part (i.e. nothing) for each channel */

  if (!strncmp(channellist, "0", 1))
    return;

  user_ptr = usertablepointer[getserverindex(numeric)][getclientindex(numeric)];

  while (*nextchan) {
    currentchan = nextchan;

    /* The next two blocks are going to turn currentchan into a pointer 
     * to a well-formed string.
     * nextchan will point to the first char of the next channel name, if any
     * or NULL if this is the last one */

    for (; *nextchan != ',' && *nextchan != '\0'; nextchan++);  /* Look for the next comma, or end of list */

    /* If we stopped on a comma, change to a NUL and advance nextchan
     * Otherwise, it was a NUL already, and we leave nextchan there so 
     * the loop will stop next time round */
    if (*nextchan == ',') {
      *nextchan = '\0';
      nextchan++;
    }

    chanptr = GetChannelPointer(currentchan);
    if (chanptr == NULL)        /* It's not a channel we care about, carry on with the next one */
      continue;

    if (IsSuspended(chanptr))   /* Same as above, we dont really care about this channel */
      continue;

    /* Check the timestamp */
    if (timestamp > 0)
      SyncTimestamp(chanptr, timestamp);

    if (chanptr->welcome)
      NoticeToUser(user_ptr, "[%s] %s", chanptr->channelname, chanptr->welcome);

    automodes = AutoModes(numeric, currentchan);

    /* Update lastused timestamp on channel if user is known on it. */
    if (automodes > 0) {
      chanptr->lastused = time(NULL);
    }

    switch (automodes) {

    case AUTOMODE_NOAUTOFLAGS:
      /* User is known, but shouldnt have voice nor op automatically. */
      /* Do the same as for AUTOMODE_NOTHING (it could be a voice user). */
    case AUTOMODE_NOTHING:
      /* It's a registered channel, which the user has no modes on */
      /* We need to deop them if it's a create */
      /* If the channel is inviteonly and it's a create, */
      /* then kick their ass if they're unknown. */
      /* We rely on the channel to be +i at all times after a createa */
      /* if we have inviteonly set as chanflag. */
      if (timestamp > 0) {
        if (!IsInviteOnly(chanptr)) { /* Channel is a normal channel */
          DeopUser(chanptr, numeric);
        } else {                /* Channel is invite only. */
          if (automodes == AUTOMODE_NOAUTOFLAGS) {
            DeopUser(chanptr, numeric); /* User has flags, so shouldnt be kicked. */
          } else {              /* Must be a AUTOMODE_NOTHING, so no flags, and therefore not known on the channel. */
            KickUser(chanptr, user_ptr->numeric, "Channel is invite only");
          }
        }
      }
      break;

    case AUTOMODE_OP:
      /* It's a registered channel, which the user has ops on */
      /* We need to op them if and only if it's NOT a create */

      if (timestamp == 0)
        OpUser(chanptr, numeric);
      else
        CheckJoined(chanptr);
      break;

    case AUTOMODE_VOICE:
      /* It's a registered channel, the user has autovoice */
      /* If it's a create, deop-and-voice, 
       * if it's a join, just voice */

      if (timestamp == 0) {
        VoiceUser(chanptr, numeric);
      } else {
        CheckJoined(chanptr);
        VoiceAndDeopUser(chanptr, numeric);
      }
      break;

    default:
      /* It's an unregistered channel, do nothing */
      /* Note that with the continue above we shouldn't get here any more */
      break;
    }
  }
}

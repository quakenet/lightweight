/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: suspend.c,v 1.7 2004/08/29 19:06:18 froo Exp $
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
#include <channelsdb.h>
#include <accountsdb.h>
#include <channels.h>

/* 
 * suspend: Suspends a channel
 *
 * Parameters:
 *  #channelname  - obvious 
 *  reason        - Reason for suspension
 */

void dosuspend(struct user *user, char *tail)
{
  char *channel;
  char *reason;
  struct reggedchannel *chan;

  if (!CheckAuthLevel(user, 240))
    return;

  /* Get the parameters */
  channel = tail;
  reason = SeperateWord(channel);

  /* No SeperateWord on reason -- we want the rest of this line for the reason */

  /* Check that we've got enough parameters and that they are sane */
  if (reason == NULL) {
    NoticeToUser(user, "Usage: suspend #channel reason");
    NoticeToUser(user, "where  #channel is channel to suspend");
    NoticeToUser(user, "       reason   is why the channel is being suspended");
    return;
  }

  if (channel[0] != '#') {
    NoticeToUser(user, "The channel name must start with #.");
    return;
  }

  if ((chan = GetChannelPointer(channel)) == NULL) {
    NoticeToUser(user, "Couldn't find that channel.");
    return;
  }

  if (IsSuspended(chan)) {
    NoticeToUser(user, "That channel is already suspended.");
    return;
  }

  PartChannel(chan, "Channel suspended.");
  SetSuspended(chan);
  chan->suspendby = user->authedas;
  chan->suspendreason = strdup(reason);

  Log("Suspend: %s (%s) requested suspend %s due to '%s'", user->nick, user->authedas->authname, channel, reason);
  NoticeToUser(user, "Channel suspended.");
}

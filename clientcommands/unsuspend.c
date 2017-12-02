/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: unsuspend.c,v 1.5 2002/05/05 11:18:32 bigfoot Exp $
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
 * unsuspend: Unsuspends a channel
 *
 * Parameters:
 *  #channelname  - obvious 
 */

void dounsuspend(struct user *user, char *tail)
{
  char *channel;
  struct reggedchannel *chan;

  if (!CheckAuthLevel(user, 240))
    return;

  /* Get the parameters */
  channel = tail;
  SeperateWord(channel);

  /* Check that we've got enough parameters and that they are sane */
  if (channel == NULL) {
    NoticeToUser(user, "Usage: unsuspend #channel");
    NoticeToUser(user, "where  #channel is channel to unsuspend");
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

  if (!IsSuspended(chan)) {
    NoticeToUser(user, "That channel is not suspended.");
    return;
  }

  ClearSuspended(chan);
  CheckJoined(chan);
  chan->suspendby = NULL;
  free(chan->suspendreason);
  chan->suspendreason = NULL;

  Log("UnSuspend: %s (%s) requested unsuspend %s", user->nick, user->authedas->authname, channel);
  NoticeToUser(user, "Channel unsuspended.");
}

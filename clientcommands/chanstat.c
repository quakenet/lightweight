/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: chanstat.c,v 1.11 2004/01/11 20:29:47 froo Exp $
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
 * chanstat: List channel stats
 *
 * Parameters:
 *  #channelname  - obvious 
 */

void dochanstat(struct user *user, char *tail)
{
  char *channel;
  struct reggedchannel *chan;

  if (!CheckAuthLevel(user, 10))
    return;

  /* Get the parameters */
  channel = tail;
  SeperateWord(channel);

  /* Check that we've got enough parameters and that they are sane */
  if (channel == NULL) {
    NoticeToUser(user, "Usage: chanstat #channel");
    NoticeToUser(user, "where  #channel is channel to examine");
    return;
  }

  if (channel[0] != '#') {
    NoticeToUser(user, "The channelname must start with #.");
    return;
  }

  if ((chan = GetChannelPointer(channel)) == NULL) {
    NoticeToUser(user, "Couldn't find that channel.");
    return;
  }

  NoticeToUser(user, "Channel        : %s", chan->channelname);
  NoticeToUser(user, "Added by       : %s", chan->addedby);
  NoticeToUser(user, "Founder        : %s", (chan->founder == NULL ? "???" : chan->founder->authname));
  NoticeToUser(user, "Date added     : %s", ctime(&chan->dateadded));
  NoticeToUser(user, "Last join      : %s", ctime(&chan->lastused));
  NoticeToUser(user, "Suspended      : %s", IsSuspended(chan) ? "Yes" : "No");
  if (IsSuspended(chan)) {
    if (user->oper) {
      NoticeToUser(user, "Suspended by   : %s",
                   (chan->suspendby == NULL ? "???" : chan->suspendby->authname));
    }
    NoticeToUser(user, "Suspend reason : %s", chan->suspendreason);
  }
  NoticeToUser(user, "InviteOnly     : %s", IsInviteOnly(chan) ? "Yes" : "No");
  Log("Chanstat: %s (%s) requested chanstat for %s", user->nick, user->authedas->authname, channel);
}

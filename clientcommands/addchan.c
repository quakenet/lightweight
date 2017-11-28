/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: addchan.c,v 1.18 2004/08/30 23:18:46 froo Exp $
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
#include <usersdb.h>

/* addchan.c */

/* 
 * addchan: Adds a new channel to the bot 
 *
 * Parameters:
 *  #channelname  - obvious 
 *  owner         - Authnick of initial owner
 */

void doaddchan(struct user *user, char *tail)
{
  char *channel;
  char *owner;
  char *msguser;
  struct account *owneracct;
  struct account *msguseracct=0;
  struct reggedchannel *chan;

  if (!CheckAuthLevel(user, 240))
    return;

  /* Get the parameters */
  channel = tail;
  owner = SeperateWord(channel);
  msguser = SeperateWord(owner);
  SeperateWord(msguser);

  /* Check that we've got enough parameters and that they are sane */
  if (owner == NULL) {
    NoticeToUser(user, "Usage: addchan #channel owner [msguser]");
    NoticeToUser(user, "where  #channel is channel to add support to");
    NoticeToUser(user, "       owner is the original owner (either #account or nick)");
    NoticeToUser(user, "       [msguser] sends an optional \"Channel added\" to that user");
    return;
  }

  if (channel[0] != '#') {
    NoticeToUser(user, "The channel name must start with #.");
    return;
  }

  owneracct = FindAccount(owner);
  if (owneracct == NULL) {
    NoticeToUser(user, "I couldn't find that user.  Use either #authname or nick.");
    return;
  }

  if (msguser != NULL) {
    if ((msguseracct = FindAccount(msguser)) == NULL) {
      NoticeToUser(user, "I couldn't find the user to notify.  Use either #authname or nick.");
      return;
    }
    if (!(msguseracct->currentuser)) {
      NoticeToUser(user, "That user is not online, adding the channel anyway.");
      msguseracct=NULL;
    }
  }

  if (GetChannelPointer(channel) != NULL) {
    NoticeToUser(user, "That channel is already registered.");
    if (msguseracct != NULL)
      NoticeToUser(msguseracct->currentuser->autheduser,
        "The channel %s is already registered (Possibly suspended)", channel);
    return;
  }

  /* Do the actual work */
  if ((chan = AddChannel(channel, owneracct, user->authedas->authname, 0, user)) == NULL) {
    NoticeToUser(user, "Error adding channel.");
	if (msguseracct != NULL)
      NoticeToUser(msguseracct->currentuser->autheduser, "Error adding the channel %s", channel);
    return;
  }

  chan->dateadded = time(NULL);

  Log("AddChan: %s (%s) requested addchan %s with owner %s (%s)", user->nick, user->authedas->authname, channel, owner,
      owneracct->authname);

  NoticeToUser(user, "Channel added successfully.");

  if (msguseracct != NULL)
    NoticeToUser(msguseracct->currentuser->autheduser, "Channel %s was added successfully.", channel);
}

/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: access.c,v 1.2 2002/07/02 16:38:53 zarjazz Exp $
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
#include <dblist.h>
#include <usersdb.h>

/*
 * Access: Display a users modes on channels
 * 
 * Parameters:
 *  #channel   - Channel to check user on
 *  user       - User to show/change modes for
 *
 */

void doaccess(struct user *user, char *tail)
{
  struct reggedchannel *chanptr;
  char *channel;
  char *targetuser;
  char *dummy;

  struct user *tmp_uptr;
  struct account *tmp_aptr;

  channel = tail;
  targetuser = SeperateWord(tail);
  dummy = SeperateWord(targetuser);

  if (channel == NULL || targetuser == NULL) {
    NoticeToUser(user, "Usage: access #channel user");
    NoticeToUser(user, "where #channel is the channel to operate on");
    NoticeToUser(user, "      user     is the user to display");
    return;
  }

  /* Check if we know this channel. */
  if ((chanptr = GetChannelPointer(channel)) == NULL) {
    NoticeToUser(user, "Unknown channel %s.", channel);
    return;
  }

  /* Get a pointer to the account in question in tmp_aptr */
  if (*targetuser == '#') {
    tmp_aptr = GetAccountByAuthName(targetuser + sizeof(char));
  } else {
    tmp_uptr = FindUserByNick(targetuser);
    if (tmp_uptr->authedas != NULL) {
      tmp_aptr = tmp_uptr->authedas;
    } else {
      NoticeToUser(user, "User isnt authed and hence not known on any channels.");
      return;
    }
  }

  if (-1 == GetUserChannelIndex(chanptr, tmp_aptr)) {
    /* User isnt known on the channel. */
    NoticeToUser(user, "User %s isnt known on %s.", targetuser, channel);
  } else {
    NoticeToUser(user, "User %s is known on %s.", targetuser, channel);
    /* Perhaps say what flags the user has? */
  }

#ifdef DEBUG
  Log("Access: %s asked what access %s had on %s", user->nick, targetuser, channel);
#endif
}

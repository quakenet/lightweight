/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: unbanall.c,v 1.8 2003/04/15 15:36:00 splidge Exp $
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

/* unbanall.c */

/*
 * unbanall: remove all bans from a channel
 * 
 * Parameters:
 *  #channel   - Channel to remove all bans from
 *
 * Requires +o flag on channel.
 *
 * Note that we're using a nasty hack to effect this :) 
 */

void dounbanall(struct user *user, char *tail)
{
  unsigned char flags;
  struct reggedchannel *chanptr;
  char *channel;
  char buf[512];

  channel = tail;
  SeperateWord(tail);

  if (channel == NULL) {
    NoticeToUser(user, "Usage: unbanall #channel");
    return;
  }

  if ((chanptr = GetChannelPointer(channel)) == NULL) {
    NoticeToUser(user, "Unknown channel %s.", channel);
    return;
  }

  flags = GetChannelFlags(user->authedas, chanptr);

  /* Check for OP or MASTER or OWNER flag */
  if (!(flags & (CFLAG_OP | CFLAG_MASTER | CFLAG_OWNER)) || IsSuspended(chanptr)) {
    /* No flags -- perhaps they are an oper? */
    if (!((user->oper) && (user->authedas->authlevel > 200))) {
      /* Nope, not an oper either */
      NoticeToUser(user, "Sorry, you need the +o flag on %s to use unbanall.", channel);
      return;
    }
  }

  /* Do the actual unbanall */
  sprintf(buf, "%s CM %s b\r\n", my_numeric, channel);
  SendLine(buf);

  Log("UnbanAll: %s (%s) requested unbanall in %s", user->nick, user->authedas->authname, channel);
  NoticeToUser(user, "Done.");
}

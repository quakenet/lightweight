/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: op.c,v 1.8 2002/07/26 00:10:23 froo Exp $
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
 * op: Op the user on a channel
 * 
 * Parameters:
 *  #channel   - Channel to op user on
 *
 * Requires +o flag on channel.
 */

void doop(struct user *user, char *tail)
{
  unsigned char flags;
  struct reggedchannel *chanptr;
  char *channel;

  channel = tail;
  SeperateWord(tail);

  if (channel == NULL) {
    NoticeToUser(user, "Usage: op #channel");
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
      NoticeToUser(user, "Sorry, you need the +o flag on %s to get ops.", channel);
      return;
    }
  }

  /* Do the actual op */
  OpUser(chanptr, user->numeric);

  /* Update lastused timestamp on channel */
  chanptr->lastused = time(NULL);

  NoticeToUser(user, "Done.");
}

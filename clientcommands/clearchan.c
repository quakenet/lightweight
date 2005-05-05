/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: clearchan.c,v 1.11 2003/09/08 01:19:25 zarjazz Exp $
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
 * clearchan: remove all modes from a channel
 * 
 * Parameters:
 *  #channel   - Channel to remove all modes from
 *
 * Requires +o flag on channel.
 */

void doclearchan(struct user *user, char *tail)
{
  unsigned char flags;
  struct reggedchannel *chanptr;
  char *channel;
  char buf[512];

  channel = tail;
  SeperateWord(tail);

  if (channel == NULL) {
    NoticeToUser(user, "Usage: clearchan #channel");
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
      NoticeToUser(user, "Sorry, you need the +o flag on %s to use clearchan.", channel);
      return;
    }
  }

  /* Do the actual clearchan */

#ifdef SIT_ON_CHANNELS
  CheckJoined(chanptr);
  sprintf(buf, "%sAAA M %s -rilkm *\r\n", my_numeric, channel);
#else
  sprintf(buf, "%s M %s -iklm *\r\n", my_numeric, channel);
#endif

  SendLine(buf);

  Log("ClearChan: %s (%s) requested clearchan for %s", user->nick, user->authedas->authname, channel);
  NoticeToUser(user, "Done.");
}

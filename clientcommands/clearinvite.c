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

#ifdef SIT_ON_CHANNELS

/*
 * setinvite: make a channel invite only
 * 
 * Parameters:
 *  #channel   - Channel to make invite only
 *
 * Requires +m flag on channel and SIT_ON_CHANNELS defined
 * at build time.
 */

void doclearinvite(struct user *user, char *tail)
{
  unsigned char flags;
  struct reggedchannel *chanptr;
  char *channel;
  char buf[513];

  channel = tail;
  SeperateWord(tail);

  if (channel == NULL) {
    NoticeToUser(user, "Usage: clearinvite #channel");
    return;
  }

  if ((chanptr = GetChannelPointer(channel)) == NULL) {
    NoticeToUser(user, "Unknown channel %s.", channel);
    return;
  }

  flags = GetChannelFlags(user->authedas, chanptr);

  /* Check for MASTER or OWNER flag */
  if (!(flags & (CFLAG_MASTER | CFLAG_OWNER)) || IsSuspended(chanptr)) {
    /* No flags -- perhaps they are an oper? */
    if (!((user->oper) && (user->authedas->authlevel > 200))) {
      /* Nope, not an oper either */
      NoticeToUser(user, "Sorry, you need the +m flag on %s to use clearinvite.", channel);
      return;
    }
  }

  /* Do the actual setinvite */
  ClearInviteOnly(chanptr);
  sprintf(buf, "%sAAA M %s -i\r\n", my_numeric, chanptr->channelname);
  SendLine(buf);
  NoticeToUser(user, "Done.");
}

#endif /* SIT_ON_CHANNELS */

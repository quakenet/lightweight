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

/*
 * recover: Do all neccesary steps to recover a channel, ie
 *          deopall, unbanall and clearchan.
 * 
 * Parameters:
 *  #channel   - Channel to recover
 */

void dorecover(struct user *user, char *tail)
{
  char *channel;
  unsigned char flags;
  struct reggedchannel *chanptr;

  channel = tail;
  SeperateWord(tail);

  if (channel == NULL) {
    NoticeToUser(user, "Usage: recover #channel");
    NoticeToUser(user, "where #channel is the channel to recover");
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
      NoticeToUser(user, "Sorry, you do not have permission to recover %s.", channel);
      return;
    }
  }

  Log("Recover: %s tried to recover %s", user->nick, channel);
  dodeopall(user, channel);
  dounbanall(user, channel);
  doclearchan(user, channel);
}

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
 * removeuser: Remove user from a channel
 * 
 * Parameters:
 *  #channel   - Channel to remove user from
 *  user       - User to nuke
 */

void doremoveuser(struct user *user, char *tail)
{
  unsigned char flags;
  struct reggedchannel *chanptr;
  char *channel;
  char *targetuser;
  char *modes;

  channel = tail;
  targetuser = SeperateWord(tail);

  if (channel == NULL) {
    NoticeToUser(user, "Usage: removeuser #channel user");
    NoticeToUser(user, "where #channel is the channel to operate on");
    NoticeToUser(user, "      user     is the user to remove");
    return;
  }

  if ((chanptr = GetChannelPointer(channel)) == NULL) {
    NoticeToUser(user, "Unknown channel %s.", channel);
    return;
  }

  flags = GetChannelFlags(user->authedas, chanptr);

  /* Check for any flag */
  if (flags == 0 || IsSuspended(chanptr)) {
    /* No flags -- perhaps they are an oper? */
    if (!((user->oper) && (user->authedas->authlevel > 200))) {
      /* Nope, not an oper either */
      NoticeToUser(user, "Sorry, you do not have permission to remove a user on %s.", channel);
      return;
    }
  }

  modes = SeperateWord(targetuser);
  if (targetuser == NULL) {
    NoticeToUser(user, "You will need to specify a user to be removed.");
    return;
  }

  Log("Removeuser: %s (%s) tried to remove user %s on %s", user->nick, user->authedas->authname, targetuser, channel);

  ChangeChanlev(user, channel, targetuser, "-amnovg");
}

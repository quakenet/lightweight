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
#include <usersdb.h>
#include <accountsdb.h>
#include <channelsdb.h>

/* cleanupdb.c */

/*
 * cleanupdb: removes unused (no authed join for DAYS_BEFORE_EXPIRE days) and
 * and empty channels (ie: empty chanlev) unless they are suspended.
 * 
 * Parameters: 
 *  nick Nick of the slave service
 *
 * Operator only.
 */

void docleanupdb2(struct user *usr_ptr, char *tail)
{
  char *option;
  struct user *target;
  int i;
  struct reggedchannel *chan;

  if (!usr_ptr->oper) {
    NoticeToUser(usr_ptr, "You are not an operator");
    return;
  }

  if (!CheckAuthLevel(usr_ptr, 255))
    return;

  option = tail;
  SeperateWord(tail);
  if (!option) {
    NoticeToUser(usr_ptr, "Usage: cleanupdb2 nick");
    return;
  }

  if ((target = FindUserByNick(option)) == NULL) {
    NoticeToUser(usr_ptr, "Can't find nick: %s", option);
  }

  Log("Cleanupdb2: %s (%s) requested cleanupdb2 %s", usr_ptr->nick, usr_ptr->authedas->authname, option);

  for (i = 0; i < HASHMAX; i++) {
    for (chan = channelhashtable[i]; chan; chan = chan->nextbychannelname) {
      if (!IsSuspended(chan)) {
        MessageToUser(target, "expirecheck %s", chan->channelname);
      }
    }
  }

  NoticeToUser(usr_ptr, "Done.");
}

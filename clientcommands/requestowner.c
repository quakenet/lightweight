/*
    requestowner.c - port of Q's requestowner function
    - skx	<skx@quakenet.org>	06/08/2004

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

#define RO_OWNEREXISTS	0x1	/* there is an owner for that channel */
#define RO_MASTEREXISTS	0x2	/* there is a master for that channel */
#define RO_NOMASTER	0x3	/* there is no master for that channel */

/*
 * requestowner: Give a master or op the owner chanlev
 *               on a channel without any owners.
 * Parameters:
 *  #channel    - Channel
 */

void dorequestowner(struct user *user, char *tail) {
  char *channel;
  unsigned char flags,status,uflags;
  struct reggedchannel *chanptr;
  int i;

  channel=tail;
  SeperateWord(tail);

  if (channel == NULL) {
    NoticeToUser(user,"Usage: requestowner #channel");
    return;
  }


  if ((chanptr = GetChannelPointer(channel)) == NULL) {
    NoticeToUser(user,"Unknown channel %s.",channel);
    return;
  }

  flags = GetChannelFlags(user->authedas, chanptr);

  if (flags & CFLAG_OWNER) {
    NoticeToUser(user,"You are already owner of %s.",channel);
    return;
  }

  if (!(flags & (CFLAG_MASTER|CFLAG_OP)) || IsSuspended(chanptr)) {
    /* User doesn't seem to be master or op */
    NoticeToUser(user,"You must be a channel master or op on %s in order to request ownership.",
                 channel);
    return;
  }

  status=RO_NOMASTER;

  for(i = 0; i < USERSPERCHANNEL; i++) {
    if (NULL != chanptr->channeluser[i]) {
      uflags=chanptr->channeluser[i]->channelflags[GetUserChannelIndex(chanptr, chanptr->channeluser[i])];

      if (uflags & CFLAG_OWNER) {
        status=RO_OWNEREXISTS;
        break;
      } else if (uflags & CFLAG_MASTER) {
        status=RO_MASTEREXISTS;
        if (!(flags & CFLAG_MASTER))
          break;
      } 
    }
  }

  switch(status) {
    case RO_OWNEREXISTS:
      NoticeToUser(user,"There is an owner for %s.",channel);
      return;
    case RO_MASTEREXISTS:
      if (!(flags & CFLAG_MASTER)) {
        NoticeToUser(user,"There is a master present on %s, due to this you need the +m CHANLEV to request ownership.",channel);
        return;
      }
    break;
  }

  SetUserChannelFlags(user->authedas, chanptr, "+n");
  NoticeToUser(user, "You have been given ownership of %s.",channel);
  Log("Requestowner: %s (%s) was given ownership of %s",user->nick,user->authedas->authname,channel);
}

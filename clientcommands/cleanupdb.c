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
#include <channels.h>

/* cleanupdb.c */

/*
 * cleanupdb: removes unused (no authed join for DAYS_BEFORE_EXPIRE days) and
 * and empty channels (ie: empty chanlev) unless they are suspended.
 * 
 * Parameters:
 *  -n  Don't execute, just show what would be done.
 *
 * Operator only.
 */

#define CHAN_OK                  0
#define CHAN_EXPIRED             1
#define CHAN_EMPTY               2
#define CHAN_OWNER               3
#define CHAN_SUSPENDED_EXPIRED  98
#define CHAN_SUSPENDED          99

static int preview, verbose;

int ExpireChannel(struct reggedchannel *chan, struct user *usr_ptr, int preview)
{
  int i, empty = 1;
  char buf[512];
  unsigned char currentflags;
  time_t now;

  now = time(NULL);
  i = (now - chan->lastused) / (3600 * 24);  

  if (IsSuspended(chan)) {      
    if (now - chan->lastused > (SUSPEND_EXPIRY_TIME * 3600 * 24)) {
      if (verbose)
        NoticeToUser(usr_ptr, "%s has been deleted. (suspended for %d days)", chan->channelname, i);
      if (!preview) {
        Log("Cleanupdb: channel %s removed (suspended for > %d days)", chan->channelname, i);
        sprintf(buf, "Channel removed, suspended for > %d days.", i);
        PartChannel(chan, buf);
        RemoveChannel(chan);
      }
      return (CHAN_SUSPENDED_EXPIRED);
    }
    return (CHAN_SUSPENDED);
  }

  if (chan->lastused && ((now - DAYS_BEFORE_EXPIRE * 3600 * 24) > chan->lastused)) {
    if (verbose)
      NoticeToUser(usr_ptr, "%s has expired. (unused for %d days)", chan->channelname, i);
    if (!preview) {
      Log("Cleanupdb: channel %s removed (expired: unused for %d days)", chan->channelname, i);
      sprintf(buf, "Channel expired, unused for %d days.", i);
      PartChannel(chan, buf);
      RemoveChannel(chan);
    }
    return (CHAN_EXPIRED);
  }

  /* remove empty channels and non-empty channels w/o any owner/master or op (+n/+m/+o) */
  for (i = 0; i < USERSPERCHANNEL; i++) {
    if (NULL != chan->channeluser[i]) {
      empty = 0;
      currentflags = chan->channeluser[i]->channelflags[GetUserChannelIndex(chan, chan->channeluser[i])];
      if (currentflags & CFLAG_OWNER || currentflags & CFLAG_MASTER || currentflags & CFLAG_OP) {
        return (CHAN_OK);
      }
    }
  }

  if (verbose)
    NoticeToUser(usr_ptr, "%s %s.", chan->channelname, (empty ? "is empty" : "has no owners, masters or operators"));
  if (!preview) {
    Log("Cleanupdb: channel %s removed (%s)", chan->channelname, (empty ? "empty" : "no owners, masters or operators"));
    sprintf(buf, "Channel %s.", (empty ? "is empty" : "has no owners, masters or operators"));
    PartChannel(chan, buf);
    RemoveChannel(chan);
  }

  return (empty ? CHAN_EMPTY : CHAN_OWNER);
}

void docleanupdb(struct user *usr_ptr, char *tail)
{
  char *option;
  int i;
  struct reggedchannel *chan;
  unsigned int chan_ok, chan_expired, chan_empty, chan_owner, chan_suspended,
			   chan_suspended_expired, chan_unknown, chan_total;

  if (!usr_ptr->oper) {
    NoticeToUser(usr_ptr, "You are not an operator");
    return;
  }

  if (!CheckAuthLevel(usr_ptr, 255))
    return;

  preview = verbose = 0;

  option = tail;
  SeperateWord(tail);
  if (option != NULL) {
    while (*option) {
      switch(*option) {
			case 'v':
              verbose++;
              break;
			case 'n':
              preview++;
              break;
			case '-':
              break;
            default:
              NoticeToUser(usr_ptr, "Usage: cleanupdb [-nv]");
              return;
      }
      *option++;
    }
  }

  Log("Cleanupdb: %s (%s) requested cleanupdb%s", usr_ptr->nick, usr_ptr->authedas->authname,
      (preview ? " -n (preview mode)." : "."));

  chan_ok = chan_expired = chan_empty = chan_owner = chan_suspended = chan_suspended_expired = chan_unknown = 0;
  for (i = 0; i < HASHMAX; i++) {
    chan = channelhashtable[i];
    while (NULL != chan) {      /* go through the list */
      switch (ExpireChannel(chan, usr_ptr, preview)) {
      case CHAN_OK:
        chan_ok++;
        break;
      case CHAN_EXPIRED:
        chan_expired++;
        break;
      case CHAN_EMPTY:
        chan_empty++;
        break;
      case CHAN_OWNER:
        chan_owner++;
        break;
      case CHAN_SUSPENDED:
        chan_suspended++;
        break;
      case CHAN_SUSPENDED_EXPIRED:
        chan_suspended_expired++;
        break;
      default:
        chan_unknown++;
        break;
      }
      chan = chan->nextbychannelname; /* next in list */
    }
  }

  chan_total = chan_expired + chan_empty + chan_owner + chan_suspended + chan_unknown + chan_ok + chan_suspended_expired;
  NoticeToUser(usr_ptr, "%d channel/s %s (unused for >= %d days)", chan_expired,
               (preview) ? "would be removed" : "removed", DAYS_BEFORE_EXPIRE);
  NoticeToUser(usr_ptr, "%d channel/s %s (empty)", chan_empty, (preview) ? "would be removed" : "removed");
  NoticeToUser(usr_ptr, "%d channel/s %s (no owners, masters or operators)", chan_owner, (preview) ? "would be removed" : "removed");
  if (chan_unknown)
    NoticeToUser(usr_ptr, "%d channel/s (unknown)", chan_unknown);
  NoticeToUser(usr_ptr, "%d channel/s suspended (untouched)", chan_suspended);
  NoticeToUser(usr_ptr, "%d channel/s suspended (cleaned)", chan_suspended_expired);
  NoticeToUser(usr_ptr, "%d channel/s still valid (was %d channel/s)",
               (chan_total - (chan_expired + chan_empty + chan_owner + chan_suspended_expired)), chan_total);

  if (!preview)
    Log("Cleanupdb: %s (%s) deleted %d channel/s (%d expired, %d empty, %d no owner/master/op) of %d channel/s", usr_ptr->nick,
        usr_ptr->authedas->authname, (chan_expired + chan_empty + chan_owner), chan_expired, chan_empty, chan_owner,
        chan_total);

  NoticeToUser(usr_ptr, "Done.");
}

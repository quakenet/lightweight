/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: chanlev.c,v 1.13 2003/09/08 01:19:25 zarjazz Exp $
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

/*
 * chanlev: Display or change user modes on channels
 * 
 * Parameters:
 *  #channel   - Channel to op user on
 *  user       - [Optional] User to show/change modes for
 *  changes    - [Optional] Flag changes to make
 *
 * Showing modes requires the user be known on that channel
 * Changing +aogv modes requires the user have +m
 * Changing +nm modes requires the user have +n
 */

void dochanlev(struct user *user, char *tail)
{
  unsigned char flags;
  struct reggedchannel *chanptr;
  char *channel;
  char *targetuser;
  char *modes;

  channel = tail;
  targetuser = SeperateWord(tail);

  if (channel == NULL) {
    NoticeToUser(user, "Usage: chanlev #channel [user] [modes]");
    NoticeToUser(user, "where #channel is the channel to operate on");
    NoticeToUser(user, "      user     is the user to display/change");
    NoticeToUser(user, "               (omit this to show all modes on channel)");
    NoticeToUser(user, "      modes    is the mode changes to make");
    NoticeToUser(user, "               (omit this to just show current modes)");
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
    if (user->authedas->authlevel < 10) {
      /* Nope, not an oper either */
      NoticeToUser(user, "Sorry, you do not have permission to view/change user flags on %s.", channel);
      return;
    }
  }

  modes = SeperateWord(targetuser);
  /* Check if they specified a user.  If not, it's just a complete listing */
  if (targetuser == NULL || '\0' == *targetuser) {
    /* General chanlev listing */
    PrintChanlev(user, channel, NULL);
    return;
  }

  SeperateWord(modes);
  if (modes == NULL || '\0' == *modes) {
    /* No change, just display */
    PrintChanlev(user, channel, targetuser);
    return;
  }

  /* OK, we got all three parameters */
#ifdef DEBUG
  Log("ChangeChanlev: %s tried to change flags (%s) for %s on %s", user->nick, modes, targetuser, channel);
#endif
  ChangeChanlev(user, channel, targetuser, modes);
}

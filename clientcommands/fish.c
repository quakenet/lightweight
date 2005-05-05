/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: fish.c,v 1.5 2003/09/08 01:19:25 zarjazz Exp $
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
 * fish: the famed fish slapping from Q
 * 
 * Parameters:
 *  #channel   - Channel (or client numeric) to slap a fish at
 *  user       - user to slap (optional)
 *
 * Requires oper powah
 */

void dofish(struct user *user, char *tail)
{
  char *channel;
  char *victim;
  char buf[512];
  int buflen;

  channel = tail;
  victim = SeperateWord(tail);
  SeperateWord(victim);

  if (channel == NULL) {
    NoticeToUser(user, "Usage: fish #channel [user]");
    return;
  }

  buflen =
    snprintf(buf, 509, "%sAAA P %s :\001ACTION slaps %s with a fish\001", my_numeric, channel,
             (victim) ? victim : channel);

  if (buflen > 509 || buflen < 0)
    buflen = 509;

  sprintf(buf + buflen, "\r\n");

  SendLine(buf);

  NoticeToUser(user, "Done.");
}

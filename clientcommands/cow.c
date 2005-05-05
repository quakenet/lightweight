/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: cow.c,v 1.5 2002/04/29 15:11:01 froo Exp $
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
 * cow: Sends a cow to the channel 
 * 
 * Parameters:
 *  #channel   - Channel to put the cow in 
 *
 * Requires oper powah
 */

void docow(struct user *user, char *tail)
{
  char *channel;
  char buf[512];

  channel = tail;
  SeperateWord(tail);

  if (channel == NULL) {
    NoticeToUser(user, "Usage: cow #channel");
    return;
  }

  sprintf(buf, "%sAAA P %s :(__)\r\n", my_numeric, channel);
  SendLine(buf);
  sprintf(buf, "%sAAA P %s : oo\\\\\\~\r\n", my_numeric, channel);
  SendLine(buf);
  sprintf(buf, "%sAAA P %s :  !!!!\r\n", my_numeric, channel);
  SendLine(buf);

  NoticeToUser(user, "Done.");
}

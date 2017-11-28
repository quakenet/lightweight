/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
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
 * welcome: display or set the channel welcome message
 *
 * Parameters:
 *  #channelname  - obvious 
 *  message       - new welcome message
 */

void dowelcome(struct user *usr_ptr, char *tail)
{
  char *channel;
  char *message;
  char *tmp;
  char chanflags;
  struct reggedchannel *chn_ptr;

  /* Get the parameters */
  channel = tail;
  message = SeperateWord(channel);

  /* No SeperateWord on reason -- we want the rest of this line for the reason */

  /* Check that we've got enough parameters and that they are sane */
  if (channel == NULL) {
    NoticeToUser(usr_ptr, "Usage: welcome #channel message");
    NoticeToUser(usr_ptr, "where  #channel is channel to set or view the welcome message");
    NoticeToUser(usr_ptr, "       message is the new welcome message (requires +n/+m for channel)");
    return;
  }

  if (channel[0] != '#') {
    NoticeToUser(usr_ptr, "The channel name must start with #.");
    return;
  }

  if ((chn_ptr = GetChannelPointer(channel)) == NULL) {
    NoticeToUser(usr_ptr, "Couldn't find that channel.");
    return;
  }

  chanflags = GetChannelFlags(usr_ptr->authedas, chn_ptr);

  if ((0 == chanflags) && !IsAdmin(usr_ptr)) {
    NoticeToUser(usr_ptr, "You're not known on channel %s", channel);
    return;
  }

  if (NULL == message) {
    if (NULL == chn_ptr->welcome)
      NoticeToUser(usr_ptr, "No welcome message is set for channel %s", channel);
    else
      NoticeToUser(usr_ptr, "Welcome message for channel %s is: %s", channel, chn_ptr->welcome);
    return;
  }

  if (!(CFLAG_OWNER & chanflags) && !(CFLAG_MASTER & chanflags) && !IsAdmin(usr_ptr)) {
    if (NULL == chn_ptr->welcome)
      NoticeToUser(usr_ptr, "No welcome message is set for channel %s", channel);
    else
      NoticeToUser(usr_ptr, "Welcome message for channel %s is: %s", channel, chn_ptr->welcome);
    return;
  }

  if (250 < strlen(message)) {
    NoticeToUser(usr_ptr, "The welcome message must be shorter than 250 characters.");
    return;
  }

  for (tmp = message; *tmp; tmp++)
    if (*tmp == '\r' || *tmp == '\n')
      *tmp = '\0';

  if (!strcmp("remove", message)) {
    if (NULL != chn_ptr->welcome)
      free(chn_ptr->welcome);
    chn_ptr->welcome = NULL;
    ClearWelcomeFlag(chn_ptr);
    NoticeToUser(usr_ptr, "Done.");
    return;
  }

  if (chn_ptr->welcome != NULL)
    free(chn_ptr->welcome);

  chn_ptr->welcome = strdup(message);
  SetWelcomeFlag(chn_ptr);

  NoticeToUser(usr_ptr, "Done.");
}

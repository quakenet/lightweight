/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: sendchanlev.c,v 1.1 2003/11/29 14:45:48 splidge Exp $
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
#include <usersdb.h>

/* 
 * sendchanlev: Sends a chanlev list to the specified user
 *
 * Parameters:
 *  #channelname - Channel to operate on
 *  user         - Target for chanlev commands
 */

void dosendchanlev(struct user *user, char *tail)
{
  char *channel, *target;
  struct reggedchannel *channel_ptr;
  struct user *targetuser;
  char flagbuf[10], *ch;
  int i;
  
  if (!CheckAuthLevel(user, 240))
    return;

  /* Get the parameters */
  channel = tail;
  target=SeperateWord(channel);	
  SeperateWord(target);

  /* Check that we've got enough parameters and that they are sane */
  if (channel == NULL || target == NULL) {
    NoticeToUser(user, "Usage: sendchanlev #channel target");
    NoticeToUser(user, "where  #channel is channel to send chanlev for");
    NoticeToUser(user, "       target   is user to send chanlev to");
    return;
  }

  if (channel[0] != '#') {
    NoticeToUser(user, "The channelname must start with #.");
    return;
  }

  if (!(channel_ptr = GetChannelPointer(channel))) {
    NoticeToUser(user, "Can't find that channel.");
    return;
  }

  if (!(targetuser = FindUserByNick(target))) {
    NoticeToUser(user, "Can't find that user.");
    return;
  }

  /* OK, we have all the info we need.. */
  for (i=0;i<USERSPERCHANNEL;i++) {
    if (channel_ptr->channeluser[i]) {
      /* Unfortunately PrintCFlags is RETARDED and
       * RIGHT JUSTIFIES its output without adding a
       * TERMINATING NUL.  It's all Steve's fault :/ */
      PrintCFlags(flagbuf, 
                  channel_ptr->channeluser[i]->channelflags[GetUserChannelIndex(channel_ptr, channel_ptr->channeluser[i])]);
      flagbuf[8]='\0'; /* 8 == magic number */
      for (ch=flagbuf;*ch==' ';ch++)
        ; /* Empty loop */
        
      MessageToUser(targetuser, "CHANLEV %s #%s +%s",channel,channel_ptr->channeluser[i]->authname,ch);
    }
  }
 
  Log("Sendchanlev: %s (%s) requested sendchanlev %s %s", user->nick, user->authedas->authname, channel, target);
  NoticeToUser(user, "Sent chanlev for %s to %s OK.",channel,target);
}

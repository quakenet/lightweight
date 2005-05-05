/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: help.c,v 1.35 2004/08/18 18:15:06 froo Exp $
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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
/* The function in here should be called when we recieve a HELP request in a
 * PRIVMSG from a user.
 */

void ListCommands(struct user *usr_ptr)
{
  NoticeToUser(usr_ptr, "Following commands are available to you");
  NoticeToUser(usr_ptr, "See help <command name> for more information");
  NoticeToUser(usr_ptr, "------------------------------------------------");
  NoticeToUser(usr_ptr, "whoami       Tells you your user information");
  NoticeToUser(usr_ptr, "whois        Tells you who someone else is");
  NoticeToUser(usr_ptr, "chanlev      Lists and sets channel flags");
  NoticeToUser(usr_ptr, "adduser      Add a user to a channel");
  NoticeToUser(usr_ptr, "removeuser   Remove a user from a channel");
  NoticeToUser(usr_ptr, "showcommands Lists commands available to you");
  NoticeToUser(usr_ptr, "op           Gives you mode +o for channel");
  NoticeToUser(usr_ptr, "voice        Gives you mode +v for channel");
#ifdef SIT_ON_CHANNELS
  NoticeToUser(usr_ptr, "invite       Invites you to a channel");
  NoticeToUser(usr_ptr, "setinvite    Makes L enforce +i on a channel");
  NoticeToUser(usr_ptr, "clearinvite  Stops L enforcing +i on a channel");
#endif /* SIT_ON_CHANNELS */
  NoticeToUser(usr_ptr, "recover      Recover a channel (deopall/unbanall/clearchan)");
  NoticeToUser(usr_ptr, "deopall      Deops every user on channel");
  NoticeToUser(usr_ptr, "unbanall     Removes all bans from channel");
  NoticeToUser(usr_ptr, "clearchan    Clears all channel modes");
  NoticeToUser(usr_ptr, "version      Tells the current L version");
  NoticeToUser(usr_ptr, "welcome      Channel welcome message");
  NoticeToUser(usr_ptr, "requestowner Request ownership of a channel on which there are no owners.");
  if (10 > usr_ptr->authedas->authlevel)
    return;
  NoticeToUser(usr_ptr, "suspendlist  Lists suspended channels");
  if (!IsAdmin(usr_ptr))
    return;
  NoticeToUser(usr_ptr, "changelev    Changes someones authlevel");
  NoticeToUser(usr_ptr, "addchan      Adds a channel to the service");
  NoticeToUser(usr_ptr, "delchan      Removes a channel from the service");
  NoticeToUser(usr_ptr, "deluser      Removes a user");
  NoticeToUser(usr_ptr, "save         Saves the DB to disk");
  NoticeToUser(usr_ptr, "suspend      Suspends a channel");
  NoticeToUser(usr_ptr, "unsuspend    Unsuspends a channel");
  NoticeToUser(usr_ptr, "channellist  Lists registered channels");
  NoticeToUser(usr_ptr, "accountlist  Lists user accounts");
  NoticeToUser(usr_ptr, "listlevel    List all users with an authlevel of \"authlevel\"");
  NoticeToUser(usr_ptr, "userlist     Lists online users");
  NoticeToUser(usr_ptr, "chanstat     Displays information about a channel");
  NoticeToUser(usr_ptr, "grep         Search the service logfiles");
  NoticeToUser(usr_ptr, "status       Tells the service status");
  NoticeToUser(usr_ptr, "noticeme     Sets if the service sends you its replys as /msg or as /notice");
  NoticeToUser(usr_ptr, "reauth       Tells the service to reauth with %s", o_nick);
  if (255 > usr_ptr->authedas->authlevel)
    return;
  NoticeToUser(usr_ptr, "setenable    Enables or disables all user commands");
  NoticeToUser(usr_ptr, "cleanupdb    Cleanup the channel database");
  NoticeToUser(usr_ptr, "raw          Makes the service issue a raw command");
  NoticeToUser(usr_ptr, "die          Save and exit");
}

void dohelp(struct user *usr_ptr, char *adtinfo)
{
  if (NULL == adtinfo) {
    ListCommands(usr_ptr);
    return;
  }
  if ('\0' == *adtinfo) {
    ListCommands(usr_ptr);
    return;
  }

  {
    FILE *helpfile;
    char tmp_s[300], fname[32];
    char *nextchar;
    int s_len;

    if (15 < strlen(adtinfo)) { /* no command is longer than 15 characters */
      NoticeToUser(usr_ptr, "Sorry, no help available for '%s'", adtinfo);
      return;
    }

    if (strchr(adtinfo, '/') || strchr(adtinfo, '\\') || strchr(adtinfo, ':')
        || strchr(adtinfo, '.')) {
      NoticeToUser(usr_ptr, "Sorry, no help available for '%s'", adtinfo);
      return;
    }

    nextchar = adtinfo;
    while (*nextchar != '\0') {
      /* Be paranoid and filter out everything but a-z and A-Z. */
      if ((*nextchar < 'a' || *nextchar > 'z') && (*nextchar < 'A' || *nextchar > 'Z')) {
        NoticeToUser(usr_ptr, "Sorry, no help available for '%s'", adtinfo);
        return;
      }
      *nextchar=tolower(*nextchar);
      nextchar++;
    }

    sprintf(fname, "help/%s.%s", adtinfo, IsAdmin(usr_ptr) ? "oper" : "user");  /* oper or user help */
    helpfile = fopen(fname, "r");

    if (NULL == helpfile) {
      NoticeToUser(usr_ptr, "Sorry, no help available for '%s'", adtinfo);
      return;
    }
    while (!feof(helpfile)) {
      fgets(tmp_s, sizeof(tmp_s), helpfile);
      if (feof(helpfile))
        break;
      s_len = strlen(tmp_s);
      if (2 > s_len)
        break;                  /* a line with one character ? no thank you */
      if ('\r' == tmp_s[s_len - 2]) /* DOS format */
        tmp_s[s_len - 2] = '\0';
      else if ('\n' == tmp_s[s_len - 1])  /* UNIX format */
        tmp_s[s_len - 1] = '\0';
      NoticeToUser(usr_ptr, "%s", tmp_s);
    }
    fclose(helpfile);
  }
}

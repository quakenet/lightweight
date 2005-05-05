/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: parseline.c,v 1.23 2003/09/08 01:19:25 zarjazz Exp $
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
#include <usersdb.h>
#include <channelsdb.h>
#include <channels.h>

int ParseLine(void)
{

  char *sender;
  char *command;
  char *tail;

  /* Make sure we dont have any spaces in front. */
  sender = StripBlanks(currentline);

  /* Given that every single line is passed through here
   * I don't see that null lines are a problem.  I'm 
   * 99% sure they are simply caused by a chunk ending
   * between the \r and \n from the server.
   * 
   * The easiest way to deal with this is simply to bounce
   * them back here silently.         - splidge
   */
  if (*sender == '\0')
    return (0);

/*  fprintf(stderr,"<< %s\n",sender); */

  /* Get the pointer to the next word (the command) and make the sender token zero terminated. */
  command = SeperateWord(sender);

  /* Get a pointer to the tail and zero terminate the command. */
  tail = SeperateWord(command);

  /* Switch over the commands we give a fuck about and fuck the rest. */

  if (command[0] == 'B' && command[1] == '\0') {
    /* Grab the timestamp from the burst */
    /* a9 B #bar 1016451591 a9AAD:ov,a8AAE:o,a9AAK */
    char *channel, *timestr, *modestr;
    struct reggedchannel *chn_ptr;
    time_t newtimestamp;
    char buf[512];

    channel = tail;
    timestr = SeperateWord(channel);
    modestr = SeperateWord(timestr);
    SeperateWord(modestr);

    chn_ptr = GetChannelPointer(channel);

    if (chn_ptr != NULL) {
      newtimestamp = strtol(timestr, NULL, 10);

      SyncTimestamp(chn_ptr, newtimestamp);

      if (IsInviteOnly(chn_ptr) && (!modestr || !strchr(modestr, 'i'))) {
        /* Channel is bursted without +i but is invite only. Send +i mode. */
        sprintf(buf, "%sAAA M %s +i\r\n", my_numeric, chn_ptr->channelname);
        SendLine(buf);
      }
    }
  } else if (command[0] == 'N' && command[1] == '\0') {
    /* Nick command recieved. */
    /* Branch on Nick introduction and Nick change. */
    /* Introdution: Create a new user in the userdb. */
    /* Change: Update the userdb. */

    if (sender[2] == '\0') {
      /* Came from server: it's a new user */
      /* AD N Q 3 000000100 TheQBot CServe.quakenet.org +oiwdk B]AAAB ADAAA :The Q Bot */

      int isoper = 0;
      char *nick = tail;
      char *numeric;
      char *tempstr;
      char *account = NULL;
      int i;

      for (i = 0; i < 5; i++)
        tail = SeperateWord(tail);

      if (*tail == '+') {
        /* There are usermodes for this user. 
         * We are interested in three things: 
         *  - We now need to advance an extra word down the sentence 
         *  - Check if the usermodes include +o (oper) 
         *  - Check if the usermodes include +r (registered nick) 
         * (and do appropriate things with this information 
         * After this block executes, "tail" will point at the IP address field 
         * (if there were no modes this was true already) */

        tempstr = tail;         /* tempstr = list of user modes */
        tail = SeperateWord(tail);
        if (IsCharInString('r', tempstr)) {
          /* Found +r usermode -- that means the user is already authed */
          /* tail is now pointing at the auth name, so grab that and advance another word */
          account = tail;
          tail = SeperateWord(tail);
        }
        if (IsCharInString('h', tempstr)) {
          /* Found +h usermode - just skip the parameter */
          tail = SeperateWord(tail);
        }
        isoper = IsCharInString('o', tempstr);
      }

      tail = SeperateWord(tail);
      numeric = tail;
      SeperateWord(tail);

      UserNickCreation(numeric, nick, isoper);

      if (account != NULL) {
        /* Account was authed: so call the auth routine now... */
        AuthUser(numeric, account);
      }
    } else {
      /* Rename */
      /* [hAAA N splidge_ 1013972401 */
      char *newnick = tail;

      tail = SeperateWord(tail);
      UserNickChange(sender, newnick);
    }

  } else if (command[0] == 'A' && command[1] == 'C' && command[2] == '\0') {
    /* ACCOUNT command received. */
    /* Auth the user */
    /* AD AC [hAAA splidge */

    char *numeric, *account;

    numeric = tail;
    account = SeperateWord(tail);
    tail = SeperateWord(tail);

    AuthUser(numeric, account);
  } else if (command[0] == 'Q' && command[1] == '\0') {
    /* Quit command recieved. */
    /* Remove user from userdb. */
    /* Thank you for flying QuakeNet. */
    /* [hAAA Q :m00 */

    DeleteUser(sender);
  } else if (command[0] == 'D' && command[1] == '\0') {
    /* Kill command received. */
    /* Much like Quit, this one */
    /* a9AAD D bcAAQ :frogsquad.netsplit.net!splidge (m00) */

    char *victim = tail;

    tail = SeperateWord(tail);

    DeleteUser(victim);
  } else if (command[0] == 'J' && command[1] == '\0') {
    /* Join command recieved. */
    /* Check if user is authed or if channel is known, whichever is fastest. */
    /* If authed or if channel is known, check if user has flags on channel, and op/voice the person in that case. */

    /* The only stuff in the tail is the list of channels to join, or 0 */
    /* Luckily, ircu seems to deal with most of the "JOIN 0" nastiness for us */

    /* a9AAD J #foo,#bar */
    /* a9AAD J 0 */

    /* Seperate off the second parameter (if any) */
    SeperateWord(tail);

    DoJoins(sender, tail, 0);   /* Last parameter = 0 => this is NOT a CREATE */
  } else if (command[0] == 'C' && command[1] == '\0') {
    /* Create command recieved. */
    /* Check if user is authed or if channel is known, whichever is fastest. */
    /* If authed or if channel is known, check if user has flags on channel, and op/voice the person in that case. Deop in the opposite case. */

    char *chanlist = tail;
    char *timestr;

    timestr = SeperateWord(tail);
    SeperateWord(timestr);

    DoJoins(sender, chanlist, strtol(timestr, NULL, 10)); /* Last parameter = timestamp */
  } else if (command[0] == 'M' && command[1] == '\0') {
    /* mode change */
    /* [hAAA M splidge[wind] :+k */
    /* [hAAA M #twilightzone -o [hAAA */

    int dir = 1;
    char *target;
    char *modes;
    char *cp;
    struct user *usrptr;

    target = tail;
    modes = SeperateWord(target);

    if (target[0] != '#') {     /* Not a channel (you don't get mode changes on + channels... */
      /* This must be a user mode change.  Make sure it all checks out */
      /* Assuming it does, we're only interested in users who are opering or deopering */

      if ((usrptr = FindUserByNumeric(sender)) == NULL) {
        Error(ERR_PROTOCOL | ERR_WARNING, "Mode change from non-existent user %s", sender);
        return 0;
      }

      if (Strcmp(usrptr->nick, target)) {
        Error(ERR_PROTOCOL | ERR_WARNING, "Mode change for other user (%s) from %s", target, usrptr->nick);
        return 0;
      }

      SeperateWord(modes);
      if (modes == NULL) {
        Error(ERR_PROTOCOL | ERR_WARNING, "Mode change with no parameters from %s", sender);
        return 0;
      }

      for (cp = modes; *cp != '\0'; cp++) { /* Step over each character in the mode string */
        switch (*cp) {

        case '+':
          dir = 1;
          break;
        case '-':
          dir = 0;
          break;
        case 'o':
          usrptr->oper = dir;
          break;
        }
      }
    } else {
      /* Channel mode change -- which we healthily ignore for now */
      /* Eeep! Need to examine -i channelmodes, since we introduced invite only channels. */
      DoChanMode(sender, target, modes);
    }
  } else if (command[0] == 'P' && command[1] == '\0') {
    /* Privmsg command recieved. */
    /* Usersupport... ugh. */
    /* a9AAD P #twilightzone :ooops */
    /* a9AAD P #twilightzone :I missed a field out :) */
    /* a9AAD P ppAAA :YOU SUCK */

    char *content;

    content = SeperateWord(tail);

    /* Ignore the target (assume it was us..) */
    /* Note that since we're not doing AUTH we don't need "secure" messages */

    /* Check that the user is authed -- we don't talk to unauthed masses */

    ProcessMessage(sender, content + 1);
  } else if (command[0] == 'G' && command[1] == '\0') {
    /* Ping command recieved. */
    /* Send "Im fine. Weather is good. Send more money.". */
    char buf[512];

    sprintf(buf, "%s Z %s\r\n", my_numeric, tail);
    SendLine(buf);
  } else if (command[0] == 'S' && command[1] == '\0') {
    /* Server command recieved. */
    /* Create server in the servertree. */
    /* AH S hub.uk.quakenet.org 2 0 1013950331 P10 AKAD] 0 :BarrysWorld QuakeNet IRC Server */

    char *servername;
    int count;
    int iparentnumeric;
    int iservernumeric;
    int imaxusersnumeric;

    servername = tail;
    for (count = 0; count < 5; count++)
      tail = SeperateWord(tail);

    iparentnumeric = NumericToLong(sender, 2);
    iservernumeric = NumericToLong(tail, 2);
    imaxusersnumeric = NumericToLong(tail + 2, 3) + 1;

    AddServer(iservernumeric, servername, imaxusersnumeric, iparentnumeric);
  } else if (command[0] == 'S' && command[1] == 'Q' && command[2] == '\0') {
    /* Server Quit command recieved. */
    /* Delete server in the servertree and remove all corresponding clients. */
    /* [h SQ wind.splidge.netsplit.net 0 :Ping timeout */

    char *servername;

    servername = tail;
    tail = SeperateWord(tail);
    HandleSquit(servername);
  } else if (command[0] == 'E' && command[1] == 'B' && command[2] == '\0') {
    /* End of burst command. */
    /* If it's OUR server we need to ack */
    char buf[10];

    /* Quick hack to get some stats whenever a EB is recieved. */
    UserHashStats();

    if (!strncmp(sender, MyServer, 2)) {
      /* It was my server which said EB so throw back a EA. */
      /* We end our own burst here too */
      burst_done = 1;
#ifndef HORRIBLE_DEOPALL_HACK
      sprintf(buf, "%s EB\r\n", my_numeric);
      SendLine(buf);
      sprintf(buf, "%s EA\r\n", my_numeric);
      SendLine(buf);
#endif
    }
  } else {
    /* Crap command recieved. */
    /* Well, is there anything else we care about? */

  }

  return (1);
}

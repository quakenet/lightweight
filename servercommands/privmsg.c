/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: privmsg.c,v 1.51 2004/08/18 16:07:50 froo Exp $
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

#include <base64.h>

void ProcessMessage(char *numeric, char *text)
{
  struct user *user_ptr;
  char *command;
  char *tail;

  user_ptr = usertablepointer[getserverindex(numeric)][getclientindex(numeric)];  /* corresponding user struct */

  if (user_ptr == NULL) {
    Error(ERR_ERROR | ERR_INTERNAL, "Message from non-existent user %s", numeric);
    return;
  }
#ifndef HORRIBLE_DEOPALL_HACK
  /* If we are talking to O, just ignore anything O says back to us */
  if (user_ptr == Optr)
    return;
#endif

  /* Firstly firstly, we need to take care of CTCP commands. */
  if (*text == '\001') {
    /* Handle CTCP's here. */
    return;
  }

  /* Firstly, we only talk to authed users */

  if (user_ptr->authedas == NULL) {
    NoticeToUser(user_ptr, "You are not authed.  Please auth with Q before sending me commands.");
    return;
  }

  if (!(user_ptr->oper || global_enable)) {
    /* We're too busy to handle commands, tell them to go away. */
    NoticeToUser(user_ptr, "Due to overload your command cannot be processed at present. Please try again in a few moments.");
    return;
  }

  /* Pick off the first word */

  command = text;
  tail = SeperateWord(text);

  /* User commands go here */

  if (!Strcmp(command, "help")) {
    dohelp(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "showcommands")) {
    doshowcommands(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "whois")) {
    dowhois(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "whoami")) {
    dowhoami(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "unbanall")) {
    dounbanall(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "clearchan")) {
    doclearchan(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "op")) {
    doop(user_ptr, tail);
    return;
  }
#ifdef SIT_ON_CHANNELS
  if (!Strcmp(command, "invite")) {
    doinvite(user_ptr, tail);
    return;
  }
  if (!Strcmp(command, "setinvite")) {
    dosetinvite(user_ptr, tail);
    return;
  }
  if (!Strcmp(command, "clearinvite")) {
    doclearinvite(user_ptr, tail);
    return;
  }
#endif /* SIT_ON_CHANNELS */

  if (!Strcmp(command, "voice")) {
    dovoice(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "deopall")) {
    dodeopall(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "chanlev")) {
    dochanlev(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "adduser")) {
    doadduser(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "removeuser")) {
    doremoveuser(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "version")) {
    doversion(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "welcome")) {
    dowelcome(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "recover")) {
    dorecover(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "suspendlist") && (10 <= user_ptr->authedas->authlevel)) {
    dosuspendlist(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "chanstat")) {
    dochanstat(user_ptr, tail);
    return;
  }

  if (!Strcmp(command, "requestowner")) {
    dorequestowner(user_ptr, tail);
    return;
  }

  if (user_ptr->oper) {
    /* Oper commands go here */
    if (!Strcmp(command, "addchan")) {
      doaddchan(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "delchan")) {
      dodelchan(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "changelev")) {
      dochangelev(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "save")) {
      dosave(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "deluser")) {
      dodeluser(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "suspend")) {
      dosuspend(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "unsuspend")) {
      dounsuspend(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "status")) {
      dostatus(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "suspendlist")) {
      dosuspendlist(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "channellist")) {
      dochannellist(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "accountlist")) {
      doaccountlist(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "listlevel")) {
      dolistlevel(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "userlist")) {
      douserlist(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "neigh")) {
      doneigh(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "cow")) {
      docow(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "accountstats")) {
      doaccountstats(user_ptr);
      return;
    }
    if (!Strcmp(command, "noticeme")) {
      donoticeme(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "fish")) {
      dofish(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "reauth")) {
      doreauth(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "raw")) {
      doraw(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "cleanupdb")) {
      docleanupdb(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "die")) {
      dodie(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "grep")) {
      dogrep(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "find")) {
      dofind(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "part")) {
      dopart(user_ptr, tail);
      return;
    }
    if (!Strcmp(command, "cleanupdb2")) {
      docleanupdb2(user_ptr, tail);
      return;
    }
    if (!Strcmp(command,"sendchanlev")) {
      dosendchanlev(user_ptr, tail);
      return;
    }
    if (!Strcmp(command,"setenable")) {
      dosetenable(user_ptr, tail);
      return;
    }

    NoticeToUser(user_ptr, "Unknown command.");
    return;
  }

  NoticeToUser(user_ptr, "This command is either unknown, or you need to be opered up to use it.");
}

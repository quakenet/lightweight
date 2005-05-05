/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: deopall.c,v 1.20 2003/04/26 21:08:03 froo Exp $
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

/* deopall.c */

/*
 * deopall: deop everyone on the channel
 * 
 * Parameters:
 *  #channel   - Channel to remove all ops from
 *
 * Requires +o flag on channel.
 *
 * Note that we're using a nasty hack to effect this :) 
 */

void dodeopall(struct user *user, char *tail)
{
  unsigned char flags;
  struct reggedchannel *chanptr;
  char *channel;

#if defined(HORRIBLE_DEOPALL_HACK) || defined(HAVE_CLEARMODE)
  char buf[512];
#endif /* HORRIBLE_DEOPALL_HACK */

  channel = tail;
  SeperateWord(tail);

  if (channel == NULL) {
    NoticeToUser(user, "Usage: deopall #channel");
    return;
  }

  if ((chanptr = GetChannelPointer(channel)) == NULL) {
    NoticeToUser(user, "Unknown channel %s.", channel);
    return;
  }

  flags = GetChannelFlags(user->authedas, chanptr);

  /* Check for MASTER or OWNER flag */
  if (!(flags & (CFLAG_MASTER | CFLAG_OWNER)) || IsSuspended(chanptr)) {
    /* No flags -- perhaps they are an oper? */
    if (!((user->oper) && (user->authedas->authlevel > 200))) {
      /* Nope, not an oper either */
      NoticeToUser(user, "Sorry, you need the +m or +n flag on %s to use deopall.", channel);
      return;
    }
  }

  /* Do the actual deopall */
  /* erm, how? */

/*
  sprintf(buf,"%s GL * +%s 0 :Clearing channel\r\n",my_numeric,channel);
  SendLine(buf);
*/

#ifdef HAVE_CLEARMODE
  sprintf(buf, "%sAAA CM %s o\r\n", my_numeric, channel);
  SendLine(buf);
  sprintf(buf, "%s M %s +o %sAAA\r\n", my_numeric, channel, my_numeric);
  SendLine(buf);
#else /* !HAVE_CLEARMODE */
# ifdef HORRIBLE_DEOPALL_HACK
  if (chanptr->timestamp > 0) {
    chanptr->timestamp--;
#  ifdef SIT_ON_CHANNELS
    /* If we're sitting on the channel, we can just rejoin it with a new timestamp... */
    if (IsJoined(chanptr)) {
      sprintf(buf, "%sAAA L %s\r\n", my_numeric, channel);
      SendLine(buf);
    }
    SetJoined(chanptr);
    sprintf(buf, "%s B %s %ld %sAAA:o\r\n", my_numeric, channel, chanptr->timestamp, my_numeric);
#  else /* !SIT_ON_CHANNELS */
    /* We're not sitting on the channel, so we send a burst with a _fake_ user */
    sprintf(buf, "%s B %s %ld %sAAB:o\r\n", my_numeric, channel, chanptr->timestamp, my_numeric);
#  endif /* SIT_ON_CHANNELS */
    SendLine(buf);
  } else {
    NoticeToUser(user, "Sorry, cannot deopall on %s at this time.", channel);
    return;
  }
# else /* !HORRIBLE_DEOPALL_HACK */
  if (NULL == Optr) {
    NoticeToUser(user, "Sorry, cannot deopall on %s at this time.", channel);
    return;
  } else {
    MessageToUser(Optr, "deopall -l %s", channel);
/*** This is no longer required
    MessageToUser(Optr, "opchan %s %s", channel, my_nick); */
  }
# endif /* HORRIBLE_DEOPALL_HACK */
#endif /* HAVE_CLEARMODE */

  Log("DeopAll: %s (%s) requested deopall in %s", user->nick, user->authedas->authname, channel);
  NoticeToUser(user, "Done.");
}

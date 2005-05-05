/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: die.c,v 1.2 2004/08/29 19:06:18 froo Exp $
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

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* die.c */

void dodie(struct user *user, char *tail)
{
  char buf[1024];

  if (!user->oper) {
    NoticeToUser(user, "You are not an operator");
    return;
  }

  /* Check auth level.. */
  if (!CheckAuthLevel(user, 255))
    return;

  if (NULL == tail) {
    NoticeToUser(user, "Usage: die <channel part reason>");
    return;
  } else if (strlen(tail) < 4) {
    NoticeToUser(user, "You are in a little maze of twisting passages, all different.");
    return;
  }

  tail[MIN(strlen(tail), 512)] = '\0';
  Log("Die: %s (%s) requested die.", user->nick, user->authedas->authname);
  dosave(user, NULL);
  NoticeToUser(user, "*POFF!* %s disappears in a dense cloud of orange smoke..", my_nick);
  sprintf(buf, "%sAAA Q :%s\r\n", my_numeric, tail);
  SendLine(buf);
  sleep(3);
  exit(0);
}

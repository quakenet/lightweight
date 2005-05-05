/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: save.c,v 1.6 2002/07/02 16:38:53 zarjazz Exp $
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

/* save.c */

void dosave(struct user *user, char *tail)
{

  /* Check auth level.. */
  if (!CheckAuthLevel(user, 240))
    return;

  /* Put all relevant saving functions here */
  FlushLogs();
  if (SaveDBToDisk()) {
    Log("Save: %s (%s) requested save. Save failed.", user->nick, user->authedas->authname);
    NoticeToUser(user, "Error saving database.. HELP!");
  } else {
    Log("Save: %s (%s) requested save. Save succeeded.", user->nick, user->authedas->authname);
    NoticeToUser(user, "Done. Save seemed OK.");
  }
}

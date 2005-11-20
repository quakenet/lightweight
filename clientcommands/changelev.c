/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
 *
 * $Id: changelev.c,v 1.9 2003/09/08 01:19:25 zarjazz Exp $
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

/* changelev.c */

/*
 * changelev: Changes a user's authlevel
 *
 * Parameters:
 *  user         - either #authname or nick
 *  level        - level to change to (0 < level < 256)
 */

void dochangelev(struct user *user, char *tail)
{
  struct account *victim;
  char *usertochange;
  char *levelstring;
  long newlevel;

  usertochange = tail;
  levelstring = SeperateWord(tail);
  SeperateWord(tail);

  /* Check that we've got enough parameters and that they are sane */
  if (levelstring == NULL) {
    NoticeToUser(user, "Usage: changelev user level");
    NoticeToUser(user, " where user   is the user to change (either #authname or nick)");
    NoticeToUser(user, "       level  is the level to set (must be less than your own)");
    return;
  }

  newlevel = strtol(levelstring, NULL, 10);
  victim = FindAccount(usertochange);

  Log("ChangeLev: %s (%s) requested changelev for %s to level %s", user->nick, user->authedas->authname, usertochange,
      levelstring);

  if (victim == NULL) {
    NoticeToUser(user, "I couldn't find that user, or they are not authed.");
    return;
  }

  if (newlevel == NULL) {
    NoticeToUser(user, "You didn't specify a new auth level.");
    return;
  }

  if (newlevel < 0 || newlevel > 255) {
    NoticeToUser(user, "That level is out of range.  Valid levels are between 0 and 255.");
    return;
  }

  if (newlevel >= user->authedas->authlevel) {
    NoticeToUser(user, "You cannot give a user a higher level than you own!");
    return;
  }

  if ((victim->authlevel >= user->authedas->authlevel) && (victim != user->authedas)) {
    NoticeToUser(user, "You cannot change the level of a user with a higher level than you!");
    return;
  }

  victim->authlevel = newlevel;

  Log("ChangeLev: %s (%s) changed level for %s (%s) to level %s", user->nick, user->authedas->authname, usertochange,
      victim->authname, levelstring);

  NoticeToUser(user, "Userlevel changed.");
}

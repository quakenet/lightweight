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

/* deluser.c */

/*
 * deluser: Removes a user from the bot
 * 
 * Parameters:
 *  nick   - user to remove
 *
 * Operator only.
 */

void dodeluser(struct user *usr_ptr, char *tail)
{
  struct account *act_ptr;  /* target */
  char *tnick;

  tnick = tail;
  SeperateWord(tail);
  act_ptr = FindAccount(tnick);

  if (NULL == tail) {           /* print the usage */
    NoticeToUser(usr_ptr, "Usage: deluser nick");
    return;
  }

  if (NULL == act_ptr) {
    NoticeToUser(usr_ptr, "'%s' no such user", tnick);
    return;
  }
  /* ready to delete */
  RemoveAccount(act_ptr);

  Log("DelUser: %s (%s) requested deluser %s", usr_ptr->nick, usr_ptr->authedas->authname, tnick);
  NoticeToUser(usr_ptr, "Done.");
}

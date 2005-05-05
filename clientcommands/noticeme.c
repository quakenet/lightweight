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

/* noticeme.c */

/*
 * noticeme: toggles usage of PRIVMSG/NOTICES
 * 
 * Parameters:
 *  on   - use NOTICE, to communicate w. the user
 *  off  - use PRIVMSG, to communicate w. the user
 *
 * Operator only.
 */

void donoticeme(struct user *usr_ptr, char *tail)
{
  char *onoff;

  onoff = tail;
  SeperateWord(tail);

  if (onoff == NULL) {          /* print the usage */
    NoticeToUser(usr_ptr, "Usage: noticeme on|off");
    return;
  }

  if (!usr_ptr->oper) {
    NoticeToUser(usr_ptr, "You are not an operator");
    return;
  }

  if (!strncmp(onoff, "on", 2)) {
    usr_ptr->oper = 1;
  } else {
    usr_ptr->oper = 2;
  }

  NoticeToUser(usr_ptr, "Done.");
}

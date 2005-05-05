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

/* setenable.c */

/*
 * setenable: Enables or disables all user commands
 * 
 * Parameters:
 *  none
 *
 */

void dosetenable(struct user *usr_ptr, char *tail)
{
  int status;
  char *option;

  if (!usr_ptr->oper) {
    NoticeToUser(usr_ptr, "You are not an operator");
    return;
  }

  if (!CheckAuthLevel(usr_ptr, 255))
    return;

  option = tail;
  SeperateWord(tail);
  if (option == NULL) {
    NoticeToUser(usr_ptr, "Usage: setenable <0|1>");
    NoticeToUser(usr_ptr, "User commands are currently: %s", global_enable ? "ENABLED" : "DISABLED");
    return;
  }

  status = strtol(option, NULL, 10);

  if (status == 0) {
    NoticeToUser(usr_ptr, "Clearing enable flag - users will be unable to send commands.");
    global_enable = 0;
  } else {
    NoticeToUser(usr_ptr, "Setting enable flag - users will now be able to send commands.");
    global_enable = 1;
  }
}

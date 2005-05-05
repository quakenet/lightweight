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

/* raw.c */

/*
 * raw: issues a raw command
 * 
 * Parameters:
 *  numeric - server numeric
 *  cmd     - P10 protocol message
 *
 * Operator only.
 */

void doraw(struct user *usr_ptr, char *tail)
{
  char *raw_numeric;
  char *raw_data;
  char buf[512];

  if (!usr_ptr->oper) {
    NoticeToUser(usr_ptr, "You are not an operator");
    return;
  }

  if (!CheckAuthLevel(usr_ptr, 255))
    return;

  raw_numeric = tail;
  raw_data = SeperateWord(tail);

  if (NULL == raw_numeric) {
    NoticeToUser(usr_ptr, "Usage: raw numeric data");
    return;
  }

  NoticeToUser(usr_ptr, "sending: %s %s", raw_numeric, raw_data);

  Log("Raw: %s (%s) requested raw '%s' '%s'", usr_ptr->nick, usr_ptr->authedas->authname, raw_numeric, raw_data);
  sprintf(buf, "%s %s\r\n", raw_numeric, raw_data);
  SendLine(buf);

  NoticeToUser(usr_ptr, "Done.");
}

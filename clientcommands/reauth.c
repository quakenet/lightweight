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

/* reauth.c */

/*
 * reauth: force's a reauth w. O
 * 
 * Parameters:
 *  none
 *
 * Operator only.
 */

void doreauth(struct user *usr_ptr, char *tail)
{
  char buf[512];
  struct user *tmp_ptr;

  if (!usr_ptr->oper) {
    NoticeToUser(usr_ptr, "You are not an operator");
    return;
  }
#ifndef HORRIBLE_DEOPALL_HACK
  tmp_ptr = FindUserByNick(o_nick);
  if (NULL == tmp_ptr) {
    NoticeToUser(usr_ptr, "%s is not online right now.", o_nick);
    return;
  }
  Optr = tmp_ptr;
  sprintf(buf, "%sAAA P %s :auth %s %s\r\n", my_numeric, o_userserver, o_login, o_pass);
  SendLine(buf);

  Log("Suspend: %s (%s) requested reauth to O", usr_ptr->nick, usr_ptr->authedas->authname);
  NoticeToUser(usr_ptr, "Done.");
#else
  NoticeToUser(usr_ptr, "%s is not activated in this build of %s.", o_nick, my_nick);
#endif
}

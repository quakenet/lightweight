/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkil� & David Mansell
 *
 *

******************************************************************************$
*/
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
USA
*/

#include <lightweight.h>
#include <globalexterns.h>
#include <channelsdb.h>
#include <accountsdb.h>
#include <channels.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern int match2strings(char *, char *);

/*
 * userlist: Lists users based on a pattern
 *
 * Parameters:
 *  pattern        - Pattern to search
 */

void douserlist(struct user *usr_ptr, char *ptrn)
{
  int i, j;
  int match = 0;
  char tmp[80];
  struct user *wrk_ptr;

  if (NULL == ptrn) {
    NoticeToUser(usr_ptr, "Usage: userlist pattern");
    NoticeToUser(usr_ptr, "where  pattern is the pattern to search for");
    return;
  }
  for (i = 0; i < HASHMAX; i++) {
    wrk_ptr = nickhashtable[i];
    while (NULL != wrk_ptr) {
      if (match2strings(ptrn, wrk_ptr->nick)) {
        if (!match) {
          NoticeToUser(usr_ptr, "Nick                Authed as           ");
          NoticeToUser(usr_ptr, "--------------------------------------");
        }
        match++;
        if (100 < match) {
          NoticeToUser(usr_ptr, "More than 100 entries found, please respecify your search");
          return;
        }
        memset(tmp, ' ', 80);
        sprintf(tmp, "%s%n", wrk_ptr->nick, &j);
        tmp[j] = ' ';
        if (NULL == wrk_ptr->authedas)
          sprintf(tmp + 20, "User is not authed");
        else
          sprintf(tmp + 20, "%s", wrk_ptr->authedas->authname);
        NoticeToUser(usr_ptr, "%s", tmp);
      }
      wrk_ptr = wrk_ptr->nextbynick;
    }
  }
  if (!match)
    NoticeToUser(usr_ptr, "Sorry no users match %s", ptrn);
}

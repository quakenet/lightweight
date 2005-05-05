/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & Raimo Nikkilä & David Mansell
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

/*
 * listlevel: Lists accounts matching authlevel [upto]
 *
 * Parameters:
 *  authlevel        - Authlevel to start at
 *  upto             - Optional authlevel to stop at
 */

void dolistlevel(struct user *user, char *tail)
{
  int i;
  struct account *wrk_ptr;
  char *levelstring;
  char *uptostring;
  long level;
  long upto;
  int match = 0, nr = 0;

  if (NULL != tail && 0 == strlen(tail)) tail = NULL;

  levelstring = tail;
  uptostring = SeperateWord(tail);
  SeperateWord(uptostring);

  if (NULL == levelstring) {
    NoticeToUser(user, "Usage: listlevel authlevel [upto]");
    NoticeToUser(user, "List all users with a auth level of \"authlevel\", or between \"authlevel\" and \"upto\" (if specified)");
    return;
  }

  level = strtol(levelstring, NULL, 10);
  upto = (uptostring) ? strtol(uptostring, NULL, 10) : level;

  if (255 < level)
    level = 255;

  if (0 > level)
    level = 0;

  if (255 < upto || level > upto || 0 == upto || 0 > upto)
    upto = level;

  for (i = 0; i < HASHMAX; i++) {
    wrk_ptr = accounthashtable[i];
    while (NULL != wrk_ptr) {
      ++nr;
      if (level <= wrk_ptr->authlevel && wrk_ptr->authlevel <= upto && match < 1000) {
        if (!match) {
          NoticeToUser(user, "Authlevel  Authname         Last auth");
          NoticeToUser(user, "----------------------------------------------------");
        }

        NoticeToUser(user, "%4d       %-15s  %s", wrk_ptr->authlevel, wrk_ptr->authname, ctime(&wrk_ptr->lastauth));

        if (1000 <= ++match) {
          NoticeToUser(user, "-- List truncated, more than 1000 hits");
          break;
        }
      }
      wrk_ptr = wrk_ptr->nextbyauthname;
    }
  }

  NoticeToUser(user, "-- End of list -- Found %d account%s (of %d)", match, (1 == match) ? "" : "s", nr);
  return;
}

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

extern int match2strings(char *, char *);

/*
 * channellist: Lists channels based on a pattern
 *
 * Parameters:
 *  pattern        - Pattern to search
 */

void dochannellist(struct user *usr_ptr, char *ptrn)
{
  int i, j;
  int match = 0;
  char tmp[80];
  struct reggedchannel *wrk_ptr;

  if (NULL == ptrn) {
    NoticeToUser(usr_ptr, "Usage: channellist pattern");
    NoticeToUser(usr_ptr, "where  pattern is the searchpattern to match the channel name ");
    return;
  }
  for (i = 0; i < HASHMAX; i++) {
    wrk_ptr = channelhashtable[i];
    while (NULL != wrk_ptr) {

      if (match2strings(ptrn, wrk_ptr->channelname)) {
        /* match */
        if (!match) {
          NoticeToUser(usr_ptr, "channel                         added by          owner");
          NoticeToUser(usr_ptr, "-------------------------------------------------------------------------------");
        }
        match++;
        if (100 < match) {
          NoticeToUser(usr_ptr, "More than 100 results, please make a more specific search");
          return;
        }
        memset(tmp, ' ', 79);
        sprintf(tmp, "%s%n", wrk_ptr->channelname, &j);
        tmp[j] = ' ';           /* remove NULL */
        sprintf(tmp + 32, "%s%n", wrk_ptr->addedby, &j);
        tmp[j + 32] = ' ';      /* remove NULL */
        if (NULL == wrk_ptr->founder)
          sprintf(tmp + 50, "Unknown");
        else
          sprintf(tmp + 50, "%.29s", wrk_ptr->founder->authname);
        NoticeToUser(usr_ptr, "%s", tmp);
      }
      wrk_ptr = wrk_ptr->nextbychannelname;
    }
  }
  if (!match)
    NoticeToUser(usr_ptr, "Sorry no channels match %s.", ptrn);
}

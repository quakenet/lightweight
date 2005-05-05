/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: accountstats.c,v 1.3 2003/09/08 01:19:25 zarjazz Exp $
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
#include <channels.h>

/*
 * accountstats: shows stats about accounts
 * 
 * Parameters:
 *  none
 *
 * Requires oper powah
 */

void doaccountstats(struct user *user)
{
  int numberofchans[CHANNELSPERUSER + 1];
  int i;
  int j;
  int k;
  struct account *currentaccount;

  for (i = 0; i <= CHANNELSPERUSER; i++)
    numberofchans[i] = 0;

  for (i = 0; i < HASHMAX; i++) {
    currentaccount = accounthashtable[i];
    while (currentaccount) {

      k = 0;
      for (j = 0; j < CHANNELSPERUSER; j++) {
        if (currentaccount->channel[j] != NULL) {
          k++;
        }
      }
      numberofchans[k]++;

      currentaccount = currentaccount->nextbyauthname;
    }
  }

  for (i = 0; i <= CHANNELSPERUSER; i++) {
    NoticeToUser(user, "%02d: %5d", i, numberofchans[i]);
  }

  NoticeToUser(user, "Done.");
}

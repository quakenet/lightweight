/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: savedbtodisk.c,v 1.18 2003/09/08 01:19:25 zarjazz Exp $
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
#include <accountsdb.h>
#include <channelsdb.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int SaveDBToDisk(void)
{
  /* This functions saves the accounts and channels to specified files. */
  int i, j, k;  /* Loopcounters. */
  char srcfile[256];  /* Source and destination filenames. */
  char dstfile[256];  /* How long should these be? */
  FILE *accountfile;
  struct account *acct;
  struct reggedchannel *chan;
  char chanflags[9];

  /* Delete the oldest savefiles. */
  snprintf(dstfile, 256, "%s.%i", ACCOUNTFILE, NUMBEROFSAVEFILES);
  unlink(dstfile);

  /* Backup the old files. */
  for (i = NUMBEROFSAVEFILES; i > 0; i--) {
    snprintf(srcfile, 256, "%s.%i", ACCOUNTFILE, i - 1);
    snprintf(dstfile, 256, "%s.%i", ACCOUNTFILE, i);
    rename(srcfile, dstfile);
  }

  /* Try to open the two output files. */
  snprintf(srcfile, 256, "%s.%i", ACCOUNTFILE, 0);
  accountfile = fopen(srcfile, "w");
  if (!accountfile) {
    Error(ERR_ERROR | ERR_LOADSAVE, "Couldn't open the data file for writing");
    return (1);
  }

  /* Run through all the accounts and save them one by one. */
  for (i = 0; i < HASHMAX; i++) {
    /* Run through the linked list using currentaccount as pointer. */
    for (acct = accounthashtable[i]; acct != NULL; acct = acct->nextbyauthname) {
      /* Think it's best to store the chanlev with the channels */
      /* so a one-liner is fine here -- splidge */
      fprintf(accountfile, "%s %d %lu\n", acct->authname, acct->authlevel, acct->lastauth);
    }
  }

  fprintf(accountfile, "--- End of users\n");

  /* And the channels... */

  /* PrintCFlags is only going to touch the first 8 chars of chanflags
   * so put a NUL in here to save doing it 237964023984092 times later
   */

  chanflags[8] = '\0';

  for (i = 0; i < HASHMAX; i++) {
    chan = channelhashtable[i];
    for (chan = channelhashtable[i]; chan != NULL; chan = chan->nextbychannelname) {
      /* Forgive my use of ?: here.. it's possible the founder account 
       * will be lost, in which case we put a placeholder "???" into the
       * file instead  - splidge */
      fprintf(accountfile, "%s %s %s %lu %lu %d\n", chan->channelname, chan->addedby,
              (chan->founder == NULL ? "???" : chan->founder->authname), chan->lastused, chan->dateadded, chan->flags);

      /* If the channel is SUSPENDed, state why */
      /* Use of ?: here is as above */
      if (IsSuspended(chan)) {
        fprintf(accountfile, "%s %s\n", (chan->suspendby == NULL ? "???" : chan->suspendby->authname),
                chan->suspendreason);
      }

      /* If the channel is WELCOMEd, state the welcomemessage */
      /* Use of ?: here is as above */
      if (IsWelcomeFlaged(chan)) {
        fprintf(accountfile, "%s\n", chan->welcome);
      }

      /* Now dump the chanlev */
      for (j = 0; j < USERSPERCHANNEL; j++) {
        if (chan->channeluser[j] == NULL)
          continue;             /* There's no attempt to keep the users compacted in the array, 
                                 * so if NULL skip to the next one */
        k = GetUserChannelIndex(chan, chan->channeluser[j]);
        PrintCFlags(chanflags, chan->channeluser[j]->channelflags[k]);
        fprintf(accountfile, "%s %.8s\n", chan->channeluser[j]->authname, chanflags);
      }

      fprintf(accountfile, "--- End of channel %s\n", chan->channelname);
    }
  }
  fclose(accountfile);
  return 0;
}

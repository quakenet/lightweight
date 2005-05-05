/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: restoredbfromdisk.c,v 1.23 2003/09/08 01:19:25 zarjazz Exp $
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
#include <channelsdb.h>
#include <accountsdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int RestoreDBFromDisk(void)
{
  char buf[512];
  FILE *infile;
  char *username, *levelstr, *timestr;
  char *channame, *addedby, *founderstr;
  char *chanuser, *flags;
  struct account *acct;
  struct reggedchannel *chan;
  int lastused;
  char *channeltimestamp;
  char *dateaddedstr;
  char *flagstr;
  char *suspendbystr, *suspendreasonstr;
  char *welcomemessage;
  char *chrp;
  int chanusers;

  sprintf(buf, "%s.0", ACCOUNTFILE);

  if ((infile = fopen(buf, "r")) == NULL) {
    Error(ERR_LOADSAVE | ERR_ERROR, "Couldn't open the account database");
    return 1;
  }

  /* Read the accounts first; use an infinite loop and break when 
   * we see the seperator */
  for (;;) {
    fgets(buf, sizeof(buf), infile);

    if (!strncmp(buf, "--- End", 7))
      break;

    if (feof(infile)) {
      /* End of file here is WRONG.  Quit. */
      Error(ERR_LOADSAVE | ERR_FATAL, "Unexpected EOF in user section");
    }

    username = buf;
    levelstr = SeperateWord(buf);
    timestr = SeperateWord(levelstr);
    SeperateWord(timestr);

    if (timestr == NULL) {
      Error(ERR_LOADSAVE | ERR_WARNING, "Corrupt account info line");
      continue;
    }

    if (strlen(username) > NICKLEN) {
      Error(ERR_LOADSAVE | ERR_WARNING, "Nick too long: %s", username);
      continue;
    }

    acct = AddAccount(username);
    acct->authlevel = strtol(levelstr, NULL, 10);
    acct->lastauth = strtol(timestr, NULL, 10);
  }

  /* Now we're onto the channels */
  while (!feof(infile)) {
    fgets(buf, sizeof(buf), infile);
    if (feof(infile))
      break;

    channame = buf;
    addedby = SeperateWord(buf);
    founderstr = SeperateWord(addedby);
    channeltimestamp = SeperateWord(founderstr);
    dateaddedstr = SeperateWord(channeltimestamp);
    flagstr = SeperateWord(dateaddedstr);
    SeperateWord(flagstr);

    if (flagstr == NULL) {
      Error(ERR_LOADSAVE | ERR_WARNING, "Corrupt channel info line %s %s %s %s %s %s", channame, addedby, founderstr,
            channeltimestamp, dateaddedstr, flagstr);
      continue;
    }

    if (strlen(channame) > 29) {
      Error(ERR_LOADSAVE | ERR_WARNING, "Channel name too long: %s", channame);
      continue;
    }

    if (strlen(addedby) > NICKLEN) {
      Error(ERR_LOADSAVE | ERR_WARNING, "Addedby name too long: %s", addedby);
      continue;
    }

    lastused = strtol(channeltimestamp, NULL, 10);
    /*
       if (strlen(addedby) > NICKLEN) {
       Error(ERR_LOADSAVE | ERR_WARNING, "Addedby name too long: %s", addedby);
       continue;
       }
     */

    /* Create the channel.  Note we DON'T pass the founder in, because
     * we don't want to add the default chanlev flags */
    chan = AddChannel(channame, NULL, addedby, lastused, NULL);
    if (chan == NULL) {
      /* Erk!  Couldn't add channel */
      Error(ERR_LOADSAVE | ERR_WARNING, "Couldn't create channel %s", channame);
      continue;
    }
    chan->founder = GetAccountByAuthName(founderstr); /* If this returns NULL we don't care.. */

    chan->dateadded = strtol(dateaddedstr, NULL, 10);
    chan->flags = strtol(flagstr, NULL, 10);

    /* Clear the "is joined" flag if it was set */
    /* All the other flags are persistent */
    ClearJoined(chan);

    /* Channel was suspended, read in who and why */
    if (IsSuspended(chan)) {
      fgets(buf, sizeof(buf), infile);
      suspendbystr = buf;
      suspendreasonstr = SeperateWord(buf);

      /* Don't SeperateWord on suspendreasonstr -- read the rest of the string in */
      /* Need to pick off the line ending though */

      for (chrp = suspendreasonstr; *chrp; chrp++)
        if (*chrp == '\r' || *chrp == '\n')
          *chrp = '\0';

      if (suspendreasonstr == NULL) {
        /* Someone fed us a bad syntax file.. */
        Error(ERR_LOADSAVE | ERR_WARNING, "Couldn't parse suspendreason!");
        continue;
      }

      chan->suspendby = GetAccountByAuthName(suspendbystr); /* Again, if this is NULL, too bad */
      chan->suspendreason = strdup(suspendreasonstr);
    }

    /* Channel was welcomed, read in welcomemessage */
    if (IsWelcomeFlaged(chan)) {
      fgets(buf, sizeof(buf), infile);
      welcomemessage = buf;

      /* Don't SeperateWord on welcomemessage -- read the rest of the string in */
      /* Need to pick off the line ending though */

      for (chrp = welcomemessage; *chrp; chrp++)
        if (*chrp == '\r' || *chrp == '\n')
          *chrp = '\0';

      if (welcomemessage == NULL) {
        /* Someone fed us a bad syntax file.. */
        Error(ERR_LOADSAVE | ERR_WARNING, "Couldn't parse welcome message!");
        continue;
      }

      chan->welcome = strdup(welcomemessage);
    }

    chanusers = 0;

    /* Now for the chanlev */
    for (;;) {
      fgets(buf, sizeof(buf), infile);

      if (feof(infile)) {
        Error(ERR_LOADSAVE | ERR_WARNING, "File ends mid-channel");
        break;
      }

      if (!strncmp(buf, "--- End", 7)) {
#ifdef DEBUG
        Error(ERR_LOADSAVE | ERR_DEBUG, "OK, end of channel %s", chan->channelname);
#endif
        buf[0] = '\0';
        break;
      }

      chanuser = buf;
      flags = SeperateWord(buf);
      SeperateWord(flags);

      if (flags == NULL) {
        Error(ERR_LOADSAVE | ERR_WARNING, "User on chanlev with no flags?");
        continue;
      }
#ifdef DEBUG
      Error(ERR_DEBUG | ERR_LOADSAVE, "Adding user %s to %s", chanuser, chan->channelname);
#endif
      acct = GetAccountByAuthName(chanuser);

      chanusers++;
      AddChannelToAccount(acct, chan);
      SetUserChannelFlags(acct, chan, flags);
    }

    if (chanusers == 0) {
      /* Empty channel: delete it.  This can happen with buggy database files. */
      RemoveChannel(chan);
    }
  }

  fclose(infile);
  return 0;
}

/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: sendinittoserver.c,v 1.17 2003/09/08 01:19:25 zarjazz Exp $
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

/* Local function, only used here. */
int GetLine(char *buffer)
{
  /* Reads one byte at a time from serverfd until it has gotten a whole line.
   * Buffer must be atleast 513 bytes long.
   */

  return (1);
}

int ErrorCheck(void)
{
  /* check for errors */
  if (!strncmp(currentline, "ERROR", 5)) {
    Error(ERR_FATAL | ERR_PROTOCOL, currentline);
    return (1);
  }
  return (0);
}

int SendInitToServer(void)
{
  /* Initialises the connection with the server. */
  char buf[512];
  int starttime = time(NULL);
  int blanksfound = 0;
  char *mypos;
  int i;
  char maxuserstr[4];
  char MyServerName[SERVERNAMELENGTH];

  /* Send pass-string. */

  sprintf(buf, "PASS :%s\r\n", server_pass);
  SendLine(buf);
  /* Send server-string. */
  sprintf(buf, "SERVER %s 1 %d %d J10 %sAAD +s6 :%s\r\n", my_servername, starttime, starttime, my_numeric,
          my_description);
  SendLine(buf);

  /* Send our nick string */
  sprintf(buf, "%s N %s 1 %d TheLBot %s +odkr L B]AAAB %sAAA :%s\r\n", my_numeric, my_nick, starttime, my_servername,
          my_numeric, my_description);
  SendLine(buf);

  /* End of burst is removed from here now -- splidge */

  for (i = 0; i <= 1; i++) {
    while (!GetLineFromChunk()) {
      ReadChunk();              /* First pass will get the PASS line, second pass will get the SERVER line */
    }
    ErrorCheck();               /* check for errors on each pass */
  }

  /* Server response will be: */
  /* SERVER (name) 1 (starttime) (linktime) J10 (num,maxclients) 0 :(description) */

  /* skip over "SERVER" */
  mypos = currentline;
  while (*mypos != ' ') {
    if (mypos == '\0')
      Error(ERR_FATAL | ERR_PROTOCOL, "Invalid SERVER line");
    mypos++;
  }

  /* Copy server name */
  i = 0;
  mypos++;
  while (*mypos != ' ') {
    if (mypos == '\0') {
      Error(ERR_FATAL | ERR_PROTOCOL, "Invalid SERVER line");
    }
    MyServerName[i] = *mypos;
    i++;
    if (i == (SERVERNAMELENGTH - 1)) {
      Error(ERR_ERROR | ERR_PROTOCOL, "Server name too long");
      break;
    }
    mypos++;
  }

  MyServerName[i] = '\0';

  /* Now look for the section with numeric and maxusers... */

  for (;; mypos++) {
    if (*mypos == '\0') {
      /* Erk. We ran out of line :( */
      Error(ERR_ERROR | ERR_PROTOCOL, "Invalid SERVER line");
    } else if (*mypos == ' ') {
      /* Got a space.. */
      if (++blanksfound == 5)
        break;
    }
  }

  MyServer[0] = *(++mypos);
  MyServer[1] = *(++mypos);
  MyServer[2] = '\0';

  maxuserstr[0] = *(++mypos);
  maxuserstr[1] = *(++mypos);
  maxuserstr[2] = *(++mypos);
  maxuserstr[3] = '\0';

  AddServer(NumericToLong(MyServer, 2), MyServerName, NumericToLong(maxuserstr, 3) + 1, NumericToLong(my_numeric, 2));

  Error(ERR_DEBUG | ERR_PROTOCOL, "Got SERVER response: %s", currentline);
  printf("I am connected to: %s\n", MyServer);

  /* Get pass-string with GetLine(). */
  /* Get server-string with GetLine(). */
  /* (These calls are necessary as the two first lines from the server
   * does not follow the standard)
   */

  return (1);
}

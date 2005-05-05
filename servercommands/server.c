/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: server.c,v 1.12 2003/09/08 01:19:25 zarjazz Exp $
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

void InitServerList()
{
  /* This function makes sure that we dont have any crap in our serverlist or our usertablepointer list to start with. */
  int i;

  for (i = 0; i < 4096; i++) {
    servertree[i] = NULL;
    usertablepointer[i] = NULL;
  }
}

void AddServer(int servernum, const char *servername, int maxusers, int parent)
{
  /* Adds server 'servernum' to the servertree with the three last args as data. */

  int i;

  /* Errorhandling. */
  if (servertree[servernum] != NULL) {
    Error(ERR_ERROR | ERR_PROTOCOL, "Trying to add server %d that already exists.. deleting old server", servernum);
    DoSquitRecursive(servernum);
  }
  if (usertablepointer[servernum] != NULL) {
    /* This should NEVER EVER EVER HAPPEN: the DoSquitRecursive() above should have zapped the user table */
    Error(ERR_FATAL | ERR_INTERNAL, "User table already exists for server %d?!?", servernum);
  }

  /* Get a new server struct that we can fill out. */
  servertree[servernum] = GetServer();

  /* Fill in the data. */
  servertree[servernum]->maxusers = maxusers;
  a_strcpy(servertree[servernum]->servername, servername, sizeof(servertree[servernum]->servername));
  servertree[servernum]->parent = parent;

  /* Allocate a block of pointers to hold the pointers to the users on the channel. (see datastructures.txt) */
  usertablepointer[servernum] = (struct user **) malloc(maxusers * sizeof(void *));
  memoryused += maxusers * sizeof(void *);
  for (i = 0; i < maxusers; i++) {
    usertablepointer[servernum][i] = NULL;
  }

#ifdef DEBUG
  Error(ERR_DEBUG | ERR_PROTOCOL, "AddServer: created server %s (%d users) parent=%d", servername, maxusers, parent);
#endif
}

void HandleSquit(char *servername)
{
  /* SQUIT's the servername server and all servers under it. */
  int i;

  /* Search the servertree for the supplied servername and SQUIT recursively. */
  for (i = 0; i < 4096; i++) {
    if (servertree[i] != NULL) {
      if (!strncmp(servername, servertree[i]->servername, SERVERNAMELENGTH)) {
        /* Got a match */
        DoSquitRecursive(i);
        return;
      }
    }
  }

  Error(ERR_WARNING | ERR_PROTOCOL, "SQUIT for unknown server: %s", servername);
}

void DoSquitRecursive(int servernum)
{
  /* SQUIT server number i and all servers directly under it. */
  int i;

  Error(ERR_DEBUG | ERR_INTERNAL, "Doing SQUIT of server %d", servernum);

  /* SQUIT all the servers under it. These are the ones with parent==i. */
  for (i = 0; i < 4096; i++)
    if (servertree[i] != NULL)
      if (servertree[i]->parent == servernum)
        DoSquitRecursive(i);

  /* And release all the users connected to the server we're handling. */
  for (i = 0; i < servertree[servernum]->maxusers; i++)
    if (usertablepointer[servernum][i] != NULL)
      DeleteUser(usertablepointer[servernum][i]->numeric);

  /* Free the resources allocated for this server. */
  memoryused -= servertree[servernum]->maxusers * sizeof(void *);
  free(usertablepointer[servernum]);
  usertablepointer[servernum] = NULL;

  FreeServer(servertree[servernum]);
  servertree[servernum] = NULL;

  /*Quick hack: do userstats whenever a server quits. */
  UserHashStats();
}

/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: connecttoserver.c,v 1.13 2002/07/02 16:38:53 zarjazz Exp $
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

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

int ConnectToServer(void)
{
  struct sockaddr_in sockaddress;
  struct hostent *host;

  serverfd = socket(PF_INET, SOCK_STREAM, 0);
  if (serverfd == -1) {
    /* TODO: Do errorhandling. */
    return (0);
  }

  sockaddress.sin_family = AF_INET;
  sockaddress.sin_port = htons(atoi(server_port));
  host = gethostbyname(server_host);
  if (!host) {
    /* TODO: Do errorhandling. */
    return (0);
  }
  memcpy(&sockaddress.sin_addr, host->h_addr, sizeof(struct in_addr));
  if (connect(serverfd, (struct sockaddr *) &sockaddress, sizeof(struct sockaddr_in)) == -1) {
    /* TODO: Do errorhandling. */
    return (0);
  }

  return (1);
}

/*
 * SendLine:
 * sends a NUL-terminated string to the server connection 
 */

void SendLine(char *text)
{
  /* Sends a line to our uplink. */

  /* Warn if we are trying to send too much. IRC protocol only allows 512bytes per line. */
  if (strlen(text) > 512) {
    Log("SendLine: Warning, strlen(text) exceeds 512 bytes.");
    FlushLogs();
    /* Perhaps we should kill ourself here? Or ditch the line? */
  }

  /* Send the string. */
  write(serverfd, (void *) text, strlen(text));

  /* Log the string sent without the trailing return. */
  if (text[strlen(text) - 1] == '\n' || text[strlen(text) - 1] == '\r') {
    text[strlen(text) - 1] = '\0';
  }
  if (text[strlen(text) - 1] == '\r' || text[strlen(text) - 1] == '\n') {
    text[strlen(text) - 1] = '\0';
  }
  DebugLog("SendLine: %s", text);
}

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE	1024

void stripQuote(char *s)
{
  char *ptr;

  ptr = s;
  do {
    if ('"' != *ptr)
      *s++ = *ptr;
  } while (*ptr++);

  *s = '\0';
}

int LoadCfg(char *configfile)
{
  int c;
  FILE *fp;
  char line[BUF_SIZE], kw[BUF_SIZE], v[BUF_SIZE];

  if (!(fp = fopen(configfile, "r"))) {
    Error(ERR_WARNING | ERR_LOADSAVE, "can't open \"%s\".", configfile);
    return (-1);
  }

  while (fgets(line, BUF_SIZE, fp)) {
    if (line[0] == '#' || line[0] == '\0' || line[0] == '\n')
      continue;

    stripQuote(line);           /* remove any quotes */
    line[strlen(line) - 1] = '\0';  /* nuke last LF at EOL */
    if ((c = sscanf(line, "%[^=]=%[^\n]", kw, v)) != 2) {
      Error(ERR_ERROR | ERR_LOADSAVE, "illegal format \"%s\", skipping.", line);
      continue;
    }
#ifdef DEBUG2
    printf("keyword = [%s], value = [%s]\n", kw, v);
#endif

    if (!strncmp(kw, "server_port", strlen("server_port"))) {
      a_strcpy(server_port, v, sizeof(server_port));
      continue;
    }

    if (!strncmp(kw, "network_name", strlen("network_name"))) {
      a_strcpy(network_name, v, sizeof(network_name));
      continue;
    }

    if (!strncmp(kw, "server_host", strlen("server_host"))) {
      a_strcpy(server_host, v, sizeof(server_host));
      continue;
    }

    if (!strncmp(kw, "server_pass", strlen("server_pass"))) {
      a_strcpy(server_pass, v, sizeof(server_pass));
      continue;
    }

    if (!strncmp(kw, "my_nick", strlen("my_nick"))) {
      a_strcpy(my_nick, v, sizeof(my_nick));
      continue;
    }

    if (!strncmp(kw, "my_numeric", strlen("my_numeric"))) {
      a_strcpy(my_numeric, v, sizeof(my_numeric));
      continue;
    }

    if (!strncmp(kw, "my_servername", strlen("my_servername"))) {
      a_strcpy(my_servername, v, sizeof(my_servername));
      continue;
    }

    if (!strncmp(kw, "my_description", strlen("my_description"))) {
      a_strcpy(my_description, v, sizeof(my_description));
      continue;
    }
#ifndef HORRIBLE_DEOPALL_HACK
    if (!strncmp(kw, "o_nick", strlen("o_nick"))) {
      a_strcpy(o_nick, v, sizeof(o_nick));
      continue;
    }

    if (!strncmp(kw, "o_login", strlen("o_login"))) {
      a_strcpy(o_login, v, sizeof(o_login));
      continue;
    }

    if (!strncmp(kw, "o_pass", strlen("o_pass"))) {
      a_strcpy(o_pass, v, sizeof(o_pass));
      continue;
    }

    if (!strncmp(kw, "o_userserver", strlen("o_userserver"))) {
      a_strcpy(o_userserver, v, sizeof(o_userserver));
      continue;
    }
#endif /* !HORRIBLE_DEOPALL_HACK */

    Error(ERR_ERROR | ERR_LOADSAVE, "unknown line \"%s\", skipping.", line);
  }

#ifdef DEBUG
  printf("server_port = [%s]\n", server_port);
  printf("server_host = [%s]\n", server_host);
  printf("server_pass = [%s]\n", server_pass);
  printf("my_nick = [%s]\n", my_nick);
  printf("my_numeric = [%s]\n", my_numeric);
  printf("my_servername = [%s]\n", my_servername);
  printf("my_description = [%s]\n", my_description);
  printf("network_name = [%s]\n", network_name);
#ifndef HORRIBLE_DEOPALL_HACK
  printf("o_nick = [%s]\n", o_nick);
  printf("o_login = [%s]\n", o_login);
  printf("o_pass = [%s]\n", o_pass);
  printf("o_userserver = [%s]\n", o_userserver);
#endif /* !HORRIBLE_DEOPALL_HACK */
#endif

  return (0);
}

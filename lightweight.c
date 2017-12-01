/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: lightweight.c,v 1.20 2004/08/18 16:07:50 froo Exp $
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

/* global defines */
int serverfd;
char *nextline; /* The next line to be parsed in chunkbuf. */
char *currentline;  /* The next line to be parsed through ParseLine. */
char *chunkbufentry;  /* Pointer to where we should start reading data in next time. */
char chunkbuf[CHUNKSIZE]; /* Buffer to read data into from the serverfd. */
int burst_done; /* Whether I have finished bursting from my server */
int global_enable = 1; /* enable commands from users */
time_t startuptime;
int memoryused;

/* See datastructures.txt for an explanation of the following. */
int servermaxusers[4096];
struct user **usertablepointer[4096];
struct account *accounthashtable[HASHMAX];
struct user *nickhashtable[HASHMAX];
struct reggedchannel *channelhashtable[HASHMAX];
struct server *servertree[4096];

char MyServer[4]; /* The numeric of the server we connect to. */

/* do not change here, use lightweight.conf instead. */
char server_port[128] = "4400";
char server_host[128] = "127.0.0.7";
char server_pass[128] = "m00L00";
char my_nick[NICKLEN] = "L";
char my_numeric[32] = "[l";
char my_servername[128] = "change.this.in.lightweight.conf";
char my_description[256] = "Drink Milk. Now fatfree.";
char network_name[32] = "QuakeNet";

#ifndef HORRIBLE_DEOPALL_HACK
char o_nick[NICKLEN] = "O";
char o_login[128] = "L";
char o_pass[128] = "secretpw";
char o_userserver[256] = "o@change.this.in.lightweight.conf";
struct user *Optr;
#endif

static char usage[] = "usage: lightweight [-c configfile]";

int main(int argc, char **argv)
{
  int ch;
  char configfile[256];
  extern int optind, opterr;
  extern char *optarg;

  time_t next_hourly; /* When to next do our hourly duties */
  time_t next_daily;  /* When to next do our daily duties */
  time_t seconds_since_midnight;

  startuptime = time(NULL);
  burst_done = 0;
  memoryused = 0;

  serverfd = 0;
  chunkbufentry = chunkbuf;
  nextline = chunkbuf;

  next_hourly = time(NULL) + 3600;
  next_daily = time(NULL) + (86400 - (time(NULL) % 86400));

  a_strcpy(configfile, CONFIGFILE, sizeof(configfile));
  while ((ch = getopt(argc, argv, "c:")) != -1) {
    switch ((char) ch) {
    case 'c':
      a_strcpy(configfile, optarg, sizeof(configfile));
      break;
    case '?':
    default:
      puts(usage);
      exit(0);
    }
  }

  LoadCfg(configfile);

  if (!RotateLogs()) {
    fprintf(stderr, "Failed to rotate logs.\n");
    return (1);
  }

  RestoreDBFromDisk();

  if (!ConnectToServer()) {
    fprintf(stderr, "Failed to connect to %s:%s.\n", server_host, server_port);
    return (1);
  }
  SendInitToServer();

  while (ReadChunk()) {
    /* Only put the time check in the outer loop -
       this should make burst performance a little better */
    if (time(NULL) > next_hourly) {
      next_hourly = time(NULL) + 3600;
      Error(ERR_INFO | ERR_INTERNAL, "Performing hourly tasks.");
      /* Put all hourly tasks here */
      FlushLogs();
      SaveDBToDisk();
    }
    if (time(NULL) > next_daily) {
      seconds_since_midnight = time(NULL) % 86400;
      if (seconds_since_midnight > 79200) { /* Allow for two hours skew. Perhaps this should be 12 instead? */
        /* If the clock has been set backwards we need to adjust. */
        next_daily = time(NULL) + (172800 - seconds_since_midnight);
      } else {
        next_daily = time(NULL) + (86400 - seconds_since_midnight);
      }
      Error(ERR_INFO | ERR_INTERNAL, "Performing daily tasks.");
      /* Put all daily tasks here */
      if (!RotateLogs()) {
        fprintf(stderr, "Failed to rotate logs.\n");
        break;
      }
    }
    while (GetLineFromChunk()) {
      ParseLine();
    }
  }

  SaveDBToDisk();

  return 0;

}

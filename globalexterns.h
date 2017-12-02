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

/* $Id: globalexterns.h,v 1.15 2004/08/18 16:07:50 froo Exp $ */

#ifndef __GLOBALEXTERNS_H
#define __GLOBALEXTERNS_H

#ifdef __cplusplus
extern "C" {
#endif

/* global defines */
  extern int serverfd;
  extern char *nextline;  /* The next line to be parsed in chunkbuf. */
  extern char *currentline; /* The next line to be parsed through ParseLine. */
  extern char *chunkbufentry;
  extern char chunkbuf[CHUNKSIZE];
  extern int servermaxusers[4096];
  extern struct user **usertablepointer[4096];
  extern struct account *accounthashtable[HASHMAX];
  extern struct user *nickhashtable[HASHMAX];
  extern struct reggedchannel *channelhashtable[HASHMAX];
  extern struct server *servertree[4096];
  extern time_t startuptime;
  extern int memoryused;

  extern int burst_done;
  extern int global_enable;

  extern char MyServer[4];

  extern char server_port[128];
  extern char server_host[128];
  extern char server_pass[128];
  extern char my_nick[NICKLEN];
  extern char my_numeric[32];
  extern char my_servername[128];
  extern char my_description[256];
  extern char network_name[32];

#ifndef HORRIBLE_DEOPALL_HACK
  extern char o_nick[NICKLEN];
  extern char o_login[128];
  extern char o_pass[128];
  extern char o_userserver[256];
  extern struct user *Optr;
#endif

#ifdef __cplusplus
}
#endif
#endif /* __GLOBALEXTERN_H */

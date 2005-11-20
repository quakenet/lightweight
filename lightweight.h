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

/* $Id: lightweight.h,v 1.64 2004/08/18 16:07:50 froo Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <strcmp.h>

#ifndef __LIGHTWEIGHT_H
#define __LIGHTWEIGHT_H

#ifdef __cplusplus
extern "C" {
#endif

/* path and filename of config file */
#define CONFIGFILE "lightweight.conf"

/* define this for debugging output */
#define DEBUG

/* define this for CM support */
#define HAVE_CLEARMODE

/* define this for DEOPALL hack */
#undef HORRIBLE_DEOPALL_HACK

/* define this for L on channels */
#define SIT_ON_CHANNELS

/* define this for modes in burst */
/* Note that this is NOT sensible in conjunction with SIT_ON_CHANNELS */
#undef AUTOMODES_IN_BURST

/* number of days before expiring unused channels */
#define DAYS_BEFORE_EXPIRE      20

/* number of days before removing suspended channels */
#define SUSPEND_EXPIRY_TIME     365

/* Internal defines. */

/* Size of the buffer that we read into from our serverfd. */
#define CHUNKSIZE 65536
/* Maxchannels that we allow a user to have flags on. */
#define CHANNELSPERUSER 20
/* Maxusers that we allow to have flags on a channel. */
#define USERSPERCHANNEL 30
/* We need a hashfunction that returns a value x, 0 <= x < HASHMAX. */
#define HASHMAX 65000
/* Define TOPICSAVE to enable lightweight to save topics. */
#undef TOPICSAVE

/* Defines for saving data. */
/* Files will be named accounts.0, accounts.1, ... accounts.x, with number 0 being the newest. */
#define ACCOUNTFILE "accounts"
#define NUMBEROFSAVEFILES 9
/* Files will be named log.0, log.1, ... log.x, with number 0 being the newest. */
/* Logs will be rotated once a day, at midnight. */
#define LOGFILE "log"
#define DEBUGFILE "debug"
#define NUMBEROFLOGFILES 7

/* Defines from ircd. */
/* Things fuck up if these arent exactly as the ircd-settings. */
#define NICKLEN 15
#define SERVERNAMELENGTH 100
#define TOPICLEN 250

/* Structs. */

/* the struct required to hold multiple auths for a single account */
  struct usersauthed {
    struct user *autheduser;  /* pointer to user authed with this account */
    struct usersauthed *nextinlist; /* next in linked list */
  };

  struct account {
    char authname[NICKLEN + 1]; /* Auth name: matches the broadcast ACCOUNT name */
    struct usersauthed *currentuser;  /* User that is currently authed with
                                         this account. */
    struct reggedchannel *channel[CHANNELSPERUSER]; /* Pointers to channels
                                                     * where this account is
                                                     * known.
                                                     */
    char channelflags[CHANNELSPERUSER]; /* Flags corresponding to the channels
                                         * above. */
    time_t lastauth;  /* last time the user authed */
    unsigned char authlevel;  /* Authlevel, more commands will be available for people
                                 * with higher authlevels. */
    struct account *nextbyauthname; /* Next in linked list of hashed authnames. */
  };

/* Struct to hold a registered channel in lightweight. */
/* Please note that this has nothing to do with the channels actually on the
 * network, this is only the registered channels. */
  struct reggedchannel {
    char channelname[30];
    struct account *channeluser[USERSPERCHANNEL]; /* Pointers back to the users
                                                   * that have flags on this
                                                   * channel. Needed when doing
                                                   * a chanlev for the channel.
                                                   */
    struct reggedchannel *nextbychannelname;  /* Next in linked list of hashed
                                               * channelnames.
                                               */
    struct account *founder;  /* the user who founded the channel (first
                                 +n) */
    char addedby[NICKLEN + 1];  /* the user who added the channel */
    time_t lastused;  /* Timestamp for determining unused channels. */
    time_t timestamp; /* Current timestamp for channel */
    time_t dateadded; /* Timestamp for when the channel was added */
    struct account *suspendby;  /* User who suspended the channel */
    char *suspendreason;  /* Suspend reason */
    char *welcome;  /* welcome message */
    char flags; /* Channel flags */
#ifdef TOPICSAVE
    char topic[250];  /* For saving topic in a channel. */
#endif
  };

/* User struct, holds a user currently online on the network. */
  struct user {
    char nick[NICKLEN + 1];
    char numeric[5 + 1];
    struct account *authedas; /* If the user is authed, this is a pointer
                                 * to the corresponding account.
                               */
    char oper;  /* 0 if non oper, non-zero if oper ('o'). */
    struct user *nextbynick;  /* Next in linked list of hashed nicks. */
  };

/* Serverstruct, holds information about a server in the servertree. */
  struct server {
    char servername[SERVERNAMELENGTH];
    int parent;
    /* This is a 'pointer' to the parent of this server, eg. the
     * server that is one hop closer to us on the path from us
     * to the server in the servertree.
     * 'Pointer' in the meaning that this is the entry in the
     * servertree array which holds the pointer to the parent
     * struct.
     */
    int maxusers; /* Maxusers of this server. */
    /* This is really a duplicate, there is a global var for this too,
     * where should it be?
     */
  };

/* Error types */

#define ERR_DEBUG        0x0100
#define ERR_INFO         0x0200
#define ERR_WARNING      0x0400
#define ERR_ERROR        0x0800
#define ERR_FATAL        0x1000

#define ERR_TYPEMASK     0x00ff
#define ERR_PROTOCOL     0x0001
#define ERR_INTERNAL     0x0002
#define ERR_STATS        0x0003
#define ERR_LOADSAVE     0x0004

/* Forward declarations. */
  int RestoreDBFromDisk();
  int ConnectToServer();
  int SendInitToServer();
  int ReadChunk();
  int GetLineFromChunk();
  int ParseLine();
  int SaveDBToDisk();
  char *StripBlanks(char *paddedstring);
  char *SeperateWord(char *sentence);

/* a_string.c */
  char *a_strstrip(char *str);
  char *a_strcpy(char *dst, const char *src, int dst_len);
  char *a_strextract(char *dst, const char *src, int dst_len);
  int a_strmatch(const char *str, const char *reg, int case_sensitive);

/* allocate.c */
  struct server *GetServer();
  void FreeServer(struct server *);
  struct user *GetUser();
  void FreeUser(struct user *);

/* general.c */
  int IsCharInString(char chr, char *string);
  long NumericToLong(const char *numeric, int numericlen);
  char *StripBlanks(char *paddedstring);
  char *SeperateWord(char *sentence);
  int CheckAuthLevel(struct user *user_ptr, unsigned char level);
  void MessageToUser(struct user *user, char *message, ...);
  void NoticeToUser(struct user *user, char *message, ...);

/* nick.c */
  void UserNickCreation(char *numeric, char *nick, char isoper);
  void UserNickChange(char *numeric, char *newnick);
  void DeleteUser(char *numeric);
  void UserHashStats(void);

/* connecttoserver.c */
  void SendLine(char *text);

/* join.c */
  void DoJoins(char *numeric, char *channellist, time_t timestamp);

/* mode.c */
  void DoChanMode(char *sender, char *channel, char *modes);

/* server.c */
  void AddServer(int servernum, const char *servername, int maxusers, int parent);
  void HandleSquit(char *servername);
  void DoSquitRecursive(int servernum);

/* privmsg.c */
  void ProcessMessage(char *numeric, char *text);

/* error.c */
  void Error(int ErrorType, char *reason, ...);

/* hash.c */
  int hash(char *buffer);

/* userdb.c */
  void RemoveAuthFromUser(struct user *); /* removes the auth from account */

/* log.c */
  void FlushLogs(void);
  int RotateLogs(void);
  void Log(char *logtext, ...);
  void DebugLog(char *logtext, ...);

/* readconfig.c */
  int LoadCfg(char *configfile);

/* clientcommands/<command>.c */

  void doaddchan(struct user *user, char *tail);
  void dodelchan(struct user *user, char *tail);
  void dochangelev(struct user *user, char *tail);
  void dosave(struct user *user, char *tail);
  void dohelp(struct user *user, char *tail);
  void doshowcommands(struct user *, char *tail);
  void dowhois(struct user *user, char *tail);
  void dowhois(struct user *user, char *tail);
  void dowhoami(struct user *user, char *tail);
  void dounbanall(struct user *user, char *tail);
  void doclearchan(struct user *user, char *tail);
  void doop(struct user *user, char *tail);
  void dovoice(struct user *user, char *tail);
  void dodeopall(struct user *user, char *tail);
  void dochanlev(struct user *user, char *tail);
  void doinvite(struct user *, char *);
  void doadduser(struct user *, char *);
  void doremoveuser(struct user *, char *);
  void dorecover(struct user *, char *);
  void doversion(struct user *, char *);
  void dodeluser(struct user *, char *);
  void dosuspend(struct user *, char *);
  void dounsuspend(struct user *, char *);
  void dochanstat(struct user *, char *);
  void dostatus(struct user *, char *);
  void dosuspendlist(struct user *, char *);
  void dochannellist(struct user *, char *);
  void doaccountlist(struct user *, char *);
  void dolistlevel(struct user *, char *);
  void douserlist(struct user *, char *);
  void doneigh(struct user *, char *);
  void docow(struct user *, char *);
  void donoticeme(struct user *, char *);
  void dofish(struct user *, char *);
  void doreauth(struct user *, char *);
  void doraw(struct user *, char *);
  void docleanupdb(struct user *, char *);
  void docleanupdb2(struct user *, char *);
  void dodie(struct user *, char *);
  void dowelcome(struct user *, char *);
  void dogrep(struct user *, char *);
  void dofind(struct user *, char *);
  void dosetinvite(struct user *, char *);
  void doclearinvite(struct user *, char *);
  void doaccountstats(struct user *);
  void dopart(struct user *, char *);
  void dosendchanlev(struct user *, char *);
  void dorequestowner(struct user *, char *);
  void dosetenable(struct user *, char *);
#ifdef __cplusplus
}
#endif
#endif /* __LIGHTWEIGHT_H */

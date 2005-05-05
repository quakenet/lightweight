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

/* $Id: accountsdb.h,v 1.22 2002/07/02 16:38:53 zarjazz Exp $ */

#ifndef __ACCOUNTSDB_H
#define __ACCOUNTSDB_H

#include <lightweight.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUTOMODE_UNREG -1
#define AUTOMODE_OP 1
#define AUTOMODE_VOICE 2
#define AUTOMODE_NOAUTOFLAGS 3
#define AUTOMODE_NOTHING 0

#define CFLAG_AUTO   0x01
#define CFLAG_BAN    0x02 /* not implemented.. */
#define CFLAG_MASTER 0x04
#define CFLAG_OWNER  0x08
#define CFLAG_OP     0x10
#define CFLAG_TOPIC  0x20 /* not implemented.. */
#define CFLAG_VOICE  0x40
#define CFLAG_GIVE   0x80

  enum { cflag_a, cflag_b, cflag_m, cflag_n, cflag_o, cflag_t, cflag_v, cflag_g };

  char getchanflag(char chanflag, char flag_ptr);

  struct account *GetAccountByAuthName(char *); /* returns the account struct* for char* authname, NULL if failed */
  struct account *AddAccount(char *); /* adds an account, char* nick, returns NULL if failed, otherwise the account struct* */
  struct account *FindAccount(char *);  /* find the account for nick or #nick */

  struct account *RemoveAccount(struct account *);  /* removes account by authname*, returns authname if succesfull, NULL otherwise */
  char *ChangeAuthName(struct account *, char *); /* oper only, changes the authname, error resistive */
  void SetAuthLevel(struct account *, unsigned char); /* oper only, sets the new authlevel */
  short int IsAdmin(struct user *); /* sees if the user is an oper with high authlevel */

  /* both of these functions add the channel to both the channel struct aswell as the account struct */
  struct reggedchannel *RemoveChannelFromAccount(struct account *, struct reggedchannel *); /* removes a channel from account, returns the parameter in success, NULL on failure */
  struct reggedchannel *AddChannelToAccount(struct account *, struct reggedchannel *);  /* adds a channel to an account, returns the parameter in success, NULL on failure */
  struct reggedchannel **GetChannelUserEntry(struct reggedchannel *, struct account *);
  short int GetUserChannelIndex(struct reggedchannel *, struct account *);  /* gets the index of the channel in user account */
  /* use channel flags stuff */
  short int AutoModes(char *, char *);  /* char* numeric, char* channelname, tells possible automodes for user */
  void DoAllAutoModes(struct user *); /* Gives a user all his automodes now */
  void PrintCFlags(char * /* char[8] */ , char);  /* prints to a string with leading spaces */
  char GetChanFlag(char, char); /* gets a certain chanflag, char chanflag, char userchannelflags */
  char GetChannelFlags(struct account *act_ptr, struct reggedchannel *chn_ptr);
  char GetChannelFlag(struct account *, struct reggedchannel *, char);  /* checks for certain chanflag */
  char GetUserChannelFlags(struct account *, struct reggedchannel *); /* gets the chanflag byte */
  char SetChanFlag(char, char, char *); /* sets a certain chanflag, char chanflag, char set/unset (non-zero is set), char * userchannelflags */
  /* sets the char* string of flags (with +/-) to the user, string must end with NULL */
  void SetUserChannelFlags(struct account *, struct reggedchannel *, char *);
  void CleanUserReferences(struct account *act_ptr);

#ifdef __cplusplus
}
#endif
#endif /* __ACCOUNTSDB_H */

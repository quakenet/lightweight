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

/* $Id: channelsdb.h,v 1.21 2003/09/08 01:19:25 zarjazz Exp $ */

#ifndef __CHANNELSDB_H
#define __CHANNELSDB_H

#ifdef __cplusplus
extern "C" {
#endif

  extern struct reggedchannel *channelhashtable[HASHMAX];

  struct reggedchannel *AddChannel(char *name, struct account *owner, char *by, int lastused, struct user *user); /* char * name, struct account * owner, NULL if not used, char* addedby */
  struct reggedchannel *RemoveChannel(struct reggedchannel *);
  struct reggedchannel *GetChannelPointer(char *);  /* gets the channel pointer based on channel name */
  struct account **GetUserChannelEntry(struct reggedchannel *, struct account *); /* gets the useraccounts entry on the channel users */
  void SetChannelSuspend(struct account *, struct reggedchannel *, int, char *);  /* sets the channel suspend */

#define CHANFLAG_JOINED      0x01
#define CHANFLAG_SUSPENDED   0x02
#define CHANFLAG_BITCH       0x04
#define CHANFLAG_PROTECT     0x08
#define CHANFLAG_FORCETOPIC  0x10
#define CHANFLAG_WELCOME     0x20
#define CHANFLAG_INVITEONLY  0x40

#define IsJoined(x)        ((x->flags) & CHANFLAG_JOINED)
#define IsSuspended(x)     ((x->flags) & CHANFLAG_SUSPENDED)
#define IsWelcomeFlaged(x) ((x->flags) & CHANFLAG_WELCOME)
#define IsInviteOnly(x)    ((x->flags) & CHANFLAG_INVITEONLY)

#define SetJoined(x)       ((x->flags) |= CHANFLAG_JOINED)
#define SetSuspended(x)    ((x->flags) |= CHANFLAG_SUSPENDED)
#define SetWelcomeFlag(x)  ((x->flags) |= CHANFLAG_WELCOME)
#define SetInviteOnly(x)   ((x->flags) |= CHANFLAG_INVITEONLY)

#define ClearJoined(x)     ((x->flags) &= ~CHANFLAG_JOINED)
#define ClearSuspended(x)  ((x->flags) &= ~CHANFLAG_SUSPENDED)
#define ClearWelcomeFlag(x) ((x->flags) &= ~CHANFLAG_WELCOME)
#define ClearInviteOnly(x) ((x->flags) &= ~CHANFLAG_INVITEONLY)

#ifdef TOPICSAVE
  void SetChannelTopic(struct reggedchannel *, char *);
  char *GetChannelTopic(struct reggedchannel *);
#endif

#ifdef __cplusplus
}
#endif
#endif /* __CHANNELSDB_H */

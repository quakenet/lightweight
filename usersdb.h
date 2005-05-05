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

/* $Id: usersdb.h,v 1.5 2002/03/18 09:39:22 zarjazz Exp $ */

#ifndef __USERDB_H
#define __USERDB_H

#ifdef __cplusplus
extern "C" {
#endif

  extern struct user *nickhashtable[HASHMAX];
  extern struct user **usertablepointer[4096];

  struct user *FindUserByNick(char *);
  struct user *FindUserByNumeric(char *);
  struct authedusers *FindUserByAuthname(char *);

  struct user **AddServerToDB(char *);  /* adds a server, numeric id needed with clients as maxclients */
  void RemoveServerToDB(char *);  /* removes a server, numeric id needed */

  void AddUser(char *, char *, char); /* Adds a nick to the network */
  void RemoveUser(char *);  /* removes a user from the network, requires numeric user code */
  void OperUser(char *);  /* marks user as oper, needs numeric user code */
  void DeOperUser(char *);  /* should the oper set -o, this is needed */
  void ChangeNick(char *, char *);  /* for NICK event, char* numeric, char* newnick */

/* numeric, authname */
  struct account *AuthUser(char *, char *); /* auths user, returns the account if succesfull, NULL on failure */

#ifdef __cplusplus
}
#endif
#endif /* __USERDB_H */

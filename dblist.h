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

/* $Id: dblist.h,v 1.9 2002/06/03 21:35:20 froo Exp $ */

#ifndef __DBLIST_H
#define __DBLIST_H

#ifdef __cplusplus
extern "C" {
#endif

/* GetWhois(value,NULL) == whoami */
/* char* is the username the whois is needed, everything is handled by the function */
  void GetWhois(struct user *, char *);
  void PrintChanlev(struct user *, char *, char *); /* prints the chanlev for user, char* channame */
  void ChangeChanlev(struct user *, char *, char *, char *);  /* who, #chan, whom, what */
  void AddToChan(struct user *, char *, char *);  /* adds a user to the channel */
  void RemoveFromChan(struct user *, char *, char *); /* removes a user from the channel */

#ifdef __cplusplus
}
#endif
#endif /* __DBLIST_H */

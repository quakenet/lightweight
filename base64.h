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

/* $Id: base64.h,v 1.4 2002/03/18 09:39:21 zarjazz Exp $ */

/* All the functions required to deal with the ircu p10 base64 numeric user codes */

/* I tested them and they should work just fine, if you however think that something
   is broken, too slow etc. mail me -strutsi (strutsi@help.quakenet.org) */

#ifndef __BASE64_H
#define __BASE64_H

#ifdef __cplusplus
extern "C" {
#endif

  /* returns the server index for numeric id char[5] */
  short int getserverindex(char *);

  /* returns the client index for numeric id char[5] */
  long int getclientindex(char *);

  /* creates the numeric id, (char[5],serverindex,clientindex) */
  void createnumeric(char *, short int, long int);

#ifdef __cplusplus
}
#endif
#endif /* __BASE64_H */

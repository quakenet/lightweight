/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: quit.c,v 1.5 2002/03/18 09:29:54 zarjazz Exp $
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

/*
  - FindUserByNumeric
  - Hash nick
  - Find nick in nickhashtable[hash] linked list and remember user_before
  - if user_before == NULL
    then
      nickhashtable[hash] = user->next
    else
      user_before->next = user->next
  - (*(usertablepointer[n]) + i*sizeof(void *)) = NULL
  - user->authedas->currentuser = NULL
  - Deallocate user, remember to clear as we will reuse it.
*/

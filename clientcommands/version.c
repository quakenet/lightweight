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
#include <channelsdb.h>
#include <accountsdb.h>

/* version.c */

/*
 * version: Tells the user the current bot version
 * 
 * Parameters:
 *  none
 *
 */

void doversion(struct user *usr_ptr, char *tail)
{
  NoticeToUser(usr_ptr, "L (minimalistic channel service for ircu P10 protocol)");
  NoticeToUser(usr_ptr, "L Version 1.00  %s", __DATE__);
  NoticeToUser(usr_ptr, "Copyright: Rasmus Have, David Mansell, Raimo Nikkilä and Fredrik Söderblom");
}

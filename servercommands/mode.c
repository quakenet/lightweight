/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: mode.c,v 1.4 2003/09/08 01:19:25 zarjazz Exp $
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
#include <channelsdb.h>

/*
  - if mode includes +o or +O
    then
      FindUserByNumeric
      set flags = +o/+O
  - if mode includes -o or -O
    then
      FindUserByNumeric
      set flags = -o/-O
  - Any other modes we need to worry about, +r perhaps?
  - All +r's needing worry are recieved in bursts, eg. NICK msg's.
  - Need to react on -i channelmode on invite only channels.
*/

void DoChanMode(char *sender, char *channel, char *modes)
{
  char buf[513];
  int dir = 0;
  char *nextmode; /* Pointer to next modeflag in the modes flagstring. */
  int finalmode = 0;  /* After the entire modestring has been parsed, is the outcome that the invite flag has been removed (1) or has it been set (0). */
  struct reggedchannel *chanptr;

  SeperateWord(modes);          /* We dont care about targets and arguments for modes, such as users and keys. */

  chanptr = GetChannelPointer(channel);

  if (chanptr == NULL) {
    /* Not one of our channels. Sod it. */
    return;
  }

  if (IsSuspended(chanptr))
    /* Bla, bla, bla. Dont give a shit. */
    return;

  /* This is a channel we care about, see if we need to run through the flags. */
  if (!IsInviteOnly(chanptr))
    return;

  /* Its a channel we care about and its set to be invite only. Make sure smartasses dont remove the +i flag. */
  nextmode = modes;
  while (*nextmode != '\0') {
    switch (*nextmode) {
    case '+':
      dir = 1;
      break;
    case '-':
      dir = 0;
      break;
    case 'i':
      if (dir == 0) {
        finalmode = 1;
      } else {
        finalmode = 0;
      }
      break;
    }
    nextmode++;
  }
  if (finalmode == 1) {
    /* Inviteflag has been removed, reset it. */
    sprintf(buf, "%sAAA M %s +i\r\n", my_numeric, chanptr->channelname);
    SendLine(buf);
  }
}

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

#include <limits.h>
#include <sys/types.h>

/* Strcmp and Strncmp case insensitive with {} -> [] */
#ifndef __STRCMP
#define __STRCMP

/*
 * Tables used for translation and classification macros
 */
extern const char ToLowerTab_8859_1[];
extern const char ToUpperTab_8859_1[];
extern const unsigned int IRCD_CharAttrTab[];

/*
 * Translation macros for channel name case translation
 * NOTE: Channel names are supposed to be lower case insensitive for
 * ISO 8859-1 character sets.
 */
#define ToLower(c)        (ToLowerTab_8859_1[(c) - CHAR_MIN])
#define ToUpper(c)        (ToUpperTab_8859_1[(c) - CHAR_MIN])

int Strcmp(const char *, const char *);
int Strncmp(const char *, const char *, int);

#endif

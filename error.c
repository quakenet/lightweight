/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: error.c,v 1.8 2002/03/26 00:47:25 zarjazz Exp $
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

#include <stdarg.h>

/* Try and put all error conditions through one place */
char *ErrorSeverity(int ErrorType)
{
  if (ErrorType & ERR_FATAL)
    return "FATAL ERROR";
  if (ErrorType & ERR_ERROR)
    return "ERROR";
  if (ErrorType & ERR_WARNING)
    return "Warning";
  if (ErrorType & ERR_INFO)
    return "Info";
  if (ErrorType & ERR_DEBUG)
    return "Debug";

  return "Unknown Error";
}

char *ErrorString(int ErrorType)
{
  ErrorType &= ERR_TYPEMASK;
  if (ErrorType == ERR_PROTOCOL)
    return "Protocol";
  if (ErrorType == ERR_INTERNAL)
    return "Internal";
  if (ErrorType == ERR_STATS)
    return "Statistics";
  if (ErrorType == ERR_LOADSAVE)
    return "Load/Save";

  return "Unknown Error";
}

void Error(int ErrorType, char *reason, ...)
{
  char reasonbuf[512];
  va_list val;

  va_start(val, reason);
  vsnprintf(reasonbuf, 511, reason, val);
  va_end(val);

  fprintf(stderr, "%s: %s: %s\n", ErrorSeverity(ErrorType), ErrorString(ErrorType), reasonbuf);

  if (ErrorType & ERR_FATAL)
    exit(1);
}

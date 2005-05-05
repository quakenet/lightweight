/*******************************************************************************
 *
 * lightweight - a minimalistic chanserv for ircu's p10-protocol
 *
 * copyright 2002 by Rasmus Have & David Mansell
 *
 * $Id: log.c,v 1.7 2002/07/02 16:38:53 zarjazz Exp $
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
#include <accountsdb.h>
#include <channelsdb.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

FILE *logfile = NULL;
FILE *debugfile = NULL;

void FlushLogs(void)
{
  fflush(logfile);
  fflush(debugfile);
}

int RotateLogs(void)
{
  /* This functions rotates the logfiles. */
  int i;  /* Loopcounter. */
  char srcfile[256];  /* Source and destination filenames. */
  char dstfile[256];  /* How long should these be? */

  /* Close the logfile so we can rotate and create a new number zero file. */
  if (logfile) {
    fclose(logfile);
    fclose(debugfile);
  }

  /* Delete the oldest logfile. */
  snprintf(dstfile, 256, "%s.%i", LOGFILE, NUMBEROFLOGFILES);
  unlink(dstfile);

  snprintf(dstfile, 256, "%s.%i", DEBUGFILE, NUMBEROFLOGFILES);
  unlink(dstfile);

  /* Backup the old files. */
  for (i = NUMBEROFLOGFILES; i > 0; i--) {
    snprintf(srcfile, 256, "%s.%i", LOGFILE, i - 1);
    snprintf(dstfile, 256, "%s.%i", LOGFILE, i);
    rename(srcfile, dstfile);

    snprintf(srcfile, 256, "%s.%i", DEBUGFILE, i - 1);
    snprintf(dstfile, 256, "%s.%i", DEBUGFILE, i);
    rename(srcfile, dstfile);
  }

  /* Try to open the new logfile. */
  snprintf(srcfile, 256, "%s.%i", LOGFILE, 0);
  logfile = fopen(srcfile, "w");
  if (!logfile) {
    Error(ERR_ERROR | ERR_LOADSAVE, "Couldn't open the log file '%s' for writing", srcfile);
    return (0);
  }

  /* Try to open the new debug logfile. */
  snprintf(srcfile, 256, "%s.%i", DEBUGFILE, 0);
  debugfile = fopen(srcfile, "w");
  if (!debugfile) {
    Error(ERR_ERROR | ERR_LOADSAVE, "Couldn't open the debug log file '%s' for writing", srcfile);
    return (0);
  }

  Log("Logfiles created.");

  return 1;
}

void Log(char *logtext, ...)
{
  char textbuf[512];
  va_list val;

  va_start(val, logtext);
  vsnprintf(textbuf, 511, logtext, val);
  va_end(val);

  fprintf(logfile, "%i: %s\n", (int) time(NULL), textbuf);

#ifdef DEBUG
  fflush(logfile);
#endif
}

void DebugLog(char *logtext, ...)
{
  char textbuf[512];
  va_list val;

  va_start(val, logtext);
  vsnprintf(textbuf, 511, logtext, val);
  va_end(val);

  fprintf(debugfile, "%i: %s\n", (int) time(NULL), textbuf);

#ifdef DEBUG
  fflush(debugfile);
#endif
}

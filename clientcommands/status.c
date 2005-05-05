/* status.c */

#include <lightweight.h>
#include <globalexterns.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void dostatus(struct user *user, char *tail)
{
  struct user *usrptr;
  struct account *actptr;
  struct reggedchannel *chanptr;
  int i, x, j, users;
  int maxchain, used, currentchain, total;
  int authed = 0;
  time_t uptime;
  int day, hr, min, sec;
  char *chptr;

  /* yeah, i know. but in the future we might allow bigger chanlevs :) /froo */
  int sizes[19] = { 1, 2, 3, 4, 5, 6, 8, 10, 15, 20, 25, 31, 50, 100, 150, 200, 250, 300, 100000 };
  int lbuckets[19];

  char filename[256];
  char currentline[1024];
  FILE *statusfile;

  uptime = time(NULL) - startuptime;

  sec = uptime % 60;
  min = (uptime / 60) % 60;
  hr = (uptime / 3600) % 24;
  day = (uptime / (3600 * 24));

  NoticeToUser(user, "Startup time: %s", ctime(&startuptime));
  NoticeToUser(user, "Uptime      : %dd %02d:%02d:%02d", day, hr, min, sec);
  NoticeToUser(user, "User/channel statistics:");

  for (i = 0, maxchain = 0, used = 0, total = 0; i < HASHMAX; i++) {
    usrptr = nickhashtable[i];
    if (usrptr != NULL) {
      used++;
      for (currentchain = 0; usrptr; usrptr = usrptr->nextbynick) {
        if (usrptr->authedas != NULL)
          authed++;
        currentchain++;
        total++;
      }
      if (currentchain > maxchain)
        maxchain = currentchain;
    }
  }

  NoticeToUser(user, "  Active users    : %6d (HASH %6d/%6d, chain %d)", total, used, HASHMAX, maxchain);
  NoticeToUser(user, "  Authed users    : %6d (%.2f%%)", authed, ((float) authed / total) * 100);

  for (i = 0, maxchain = 0, used = 0, total = 0; i < HASHMAX; i++) {
    actptr = accounthashtable[i];
    if (actptr != NULL) {
      used++;
      for (currentchain = 0; actptr; actptr = actptr->nextbyauthname) {
        currentchain++;
        total++;
      }
      if (currentchain > maxchain)
        maxchain = currentchain;
    }
  }

  NoticeToUser(user, "  Registered users: %6d (HASH %6d/%6d, chain %d)", total, used, HASHMAX, maxchain);

  for (i = 0, maxchain = 0, used = 0, total = 0; i < HASHMAX; i++) {
    chanptr = channelhashtable[i];
    if (chanptr != NULL) {
      used++;
      for (currentchain = 0; chanptr; chanptr = chanptr->nextbychannelname) {
        currentchain++;
        total++;
      }
      if (currentchain > maxchain)
        maxchain = currentchain;
    }
  }

  NoticeToUser(user, "  Active channels : %6d (HASH %6d/%6d, chain %d)", total, used, HASHMAX, maxchain);

  /* channel histogram (heavily based on splidge's S code. /froo) */
  for (i = 0; i < 19; i++)
    lbuckets[i] = 0;

  for (i = 0; i < HASHMAX; i++) {
    chanptr = channelhashtable[i];
    while (NULL != chanptr) {   /* go through the list */

      users = 0;
      for (x = 0; x < USERSPERCHANNEL; x++)
        if (NULL != chanptr->channeluser[x])
          users++;

      for (j = 0; users >= sizes[j]; j++) /* cumulative score */
        lbuckets[j]++;

      chanptr = chanptr->nextbychannelname; /* next in list */
    }
  }

  NoticeToUser(user, "Channel histogram");
  NoticeToUser(user, "   Size Range  InRnge Range+");

  for (i = 0; i < 18; i++) {
    if (lbuckets[i]) {
      NoticeToUser(user, "  %4d - %-5d %6d %6d", sizes[i], sizes[i + 1] - 1, lbuckets[i] - lbuckets[i + 1],
                   lbuckets[i]);
    }
  }

  NoticeToUser(user, "Memory usage information:");

  /* Open the status file. */
  sprintf(filename, "/proc/%d/status", getpid());
  statusfile = fopen(filename, "r");
  /* Read the lines in it one at a time. */
  while (!feof(statusfile)) {
    /* If we havent reached EOF yet, we should read a line and check it out. */
    fgets(currentline, 1024, statusfile);
    /* Check if the line we just read was totally cool. */
    if (strncmp(currentline, "VmSize:", 7) == 0 || strncmp(currentline, "VmRSS:", 6) == 0) {
      /* If it is, throw it at the user. */
      /* BUG: the currentline contains a newline in the end, it should be stripped. */
      /* BUG2: line contains embedded TAB's */
      /* FIX: Code monkey to the rescue! */
      for (chptr = currentline; *chptr; chptr++) {
        if (*chptr == '\n' || *chptr == '\r')
          *chptr = '\0';
        if (*chptr == '\t')
          *chptr = ' ';
	  }
      NoticeToUser(user, "  %s", currentline);
    }
  }
  fclose(statusfile);

  NoticeToUser(user, "  Malloced:%7i kB", (int) memoryused / 1024);
}

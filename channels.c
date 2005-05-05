/* channels.c -- general channel functions */

#include <lightweight.h>
#include <globalexterns.h>
#include <stdio.h>
#include <channelsdb.h>
#include <channels.h>

/* 
 * SyncTimestamp
 *
 * Given a new timestamp for a channel, works out if it's "better" 
 * than ours.  Handles appropriately if we're bursting or whatever.
 *
 * Also, if we are sitting on channels, this function should cause 
 * us to join the channel if we haven't already.
 */

void SyncTimestamp(struct reggedchannel *chanptr, time_t timestamp)
{
  char buf[512];

  /* Check if we need to do anything */
  if (chanptr == NULL)          /* Abort if it's NULL */
    return;

  if ((chanptr->timestamp != 0) && (chanptr->timestamp <= timestamp))
    /* Our timestamp is older/same and valid.  Forget it. */
    /* Note that if we're sitting on channels we MUST be
     * on the chan if we need to be already to have a valid timestamp */
    return;

  /* ONLY WRITE TO chanptr->timestamp HERE!! */
  chanptr->timestamp = timestamp;

#ifdef SIT_ON_CHANNELS
  if (!IsSuspended(chanptr)) {
    if (IsJoined(chanptr)) {
      /* We are already on the channel, hack mode */
      HackOps(chanptr);
    } else {
      /* We are joining the channel */
      if (burst_done == 0) {
        /* We are still bursting, we can join seamlessly */
        sprintf(buf, "%s B %s %ld %sAAA:o\r\n", my_numeric, chanptr->channelname, timestamp, my_numeric);
        SendLine(buf);
      } else {
#ifdef HORRIBLE_DEOPALL_HACK
        /* we are still under burst; do the same as above anyway :) */
        sprintf(buf, "%s B %s %ld %sAAA:o\r\n", my_numeric, chanptr->channelname, timestamp, my_numeric);
        SendLine(buf);
#else
        /* Sitting on channels but being nice with the protocol?  Need to join and hack mode */
        CheckJoined(chanptr);
#endif
      }                         /* if (burst_done ... */
      SetJoined(chanptr);
    }                           /* if (IsJoined ... */
  }                             /* if (IsSuspended ... */
#endif /* SIT_ON_CHANNELS */
}

void HackOps(struct reggedchannel *chanptr)
{
  /* We do this so often, bring it out into a function */
  char buf[512];

  sprintf(buf, "%s M %s +o %sAAA\r\n", my_numeric, chanptr->channelname, my_numeric);
  SendLine(buf);
}

void OpUser(struct reggedchannel *chanptr, char *numeric)
{
  /* Op user on channel */
  char buf[512];

#ifdef SIT_ON_CHANNELS
  CheckJoined(chanptr);
  sprintf(buf, "%sAAA M %s +o %s\r\n", my_numeric, chanptr->channelname, numeric);
  SendLine(buf);
#else
  sprintf(buf, "%s M %s +o %s\r\n", my_numeric, chanptr->channelname, numeric);
  SendLine(buf);
#endif
}

void VoiceUser(struct reggedchannel *chanptr, char *numeric)
{
  /* Voice user on channel */
  char buf[512];

#ifdef SIT_ON_CHANNELS
  CheckJoined(chanptr);
  sprintf(buf, "%sAAA M %s +v %s\r\n", my_numeric, chanptr->channelname, numeric);
  SendLine(buf);
#else
  sprintf(buf, "%s M %s +v %s\r\n", my_numeric, chanptr->channelname, numeric);
  SendLine(buf);
#endif
}

void VoiceAndDeopUser(struct reggedchannel *chanptr, char *numeric)
{
  /* Voice user on channel */
  char buf[512];

#ifdef SIT_ON_CHANNELS
  CheckJoined(chanptr);
  sprintf(buf, "%sAAA M %s -o+v %s %s\r\n", my_numeric, chanptr->channelname, numeric, numeric);
  SendLine(buf);
#else
  sprintf(buf, "%s M %s -o+v %s %s\r\n", my_numeric, chanptr->channelname, numeric, numeric);
  SendLine(buf);
#endif
}

void DeopUser(struct reggedchannel *chanptr, char *numeric)
{
  /* Deop user on channel */
  char buf[512];

#ifdef SIT_ON_CHANNELS
  CheckJoined(chanptr);
  sprintf(buf, "%sAAA M %s -o %s\r\n", my_numeric, chanptr->channelname, numeric);
  SendLine(buf);
#else
  sprintf(buf, "%s M %s -o %s\r\n", my_numeric, chanptr->channelname, numeric);
  SendLine(buf);
#endif
}

void KickUser(struct reggedchannel *chanptr, char *nick, char *reason)
{
  /* Deop user on channel */
  char buf[512];

#ifdef SIT_ON_CHANNELS
  CheckJoined(chanptr);
  sprintf(buf, "%sAAA K %s %s :%s\r\n", my_numeric, chanptr->channelname, nick, reason);
  SendLine(buf);
#else
  sprintf(buf, "%s K %s %s :%s\r\n", my_numeric, chanptr->channelname, nick, reason);
  SendLine(buf);
#endif
}

#ifdef SIT_ON_CHANNELS

void InviteUser(struct reggedchannel *chanptr, char *nick)
{
  /* invite user to channel */
  char buf[512];

  CheckJoined(chanptr);
  sprintf(buf, "%sAAA I %s :%s\r\n", my_numeric, nick, chanptr->channelname);
  SendLine(buf);
}

void CheckJoined(struct reggedchannel *chanptr)
{
  /* Check we're joined to a channel */
  char buf[512];

  if (!IsJoined(chanptr) && !IsSuspended(chanptr)) {
    sprintf(buf, "%sAAA J %s\r\n", my_numeric, chanptr->channelname);
    SendLine(buf);
    HackOps(chanptr);
    SetJoined(chanptr);
    if (IsInviteOnly(chanptr)) {
      sprintf(buf, "%sAAA M %s +isn\r\n", my_numeric, chanptr->channelname);
      SendLine(buf);
    }
  }
}

void PartChannel(struct reggedchannel *chanptr, char *reason)
{
  char buf[512];

  if (IsJoined(chanptr)) {
    sprintf(buf, "%sAAA L %s :%s\r\n", my_numeric, chanptr->channelname,
      (reason != NULL) ? reason : "So long, and thanks for all the fish.");
    SendLine(buf);
    ClearJoined(chanptr);
    /* Clear the timestamp.  We are trusting the person telling us to part 
     * that we are the last user in the channel and that the channel will now cease to exist. */
    chanptr->timestamp = 0;
  }
}

#endif

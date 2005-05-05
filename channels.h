
/* channels.h */

void SyncTimestamp(struct reggedchannel *chanptr, time_t timestamp);
void HackOps(struct reggedchannel *chanptr);
void OpUser(struct reggedchannel *chanptr, char *numeric);
void VoiceUser(struct reggedchannel *chanptr, char *numeric);
void VoiceAndDeopUser(struct reggedchannel *chanptr, char *numeric);
void DeopUser(struct reggedchannel *chanptr, char *numeric);
void InviteUser(struct reggedchannel *chanptr, char *nick);
void KickUser(struct reggedchannel *chanptr, char *nick, char *reason);

#ifdef SIT_ON_CHANNELS
void CheckJoined(struct reggedchannel *chanptr);
void PartChannel(struct reggedchannel *chanptr, char *reason);
#else
#define CheckJoined(x)  0
#define PartChannel(x, y)  0
#endif

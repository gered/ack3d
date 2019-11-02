/* Header file to accompany ACKSND.C */

#define DEV_NOSOUND		0
#define DEV_PCSPEAKER		1
#define DEV_SOUNDBLASTER	2
#define DEV_ADLIB		3

#define SOUND_WALKING		0
#define SOUND_HITWALL		1
#define SOUND_HITOBJECT		2
#define SOUND_HITBADOBJECT	3
#define SOUND_DOOROPENING	4
#define SOUND_SECRETDOOROPENING 5
#define SOUND_FIRING		6
#define SOUND_DOORCLOSING	7
#define SOUND_LOST		8
#define SOUND_WON		9
#define SOUND_INTRO		10
#define SOUND_EXPLODE		11
#define SOUND_USER1		12
#define SOUND_USER2		13
#define SOUND_USER3		14
#define SOUND_USER4		15

#define SOUND_MAX_INDEX		15


short AckSoundInitialize(short DefaultSoundDevice);
short AckPlayBackground(char *MusicFile);
void AckPlaySound(short SoundIndex);
void AckSoundShutdown(void);
void AckStopBackground(void);
short AckLoadSound(short VocIndex,char *VocFileName);


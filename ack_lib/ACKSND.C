/******************* ( Animation Construction Kit 3D ) ***********************/
/* Sound Routines: play CMF, VOC files in background on SB, Adlib or speaker */
/* CopyRight (c) 1993  Authors: Lary Myers & Frank Sachse		     */
/*****************************************************************************/

//***************************** WARNING **************************************
// NOTE: The Worx sound functions have NOT been converted to flat model.
// **** DO NOT USE THESE FUNCTIONS!!!! ****
//***************************** WARNING **************************************



#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <mem.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys\stat.h>

#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

#include "acksnd.h"
#include "worx.h"

static short SoundDevice;

void AckPlayNoSound(short index);
void AckPlayPCSpeaker(short index);
void AckPlayAdlib(short index);
void AckPlaySoundBlaster(short index);
short AckPlayMusic(char *MusicFile);

short IRQ; /* SB */
	    char    *Cmf;
	    char    *VocTable[SOUND_MAX_INDEX+1];


/****************************************************************************
** Call into here by the application before using any of the sound routines**
** The default sound device can be used as an overide, such as when the	   **
** application or user doesn't want any sound.                             **
****************************************************************************/
short AckSoundInitialize(short DefaultSoundDevice)
{

if (DefaultSoundDevice == DEV_NOSOUND)
    {
    SoundDevice = DefaultSoundDevice;
    return(0);
    }

/* Checks for sound board & sets SoundDevice to DEV_ define in ACKSND.H */

/* for use only with WORX lib, not WORXlite...
StartWorx();
*/

StartWorx();

if (DefaultSoundDevice == DEV_PCSPEAKER)
    {
    SoundDevice = DefaultSoundDevice;
    return(0);
    }

if (AdlibDetect())
    {
    SoundDevice = DEV_ADLIB;
    SetFMVolume(0xf,0xf);
    }
else
    SoundDevice = DEV_PCSPEAKER;

if (DefaultSoundDevice == DEV_ADLIB)
    {
    SoundDevice = DEV_ADLIB;
    return(0);
    }

IRQ = DSPReset();
if (IRQ > 0)
    {
    SoundDevice = DEV_SOUNDBLASTER;
    SetMasterVolume(0xf,0xf);
    SetVOCVolume(0xf,0xf);
    }

#if 0
/* for use only with WORX lib, not WORXlite...
IRQ = DSPReset() - if > 0, then SB present
if (WorxPresent())  /* use only with WORXlite */
    {
    SoundDevice = DEV_SOUNDBLASTER;
    SetMasterVolume(0xf,0xf);
    SetVOCVolume(0xf,0xf);
    }
*/
#endif

return(0);
}

/****************************************************************************
** Here the application can call in and load the VOC files that it needs   **
**									   **
****************************************************************************/
short AckLoadSound(short VocIndex,char *VocFile)
{
    char    buf[81];

if (SoundDevice == DEV_NOSOUND)
    return(0);

if (VocIndex < 0 || VocIndex > SOUND_MAX_INDEX)
    return(-1);	 /* index out of range */

strcpy(buf,VocFile);
strtok(buf,".");

switch (SoundDevice)
    {
    case DEV_SOUNDBLASTER:
	strcat(buf,".VOC");  /* force extension */
	break;

    case DEV_ADLIB:  /* adlib can't play voc's -> put thru pc spkr */
    case DEV_PCSPEAKER:
    #if 0
	strcat(buf,".PWM");  /* force extension */
    #endif
	strcat(buf,".VOC");  /* force extension */
	break;

    default:
	return(-2);  /* Error if unknown device */
    }

VocTable[VocIndex] = LoadOneShot(buf);	/* load voc/pwm into mem */

if (VocTable[VocIndex] == NULL)
    return(-2);	 /* file not found */

return(0);
}

/****************************************************************************
**									   **
****************************************************************************/
void AckStopBackground(void)
{

switch (SoundDevice)
    {

    case DEV_NOSOUND:
	break;

    case DEV_PCSPEAKER:
	break;

    case DEV_SOUNDBLASTER:
	if (SequencePlaying())
	    StopSequence();
	break;

    case DEV_ADLIB:
	if (SequencePlaying())
	    StopSequence();
	break;

    default:
	break;
    }

}


/****************************************************************************
** The Application would call this routine to begin playing background	   **
** music.								   **
**									   **
****************************************************************************/
short AckPlayBackground(char *MusicFile)
{
    short     result = 0;

switch (SoundDevice)
    {

    case DEV_NOSOUND:
	break;

    case DEV_PCSPEAKER:
	break;

    case DEV_SOUNDBLASTER:
    result = AckPlayMusic(MusicFile);
	break;

    case DEV_ADLIB:
    result = AckPlayMusic(MusicFile);
	break;

    default:
	break;
    }

return(result);
}


/****************************************************************************
** Start the music file playing in this routine.			   **
**									   **
****************************************************************************/
short AckPlayMusic(char *MusicFile)
{
	char *BufPtr;
	char buf[81];

strcpy(buf,MusicFile);
strtok(buf,".");				/* force CMF extention */
strcat(buf,".CMF");

Cmf = GetSequence(buf);			       /* load cmf into mem */

if(Cmf==NULL)
    return(1);

SetLoopMode(1);				     /* set for continuous play */
PlayCMFBlock(Cmf);			      /* play background cmf */

return(0);
}


/****************************************************************************
** Call into here to play a particular sound. The indexes available are	   **
** listed in ACKSND.H which will be included by the application.	   **
**									   **
****************************************************************************/
void AckPlaySound(short SoundIndex)
{

switch (SoundDevice)
    {

    case DEV_NOSOUND:
	break;

    case DEV_PCSPEAKER:
	AckPlayPCSpeaker(SoundIndex);
	break;

    case DEV_SOUNDBLASTER:
	AckPlaySoundBlaster(SoundIndex);
	break;

    case DEV_ADLIB:  /* can't play VOC's on Adlib -> play thru speaker */
	AckPlayPCSpeaker(SoundIndex);
	break;

    default:
	break;
    }

}


/****************************************************************************
** This routine is used for simple speaker sounds. The ones here are for   **
** testing at this point. Better ones would be need to be in the final	   **
** version.								   **
****************************************************************************/
/* I will adjust this later to play VOC's thru speaker x*/
void AckPlayPCSpeaker(short index)
{

if (VocTable[index] == NULL || index < 0 || index > SOUND_MAX_INDEX)
    return;

PlayPWMBlock(VocTable[index]);

}


/****************************************************************************
** Sound Blaster routines go here.					   **
**									   **
**									   **
****************************************************************************/
void AckPlaySoundBlaster(short index)
{

if (VocTable[index] == NULL || index < 0 || index > SOUND_MAX_INDEX)
    return;

PlayVOCBlock(VocTable[index],255);

}


/****************************************************************************
** Call into here by the application before exiting.			   **
****************************************************************************/
void AckSoundShutdown(void)
{

switch (SoundDevice)
    {

    case DEV_NOSOUND:
	break;

    case DEV_PCSPEAKER:
    CloseWorx();
    break;

    case DEV_SOUNDBLASTER:
    if (SequencePlaying())
	StopSequence();			     /* stop background CMF, if playing */
    DSPClose();					  /* free Sound Blaster DSP */
    /* for use only with WORX lib, not WORXlite...
    CloseWorx();
    */
    CloseWorx();
	break;

    case DEV_ADLIB:
    if (SequencePlaying())
	StopSequence();
    /* for use only with WORX lib, not WORXlite...
    CloseWorx();
    */
	break;

    default:
	break;
    }

}


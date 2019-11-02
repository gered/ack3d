/*  modplay.h - Tiny MOD Player V2.02 for Watcom C/C++32 and DOS/4GW header file

    Module player for Sound Blaster and compatibles.

    Copyright 1993,94 Carlos Hasan
*/

/******************************************************************************\
 QUICK REFERENCE GUIDE:

 MODPlayModule
 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
 Function:      Start playing a Modulefile.

 Prototype:     int MODPlayModule(Song,Chans,Rate,Port,IRQ,DRQ)

 Parameters:    Song    - Address of the Modulefile Image
                Chans   - Number of channels (1-8)
                Rate    - Mixing speed in Hertz (4kHz-22kHz)
                Port    - I/O Port address (220h..280h)
                IRQ     - IRQ level (2,3,5,7,11)
                DRQ     - DMA channel (0,1,3)

 Returns:       On success, return a zero value.
                On error, return a non zero value.

 Remarks:       This function will initialize the Sound Blaster card and
                will start playing the module file immediately. The module
                file must be a 4,6 or 8 channels Protracker or FastTracker
                music module file.
                The player supports two kinds of channels, music and sample
                audio channels. The music channels are used by the player
                to play the module, and the sample channels are used for
                sound effects and the like.
                The channels voices 0 to N-1 are used for music channels,
                where N is the number of channels of the module file.


 MODStopModule
 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
 Function:      Stop playing the current Modulefile.

 Prototype:     void MODStopModule()

 Parameters:    None.

 Returns:       None.

 Remarks:       This function shut down the playing system. Must be called
                before exiting the user program.


 MODPlaySample
 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
 Function:      Play instrument at specified period and volume.

 Prototype:     void MODPlaySample(Voice,Instr)

 Parameters:    Voice   - Audio channel number (0-7)
                Instr   - Instrument address

 Returns:       None.

 Remarks:       This function is useful to play samples over music. The sample
                structure holds the period, volume and the address of the 8-bit
                signed samples  to be played in the channel.


 MODPlayVoice
 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
 Function:      Set the sample, period and volume for a sample channel.

 Prototype:     void MODPlayVoice(Voice,Instr,Period,Volume)

 Parameters:    Voice   - Audio channel number (0-7)
                Instr   - Instrument number (1-31)
                Period  - Amiga period value (113-856)
                Volume  - Volume value (0-64)

 Returns:       None.

 Remarks:       This function is useful to play samples over music. You must
                use a sample channel or the music system will override
                these parameters when polling the current module.
                The amiga period value can be translated to hertz using the
                following formula:  Hertz = 8363*428/Period


 MODStopVoice
 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
 Function:      Stop a sample channel.

 Prototype:     void MODStopVoice(Voice)

 Parameters:    Voice   - Audio channel number (0-7)

 Remarks:       This function will stop the specified voice setting the channel
                volume to zero. The voice should be a sample channel.


 MODSetPeriod
 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
 Function:      Set the sample channel period value.

 Prototype:     void MODSetPeriod(Voice,Period)

 Parameters:    Voice   - Audio channel number (0-7)
                Period  - Amiga Period (113-856)

 Returns:       None.

 Remarks:       This function will change the current frequency of the sample
                channel. The voice should be a sample channel.


 MODSetVolume
 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
 Function:      Set the sample channel volume.

 Prototype:     void MODSetVolume(Voice,Volume)

 Parameters:    Voice   - Audio channel number (0-7)
                Volume  - Volume (0-64)

 Returns:       None.

 Remarks:       This function will change the channel volume. The voice should
                be a sample channel.
\******************************************************************************/

#ifndef __MODPLAY_H
#define __MODPLAY_H

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

typedef struct {
    word    Period;
    word    Volume;
    dword   Length;
    void    *Data;
} Sample;

#pragma aux MODPlayModule "_*" parm caller [];
#pragma aux MODStopModule "_*" parm caller [];
#pragma aux MODPlaySample "_*" parm caller [];
#pragma aux MODPlayVoice  "_*" parm caller [];
#pragma aux MODStopVoice  "_*" parm caller [];
#pragma aux MODSetPeriod  "_*" parm caller [];
#pragma aux MODSetVolume  "_*" parm caller [];

extern int MODPlayModule(void *Modulefile,word Chans,word Rate,word Port,byte IRQ,byte DRQ);
extern void MODStopModule(void);
extern void MODPlaySample(byte Voice,Sample *Instr);
extern void MODPlayVoice(byte Voice,byte Instr,word Period,byte Volume);
extern void MODStopVoice(byte Voice);
extern void MODSetPeriod(byte Voice,word Period);
extern void MODSetVolume(byte Voice,byte Volume);

#endif


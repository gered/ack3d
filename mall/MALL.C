// Quick demo of walking around in a Mall using ACK-3D Kit
// Started: 06/25/94
//  Author: Lary Myers
//  Module: MALL.C
// (c) CopyRight 1994 All Rights Reserved

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include <string.h>
#include <dos.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include "ack3d.h"
#include "ackeng.h"
#include "kit.h"
#include "modplay.h"

#define DEMO_RESOURCE   69
#define HAND1_RESOURCE  135
#define HAND2_RESOURCE  136

#define KEYBD  0x9    /* INTERRUPT 9 */
#define RIGHT_ARROW_KEY     77
#define UP_ARROW_KEY        72
#define LEFT_ARROW_KEY      75
#define DOWN_ARROW_KEY      80
#define MINUS_KEY       74
#define PLUS_KEY        78
#define NUMBER_5_KEY        76
#define ESCAPE_KEY      1
#define PGUP_KEY        73
#define PGDN_KEY        81
#define B_KEY           48
#define C_KEY           46
#define F_KEY           33
#define I_KEY           23
#define R_KEY           19
#define S_KEY           31
#define W_KEY           17
#define NUM_1_KEY       2
#define NUM_2_KEY       3
#define NUM_3_KEY       4
#define NUM_4_KEY       5
#define NUM_5_KEY       6
#define NUM_6_KEY       7
#define NUM_7_KEY       8
#define NUM_8_KEY       9
#define NUM_9_KEY       10

typedef struct {
    int     mdx;
    int     mdy;
    int     mButtons;
} MOUSE;

#define MAX_AMOUNT  64
#define MAX_MAG_AMOUNT  64
#define MAX_STR_AMOUNT  128
#define MAX_STR_HALF_AMOUNT 64

extern  long    mFactor;
extern  long    dFactor;

extern  long    zdTable[VIEW_WIDTH][200];

extern  UCHAR   colordat[];
extern  UINT    *ObjGrid;
extern  UINT    *Grid;
extern  UINT    FloorMap[];
extern  short   ViewHeight;
extern  short   CeilingHeight;
extern  short   Resolution;
extern  long    kduFactor;
extern  long    kdvFactor;
extern  long    kxFactor;
extern  long    kyFactor;
extern  short   tuFactor;
extern  short   tvFactor;
extern  short   rsHandle;
extern  ULONG   *rbaTable;
extern  UCHAR   *BackArray[];
extern  long    *xNextTable;
extern  long    *yNextTable;
extern  UINT    *Grid;
extern  long    *CosTable;
extern  long    *SinTable;

    ACKENG  *ae;

// These are the ranges used for distance shading. Will need to be modified
// for the new color palette used.
ColorRange  ranges[64] = {
    16,15,
    32,16,
    48,16,
    64,16,
    80,16,
    96,8,
    104,8,
    112,8,
    120,8,
    128,8,
    136,8,
    144,8,
    152,8,
    160,8,
    168,8,
    176,8,
    184,16,
    200,16,
    216,16,
    232,16,
    248,16,
    0,0
    };



    UCHAR   scanCode;
    UCHAR   KeyPressed;
    UCHAR   MiniKey;
    UCHAR   Keys[128];
    int     HaveMouse;
    short   FlashIndex;
    short   FlashCount;
    UCHAR   *bmFlash[2];
    UCHAR   *bmFlash1[2];
    char    LineBuffer[200];
    char    *smFont;
    UCHAR   FontTransparent;
    UCHAR   FontColor;
    UCHAR   TextBGcolor;
    int     Throwing;
    int     Shooting;
    int     ShutDownFlag;
    short   MagAmount;
    short   StrAmount;
    int     ShowHitFlag;
    int     MapResource;
    int     PalResource;
    int     ResScreenBack;
    int     ResScrollBack;
    void    *BGmusic;

    UCHAR   *DemoPtr;
    short   Demoht;
    short   Demowt;
    UCHAR   *pHand1;
    short   Handw1;
    short   Handh1;
    UCHAR   *pHand2;
    short   Handw2;
    short   Handh2;

    long    TimerCounter;
    long    ObjCounter[64];
    void    (__interrupt __far *oldvec)();
    void    __interrupt __far myInt();

    void    (__interrupt __far *oldTimer)();
    void    __interrupt __far myTimer();

    short   LastObjectIndex;

//-----------------------------------------------------------------------------
// Globals used by the frame counting routines
//-----------------------------------------------------------------------------
volatile short framespersec = 0;
volatile short cframes=0, count=0, ticks=0;
volatile short AckTmCount=0, AckTmDelay=0;

    char    *ErrorMsgs[] = {
        "ERR_BADFILE      ",
        "ERR_BADCOMMAND   ",
        "ERR_BADOBJNUMBER ",
        "ERR_BADSYNTAX    ",
        "ERR_LOADINGBITMAP",
        "ERR_BADDIRECTION ",
        "ERR_BADSTARTX    ",
        "ERR_BADSTARTY    ",
        "ERR_BADANGLE     ",
        "ERR_BADMAPFILE   ",
        "ERR_READINGMAP   ",
        "ERR_BADPICNAME   ",
        "ERR_INVALIDFORM  ",
        "ERR_NOPBM        ",
        "ERR_BADPICFILE   ",
        "ERR_NOMEMORY     ",
        "ERR_BADPALFILE   ",
        "ERR_BADWINDOWSIZE",
        "ERR_TOMANYVIEWS  ",
        "ERR_BADOBJECTNUM ",
        "ERR_BADOBJTYPE   "
         };


void AckRegisterStructure(ACKENG *ae);
void AckSpeedUp(short);
void AckSlowDown(void);

//=============================================================================
// Keyboard interrupt 9
//=============================================================================
void __interrupt __far myInt(void)
{
  register char x;

//oldvec();    // Use when screen captures are wanted - calls orig vector

scanCode = inp(0x60); // read keyboard data port
x = inp(0x61);
outp(0x61, (x | 0x80));
outp(0x61, x);
outp(0x20, 0x20);

Keys[scanCode & 127] = 1;
KeyPressed = 1;
if (scanCode & 128)
    {
    Keys[scanCode & 127] = 0;
    KeyPressed = 0;
    }
else
    MiniKey = 1;

}

//=============================================================================
// Timer interrupt - simply increments a counter for use in program
// Calls the old timer after X iterations have cycled so clock stays correct
//=============================================================================
void __interrupt __far myTimer(void)
{
if (ShutDownFlag)
    {
    _enable();
    outp(0x20,0x20);
    return;
    }

TimerCounter++;

AckTmCount++;
if (AckTmCount > AckTmDelay)
    {
    oldTimer();
    AckTmCount -= AckTmDelay;
    }
else
    {
    _enable();
    outp(0x20,0x20);
    }

}

    short   ModSound;       // 0 = Off, 1 = On
    short   ModPort;        // Port of SB card
    short   ModIRQ;     // IRQ number of SB card
    short   ModDMA;     // DMA channel of SB card
    char    ModName[128];   // Filename of MOD file to read

//=============================================================================
//
//=============================================================================
char *StripEndOfLine(char *s)
{
    int     len;
    char    ch;

len = strlen(s);

while (--len >= 0)
    {
    ch = s[len];
    if (ch != ' ' && ch != ';' && ch != '\t' && ch != 13 && ch != 10)
    break;

    s[len] = '\0';
    }

return(s);
}

//=============================================================================
//
//=============================================================================
short atoHex(char *s)
{
    short   value;
    char    ch;

value = 0;

while (*s)
    {
    ch = toupper(*s++);
    if (ch == ' ' || ch == '\t' || ch == ',')
    continue;

    if (ch < '0') break;
    if (ch > 'F') break;
    if (ch > '9' && ch < 'A')
    break;

    value <<= 4;
    if (ch >= 'A') ch -= 7;
    ch -= '0';
    value |= ch;
    }


return(value);
}

//=============================================================================
//
//=============================================================================
void ReadConfigFile(void)
{
    FILE    *fp;
    char    *s;
    char    rBuf[128];

ModSound = 0;
ModPort = 0x220;
ModIRQ = 7;
ModDMA = 1;
strcpy(ModName,"MALL.MOD");

fp = fopen("ack3d.cfg","rt");
if (fp == NULL)
    return;

while (1)
    {
    if (feof(fp))
    break;

    *rBuf = '\0';
    fgets(rBuf,125,fp);
    if (*rBuf == ';')
    continue;

    if (!strnicmp(rBuf,"SOUND:",6))
    {
    s = SkipSpaces(&rBuf[6]);
    if (!strnicmp(s,"ON",2))
        ModSound = 1;
    else
        ModSound = 0;

    continue;
    }

    if (!strnicmp(rBuf,"SBPORT:",7))
    {
    ModPort = atoHex(&rBuf[7]);
    continue;
    }

    if (!strnicmp(rBuf,"SBIRQ:",6))
    {
    ModIRQ = atoi(&rBuf[6]);
    continue;
    }

    if (!strnicmp(rBuf,"SBDMA:",6))
    {
    ModDMA = atoi(&rBuf[6]);
    continue;
    }

    if (!strnicmp(rBuf,"SOUNDFILE:",10))
    {
    s = SkipSpaces(&rBuf[10]);
    strcpy(ModName,s);
    StripEndOfLine(ModName);
    }

    }

}

//=============================================================================
//
//=============================================================================
/* MOD loading routines */
void *MODLoadModule(char *Path)
{
    int handle;
    unsigned modsize;
    void *modfile = NULL;
    if ((handle = open(Path,O_RDONLY | O_BINARY)) != -1) {
    modsize = lseek(handle,0L,SEEK_END);
    lseek(handle,0L,SEEK_SET);
    if ((modfile=(void*)AckMalloc(modsize)) != NULL) {
        if (read(handle,modfile,modsize) != modsize) {
        AckFree(modfile);
        modfile = NULL;
        }
    }
    close(handle);
    }
    return modfile;
}

//=============================================================================
//
//=============================================================================
void MODFreeModule(void *Module)
{
    if (Module) AckFree(Module);
}

//=============================================================================
//
//=============================================================================
short StartBGmusic(void)
{
if (!ModSound)
    return(1);

if ((BGmusic = MODLoadModule(ModName)) == NULL)
    return(-1);

//if (MODPlayModule(BGmusic,5,22000,0x220,7,1))
if (MODPlayModule(BGmusic,5,22000,ModPort,ModIRQ,ModDMA))
    return(-2);

return(0);
}

//=============================================================================
//
//=============================================================================
void EndBGmusic(void)
{

if (!ModSound)
    return;

if (BGmusic == NULL)
    return;

MODStopModule();
MODFreeModule(BGmusic);

}


//=============================================================================
//
//=============================================================================
short LoadSmallFont(void)
{
    short   ht,wt;
    int     len;

ht = 2;
smFont = AckReadiff((UCHAR *)ht);
if (smFont == NULL)
    return(-1);

ht = (*(short *)smFont);
wt = (*(short *)&smFont[2]);
len = ht * wt;
memmove(smFont,&smFont[4],len);

return(0);
}

//=============================================================================
//
//=============================================================================
void smWriteChar(short x,short y,unsigned char ch)
{
        int  FontOffset,VidOffset;
        int  row,col;
        UCHAR    *Video;

VidOffset = (y * 320) + x;
Video = (UCHAR *)0xA0000;
Video += VidOffset;
FontOffset = ((ch-32) * 5);

if (FontTransparent)
    Video = ae->ScreenBuffer + VidOffset;

for (row = 0; row < 5; row++)
    {
    if (!FontTransparent)
    {
    Video[0] = TextBGcolor;
    Video[1] = TextBGcolor;
    Video[2] = TextBGcolor;
    Video[3] = TextBGcolor;
    }

    if (smFont[FontOffset])
    Video[0] = FontColor;
    if (smFont[FontOffset+1])
    Video[1] = FontColor;
    if (smFont[FontOffset+2])
    Video[2] = FontColor;
    if (smFont[FontOffset+3])
    Video[3] = FontColor;

    Video += 320;
    FontOffset += 294;
    }


}

//=============================================================================
//
//=============================================================================
short smWriteString(short x,short y,char *s)
{
    short   OrgX;
    char    ch;

OrgX = x;

while (*s)
    {
    ch = *s++;

    if (ch == 10)
    {
    x = OrgX;
    y += 8;
    continue;
    }

    if (ch < ' ')
    continue;

    ch = toupper(ch);
    smWriteChar(x,y,ch);
    x += 5;
    }

return(y);
}

//=============================================================================
//
//=============================================================================
void smWriteHUD(short x,short y,UCHAR color,char *s)
{
FontTransparent = 1;
FontColor = color;
smWriteString(x,y,s);
FontTransparent = 0;
FontColor = 15;
}

//=============================================================================
// Checks mouse movement and calculates delta movement in X and Y directions
//=============================================================================
void CheckMouse(MOUSE *m)
{
    int     dx,dy;
    short   x,y,buttons;


buttons = ReadMouseCursor(&y,&x);
dx = x - 160;
dy = y - 120;
m->mButtons = buttons;
SetMouseCursor(120,160);

if (abs(dy) > 10 && abs(dx) < 32)
    dx >>= 2;

m->mdx = dx;
m->mdy = dy;

}

//=============================================================================
// Reads a text line from the resource file
//=============================================================================
int ReadLine(void)
{
    int     len;
    char    ch;

len = 0;
while (len < 200)
    {
    if (read(rsHandle,&LineBuffer[len],1) != 1)
    break;

    ch = LineBuffer[len];
    if (ch == 10)
    continue;

    if (ch == 13)
    break;

    len++;
    }

LineBuffer[len] = '\0';

return(len);
}

//=============================================================================
// Skips to the next parameter in a text line
//=============================================================================
char *GetNextParm(char *s)
{
    char    ch;

while (*s)
    {
    ch = *s++;
    if (ch == ',')
    {
    while (*s)
        {
        ch = *s;
        if (ch != ',' && ch != ' ' && ch != '\t')
        return(s);
        s++;
        }
    return(NULL);
    }

    }

return(NULL);
}

//=============================================================================
// Loads a wall bitmap specified in info file
//=============================================================================
int LoadWall(void)
{
    int     wnum,rnum,result;
    long    pos;
    char    *lb;


lb = LineBuffer;        // Info file buffer
wnum = atoi(lb);        // Wall number to load into

lb = GetNextParm(lb);

if (lb == NULL)
    return(-1);

rnum = atoi(lb);        // Resource number

pos = lseek(rsHandle,0L,SEEK_CUR);
result = AckLoadWall(ae,wnum,(char *)rnum);
lseek(rsHandle,pos,SEEK_SET);

return(result);
}


//=============================================================================
// Loads an object bitmap specified in info file
//=============================================================================
int LoadObject(void)
{
    int     onum,rnum,result;
    long    pos;
    char    *lb;

lb = LineBuffer;
onum = atoi(lb);        // Object bitmap number

lb = GetNextParm(lb);

if (lb == NULL)
    return(-2);

rnum = atoi(lb);        // Resource number
pos = lseek(rsHandle,0L,SEEK_CUR);
result = AckLoadObject(ae,onum,(char *)rnum);
lseek(rsHandle,pos,SEEK_SET);

return(result);
}


//=============================================================================
// Skip any leading spaces in the string
// NOTE: Actually modifies the string passed!
//=============================================================================
char *SkipSpaces(char *s)
{

while (*s == ' ' || *s == '\t' || *s == ',')
    strcpy(s,&s[1]);

return(s);
}


//=============================================================================
// Creates and object of the desired style
//=============================================================================
int CreateObject(void)
{
    int     onum,vnum,speed;
    short   result,oType;
    short   NumViews,bmPerView;
    UINT    flags;
    char    *lb;
    OBJSEQ  os;

lb = LineBuffer;

if (!strnicmp(lb,"NUMBER:",7))
    {
    lb = &lb[7];
    onum = atoi(lb);
    if (onum < 1 || onum > MAX_OBJECTS)
    return(-3);

    result = AckCreateObject(ae,onum);
    if (result)
    return(result);

    LastObjectIndex = onum;
    lb = GetNextParm(lb);
    if (lb == NULL)
    return(-4);

    ae->ObjList[onum]->Speed = atoi(lb);
    return(0);
    }

onum = LastObjectIndex;     // Object number

oType = 0;

if (!strnicmp(lb,"CREATE:",7))
    {
    oType = NO_CREATE;
    lb = &lb[7];
    }

if (!strnicmp(lb,"DESTROY:",8))
    {
    oType = NO_DESTROY;
    lb = &lb[8];
    }

if (!strnicmp(lb,"WALK:",5))
    {
    oType = NO_WALK;
    lb = &lb[5];
    }

if (!strnicmp(lb,"ATTACK:",7))
    {
    oType = NO_ATTACK;
    lb = &lb[7];
    }

if (!strnicmp(lb,"INTERACT:",9))
    {
    oType = NO_INTERACT;
    lb = &lb[9];
    }


if (!oType)
    return(-5);



lb = SkipSpaces(lb);
if (lb == NULL)
    return(-6);

flags = 0;
strupr(lb);

if (strstr(lb,"ANIMATE") != NULL)
    flags |= OF_ANIMATE;

if (strstr(lb,"MOVEABLE") != NULL)
    flags |= OF_MOVEABLE;

if (strstr(lb,"PASSABLE") != NULL)
    flags |= OF_PASSABLE;

if (strstr(lb,"MULTIVIEW") != NULL)
    flags |= OF_MULTIVIEW;

if (strstr(lb,"SHOWONCE") != NULL)
    flags |= OF_ANIMONCE;

lb = GetNextParm(lb);
if (lb == NULL)
    return(-5);

NumViews = atoi(lb);
if (NumViews < 1)
    return(-6);

lb = GetNextParm(lb);
if (lb == NULL)
    return(-7);

bmPerView = atoi(lb);
if (bmPerView < 1)
    return(-7);

vnum = NumViews * bmPerView;
if (vnum > MAX_OBJ_BITMAPS)
    return(-8);

lb = GetNextParm(lb);
if (lb == NULL)
    return(-9);

vnum = 0;

while (lb != NULL && vnum < MAX_OBJ_BITMAPS)
    {
    os.bitmaps[vnum++] = atoi(lb);
    lb = GetNextParm(lb);
    }

os.bmBitmapsPerView = bmPerView;
os.flags = flags;
os.MaxBitmaps = bmPerView;
os.bmSides = NumViews;

result = AckSetupObject(ae,onum,oType,&os);

return(result);
}

//=============================================================================
// Reads the ASCII info file and processes the commands.
//=============================================================================
int ProcessInfoFile(void)
{
    int     result;
    int     mode;
    long    pos;
    char    *lb;


// Position to start of info file within resource file
lseek(rsHandle,rbaTable[0],SEEK_SET);

mode = result = 0;

while (!result)
    {
    if (!ReadLine())
    continue;

    if (*LineBuffer == ';')
    continue;

    if (!strnicmp(LineBuffer,"END:",4))
    break;

    printf(".");

    switch (mode)
    {

    case 1:     // Read walls
        if (!strnicmp(LineBuffer,"LOADTYPE:",9))
        {
        ae->bmLoadType = atoi(&LineBuffer[9]);  // Sets for GIF or BBM
        break;
        }

        if (!strnicmp(LineBuffer,"ENDBITMAPS:",11))
        mode = 4;
        else
        result = LoadWall();
        break;

    case 2:     // Object bitmaps
        if (!strnicmp(LineBuffer,"LOADTYPE:",9))    // Sets for GIF or BBM
        {
        ae->bmLoadType = atoi(&LineBuffer[9]);
        break;
        }
        if (!strnicmp(LineBuffer,"ENDBITMAPS:",11))
        mode = 5;
        else
        result = LoadObject();
        break;

    case 3:     // Create Object
        if (!strnicmp(LineBuffer,"ENDDESC:",8))
        mode = 5;
        else
        result = CreateObject();
        break;

    case 4:     // Walls topic
        if (!strnicmp(LineBuffer,"BITMAPS:",8))
        mode = 1;

        if (!strnicmp(LineBuffer,"ENDWALLS:",9))
        mode = 0;
        break;


    case 5:     // Objects topic
        if (!strnicmp(LineBuffer,"BITMAPS:",8))
        mode = 2;

        if (!strnicmp(LineBuffer,"OBJDESC:",8))
        mode = 3;

        if (!strnicmp(LineBuffer,"ENDOBJECTS:",11))
        mode = 0;
        break;


    default:
        if (!strnicmp(LineBuffer,"WALLS:",6))
        {
        mode = 4;
        break;
        }

        if (!strnicmp(LineBuffer,"OBJECTS:",8))
        {
        mode = 5;
        break;
        }

        if (!strnicmp(LineBuffer,"MAPFILE:",8))
        {
        MapResource = atoi(&LineBuffer[8]);
        pos = lseek(rsHandle,0L,SEEK_CUR);
        result = AckReadMapFile(ae,(char *)MapResource);
        lseek(rsHandle,pos,SEEK_SET);
        break;
        }

        if (!strnicmp(LineBuffer,"PALFILE:",8))
        {
        PalResource = atoi(&LineBuffer[8]);
        break;
        }

        if (!strnicmp(LineBuffer,"XPLAYER:",8))
        {
        ae->xPlayer = atoi(&LineBuffer[8]);
        break;
        }

        if (!strnicmp(LineBuffer,"YPLAYER:",8))
        {
        ae->yPlayer = atoi(&LineBuffer[8]);
        break;
        }

        if (!strnicmp(LineBuffer,"PLAYERANGLE:",12))
        {
        ae->PlayerAngle = atoi(&LineBuffer[12]);
        break;
        }

        if (!strnicmp(LineBuffer,"SCREENBACK:",11))
        {
        ResScreenBack = atoi(&LineBuffer[11]);
        break;
        }

        if (!strnicmp(LineBuffer,"SCROLLBACK:",11))
        {
        ResScrollBack = atoi(&LineBuffer[11]);
        break;
        }

        if (!strnicmp(LineBuffer,"TOPCOLOR:",9))
        {
        ae->TopColor = atoi(&LineBuffer[9]);
        break;
        }

        if (!strnicmp(LineBuffer,"BOTTOMCOLOR:",12))
        {
        ae->BottomColor = atoi(&LineBuffer[12]);
        break;
        }

        if (!strnicmp(LineBuffer,"SHADING:",8))
        {
        strupr(LineBuffer);
        if (strstr(&LineBuffer[8],"OFF") != NULL)
            ae->LightFlag = SHADING_OFF;
        else
            ae->LightFlag = SHADING_ON;
        break;
        }

        if (!strnicmp(LineBuffer,"FLOORS:",7))
        {
        strupr(LineBuffer);
        if (strstr(&LineBuffer[7],"OFF") != NULL)
            ae->SysFlags |= SYS_SOLID_FLOOR;
        else
            {
            ae->SysFlags &= ~SYS_SOLID_FLOOR;
            }
        break;
        }

        if (!strnicmp(LineBuffer,"RESOLUTION:",11))
        {
        Resolution = atoi(&LineBuffer[11]);
        break;
        }

        if (!strnicmp(LineBuffer,"LOADTYPE:",9))
        {
        ae->bmLoadType = atoi(&LineBuffer[9]);  // Sets for GIF or BBM
        break;
        }

        break;

    }

    }

if (!result)
    {
    result = AckCreateObject(ae,0); // Create a dummy object for later
    if (!result)
    ae->ObjList[0]->Active = 0;  // Turn off checking the object
    }

printf("done\n");
return(result);
}

//=============================================================================
// Quick routine to display a bitmap into the desired buffer. Handles
// transparent colors. Currently used to display the ACK3D Demo bitmap.
//=============================================================================
void ShowBitmap(short x,short y,UCHAR *dst,short ht,short wt,UCHAR *bm)
{
    int     offset,col;
    short   endy;
    UCHAR   ch;

offset = (y*320) + x;
dst += offset;
endy = ae->WinEndY + 2;

while (ht-- > 0 && y++ <= endy)
    {
    for (col = 0; col < wt; col++)
    {
    ch = *bm++;
    if (ch)
        dst[col] = ch;
    }
    dst += 320;
    }

}

#define FONT_RESOURCE   52

    UCHAR   *Fonts[256];
    short   MaxWt;
    short   MaxHt;
    short   FontSize;
    UCHAR   FontColorTable[16];

//=============================================================================
//
//=============================================================================
void LoadFont(void)
{
    short   i;
    char    wBuf[30];
    char    *w;


lseek(rsHandle,rbaTable[FONT_RESOURCE],SEEK_SET);

read(rsHandle,wBuf,7);
if (strncmp(wBuf,"GKF",3))
    {
    return;
    }

w = wBuf + 3;
MaxHt = (*(short *)w);
w += 2;
MaxWt = (*(short *)w);
FontSize = MaxHt * MaxWt;

for (i = 0; i < 256; i++)
    {
    Fonts[i] = AckMalloc(FontSize);
    if (Fonts[i] == NULL)
    break;

    read(rsHandle,Fonts[i],FontSize);
    }

}

//=============================================================================
//
//=============================================================================
void WriteChar(short x,short y,char c)
{
    UCHAR   *fPtr,*Video;
    short   offset,row,col;

if (c < ' ' || c > 'z')
    return;

c = toupper(c);
fPtr = Fonts[c];
Video = (UCHAR *)0xA0000;
offset = (y * 320) + x;
Video += offset;

for (row = 0; row < MaxHt; row++)
    {
    for (col = 0; col < MaxWt; col++)
    Video[col] = FontColorTable[*fPtr++];

    Video += 320;
    }

}

//=============================================================================
//
//=============================================================================
void WriteString(short x,short y,char *s)
{
    short   cx,cy;
    char    ch;

cx = x;
cy = y;

while (*s)
    {
    ch = *s++;
    if (ch == 13)
    {
    x = cx;
    cy += MaxHt;
    continue;
    }

    if (ch < ' ')
    continue;

    WriteChar(x,y,ch);
    x += MaxWt;
    }

}

//=============================================================================
//
//=============================================================================
UCHAR GetPoint(short x,short y)
{
    short   offset;
    UCHAR   *Video;

offset = (y * 320) + x;
Video = (UCHAR *)0xA0000;

return(Video[offset]);

}

//=============================================================================
//
//=============================================================================
void CenterString(short y,char *s)
{
    short   x,len;

len = strlen(s) * MaxWt;

x = (320 - len) / 2;
WriteString(x,y,s);

}

//=============================================================================
//
//=============================================================================
short Initialize(void)
{
    short     i,bnum;

if (MouseInstalled() != -1)
    {
    printf("Mouse is required to run.\n");
    return(1);
    }

ae = AckMalloc(sizeof(ACKENG));
if (ae == NULL)
    {
    printf("Unable to get required memory.\n");
    return(2);
    }

memset(ae,0,sizeof(ACKENG));

// ae->WinStartX = 0;
// ae->WinEndX = 319;
// ae->WinStartY = 0;
// ae->WinEndY = 158;

ae->WinStartX = 20;
ae->WinEndX = 299;
ae->WinStartY = 14;
ae->WinEndY = 156;


ae->LightFlag = SHADING_ON;
ae->xPlayer = 192;
ae->yPlayer = 640;
ae->PlayerAngle = 0;
ae->TopColor = 0;
ae->BottomColor = 24;
ae->DoorSpeed = 6;
ae->NonSecretCode = 1;
ae->SysFlags |= SYS_SOLID_BACK;

printf("Initializing.\n");

i = AckOpenResource("kit.ovl");
if (i)
    {
    printf("Unable to open resource KIT.OVL\n");
    return(3);
    }

i = LoadSmallFont();
if (i)
    {
    printf("Error loading font BBM.\n");
    return(5);
    }

i = AckInitialize(ae);

AckCloseResource();

if (i)
    {
    printf("Error initializing. Error code: %d\n",i);
    return(3);
    }

printf("Processing DTF file ");

i = AckOpenResource("pics.dtf");
if (i)
    {
    printf("Unable to open resource PICS.DTF\n");
    return(3);
    }

i = ProcessInfoFile();
if (i)
    {
    printf("Error reading INF file.\n");
    if (i > 100 && i < 121)
    printf("Last error was %s\n",ErrorMsgs[i-100]);
    else
    printf("Error code: %d\n",i);

    printf("Line was: \"%s\"\n",LineBuffer);

    return(4);
    }

bmFlash[0] = ae->bMaps[4];
bmFlash[1] = ae->bMaps[8];
bmFlash1[0] = ae->bMaps[24];
bmFlash1[1] = ae->bMaps[2];
FlashIndex = 0;
LoadFont();

return(0);
}

//=============================================================================
// Loads and displays the full screen picture and sets the palette to the one
// loaded with the picture.
//=============================================================================
short LoadAndShow(char *fName)
{
    short   i,j;
    UINT    pos,begpos;
    UCHAR   pMask;
    UCHAR   *buf,*bPtr;
    UCHAR   *Video;

buf = AckReadiff(fName);
if (buf == NULL)
    return(-1);

Video = (char *)0xA0000;
memmove(Video,&buf[4],64000);

AckSetPalette(colordat);

AckFree(buf);
return(0);
}

//=============================================================================
// Loads a background image (mountains) and processes the image into the
// separate slices for use at display time. Currently a background image
// can be 640 columns wide. This number can be made dynamic if needbe and would
// need to be changed in the routine below and in the DrawBackground routine
// in the ACK engine.
//=============================================================================
int LoadBackDrop(void)
{
    int result;
    int i,j,pos;
    UCHAR   *bPtr;
    UCHAR   *aPtr;

result = 0;

if (ResScrollBack)
    {
    printf("Loading background image....\n");
    bPtr = AckReadiff((char *)ResScrollBack);
    printf("Processing background image.\n");
    if (bPtr != NULL)
    {
    for (i = 0; i < 320; i++)
        {
        pos = i + 4;
        aPtr = BackArray[i];
        for (j = 0; j < 100; j++)
        {
        *aPtr++ = bPtr[pos];
        pos += 320;
        }
        }

    for (i = 320; i < 640; i++)
        {
        pos = (i - 320) + 32004;
        aPtr = BackArray[i];
        for (j = 0; j < 100; j++)
        {
        *aPtr++ = bPtr[pos];
        pos += 320;
        }
        }

    AckFree(bPtr);
    }
    else
    {
    printf("Unable to load background image.\n");
    result = 8;
    }
    }

return(result);
}

//=============================================================================
//
//=============================================================================
void ShowHit(void)
{
    int     x,y,result,wNum;
    int     flag;
    short   oNum,mPos;

ae->ObjList[0]->x = ae->xPlayer;
ae->ObjList[0]->y = ae->yPlayer;

wNum = 32;
oNum = 0;
flag = 0;

while (wNum--)
    {
    result = AckMoveObjectPOV(0,ae->PlayerAngle,12);

    if (result == POV_OBJECT)
    {
    flag = 1;
    oNum = AckGetObjectHit();
    break;
    }

    if (result == POV_XWALL)
    {
    flag = 2;
    oNum = ae->xGrid[AckGetWallHit()];
    if (oNum & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT))
        flag = 4;
    oNum &= 0xFF;
    break;
    }

    if (result == POV_YWALL)
    {
    flag = 3;
    oNum = ae->yGrid[AckGetWallHit()];
    if (oNum & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT))
        flag = 4;
    oNum &= 0xFF;
    break;
    }
    }


if (flag)
    {
    mPos = (ae->ObjList[0]->y & 0xFFC0) + (ae->ObjList[0]->x >> 6);

    switch (flag)
    {

    case 1:
        sprintf(LineBuffer,"Object %d hit\nat location %d",oNum,mPos);
        break;

    case 2:
        sprintf(LineBuffer,"Xwall %d hit\nat location %d",oNum,mPos);
        break;

    case 3:
        sprintf(LineBuffer,"Ywall %d hit\nat location %d",oNum,mPos);
        break;

    case 4:
        sprintf(LineBuffer,"Door %d hit\nat location %d",oNum,mPos);
        break;

    default:
        *LineBuffer = '\0';
        break;
    }

    ShowHitFlag = 1;
    }


}

//=============================================================================
//
//=============================================================================
void UpdateBlast(void)
{
    short   j;

if (ae->ObjList[99]->Flags & OF_ANIMDONE &&
    ae->ObjList[99]->CurrentType != NO_WALK)
    {
    AckSetObjectType(ae,99,NO_WALK);
    ae->ObjList[99]->Flags &= ~OF_ANIMDONE;
    }

j = AckMoveObjectPOV(99,ae->ObjList[99]->Dir,ae->ObjList[99]->Speed);

if (j > 0 && j != POV_PLAYER)
    {
    if (j != POV_OBJECT)
    {
    ae->ObjList[99]->Active = 0;
    Shooting = 0;
    return;
    }

    j = AckGetObjectHit();

    if (j > 4 && j < 13)
    {
    if (ae->ObjList[j]->CurrentType == NO_WALK)
        {
        AckSetObjectType(ae,j,NO_INTERACT);
        ObjCounter[j] = TimerCounter + 18 + (rand() % 120);
        }
    }

    if (j == 51)
    ae->ObjList[j]->Active = 0;

    ae->ObjList[99]->Active = 0;
    Shooting = 0;
    }

}

//=============================================================================
//
//=============================================================================
void CheckMonsters(void)
{
    int     i,tFlag,xp,yp;
    int     pMap,oMap,oRow,oCol;
    int     row,col,offset;
    long    dx,dy,dx2,dy2,dist;
    short   cType;
    UCHAR   oFlags;

xp = ae->xPlayer;
yp = ae->yPlayer;

pMap = (yp & 0xFFC0) + (xp >> 6);

i = 5;
//for (i = 5; i < 13; i++)
while (1)
    {
    if (ae->ObjList[i]->Active)
    {
    oFlags = ae->ObjList[i]->Flags;
    cType = ae->ObjList[i]->CurrentType;
    dx = xp - ae->ObjList[i]->x;
    dy = yp - ae->ObjList[i]->y;

    dx2 = dx * dx;
    dy2 = dy * dy;
    if ((dx2+dy2) < 128000)
        {
        ae->ObjList[i]->Dir = AckGetObjectAngle(dx,dy);
        if (cType == NO_WALK)
        {
        ae->ObjList[i]->Flags |= OF_ANIMATE;
        oFlags |= OF_ANIMATE;
        }

        if (cType == NO_CREATE && !(oFlags & OF_ANIMATE))
        {
        if (i < 50)
            {
            ae->ObjList[i]->Flags |= OF_ANIMATE;
            oFlags |= OF_ANIMATE;
            }
        else
            AckSetObjectType(ae,i,NO_WALK);
        }

        }

    if (cType == NO_WALK && (oFlags & OF_ANIMATE))
        {
        tFlag = AckMoveObjectPOV(i,ae->ObjList[i]->Dir,8);
        oRow = ae->ObjList[i]->Dir / INT_ANGLE_90;
        switch (tFlag)
        {
        case POV_PLAYER:
            StrAmount--;
            if (StrAmount < 0) StrAmount = 0;
            ShowStatus();
            break;

        case POV_NOTHING:
            break;

        case POV_SLIDEX:
            if (oRow > 1) ae->ObjList[i]->Dir = INT_ANGLE_270;
            else ae->ObjList[i]->Dir = INT_ANGLE_90;
            break;

        case POV_SLIDEY:
            if (oRow > 0 && oRow < 3)
            ae->ObjList[i]->Dir = INT_ANGLE_180;
            else
            ae->ObjList[i]->Dir = 0;
            break;

        case POV_OBJECT:
            ae->ObjList[i]->Dir = rand() % INT_ANGLE_360;
            break;

        default:
            if (i < 50)
            ae->ObjList[i]->Flags &= ~OF_ANIMATE;
            break;

        }
        }
    }

    i++;
    if (i == 13) i = 51;
    if (i == 52) break;
    }

}

//=============================================================================
//
//=============================================================================
void ShowStatus(void)
{

#if 0
    UCHAR   *Video;
    int     row,offset,rlen,glen;


offset = (163 * 320) + 34;
glen = MagAmount;
if (glen > MAX_MAG_AMOUNT) glen = MAX_MAG_AMOUNT;
rlen = MAX_MAG_AMOUNT - glen;
Video = (UCHAR *)0xA0000;
Video += offset;

for (row = 0; row < 11; row++)
    {
    if (glen)
    memset(Video,105,glen);
    if (rlen)
    memset(&Video[glen],39,rlen);

    Video += 320;
    }

offset = (183 * 320) + 34;
glen = StrAmount >> 1;
if (glen > MAX_STR_HALF_AMOUNT) glen = MAX_STR_HALF_AMOUNT;
rlen = MAX_STR_HALF_AMOUNT - glen;
Video = (UCHAR *)0xA0000;
Video += offset;

for (row = 0; row < 11; row++)
    {
    if (glen)
    memset(Video,105,glen);
    if (rlen)
    memset(&Video[glen],39,rlen);

    Video += 320;
    }
#endif

}

//=============================================================================
//
//=============================================================================
void DrawBmpBox(int x1,int y1,int x2,int y2,long d1,long d2)
{
    long    NumCols,DeltaDist;
    long    htStep,dst;
    long    ht1,ht2,bmht;
    long    offset,xpos,ypos,bmPos,yposLow;
    long    bmCol,bmColStep;
    UCHAR   *Video,*bmp,*VidOrg,*VidEnd;
    UCHAR   *vPtr;

dst = d1;
if (d1 > d2) dst = d2;

bmht = 64 * 256;
DeltaDist = labs(d2 - d1);
ht1 = bmht / dst;
NumCols = ht1;
if (DeltaDist)
    NumCols = (ht1 * (64 - DeltaDist)) / 64;

if (NumCols < 1) NumCols = 1;

bmColStep = (64*65536) / NumCols;
htStep = (DeltaDist<<16) / NumCols;
if (d2 < d1) htStep = -htStep;

bmp = ae->bMaps[1];
VidOrg = Video = ae->ScreenBuffer;
VidEnd = VidOrg + 64000;
offset = (y1 * 320) + x1;
Video += offset;

bmCol = 0;
d2 = (d1<<16);

for (xpos = 0; xpos < NumCols; xpos++)
    {
    vPtr = Video;
    ypos = 0x1FFF;
    yposLow = 0x1F;
    bmPos = (bmCol>>16) * 64;
    d1 = (d2>>16);

    while (yposLow > 0 && vPtr > VidOrg)
    {
    *vPtr = bmp[bmPos+yposLow];
    vPtr -= 320;
    ypos -= d1;
    yposLow = ypos >> 8;
    }

    vPtr = Video + 320;
    ypos = 0x2000;
    yposLow = 0x20;

    while (yposLow < 64 && vPtr < VidEnd)
    {
    *vPtr = bmp[bmPos+yposLow];
    vPtr += 320;
    ypos += d1;
    yposLow = ypos >> 8;
    }

    d2 += htStep;
    bmCol += bmColStep;
    Video++;
    }


}

//=============================================================================
//
//=============================================================================
void ClsBmpBox(int x1,int y1,int x2,int y2)
{
    int     offset,wt;
    UCHAR   *Video;

Video = (UCHAR *)0xA0000;
offset = (y1 * 320) + x1;
wt = (x2 - x1) + 1;
Video += offset;

while (y1++ <= y2)
    {
    memset(Video,0,wt);
    Video += 320;
    }

}


//=============================================================================
//
//=============================================================================
short main(void)
{
    short   i,j,done,fpos;
    char    ch;
    MOUSE   mouse;
    UCHAR   *Video;
    int     Spin,MoveAmount;
    int     SpinAngle,MoveAngle;
    long    TimerEnd;
    short   DemoFlag;
    short   lpx,lpy,lpa;
    int     StartTime,EndTime,InfoFlag;
    int     CkStart,CkEnd;
    int     TurnFactor,MoveFactor,MoveHalfFactor;
    int     HandX,HandY,HandDY;
    long    x1,y1;

if ((done = Initialize()) != 0)
   return(done);

if ((done = LoadBackDrop()) != 0)
    return(done);

Shooting = 0;
MagAmount = MAX_MAG_AMOUNT;
StrAmount = MAX_STR_AMOUNT;

// Setup keyboard and timer interrupts
oldvec=_dos_getvect(KEYBD);
_dos_setvect(KEYBD,myInt);
oldTimer=_dos_getvect(8);
_dos_setvect(8,myTimer);
AckTmDelay = 3;
AckSpeedUp(AckTmDelay);     // Set the timer interrupt at 3 times normal

ReadConfigFile();
StartBGmusic();

// Switch to mode 13
AckSetVGAmode();

// Put up the main screen
LoadAndShow((char *)ResScreenBack);

// Set palette for shading if needed
AckSetupPalRanges(ae,ranges);

ShowStatus();

done = 0;
SpinAngle = Spin = MoveAmount = 0;
MouseReleased();
SetMouseCursor(120,160);
fpos = 64;
DemoFlag = 0;
//if (DemoPtr != NULL) DemoFlag = 1;
TimerEnd = TimerCounter + 180;


// MUST register each ACKENG structure once before use and after AckInitialize
AckRegisterStructure(ae);

StartTime = TimerCounter;
AckBuildView();
AckDisplayScreen();
FontColorTable[0] = GetPoint(100,10);
CenterString(3,"An ACK-3D Demonstration");
EndTime = TimerCounter - StartTime;

if (!EndTime) EndTime = 1;

TurnFactor = INT_ANGLE_1 * EndTime;
MoveFactor = 3 * EndTime;
MoveHalfFactor = MoveFactor >> 1;

StartTime = clock();
EndTime   = StartTime;
InfoFlag = 0;

HandX = 134;
HandY = 132;
HandDY = -2;
Throwing = 0;

while (!done)
    {

    if (SpinAngle)
    {
    ae->PlayerAngle += SpinAngle;
    if (ae->PlayerAngle >= INT_ANGLE_360)
        ae->PlayerAngle -= INT_ANGLE_360;
    if (ae->PlayerAngle < 0)
        ae->PlayerAngle += INT_ANGLE_360;

    SpinAngle >>= 3;
    if (SpinAngle == -1) SpinAngle = 0;
    }

    if (MoveAmount)
    {

#if 0
    HandY += HandDY;
    if (HandY < 125) HandDY = 2;
    if (HandY > 132) HandDY = -2;
#endif

    j = AckMovePOV(MoveAngle,MoveAmount);
    AckCheckDoorOpen(ae->xPlayer,ae->yPlayer,MoveAngle);
    MoveAmount >>= 1;

#if 0
    if (j == POV_OBJECT)
        {
        j = AckGetObjectHit();

        if (j > 4 && j < 13)
        {
        StrAmount--;
        if (StrAmount < 0)
            StrAmount = 0;

        ShowStatus();
        }

        if (j >= 36 && j <= 49)
        {
        ae->ObjList[j]->Active = 0;
        MagAmount += 4;
        if (MagAmount > MAX_MAG_AMOUNT)
            MagAmount = MAX_MAG_AMOUNT;
        ShowStatus();
        }

        if (j == 50 && ae->ObjList[50]->CurrentType == NO_CREATE)
        {
        AckSetObjectType(ae,j,NO_WALK);
        StrAmount = MAX_STR_AMOUNT;
        ShowStatus();
        }

        }
#endif

    }

#if 0
    else
    {
    if (HandY < 132)
        HandY++;
    }
#endif

    AckCheckObjectMovement();   // Animate objects if needed

    for (j = 5; j < 13; j++)
    {
    if (ae->ObjList[j]->Flags & OF_ANIMDONE &&
        ae->ObjList[j]->CurrentType == NO_DESTROY)
        {
        AckSetObjectType(ae,j,NO_CREATE);
        ae->ObjList[j]->Flags &= ~OF_ANIMDONE;
        ae->ObjList[j]->Flags &= ~OF_ANIMATE;
        ae->ObjList[j]->Active = 0;
        }

    if (ae->ObjList[j]->Flags & OF_ANIMDONE &&
        ae->ObjList[j]->CurrentType != NO_WALK)
        {
        AckSetObjectType(ae,j,NO_WALK);
        ae->ObjList[j]->Flags &= ~OF_ANIMDONE;
        ObjCounter[j] = TimerCounter + 18 + (rand() % 120);
        }



    if (TimerCounter > ObjCounter[j])
        {
        ObjCounter[j] = TimerCounter + 180 + (rand() % 180);
        if (ae->ObjList[j]->CurrentType == NO_WALK)
        AckSetObjectType(ae,j,NO_ATTACK);
        else
        {
        if (ae->ObjList[j]->CurrentType == NO_INTERACT)
            AckSetObjectType(ae,j,NO_DESTROY);

        }
        }
    }

    if (Shooting)
    UpdateBlast();

    CheckMonsters();

    CkStart = TimerCounter;
    AckBuildView(); // Build floor, ceiling, and walls into ScrnBuffer

#if 0
    if (DemoFlag)
    {
    switch (DemoFlag)
        {
        case 1:
        if (TimerCounter > TimerEnd)
            {
            DemoFlag++;
            TimerEnd = TimerCounter + 540;
            }
        break;

        case 2:
        ShowBitmap(130,20,ae->ScreenBuffer,Demoht,Demowt,&DemoPtr[4]);
        if (TimerCounter > TimerEnd)
            {
            DemoFlag = 1;
            TimerEnd = TimerCounter + 2160;
            }
        break;

        default:
        break;
        }

    }

    switch (Throwing)
    {
    case 0:
        ShowBitmap(HandX,HandY,ae->ScreenBuffer,Handh1,Handw1,&pHand1[4]);
        break;

    case 1:
        HandY -= 20;
        if (HandY < 97)
        {
        HandY = 97;
        Throwing++;
        }
        ShowBitmap(HandX,HandY,ae->ScreenBuffer,Handh1,Handw1,&pHand1[4]);
        break;

    case 2:
        HandY -= 5;
        Throwing++;
        ShowBitmap(HandX,HandY,ae->ScreenBuffer,Handh2,Handw2,&pHand2[4]);
        break;

    case 3:
        ae->ObjList[99]->Active = 1;
        ae->ObjList[99]->x = ae->xPlayer;
        ae->ObjList[99]->y = ae->yPlayer;
        ae->ObjList[99]->Dir = ae->PlayerAngle;
        ae->ObjList[99]->mPos = (ae->yPlayer & 0xFFC0) + ae->xPlayer >> 6;
        AckSetObjectType(ae,99,NO_CREATE);
        Shooting = 1;
        MagAmount--;
        ShowStatus();
        Throwing++;
        ShowBitmap(HandX,HandY,ae->ScreenBuffer,Handh2,Handw2,&pHand2[4]);
        break;

    case 4:
        HandY += 5;
        Throwing++;
        ShowBitmap(HandX,HandY,ae->ScreenBuffer,Handh2,Handw2,&pHand2[4]);
        break;

    case 5:
        HandY += 20;
        if (HandY > 132)
        Throwing = 0;
        ShowBitmap(HandX,HandY,ae->ScreenBuffer,Handh1,Handw1,&pHand1[4]);
        break;


    default:
        break;
    }

    if (InfoFlag)
    {
    EndTime = clock();
    if (EndTime > StartTime+100 )
        {
        StartTime = EndTime;
        cframes = framespersec;
        framespersec = 0;
        }

    sprintf(LineBuffer,"FPS: %6ld,%d,%d,%d",
        (long)cframes,ae->xPlayer,ae->yPlayer,ae->PlayerAngle);
    smWriteHUD(10,20,32,LineBuffer);
    framespersec++;
    }

    if (ShowHitFlag)
    smWriteHUD(10,10,32,LineBuffer);
#endif

    AckDisplayScreen();     // Copy ScrnBuffer to actual video
    CkEnd = TimerCounter - CkStart;
    if (!CkEnd) CkEnd = 1;

    TurnFactor = INT_ANGLE_1 * CkEnd;
    MoveFactor = 3 * CkEnd;
    MoveHalfFactor = MoveFactor >> 1;

#if 0
    i = (ae->yPlayer & 0xFFC0) + (ae->xPlayer >> 6);

    if (FloorMap[i] == 0x28)
    {
    StrAmount--;
    if (StrAmount < 1)
        break;
    ShowStatus();
    }
#endif

    CheckMouse(&mouse);

#if 0
    if (mouse.mButtons & 1)
    {
    if (!(ae->ObjList[99]->Active) && MagAmount > 0 && !Throwing)
        {
        Throwing = 1;
    #if 0
        ae->ObjList[99]->Active = 1;
        ae->ObjList[99]->x = ae->xPlayer;
        ae->ObjList[99]->y = ae->yPlayer;
        ae->ObjList[99]->Dir = ae->PlayerAngle;
        ae->ObjList[99]->mPos = (ae->yPlayer & 0xFFC0) + ae->xPlayer >> 6;
        AckSetObjectType(ae,99,NO_CREATE);
        Shooting = 1;
        MagAmount--;
        ShowStatus();
    #endif
        }
    }
#endif

    if (mouse.mButtons & 2)
    {
    MoveAmount += MoveFactor; // 16;
    if (MoveAmount > MAX_AMOUNT)
        MoveAmount = MAX_AMOUNT;
    MoveAngle = ae->PlayerAngle;
    }

    if (mouse.mdx < 0)
    {
    Spin = -mouse.mdx;
    Spin >>= 5;
    SpinAngle = -TurnFactor * Spin; // -INT_ANGLE_1 * Spin;
    Spin = 1;
    }

    if (mouse.mdx > 0)
    {
    Spin = mouse.mdx;
    Spin >>= 5;
    SpinAngle = TurnFactor * Spin; // INT_ANGLE_1 * Spin;
    Spin = 1;
    }

    if (mouse.mdy < 0)
    {
    i = -mouse.mdy;
    i >>= 2;
    i += MoveHalfFactor;
    MoveAmount += i;
    if (MoveAmount > MAX_AMOUNT)
        MoveAmount = MAX_AMOUNT;
    MoveAngle = ae->PlayerAngle;
    }

    if (mouse.mdy > 20)
    {
    i = mouse.mdy;
    i >>= 3;
    i += MoveHalfFactor;
    j = ae->PlayerAngle + INT_ANGLE_180;
    if (j >= INT_ANGLE_360)
        j -= INT_ANGLE_360;

    MoveAmount += i;
    if (MoveAmount > MAX_AMOUNT)
        MoveAmount = MAX_AMOUNT;
    MoveAngle = j;
    }

    if (Keys[ESCAPE_KEY])
    break;

    if(Keys[RIGHT_ARROW_KEY])
    {
    Spin += 1;
    SpinAngle += TurnFactor; // INT_ANGLE_1 * Spin;
    }

    if(Keys[LEFT_ARROW_KEY])
    {
    Spin += 1;
    SpinAngle -= TurnFactor; // -INT_ANGLE_1 * Spin;
    }

    if(Keys[UP_ARROW_KEY])
    {
    MoveAmount += (MoveFactor + MoveHalfFactor); // 12;
    if (MoveAmount > MAX_AMOUNT)
        MoveAmount = MAX_AMOUNT;
    MoveAngle = ae->PlayerAngle;
    }

    if(Keys[DOWN_ARROW_KEY])
    {
    j = ae->PlayerAngle + INT_ANGLE_180;
    if (j >= INT_ANGLE_360)
        j -= INT_ANGLE_360;

    MoveAmount += (MoveFactor + MoveHalfFactor); // 12;
    if (MoveAmount > MAX_AMOUNT)
        MoveAmount = MAX_AMOUNT;
    MoveAngle = j;
    }


#if 0
    if (Keys[C_KEY])
    {
    ae->SysFlags ^= SYS_SOLID_CEIL;
    ae->SysFlags &= ~SYS_SOLID_BACK;
    Keys[C_KEY] = 0;

    if ((ae->SysFlags & SYS_SOLID_CEIL) && (ResScrollBack != 0))
        ae->SysFlags |= SYS_SOLID_BACK;
    AckRegisterStructure(ae);
    }

    if (Keys[R_KEY])
    {
    Keys[R_KEY] = 0;
    Resolution++;
    if (Resolution > 2)
        Resolution = 0;
    }

    if (Keys[F_KEY])
    {
    ae->SysFlags ^= SYS_SOLID_FLOOR;
    Keys[F_KEY] = 0;
    AckRegisterStructure(ae);
    }


    if (Keys[PGUP_KEY] && ViewHeight < 60)
    {
    ViewHeight++;
    CeilingHeight++;
    }

    if (Keys[PGDN_KEY] && ViewHeight > 4)
    {
    ViewHeight--;
    CeilingHeight--;
    }

    if (Keys[NUM_1_KEY])
    {
    Keys[NUM_1_KEY]=0;
    dFactor--;
    }

    if (Keys[NUM_2_KEY])
    {
    Keys[NUM_2_KEY]=0;
    dFactor++;
    }


    if (Keys[MINUS_KEY])
    {
    Keys[MINUS_KEY]=0;
    mFactor--;
    }

    if (Keys[PLUS_KEY])
    {
    Keys[PLUS_KEY]=0;
    mFactor++;
    }

    if (Keys[I_KEY])
    {
    Keys[I_KEY] = 0;
    InfoFlag ^= 1;
    }

    if (Keys[B_KEY])
    {
    Keys[B_KEY]=0;
    mFactor -= 64;
    }

    if (Keys[S_KEY])
    {
    Keys[S_KEY] = 0;
    mFactor += 64;
    }
#endif


    }

EndBGmusic();
ShutDownFlag = 1;
_disable();
AckSlowDown();          // Set the timer back to normal speed
AckWrapUp(ae);
AckSetTextmode();
_dos_setvect(KEYBD,oldvec);
_dos_setvect(8,oldTimer);
_enable();

if (kbhit())
    getch();

return(0);
}


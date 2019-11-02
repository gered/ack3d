// Map Editor for Animation Construction Kit 3D
// Utilities
// Author: Lary Myers
// Copyright (c) 1994

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <dos.h>
#include <mem.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <process.h>
#include <bios.h>
#include <sys\stat.h>
#include "ack3d.h"
#include "ackeng.h"
#include "m1.h"

    char    *smFont;
    char    *mdFont;
    UCHAR   FontUseColor;
    UCHAR   FontTransparent;
    UCHAR   FontColor;
    UCHAR   TextBGcolor;
    UCHAR   ButtonColor;
    UCHAR   ButtonUpperColor;
    UCHAR   ButtonLowerColor;
    UCHAR   BoxColor;

//=============================================================================
//
//=============================================================================
void SoundBeep(void)
{
sound(440);
delay(50);
nosound();
}


//=============================================================================
//
//=============================================================================
short LoadSmallFont(void)
{
    short   ht,wt;
    int     len;

ht = 2;
smFont = AckReadiff((char *)RES_SMFONT);   // "spfont.bbm");
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
short LoadMedFont(void)
{
    short   ht,wt;
    int     len;

ht = 2;
mdFont = AckReadiff((char *)RES_MDFONT);   // "font6x9.bbm");
if (mdFont == NULL)
    return(-1);

ht = (*(short *)mdFont);
wt = (*(short *)&mdFont[2]);
len = ht * wt;
memmove(mdFont,&mdFont[4],len);

return(0);
}

//=============================================================================
//
//=============================================================================
void mdWriteChar(short x,short y,unsigned char ch)
{
        int  FontOffset,VidOffset;
        int  row,col;
        UCHAR    *Video;

VidOffset = (y * 320) + x;
Video = VIDSEG;
Video += VidOffset;
FontOffset = ((ch-32) * 6);

if (ch >= 'A')
    FontOffset += 1386;

for (row = 0; row < 8; row++)
    {
    if (FontUseColor)
    {
    ch = mdFont[FontOffset];
    if (ch == ButtonColor) ch = TextBGcolor;
    Video[0] = ch;
    ch = mdFont[FontOffset+1];
    if (ch == ButtonColor) ch = TextBGcolor;
    Video[1] = ch;
    ch = mdFont[FontOffset+2];
    if (ch == ButtonColor) ch = TextBGcolor;
    Video[2] = ch;
    ch = mdFont[FontOffset+3];
    if (ch == ButtonColor) ch = TextBGcolor;
    Video[3] = ch;
    ch = mdFont[FontOffset+4];
    if (ch == ButtonColor) ch = TextBGcolor;
    Video[4] = ch;
    ch = mdFont[FontOffset+5];
    if (ch == ButtonColor) ch = TextBGcolor;
    Video[5] = ch;
    }
    else
    {
    Video[0] = mdFont[FontOffset];
    Video[1] = mdFont[FontOffset+1];
    Video[2] = mdFont[FontOffset+2];
    Video[3] = mdFont[FontOffset+3];
    Video[4] = mdFont[FontOffset+4];
    Video[5] = mdFont[FontOffset+5];
    }
    Video += 320;
    FontOffset += 198;
    }

}

//=============================================================================
//
//=============================================================================
short mdWriteString(short x,short y,char *s)
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
    y += 10;
    continue;
    }

    if (ch < ' ')
    continue;

    ch = toupper(ch);
    mdWriteChar(x,y,ch);
    x += 6;
    }

return(y);
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
Video = VIDSEG;
Video += VidOffset;
FontOffset = ((ch-32) * 5);

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
//
//=============================================================================
void BlitBlock(int x,int y,int x1,int y1,UCHAR *buf)
{
    int     offset,wt;
    UCHAR   *Video;

offset = (y * 320) + x;
Video = VIDSEG + offset;
buf += offset;
wt = (x1 - x) + 1;

HideMouse();
while (y++ <= y1)
    {
    memmove(Video,buf,wt);
    Video += 320;
    buf += 320;
    }
ShowMouse();

}

//=============================================================================
//
//=============================================================================
void DrawFillBox(int x,int y,int x1,int y1,UCHAR color)
{
    int     wt;
    UCHAR   *Video;

wt = (y * 320) + x;
Video = VIDSEG + wt;

wt = (x1-x)+1;

while (y++ <= y1)
    {
    memset(Video,color,wt);
    Video += 320;
    }

}


//=============================================================================
//
//=============================================================================
void DrawHorzLine(int x,int y,int x1,UCHAR color)
{
    UCHAR   *Video;
    int     offset;

offset = (y * 320) + x;
Video = VIDSEG + offset;

memset(Video,color,(x1-x)+1);

}

//=============================================================================
//
//=============================================================================
void DrawVertLine(int x,int y,int y1,UCHAR color)
{
    UCHAR   *Video;
    int     offset;

offset = (y * 320) + x;
Video = VIDSEG + offset;

while (y++ <= y1)
    {
    *Video = color;
    Video += 320;
    }

}

//=============================================================================
//
//=============================================================================
void DrawBox(int x,int y,int x1,int y1,UCHAR color)
{

DrawHorzLine(x,y,x1,color);
DrawHorzLine(x,y1,x1,color);
DrawVertLine(x,y,y1,color);
DrawVertLine(x1,y,y1,color);

}


//=============================================================================
//
//=============================================================================
UCHAR GetColor(int x,int y)
{
    int     offset;
    UCHAR   *Video;

offset = (y * 320) + x;
Video = VIDSEG + offset;
return(*Video);
}


//=============================================================================
//
//=============================================================================
void SaveVideo(UCHAR *buf)
{
memmove(buf,VIDSEG,64000);
}

//=============================================================================
//
//=============================================================================
void RestoreVideo(UCHAR *buf)
{
memmove(VIDSEG,buf,64000);
}


//=============================================================================
//
//=============================================================================
void CreateButton(int x,int y,HS *hs,char *s)
{
    int     len,plen,x1,y1;

hs->x = x;
hs->y = y;

len = strlen(s);
plen = len * 6;
hs->x1 = x1 = x + plen + 5;
hs->y1 = y1 = y + 11;

DrawFillBox(x,y,x1,y1,ButtonColor);
DrawHorzLine(x,y,x1,ButtonUpperColor);
DrawVertLine(x,y,y1,ButtonUpperColor);
DrawVertLine(x1,y+1,y1,ButtonLowerColor);
DrawHorzLine(x,y1,x1,ButtonLowerColor);

x1 = x + 3;
y1 = y + 2;
while (*s)
    {
    mdWriteChar(x1,y1,toupper(*s++));
    x1 += 6;
    }

}

//=============================================================================
//
//=============================================================================
UCHAR *SaveBlock(int x,int y,int x1,int y1)
{
    int     ht,wt,size,offset;
    UCHAR   *buf,*bPtr,*vPtr;

ht = (y1 - y) + 1;
wt = (x1 - x) + 1;
size = (ht * wt) + 4;
buf = (UCHAR *)malloc(size);
if (buf == NULL)
    return(buf);

bPtr = buf;
(*(short *)bPtr) = ht;
bPtr += 2;
(*(short *)bPtr) = wt;
bPtr += 2;

offset = (y * 320) + x;
vPtr = VIDSEG + offset;

HideMouse();

while (ht-- > 0)
    {
    memmove(bPtr,vPtr,wt);
    vPtr += 320;
    bPtr += wt;
    }

ShowMouse();

return(buf);
}

//=============================================================================
//
//=============================================================================
void RestoreBlock(int x,int y,UCHAR *buf)
{
    int     ht,wt,offset;
    UCHAR   *vPtr;

if (buf == NULL)
    return;

offset = (y * 320) + x;
vPtr = VIDSEG + offset;

ht = (*(short *)buf);
buf += 2;
wt = (*(short *)buf);
buf += 2;
HideMouse();
while (ht-- > 0)
    {
    memmove(vPtr,buf,wt);
    vPtr += 320;
    buf += wt;
    }
ShowMouse();
}


//=============================================================================
//
//=============================================================================
int QueryBox(int x,int y,char *Msg)
{
    int     done,plen;
    int     x1,y1;
    UCHAR   SaveColor,SaveBGcolor;
    short   mx,my,mbutton;
    char    *s;
    UCHAR   *vBuf;
    HS      hs[2];


SaveColor = BoxColor;
SaveBGcolor = TextBGcolor;
BoxColor = TextBGcolor = ButtonColor;
y1 = y + 42;
s = Msg;
plen = 0;
x1 = 0;

while (*s)
    {
    if (*s++ == 10)
    {
    y1 += 10;
    x1 = 0;
    }
    else
    {
    x1 += 6;
    if (x1 > plen) plen = x1;
    }
    }

x1 = x + plen + 10;

if ((x1-x) < 100)
    x1 = x + 100;

HideMouse();
vBuf = SaveBlock(x,y,x1,y1);
DrawBackBox(x,y,x1,y1);
FontUseColor = 1;
mdWriteString(x+5,y+6,Msg);
BoxColor = SaveColor;
TextBGcolor = SaveBGcolor;
FontUseColor = 0;
CreateButton(x+5,y1-15,&hs[0],"  Ok  ");
CreateButton(x1-46,y1-15,&hs[1],"CANCEL");
ShowMouse();

done = 0;
while (!done)
    {
    mbutton = ReadMouseCursor(&my,&mx);

    if (mbutton & 1)
    {
    for (x1 = 0; x1 < 2; x1++)
        {
        if (mx >= hs[x1].x && mx <= hs[x1].x1 && my >= hs[x1].y && my <= hs[x1].y1)
        {
        done = 1;
        break;
        }

        }

    }

    }

if (vBuf != NULL)
    {
    RestoreBlock(x,y,vBuf);
    free(vBuf);
    }
return(x1);
}




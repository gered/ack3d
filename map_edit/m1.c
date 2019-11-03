// Map Editor for Animation Construction Kit 3D
// Main program
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

extern  UCHAR   colordat[];
extern  char    *smFont;
extern  char    *mdFont;
extern  UCHAR   FontUseColor;
extern  UCHAR   FontTransparent;
extern  UCHAR   FontColor;
extern  UCHAR   TextBGcolor;
extern  UCHAR   ButtonColor;
extern  UCHAR   ButtonUpperColor;
extern  UCHAR   ButtonLowerColor;
extern  UCHAR   BoxColor;
extern  short   ReadErrorCode;

void ShowMaskedBitmap(int x,int y,UCHAR *bm);

//=============================================================================
// Globals
//=============================================================================
    HS      HotSpots[] = {
            AI_UP,283,82,292,93,            // Up
            AI_RIGHT,292,95,304,104,        // Right
            AI_DOWN,283,106,292,117,        // Down
            AI_LEFT,271,95,283,104,         // Left
            AI_MINI,MINI_X,MINI_Y,MINI_X1,MINI_Y1,  // Small map
            AI_SELECT,270,61,306,74,        // Select
            AI_VIEW,5,183,44,196,           // View
            AI_FLOORS,52,165,81,178,        // Floor
            AI_SAVE,52,183,81,196,          // Save
            AI_MAP,88,165,118,178,          // Map
            AI_WALLS,88,183,118,196,        // Walls
            AI_CEILING,127,165,169,178,     // Ceiling
            AI_OBJECTS,127,183,169,196,     // Objects
            AI_OPTIONS,179,183,221,196,     // Options
            AI_EDITWIN,0,0,256,160,         // Edit Window
            AI_BM_UP,308,15,317,26,         // Bitmap Up
            AI_BM_DOWN,308,39,317,50,       // Bitmap Down
            AI_EXIT,5,165,44,178,           // Exit
            -1,0,0,0,0              // End of table
             };


    HS      SelHotSpots[] = {
            1,6,181,40,194,     // Cancel
            2,115,181,140,194,  // Prev
            3,178,181,204,194,  // Next
            4,0,0,319,178,      // Bitmap window
            -1,0,0,0,0      // End of table
             };


    short   ButtonTable[] = {
            AI_MAP,
            AI_FLOORS,
            AI_CEILING
            };


    OPTS    LeftOpts[] = {
            OPT_NORMAL,   0,0,0,"   Normal Sq  ",
            OPT_SLIDING,  0,0,0," Sliding Door ",
            OPT_SPLIT,    0,0,0,"  Split Door  ",
            OPT_SECRET,   0,0,0,"  Secret Door ",
            OPT_LOCKED,   0,0,0,"  Locked Door ",
            OPT_TRANS,    0,0,0,"  Transparent ",
            OPT_MULTI,    0,0,0,"  MultiHeight ",
            OPT_RAISED,   0,0,0,"  Raised Wall ",
            OPT_CANCEL,  44,0,0,"    Cancel    ",
              -1,0,0,0,0
            };

    OPTS    RightOpts[] = {
            OPT_FILL,     0,0,0,"  Fill Map    ",
            OPT_BORDER,   0,0,0,"  Draw Border ",
            OPT_CLEAR,    0,0,0,"  Clear Map   ",
            OPT_PASSABLE, 0,0,0,"  Passable    ",
            OPT_PALETTE,  0,0,0,"  Bm Palette  ",
            OPT_MAPPAL,   0,0,0,"  Map Palette ",
              -1,0,0,0,0
            };



    OPTS    MultiOpts[] = {
            0,0,0,0," Select Bitmap ",
            1,0,0,0,"    Accept     ",
            2,0,0,0,"    Cancel     ",
            -1,0,0,0,0
            };


//=============================================================================
// More globals
//=============================================================================
    UCHAR   **BitmapPtr;
    UCHAR   *Bitmaps[256];
    UCHAR   *ObjBitmaps[256];
    UCHAR   ObjbmNum[256];
    USHORT  MedxGrid[GRID_ARRAY];
    USHORT  MedyGrid[GRID_ARRAY];
    USHORT  MedGrid[GRID_MAX];
    USHORT  MobGrid[GRID_MAX];
    USHORT  MedFloorMap[GRID_MAX];
    USHORT  MedCeilMap[GRID_MAX];
    UCHAR   *MedMultiPtrs[GRID_MAX];
    int     GridRow,GridCol;
    int     ViewMode;
    int     ViewType,LastViewType;
    int     EditType;
    USHORT  WallFlags;
    USHORT  ObjFlags;
    int     MaxCol,MaxRow;
    int     LastgRow,LastgCol;
    int     ModifyFlag;
    short   LastMX,LastMY;
    USHORT  CurrentCode;
    USHORT  LastWallCode;
    USHORT  LastObjCode;
    UCHAR   BaseColor;
    UCHAR   *VidBuf;
    UCHAR   *BgBuf;
    UCHAR   *BgBuf3D;
    UCHAR   *SelBgBuf;
    UCHAR   *BlankbmBuf;
    char    MapName[128];
    char    PalName[128];
    UCHAR   bmPalette[768];
    UCHAR   MapPalette[768];

//=============================================================================
//
//=============================================================================
short GetAction(short mx,short my)
{
    int     index;

index = 0;

while (1)
    {
    if (HotSpots[index].Action == -1)
    break;

    if (mx >= HotSpots[index].x &&
    mx <= HotSpots[index].x1 &&
    my >= HotSpots[index].y &&
    my <= HotSpots[index].y1)
    {
    break;
    }
    index++;
    }

return(HotSpots[index].Action);
}

//=============================================================================
//
//=============================================================================
short GetActionIndex(short action)
{
    short   index;

index = 0;

while (HotSpots[index].Action != -1)
    {
    if (HotSpots[index].Action == action)
    break;

    index++;
    }

return(index);
}


//=============================================================================
//
//=============================================================================
void PressButton(short ActionCode)
{
    int     x,y,x1,y1;
    short   index;

index = GetActionIndex(ActionCode);

x = HotSpots[index].x;
y = HotSpots[index].y;
x1 = HotSpots[index].x1;
y1 = HotSpots[index].y1;

HideMouse();
DrawHorzLine(x,y,x1,ButtonLowerColor);
DrawVertLine(x,y+1,y1,ButtonLowerColor);
DrawVertLine(x1,y+1,y1,ButtonUpperColor);
DrawHorzLine(x,y1,x1,ButtonUpperColor);
ShowMouse();
}


//=============================================================================
//
//=============================================================================
void ReleaseButton(short ActionCode)
{
    int     x,y,x1,y1;
    short   index;

index = GetActionIndex(ActionCode);

x = HotSpots[index].x;
y = HotSpots[index].y;
x1 = HotSpots[index].x1;
y1 = HotSpots[index].y1;

HideMouse();
DrawHorzLine(x,y,x1,ButtonUpperColor);
DrawVertLine(x,y+1,y1,ButtonUpperColor);
DrawVertLine(x1,y+1,y1,ButtonLowerColor);
DrawHorzLine(x,y1,x1,ButtonLowerColor);
ShowMouse();
}



//=============================================================================
//
//=============================================================================
short GetSelAction(short mx,short my)
{
    int     index;

index = 0;

while (1)
    {
    if (SelHotSpots[index].Action == -1)
    break;

    if (mx >= SelHotSpots[index].x &&
    mx <= SelHotSpots[index].x1 &&
    my >= SelHotSpots[index].y &&
    my <= SelHotSpots[index].y1)
    {
    break;
    }
    index++;
    }

return(SelHotSpots[index].Action);
}



//=============================================================================
//
//=============================================================================
void DrawYline(int x,int y)
{
    int     i;
    UCHAR   *Video;
    int     offset;

offset = (y*320)+x;
Video = (UCHAR *)VidBuf;
Video += offset;

for (i = 0; i < GD_HHT; i++)
    {
    *Video++ = 15;
    *Video++ = 15;
    Video += 320;
    }

}


//=============================================================================
//
//=============================================================================
void DrawXline(int x,int y)
{
    UCHAR   *Video;
    int     i,offset;

offset = (y*320)+x;
Video = (UCHAR *)VidBuf;
Video += offset;

for (i = 0; i < GD_HWT; i++)
    {
    *Video-- = 15;
    *Video-- = 15;
    Video += 320;
    }

}

//=============================================================================
//
//=============================================================================
void DrawGridBlock(int x,int y)
{

DrawYline(x,y);
DrawXline(x+1,y);
DrawYline(x-(GD_FWT-2),y+GD_HHT-1);
DrawXline(x+GD_FWT-1,y+GD_HHT-1);

}

//=============================================================================
//
//=============================================================================
void DrawBitmapBlock(int x,int y,UCHAR *bm)
{
    int     row,col,rFact;
    UCHAR   *Video,*vPos;
    int     offset;

offset = (y*320)+x;
Video = (UCHAR *)VidBuf;
Video += offset;
vPos = Video;
bm = &bm[4];

for (row = 0; row < GD_HHT; row++)
    {
    Video = vPos;
    for (col = 0; col < GD_HWT; col++)
    {
    *Video++ = *bm;
    bm += 2;
    *Video++ = *bm;
    bm += 2;
    Video += 320;
    }

    bm += 66;
    vPos += 320;
    Video = vPos;
    for (col = 1; col < GD_HWT; col++)
    {
    *Video++ = *bm;
    bm += 2;
    *Video++ = *bm;
    bm += 2;
    Video += 320;
    }
    bm += 66;
    vPos -= 2;
    }

}



//=============================================================================
//
//=============================================================================
void DrawYbitmap(int x,int y,UCHAR *bm,UCHAR Shade,int TransFlag)
{
    UCHAR   *Video,*vPos;
    UCHAR   ch;
    int     row,col,offset,pos;

offset = (y*320)+x;
Video = (UCHAR *)VidBuf;
Video += offset;
vPos = Video;
bm = &bm[4];

if (!TransFlag)
    {
    for (row = 0; row < GD_FHT; row++)
    {
    Video = vPos;
    pos = 0;
    for (col = 0; col < GD_HWT; col++)
        {
        ch = bm[pos] + Shade;
        *Video++ = ch;
        pos += 2;
        ch = bm[pos] + Shade;
        *Video++ = ch;
        pos += 2;
        Video += 320;
        }

    bm += 128;
    vPos += 320;
    }
    }
else
    {
    for (row = 0; row < GD_FHT; row++)
    {
    Video = vPos;
    pos = 0;
    for (col = 0; col < GD_HWT; col++)
        {
        ch = bm[pos];
        if (ch)
        *Video = ch + Shade;
        Video++;
        pos += 2;
        ch = bm[pos];
        if (ch)
        *Video = ch + Shade;
        pos += 2;
        Video += 321;
        }

    bm += 128;
    vPos += 320;
    }
    }
}


//=============================================================================
//
//=============================================================================
void DrawXbitmap(int x,int y,UCHAR *bm,UCHAR Shade,int TransFlag)
{
    UCHAR   *Video,*vPos;
    UCHAR   ch;
    int     row,col,offset,pos;

offset = (y*320)+x;
Video = (UCHAR *)VidBuf;
Video += offset;
vPos = Video;
bm = &bm[4];

if (!TransFlag)
    {
    for (row = 0; row < GD_FHT; row++)
    {
    Video = vPos;
    pos = 0;
    for (col = 0; col < GD_HWT; col++)
        {
        ch = bm[pos] + Shade;
        *Video-- = ch;
        pos += 2;
        ch = bm[pos] + Shade;
        *Video-- = ch;
        pos += 2;
        Video += 320;
        }

    bm += 128;
    vPos += 320;
    }
    }
else
    {
    for (row = 0; row < GD_FHT; row++)
    {
    Video = vPos;
    pos = 0;
    for (col = 0; col < GD_HWT; col++)
        {
        ch = bm[pos];
        if (ch)
        *Video = ch + Shade;
        Video--;
        pos += 2;
        ch = bm[pos];
        if (ch)
        *Video = ch + Shade;
        pos += 2;
        Video += 319;
        }

    bm += 128;
    vPos += 320;
    }
    }
}

//=============================================================================
//
//=============================================================================
void ClearGridArea(UCHAR *BufPtr)
{
    int     row,wt;
    UCHAR   *Video,*bPtr;

Video = (UCHAR *)VidBuf;
bPtr = &BufPtr[4];
row = ((MAP_Y*320)+MAP_X);
Video += row;
bPtr += row;
wt = (MAP_X1 - MAP_X) + 1;

for (row = MAP_Y; row < MAP_Y1; row++)
    {
    memmove(Video,bPtr,wt);
    Video += 320;
    bPtr += 320;
    }

Video = (UCHAR *)VidBuf;
bPtr = &BgBuf[4];
row = ((MINI_Y*320)+MINI_X);
Video += row;
bPtr += row;

for (row = 0; row < 64; row++)
    {
    memmove(Video,bPtr,64);
    Video += 320;
    bPtr += 320;
    }

}

//=============================================================================
//
//=============================================================================
void RefreshGrid(void)
{
    int     row,wt;
    UCHAR   *Video,*vPtr;

Video = VIDSEG;
vPtr = VidBuf;
row = ((MAP_Y*320)+MAP_X);
Video += row;
vPtr += row;
wt = (MAP_X1 - MAP_X) + 1;

for (row = MAP_Y; row < MAP_Y1; row++)
    {
    memmove(Video,vPtr,wt);
    Video += 320;
    vPtr += 320;
    }

Video = VIDSEG;
vPtr = VidBuf;
row = ((MINI_Y*320)+MINI_X);
Video += row;
vPtr += row;

for (row = 0; row < 64; row++)
    {
    memmove(Video,vPtr,64);
    Video += 320;
    vPtr += 320;
    }

}

//=============================================================================
//
//=============================================================================
void DrawMultiBitmaps(int gPos,int x,int y)
{
    int     cnt;
    UCHAR   *bmPtr,*mPtr;
    USHORT  bNum;

mPtr = MedMultiPtrs[gPos];
if (mPtr == NULL)
    return;

cnt = 0;
y -= GD_FHT;

while (cnt < MAX_MULTI && y >= MAP_Y)
    {
    bNum = mPtr[cnt];
    if (bNum)
    {
    bmPtr = Bitmaps[bNum & 0xFF];
    DrawYbitmap(x,y,bmPtr,0,0);
    DrawXbitmap(x+1,y,bmPtr,0,0);
    DrawYbitmap(x-(GD_FWT-2),y+GD_HHT,bmPtr,0,0);
    DrawXbitmap(x+(GD_FWT-1),y+GD_HHT-1,bmPtr,0,0);
    }
    cnt++;
    y -= GD_FHT;
    }

}

//=============================================================================
//
//=============================================================================
void DrawGrid3D(void)
{
    int     cnt,x,y,xOrg,yOrg;
    int     gPos,gOrg,row,col;
    UCHAR   *bmPtr;
    UCHAR   sValue,color,sValue1;
    USHORT  bNum;

xOrg = MAP_X + 128;
yOrg = MAP_Y + GD_FHT;

gOrg = (GridRow * GRID_WIDTH) + GridCol;
ClearGridArea(BgBuf3D);

row = GridRow;
sValue = sValue1 = 0;

while (xOrg > MAP_X && yOrg < MAP_Y1)
    {
    x = xOrg;
    y = yOrg;
    gPos = gOrg;
    col = GridCol;
    cnt = 0;
    while ((x+GD_FWT) <= MAP_X1 && cnt++ < 4)
    {

    #if 0
    bNum = MedGrid[gPos];
    if (!bNum)
        {
        bNum = MedFloorMap[gPos];
        if (!bNum)
        DrawGridBlock(x,y);
        else
        DrawBitmapBlock(x,y,Bitmaps[bNum & 0xFF]);
        }
    else
        {
        sValue1 = sValue = 1;
        if (col & 1) sValue = 0;
        if (row & 1) sValue1 = 0;
        bmPtr = Bitmaps[bNum & 0xFF];
        bmPtr = &bmPtr[4];
        DrawYbitmap(x,y-GD_FHT,bmPtr,sValue1);
        DrawXbitmap(x+1,y-GD_FHT,bmPtr,sValue);
        DrawYbitmap(x-(GD_FWT-2),y-GD_HHT,bmPtr,0);
        DrawXbitmap(x+(GD_FWT-1),y-GD_HHT-1,bmPtr,0);
        }
    #endif

    DrawBitmapBlock(x,y,Bitmaps[MedFloorMap[gPos] & 0xFF]);
    sValue1 = sValue = 1;
    if (col & 1) sValue = 0;
    if (row & 1) sValue1 = 0;

    bNum = MedyGrid[gPos];
    if (bNum)
        DrawYbitmap(x,y-GD_FHT,Bitmaps[bNum & 0xFF],sValue1,(bNum & WALL_TYPE_TRANS));

    bNum = MedxGrid[gPos];
    if (bNum)
        DrawXbitmap(x+1,y-GD_FHT,Bitmaps[bNum & 0xFF],sValue,(bNum & WALL_TYPE_TRANS));

    bNum = MobGrid[gPos];
    if (bNum)
        ShowMaskedBitmap(x-GD_HWT,y-GD_HHT,ObjBitmaps[ObjbmNum[bNum & 255]]);

    bNum = MedyGrid[gPos + GRID_WIDTH];
    if (bNum)
        DrawYbitmap(x-(GD_FWT-2),y-GD_HHT,Bitmaps[bNum & 0xFF],0,(bNum & WALL_TYPE_TRANS));

    bNum = MedxGrid[gPos + 1];
    if (bNum)
        DrawXbitmap(x+(GD_FWT-1),y-GD_HHT-1,Bitmaps[bNum & 0xFF],0,(bNum & WALL_TYPE_TRANS));

    if (ViewType == VT_CEIL)
        {
        bNum = MedCeilMap[gPos];
        if (bNum)
        DrawBitmapBlock(x,y-GD_FHT,Bitmaps[bNum & 0xFF]);
        }

    if (MedGrid[gPos] & WALL_TYPE_MULTI)
        DrawMultiBitmaps(gPos,x,y);


    col++;
    gPos++;
    x += (GD_FWT-2);
    y += (GD_HHT-1);
    }

    row++;
    gOrg += GRID_WIDTH;

    xOrg -= (GD_FWT-2);
    yOrg += (GD_HHT-1);
    }

gPos = 0;

for (y = 0; y < GRID_HEIGHT; y++)
    {
    for (x = 0; x < GRID_WIDTH; x++)
    {
    sValue = MedGrid[gPos++];
    color = 255;
    if (sValue) color = sValue;
    VidBuf[((y+MINI_Y)*320)+(x+MINI_X)] = color;
    }
    }

x = MINI_X + GridCol;
y = MINI_Y + GridRow;

VidBuf[(y*320)+x] = 128;

HideMouse();
RefreshGrid();
ShowMouse();

}

//=============================================================================
//
//=============================================================================
void Draw2Dxline(int x,int y,UCHAR color)
{
    int     row,offset;
    UCHAR   *vPtr;

offset = (y*320)+x;
vPtr = VidBuf + offset;

vPtr -= 638;
*vPtr = color;
vPtr += 319;
memset(vPtr,color,3);
vPtr += 319;

for (row = 6; row < 18; row++)
    {
    memset(vPtr,color,5);
    vPtr += 320;
    }

vPtr++;
memset(vPtr,color,3);
vPtr[321] = color;

}

//=============================================================================
//
//=============================================================================
void DrawxLineMulti(int x,int y,UCHAR color)
{
    int     row,offset;
    UCHAR   *vPtr;

offset = (y*320)+x;
vPtr = VidBuf + offset + 322;
for (row = 0; row < 12; row += 2)
    {
    *vPtr = color;
    vPtr += 640;
    }

}

//=============================================================================
//
//=============================================================================
void Draw2Dyline(int x,int y,UCHAR color)
{
    int     offset;
    UCHAR   *vPtr;

offset = (y*320)+x;
vPtr = VidBuf + offset;

memset(vPtr,color,15);
vPtr += 319;
memset(vPtr,color,17);
vPtr += 319;
memset(vPtr,color,19);
vPtr += 321;
memset(vPtr,color,17);
vPtr += 321;
memset(vPtr,color,15);

}

//=============================================================================
//
//=============================================================================
void DrawyLineMulti(int x,int y,UCHAR color)
{
    int     offset;
    UCHAR   *vPtr;

offset = (y*320)+x;
vPtr = VidBuf + offset + 640;

for (offset = 0; offset < 15; offset += 2)
    vPtr[offset] = color;

}

//=============================================================================
//
//=============================================================================
void DrawGrid2D(void)
{
    int     x,y,gRow,gCol;
    int     row,col,gOrg,gPos;
    USHORT  mCode;
    UCHAR   sValue,color,sValue1;

gOrg = (GridRow * GRID_WIDTH) + GridCol;

gRow = GridRow;
y = 1;

for (row = 0; row < 9; row++)
    {
    gCol = GridCol;
    gPos = gOrg;
    x = 8;
    for (col = 0; col < 12; col++)
    {
    mCode = MedxGrid[gPos];
    if (mCode)
        {
        mCode += BaseColor;
        Draw2Dxline(x-5,y+5,mCode);
        if (mCode & WALL_TYPE_MULTI)
        DrawxLineMulti(x-5,y+5,(mCode ^ 15)-14);
        if (mCode & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT))
        DrawxLineMulti(x-5,y+5,255);
        }

    mCode = MedxGrid[gPos+1];
    if (mCode)
        {
        mCode += BaseColor;
        Draw2Dxline(x+15,y+5,mCode);
        if (mCode & WALL_TYPE_MULTI)
        DrawxLineMulti(x+15,y+5,(mCode ^ 15)-14);
        if (mCode & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT))
        DrawxLineMulti(x+15,y+5,255);
        }

    mCode = MedyGrid[gPos];
    if (mCode)
        {
        mCode += BaseColor;
        Draw2Dyline(x,y,mCode);
        if (mCode & WALL_TYPE_MULTI)
        DrawyLineMulti(x,y,(mCode ^ 15)-14);
        if (mCode & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT))
        DrawyLineMulti(x,y,255);
        }

    mCode = MedyGrid[gPos+GRID_WIDTH];
    if (mCode)
        {
        mCode += BaseColor;
        Draw2Dyline(x,y+17,mCode);
        if (mCode & WALL_TYPE_MULTI)
        DrawyLineMulti(x,y+17,(mCode ^ 15)-14);
        if (mCode & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT))
        DrawyLineMulti(x,y+17,255);
        }

    x += 20;
    gPos++;
    }

    gOrg += 64;
    y += 17;
    }


gPos = 0;

for (y = 0; y < GRID_HEIGHT; y++)
    {
    for (x = 0; x < GRID_WIDTH; x++)
    {
    sValue = MedGrid[gPos++];
    color = 255;
    if (sValue) color = sValue;
    VidBuf[((y+MINI_Y)*320)+(x+MINI_X)] = color;
    }
    }


x = MINI_X + GridCol;
y = MINI_Y + GridRow;

VidBuf[(y*320)+x] = 128;

}

//=============================================================================
//
//=============================================================================
void DrawTinyBitmap(int x,int y,UCHAR *bm)
{
    int     offset,row,col,pos,pOrg;
    UCHAR   *Video,*bmPtr;

offset = (y * 320) + x;
Video = VidBuf + offset;
bmPtr = &bm[4];

for (row = 0; row < 9; row++)
    {
    pos = 0;
    for (col = 0; col < 12; col++)
    {
    *Video++ = bmPtr[pos];
    pos += 5;
    }
    *Video = bmPtr[63];
    Video += 308;
    bmPtr += 384;
    }

bmPtr = &bm[4036];
pos = 0;
for (col = 0; col < 12; col++)
    {
    *Video++ = bmPtr[pos];
    pos += 5;
    }
*Video = bmPtr[63];

}

//=============================================================================
//
//=============================================================================
void DrawFloorCeil(void)
{
    int     row,col,x,y,pos,pOrg;
    USHORT  bCode;
    USHORT  *buf;

if (ViewType == VT_FLOOR)
    buf = MedFloorMap;
else
    buf = MedCeilMap;

pOrg = (GridRow * GRID_WIDTH) + GridCol;

y = 7;

for (row = 0; row < 9; row++)
    {
    x = 9;
    pos = pOrg;
    for (col = 0; col < 12; col++)
    {
    bCode = buf[pos++];
    if (bCode)
        DrawTinyBitmap(x,y,Bitmaps[bCode & 255]);

    x += 20;
    }

    y += 17;
    pOrg += GRID_WIDTH;
    }

HideMouse();
RefreshGrid();
ShowMouse();

}

//=============================================================================
//
//=============================================================================
void DrawObjects(void)
{
    int     row,col,x,y,pos,pOrg;
    USHORT  bCode;
    USHORT  *buf;
    UCHAR   bcLow;

buf = MobGrid;

pOrg = (GridRow * GRID_WIDTH) + GridCol;

y = 7;

for (row = 0; row < 9; row++)
    {
    x = 9;
    pos = pOrg;
    for (col = 0; col < 12; col++)
    {
    bCode = buf[pos++];
    if (bCode)
        {
        bcLow = bCode & 0xFF;
        DrawTinyBitmap(x,y,ObjBitmaps[ObjbmNum[bcLow]]);
        }
    x += 20;
    }

    y += 17;
    pOrg += GRID_WIDTH;
    }

HideMouse();
RefreshGrid();
ShowMouse();
}

//=============================================================================
//
//=============================================================================
void DrawGrid(void)
{

if (ViewMode == VM_3D)
    DrawGrid3D();
else
    {
    ClearGridArea(BgBuf);
    DrawGrid2D();
    if (ViewType != VT_MAP)
    DrawFloorCeil();
    else
    DrawObjects();
    }
}

//=============================================================================
//
//=============================================================================
void ShowScreen(char *Name)
{

BgBuf = AckReadiff(Name);
if (BgBuf == NULL)
    return;

memmove(VIDSEG,&BgBuf[4],64000);
AckSetPalette(colordat);
memmove(MapPalette,colordat,768);
}

//=============================================================================
//
//=============================================================================
void ShowMaskedBitmap(int x,int y,UCHAR *bm)
{
    int     row,pos;
    UCHAR   ch;
    UCHAR   *Video;

row = ((y * 320) + x);
Video = VidBuf;
Video += row;
bm += 4;

for (row = 0; row < 32; row++)
    {
    for (pos = 0; pos < 64; pos += 2)
    {
    ch = bm[pos];
    if (ch)
        *Video = ch;
    Video++;
    }
    Video += (320-32);
    bm += 128;
    }

}


//=============================================================================
//
//=============================================================================
void ShowBitmap(int x,int y,UCHAR *bm)
{
    int     row,pos;
    UCHAR   *Video;

row = ((y * 320) + x);
Video = VIDSEG;
Video += row;
bm += 4;

HideMouse();

for (row = 0; row < 32; row++)
    {
    for (pos = 0; pos < 64; pos += 2)
    *Video++ = bm[pos];

    Video += (320-32);
    bm += 128;
    }

ShowMouse();
}

//=============================================================================
//
//=============================================================================
short LoadGridMap(char *Name)
{
    short   handle,count,i,pos;
    int     mLen,aLen;
    UCHAR   *mPtr;

handle = open(Name,O_RDONLY|O_BINARY);
if (handle < 1)
    return(-1);

aLen = GRID_ARRAY * 2;
mLen = GRID_MAX * 2;

if (read(handle,MedGrid,mLen) != mLen)
    {
    close(handle);
    return(-2);
    }

if (read(handle,MobGrid,mLen) != mLen)
    {
    close(handle);
    return(-2);
    }

if (read(handle,MedxGrid,aLen) != aLen)
    {
    close(handle);
    return(-2);
    }

if (read(handle,MedyGrid,aLen) != aLen)
    {
    close(handle);
    return(-2);
    }

if (read(handle,MedFloorMap,mLen) != mLen)
    {
    close(handle);
    return(-2);
    }

if (read(handle,MedCeilMap,mLen) != mLen)
    {
    close(handle);
    return(-2);
    }


read(handle,&count,2);

if (count)
    {
    for (i = 0; i < count;i++)
    {
    read(handle,&pos,2);
    mPtr = (UCHAR *)malloc(MAX_MULTI);
    if (mPtr == NULL)
        {
        close(handle);
        return(-3);
        }

    MedMultiPtrs[pos] = mPtr;
    read(handle,mPtr,MAX_MULTI);
    }
    }

close(handle);
return(0);
}

//=============================================================================
//
//=============================================================================
short SaveGridMap(char *Name)
{
    short   handle,count,i;
    long    rba;
    int     mLen,aLen;
    UCHAR   *mPtr;

handle = open(Name,O_RDWR|O_BINARY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
if (handle < 1)
    return(-1);


// Write the map grids below, keeping track of the Relative Byte Address

aLen = GRID_ARRAY * 2;
mLen = GRID_MAX * 2;
rba = 0;

if (write(handle,MedGrid,mLen) != mLen)
    {
    close(handle);
    return(-2);
    }

rba += mLen;

if (write(handle,MobGrid,mLen) != mLen)
    {
    close(handle);
    return(-2);
    }

rba += mLen;

if (write(handle,MedxGrid,aLen) != aLen)
    {
    close(handle);
    return(-2);
    }

rba += aLen;
if (write(handle,MedyGrid,aLen) != aLen)
    {
    close(handle);
    return(-2);
    }

rba += aLen;

if (write(handle,MedFloorMap,mLen) != mLen)
    {
    close(handle);
    return(-2);
    }

rba += mLen;

if (write(handle,MedCeilMap,mLen) != mLen)
    {
    close(handle);
    return(-2);
    }

rba += mLen;
count = 0;

// Write the initial count of multi-height walls

write(handle,&count,2);

for (i = 0; i < GRID_MAX; i++)
    {
    mPtr = MedMultiPtrs[i];
    if (mPtr)
    {
    count++;
    write(handle,&i,2);        // Write the position of this wall
    write(handle,mPtr,MAX_MULTI);  // Write the bitmap numbers for this wall
    }
    }

if (count)
    {
    lseek(handle,rba,SEEK_SET);     // Seek back to the count
    write(handle,&count,2);     // And write the new count
    }

close(handle);
ModifyFlag = 0;
return(0);
}

//=============================================================================
//
//=============================================================================
void ShowCurrentBitmap(void)
{
    char    buf[10];
    UCHAR   cCode;

cCode = CurrentCode & 0xFF;
if (EditType == ED_OBJECTS)
    cCode = ObjbmNum[cCode];

HideMouse();
ShowBitmap(BM_X,BM_Y,BitmapPtr[cCode]);
DrawFillBox(BM_COLOR_X,BM_COLOR_Y,BM_COLOR_X1,BM_COLOR_Y1,cCode+BaseColor);
sprintf(buf,"%02X",CurrentCode & 0xFF);
smWriteString(BM_NUM_X,BM_NUM_Y,buf);
ShowMouse();
}

//=============================================================================
//
//=============================================================================
void UpdateScreen(void)
{
    UCHAR   *Buf;

LastViewType = -1;
LastgRow = -1;

Buf = &BgBuf[4];
if (ViewMode == VM_3D)
    Buf = &BgBuf3D[4];

HideMouse();
memmove(VIDSEG,Buf,64000);
ShowCurrentBitmap();

if (EditType == ED_WALLS)
    PressButton(AI_WALLS);
else
    PressButton(AI_OBJECTS);

ShowMouse();
}

//=============================================================================
//
//=============================================================================
int GetGridPos(short mx,short my)
{
    int     row,col,x,y,pos;

mx -= 2;
row = my / 17;
col = mx / 20;
if (row > 9 || col > 12)
    return(-1);

row += GridRow;
col += GridCol;
pos = (row*GRID_WIDTH)+col;
return(pos);
}

//=============================================================================
//
//=============================================================================
short ObjectInMap(USHORT oCode)
{
    int     i;

oCode &= 0xFF;

for (i = 0; i < GRID_MAX; i++)
    {
    if (oCode == (MobGrid[i] & 0xFF))
    return(1);
    }

return(0);
}


//=============================================================================
//
//=============================================================================
void PutCode(short mx,short my)
{
    int     x,y,pos;
    USHORT  wFlags,cCode;

if ((pos = GetGridPos(mx,my)) == -1)
    {
    SoundBeep();
    return;
    }

mx -= 2;
x = mx % 20;
y = my % 17;

ModifyFlag = 1;

if (ViewType == VT_MAP && EditType == ED_OBJECTS)
    {
    if (CurrentCode > 0 && ObjectInMap(CurrentCode))
    {
    SoundBeep();
    return;
    }

    if (CurrentCode)
    MobGrid[pos] = ObjFlags | CurrentCode;
    else
    MobGrid[pos] = 0;
    return;
    }

switch (ViewType)
    {
    case VT_MAP:
    if (!CurrentCode && (MedGrid[pos] & WALL_TYPE_MULTI))
        {
        if (MedMultiPtrs[pos] != NULL)
        {
        free(MedMultiPtrs[pos]);
        MedMultiPtrs[pos] = NULL;
        }
        x = y = y;      // Fake it out to clear entire block
        }

    wFlags = WallFlags;
    if (!CurrentCode) wFlags = 0;
    //  if (CurrentCode == DOOR_XCODE || CurrentCode == DOOR_YCODE)
    //      wFlags = DoorFlags;

    cCode = wFlags | CurrentCode;

    MedGrid[pos] &= wFlags;
    MedGrid[pos] |= cCode;

    if (x > 6 && x < 20 && y < 6)
        {
        MedyGrid[pos] &= wFlags;
        MedyGrid[pos] |= cCode;
        }
    else
        if (x > 6 && x < 20 && y > 12)
        {
        MedyGrid[pos+GRID_WIDTH] &= wFlags;
        MedyGrid[pos+GRID_WIDTH] |= cCode;
        }
        else
        if (x < 7 && y > 5 && y < 13)
            {
            MedxGrid[pos] &= wFlags;
            MedxGrid[pos] |= cCode;
            }
        else
            if (x > 19 && y > 5 && y < 13)
            {
            MedxGrid[pos+1] &= wFlags;
            MedxGrid[pos+1] |= cCode;
            }
        else
            if (x > 6 && x < 20 && y > 6 && y < 13)
            {
            MedxGrid[pos] &= wFlags;
            MedxGrid[pos+1] &= wFlags;
            MedyGrid[pos] &= wFlags;
            MedyGrid[pos+GRID_WIDTH] &= wFlags;
            MedxGrid[pos] |= cCode;
            MedxGrid[pos+1] |= cCode;
            MedyGrid[pos] |= cCode;
            MedyGrid[pos+GRID_WIDTH] |= cCode;
            }
    break;

    case VT_FLOOR:
    MedFloorMap[pos] = CurrentCode;
    break;

    case VT_CEIL:
    MedCeilMap[pos] = CurrentCode;
    break;

    default:
    break;
    }
}

//=============================================================================
//
//=============================================================================
void GetCode(short mx,short my)
{
    int     x,y,pos;
    USHORT  wFlags,cCode;

if ((pos = GetGridPos(mx,my)) == -1)
    {
    SoundBeep();
    return;
    }

mx -= 2;
x = mx % 20;
y = my % 17;

if (ViewType == VT_MAP && EditType == ED_OBJECTS)
    {
    CurrentCode = MobGrid[pos] & 255;
    return;
    }

switch (ViewType)
    {
    case VT_MAP:
    if (x > 6 && x < 20 && y < 6)
        {
        CurrentCode = MedyGrid[pos] & 0xFF;
        }
    else
        if (x > 6 && x < 20 && y > 12)
        {
        CurrentCode = MedyGrid[pos+GRID_WIDTH] & 0xFF;
        }
        else
        if (x < 7 && y > 5 && y < 13)
            {
            CurrentCode = MedxGrid[pos] & 0xFF;
            }
        else
            if (x > 19 && y > 5 && y < 13)
            {
            CurrentCode = MedxGrid[pos+1] & 0xFF;
            }
        else
            if (x > 6 && x < 20 && y > 6 && y < 13)
            {
            CurrentCode = MedGrid[pos] & 0xFF;
            }
    break;

    case VT_FLOOR:
    CurrentCode = MedFloorMap[pos] & 0xFF;
    break;

    case VT_CEIL:
    CurrentCode = MedCeilMap[pos] & 0xFF;
    break;

    default:
    break;
    }
}




//=============================================================================
//
//=============================================================================
void UpdatePosn(short mx,short my)
{
    short   row,col;
    char    buf[20];

if (ViewType != LastViewType)
    {
    if (LastViewType != -1)
    ReleaseButton(ButtonTable[LastViewType]);
    LastViewType = ViewType;
    PressButton(ButtonTable[LastViewType]);
    }

FontColor = 0;

if (GridRow != LastgRow || GridCol != LastgCol)
    {
    LastgRow = GridRow;
    LastgCol = GridCol;
    sprintf(buf,"%02d,%02d",GridCol,GridRow);
    smWriteString(MAPXY_X,MAPXY_Y,buf);
    }

if (mx > MAP_X1 || my > MAP_Y1)
    return;

if (mx == LastMX && my == LastMY)
    return;

LastMX = mx;
LastMY = my;

mx -= 2;
row = (my / 17) + GridRow;
col = (mx / 20) + GridCol;
sprintf(buf,"%02d,%02d",col,row);
smWriteString(CURXY_X,CURXY_Y,buf);
}


//=============================================================================
//
//=============================================================================
void ShowSelBitmaps(UCHAR bCode)
{
    int     row,col,x,y;
    UCHAR   color;

y = 0;

HideMouse();
for (row = 0; row < 5; row++)
    {
    x = 3;
    for (col = 0; col < 9; col++)
    {
    color = 255;
    if (bCode == CurrentCode) color = 32;
    DrawBox(x,y,x+33,y+33,color);
    if (EditType == ED_WALLS)
        ShowBitmap(x+1,y+1,BitmapPtr[bCode++]);
    else
        ShowBitmap(x+1,y+1,BitmapPtr[ObjbmNum[bCode++]]);
    x += 35;
    }

    y += 35;
    }
ShowMouse();
}

//=============================================================================
//
//=============================================================================
int SelectScreen(void)
{
    int     row,col,x,y;
    int     done;
    short   mx,my,button,action;
    UCHAR   bCode;


bCode = 0;

HideMouse();
memmove(VIDSEG,&SelBgBuf[4],64000);
ShowSelBitmaps(bCode);
MouseReleased();
ShowMouse();

done = 0;

while (!done)
    {
    button = ReadMouseCursor(&my,&mx);

    if (button & 1)
    {
    action = GetSelAction(mx,my);

    switch (action)
        {
        case 1:     // Cancel
        done = -1;
        break;

        case 2:     // Prev
        bCode -= 9;
        ShowSelBitmaps(bCode);
        delay(50);
        break;

        case 3:     // Next
        bCode += 9;
        ShowSelBitmaps(bCode);
        delay(50);
        break;

        case 4:     // Bitmap window
        mx -= 3;
        row = my / 35;
        col = mx / 35;
        if (col > 8 || row > 8)
            break;

        bCode = bCode + ((row*9)+col);
        CurrentCode = bCode;
        done = 1;
        break;

        default:
        break;
        }
    }
    }

return(done);
}

//=============================================================================
//
//=============================================================================
UCHAR GetIndexColor(short index)
{
return(GetColor(HotSpots[index].x,HotSpots[index].y));
}

//=============================================================================
//
//=============================================================================
UCHAR GetIndexColor2(short index)
{
return(GetColor(HotSpots[index].x1,HotSpots[index].y1));
}

//=============================================================================
//
//=============================================================================
void DrawBackBox(int x,int y,int x1,int y1)
{
DrawFillBox(x,y,x1,y1,BoxColor);
DrawBox(x,y,x1,y1,ButtonUpperColor);
DrawBox(x+1,y+1,x1-1,y1-1,ButtonLowerColor);
DrawBox(x+2,y+2,x1-2,y1-2,ButtonUpperColor);
}

//=============================================================================
//
//=============================================================================
void ShowButtons(OPTS *op,HS *hs,int x,int y,int xAmt,int yAmt)
{

while (op->id != -1)
    {
    CreateButton(x+op->xBias,y+op->yBias,&hs[op->id],op->Text);
    if (op->Flags & OPTF_CHECKED)
    mdWriteChar(x+3,y+2,'^');   // Using the hat as a checkmark
    x += xAmt;
    y += yAmt;
    op++;
    }

}

//=============================================================================
//
//=============================================================================
int ShowOptions(void)
{
    int     x,y,done,i;
    short   mx,my,mbutton;
    HS      hs[OPT_MAX];

LeftOpts[OPT_NORMAL].Flags &= ~OPTF_CHECKED;
if (!WallFlags)
    LeftOpts[OPT_NORMAL].Flags |= OPTF_CHECKED;

LeftOpts[OPT_TRANS].Flags &= ~OPTF_CHECKED;
if (WallFlags & WALL_TYPE_TRANS)
    LeftOpts[OPT_TRANS].Flags |= OPTF_CHECKED;

LeftOpts[OPT_RAISED].Flags &= ~OPTF_CHECKED;
if (WallFlags & WALL_TYPE_UPPER)
    LeftOpts[OPT_RAISED].Flags |= OPTF_CHECKED;


RightOpts[OPT_PASSABLE-OPT_FILL].Flags &= ~OPTF_CHECKED;

if (EditType == ED_WALLS && (WallFlags & WALL_TYPE_PASS))
    RightOpts[OPT_PASSABLE-OPT_FILL].Flags |= OPTF_CHECKED;

//if (EditType == ED_OBJECTS && (ObjFlags & OF_PASSABLE))
//    RightOpts[OPT_PASSABLE-OPT_FILL].Flags |= OPTF_CHECKED;


LeftOpts[OPT_SLIDING].Flags &= ~OPTF_CHECKED;
if (WallFlags & DOOR_TYPE_SLIDE)
    LeftOpts[OPT_SLIDING].Flags |= OPTF_CHECKED;

LeftOpts[OPT_SPLIT].Flags &= ~OPTF_CHECKED;
if (WallFlags & DOOR_TYPE_SPLIT)
    LeftOpts[OPT_SPLIT].Flags |= OPTF_CHECKED;

LeftOpts[OPT_SECRET].Flags &= ~OPTF_CHECKED;
if (WallFlags & DOOR_TYPE_SECRET)
    LeftOpts[OPT_SECRET].Flags |= OPTF_CHECKED;

LeftOpts[OPT_LOCKED].Flags &= ~OPTF_CHECKED;
if (WallFlags & DOOR_LOCKED)
    LeftOpts[OPT_LOCKED].Flags |= OPTF_CHECKED;

HideMouse();
DrawBackBox(18,14,234,144);
ShowButtons(LeftOpts,hs,30,22,0,13);
ShowButtons(RightOpts,hs,132,22,0,13);
ShowMouse();

done = 0;
while (!done)
    {
    mbutton = ReadMouseCursor(&my,&mx);

    if (mbutton & 1)
    {
    for (x = 0; x < OPT_MAX; x++)
        {
        if (mx >= hs[x].x && mx <= hs[x].x1 && my >= hs[x].y && my <= hs[x].y1)
        {
        done = 1;
        break;
        }
        }
    }
    }

return(x);
}

//=============================================================================
//
//=============================================================================
void FillMap(UCHAR bCode)
{
    int     i;

ModifyFlag = 1;
switch (ViewType)
    {
    case VT_MAP:
    if (EditType == ED_WALLS)
        {
        for (i = 0; i < GRID_MAX; i++)
        MedGrid[i] = bCode;

        for (i = 0; i < GRID_ARRAY; i++)
        {
        MedxGrid[i] = bCode;
        MedyGrid[i] = bCode;
        }
        }
    else
        {
        for (i = 0; i < GRID_MAX; i++)
        MobGrid[i] = bCode;
        }
    break;

    case VT_FLOOR:
    for (i = 0; i < GRID_MAX; i++)
        MedFloorMap[i] = bCode;
    break;

    case VT_CEIL:
    for (i = 0; i < GRID_MAX; i++)
        MedCeilMap[i] = bCode;
    break;

    default:
    break;
    }
}

//=============================================================================
//
//=============================================================================
void DrawBorder(USHORT bCode)
{
    short   row,col;
    USHORT  offset;

if (ViewType != VT_MAP)
    {
    SoundBeep();
    return;
    }

ModifyFlag = 1;

offset = 0;
for (col = 0; col < 64; col++)
    {
    MedGrid[col] = bCode;
    MedyGrid[col] = bCode;
    }
for (row = 0; row < 64; row++)
    {
    MedGrid[offset] = bCode;
    MedGrid[offset+63] = bCode;
    MedxGrid[offset] = bCode;
    MedxGrid[offset+63] = bCode;
    offset += 64;
    }

offset = 64 * 63;
for (col = 0; col < 64; col++)
    {
    MedyGrid[offset] = bCode;
    MedGrid[offset++] = bCode;
    }

}


//=============================================================================
//
//=============================================================================
void DrawArrow(int x,int y,UCHAR color)
{
    int     offset;
    UCHAR   *Video;

offset = (y * 320) + x;
Video = VIDSEG + offset;
HideMouse();
DrawHorzLine(x,y,x+10,color);
Video[-319] = color;
Video[321] = color;
Video[-638] = color;
Video[642] = color;

ShowMouse();
}

//=============================================================================
//
//=============================================================================
int SelectGridBox(void)
{
    int     pos;
    short   mx,my,mbutton;

if (QueryBox(40,40,"Select a grid square"))
    return(-1);

MouseReleased();
DrawGrid();
pos = 0;

while (1)
    {
    mbutton = ReadMouseCursor(&my,&mx);
    if (mbutton & 2)
    return(-1);

    if (mbutton & 1)
    {
    if ((pos = GetGridPos(mx,my)) == -1)
        continue;
    break;
    }
    }

MouseReleased();
return(pos);
}


//=============================================================================
//
//=============================================================================
void EditMulti(int mode,short gx,short gy)
{
    int     i,x,y,x1,y1,pos,done,num;
    short   mx,my,mbuttons;
    USHORT  OldCurrent;
    UCHAR   bCode;
    UCHAR   *mPtr;
    HS      hs[4];
    UCHAR   mBuf[8];

if (!mode)          // User must pick a grid square
    {
    pos = SelectGridBox();
    if (pos == -1)
    return;
    }
else                // Otherwise mouse coordinates are used
    {
    if ((pos = GetGridPos(gx,gy)) == -1)
    return;
    }

OldCurrent = CurrentCode;

memset(mBuf,0,8);
mPtr = MedMultiPtrs[pos];
if (mPtr)
    memmove(mBuf,mPtr,MAX_MULTI);

HideMouse();
DrawFillBox(MAP_X,MAP_Y,MAP_X1-3,MAP_Y1,0);
x = y = 0;
for (i = 2; i >= 0; i--)
    {
    DrawBox(x,y,x+34,y+34,ButtonUpperColor);
    if (mPtr != NULL)
    {
    bCode = mPtr[i];
    if (bCode)
        {
        ShowBitmap(x+1,y+1,Bitmaps[bCode]);
        }
    }
    y += 36;
    }

DrawBox(x,y,x+34,y+34,ButtonUpperColor);
ShowBitmap(x+1,y+1,Bitmaps[MedGrid[pos] & 0xFF]);
ShowButtons(MultiOpts,hs,80,40,0,20);
ShowMouse();

num = 0;
done = 0;
y = 2 * 36;
DrawArrow(38,y+16,ButtonUpperColor);

while (!done)
    {
    mbuttons = ReadMouseCursor(&my,&mx);

    if (mbuttons & 2)
    {
    if (mx < 34 && my < 110)
        {
        DrawArrow(38,y+16,0);
        num = 2 - (my / 36);
        y = (2 - num) * 36;
        DrawArrow(38,y+16,ButtonUpperColor);
        mBuf[num] = 0;
        HideMouse();
        ShowBitmap(1,y+1,Bitmaps[0]);
        ShowMouse();
        }
    MouseReleased();
    continue;
    }

    if (mbuttons & 1)
    {
    if (mx < 34 && my < 110)
        {
        DrawArrow(38,y+16,0);
        num = 2 - (my / 36);
        y = (2 - num) * 36;
        DrawArrow(38,y+16,ButtonUpperColor);
        MouseReleased();
        continue;
        }

    for (i = 0; i < 3; i++)
        {
        if (mx >= hs[i].x && mx <= hs[i].x1 &&
        my >= hs[i].y && my <= hs[i].y1)
        {
        if (i)
            done = 1;
        else
            {
            HideMouse();
            SaveVideo(VidBuf);
            ShowMouse();
            x1 = SelectScreen();
            HideMouse();
            RestoreVideo(VidBuf);
            ShowMouse();
            if (x1 == -1)
            continue;

            ShowBitmap(1,y+1,Bitmaps[CurrentCode & 0xFF]);
            mBuf[num] = CurrentCode;
            MouseReleased();
            if (num < 2)
            {
            DrawArrow(38,y+16,0);
            num++;
            y = (2 - num) * 36;
            DrawArrow(38,y+16,ButtonUpperColor);
            }
            }
        break;
        }
        }
    }
    }


if (i == 1)
    {
    if (mPtr == NULL)
    mPtr = (UCHAR *)malloc(MAX_MULTI);

    MedMultiPtrs[pos] = mPtr;
    if (mPtr != NULL)
    {
    memmove(mPtr,mBuf,MAX_MULTI);
    ModifyFlag = 1;
    MedGrid[pos] |= WALL_TYPE_MULTI;
    MedxGrid[pos] |= WALL_TYPE_MULTI;
    MedxGrid[pos+1] |= WALL_TYPE_MULTI;
    MedyGrid[pos] |= WALL_TYPE_MULTI;
    MedyGrid[pos+GRID_WIDTH] |= WALL_TYPE_MULTI;
    }
    }

CurrentCode = OldCurrent;
HideMouse();
ShowCurrentBitmap();
ShowMouse();
}


//=============================================================================
//
//=============================================================================
void ProcessOptions(int OptNum)
{

switch (OptNum)
    {
    case OPT_NORMAL:
    WallFlags = 0;
    break;

    case OPT_SLIDING:
    WallFlags &= ~(DOOR_TYPE_SPLIT+DOOR_TYPE_SECRET);
    WallFlags |= DOOR_TYPE_SLIDE;
    break;

    case OPT_SPLIT:
    WallFlags &= ~(DOOR_TYPE_SLIDE+DOOR_TYPE_SECRET);
    WallFlags |= DOOR_TYPE_SPLIT;
    break;

    case OPT_SECRET:
    WallFlags &= ~(DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT);
    WallFlags |= DOOR_TYPE_SECRET;
    break;

    case OPT_LOCKED:
    WallFlags ^= DOOR_LOCKED;
    break;

    case OPT_TRANS:
    WallFlags ^= WALL_TYPE_TRANS;
    break;

    case OPT_MULTI:
    EditMulti(0,0,0);
    break;

    case OPT_RAISED:
    WallFlags ^= WALL_TYPE_UPPER;
    break;

    case OPT_FILL:
    if (!QueryBox(70,20,"Fill entire map?"))
        FillMap(CurrentCode);
    break;

    case OPT_BORDER:
    if (!QueryBox(30,20,"Draw Border with current bitmap?"))
        DrawBorder(CurrentCode);
    break;

    case OPT_CLEAR:
    if (!QueryBox(70,20,"Clear entire map?"))
        FillMap(0);
    break;

    case OPT_PASSABLE:
    if (EditType == ED_WALLS)
        WallFlags ^= WALL_TYPE_PASS;
//    else
//        ObjFlags ^= OF_PASSABLE;
    break;

    case OPT_PALETTE:
    AckSetPalette(bmPalette);
    break;

    case OPT_MAPPAL:
    AckSetPalette(MapPalette);
    break;

    default:
    break;
    }

}

//=============================================================================
//
//=============================================================================
int main(short argc,char **argv)
{
    short   mx,my,mButtons;
    short   Action,done,rFlag;
    int     result;
    USHORT  OldCode,key;

if (MouseInstalled() != -1)
    {
    printf("Mouse required.\n");
    return 1;
    }

VidBuf = (UCHAR *)malloc(64000);
BlankbmBuf = (UCHAR *)malloc(4100);
if (VidBuf == NULL || BlankbmBuf == NULL)
    {
    printf("Not enough memory\n");
    return 1;
    }

memset(BlankbmBuf,0,4100);

for (mx = 0; mx < 256; mx++)
    {
    Bitmaps[mx] = BlankbmBuf;
    ObjBitmaps[mx] = BlankbmBuf;
    }


if (LoadDescFile(argv[1]))
    {
    printf("\nError reading ASCII file - ErrorCode = %d\n",ReadErrorCode);
    return 1;
    }

memmove(bmPalette,colordat,768);

if (LoadGridMap(MapName))
    {
//  printf("\nError loading map file - ErrorCode = %d\n",ReadErrorCode);
//  return;
    memset(MedGrid,0,(GRID_MAX * 2));
    memset(MobGrid,0,(GRID_MAX * 2));
    memset(MedFloorMap,0,(GRID_MAX * 2));
    memset(MedCeilMap,0,(GRID_MAX * 2));
    printf("\nNew map file %s will be created.\n",MapName);
    }

if (AckOpenResource("MEDIT.DTF"))
    {
    printf("Unable to locate MEDIT.DTF\n");
    return 1;
    }


if (LoadSmallFont())
    {
    printf("Error loading SPFONT.BBM\n");
    return 1;
    }

if (LoadMedFont())
    {
    printf("Error loading FONT6X9.BBM\n");
    return 1;
    }


ViewMode = VM_2D;
ViewType = VT_MAP;
MaxCol = GRID_WIDTH-12;
MaxRow = GRID_HEIGHT-9;
BaseColor = 99;

AckSetVGAmode();
ShowScreen((char *)RES_2DSCREEN);   // "m12d.lbm");
TextBGcolor = GetColor(MAPXY_X,MAPXY_Y);
BoxColor = TextBGcolor;
Action = GetActionIndex(AI_MAP);
ButtonColor = GetColor(HotSpots[Action].x+1,HotSpots[Action].y+1);
ButtonUpperColor = GetIndexColor(AI_MAP);
ButtonLowerColor = GetIndexColor2(AI_MAP);
LastgRow = -1;
LastViewType = -1;
WallFlags = ObjFlags = 0;
EditType = ED_WALLS;
BitmapPtr = Bitmaps;
ShowMouse();
DrawGrid();
LastObjCode = LastWallCode = CurrentCode = 1;
ShowCurrentBitmap();
PressButton(AI_WALLS);

BgBuf3D = AckReadiff((char *)RES_3DSCREEN);    // "m13d.lbm");
SelBgBuf = AckReadiff((char *)RES_SELSCREEN);  //"m1sel.lbm");

AckCloseResource();

done = 0;
rFlag = 0;

while (!done)
    {
    key = inkey();
    if (key == KEY_ESC)
    {
    PressButton(AI_EXIT);
    if (ModifyFlag)
        {
        if (QueryBox(20,40,"Map has Changed.\nExit Anyway?"))
        {
        ReleaseButton(AI_EXIT);
        continue;
        }
        }
    break;
    }
    if (key == KEY_SPACE)
    {
    PressButton(AI_OPTIONS);
    result = ShowOptions();
    ReleaseButton(AI_OPTIONS);
    rFlag = 2;
    }

    mButtons = ReadMouseCursor(&my,&mx);

    if (key == KEY_F2)
    {
    EditMulti(1,mx,my);
    MouseReleased();
    rFlag = 1;
    }

    if (key == KEY_F3)
    {
    GetCode(mx,my);
    ShowCurrentBitmap();
    }

    if (mButtons & 2)
    {
    if (GetAction(mx,my) == 15)
        {
        OldCode = CurrentCode;
        CurrentCode = 0;
        PutCode(mx,my);
        CurrentCode = OldCode;
        rFlag = 1;
        }
    }

    UpdatePosn(mx,my);

    if (key == KEY_UP)
    {
    if (GridRow > 0)
        {
        GridRow--;
        rFlag = 1;
        }
    }

    if (key == KEY_RIGHT)
    {
    if (GridCol < MaxCol)
        {
        GridCol++;
        rFlag = 1;
        }
    }

    if (key == KEY_DOWN)
    {
    if (GridRow < MaxRow)
        {
        GridRow++;
        rFlag = 1;
        }
    }

    if (key == KEY_LEFT)
    {
    if (GridCol > 0)
        {
        GridCol--;
        rFlag = 1;
        }
    }

    if (key == KEY_PGUP && BaseColor > 0)
    {
    BaseColor--;
    HideMouse();
    ShowCurrentBitmap();
    ShowMouse();
    rFlag = 1;
    }

    if (key == KEY_PGDN && BaseColor < 255)
    {
    BaseColor++;
    HideMouse();
    ShowCurrentBitmap();
    ShowMouse();
    rFlag = 1;
    }

    if (mButtons & 1)
    {
    Action = GetAction(mx,my);

    switch (Action)
        {
        case AI_UP:
        if (GridRow > 0)
            {
            GridRow--;
            rFlag = 1;
            delay(50);
            }
        break;

        case AI_RIGHT:
        if (GridCol < MaxCol)
            {
            GridCol++;
            rFlag = 1;
            delay(50);

            }
        break;

        case AI_DOWN:
        if (GridRow < MaxRow)
            {
            GridRow++;
            rFlag = 1;
            delay(50);
            }
        break;

        case AI_LEFT:
        if (GridCol > 0)
            {
            GridCol--;
            rFlag = 1;
            delay(50);
            }
        break;

        case AI_MINI:
        GridRow = my - MINI_Y;
        GridCol = mx - MINI_X;
        if (GridCol > MaxCol)
            GridCol = MaxCol;
        if (GridRow > MaxRow)
            GridRow = MaxRow;
        rFlag = 1;
        delay(20);
        break;

        case AI_SELECT:
        PressButton(Action);
        SelectScreen();
        UpdateScreen();
        rFlag = 2;
        break;

        case AI_VIEW:
        ViewMode ^= VM_3D;
        if (ViewMode == VM_2D)
            {
            MaxCol = GRID_WIDTH-12;
            MaxRow = GRID_HEIGHT-9;
            }
        else
            {
            MaxCol = GRID_WIDTH-4;
            MaxRow = GRID_HEIGHT-4;
            }

        if (GridRow > MaxRow) GridRow = MaxRow;
        if (GridCol > MaxCol) GridCol = MaxCol;

        UpdateScreen();
        rFlag = 2;
        break;

        case AI_FLOORS:  // Floor
        ViewType = VT_FLOOR;
        rFlag = 2;
        break;

        case AI_SAVE:       // Save
        PressButton(AI_SAVE);

        if (access(MapName,0) == 0)
            {
            if (QueryBox(40,40,"Overwrite existing map?"))
            {
            ReleaseButton(AI_SAVE);
            rFlag = 2;
            break;
            }
            DrawGrid();
            }

        if (SaveGridMap(MapName) < 0)
            QueryBox(40,40,"Error saving map file");
        else
            QueryBox(40,40,"Map file saved");

        ReleaseButton(AI_SAVE);
        rFlag = 2;
        break;

        case AI_MAP:          // Map
        ViewType = VT_MAP;
        rFlag = 2;
        break;

        case AI_CEILING:          // Ceiling
        ViewType = VT_CEIL;
        rFlag = 2;
        break;

        case AI_WALLS:
        if (EditType != ED_WALLS)
            {
            ReleaseButton(AI_OBJECTS);
            EditType = ED_WALLS;
            PressButton(AI_WALLS);
            BitmapPtr = Bitmaps;
            LastObjCode = CurrentCode;
            CurrentCode = LastWallCode;
            ShowCurrentBitmap();
            MouseReleased();
            }
        break;

        case AI_OBJECTS:
        if (EditType != ED_OBJECTS)
            {
            ReleaseButton(AI_WALLS);
            EditType = ED_OBJECTS;
            PressButton(AI_OBJECTS);
            BitmapPtr = ObjBitmaps;
            LastWallCode = CurrentCode;
            CurrentCode = LastObjCode;
            ShowCurrentBitmap();
            MouseReleased();
            }
        break;

        case AI_OPTIONS:    // Options
        PressButton(AI_OPTIONS);
        result = ShowOptions();
        ReleaseButton(AI_OPTIONS);
        DrawGrid();
        MouseReleased();
        ProcessOptions(result);
        rFlag = 2;
        break;

        case AI_EDITWIN:
        PutCode(mx,my);
        rFlag = 1;
        if (EditType == ED_OBJECTS)
            rFlag = 2;
        break;

        case AI_BM_UP:
        CurrentCode--;
        if (CurrentCode > 255) CurrentCode = 255;
        ShowCurrentBitmap();
        delay(150);
        break;

        case AI_BM_DOWN:
        CurrentCode++;
        if (CurrentCode > 255) CurrentCode = 0;
        ShowCurrentBitmap();
        delay(150);
        break;

        case AI_EXIT:
        PressButton(Action);
        if (ModifyFlag)
            {
            if (QueryBox(20,40," Map has Changed, Exit Anyway? "))
            {
            ReleaseButton(Action);
            break;
            }
            }
        done = 1;
        break;

        default:
        break;

        }


    }

    if (rFlag)
    {
    DrawGrid();
    if (rFlag == 2)
        MouseReleased();

    rFlag = 0;
    }
    }

AckSetTextmode();
return 0;

}


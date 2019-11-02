/* Map Editor for Animation Construction Kit 3D */
/* Author: Lary Myers */

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
#include "mapedit.h"


#define GRID_FLOOR_FLAGS    (WALL_TYPE_UPPER+DOOR_TYPE_SECRET+DOOR_TYPE_SPLIT+DOOR_TYPE_SLIDE)

extern  unsigned char colordat[];


void smWriteString(short x,short y,char *s);
unsigned char * Readiff(char *picname);
        short       rsHandle;
    unsigned    char        *BkBuffer;
    unsigned    char        *smFont;
        short         ErrorCode;
        short         BigBitmap;
        short         GridX;
        short         GridY;
        short         LastObjCode;
        short         LastWallCode;
        short         CurrentType;
        short         CurrentCode;
        short         ModifiedFlag;
        short         GridFlag;
        short         PalSetFlag;
        UINT        WallFlags;

    unsigned    short         MapGrid[GRID_MAX+1];
    unsigned    short         ObjGrid[GRID_MAX+1];
    unsigned    short         FloorGrid[GRID_MAX+1];
    unsigned    short         CeilGrid[GRID_MAX+1];
    unsigned    short         *CurrentGrid;

    unsigned    char        *WallBitmaps[256];
    unsigned    char        *ObjBitmaps[256];

    unsigned    char        ObjbmNum[256];
    unsigned    char        Palette[768];

        char        GridFile[64];
        char        PalFile[64];

typedef struct {
        short     x;
        short     y;
        short     MapType;
unsigned    short     MapCode;
        char    *Text;
} SELECTBOX;



    SELECTBOX   Squares[] = {
        40,6,-1,0,"Choose one of the following",
        60,20,TYPE_WALL,0           ,"Normal square",
        60,30,TYPE_WALL,DOOR_TYPE_SLIDE     ,"Sliding door ",
        60,40,TYPE_WALL,DOOR_TYPE_SPLIT     ,"Split door   ",
        60,50,TYPE_WALL,DOOR_TYPE_SECRET    ,"Secret door  ",
        60,60,TYPE_WALL,DOOR_LOCKED     ,"Locked door  ",
        60,70,TYPE_WALL,MAP_STARTCODE       ,"Start square ",
        60,80,TYPE_WALL,MAP_UPCODE      ,"Up square    ",
        60,90,TYPE_WALL,MAP_DOWNCODE        ,"Down square  ",
        60,100,TYPE_WALL,MAP_GOALCODE       ,"Goal square  ",
        60,110,TYPE_WALL,WALL_TYPE_TRANS    ,"Transparent  ",
        60,120,TYPE_WALL,WALL_TYPE_MULTI    ,"Multi-Height ",
        60,130,TYPE_WALL,WALL_TYPE_UPPER    ,"Raised Wall  ",
        70,140,-2,0             ,"Cancel",
        -1,-1,-1,0,0
        };

    SELECTBOX   FillBox[] = {
        30,20,-1,0,"Fill will place the current",
        30,30,-1,0,"bitmap in all locations of",
        30,40,-1,0,"the map. Do you wish to do this?",
        60,60,-2,1," Yes ",
        110,60,-2,2," No ",
        -1,-1,-1,0,0
        };

    SELECTBOX   ClsBox[] = {
        40,20,-1,0,"Clear will erase the entire",
        40,30,-1,0,"map. Do you wish to do this?",
        60,50,-2,1," Yes ",
        110,50,-2,2," No ",
        -1,-1,-1,0,0
        };

    SELECTBOX   SaveBox[] = {
        40,20,-1,0,"Map has been modified.",
        40,30,-1,0,"Do you wish to save it.",
        60,50,-2,1," Yes ",
        110,50,-2,2," No ",
        -1,-1,-1,0,0
        };

    SELECTBOX   MapSavedBox[] = {
        55,50,-1,0,"Map has been saved.",
        80,70,-2,0," Okay ",
        -1,-1,-1,0,0
        };


    SELECTBOX   OverwriteBox[] = {
        40,30,-1,0,"File exist. Do you wish",
        40,40,-1,0,"overwrite it?",
        60,60,-2,1," Yes ",
        110,60,-2,2," No ",
        -1,-1,-1,0,0
        };

    SELECTBOX   LoadBox[] = {
        40,20,-1,0,"Load map will reload the",
        40,30,-1,0,"original map and erase",
        40,40,-1,0,"any changes. Do you wish",
        40,50,-1,0,"to do this?",
        60,70,-2,1," Yes ",
        110,70,-2,2," No ",
        -1,-1,-1,0,0
        };

    SELECTBOX   BorderBox[] = {
        40,20,-1,0,"Border draw will fill",
        40,30,-1,0,"the border with the",
        40,40,-1,0,"current bitmap. Do you",
        40,50,-1,0,"wish to do this?",
        60,70,-2,1," Yes ",
        110,70,-2,2," No ",
        -1,-1,-1,0,0
        };


    RECT    HotSpots[] = {
        2,2,        178,143,    /* 0 - Main grid */

        183,3,      196,14, /* 1 - Map Up arrow */
        186,134,    196,145,    /* 2 - Map Dn arrow */
        165,150,    181,160,    /* 3 - Map Rt arrow */
        3,150,      19,160, /* 4 - Map Lt arrow */

        241,0,      251,11, /* 5 - Box Up arrow */
        241,56,     251,67, /* 6 - Box Dn arrow */

        209,22,     231,40, /* 7 - Show multiple boxes */

        252,70,     283,94, /* 8 - Wall button */
        287,70,     318,94, /* 9 - Object button */
        252,97,     283,121,    /* 10 - Fill */
        287,97,     318,121,    /* 11 - Clear */
        252,124,    283,148,    /* 12 - Save */
        287,124,    318,148,    /* 13 - Load */
        287,151,    318,175,    /* 14 - Exit */

        217,70,     248,94, /* 15 - Horizontal door */
        217,97,     248,121,    /* 16 - Vertical door */

        193,48,     199,54, /* 17 - Transparent toggle */
        193,58,     199,64, /* 18 - MultiHeight toggle */

        217,151,    248,175,    /* 19 - Floor toggle */
        253,151,    283,175,    /* 20 - Ceiling toggle */
        217,124,    248,148,    /* 21 - Map toggle */
        -1,-1,      -1,-1   /* End of table */
        };


short MouseInstalled(void);
void ShowMouse(void);
void HideMouse(void);
short ReadMouseCursor(short *mrow,short *mcol);
short ReadMouseButtons(void);
void SetMouseCursor(short mrow,short mcol);
void SetMouseMinMaxColumns(short mincol,short maxcol);
void SetMouseMinMaxRows(short minrow,short maxrow);
void SetMouseShape(short hsrow,short hscol,char far *mask);
void MouseReleased(void);
void PutHex(char *buf,UINT mCode);


/****************************************************************************
**                                     **
****************************************************************************/
void ShowCheck(short x,short y,short x1,short y1,UCHAR color)
{
    UINT      offset;
    short     xp,yp;
    UCHAR   *Video;

xp = x;
yp = y;
Video = (char *)0xA0000;

while (xp <= x1)
    {
    offset = (yp * 320) + xp;
    Video[offset] = color;
    xp++;
    yp++;
    }

xp = x1;
yp = y;

while (xp >= x)
    {
    offset = (yp * 320) + xp;
    Video[offset] = color;
    xp--;
    yp++;
    }

}

/****************************************************************************
**                                     **
****************************************************************************/
void ShowHotspotCheck(short index,UCHAR color)
{
    short     x,y,x1,y1;


x = HotSpots[index].x + 1;
y = HotSpots[index].y + 1;
x1 = HotSpots[index].x1 - 1;
y1 = HotSpots[index].y1 - 1;

HideMouse();
ShowCheck(x,y,x1,y1,color);
ShowMouse();
}


/****************************************************************************
**                                     **
****************************************************************************/
void SoundBeep(void)
{
sound(440);
delay(50);
nosound();
}


/****************************************************************************
**                                     **
****************************************************************************/
short GetAction(short mx,short my)
{
    short     i = 0;

while (1)
    {
    if (HotSpots[i].x < 0)
    return(-1);

    if (mx >= HotSpots[i].x &&
    mx <= HotSpots[i].x1 &&
    my >= HotSpots[i].y &&
    my <= HotSpots[i].y1)
    break;

    i++;
    }

return(i);
}


/****************************************************************************
**                                     **
****************************************************************************/
short LoadAndShowScreen(void)
{
    unsigned    char *Video;

BkBuffer = Readiff("medit.lbm");

if (BkBuffer == NULL)
    return(-1);

Video = (char *)0xA0000;

HideMouse();
memmove(Video,&BkBuffer[4],64000);
ShowMouse();

return(0);
}

/****************************************************************************
**                                     **
****************************************************************************/
short LoadSmallFont(void)
{
    short     ht,wt,len;

smFont = Readiff("smfont.bbm");
if (smFont == NULL)
    return(-1);

ht = (*(short *)smFont);
wt = (*(short *)&smFont[2]);
len = ht * wt;
memmove(smFont,&smFont[4],len);

return(0);
}

/****************************************************************************
**                                     **
****************************************************************************/
void ClearMapArea(void)
{
    short     row;
    unsigned char *Video;

Video = (char *)0xA0000;
Video += 642;

HideMouse();
for (row = 2; row < 146; row++)
    {
    memset(Video,SCREEN_COLOR,180);
    Video += 320;
    }
ShowMouse();
}

/****************************************************************************
**                                     **
****************************************************************************/
void DrawBox(short x,short y,short x1,short y1,UCHAR color)
{
    UINT      offset,wt;
    unsigned char *Video;

offset = (y * 320) + x;
Video = (char *)0xA0000;
Video += offset;
wt = x1 - x;
memset(Video,color,wt+1);
Video += 320;

while (++y < y1)
    {
    *Video = color;
    Video[wt] = color;
    Video += 320;
    }

memset(Video,color,wt+1);

}

/****************************************************************************
**                                     **
****************************************************************************/
void ShowBitmap(short x,short y,unsigned char *bmp)
{
    short      row,col,brow,bcol;
    unsigned char *Video;

Video = (char *)0xA0000;
Video += ((y * 320) + x);
HideMouse();

if (BigBitmap)
    {
    brow = 0;
    for (row = 0; row < 64; row++)
    {
    bcol = brow * 128;
    for (col = 0; col < 64; col++)
        {
        Video[col] = bmp[bcol];
        bcol += 2;
        }
    Video += 320;
    brow += 2;
    }

    }
else
    {
    for (row = 0; row < 64; row++)
    {
    memmove(Video,bmp,64);
    bmp += 64;
    Video += 320;
    }
    }
ShowMouse();
}


/****************************************************************************
**                                     **
****************************************************************************/
void ShowSelectBox(SELECTBOX *sb)
{
    short     x1,y1;

HideMouse();
ClearMapArea();

while (sb->x != -1)
    {
    smWriteString(sb->x,sb->y,sb->Text);

    if (sb->MapType == -2)          /* A button */
    {
    x1 = sb->x + (strlen(sb->Text) * 4);
    y1 = sb->y + 5;
    DrawBox(sb->x - 2,sb->y - 2,x1+1,y1+1,BLACK);
    }
    sb++;
    }
ShowMouse();

}

/****************************************************************************
**                                     **
****************************************************************************/
UCHAR DetermineMapColor(short offset)
{
    UCHAR   color;
    unsigned short Mcode;

if (GridFlag && MapGrid[offset])
    return(LIGHTBLUE);


color = SCREEN_COLOR;
Mcode = CurrentGrid[offset];
if (!Mcode)
    {
    Mcode = ObjGrid[offset];
    if (Mcode & 0xFF)
    color = LIGHTBLUE;

    }
else
    {
    if (Mcode & DOOR_TYPE_SECRET)
    color = GREEN;

    if (Mcode & DOOR_TYPE_SLIDE)
    color = RED;

    if (Mcode & DOOR_TYPE_SPLIT)
    color = LIGHTRED;

    if (Mcode & WALL_TYPE_TRANS)
    color = LIGHTGREEN;

    if (Mcode & WALL_TYPE_MULTI)
    color = LIGHTMAGENTA;

    if (Mcode & DOOR_LOCKED)
    color++;
    }


return(color);
}

/****************************************************************************
**                                     **
****************************************************************************/
void ShowGrid(void)
{
    int       row,col,offset;
    int       x,y;
    char    buf[8];
    char    blank[3];
    UCHAR   color;
unsigned short  Mcode;

offset = (GridY * GRID_WIDTH) + GridX;
*blank = ' ';
blank[1] = ' ';
blank[2] = '\0';
y = 2;
HideMouse();
for (row = 0; row < 18; row++)
    {
    x = 2;
    for (col = 0; col < 18; col++)
    {
    DrawBox(x,y,x+10,y+8,BLACK);
    color = DetermineMapColor(offset);
    Mcode = CurrentGrid[offset];
    if (!Mcode)
        Mcode = ObjGrid[offset];

    if (Mcode & 0xFF)
        {
    //  sprintf(buf,"%02X",Mcode & 0xFF);
        PutHex(buf,Mcode & 0xFF);
        smWriteString(x+2,y+2,buf);
        }
    else
        smWriteString(x+2,y+2,blank);

    DrawBox(x+1,y+1,x+9,y+7,color);
    x += 10;
    offset++;
    }

    y += 8;
    offset += GRID_WIDTH - 18;
    }

ShowMouse();
}

/****************************************************************************
**                                     **
****************************************************************************/
void smWriteChar(short x,short y,unsigned char ch)
{
        UINT       FontOffset,VidOffset;
        UINT       row,col;
    unsigned    char *Video;

VidOffset = (y * 320) + x;
Video = (char *)0xA0000;
Video += VidOffset;
row = ch / 32;
col = ch - (row * 32);

FontOffset = ((row * 1344) + 192) + ((col * 6) + 1);

for (row = 0; row < 5; row++)
    {
    Video[0] = smFont[FontOffset];
    Video[1] = smFont[FontOffset + 1];
    Video[2] = smFont[FontOffset + 2];
    Video[3] = smFont[FontOffset + 3];
    Video += 320;
    FontOffset += 192;
    }

}

/****************************************************************************
**                                     **
****************************************************************************/
void smWriteString(short x,short y,char *s)
{
    char    ch;

HideMouse();
while (*s)
    {
    ch = toupper(*s++);
    smWriteChar(x,y,ch);
    x += 4;
    if (ch == 'M' || ch == 'N' || ch == 'W')
    x++;

    }
ShowMouse();

}

/****************************************************************************
**                                     **
****************************************************************************/
void ShowCoords(short x,short y)
{
    short     row,col;
    char    buf[12];

if (x < 2 || y < 2 || x > 182 || y > 146)
    return;

x -= 2;
y -= 2;

row = (y / 8) + GridY;
col = (x / 10) + GridX;

sprintf(buf,"%02d,%02d",col,row);
smWriteString(108,154,buf);

}

/****************************************************************************
**                                     **
****************************************************************************/
void ShowGridCoords(void)
{
    char    buf[10];

sprintf(buf,"%02d,%02d",GridX,GridY);
smWriteString(82,154,buf);

}


/****************************************************************************
**                                     **
****************************************************************************/
short FindSelectHit(SELECTBOX *sb,short mx,short my)
{
    short     index,x1,y1;

index = 0;

while (sb->x != -1)
    {
    if (sb->MapType != -1)
    {
    x1 = sb->x + (strlen(sb->Text) * 4);
    y1 = sb->y + 5;
    if (mx >= sb->x && my >= sb->y && mx <= x1 && my <= y1)
        return(index);

    }

    sb++;
    index++;
    }

return(-1);
}

/****************************************************************************
**                                     **
****************************************************************************/
void GetSpecialCode(short x,short y)
{
    short     GridOffset;
    short     mx,my,mbutton;
    short     index,done;
    UINT    MapCode;

x -= 2;
y -= 2;
GridOffset = (((y / 8) + GridY) * GRID_WIDTH) + (x / 10) + GridX;

ShowSelectBox(Squares);
done = 0;
MouseReleased();

while (!done)
    {
    mbutton = ReadMouseCursor(&my,&mx);

    if (mbutton & 1)
    {
    index = FindSelectHit(Squares,mx,my);

    if (index != -1)
        {
        if (Squares[index].MapType == -2)   /* Cancel */
        {
        done = 1;
        break;
        }

        MapCode = Squares[index].MapCode;

        if (Squares[index].MapType == TYPE_WALL)
        {
        if (!MapCode)
            CurrentGrid[GridOffset] &= 0x00FF;

        if (MapCode & 0xFF00)
            CurrentGrid[GridOffset] |= MapCode;
        else
            if (MapCode & 0xFF)
            {
            CurrentGrid[GridOffset] &= 0xFF00;
            CurrentGrid[GridOffset] |= MapCode;
            }
        }

        done = 1;
        }

    }
    }

MouseReleased();
ClearMapArea();
ShowGrid();
}

/****************************************************************************
**                                     **
****************************************************************************/
short GeneralSelectBox(SELECTBOX *sb,short DoBeep)
{
    short     done,index;
    short     result,mx,my,mbutton;

ShowSelectBox(sb);

if (DoBeep)
    SoundBeep();

result = done = 0;

while (!done)
    {
    mbutton = ReadMouseCursor(&my,&mx);
    if (mbutton & 1)
    {
    index = FindSelectHit(sb,mx,my);
    if (index != -1)
        {
        sb += index;
        result = sb->MapCode;
        done = 1;
        }
    }
    }

MouseReleased();
ClearMapArea();
ShowGrid();

return(result);
}

/****************************************************************************
**                                     **
****************************************************************************/
short DupObject(UINT Mcode)
{
    short     i;

Mcode &= 0xFF;

for (i = 0; i < GRID_MAX; i++)
    {
    if ((ObjGrid[i] & 0xFF) == Mcode)
    return(1);

    }

return(0);
}

/****************************************************************************
**                                     **
****************************************************************************/
void PutCode(short x,short y,UINT Mcode)
{
    short     row,col;
    short     GridOffset;
    UINT    mCode;
    UCHAR   color;
    char    buf[4];

if (CurrentType == TYPE_OBJECT && Mcode > 0 && DupObject(Mcode))
    {
    SoundBeep();
    return;
    }

x -= 2;
y -= 2;

if (x >= HotSpots[0].x1 ||
    x <= HotSpots[0].x ||
    y >= HotSpots[0].y1 ||
    y <= HotSpots[0].y)
    {
    return;
    }

row = (y / 8) + GridY;
col = (x / 10) + GridX;
if (row > 63 || col > 63 || row < 0 || col < 0)
    return;

GridOffset = (row * GRID_WIDTH) + col;

#if 0
if (GridFlag)
    {
    mCode = MapGrid[GridOffset];
    if (mCode > 0 && (!(mCode & GRID_FLOOR_FLAGS)))
    {
    SoundBeep();
    return;
    }
    }
#endif

if (CurrentType == TYPE_WALL)
    {
    CurrentGrid[GridOffset] &= 0xFF00;
    CurrentGrid[GridOffset] |= Mcode;
    if (Mcode && !GridFlag)
    CurrentGrid[GridOffset] |= WallFlags;
    }
else
    {
    if (!GridFlag)
    {
    ObjGrid[GridOffset] &= 0xFF00;
    ObjGrid[GridOffset] |= Mcode;
    }
    }

if (!Mcode)
    {
    CurrentGrid[GridOffset] = 0;
    if (!GridFlag)
    ObjGrid[GridOffset] = 0;
    }


color = DetermineMapColor(GridOffset);

row = ((y / 8) * 8);
col = ((x / 10) * 10);

if (Mcode & 0xFF)
//  sprintf(buf,"%02X",Mcode & 0xFF);
    PutHex(buf,Mcode & 0xFF);
else
    strcpy(buf,"  ");

col += 2;
row += 2;
HideMouse();
smWriteString(col + 2,row + 2,buf);
DrawBox(col+1,row+1,col+9,row+7,color);
ShowMouse();
ModifiedFlag = 1;
}

/****************************************************************************
**                                     **
****************************************************************************/
void ShowCurrentCode(void)
{
    short     row;
    UCHAR   color;
    char    buf[4];
    unsigned char *Video,*bmp;

//sprintf(buf,"%02X",CurrentCode & 0xFF);
PutHex(buf,CurrentCode & 0xFF);
HideMouse();
DrawBox(241,27,251,35,BLACK);
smWriteString(243,29,buf);
if (CurrentType == TYPE_WALL)
    {
    color = SCREEN_COLOR;
    bmp = WallBitmaps[CurrentCode & 0xFF];
    }
else
    {
    color = LIGHTBLUE;
    bmp = ObjBitmaps[ObjbmNum[CurrentCode & 0xFF]];
    }

DrawBox(242,28,250,34,color);

if (bmp != NULL)
    ShowBitmap(254,2,bmp);
else
    {
//  Video = MK_FP(0xA000,(2 * 320) + 254);
    Video = (char *)0xA0000;
    Video += 894;
    for (row = 0; row < 64; row++)
    {
    memset(Video,SCREEN_COLOR,64);
    Video += 320;
    }
    SoundBeep();
    }

ShowMouse();
}

/****************************************************************************
**                                     **
****************************************************************************/
void ShowButton(short x,short y,char *Text)
{

smWriteString(x,y,Text);
DrawBox(x-2,y-2,x + (strlen(Text) * 4) + 1,y + 6,BLACK);

}

/****************************************************************************
**                                     **
****************************************************************************/
void ShowNextGroup(short StartCode)
{
    short     i,x,y;
unsigned    char      cCode;
unsigned    char    *bmp,*Video;
        char    buf[10];

x = 0;
cCode = StartCode & 0xFF;

HideMouse();
for (i = 0; i < 5; i++)
    {
    if (CurrentType == TYPE_WALL)
    bmp = WallBitmaps[cCode];
    else
    bmp = ObjBitmaps[ObjbmNum[cCode]];

    if (bmp != NULL)
    ShowBitmap(x,0,bmp);
    else
    {
//  Video = MK_FP(0xA000,x);
    Video = (char *)0xA0000;
    Video += x;
    for (y = 0; y < 64; y++)
        {
        memset(Video,SCREEN_COLOR,64);
        Video += 320;
        }
    DrawBox(x,0,x+63,63,BLACK);
    }

//  sprintf(buf,"%02X",cCode);
    PutHex(buf,cCode);
    ShowButton(x+30,70,buf);

    if (CurrentType == TYPE_WALL)
    {
    smWriteString(x,80,"                ");

    if (cCode == DOOR_XCODE)
        smWriteString(x,80,"Vertical door");
    if (cCode == DOOR_YCODE)
        smWriteString(x,80,"Horizontal door");
    if (cCode == DOOR_SIDECODE)
        smWriteString(x,80,"Door side");
    }

    cCode++;
    if (CurrentType == TYPE_OBJECT && cCode >= MAX_OBJECTS)
    cCode = 0;

    x += 64;
    }
ShowMouse();

}


/****************************************************************************
**                                     **
****************************************************************************/
void ShowBitmapGroup(void)
{
         short    i,x,y;
         short    mbutton,mx,my;
         short    StartCode;
    unsigned char *Video,*bmp;

Video = (char *)0xA0000;

MouseReleased();
HideMouse();
memmove(BkBuffer,Video,64000);
memset(Video,SCREEN_COLOR,64000);

StartCode = CurrentCode;
ShowNextGroup(StartCode);

ShowButton(6,100,"<- Previous");
ShowButton(150,100,"Cancel");
ShowButton(280,100,"Next ->");

ShowMouse();


while (1)
    {
    mbutton = ReadMouseCursor(&my,&mx);
    if (mbutton & 1)
    {
    if (mx > 5 && mx < 50 && my > 99 && my < 109)
        {
        StartCode -= 1;
        if (CurrentType == TYPE_WALL)
        StartCode &= 0xFF;
        else
        {
        if (StartCode < 0)
            StartCode = MAX_OBJECTS - 1;
        }
        ShowNextGroup(StartCode);
        delay(90);
        continue;
        }

    if (mx > 149 && mx < 174 && my > 99 && my < 109)
        break;


    if (mx > 279 && mx < 309 && my > 99 && my < 109)
        {
        StartCode += 1;
        if (CurrentType == TYPE_WALL)
        StartCode &= 0xFF;
        else
        {
        if (StartCode >= MAX_OBJECTS)
            StartCode = 0;
        }
        ShowNextGroup(StartCode);
        delay(90);
        continue;
        }

    if (my < 64)
        {
        mx /= 64;
        StartCode += mx;
        if (CurrentType == TYPE_WALL)
        StartCode &= 0xFF;
        else
        {
        if (StartCode >= MAX_OBJECTS)
            StartCode -= MAX_OBJECTS;
        }
        CurrentCode = StartCode;
        break;
        }

    }

    }

HideMouse();
memmove(Video,BkBuffer,64000);
ShowCurrentCode();
ShowMouse();
MouseReleased();

}

/****************************************************************************
**                                     **
****************************************************************************/
char *GetExtent(char *s)
{
    char    *e;

e = strchr(s,'.');
if (e == NULL)
    return(s);
e++;

return(e);
}

/****************************************************************************
**                                     **
****************************************************************************/
short LoadBitmap(short BitmapNumber,char *BitmapName,short BitmapType)
{
    short     LoadType;
    short     handle;
    short     x,y;
    short     sPos,dPos;
    unsigned char ch;
    unsigned char *bmp;

LoadType = 0;
if (!(stricmp(GetExtent(BitmapName),"BBM")))
    LoadType = 1;

if (!(stricmp(GetExtent(BitmapName),"GIF")))
    LoadType = 2;


if (LoadType)
    {
    if (LoadType == 1)
    bmp = Readiff(BitmapName);
    else
    bmp = AckReadgif(BitmapName);

    if (bmp == NULL)
    {
    ErrorCode = ERR_NOMEMORY;
    return(-1);
    }

    if (BitmapType == TYPE_WALL)
    WallBitmaps[BitmapNumber] = bmp;

    if (BitmapType == TYPE_OBJECT)
    ObjBitmaps[BitmapNumber] = bmp;

    if ((*(short *)bmp) == 128)
    {
    BigBitmap = 1;
    memmove(bmp,&bmp[4],16384);
    }
    else
    memmove(bmp,&bmp[4],4096);

    return(0);
    }


bmp = malloc(4096);
if (bmp == NULL)
    {
    ErrorCode = ERR_NOMEMORY;
    return(-1);
    }

if (BitmapType == TYPE_WALL)
    WallBitmaps[BitmapNumber] = bmp;

if (BitmapType == TYPE_OBJECT)
    ObjBitmaps[BitmapNumber] = bmp;


handle = open(BitmapName,O_RDWR|O_BINARY);
if (handle < 1)
    {
    free(bmp);
    ErrorCode = ERR_BADFILE;
    return(-1);
    }

read(handle,bmp,4);     /* Skip width and height for now */
read(handle,bmp,4096);
close(handle);

return(0);
}

/****************************************************************************
**                                     **
****************************************************************************/
char *StripEndOfLine(char *s)
{
    short     len;
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

/****************************************************************************
**                                     **
****************************************************************************/
char *SkipSpaces(char *s)
{

while (*s == ' ' || *s == '\t' || *s == ',')
    strcpy(s,&s[1]);

return(s);
}

/****************************************************************************
**                                     **
****************************************************************************/
char *AddExtent(char *s,char *ext)
{
if (strchr(s,'.') == NULL)
    strcat(s,ext);

return(s);
}

/****************************************************************************
**                                     **
****************************************************************************/
char *CopyToComma(char *dest,char *src)
{
    char    ch;

while (*src)
    {
    ch = *src++;
    if (ch == ' ' || ch == '\t' || ch == ',')
    break;

    *dest++ = ch;
    }

*dest = '\0';

return(src);
}



/****************************************************************************
**                                     **
****************************************************************************/
short LoadDescFile(char *fName)
{
    FILE    *fp;
    short     Mode,fMode,result;
    short     bType,value,bNum,ObjIndex;
    char    LineBuf[128];
    char    fBuf[128];
    char    *s;

fp = fopen(fName,"rt");
if (fp == NULL)
    {
    printf("Unable to open description file: %s\n",fName);
    return(-1);
    }

printf("Processing description file %s ",fName);


ObjIndex = 0;
Mode = 0;
result = 0;
*GridFile = '\0';

while (1)
    {
    if (feof(fp))
    break;

    *LineBuf = '\0';
    fgets(LineBuf,127,fp);

    if (*LineBuf == ';')
    continue;

    StripEndOfLine(LineBuf);
    SkipSpaces(LineBuf);

    if (!strlen(LineBuf))
    continue;

    printf(".");

    if (!stricmp(LineBuf,"WALLS:"))
    {
    bType = TYPE_WALL;
    Mode = 1;
    continue;
    }

    if (!stricmp(LineBuf,"ENDWALLS:"))
    {
    if (Mode != 1)
        {
        printf("Invalid place for command: %s.\n",LineBuf);
        result = -1;
        }

    Mode = 0;
    continue;
    }

    if (!stricmp(LineBuf,"OBJECTS:"))
    {
    bType = TYPE_OBJECT;
    Mode = 2;
    continue;
    }

    if (!stricmp(LineBuf,"FILES:"))
    {
    fMode = 1;
    continue;
    }

    if (!stricmp(LineBuf,"ENDFILES:"))
    {
    fMode = 0;
    continue;
    }

    if (!strnicmp(LineBuf,"PALFILE:",8))
    {
    strcpy(PalFile,SkipSpaces(&LineBuf[8]));
    continue;
    }

    if (!strnicmp(LineBuf,"MAPFILE:",8))
    {
    strcpy(GridFile,SkipSpaces(&LineBuf[8]));
    continue;
    }

    if (Mode == 2)
    {
    if (!strnicmp(LineBuf,"NUMBER:",7))
        {
        value = atoi(&LineBuf[7]);

        if (value < 1 || value >= 255)
        {
        printf("Invalid object number:\n%s\n",LineBuf);
        result = -1;
        break;
        }
        ObjIndex = value;
        continue;
        }

    if (!strnicmp(LineBuf,"BITMAPS:",8))
        {
        strcpy(LineBuf,SkipSpaces(&LineBuf[8]));
        value = 0;
        strcpy(LineBuf,CopyToComma(fBuf,LineBuf));
        SkipSpaces(fBuf);
        bNum = atoi(fBuf);

        if (bNum < 1 || bNum > 255)
        {
        printf("Invalid bitmap number for object: %d\n",ObjIndex);
        result = -1;
        break;
        }

        ObjbmNum[ObjIndex] = bNum;
        continue;
        }
    }

    if (fMode)
    {
    value = atoi(LineBuf);
    if (value < 1 || value > 255)
        {
        printf("Invalid number for object: %s.\n",LineBuf);
        result = -1;
        continue;
        }

    s = strpbrk(LineBuf,", \t");
    if (s == NULL)
        {
        printf("Unable to locate bitmap name for object: %s.\n",LineBuf);
        result = -1;
        continue;
        }

    strcpy(fBuf,SkipSpaces(s));
    AddExtent(fBuf,".img");

    if (LoadBitmap(value,fBuf,bType))
        {
        printf("Error loading bitmap \"%s\".\n",fBuf);
        result = -1;
        }
    continue;
    }

    }

fclose(fp);

printf("done\n");

return(result);
}

/****************************************************************************
**                                     **
****************************************************************************/
short LoadGrid(void)
{
    short     handle;

handle = open(GridFile,O_RDWR|O_BINARY);
if (handle < 1)
    {
    printf("Unable to open MapFile: %s\n",GridFile);
    return(-1);
    }

if (read(handle,MapGrid,8192) != 8192)
    {
    close(handle);
    printf("Error reading MapFile: %s\n",GridFile);
    return(-1);
    }

if (read(handle,ObjGrid,8192) != 8192)
    {
    close(handle);
    printf("Error reading MapFile: %s\n",GridFile);
    return(-1);
    }

read(handle,FloorGrid,8192);
read(handle,CeilGrid,8192);

close(handle);
return(0);
}


/****************************************************************************
**                                     **
****************************************************************************/
short SaveGrid(void)
{
    short     handle;

handle = open(GridFile,O_RDWR|O_BINARY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
if (handle < 1)
    {
    return(-1);
    }

if (write(handle,MapGrid,8192) != 8192)
    {
    close(handle);
    return(-2);
    }

if (write(handle,ObjGrid,8192) != 8192)
    {
    close(handle);
    return(-2);
    }

write(handle,FloorGrid,8192);
write(handle,CeilGrid,8192);

close(handle);
ModifiedFlag = 0;
GeneralSelectBox(MapSavedBox,0);
return(0);
}



/****************************************************************************
**                                     **
****************************************************************************/
short LoadPalette(char *pName)
{
    short     handle;

handle = open(pName,O_RDWR|O_BINARY);
if (handle < 1)
    {
    printf("Unable to open PalFile: %s\n",pName);
    ErrorCode = ERR_BADPALFILE;
    return(-1);
    }

if (read(handle,Palette,768) != 768)
    {
    close(handle);
    printf("Error reading PalFile: %s\n",pName);
    ErrorCode = ERR_BADPALFILE;
    return(-1);
    }

close(handle);
return(0);
}

/****************************************************************************
**                                     **
****************************************************************************/
void FillBorder(void)
{
    short   row,col;
    UINT    offset;

offset = 0;
for (col = 0; col < 64; col++)
    MapGrid[col] = CurrentCode;

for (row = 0; row < 64; row++)
    {
    MapGrid[offset] = CurrentCode;
    MapGrid[offset+63] = CurrentCode;
    offset += 64;
    }

offset = 64 * 63;
for (col = 0; col < 64; col++)
    MapGrid[offset++] = CurrentCode;

ShowGrid();

}


/****************************************************************************
**                                     **
****************************************************************************/
short main(short argc,char *argv[])
{
    short     done,mx,my,mbutton,lastx,lasty,index;
    short     i;
    unsigned short key;
    UCHAR   color;
    char    *s;

if (MouseInstalled() != -1)
    {
    puts("Mouse required");
    exit(1);
    }

if (LoadSmallFont())
    {
    puts("Unable to load font file");
    exit(1);
    }

if (argc < 2)
    {
    printf("ACK-3D Map Editor Version 1.0\n");
    printf("Usage:\n");
    printf("   medit ascfile.ext\n");
    printf("   where:\n");
    printf("         ascfile.ext is the name of the ASCII file that contains\n");
    printf("                     wall and object bitmap filenames.\n");
    exit(1);
    }

if (LoadDescFile(argv[1]))
    {
    printf("\nError reading ASCII file - ErrorCode = %d\n",ErrorCode);
    exit(1);
    }

if (LoadGrid())
    {
    printf("\nError loading map file - ErrorCode = %d\n",ErrorCode);
    exit(1);
    }

if (LoadPalette(PalFile))
    {
    printf("\nError loading palette file - ErrorCode = %d\n",ErrorCode);
    exit(1);
    }

SetVGAmode();

CurrentGrid = &MapGrid[0];
GridFlag = 0;

LoadAndShowScreen();

//SetPalette2(&Palette[16 * 3]);
SetPalette2(Palette,256);

WallFlags = 0;
ShowHotspotCheck(17,SCREEN_COLOR);
ShowHotspotCheck(18,SCREEN_COLOR);

GridX = GridY = 0;
CurrentType = TYPE_WALL;
LastWallCode = LastObjCode = CurrentCode = 1;

ShowMouse();
mbutton = ReadMouseCursor(&my,&mx);
lastx = mx;
lasty = my;

ShowCoords(lastx,lasty);

ShowGrid();
ShowCurrentCode();

//smWriteString(4,194,"Press spacebar to select square options");
s = GetExtent(argv[1]);
smWriteString(82,166,&s[1]);

smWriteString(82,178,"Map    ");
done = 0;

while (!done)
    {
    key = inkey();
    if (key == 0x11B)
    break;

    if (key == 0x3200)
    {
    CurrentGrid = &MapGrid[0];
    GridFlag = 0;
    ShowGrid();
    smWriteString(82,178,"Map    ");
    }

    if (key == 0x2100)
    {
    CurrentGrid = &FloorGrid[0];
    GridFlag = 1;
    ShowGrid();
    smWriteString(82,178,"Floor  ");
    }

    if (key == 0x2E00)
    {
    CurrentGrid = &CeilGrid[0];
    GridFlag = 2;
    ShowGrid();
    smWriteString(82,178,"Ceiling");
    }

    mbutton = ReadMouseCursor(&my,&mx);

    if (lastx != mx || lasty != my)
    {
    lastx = mx;
    lasty = my;
    ShowCoords(lastx,lasty);
    }

    if (key == 0x3920 && !GridFlag)
    {
    if (mx > 1 && mx < 183 && my > 1 && my < 147)
        {
        GetSpecialCode(mx,my);
        SetMouseCursor(my,mx);
        }
    else
        SoundBeep();
    }


    if (key == 0x3000 && !GridFlag)
    {
    if (GeneralSelectBox(BorderBox,1) == 1)
        FillBorder();
    }

    if (mbutton & 2)
    PutCode(mx,my,0);


    if (mbutton & 1)
    {
    index = GetAction(mx,my);

    switch (index)
        {

        case 0:
        PutCode(mx,my,CurrentCode);
        if (CurrentType == TYPE_OBJECT)
            MouseReleased();

        if (CurrentCode == DOOR_YCODE || CurrentCode == DOOR_XCODE)
            {
            GetSpecialCode(mx,my);
            SetMouseCursor(my,mx);
            }

        break;

        case 1:     /* Map Up arrow */
        if (GridY)
            {
            GridY--;
            ShowGrid();
            ShowGridCoords();
            ShowCoords(mx,my);
            }
        break;

        case 2:     /* Map Dn arrow */
        if (GridY < (GRID_HEIGHT - 18))
            {
            GridY++;
            ShowGrid();
            ShowGridCoords();
            ShowCoords(mx,my);
            }
        break;


        case 3:     /* Map Rt arrow */
        if (GridX < (GRID_WIDTH - 18))
            {
            GridX++;
            ShowGrid();
            ShowGridCoords();
            ShowCoords(mx,my);
            }
        break;

        case 4:     /* Map Lt arrow */
        if (GridX)
            {
            GridX--;
            ShowGrid();
            ShowGridCoords();
            ShowCoords(mx,my);
            }
        break;

        case 5:     /* Bitmap Up arrow */
        CurrentCode -= 1;
        if (CurrentType == TYPE_WALL)
            {
            CurrentCode &= 0xFF;
            LastWallCode = CurrentCode;
            }
        else
            {
            if (CurrentCode < 0)
            CurrentCode = MAX_OBJECTS - 1;
            LastObjCode = CurrentCode;
            }

        ShowCurrentCode();
        delay(200);
        break;

        case 6:     /* Bitmap Dn arrow */
        CurrentCode += 1;
        if (CurrentType == TYPE_WALL)
            {
            CurrentCode &= 0xFF;
            LastWallCode = CurrentCode;
            }
        else
            {
            if (CurrentCode >= MAX_OBJECTS)
            CurrentCode = 0;
            LastObjCode = CurrentCode;
            }
        ShowCurrentCode();
        delay(200);
        break;

        case 7:     /* Show Bitmap Group */
        ShowBitmapGroup();
        if (CurrentType == TYPE_WALL)
            LastWallCode = CurrentCode;
        else
            LastObjCode = CurrentCode;

        break;

        case 8:     /* Wall button */
        CurrentType = TYPE_WALL;
        CurrentCode = LastWallCode;
        ShowCurrentCode();
        MouseReleased();
        break;

        case 9:     /* Object button */
        CurrentType = TYPE_OBJECT;
        CurrentCode = LastObjCode;
        ShowCurrentCode();
        MouseReleased();
        break;


        case 10:        /* Fill */
        if (GeneralSelectBox(FillBox,1) == 2)
            break;

        switch (GridFlag)
            {
            case 0:
            for (i = 0; i < GRID_MAX; i++)
                {
                CurrentGrid[i] = CurrentCode;
                ObjGrid[i] = 0;
                }
            break;

            case 1:
            for (i = 0; i < GRID_MAX; i++)
                {
                if (!MapGrid[i] || (MapGrid[i] & GRID_FLOOR_FLAGS))
                FloorGrid[i] = CurrentCode;
                else
                   FloorGrid[i] = MapGrid[i] & 0xFF;
                }
            break;

            case 2:
            for (i = 0; i < GRID_MAX; i++)
                {
                if (!MapGrid[i] || (MapGrid[i] & GRID_FLOOR_FLAGS))
                CeilGrid[i] = CurrentCode;
                else
                CeilGrid[i] = MapGrid[i] & 0xFF;
                }
            break;

            default:
            break;
            }

        ModifiedFlag = 1;
        GridX = GridY = 0;
        ShowGrid();
        ShowGridCoords();
        ShowCoords(mx,my);
        break;

        case 11:        /* Clear */
        if (GeneralSelectBox(ClsBox,1) == 2)
            break;

        if (!GridFlag)
            {
            for (i = 0; i < GRID_MAX; i++)
            {
            CurrentGrid[i] = 0;
            ObjGrid[i] = 0;
            }
            }
        else
            {
            if (GridFlag == 1)
            {
            for (i = 0; i < GRID_MAX; i++)
                FloorGrid[i] = 0;
            }
            if (GridFlag == 2)
            {
            for (i = 0; i < GRID_MAX; i++)
                CeilGrid[i] = 0;
            }
            }

        ModifiedFlag = 1;
        GridX = GridY = 0;
        ShowGrid();
        ShowGridCoords();
        ShowCoords(mx,my);
        break;

        case 12:        /* Save */

        if (!access(GridFile,0))
            {
            if (GeneralSelectBox(OverwriteBox,1) == 2)
            break;
            }

        SaveGrid();
        break;

        case 13:        /* Load */
        if (ModifiedFlag)
            {
            if (GeneralSelectBox(LoadBox,1) == 2)
            break;
            }

        if (!LoadGrid())
            {
            ModifiedFlag = 0;
            GridX = GridY = 0;
            CurrentType = TYPE_WALL;
            CurrentCode = 1;
            ShowGrid();
            ShowCurrentCode();
            }
        else
            SoundBeep();
        break;

        case 14:        /* Exit */
        done = 1;
        break;

        case 15:        /* Horiz door */
        CurrentCode = DOOR_YCODE;
        ShowCurrentCode();
        MouseReleased();
        break;

        case 16:
        CurrentCode = DOOR_XCODE;
        ShowCurrentCode();
        MouseReleased();
        break;

        case 17:        /* Trans toggle */
        WallFlags ^= WALL_TYPE_TRANS;
        color = SCREEN_COLOR;
        if (WallFlags & WALL_TYPE_TRANS)
            color = BLACK;
        ShowHotspotCheck(17,color);
        MouseReleased();
        break;

        case 18:        /* Multi toggle */
        WallFlags ^= WALL_TYPE_MULTI;
        color = SCREEN_COLOR;
        if (WallFlags & WALL_TYPE_MULTI)
            color = BLACK;
        ShowHotspotCheck(18,color);
        MouseReleased();
        break;

        case 19:        /* Show Floors */
        CurrentGrid = &FloorGrid[0];
        GridFlag = 1;
        ShowGrid();
        smWriteString(82,178,"Floor  ");
        MouseReleased();
        break;

        case 20:        /* Show Ceiling */
        CurrentGrid = &CeilGrid[0];
        GridFlag = 2;
        ShowGrid();
        smWriteString(82,178,"Ceiling");
        MouseReleased();
        break;

        case 21:        /* Show Map */
        CurrentGrid = &MapGrid[0];
        GridFlag = 0;
        ShowGrid();
        smWriteString(82,178,"Map    ");
        MouseReleased();
        break;


        default:
        break;
        }
    }

    }


if (ModifiedFlag)
    {
    if (GeneralSelectBox(SaveBox,1) == 1)
    SaveGrid();
    }

SetTextMode();

return(0);
}


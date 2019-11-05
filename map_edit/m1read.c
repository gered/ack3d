// Map Editor for Animation Construction Kit 3D
// ASCII Reader program
// Author: Lary Myers
// Copyright (c) 1994

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <sys\stat.h>
#include "ack3d.h"
#include "ackeng.h"


extern      UCHAR   *Bitmaps[];
extern      UCHAR   *ObjBitmaps[];
extern      UCHAR   ObjbmNum[];
extern      char    MapName[];
extern      char    PalName[];

        short   ReadErrorCode;

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
if (!(stricmp(GetExtent(BitmapName),"PCX")))
    LoadType = 3;


if (LoadType)
    {
    switch (LoadType) {
    case 1:
        bmp = AckReadiff(BitmapName);
        break;
    case 2:
        bmp = AckReadgif(BitmapName);
        break;
    case 3:
        bmp = AckReadPCX(BitmapName);
        break;
    }

    if (bmp == NULL)
    {
    ReadErrorCode = ERR_NOMEMORY;
    return(-1);
    }

    if (BitmapType == TYPE_WALL)
    Bitmaps[BitmapNumber] = bmp;

    if (BitmapType == TYPE_OBJECT)
    ObjBitmaps[BitmapNumber] = bmp;

    return(0);
    }


bmp = (unsigned char*)malloc(4096);
if (bmp == NULL)
    {
    ReadErrorCode = ERR_NOMEMORY;
    return(-1);
    }

if (BitmapType == TYPE_WALL)
    Bitmaps[BitmapNumber] = bmp;

if (BitmapType == TYPE_OBJECT)
    ObjBitmaps[BitmapNumber] = bmp;


handle = open(BitmapName,O_RDWR|O_BINARY);
if (handle < 1)
    {
    free(bmp);
    ReadErrorCode = ERR_BADFILE;
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
*MapName = '\0';

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
    strcpy(PalName,SkipSpaces(&LineBuf[8]));
    continue;
    }

    if (!strnicmp(LineBuf,"MAPFILE:",8))
    {
    strcpy(MapName,SkipSpaces(&LineBuf[8]));
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


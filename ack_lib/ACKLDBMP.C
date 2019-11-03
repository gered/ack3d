
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

char *GetExtent(char *s);
UCHAR *AckReadiff(char *s);
UCHAR *AckReadPCX(char *s);
short BlankSlice(short,UCHAR *);

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Determines if the column of the bitmap contains all transparent colors
// or not. If so then it is marked to be skipped during the draw phase.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short BlankSlice(short col,UCHAR *bmp)
{
    short     i,pos;

pos = col * 64;
for (i = 0; i < 64; i++)
    {
    if (bmp[pos++])
    return(1);
    }

return(0);
}


//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Loads a bitmap of different formats based on the setting of bmLoadType
// in the ACKENG interface structure. The bitmap loaded is placed into
// either the wall bitmap array or the object array based on the value
// of BitmapType passed to this function.
// BitmapName can be either a filename or an index into the currently
// open resource file.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckLoadBitmap(ACKENG *ae,short BitmapNumber,short BitmapType,char *BitmapName)
{
    int      handle;
    short    bFlag;
    short    x,y,bLen;
    short    sPos,dPos;
    UCHAR    ch;
    UCHAR    *buf;
    UCHAR    *bmp;
    UCHAR    *bmpFlags;

bFlag = 0;

bLen = BITMAP_SIZE + BITMAP_WIDTH;
buf = NULL;

if (ae->bmLoadType == BMLOAD_BBM)
    buf = AckReadiff(BitmapName);

if (ae->bmLoadType == BMLOAD_GIF)
    buf = AckReadgif(BitmapName);

if (ae->bmLoadType == BMLOAD_PCX)
    buf = AckReadPCX(BitmapName);

if (buf == NULL)
    return(ERR_LOADINGBITMAP);

x = (*(short *)buf);
y = (*(short *)&buf[2]);
if ((x*y) != BITMAP_SIZE)
    {
    AckFree(buf);
    return(ERR_INVALIDFORM);
    }

memmove(buf,&buf[4],BITMAP_SIZE);
bFlag = 1;

bmp = (UCHAR*)AckMalloc(bLen);
if (bmp == NULL)
    {
    AckFree(buf);
    return(ERR_NOMEMORY);
    }

if (BitmapType == TYPE_WALL)
    {
    ae->bMaps[BitmapNumber] = bmp;
    }

if (BitmapType == TYPE_OBJECT)
    {
    ae->oMaps[BitmapNumber] = bmp;
    }

if (!bFlag)
    {
    handle = open(BitmapName,O_RDONLY|O_BINARY);
    if (handle < 1)
        {
        AckFree(buf);
        AckFree(bmp);
        return(ERR_BADFILE);
        }

    read(handle,buf,4);     // Skip width and height for now
    read(handle,buf,BITMAP_SIZE);
    close(handle);
    }

for (y = 0; y < BITMAP_HEIGHT; y++)
    {
    sPos = y;
    dPos = y * BITMAP_WIDTH;
    for (x = 0; x < BITMAP_WIDTH; x++)
        {
        ch = buf[sPos];
        bmp[dPos++] = ch;
        sPos += BITMAP_WIDTH;
        }
    }


bmpFlags = &bmp[BITMAP_SIZE];
memset(bmpFlags,0,BITMAP_WIDTH);

for (x = 0; x < BITMAP_WIDTH; x++)
    {
    if (!BlankSlice(x,bmp))
    bmpFlags[x] = 1;

    }

AckFree(buf);
return(0);
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Returns a pointer to the file extent
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
char *GetExtent(char *s)
{
    char    *e;

e = strchr(s,'.');
if (e == NULL)
    return(s);
e++;

return(e);
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Calls AckLoadBitmap with the TYPE_WALL flag set so the bitmap is placed
// in the wall array.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckLoadWall(ACKENG *ae,short WallNumber,char *bmFileName)
{
return( AckLoadBitmap(ae,WallNumber,TYPE_WALL,bmFileName) );
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Calls AckLoadBitmap with the TYPE_OBJECT flag set so the bitmap is
// placed in the object array.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckLoadObject(ACKENG *ae,short BmpNumber,char *bmFileName)
{
return( AckLoadBitmap(ae,BmpNumber,TYPE_OBJECT,bmFileName) );
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Creates an object structure. This function MUST be called before the
// object data can be initialized in the NEWOBJECT structure.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckCreateObject(ACKENG *ae,short ObjNumber)
{

if (ae->ObjList[ObjNumber] == NULL)
    {
    ae->ObjList[ObjNumber] = (NEWOBJECT *)AckMalloc(sizeof(NEWOBJECT));

    if (ae->ObjList[ObjNumber] == NULL)
    return(ERR_NOMEMORY);

    memset(ae->ObjList[ObjNumber],0,sizeof(NEWOBJECT));
    }

if (ObjNumber >= ae->MaxObjects)
    ae->MaxObjects = ObjNumber + 1;

return(0);
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Sets an object up into one of the predefined phase types (CREATE,DESTROY,
// etc.). Moveable objects are placed into a special list that is used
// later in the drawing phase.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckSetObjectType(ACKENG *ae,short oNum,short oType)
{
    short   i,j,result = 0;
    OBJSEQ  *os;


switch (oType)
    {
    case NO_CREATE:
    os = &ae->ObjList[oNum]->Create;
    break;

    case NO_DESTROY:
    os = &ae->ObjList[oNum]->Destroy;
    break;

    case NO_WALK:
    os = &ae->ObjList[oNum]->Walk;
    break;

    case NO_ATTACK:
    os = &ae->ObjList[oNum]->Attack;
    break;

    case NO_INTERACT:
    os = &ae->ObjList[oNum]->Interact;
    break;

    default:
    result = ERR_BADOBJTYPE;
    break;
    }

if (!result)
    {
    ae->ObjList[oNum]->CurrentBitmaps = (UCHAR *)&os->bitmaps;
    ae->ObjList[oNum]->Flags = os->flags;
    ae->ObjList[oNum]->Sides = os->bmSides;
    ae->ObjList[oNum]->BitmapsPerView = os->bmBitmapsPerView;
    ae->ObjList[oNum]->CurrentBm = 0;
    ae->ObjList[oNum]->Maxbm = os->MaxBitmaps;
    ae->ObjList[oNum]->CurrentType = oType;
    ae->ObjList[oNum]->aFactor = os->AngleFactor;
    }

if (ae->ObjList[oNum]->Flags & OF_MOVEABLE)
    {
    j = 0;
    for (i = 0; i < MoveObjectCount; i++)
    {
    if (MoveObjectList[i] == oNum)
        {
        j = 1;
        break;
        }
    }

    if (!j)
    MoveObjectList[MoveObjectCount++] = oNum;

    i = (ae->ObjList[oNum]->y & 0xFFC0) + (ae->ObjList[oNum]->x >> 6);
    ObjGrid[i] = 0;
    }

return(result);
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Fills in the object structure with a communication structure passed
// by the application. This allows the application to setup the fields
// such as number of sides to an object, what bitmaps are displayed for
// each side, etc. The object structures are defined in ACK3D.H
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckSetupObject(ACKENG *ae,short oNum,short oType,OBJSEQ *os)
{
    short   result = 0;

if (ae->ObjList[oNum] == NULL)
    return(ERR_BADOBJECTNUM);

if (os->flags & OF_MULTIVIEW)
    {
    os->AngleFactor = INT_ANGLE_360 / os->bmSides;
    }

switch (oType)
    {
    case NO_CREATE:
    memmove(&ae->ObjList[oNum]->Create,os,sizeof(OBJSEQ));
    break;

    case NO_DESTROY:
    memmove(&ae->ObjList[oNum]->Destroy,os,sizeof(OBJSEQ));
    break;

    case NO_WALK:
    memmove(&ae->ObjList[oNum]->Walk,os,sizeof(OBJSEQ));
    break;

    case NO_ATTACK:
    memmove(&ae->ObjList[oNum]->Attack,os,sizeof(OBJSEQ));
    break;

    case NO_INTERACT:
    memmove(&ae->ObjList[oNum]->Interact,os,sizeof(OBJSEQ));
    break;

    default:
    result = ERR_BADOBJTYPE;
    break;
    }

if (!result && ae->ObjList[oNum]->CurrentBitmaps == NULL)
    result = AckSetObjectType(ae,oNum,oType);

return(result);
}

// **** End of Source ****


// This file contains the key initialization functions for the ACK-3D engine.
// The main function AckInitialize() must be called first before any of the
// other ACK-3D functions are called. The internal functions defined in this file
// perform all of the set up work of loading tables and resource files.
#include <windows.h>
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

extern  char AckKeyboardSetup;
extern  char AckTimerSetup;

short   *LowerTable[2048];
short   tmpLowerValue[400];
short   LowerLen[2048];
short   OurDataSeg;

char    rsName[128];

long FloorCosTable[VIEW_WIDTH+1];

short AckBuildTables(ACKENG *ae);
void AckBuildHeightTables(ACKENG *ae);
void AckBuildGrid(ACKENG *ae);
void SetupFloors(ACKENG *ae);

//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
// Internal function called by AckInitialize(). This function sets up the
// internal variables that are required to support the off-screen buffer
//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
void AckSetupWindow(ACKENG *ae)
{
// Access the center row of the viewport
ae->CenterRow      = ae->WinStartY + ((ae->WinEndY - ae->WinStartY) / 2);
// Access a memory location for the center row
ae->CenterOffset   = ae->CenterRow * BYTES_PER_ROW;
// Access the starting memory location of the viewport
ae->WinStartOffset = ae->WinStartY * BYTES_PER_ROW;
// Calculate the window length in double words
ae->WinLength      = ((ae->WinEndY - ae->WinStartY)+1) * DWORDS_PER_ROW;
// Calculates the viewport window width and height
ae->WinWidth      = (ae->WinEndX - ae->WinStartX) + 1;
ae->WinHeight     = (ae->WinEndY - ae->WinStartY) + 1;
}

//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
// Initializes the ACK interface structure and reads in the TRIG tables
// from either the stand alone file TRIG.DAT or from a resource file that
// was opened previous to this call.
// This function MUST be called before AckBuildView() and AckDisplayScreen()
//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
short AckInitialize(ACKENG *ae)
{
    short  i,result = 0;
    short  j;
    UCHAR  topcolor;

#ifdef __BORLANDC__      // Conditional for Borland C++
OurDataSeg = _DS;
#endif

AckKeyboardSetup = 0;    // Indicates keyboard interrupt has not been set up
AckTimerSetup = 0;       // Indicates timer has not been set up

// Check to see if viewport coordinates are set up properly
if (!ae->WinEndY || !ae->WinEndX ||
    (ae->WinEndY - ae->WinStartY) < 10 ||  // Height is less than 10 pixels
    (ae->WinEndX - ae->WinStartX) < 10)    // Width is less than 10 pixels
{
    return(ERR_BADWINDOWSIZE);    // Return error code for invalid viewport
}

result = AckBuildTables(ae);      // Read in TRIG.DAT and allocate tables
if (result)
    return(result);

AckSetupWindow(ae);         // Set up the internal coordinates for the viewport
SetupFloors(ae);            // Set up the floors
AckBuildHeightTables(ae);   // Build height and adjustment tables
topcolor = ae->TopColor;
BackDropRows = 100;

for (i = 0; i < 640; i++)
    {
    BackArray[i] = AckMalloc(BackDropRows+1);
    if (BackArray[i] == NULL)
        return(ERR_NOMEMORY);
    memset(BackArray[i],topcolor,BackDropRows);
    }
return(result);
}

//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
// Opens a resource file for use by any ACK routine that requires a
// filename. Only one resource file can be opened at a time.
//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
short AckOpenResource(char *fName)
{
    ULONG   hLen;

if (rsHandle)            // Is a resource file currently opened?
    _lclose(rsHandle);   // Close it before opening a new one

rsHandle = _lopen(fName,OF_READ);  // Open new resource file
if (rsHandle < 1)                  // Check to see if file is opened properly
    {
    rsHandle = 0;                  // Reset file handle
    return(ERR_BADFILE);           // Return error code for faliure
    }

hLen = MAX_RBA * sizeof(long);     // Get size of file
if (rbaTable == NULL)
    rbaTable = (ULONG *)AckMalloc(hLen);  // Allocate buffer for file
if (rbaTable == NULL)                     // Was memory available?
    {
    _lclose(rsHandle);                    // Close file
    rsHandle = 0;                         // Reset file handle
    return(ERR_NOMEMORY);                 // Return error code
    }

// Read in the file and check for byte count error
if (_lread(rsHandle,(ULONG *)rbaTable,hLen) != hLen)
    {
    _lclose(rsHandle);         // Close file
    rsHandle = 0;              // Reset file handle
    AckFree(rbaTable);         // Free up buffer
    return(ERR_BADFILE);       // Return file error code
    }

strcpy(rsName,fName);         // Store resource filename
return(0);
}

//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
// Closes a resource file if one is opened.
//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
void AckCloseResource(void)
{
if (rsHandle)                // Check to make sure resource file is opened
    _lclose(rsHandle);       // Close the resource

if (rbaTable != NULL)        // Do we need to free the memory for the file buffer?
    {
    AckFree(rbaTable);       // Free up the file buffer
    rbaTable = NULL;
    }

rsHandle = 0;               // Reset the file handle
}

//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
// Internal function used to pre-define height tables for the wall
// drawing code.
//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
void BuildWallDstTables(void)
{
    short i,j,dst,row,HiValue;
    long        ldst,value,LowValue,len;
    short *lp;

for (ldst = 10;ldst < 2048; ldst++)
    {
    HiValue = value = 0;
    row = 0;

    while (HiValue < 64 && row < 100)
        {
        HiValue = (value >> 8) & 0xFF;
        tmpLowerValue[row] = HiValue;
        row++;
        value += ldst;
        }

    LowerLen[ldst] = row;
    len = row * 2;
    j = 1;
    if (row == LowerLen[ldst-1])
        {
        j = 0;
        lp = LowerTable[ldst-1];
        for (i = 0; i < row; i++)
            {
            if (tmpLowerValue[i] != lp[i])
                {
                j = 1;
                break;
                }
            }
       }
    if (j)
        {
        lp = AckMalloc(len);
        if (lp == NULL)
            {
            return;
            }
        LowerTable[ldst] = lp;
        for (i = 0; i < row; i++)
            lp[i] = tmpLowerValue[i];
        }
    else
        {
        LowerTable[ldst] = LowerTable[ldst-1];
        }
    }
}

//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
// Internal function called from AckInitialize() to read in the trig tables
// and allocate memory for the various buffers.
//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
short AckBuildTables(ACKENG *ae)
{
    short handle,len,ca,na;
    int c,s,ang;
    long        fAng,tu,tv;
    SLICE *sa,*saNext;

BuildWallDstTables();            // Create the distance tables

if (!rsHandle)                   // Check to make sure resource file is not opened
    {
    handle = _lopen("trig.dat",OF_READ);    // Open trig data file
    if (handle < 1)
        return(ERR_BADFILE);                // File can't be opened; return error code
    }
else
    {
    handle = rsHandle;                           // Get handle for resource file
    _llseek(handle,rbaTable[0],SEEK_SET);
    }

// Allocate memory for trig and coordinate tables
LongTanTable      = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
LongInvTanTable   = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
CosTable          = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
SinTable          = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
LongCosTable      = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
xNextTable        = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
yNextTable        = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
ViewCosTable      = (long *)AckMalloc(sizeof(long) * VIEW_WIDTH);

// Allocate memory for map grid and object grid
Grid = (unsigned short *)AckMalloc((GRID_MAX * 2)+1);
ObjGrid = (unsigned short *)AckMalloc((GRID_MAX * 2)+1);

// Allocate memory for height adjustment table
AdjustTable = (long *)AckMalloc((MAX_DISTANCE+1) * sizeof(long));
// Allocate memory for screen buffers
ae->ScreenBuffer = (UCHAR *)AckMalloc(SCREEN_SIZE+640);
ae->BkgdBuffer = (UCHAR *)AckMalloc(SCREEN_SIZE+640);

if (LongTanTable     == NULL ||         // Make sure memory is allocated for tables
    LongInvTanTable  == NULL ||
    CosTable         == NULL ||
    SinTable         == NULL ||
    LongCosTable     == NULL ||
    xNextTable       == NULL ||
    yNextTable       == NULL ||
    Grid             == NULL ||
    ObjGrid          == NULL ||
    AdjustTable      == NULL ||
    ae->ScreenBuffer == NULL ||
    ae->BkgdBuffer   == NULL ||
    ViewCosTable     == NULL)
    {
    if (!rsHandle)
        _lclose(handle);
    return(ERR_NOMEMORY);                 // Return memory allocation error code
    }

len = sizeof(long) * INT_ANGLE_360;       // Calculate size for each trig table
_lread(handle,SinTable,len);              // Read in trig data and place in appropriate tables
_lread(handle,CosTable,len);
_lread(handle,LongTanTable,len);
_lread(handle,LongInvTanTable,len);
_lread(handle,InvCosTable,len);
_lread(handle,InvSinTable,len);
_lread(handle,LongCosTable,len);

if (!rsHandle)
    _lclose(handle);                  // Done reading, close trig.dat

ca = INT_ANGLE_32;
na = -1;

// Set up viewing tables for 32 to -32 angle sweep
for (len = 0; len < VIEW_WIDTH; len++)
    {
    ViewCosTable[len] = LongCosTable[ca];
    FloorCosTable[len] = InvCosTable[ca] >> 6;
    ca += na;
    if (ca <= 0)        // Index is less than 0 so switch
        {
        ca = -ca;
        na = -na;
        }
    }

// Adjust tables for 90, 180, and 270 degree angles
LongTanTable[INT_ANGLE_90] = LongTanTable[INT_ANGLE_90+1];
LongInvTanTable[INT_ANGLE_90] = LongInvTanTable[INT_ANGLE_90+1];
LongTanTable[INT_ANGLE_180] = LongTanTable[INT_ANGLE_180+1];
LongInvTanTable[INT_ANGLE_180] = LongInvTanTable[INT_ANGLE_180+1];
LongTanTable[INT_ANGLE_270] = LongTanTable[INT_ANGLE_270+1];
LongInvTanTable[INT_ANGLE_270] = LongInvTanTable[INT_ANGLE_270+1];

for (len = 0; len < INT_ANGLE_360; len++)
    {
    yNextTable[len] = (long)BITMAP_WIDTH * LongTanTable[len];    // Calculate y intercept increments
    xNextTable[len] = (long)BITMAP_WIDTH * LongInvTanTable[len]; // Calculate x intercept increments
    InvCosTable[len] = InvCosTable[len] >> 4;   // Scale inverse tables
    InvSinTable[len] = InvSinTable[len] >> 6;
    }

// Set up the array od slice structures to represent the full width of the view
// Each slice structure is initialized by setting its data fields to 0s
// Each slice in the array is also linked to a second slice to reference a slice that
// could be visually behind the current slice
for (len = 0; len < VIEW_WIDTH; len++)
    {
    sa = &Slice[len];                        // Initialize array of slice structures
    memset(sa,0,sizeof(SLICE));              // Set all data to 0
    for (ca = 0; ca < 8; ca++)
        {
        saNext = AckMalloc(sizeof(SLICE));   // Create a slice structure to link in
        if (saNext == NULL)
            return(ERR_NOMEMORY);            // Check for memory allocation
        memset(saNext,0,sizeof(SLICE));      // Initialize all data to 0
        sa->Next = saNext;                   // Link in slice
        saNext->Prev = sa;
        sa = saNext;
        }
    }
return(0);
}

//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
// Reads a map file and processes any multi-height walls.
//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
short AckReadMapFile(ACKENG *ae,char *fName)
{
    short   len,handle,rdlen,count,i,pos;
    int     mLen,aLen;
    UCHAR   buf[MAX_MULTI+2];
    UCHAR   *mPtr;

if (!rsHandle)            // Check to see if resource file is open already
    {                                  // No resource file so open new one
    handle = _lopen(fName,OF_READ);    // Open the specified resource
    if (handle < 1)
        return(ERR_BADMAPFILE);       // File was not opened; return error code
    }
else
    {
    handle = rsHandle;                // Get handle to open resource
    _llseek(handle,rbaTable[(ULONG)fName],SEEK_SET);  // Access opened resource file
    }

aLen = GRID_ARRAY * 2;
mLen = GRID_MAX * 2;

if (_lread(handle,Grid,mLen) != mLen)   // Read in grid map data
    {
    if (!rsHandle)
        _lclose(handle);
    return(ERR_READINGMAP);            // Return file read error code
    }

if (_lread(handle,ObjGrid,mLen) != mLen)   // Read in object map data
    {
    if (!rsHandle)
        _lclose(handle);
    return(ERR_READINGMAP);
    }

if (_lread(handle,ae->xGrid,aLen) != aLen)   // Read in x grid data
    {
    if (!rsHandle)
        _lclose(handle);
    return(ERR_READINGMAP);
    }

if (_lread(handle,ae->yGrid,aLen) != aLen)   // Read in y grid data
    {
    if (!rsHandle)
        _lclose(handle);
    return(ERR_READINGMAP);
    }

if (_lread(handle,FloorMap,mLen) != mLen)   // Read in floor map data
    {
    if (!rsHandle)
        _lclose(handle);
    return(ERR_READINGMAP);
    }

if (_lread(handle,CeilMap,mLen) != mLen)    // Read in ceiling map data
    {
    if (!rsHandle)
        _lclose(handle);
    return(ERR_READINGMAP);
    }

_lread(handle,&count,2);               // Check counter for multi-height walls
if (count)
    {
    for (i = 0; i < count;i++)         // Read in multi-height wall data
        {
        _lread(handle,&pos,2);     // Get grid position for this multi-height wall
        mPtr = (UCHAR *)AckMalloc(MAX_MULTI+1);  // Allocate memory for multi-height wall data
        if (mPtr == NULL)
            {
            if (!rsHandle)
                _lclose(handle);
            return(ERR_NOMEMORY);
            }

        ae->mxGrid[pos] = mPtr;   // Store pointer to multi-height wall
        ae->myGrid[pos] = mPtr;
        ae->mxGrid[pos+1] = mPtr;
        ae->myGrid[pos+GRID_WIDTH] = mPtr;
        _lread(handle,buf,MAX_MULTI);
        buf[MAX_MULTI] = '\0';
        len = strlen(buf);
        if (len > MAX_MULTI) len = MAX_MULTI;
        *mPtr = len;
        if (len)
            memmove(&mPtr[1],buf,len);
        }
    }

if (!rsHandle)                  // Close handle
    _lclose(handle);

AckBuildGrid(ae);       // Build object lists
return(0);
}

//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
// Internal function to create height and distance tables for objects. In
// the DistanceTable[] each entry represents the distance from the player
// to a wall. The value stored in the array is the hight of the wall at
// the corresponding distance. For example, DistanceTable[100] indicates
// that the distance to the wall is 100 units. The value stored at this
// location is 81--the pixel height f the wall.
//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
void AckBuildHeightTables(ACKENG *ae)
{
    short     i,x;
    short     result;
    long    height;

height = BITMAP_WIDTH * 128L;     // Calculate distance to height conversion factor

DistanceTable[0] = MAX_HEIGHT;    // First entry = max. height (960)

//************ 64 * 65536 ********
AdjustTable[0] = 4194304L / height;

for (i = 1; i < MAX_DISTANCE; i++)   // Loop to calculate each entry for the arrays
    {
    DistanceTable[i] = height / i;
    if (height - (DistanceTable[i] * i) > (i / 2))
        DistanceTable[i]++;             // Add 1 to height value
    if (DistanceTable[i] < MIN_HEIGHT)  // Adjust for min. height (8)
        DistanceTable[i] = MIN_HEIGHT;
    if (DistanceTable[i] > MAX_HEIGHT)  // Adjust for maax. height (960)
        DistanceTable[i] = MAX_HEIGHT;
    AdjustTable[i] = 2097152L / DistanceTable[i];  // Calculate entry for
    }
}

//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
// Internal function called by AckReadMapFile() to process the objects
// in the map. Moveable vs stationary objects are processed here.
//сссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссссс
void AckBuildGrid(ACKENG *ae)
{
short     i,j,CurIndex,pos,x1,y1;
USHORT    MapCode,MapHiCode;

// Initialize doors
for (i = 0; i < MAX_DOORS; i++)
    {
    ae->Door[i].ColOffset = 0;
    ae->Door[i].mPos = ae->Door[i].mPos1 = -1;
    }

ae->SysFlags |= SYS_NO_WALLS;    // Assume no floating walls

CurIndex     = 1;
TotalSpecial = 0;
TotalSecret  = 0;
for (i = 0; i < GRID_HEIGHT; i++)     // Loop until entire grid has been checked
    {
    for (j = 0; j < GRID_WIDTH; j++)
        {
        pos = (i * GRID_WIDTH) + j;
        MapCode = ObjGrid[pos];     // Check object at current grid position
        if (MapCode)                // Is there an object here?
            {
            CurIndex = MapCode & 0xFF;
            if (CurIndex < MAX_OBJECTS)   // Get the index of the object into
                {
                if (ae->ObjList[CurIndex] == NULL)  // No object allocated yet
                    {
                     // Allocate memory for object
                    ae->ObjList[CurIndex] = (NEWOBJECT *)AckMalloc(sizeof(NEWOBJECT));
                    if (ae->ObjList[CurIndex] != NULL)
                        memset(ae->ObjList[CurIndex],0,sizeof(NEWOBJECT));
                    }
                // If memory has been allocated calculate coordinates for object
                if (ae->ObjList[CurIndex] != NULL)
                   {
                   x1 = (j * BITMAP_WIDTH) + (BITMAP_WIDTH/2);
                   y1 = (i * BITMAP_WIDTH) + (BITMAP_WIDTH/2);
                   ae->ObjList[CurIndex]->x = x1;      // Store x,y position
                   ae->ObjList[CurIndex]->y = y1;
                   ae->ObjList[CurIndex]->mPos = pos;  // Store map position
                   ae->ObjList[CurIndex]->Active = 1;  // Indicates object is active
                   }
                }
            }
        }
    }

}

// **** End of Source ****


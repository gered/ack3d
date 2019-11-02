// This file contains the declarations and functions to set up views for the
// ray casting engine.
#include <windows.h>    // Required for Windows version of engine
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys\stat.h>
#include <limits.h>

#include "ack3d.h"      // Main ACK-3D internal and interface data structures
#include "ackeng.h"     // Intrnal structures and constants
#include "ackext.h"     // Defines external (global) variables

extern  long FloorCosTable[];

void    (*FloorCeilRtn)(void);
void    (*WallRtn)(void);
void    (*WallMaskRtn)(void);

short   gWinFullWidth;          // Global variables for setting up a viewport
long    gWinDWORDS;             // These are the global variables used by the
long    gWinStartOffset;                // low-level assembly language routines to draw slices
short   gWinStartX;
short   gWinStartY;
short   gWinEndX;
short   gWinEndY;
short   gWinHeight;
short   gWinHalfHeight;
short   gWinWidth;
short   gCenterRow;
short   gCenterOff;
long    gBottomOff;
UCHAR   *gScrnBufferCenter;
UCHAR   *gScrnBuffer;
UCHAR   *gBkgdBuffer;
UCHAR   *gPalTable;
short   gMultiWalls;

UCHAR   **mxGridGlobal; // Global variables to reference the x and y
UCHAR   **myGridGlobal; // map arrays

UCHAR   gTopColor;
UCHAR   gBottomColor;

UCHAR   *scVid;         // Variables used in low level routines for
UCHAR   *scWall;                // building and drawing slices
UCHAR   *scPal;
short   scdst;
short   scwht;
short   scmulti;
short   sctopht;
short   scbotht;
short   scsavwht;
short   scmulcnt;
UCHAR   *scsavVid;
USHORT    scbNum;
UCHAR   *scMulData;
UCHAR   *scColumn;
UCHAR   *gPtr;
UCHAR   *gmPtr;
short   gBitmapNumber;
short   gBitmapColumn;
short   gyBitmapNumber;
short   gyBitmapColumn;
long    gWallDistance;
short   gmPos;
DOORS   *gDoor;
DOORS   *gDoorPosn;
short   wFound;
UCHAR   *mgPtr;

short   BegX,EndX;

extern  long        x_xPos;     // Variables for tracking coordinates during
extern  long        x_yPos;     // the ray casting process
extern  long        x_xNext;
extern  long        x_yNext;
extern  long        y_xPos;
extern  long        y_yPos;
extern  long        y_xNext;
extern  long        y_yNext;

short   LastVht;
long    WallDistTable[VIEW_WIDTH];

// Functions used to build views and perform the ray casting process
void AckSetupWindow(ACKENG *ae);        // Sets up variables for viewport
void BuildUpView(void);                 // Main assemply language routine for building views
void BuildSlice(void);                  // Assembly language routines for building slices
void BuildSliceMulti(void);             // Assembly language routine for building multi-slices
void CheckDoors(void);                  // Internal routines for locating and checking doors
void FindObject(void);                  // and objects
short FindDoor(short MapPosn);

void FloorLoop(void);
void CeilLoop(void);
void DrawSlices(void);
short BlankSlice(USHORT col,UCHAR *bmp);
void BuildSliceAsm(void);

void xRaySetup(void);           // Routines for setting up and casting rays
USHORT xRayCast(void);
void yRaySetup(void);
USHORT yRayCast(void);
USHORT xRayCastMulti(void);
USHORT yRayCastMulti(void);

void ShowCol(void);                     // Routines for drawing a slice 
void ShowColMask(void);                 // column by column
void ShowColNS(void);
void ShowColMaskNS(void);
void ShowColLow(void);
void ShowColMaskLow(void);

void DrawFloorCeiling(void);            // Routines for drawing floors and ceilings
void AckDrawFloor(void);
void AckDrawFloorOnly(void);
void AckDrawCeilingOnly(void);
void AckDrawFloorNS(void);
void AckDrawFloorOnlyNS(void);
void AckDrawCeilingOnlyNS(void);
void AckDrawFloorHz(void);
void AckDrawOneFloor(void);
void DrawSolidCeilAndFloor(void);
void DrawSolidCeilAndFloorNS(void);
void DrawSolidFloorAndCeil(void);
void DrawSolidFloorAndCeilNS(void);
void DrawSolidCeilSolidFloor(void);

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
// Transfers certain variables from the interface structure to global
// variables that can be accessed faster by the drawing functions. The
// interface structure is kept by the application and more than one of
// them can be used for different views. Each time a new view needs to
// be processed, this function MUST be called before calling the
// drawing routines.
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
void AckRegisterStructure(ACKENG *ae)
{
    int mode,i;

aeGlobal    = ae;                       // Global variable to reference ACKENG structure
AckSetupWindow(ae);             // Assign window variables to ACKENG structure
xGridGlobal  = ae->xGrid;       // Global map for x walls
yGridGlobal  = ae->yGrid;       // Global map for y walls
mxGridGlobal = ae->mxGrid;      // Wall data for multi-height walls
myGridGlobal = ae->myGrid;
WallbMaps    = ae->bMaps;               // Wall bitmap data
gWinStartX   = ae->WinStartX;           // Coordinates of viewport; upper-left
gWinStartY   = ae->WinStartY;
gWinEndX     = ae->WinEndX;             // Lower-right viewport coordinates
gWinEndY     = ae->WinEndY;
gWinHeight   = ae->WinHeight;           // Height of viewport
gWinWidth    = ae->WinWidth;            // Width of viewport
gWinHalfHeight = (gWinEndY - (gWinHeight >> 1)) + 1;
gCenterRow  = ae->CenterRow;            // Start of center row in viewport
gCenterOff  = ae->CenterOffset;         // Offset to center of viewport
gScrnBuffer = ae->ScreenBuffer;         // Screen buffer access
gScrnBufferCenter = gScrnBuffer + gCenterOff;   
gBkgdBuffer = ae->BkgdBuffer;           // Buffer for ceiling and floors
gPalTable   = ae->PalTable;             // Palette of colors used
gDoor       = &ae->Door[0];             // List of moving doors
gTopColor   = ae->TopColor;             // Base color of ceiling
gBottomColor = ae->BottomColor;         // Base color of floor
LightFlag    = ae->LightFlag;           // Light shading on or off indicator
SysFlags     = ae->SysFlags;    // Scene display attributes (floors and ceilings)

mode = 0;                               // Draw both textured floor and ceiling
if (SysFlags & SYS_SOLID_CEIL)  // Soild ceiling is selcted
    {
    mode = 1;                           // Draw floor only (ceiling will be solid)
    if (SysFlags & SYS_SOLID_FLOOR)     // Solid floor is selcted
        mode = 2;                               // Draw solid floor and ceiling 
    }

if (SysFlags & SYS_SOLID_FLOOR) // Solid floor is selected
    {
    if (!mode)
        mode = 3;                               // Draw Ceiling only (floor will be solid)
    }

if (!LightFlag)                         // No light shading used
    {
    WallRtn = ShowColNS;                // Assembly routines for drawing slices
    WallMaskRtn = ShowColMaskNS;        // using light shading
    switch (mode)                               // Check floor and ceiling type
        {
        case 0:                         // Draw both solid floor and ceiling 
            if (ae->SysFlags & SYS_SINGLE_BMP)
                FloorCeilRtn = AckDrawOneFloor;   // Use the same bitmap for each
            else
                FloorCeilRtn = AckDrawFloorHz;
            break;
        case 1:                         // Draw solid ceiling and texture floor 
            FloorCeilRtn = DrawSolidCeilAndFloorNS;
            break;
        case 2:                         // Draw both solid floor and solid ceiling
            FloorCeilRtn = DrawSolidCeilSolidFloor;
            break;
        case 3:                         // Draw solid floor and texture ceiling
            FloorCeilRtn = DrawSolidFloorAndCeilNS;
            break;
        default:
            break;
        }
    }
else                    // Light shading is used
    {
    WallRtn = ShowCol;                  // Assembly routines for drawing slices
    WallMaskRtn = ShowColMask;          // using light shading
    switch (mode)
        {
        case 0:         // Draw both floor and ceiling
            FloorCeilRtn = AckDrawFloor;
            break;
        case 1:         // Draw solid ceiling and texture floor
            FloorCeilRtn = DrawSolidCeilAndFloor;
            break;
        case 2:         // Draw both solid floor and solid ceiling
            FloorCeilRtn = DrawSolidCeilSolidFloor;
            break;
        case 3:         // Draw solid floor and texture ceiling
            FloorCeilRtn = DrawSolidFloorAndCeil;
            break;
        default:
            break;
        }
    }

// Test to see if viewport is full width (320 units)
gWinStartOffset = ae->WinStartOffset;           // Offset to viewport
gBottomOff  = (gWinEndY * 320);
gWinFullWidth = 0;                      // Set flag to indicate viewport is not full width
if (gWinStartX == 0 && gWinEndX == 319) // Viewport is full size
    {
    gWinFullWidth = 1;                          // Indicates viewport is full size
    gWinDWORDS = (gWinEndY - gWinStartY) * 80;  // Calculate number of double
    }                                                   // words to access buffer

// Test to see if multi-height walls are used
gMultiWalls = 0;
for (i = 0; i < GRID_MAX; i++)
    {
    if ((xGridGlobal[i] & WALL_TYPE_MULTI) || (yGridGlobal[i] & WALL_TYPE_MULTI))
        {
        gMultiWalls = 1;                // Indicates multi-height walls are used
        break;
        }
    }
}

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
// Render the current scene into the off-screen buffer based on the POV
// coordinates and angle. This function does NOT display the scene once
// it is rendered so the application can overlay graphics into the buffer
// before it is actually displayed.
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
void AckBuildView(void)
{
// Set up global variables to be used with assembly language routines
xPglobal    = aeGlobal->xPlayer;                // The player's x coordinate
xBegGlobal  = xPglobal & GRID_MASK;     // Upper left corner (x) of the grid
                                        // square the player is in
xPglobalHI  = ((long)xPglobal << FP_SHIFT);     // Convert x coordinate to fixed point
yPglobal    = aeGlobal->yPlayer;                        // The player's y coordinate
yBegGlobal  = yPglobal & GRID_MASK;             // Upper left corner (y) of grid square
yPglobalHI  = ((long)yPglobal << FP_SHIFT);     // Convert y coordinate to fixed point
PlayerAngle = aeGlobal->PlayerAngle;            // The player's angle
SysFlags    = aeGlobal->SysFlags;                       // Ceiling and floor attributes; 
BuildUpView();          // Assembly routine defined in ACKRTN3.ASM. This routine
                        // kicks off the ray casting process.
}

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
// Stub function for drawing slices that do not contain walls.
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
void ShowNone(void)
{
return;
}

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
// Internal function to cast the x and y rays and build a slice structure
// for each column of the viewing window.
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
void BuildSlice(void)
{
    short   j,index,wFound;
    long    xBitmap,yBitmap,BitmapNumber;
    short   DoorOpenColumn;
    ULONG   xDistance,yDistance;
    ULONG   xd,yd,mf;
    long    WallDistance,UnAdjustDist;
    long    distance,LightAdj;
    USHORT    BitmapColumn,yBitmapColumn;
    long    OldMapPosn,OldMapPosn1;
    long    HoldAngle;
    long    offset;
    long    mPos;

    USHORT    *gPtr;
    UCHAR   *mgPtr;
    SLICE   *spNext;

WallDistance = 3000000;         // Set to a ridiculous distance 
sPtr->Distance = 200;           // Initialize the distance from the POV to the slice
wFound = 0;                     // Wall not found yet
sPtr->Fnc = ShowNone;           // Use the stub function for now for drawing the slice

// Call the low level ray casting function and see if anything is hit.
if ((BitmapNumber = xRayCast()) != 0)   // Something has been hit while casting
    {                                   // in the x plane
    wFound = 1;         // Set flag to indicate a hit
    // Use the y intercept to determine the column of the slice
    BitmapColumn = (LastY1 >> FP_SHIFT) & (BITMAP_WIDTH-1);

    // Keep the orientation the same no matter which side we're on 
    if ((short)iLastX < xPglobal)
        BitmapColumn = (BITMAP_WIDTH-1) - BitmapColumn;

    // Did we strike a door? 
    if ((BitmapNumber & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT)))
        {
        index = FindDoor(xMapPosn);     // Locate the position of the door
        if (index >= 0)                 // Is this is a valid door?
            {
            j = aeGlobal->Door[index].ColOffset;        // Get the door's  current pos
            offset = 0;
            if (BitmapNumber & DOOR_TYPE_SLIDE)         // Is the door a slider?
                {
                DoorOpenColumn = BITMAP_WIDTH-1;
                if ((short)iLastX > xPglobal)           // Handle orientation
                    j = -j;
                BitmapColumn += j;                      // Adjust column to show
                }
            if (BitmapNumber & DOOR_TYPE_SPLIT)         // Is the door a split door?
                {
                DoorOpenColumn = (BITMAP_WIDTH/2)-1;
                if (BitmapColumn < (BITMAP_WIDTH/2))
                    {
                    BitmapColumn += j;
                    if (BitmapColumn > (BITMAP_WIDTH/2)-1)
                        offset = 1;
                    }
                else
                    {
                    BitmapColumn -= j;
                    if (BitmapColumn < (BITMAP_WIDTH/2))
                        offset = 1;
                    }
                }               // End processing split door

            if (offset == 1 || BitmapColumn > (BITMAP_WIDTH-1))
                {
                // Get the grid coordinates for this door
                OldMapPosn = aeGlobal->Door[index].mPos;
                OldMapPosn1 = aeGlobal->Door[index].mPos1;
                // Fake the engine into thinking no door is there
                xGridGlobal[OldMapPosn] = 0;
                xGridGlobal[OldMapPosn1] = 0;
                // Cast the ray to get walls beyond the door
                BitmapNumber = xRayCast();
                // Put back the door codes if not fully open
                if (aeGlobal->Door[index].ColOffset < DoorOpenColumn)
                    {
                    xGridGlobal[OldMapPosn] = aeGlobal->Door[index].mCode;
                    xGridGlobal[OldMapPosn1] = aeGlobal->Door[index].mCode1;
                    }
                // Calc the new bitmap column of wall behind door
                BitmapColumn = (LastY1 >> FP_SHIFT) & (BITMAP_WIDTH-1);
                if ((short)iLastX < xPglobal)
                    BitmapColumn = (BITMAP_WIDTH-1) - BitmapColumn;
                }
            }
        }               // End processing doors

//==============================================================
// Calculate the distance to the wall. Since the x position was
// fixed to move 64 or -64 we can use it to determine the actual
// wall distance. The InvCosTable values were stored with a fixed
// point of 20 decimal places. At this time we'll knock off 14 of
// them so we can later multiply with a fixed point value of 16
//==============================================================
    xd = iLastX - xPglobal;                     // x Distance to the found slice
    mf = InvCosTable[ViewAngle];                // Get the cosine of the angle
    WallDistance = (xd * mf) >> 10;     // Calculate the actual distance to the slice
    if (WallDistance > 33554432L)               // Check for out of range
        WallDistance = 1200000L;
    gPtr = xGridGlobal;                 // Point to xGrid map
    mgPtr = mxGridGlobal[xMapPosn];     // Point to grid map for multi-height walls
    mPos = xMapPosn;                    // Access actual map position
    }                             // End (if BitmapNumber = xRayCast() ...)

// Time to cast the ray in the y plane
if ((yBitmap = yRayCast()) != 0)                // Something has been hit while casting
    {
    // Use the X intercept to determine the column of the bitmap
    yBitmapColumn = (LastX1 >> FP_SHIFT) & (BITMAP_WIDTH-1);
    // Handle orientation from either side of the wall
    if ((short)iLastY > yPglobal)
        yBitmapColumn = (BITMAP_WIDTH-1) - yBitmapColumn;

    // Did we strike a door?
    if ((yBitmap & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT)))
        {
        index = FindDoor(yMapPosn);
        if (index >= 0)                 // Is this is a valid door?
            {
            // Get the current door column offset
            j = aeGlobal->Door[index].ColOffset;
            offset = 0;
            // Deal with orientation
            if (yBitmap & DOOR_TYPE_SLIDE)      // Is this a sliding door?
                {
                DoorOpenColumn = BITMAP_WIDTH-1;
                if ((short)iLastY < yPglobal)
                    j = -j;
                yBitmapColumn += j;
                }               // End processing sliding door
            if (yBitmap & DOOR_TYPE_SPLIT)      // Is this a split door?
                {
                DoorOpenColumn = (BITMAP_WIDTH/2)-1;
                if (yBitmapColumn < (BITMAP_WIDTH/2))
                   {
                        yBitmapColumn += j;
                        if (yBitmapColumn > (BITMAP_WIDTH/2)-1)
                            offset = 1;
                    }
                else
                    {
                         yBitmapColumn -= j;
                         if (yBitmapColumn < (BITMAP_WIDTH/2))
                             offset = 1;
                     }
                 }              // End processing split door
            // If beyond width of bitmap than cast again
            if (offset == 1 || yBitmapColumn > (BITMAP_WIDTH-1))
                {
                // Get the yGrid coordinates for this door
                OldMapPosn = aeGlobal->Door[index].mPos;
                OldMapPosn1 = aeGlobal->Door[index].mPos1;
                // Fool the engine into thinking no door is there
                yGridGlobal[OldMapPosn] = 0;
                yGridGlobal[OldMapPosn1] = 0;
                // Cast again for walls beyond the door
                yBitmap = yRayCast();
                // Put door code back if not fully open
                if (aeGlobal->Door[index].ColOffset < DoorOpenColumn)
                    {
                     yGridGlobal[OldMapPosn] = aeGlobal->Door[index].mCode;
                     yGridGlobal[OldMapPosn1] = aeGlobal->Door[index].mCode1;
                    }
                // Get the bitmap column of wall beyond door
                yBitmapColumn = (LastX1 >> FP_SHIFT) & (BITMAP_WIDTH-1);
                if ((short)iLastY > yPglobal)
                    yBitmapColumn = (BITMAP_WIDTH-1) - yBitmapColumn;
                }
            }
        }

//==============================================================
//  Calculate the distance to the wall. Since the y position was
//  fixed to move 64 or -64 we can use it to determine the actual
//  wall distance. The InvSinTable values were stored with a fixed
//  point of 20 decimal places. At this time we'll knock off 14 of
//  them so we can later multiply with a fixed point value of 16
//==============================================================
    yd = iLastY - yPglobal;             // Distance from player's position to intersection point
    mf = InvSinTable[ViewAngle];        // Use angle with look up table
    yDistance = (yd * mf) >> 8; // Calculate y distance
    if (yDistance > 33554432L)  // Distance is out of range, adjust
        yDistance = 120000L;

//==============================================================
//  At this point check the distance to the y wall against the x
//  wall to see which one is closer. The closer one is the one
//  we'll draw at this column of the screen.
//==============================================================
    if (yDistance < WallDistance)               // Use distance to y slice if this slice is closer
        {
        wFound = 1;                     // Indicates that a slice has been found
        WallDistance = yDistance;               // Use distance to y slice
        BitmapNumber = yBitmap;         // Transfer bitmap number
        BitmapColumn = yBitmapColumn;   // Transfer bitmap column
        gPtr = yGridGlobal;                     // Store pointer to global y grid
        mPos = yMapPosn;                        // Store position of y wall in map
        mgPtr = myGridGlobal[mPos];
        }
    }           // End (if yBitmap = yRayCast()) != 0)

// A slice has been found so process it
if (wFound)
    {
//=============================================================
//  To avoid a fishbowl affect we need to adjust the distance so
//  it appears perpendicular to the center point of the display
//  which is relative angle 0 from the players current angle. We
//  started at -32 degrees for the first screen column and will
//  cycle from -32 down to 0 then back up to +32 degrees. This
//  cosine value was pre-calculated and placed in ViewCosTable.
//=============================================================
    UnAdjustDist = WallDistance >> 5;
    yd = ViewCosTable[ViewColumn] >> 3; // Use current column as look up index
    WallDistance *= yd;

    // Now we strip off somemore decimal points and check round-up
    xd = WallDistance >> 12;
    if (WallDistance - (xd << 12) >= 2048)
        xd++;

    // The last decimal points from the multiplication after the x and
    // y rays is stripped off and checked for round-up
    WallDistance = xd >> 5;
    if (xd - (WallDistance << 5) >= 16)
        WallDistance++;

    // This is an arbitrary minimum distance to look for
    if (WallDistance < 10)
        WallDistance = 10;              // Reset distance to minimum allowed

    // Don't want it to go outside our table boundaries
    if (WallDistance >= MAX_DISTANCE)
        WallDistance = MAX_DISTANCE - 1;        // Reset distance to max allowed

    // Save the wall data to display when done with entire screen
    sPtr->Distance = WallDistance;              // Store the adjusted distance to the wall
    sPtr->bNumber  = BitmapNumber;      // The bitmap number for the wall slice
    sPtr->bColumn  = BitmapColumn;      // The screen column position to display the bitmap
    sPtr->bMap     = WallbMaps;         // Pointer to the bitmap
    sPtr->Active   = 0;                 // Indicates slice is not displayable yetlast one
    sPtr->Type  = ST_WALL;              // Indicates this slice is part of a wall
    sPtr->Fnc   = WallRtn;              // Pointer to function to draw slice
    sPtr->mPos  = mPos;                 // Position of the slice in the grid map
    sPtr->mPtr  = mgPtr;                // Grid pointer to reference multi-height wall
    spNext      = sPtr->Next;           // Reference wall slice behind current slice

    if (WallDistance > MaxDistance)
        MaxDistance = WallDistance;
    if (CeilMap[mPos])
        LastWallHeight = 9000;
    if (((BitmapNumber & WALL_TYPE_UPPER) ||            // Wall is above the floor level
        (BitmapNumber & WALL_TYPE_TRANS)) &&    // or wall slice is transparent and it
        spNext != NULL)                                 // has something behind it
        {
        BitmapColumn = gPtr[mPos];              // Get position of the mult-height wall
        gPtr[mPos]   = 0;
        sPtr->Active = 1;               // More slices to follow if wall is displayable!
        if (BitmapNumber & WALL_TYPE_TRANS)     // The wall is transparent
            {
            sPtr->Type = ST_OBJECT;      // We have an object
            sPtr->Fnc  = WallMaskRtn;   // Using a different drawing routine
            }
        sPtr = spNext;          // Point to slice behind
        spNext->Active = 0;             // Initialize the slice behind before building it
        spNext->bNumber = 0;
        BuildSlice();           // Call BuildSlice() again to build the slice behind
        gPtr[mPos] = BitmapColumn;              // Store position of slice
        if (!sPtr->bNumber)             // Slice behind is no longer visible
            {
            spNext = sPtr->Prev;       // Link up slices
            if (spNext != NULL)
                spNext->Active = 0;
            }
        }
    else
        {       // Keep tack of each distance to wall slice for drawing floor later on
        if (UnAdjustDist < WallDistTable[ViewColumn])
            WallDistTable[ViewColumn] = UnAdjustDist;
        }
    }
}

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
// Continues the ray cast for multi-height walls so that any wall that is
// taller than the walls in front of it will be displayed.
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
void BuildSliceMulti(void)
{
    short   j,index,wFound;
    USHORT    yHeight;
    USHORT    xBitmap,yBitmap,BitmapNumber;
    short   DoorOpenColumn;
    short   xWallHeight,yWallHeight;
    ULONG   xDistance,yDistance;
    ULONG   xd,yd,mf;

    long    WallDistance;
    USHORT    distance,LightAdj;
    USHORT    BitmapColumn,yBitmapColumn;
    short   OldMapPosn,OldMapPosn1;
    short   HoldAngle,HoldX,HoldY,xp1,yp1;
    USHORT    offset;
    short   mPos;
    USHORT    *gPtr;
    UCHAR   *mgPtr;
    SLICE   *spNext;

WallDistance = 3000000;         // Set to a very far off distance
wFound = 0;                     // Indicates a slice has not been found

// Don't cast an x ray if its impossible to intercept any x walls
if (ViewAngle != INT_ANGLE_90 && ViewAngle != INT_ANGLE_270)
    {
    if ((BitmapNumber = xRayCastMulti()) != 0)  // Cast the x ray and build multi-height slice
        {
        xBitmap = BitmapNumber & 0xFF;                  // Get the bitmap code
        if (((BitmapNumber & WALL_TYPE_MULTI) ||        // Check for multi-height
            (BitmapNumber & WALL_TYPE_UPPER)) &&        // or slice that is taller than one in front
            (xBitmap > LastWallHeight))
            {
            // LastWallHeight = xBitmap;
            wFound = 1;                 // Indicates a multi-slice wall has been found
            // Use the y intercept to determine the wall column
            BitmapColumn = (LastY1 >> FP_SHIFT) & (BITMAP_WIDTH-1);

            // Keep the orientation the same no matter which side we're on
            if ((short)iLastX < xPglobal)
                BitmapColumn = (BITMAP_WIDTH-1) - BitmapColumn;
            xd = iLastX - xPglobal;
            mf = InvCosTable[ViewAngle];                // Use angle to calculate distance to slice
            WallDistance = (xd * mf) >> 10;
            if (WallDistance > 33554432L)               // Check for out of range
                WallDistance = 1200000L;
            gPtr = xGridGlobal;                 // Reference global map grid
            mPos = xMapPosn;                    // Use position of slice
            mgPtr = mxGridGlobal[mPos];         // Get pointer to multi-height slice found
            }
        }
    }

// Don't cast a y ray if its impossible to intercept any y walls
if (ViewAngle != 0 && ViewAngle != INT_ANGLE_180)
    {
    if ((yBitmap = yRayCastMulti()) != 0)               // Cast the y ray to build multi-height slice
        {
        yHeight = yBitmap & 0xFF;                       // Get the bitmap code of slice
        if (((yBitmap & WALL_TYPE_MULTI) ||
            (yBitmap & WALL_TYPE_UPPER)) &&
            (yHeight > LastWallHeight))
            {
            yWallHeight = yHeight;
            // Use the x intercept to determine the column of the bitmap
            yBitmapColumn = (LastX1 >> FP_SHIFT) & (BITMAP_WIDTH-1);
            // Handle orientation from either side of the wall
            if ((short)iLastY > yPglobal)
                yBitmapColumn = (BITMAP_WIDTH-1) - yBitmapColumn;
             yd = iLastY - yPglobal;
            mf = InvSinTable[ViewAngle];        // Use angle to calculate distance to slice
            yDistance = (yd * mf) >> 8;
            if (yDistance > 33554432L)  // Is distance in range?
                yDistance = 120000L;

//==============================================================
//  At this point check the distance to the Y wall against the
//  wall to see which one is closer. The closer one is the one
//  we'll draw at this column of the screen.
//==============================================================
            if (yDistance < WallDistance)               // Use distance to y slice if this slice is closer
                {
                wFound = 1;                             // Indicates that a slice has been found
                WallDistance = yDistance;               // Use distance to y slice
                BitmapNumber = yBitmap;         // Transfer bitmap number
                BitmapColumn = yBitmapColumn;   // Transfer bitmap column
                gPtr = yGridGlobal;                     // Store pointer to global y grid
               mPos = yMapPosn;                 // Store position of y wall in map
               xBitmap = yHeight;
               mgPtr = myGridGlobal[mPos];              // Store pointer to multi-height slice
               }
            }
        }
    }

if (wFound)             // A multi-wall slice has been found so process it
    {
    LastWallHeight = xBitmap;
    yd = ViewCosTable[ViewColumn] >> 2;
    WallDistance *= yd;
    // Now we strip off somemore decimal points and check round-up
    xd = WallDistance >> 12;
    if (WallDistance - (xd << 12) >= 2048)
        xd++;

//==============================================================
// The last decimal points from the multiplication after the X and
// Y rays is stripped off and checked for round-up.
//==============================================================
    WallDistance = xd >> 5;
    if (xd - (WallDistance << 5) >= 16)
        WallDistance++;
    // Don't really need to, but put it into an integer for fast compare
    distance = WallDistance;
    // This is an arbitrary minimum distance to look for
    if (distance < 10)
        distance = 10;

    // Don't want it to go outside our table boundaries
    if (distance >= MAX_DISTANCE)
        distance = MAX_DISTANCE - 1;

    // Save the wall data to display when done with entire screen
    sPtr->Active = 1;
    sPtr         = sPtr->Next;
    if (sPtr == NULL)
        return;

    sPtr->Distance = distance;          // Update slice data in slice structure
    sPtr->bNumber  = BitmapNumber;
    sPtr->bColumn  = BitmapColumn;
    sPtr->bMap     = WallbMaps;
    sPtr->Active   = 0;
    sPtr->Type     = ST_WALL;
    sPtr->Fnc      = WallRtn;
    sPtr->mPos     = mPos;
    sPtr->mPtr     = mgPtr;
    spNext         = sPtr->Next;

    if (spNext != NULL)
        {
        BuildSliceMulti();              // Recursive call to build the slice behind
        }
    }
}

//********************************************************************
// Internal function called by AckCheckDoorOpen() to
// check for a collision with a wall within a certain distance.
//********************************************************************
short AckCheckHit(short xPlayer,short yPlayer,short vAngle)
{
    short   BitmapNumber,yBitmap;
    short   WallCode;
    ULONG   WallDistance;
    ULONG   xd,yd,mf,yDistance;
    long    CheckDist;

WallDistance = 3000000;     // Set to a ridiculous value
WallCode     = POV_NOTHING;
CheckDist    = 56L;         // (was 48) Initial minimum distance to look for
BitmapNumber = 0;           // Initialize to no bitmap found

xPglobal = xPlayer;
xBegGlobal = xPglobal & GRID_MASK;
xPglobalHI = ((long)xPglobal << FP_SHIFT);
yPglobal = yPlayer;
yBegGlobal = yPglobal & GRID_MASK;
yPglobalHI = ((long)yPglobal << FP_SHIFT);

ViewAngle = vAngle;

if (MoveObjectCount)
    memmove(ObjectsSeen,MoveObjectList,MoveObjectCount);

FoundObjectCount = MoveObjectCount;

//************************************************************************
// Don't allow one of these angles, causes either the X or Y ray to not be
// cast which gives a false reading about an obstacle.
//************************************************************************
if (ViewAngle == INT_ANGLE_45 ||
    ViewAngle == INT_ANGLE_135 ||
    ViewAngle == INT_ANGLE_225 ||
    ViewAngle == INT_ANGLE_315)
    {
    ViewAngle++;
    }

xRaySetup();
BitmapNumber = xRayCast();

if (BitmapNumber & (WALL_TYPE_UPPER+WALL_TYPE_PASS))
    BitmapNumber = 0;

if (BitmapNumber)
    {
    xd = iLastX - xPlayer;
    mf = InvCosTable[ViewAngle];
    WallDistance = (xd * mf) >> 10;
    if (WallDistance > 33554432L)
        WallDistance = 1200000L;
        // Set the wall struck code to an X wall
        WallCode = POV_XWALL;
        LastMapPosn = xMapPosn;
    }

yRaySetup();
yBitmap = yRayCast();

if (yBitmap & (WALL_TYPE_UPPER+WALL_TYPE_PASS))
    yBitmap = 0;

if (yBitmap)
    {
    yd = iLastY - yPlayer;
    mf = InvSinTable[ViewAngle];
    yDistance = (yd * mf) >> 8;
    if (yDistance > 33554432L)
        yDistance = 120000L;
    // If Y wall closer than X wall then use Y wall data
    if (yDistance < WallDistance)
        {
        WallDistance = yDistance;
        // Indicate the wall struck was a Y wall
        WallCode = POV_YWALL;
        BitmapNumber = yBitmap;
        LastMapPosn = yMapPosn;
        }
    }

//************************************************************************
// Since doors appear in the middle of the wall, adjust the minimum distance
// to it. This handles walking up close to a door.
//************************************************************************
if (BitmapNumber & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT))
    CheckDist += 64L;
BitmapNumber &= 0xFF;
if (WallCode)
    {
    yd = ViewCosTable[160] >> 3;
    WallDistance *= yd;
    // Now we strip off somemore decimal points and check round-up
    xd = WallDistance >> 12;
    if (WallDistance - (xd << 12) >= 2048)
        xd++;

//*********************************************************************
//    The last decimal points from the multiplication after the X and
//    Y rays is stripped off and checked for round-up.
//*********************************************************************
    WallDistance = xd >> 5;
    if (xd - (WallDistance << 5) >= 16)
        WallDistance++;

//*********************************************************************
// If the wall or object is further than the minimum distance, we can
// continue moving in this direction.
//*********************************************************************
    if (WallDistance > CheckDist)
        WallCode = POV_NOTHING;
    }

return(WallCode);
}
// **** End of Source ****


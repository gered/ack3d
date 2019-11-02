// This source file contains the functions needed to process floors.
// (c) 1995 ACK Software (Lary Myers)
#include <windows.h>
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

#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

#define MAX_F_VIEWHALFHEIGHT   50

extern  long FloorCosTable[];
extern    short   gWinStartX;
extern    short   gWinStartY;
extern    short   gWinEndX;
extern    short   gWinHalfHeight;
extern    UCHAR   *gScrnBufferCenter;
extern    long    WallDistTable[];

    long    zdTable[VIEW_WIDTH][50];
    long    mFactor;
    long    dFactor;

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Internal function called during the initialize process to setup the
// floor and light shading arrays.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
void SetupFloors(ACKENG *ae)
{
    short   i,a;
    int     ht,scanline,ht1;
    int     Scale_Fac;
    long    scan1,scan2,f;
    long    x,y,dist;
    long    Lastx,Lasty;

for ( i=0; i<12;i++ ) scantables[i] = ae->PalTable + (7*256);
for ( i=12;i<24;i++ ) scantables[i] = ae->PalTable + (6*256);
for ( i=24;i<36;i++ ) scantables[i] = ae->PalTable + (5*256);
for ( i=36;i<48;i++ ) scantables[i] = ae->PalTable + (4*256);
for ( i=48;i<60;i++ ) scantables[i] = ae->PalTable + (3*256);
for ( i=60;i<72;i++ ) scantables[i] = ae->PalTable + (2*256);
for ( i=72;i<84;i++ ) scantables[i] = ae->PalTable + (1*256);
for ( i=84;i<96;i++ ) scantables[i] = ae->PalTable;

Scale_Fac = (89 - ViewHeight) * 5;

ht = 89 - ViewHeight;

ht *= Scale_Fac;

for (i = 0; i < VIEW_WIDTH; i++)
    {
    f = FloorCosTable[i];
    zdTable[i][0] = 0;
    for (scanline = 1; scanline < MAX_F_VIEWHALFHEIGHT; scanline++)
        {
        scan2 = ht / scanline;
        zdTable[i][scanline] = (f * scan2) >> 15;
        if (zdTable[i][scanline-1] < zdTable[i][scanline])
            zdTable[i][scanline-1] = zdTable[i][scanline];

        }
    }

// Some debugging values for internal use
mFactor = 10368;
dFactor = 160;

}

// Optimization attempts were made with drawing the rotating background
// as the ceiling was being drawn and drawing the entire background before
// drawing any ceiling tiles or walls. The former seemed to be faster
// in most cases, so the define below was used to toggle between the two
// methods.
#define DRAW_BACK       1

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Draws the floor and ceiling horizontally.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
void AckDrawFloorHz(void)
{
    int     i,col,row,ht,Rcol,EndCol,BegCol;
    int     Scale_Fac,ScaleHt;
    UCHAR   *scr,*fscr,*bmp,*scrCeil,*cscr;
    UCHAR   *Rscr,*Rfscr,*RscrCeil,*Rcscr;
    UCHAR   *ba,*ba1,*Rba,*Rba1;
    short   va,va1;
    long    LastDist,scan2;
    long    cv,sv,dist,x,y,bx,by,mPos,bPos,wdist,bcol;
    long    Rbcol,Rwdist,xp,yp;
    long    *zd,*wPtr,*RwPtr,*xyPtr;
    long    fcv;
    UCHAR   ch;
    USHORT    bCode;

#if !(DRAW_BACK)
DrawBackDrop(); // Draw entire background
#endif

BegCol = gWinStartX;
EndCol = gWinEndX;

// Get the starting offset of the center row of the view
scr = gScrnBufferCenter + BegCol;
// Get the left side of the POV view
va = PlayerAngle - INT_ANGLE_32;
// Check for wrap-around
if (va < 0) va += INT_ANGLE_360;

// Adjust the viewing angle based on the starting view column
va += BegCol;
// and check for wrap-around
if (va >= INT_ANGLE_360)
    va -= INT_ANGLE_360;

// Get the starting column for the background, there are 640
// columns possible, so the remainder when we divide is where
// we want to begin displaying
bcol = va % 640;
// Temporarily hold onto the view height / 2
ht = gWinHalfHeight;
// Since our horizon should always have some form of wall or object,
// no matter how far away it is, we can optimize alittle here by not
// drawing the first few horizon rows (it does actually save time!).
// Start 5 video rows above the center row
scrCeil = scr - 1600;
// Start 6 video rows below the center row
scr += 1920;
// Initial right side of view
Rcol = 319;
wPtr = &WallDistTable[BegCol];      // Get pointers to avoid indexing
RwPtr = &WallDistTable[Rcol];

// The ScaleHt is used to determine the distance from the player for each
// row of the floor and ceiling. The value 89 was arrived at based on a
// reasonable height above the floor that the player is standing. By
// experimenting with this value, the effect of jumping up and down can
// be achieved, (as long as the walls and objects are made to jump by the
// same amount).
Scale_Fac = (89 - ViewHeight) * 5;
ScaleHt = 89 - ViewHeight;
ScaleHt *= Scale_Fac;

// Here we loop through each column of the viewing window
for (col = BegCol; col < EndCol; col += 2)
    {
// Pick up the cosine and sine values for the current angle
    cv = CosTable[va];
    sv = SinTable[va];
// Point to the left side of the current floor and ceiling columns
    fscr = scr;
    cscr = scrCeil;
// Advance the columns for the next pass
    scr += 2;
    scrCeil += 2;
// Pick up the current distance to the wall that was found for the
// current column
    wdist = *wPtr;
// Advance the wall distance pointer for the next pass
    wPtr += 2;

#if DRAW_BACK
// Pick up the pointer to the background image for the current column
    ba = BackArray[bcol++];
// Check for wrap-around in our 640 column image
    if (bcol > 639) bcol = 0;
// Pick up the next column as well since the floor and ceiling are
// always done in low resolution
    ba1 = BackArray[bcol++];
    if (bcol > 639) bcol = 0;
    ba += ht;
    ba1 += ht;
#endif

// Initialize a last distance variable
    LastDist = -1;
// Our floor cosine is used to counteract a fisheye effect
    fcv = FloorCosTable[col];

    for (row = 6; row <= ht; row++)
        {

        scan2 = ScaleHt / row;
// Get the distance for the current column and row position
        dist = (fcv * scan2) >> 15;

// If we're still closer than any wall
        if (dist < wdist)
            {
// Do some extra calculations if it's a new distance from last time
// (Sometimes our distance works out to be the same so we can avoid
// the next few lines)
            if (dist != LastDist)
                {
                x = xPglobal + ((cv * dist) >> 16);
                y = yPglobal + ((sv * dist) >> 16);
                LastDist = dist;
                }
            mPos = (y & 0xFC0) + (x >> 6);      // Calc the Map Posn
            if (mPos < 0L || mPos > 4095L)
                continue;
            bPos = (y & 63) + ((x & 63)<<6);    // Calc the Bitmap Pixel

// Get the bitmap number for our floor
            bCode = FloorMap[mPos];
// And use it to pull the actual bitmap from our array of wall bitmaps
            bmp = WallbMaps[bCode];
// Make sure it's really a bitmap and then get the actual pixel to display
            if (bmp != NULL)
                ch = bmp[bPos];

// Place the pixel in this column as well as the next. Since we have a
// scattering effect we can display in low resolution without too much
// loss in detail. This really speeds up the drawing of the floor and
// ceiling.
            *fscr = ch;                         // Put it into two locations
            fscr[1] = ch;                       // for low resolution

// Only draw a ceiling pixel if there is a ceiling tile placed in the map
// This gives the effect of seeing the background through holes in the ceiling
            if ((bCode = CeilMap[mPos]) != 0)
                {
                bmp = WallbMaps[bCode];
                if (bmp != NULL)
                    ch = bmp[bPos];
                *cscr = ch;
                cscr[1] = ch;
                }
#if DRAW_BACK
// If no ceiling is drawn, then we need to draw the background image
            else
                {
                *cscr = *ba;
                cscr[1] = *ba1;
                }
#endif
            }

        fscr += 320;        // Advance our screen position for the floor
        cscr -= 320;        // and the ceiling

#if DRAW_BACK
// Advance the pointers to the background image for the next pass
        ba--;
        ba1--;
#endif
        }

// Advance our current viewing angle by 2 since we are in low resolution
    va += 2;
// and check for wrap-around
    if (va >= INT_ANGLE_360) va -= INT_ANGLE_360;
    }

}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Draws the floor and ceiling horizontally.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
void AckDrawCeilingOnlyNS(void)
{
    int     i,col,row,ht,Rcol,EndCol,BegCol;
    int     Scale_Fac,ScaleHt;
    UCHAR   *scr,*fscr,*bmp,*scrCeil,*cscr;
    UCHAR   *Rscr,*Rfscr,*RscrCeil,*Rcscr;
    UCHAR   *ba,*ba1,*Rba,*Rba1;
    short   va,va1;
    long    LastDist,scan2;
    long    cv,sv,dist,x,y,bx,by,mPos,bPos,wdist,bcol;
    long    Rbcol,Rwdist,xp,yp;
    long    *zd,*wPtr,*RwPtr,*xyPtr;
    long    fcv;
    UCHAR   ch;
    USHORT    bCode;

#if !(DRAW_BACK)
DrawBackDrop(); // Draw entire background
#endif

BegCol = gWinStartX;
EndCol = gWinEndX;

// Get the starting offset of the center row of the view
scr = gScrnBufferCenter + BegCol;
// Get the left side of the POV view
va = PlayerAngle - INT_ANGLE_32;
// Check for wrap-around
if (va < 0) va += INT_ANGLE_360;

// Adjust the viewing angle based on the starting view column
va += BegCol;
// and check for wrap-around
if (va >= INT_ANGLE_360)
    va -= INT_ANGLE_360;

// Get the starting column for the background, there are 640
// columns possible, so the remainder when we divide is where
// we want to begin displaying
bcol = va % 640;
// Temporarily hold onto the view height / 2
ht = gWinHalfHeight;
// Since our horizon should always have some form of wall or object,
// no matter how far away it is, we can optimize alittle here by not
// drawing the first few horizon rows (it does actually save time!).
// Start 5 video rows above the center row
scrCeil = scr - 1600;
// Initial right side of view
Rcol = 319;
wPtr = &WallDistTable[BegCol];      // Get pointers to avoid indexing
RwPtr = &WallDistTable[Rcol];

// The ScaleHt is used to determine the distance from the player for each
// row of the floor and ceiling. The value 89 was arrived at based on a
// reasonable height above the floor that the player is standing. By
// experimenting with this value, the effect of jumping up and down can
// be achieved, (as long as the walls and objects are made to jump by the
// same amount).
Scale_Fac = (89 - ViewHeight) * 5;
ScaleHt = 89 - ViewHeight;
ScaleHt *= Scale_Fac;

// Here we loop through each column of the viewing window
for (col = BegCol; col < EndCol; col += 2)
    {
// Pick up the cosine and sine values for the current angle
    cv = CosTable[va];
    sv = SinTable[va];
// Point to the left side of the current ceiling columns
    cscr = scrCeil;
// Advance the columns for the next pass
    scrCeil += 2;
// Pick up the current distance to the wall that was found for the
// current column
    wdist = *wPtr;
// Advance the wall distance pointer for the next pass
    wPtr += 2;

#if DRAW_BACK
// Pick up the pointer to the background image for the current column
    ba = BackArray[bcol++];
// Check for wrap-around in our 640 column image
    if (bcol > 639) bcol = 0;
// Pick up the next column as well since the floor and ceiling are
// always done in low resolution
    ba1 = BackArray[bcol++];
    if (bcol > 639) bcol = 0;
    ba += ht;
    ba1 += ht;
#endif

// Initialize a last distance variable
    LastDist = -1;
// Our floor cosine is used to counteract a fisheye effect
    fcv = FloorCosTable[col];

    for (row = 6; row <= ht; row++)
        {

        scan2 = ScaleHt / row;
// Get the distance for the current column and row position
        dist = (fcv * scan2) >> 15;

// If we're still closer than any wall
        if (dist < wdist)
            {
// Do some extra calculations if it's a new distance from last time
// (Sometimes our distance works out to be the same so we can avoid
// the next few lines)
            if (dist != LastDist)
                {
                x = xPglobal + ((cv * dist) >> 16);
                y = yPglobal + ((sv * dist) >> 16);
                LastDist = dist;
                }
            mPos = (y & 0xFC0) + (x >> 6);      // Calc the Map Posn
            if (mPos < 0L || mPos > 4095L)
                continue;
            bPos = (y & 63) + ((x & 63)<<6);    // Calc the Bitmap Pixel

// Only draw a ceiling pixel if there is a ceiling tile placed in the map
// This gives the effect of seeing the background through holes in the ceiling
            if ((bCode = CeilMap[mPos]) != 0)
                {
                bmp = WallbMaps[bCode];
                if (bmp != NULL)
                    ch = bmp[bPos];
                *cscr = ch;
                cscr[1] = ch;
                }
#if DRAW_BACK
// If no ceiling is drawn, then we need to draw the background image
            else
                {
                *cscr = *ba;
                cscr[1] = *ba1;
                }
#endif
            }

        cscr -= 320;        // and the ceiling

#if DRAW_BACK
// Advance the pointers to the background image for the next pass
        ba--;
        ba1--;
#endif
        }

// Advance our current viewing angle by 2 since we are in low resolution
    va += 2;
// and check for wrap-around
    if (va >= INT_ANGLE_360) va -= INT_ANGLE_360;
    }

}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Draws the floor and ceiling horizontally.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
void AckDrawFloorOnlyNS(void)
{
    int     i,col,row,ht,Rcol,EndCol,BegCol;
    int     Scale_Fac,ScaleHt;
    UCHAR   *scr,*fscr,*bmp,*scrCeil,*cscr;
    UCHAR   *Rscr,*Rfscr,*RscrCeil,*Rcscr;
    UCHAR   *ba,*ba1,*Rba,*Rba1;
    short   va,va1;
    long    LastDist,scan2;
    long    cv,sv,dist,x,y,bx,by,mPos,bPos,wdist,bcol;
    long    Rbcol,Rwdist,xp,yp;
    long    *zd,*wPtr,*RwPtr,*xyPtr;
    long    fcv;
    UCHAR   ch;
    USHORT    bCode;

#if !(DRAW_BACK)
DrawBackDrop(); // Draw entire background
#endif

BegCol = gWinStartX;
EndCol = gWinEndX;

// Get the starting offset of the center row of the view
scr = gScrnBufferCenter + BegCol;
// Get the left side of the POV view
va = PlayerAngle - INT_ANGLE_32;
// Check for wrap-around
if (va < 0) va += INT_ANGLE_360;

// Adjust the viewing angle based on the starting view column
va += BegCol;
// and check for wrap-around
if (va >= INT_ANGLE_360)
    va -= INT_ANGLE_360;

// Get the starting column for the background, there are 640
// columns possible, so the remainder when we divide is where
// we want to begin displaying
bcol = va % 640;
// Temporarily hold onto the view height / 2
ht = gWinHalfHeight;
// Since our horizon should always have some form of wall or object,
// no matter how far away it is, we can optimize alittle here by not
// drawing the first few horizon rows (it does actually save time!).
// Start 6 video rows below the center row
scr += 1920;
// Initial right side of view
Rcol = 319;
wPtr = &WallDistTable[BegCol];      // Get pointers to avoid indexing
RwPtr = &WallDistTable[Rcol];

// The ScaleHt is used to determine the distance from the player for each
// row of the floor and ceiling. The value 89 was arrived at based on a
// reasonable height above the floor that the player is standing. By
// experimenting with this value, the effect of jumping up and down can
// be achieved, (as long as the walls and objects are made to jump by the
// same amount).
Scale_Fac = (89 - ViewHeight) * 5;
ScaleHt = 89 - ViewHeight;
ScaleHt *= Scale_Fac;

// Here we loop through each column of the viewing window
for (col = BegCol; col < EndCol; col += 2)
    {
// Pick up the cosine and sine values for the current angle
    cv = CosTable[va];
    sv = SinTable[va];
// Point to the left side of the current floor and ceiling columns
    fscr = scr;
// Advance the columns for the next pass
    scr += 2;
// Pick up the current distance to the wall that was found for the
// current column
    wdist = *wPtr;
// Advance the wall distance pointer for the next pass
    wPtr += 2;

#if DRAW_BACK
// Pick up the pointer to the background image for the current column
    ba = BackArray[bcol++];
// Check for wrap-around in our 640 column image
    if (bcol > 639) bcol = 0;
// Pick up the next column as well since the floor and ceiling are
// always done in low resolution
    ba1 = BackArray[bcol++];
    if (bcol > 639) bcol = 0;
    ba += ht;
    ba1 += ht;
#endif

// Initialize a last distance variable
    LastDist = -1;
// Our floor cosine is used to counteract a fisheye effect
    fcv = FloorCosTable[col];

    for (row = 6; row <= ht; row++)
        {

        scan2 = ScaleHt / row;
// Get the distance for the current column and row position
        dist = (fcv * scan2) >> 15;

// If we're still closer than any wall
        if (dist < wdist)
            {
// Do some extra calculations if it's a new distance from last time
// (Sometimes our distance works out to be the same so we can avoid
// the next few lines)
            if (dist != LastDist)
                {
                x = xPglobal + ((cv * dist) >> 16);
                y = yPglobal + ((sv * dist) >> 16);
                LastDist = dist;
                }
            mPos = (y & 0xFC0) + (x >> 6);      // Calc the Map Posn
            if (mPos < 0L || mPos > 4095L)
                continue;
            bPos = (y & 63) + ((x & 63)<<6);    // Calc the Bitmap Pixel

// Get the bitmap number for our floor
            bCode = FloorMap[mPos];
// And use it to pull the actual bitmap from our array of wall bitmaps
            bmp = WallbMaps[bCode];
// Make sure it's really a bitmap and then get the actual pixel to display
            if (bmp != NULL)
                ch = bmp[bPos];

// Place the pixel in this column as well as the next. Since we have a
// scattering effect we can display in low resolution without too much
// loss in detail. This really speeds up the drawing of the floor and
// ceiling.
            *fscr = ch;                         // Put it into two locations
            fscr[1] = ch;                       // for low resolution
            }

        fscr += 320;        // Advance our screen position for the floor

#if DRAW_BACK
// Advance the pointers to the background image for the next pass
        ba--;
        ba1--;
#endif
        }

// Advance our current viewing angle by 2 since we are in low resolution
    va += 2;
// and check for wrap-around
    if (va >= INT_ANGLE_360) va -= INT_ANGLE_360;
    }

}


//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// The function performs the same process as AckDrawFloorHz except here
// we include light shading with distance.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
void AckDrawFloor(void)
{
    int     i,col,row,ht,Rcol,EndCol,BegCol;
    int     Scale_Fac,ScaleHt;
    UCHAR   *scr,*fscr,*bmp,*scrCeil,*cscr;
    UCHAR   *Rscr,*Rfscr,*RscrCeil,*Rcscr;
    UCHAR   *ba,*ba1,*Rba,*Rba1;
    char    *stPtr;
    short   va,va1,LineNum;
    long    LastDist,scan2;
    long    cv,sv,dist,x,y,bx,by,mPos,bPos,wdist,bcol;
    long    Rbcol,Rwdist,xp,yp;
    long    *zd,*wPtr,*RwPtr,*xyPtr;
    long    fcv;
    UCHAR   ch;
    USHORT    bCode;

#if !(DRAW_BACK)
DrawBackDrop(); // Draw entire background
#endif

BegCol = gWinStartX;
EndCol = gWinEndX;

// Get the starting offset of the center row of the view
scr = gScrnBufferCenter + BegCol;
// Get the left side of the POV view
va = PlayerAngle - INT_ANGLE_32;
// Check for wrap-around
if (va < 0) va += INT_ANGLE_360;

// Adjust the viewing angle based on the starting view column
va += BegCol;
// and check for wrap-around
if (va >= INT_ANGLE_360)
    va -= INT_ANGLE_360;

// Get the starting column for the background, there are 640
// columns possible, so the remainder when we divide is where
// we want to begin displaying
bcol = va % 640;
// Temporarily hold onto the view height / 2
ht = gWinHalfHeight;
// Since our horizon should always have some form of wall or object,
// no matter how far away it is, we can optimize alittle here by not
// drawing the first few horizon rows (it does actually save time!).
// Start 5 video rows above the center row
scrCeil = scr - 1600;
// Start 6 video rows below the center row
scr += 1920;
// Initial right side of view
Rcol = 319;
wPtr = &WallDistTable[BegCol];      // Get pointers to avoid indexing
RwPtr = &WallDistTable[Rcol];

// The ScaleHt is used to determine the distance from the player for each
// row of the floor and ceiling. The value 89 was arrived at based on a
// reasonable height above the floor that the player is standing. By
// experimenting with this value, the effect of jumping up and down can
// be achieved, (as long as the walls and objects are made to jump by the
// same amount).
Scale_Fac = (89 - ViewHeight) * 5;
ScaleHt = 89 - ViewHeight;
ScaleHt *= Scale_Fac;

// Here we loop through each column of the viewing window
for (col = BegCol; col < EndCol; col += 2)
    {
// Pick up the cosine and sine values for the current angle
    cv = CosTable[va];
    sv = SinTable[va];
// Point to the left side of the current floor and ceiling columns
    fscr = scr;
    cscr = scrCeil;
// Advance the columns for the next pass
    scr += 2;
    scrCeil += 2;
// Pick up the current distance to the wall that was found for the
// current column
    wdist = *wPtr;
// Advance the wall distance pointer for the next pass
    wPtr += 2;

#if DRAW_BACK
// Pick up the pointer to the background image for the current column
    ba = BackArray[bcol++];
// Check for wrap-around in our 640 column image
    if (bcol > 639) bcol = 0;
// Pick up the next column as well since the floor and ceiling are
// always done in low resolution
    ba1 = BackArray[bcol++];
    if (bcol > 639) bcol = 0;
    ba += ht;
    ba1 += ht;
#endif

// Initialize a last distance variable
    LastDist = -1;
// Our floor cosine is used to counteract a fisheye effect
    fcv = FloorCosTable[col];
    LineNum = 0;

    for (row = 6; row <= ht; row++)
        {
        stPtr = scantables[LineNum++];

        scan2 = ScaleHt / row;
// Get the distance for the current column and row position
        dist = (fcv * scan2) >> 15;

// If we're still closer than any wall
        if (dist < wdist)
            {
// Do some extra calculations if it's a new distance from last time
// (Sometimes our distance works out to be the same so we can avoid
// the next few lines)
            if (dist != LastDist)
                {
                x = xPglobal + ((cv * dist) >> 16);
                y = yPglobal + ((sv * dist) >> 16);
                LastDist = dist;
                }
            mPos = (y & 0xFC0) + (x >> 6);      // Calc the Map Posn
            if (mPos < 0L || mPos > 4095L)
                continue;
            bPos = (y & 63) + ((x & 63)<<6);    // Calc the Bitmap Pixel

// Get the bitmap number for our floor
            bCode = FloorMap[mPos];
// And use it to pull the actual bitmap from our array of wall bitmaps
            bmp = WallbMaps[bCode];
// Make sure it's really a bitmap and then get the actual pixel to display
            if (bmp != NULL)
                ch = bmp[bPos];

            ch = stPtr[ch]; // Get shaded value for pixel

// Place the pixel in this column as well as the next. Since we have a
// scattering effect we can display in low resolution without too much
// loss in detail. This really speeds up the drawing of the floor and
// ceiling.
            *fscr = ch;                         // Put it into two locations
            fscr[1] = ch;                       // for low resolution

// Only draw a ceiling pixel if there is a ceiling tile placed in the map
// This gives the effect of seeing the background through holes in the ceiling
            if ((bCode = CeilMap[mPos]) != 0)
                {
                bmp = WallbMaps[bCode];
                if (bmp != NULL)
                    ch = bmp[bPos];
                ch = stPtr[ch];     // Get shaded value for pixel
                *cscr = ch;
                cscr[1] = ch;
                }
#if DRAW_BACK
// If no ceiling is drawn, then we need to draw the background image
            else
                {
                *cscr = *ba;
                cscr[1] = *ba1;
                }
#endif
            }

        fscr += 320;        // Advance our screen position for the floor
        cscr -= 320;        // and the ceiling

#if DRAW_BACK
// Advance the pointers to the background image for the next pass
        ba--;
        ba1--;
#endif
        }

// Advance our current viewing angle by 2 since we are in low resolution
    va += 2;
// and check for wrap-around
    if (va >= INT_ANGLE_360) va -= INT_ANGLE_360;
    }

}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Draw ceiling texture bitmaps only. Floor is assumed to be a solid color.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
void AckDrawCeilingOnly(void)
{
    int     i,col,row,ht,Rcol,EndCol,BegCol;
    int     Scale_Fac,ScaleHt;
    UCHAR   *scr,*fscr,*bmp,*scrCeil,*cscr;
    UCHAR   *Rscr,*Rfscr,*RscrCeil,*Rcscr;
    UCHAR   *ba,*ba1,*Rba,*Rba1;
    short   va,va1,LineNum;
    char    *stPtr;
    long    LastDist,scan2;
    long    cv,sv,dist,x,y,bx,by,mPos,bPos,wdist,bcol;
    long    Rbcol,Rwdist,xp,yp;
    long    *zd,*wPtr,*RwPtr,*xyPtr;
    long    fcv;
    UCHAR   ch;
    USHORT    bCode;

#if !(DRAW_BACK)
DrawBackDrop(); // Draw entire background
#endif

BegCol = gWinStartX;
EndCol = gWinEndX;

// Get the starting offset of the center row of the view
scr = gScrnBufferCenter + BegCol;
// Get the left side of the POV view
va = PlayerAngle - INT_ANGLE_32;
// Check for wrap-around
if (va < 0) va += INT_ANGLE_360;

// Adjust the viewing angle based on the starting view column
va += BegCol;
// and check for wrap-around
if (va >= INT_ANGLE_360)
    va -= INT_ANGLE_360;

// Get the starting column for the background, there are 640
// columns possible, so the remainder when we divide is where
// we want to begin displaying
bcol = va % 640;
// Temporarily hold onto the view height / 2
ht = gWinHalfHeight;
// Since our horizon should always have some form of wall or object,
// no matter how far away it is, we can optimize alittle here by not
// drawing the first few horizon rows (it does actually save time!).
// Start 5 video rows above the center row
scrCeil = scr - 1600;
// Initial right side of view
Rcol = 319;
wPtr = &WallDistTable[BegCol];      // Get pointers to avoid indexing
RwPtr = &WallDistTable[Rcol];

// The ScaleHt is used to determine the distance from the player for each
// row of the floor and ceiling. The value 89 was arrived at based on a
// reasonable height above the floor that the player is standing. By
// experimenting with this value, the effect of jumping up and down can
// be achieved, (as long as the walls and objects are made to jump by the
// same amount).
Scale_Fac = (89 - ViewHeight) * 5;
ScaleHt = 89 - ViewHeight;
ScaleHt *= Scale_Fac;

// Here we loop through each column of the viewing window
for (col = BegCol; col < EndCol; col += 2)
    {
// Pick up the cosine and sine values for the current angle
    cv = CosTable[va];
    sv = SinTable[va];
// Point to the left side of the current ceiling columns
    cscr = scrCeil;
// Advance the columns for the next pass
    scrCeil += 2;
// Pick up the current distance to the wall that was found for the
// current column
    wdist = *wPtr;
// Advance the wall distance pointer for the next pass
    wPtr += 2;

#if DRAW_BACK
// Pick up the pointer to the background image for the current column
    ba = BackArray[bcol++];
// Check for wrap-around in our 640 column image
    if (bcol > 639) bcol = 0;
// Pick up the next column as well since the floor and ceiling are
// always done in low resolution
    ba1 = BackArray[bcol++];
    if (bcol > 639) bcol = 0;
    ba += ht;
    ba1 += ht;
#endif

// Initialize a last distance variable
    LastDist = -1;
// Our floor cosine is used to counteract a fisheye effect
    fcv = FloorCosTable[col];
    LineNum = 0;

    for (row = 6; row <= ht; row++)
        {
        stPtr = scantables[LineNum++];
        scan2 = ScaleHt / row;
// Get the distance for the current column and row position
        dist = (fcv * scan2) >> 15;

// If we're still closer than any wall
        if (dist < wdist)
            {
// Do some extra calculations if it's a new distance from last time
// (Sometimes our distance works out to be the same so we can avoid
// the next few lines)
            if (dist != LastDist)
                {
                x = xPglobal + ((cv * dist) >> 16);
                y = yPglobal + ((sv * dist) >> 16);
                LastDist = dist;
                }
            mPos = (y & 0xFC0) + (x >> 6);      // Calc the Map Posn
            if (mPos < 0L || mPos > 4095L)
                continue;
            bPos = (y & 63) + ((x & 63)<<6);    // Calc the Bitmap Pixel

// Only draw a ceiling pixel if there is a ceiling tile placed in the map
// This gives the effect of seeing the background through holes in the ceiling
            if ((bCode = CeilMap[mPos]) != 0)
                {
                bmp = WallbMaps[bCode];
                if (bmp != NULL)
                    ch = bmp[bPos];

                ch = stPtr[ch]; // Get shaded pixel
                *cscr = ch;
                cscr[1] = ch;
                }
#if DRAW_BACK
// If no ceiling is drawn, then we need to draw the background image
            else
                {
                *cscr = *ba;
                cscr[1] = *ba1;
                }
#endif
            }

        cscr -= 320;        // and the ceiling

#if DRAW_BACK
// Advance the pointers to the background image for the next pass
        ba--;
        ba1--;
#endif
        }

// Advance our current viewing angle by 2 since we are in low resolution
    va += 2;
// and check for wrap-around
    if (va >= INT_ANGLE_360) va -= INT_ANGLE_360;
    }

}


//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Draw floor textured bitmaps only. Ceiling is a solid color.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
void AckDrawFloorOnly(void)
{
    int     i,col,row,ht,Rcol,EndCol,BegCol;
    int     Scale_Fac,ScaleHt;
    UCHAR   *scr,*fscr,*bmp,*scrCeil,*cscr;
    UCHAR   *Rscr,*Rfscr,*RscrCeil,*Rcscr;
    UCHAR   *ba,*ba1,*Rba,*Rba1;
    short   va,va1,LineNum;
    char    *stPtr;
    long    LastDist,scan2;
    long    cv,sv,dist,x,y,bx,by,mPos,bPos,wdist,bcol;
    long    Rbcol,Rwdist,xp,yp;
    long    *zd,*wPtr,*RwPtr,*xyPtr;
    long    fcv;
    UCHAR   ch;
    USHORT    bCode;

#if !(DRAW_BACK)
DrawBackDrop(); // Draw entire background
#endif

BegCol = gWinStartX;
EndCol = gWinEndX;

// Get the starting offset of the center row of the view
scr = gScrnBufferCenter + BegCol;
// Get the left side of the POV view
va = PlayerAngle - INT_ANGLE_32;
// Check for wrap-around
if (va < 0) va += INT_ANGLE_360;

// Adjust the viewing angle based on the starting view column
va += BegCol;
// and check for wrap-around
if (va >= INT_ANGLE_360)
    va -= INT_ANGLE_360;

// Get the starting column for the background, there are 640
// columns possible, so the remainder when we divide is where
// we want to begin displaying
bcol = va % 640;
// Temporarily hold onto the view height / 2
ht = gWinHalfHeight;
// Since our horizon should always have some form of wall or object,
// no matter how far away it is, we can optimize alittle here by not
// drawing the first few horizon rows (it does actually save time!).
// Start 6 video rows below the center row
scr += 1920;
// Initial right side of view
Rcol = 319;
wPtr = &WallDistTable[BegCol];      // Get pointers to avoid indexing
RwPtr = &WallDistTable[Rcol];

// The ScaleHt is used to determine the distance from the player for each
// row of the floor and ceiling. The value 89 was arrived at based on a
// reasonable height above the floor that the player is standing. By
// experimenting with this value, the effect of jumping up and down can
// be achieved, (as long as the walls and objects are made to jump by the
// same amount).
Scale_Fac = (89 - ViewHeight) * 5;
ScaleHt = 89 - ViewHeight;
ScaleHt *= Scale_Fac;

// Here we loop through each column of the viewing window
for (col = BegCol; col < EndCol; col += 2)
    {
// Pick up the cosine and sine values for the current angle
    cv = CosTable[va];
    sv = SinTable[va];
// Point to the left side of the current floor and ceiling columns
    fscr = scr;
// Advance the columns for the next pass
    scr += 2;
// Pick up the current distance to the wall that was found for the
// current column
    wdist = *wPtr;
// Advance the wall distance pointer for the next pass
    wPtr += 2;

#if DRAW_BACK
// Pick up the pointer to the background image for the current column
    ba = BackArray[bcol++];
// Check for wrap-around in our 640 column image
    if (bcol > 639) bcol = 0;
// Pick up the next column as well since the floor and ceiling are
// always done in low resolution
    ba1 = BackArray[bcol++];
    if (bcol > 639) bcol = 0;
    ba += ht;
    ba1 += ht;
#endif

// Initialize a last distance variable
    LastDist = -1;
// Our floor cosine is used to counteract a fisheye effect
    fcv = FloorCosTable[col];
    LineNum = 0;

    for (row = 6; row <= ht; row++)
        {
        stPtr = scantables[LineNum++];
        scan2 = ScaleHt / row;
// Get the distance for the current column and row position
        dist = (fcv * scan2) >> 15;

// If we're still closer than any wall
        if (dist < wdist)
            {
// Do some extra calculations if it's a new distance from last time
// (Sometimes our distance works out to be the same so we can avoid
// the next few lines)
            if (dist != LastDist)
                {
                x = xPglobal + ((cv * dist) >> 16);
                y = yPglobal + ((sv * dist) >> 16);
                LastDist = dist;
                }
            mPos = (y & 0xFC0) + (x >> 6);      // Calc the Map Posn
            if (mPos < 0L || mPos > 4095L)
                continue;
            bPos = (y & 63) + ((x & 63)<<6);    // Calc the Bitmap Pixel

// Get the bitmap number for our floor
            bCode = FloorMap[mPos];
// And use it to pull the actual bitmap from our array of wall bitmaps
            bmp = WallbMaps[bCode];
// Make sure it's really a bitmap and then get the actual pixel to display
            if (bmp != NULL)
                ch = bmp[bPos];

            ch = stPtr[ch]; // Get shaded pixel

// Place the pixel in this column as well as the next. Since we have a
// scattering effect we can display in low resolution without too much
// loss in detail. This really speeds up the drawing of the floor and
// ceiling.
            *fscr = ch;                         // Put it into two locations
            fscr[1] = ch;                       // for low resolution
            }

        fscr += 320;        // Advance our screen position for the floor

#if DRAW_BACK
// Advance the pointers to the background image for the next pass
        ba--;
        ba1--;
#endif
        }

// Advance our current viewing angle by 2 since we are in low resolution
    va += 2;
// and check for wrap-around
    if (va >= INT_ANGLE_360) va -= INT_ANGLE_360;
    }

}




//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// This function performs the same process as AckDrawFloorHz except
// we don't need to check for a bitmap index for every pixel since only
// one bitmap will be used for the floor and one bitmap for the ceiling.
// Draws a floor that contains only one type of bitmap. This is a much
// faster process and may be useful in some applications.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
void AckDrawOneFloor(void)
{
    int     i,col,row,ht,Rcol,EndCol,BegCol;
    int     Scale_Fac,ScaleHt;
    UCHAR   *scr,*fscr,*bmp,*cbmp,*scrCeil,*cscr;
    UCHAR   *Rscr,*Rfscr,*RscrCeil,*Rcscr;
    UCHAR   *ba,*ba1,*Rba,*Rba1;
    short   va,va1;
    long    LastDist,scan2;
    long    cv,sv,dist,x,y,bx,by,mPos,bPos,wdist,bcol;
    long    Rbcol,Rwdist,xp,yp;
    long    *zd,*wPtr,*RwPtr,*xyPtr;
    long    fcv;
    UCHAR   ch;
    USHORT    bCode;


BegCol = gWinStartX;
EndCol = gWinEndX;

// Get the starting offset of the center row of the view
scr = gScrnBufferCenter + BegCol;
// Get the left side of the POV view
va = PlayerAngle - INT_ANGLE_32;
// Check for wrap-around
if (va < 0) va += INT_ANGLE_360;

// Adjust the viewing angle based on the starting view column
va += BegCol;
// and check for wrap-around
if (va >= INT_ANGLE_360)
    va -= INT_ANGLE_360;

// Temporarily hold onto the view height / 2
ht = gWinHalfHeight;
// Since our horizon should always have some form of wall or object,
// no matter how far away it is, we can optimize alittle here by not
// drawing the first few horizon rows (it does actually save time!).
// Start 5 video rows above the center row
scrCeil = scr - 1600;
// Start 6 video rows below the center row
scr += 1920;
// Initial right side of view
Rcol = 319;
wPtr = &WallDistTable[BegCol];      // Get pointers to avoid indexing
RwPtr = &WallDistTable[Rcol];

// The ScaleHt is used to determine the distance from the player for each
// row of the floor and ceiling. The value 89 was arrived at based on a
// reasonable height above the floor that the player is standing. By
// experimenting with this value, the effect of jumping up and down can
// be achieved, (as long as the walls and objects are made to jump by the
// same amount).
Scale_Fac = (89 - ViewHeight) * 5;
ScaleHt = 89 - ViewHeight;
ScaleHt *= Scale_Fac;

bmp = aeGlobal->bMaps[aeGlobal->FloorBitmap];
cbmp = aeGlobal->bMaps[aeGlobal->CeilBitmap];

// Here we loop through each column of the viewing window
for (col = BegCol; col < EndCol; col += 2)
    {
// Pick up the cosine and sine values for the current angle
    cv = CosTable[va];
    sv = SinTable[va];
// Point to the left side of the current floor and ceiling columns
    fscr = scr;
    cscr = scrCeil;
// Advance the columns for the next pass
    scr += 2;
    scrCeil += 2;
// Pick up the current distance to the wall that was found for the
// current column
    wdist = *wPtr;
// Advance the wall distance pointer for the next pass
    wPtr += 2;

// Initialize a last distance variable
    LastDist = -1;
// Our floor cosine is used to counteract a fisheye effect
    fcv = FloorCosTable[col];

    for (row = 6; row <= ht; row++)
        {
        scan2 = ScaleHt / row;
// Get the distance for the current column and row position
        dist = (fcv * scan2) >> 15;

// If we're still closer than any wall
        if (dist < wdist)
            {
// Do some extra calculations if it's a new distance from last time
// (Sometimes our distance works out to be the same so we can avoid
// the next few lines)
            if (dist != LastDist)
                {
                x = xPglobal + ((cv * dist) >> 16);
                y = yPglobal + ((sv * dist) >> 16);
                LastDist = dist;
                }

            bPos = (y & 63) + ((x & 63)<<6);    // Calc the Bitmap Pixel
            ch = bmp[bPos];

// Place the pixel in this column as well as the next. Since we have a
// scattering effect we can display in low resolution without too much
// loss in detail. This really speeds up the drawing of the floor and
// ceiling.
            *fscr = ch;                         // Put it into two locations
            fscr[1] = ch;                       // for low resolution

            ch = cbmp[bPos];
            *cscr = ch;
            cscr[1] = ch;
            }

        fscr += 320;        // Advance our screen position for the floor
        cscr -= 320;        // and the ceiling
        }

// Advance our current viewing angle by 2 since we are in low resolution
    va += 2;
// and check for wrap-around
    if (va >= INT_ANGLE_360) va -= INT_ANGLE_360;
    }

}

// **** End of Source ****


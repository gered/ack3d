//****************** ( Animation Construction Kit 3D ) **********************
//            Ray Casting Routines
// CopyRight (c) 1993      Author: Lary Myers
//***************************************************************************

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

extern  short     ViewAngle;

//**************************************************************************
//
//**************************************************************************
short ObjectExist(UCHAR onum)
{
    short     i;
    short     result = 0;

if (!FoundObjectCount)
    return(result);

for (i = 0; i < FoundObjectCount; i++)
    {
    if (ObjectsSeen[i] == onum)
    return(1);
    }

return(result);
}

    long    x_xPos,x_yPos,x_xNext,x_yNext;
    long    y_xPos,y_yPos,y_xNext,y_yNext;

//*************************************************************************
//
//*************************************************************************
void xRaySetup(void)
{

x_yNext = yNextTable[ViewAngle];  // PreCalc'd value of BITMAP_WIDTH * Tan(angle)

if (ViewAngle > INT_ANGLE_270 || ViewAngle < INT_ANGLE_90)
    {
    x_xPos  = xBegGlobal + BITMAP_WIDTH;   // Looking to the right
    x_xNext = BITMAP_WIDTH;        // Positive direction
    }
else
    {
    x_xPos  = xBegGlobal;          // Looking to the left
    x_xNext = -BITMAP_WIDTH;           // Negative direction
    x_yNext = -x_yNext;
    }

// Calculate the Y coordinate for the current square
x_yPos = (((long)x_xPos - (long)xPglobal) * LongTanTable[ViewAngle]) + yPglobalHI;

}

//**************************************************************************
//
//**************************************************************************
UINT xRayCast(void)
{
    UINT    Color;
    short     i,j,mx,my;
    short     TablePosn;
    short     MapPosn,CurPosn;
    short     xBeg;
    short     BitmapColumn;
    short     xCenter,yCenter,xAdj;
    short     ObjPosn;
    short     oBegX,oBegY;
    long    xd,yd,sy;
    long    ObjDist;

while (1)
    {
    if (x_xPos < 0 || x_xPos > GRID_XMAX ||
    x_yPos < 0 || x_yPos > GRID_YMAXLONG)
    break;

//*************   Fixed point     Y/64 * 64 X / 64 **********
    MapPosn = ((x_yPos >> FP_SHIFT) & 0xFFC0) + (x_xPos >> 6);


    if ((Color = ObjGrid[MapPosn]) != 0)
    {
    Color &= 0x7F;
    if (!ObjectExist(Color))
        ObjectsSeen[FoundObjectCount++] = Color;

    }



// Check to see if a wall is being struck by the ray
    if ((Color = xGridGlobal[MapPosn]) != 0)
    {
    xMapPosn = MapPosn;     // Hold onto the map location
    iLastX   = x_xPos;
    LastY1   = x_yPos;
    if ((Color & 0xFF) == DOOR_XCODE)    // Is this a door?
        {
        yd = ((x_yPos >> FP_SHIFT) & GRID_MASK);  // Get the left side
        xd = yd + BITMAP_WIDTH;           // And the right side
        ObjDist = (x_yPos + (x_yNext >> 1)) >> FP_SHIFT; // Calc door distance
        if (ObjDist < yd || ObjDist > xd)   // Is door visible?
        {
        x_xPos += x_xNext;      // Nope, continue casting
        x_yPos += x_yNext;      // the ray as before
        continue;
        }

        LastY1 = x_yPos + (x_yNext >> 1);   // Adjust the X,Y values so
        iLastX += (x_xNext >> 1);       // the door is halfway in sq.
        }

    if (Color & DOOR_TYPE_SECRET)
        {
        if (xSecretColumn != 0)
        {
        sy = xSecretColumn * LongTanTable[ViewAngle];
        ObjDist = (x_yPos + sy) >> FP_SHIFT;
        yd = ((x_yPos >> FP_SHIFT) & GRID_MASK); // Get the left side
        xd = yd + BITMAP_WIDTH;          // And the right side
        if (ObjDist < yd || ObjDist > xd)    // Is door visible?
            {
            x_xPos += x_xNext;          // Nope, continue casting
            x_yPos += x_yNext;          // the ray as before
            continue;
            }
        LastY1 = x_yPos + sy;
        iLastX += xSecretColumn;
        }
        }

    return(Color);
    }

    x_xPos += x_xNext;      // Next X coordinate (fixed at 64 or -64)
    x_yPos += x_yNext;      // Next calculated Y coord for a delta of X
    }

return(0);          // Return that no wall was found
}


//*************************************************************************
//
//*************************************************************************
UINT OldxRay(void)
{
    UINT    Color;
    short     i,j,mx,my;
    short     TablePosn;
    short     MapPosn,CurPosn;
    short     xBeg;
    long    xPos,xNext;
    short     BitmapColumn;
    short     xCenter,yCenter,xAdj;
    short     ObjPosn;
    short     oBegX,oBegY;
    long    yPos;
    long    yNext;
    long    xd,yd,sy;
    long    ObjDist;

yNext = yNextTable[ViewAngle];  // PreCalc'd value of BITMAP_WIDTH * Tan(angle)

if (ViewAngle > INT_ANGLE_270 || ViewAngle < INT_ANGLE_90)
    {
    xPos  = xBegGlobal + BITMAP_WIDTH;  // Looking to the right
    xNext = BITMAP_WIDTH;       // Positive direction
    }
else
    {
    xPos  = xBegGlobal;         // Looking to the left
    xNext = -BITMAP_WIDTH;      // Negative direction
    yNext = -yNext;
    }

// Calculate the Y coordinate for the current square
yPos = (((long)xPos - (long)xPglobal) * LongTanTable[ViewAngle]) + yPglobalHI;

while (1)
    {
    if (xPos < 0 || xPos > GRID_XMAX ||
    yPos < 0 || yPos > GRID_YMAXLONG)
    break;

//*************   Fixed point     Y/64 * 64 X / 64 ***********
    MapPosn = ((yPos >> FP_SHIFT) & 0xFFC0) + (xPos >> 6);


    if ((Color = ObjGrid[MapPosn]) != 0)
    {
    Color &= 0x7F;
    if (!ObjectExist(Color))
        ObjectsSeen[FoundObjectCount++] = Color;

    }



// Check to see if a wall is being struck by the ray
    if ((Color = xGridGlobal[MapPosn]) != 0)
    {
    xMapPosn = MapPosn;     // Hold onto the map location
    iLastX   = xPos;
    LastY1   = yPos;
    if ((Color & 0xFF) == DOOR_XCODE)   // Is this a door?
        {
        yd = ((yPos >> FP_SHIFT) & GRID_MASK);  // Get the left side
        xd = yd + BITMAP_WIDTH;         // And the right side
        ObjDist = (yPos + (yNext >> 1)) >> FP_SHIFT; // Calc door distance
        if (ObjDist < yd || ObjDist > xd)   // Is door visible?
        {
        xPos += xNext;          // Nope, continue casting
        yPos += yNext;          // the ray as before
        continue;
        }

        LastY1 = yPos + (yNext >> 1);   // Adjust the X,Y values so
        iLastX += (xNext >> 1);     // the door is halfway in sq.
        }

    if (Color & DOOR_TYPE_SECRET)
        {
        if (xSecretColumn != 0)
        {
        sy = xSecretColumn * LongTanTable[ViewAngle];
        ObjDist = (yPos + sy) >> FP_SHIFT;
        yd = ((yPos >> FP_SHIFT) & GRID_MASK); // Get the left side
        xd = yd + BITMAP_WIDTH;            // And the right side
        if (ObjDist < yd || ObjDist > xd)      // Is door visible?
            {
            xPos += xNext;             // Nope, continue casting
            yPos += yNext;             // the ray as before
            continue;
            }
        LastY1 = yPos + sy;
        iLastX += xSecretColumn;
        }
        }

    return(Color);
    }

    xPos += xNext;  // Next X coordinate (fixed at 64 or -64)
    yPos += yNext;  // Next calculated Y coord for a delta of X
    }

return(0);      // Return that no wall was found
}


//*************************************************************************
//
//*************************************************************************
void yRaySetup(void)
{

y_xNext = xNextTable[ViewAngle];  // Pre-calc'd value of BITMAP_WIDTH / tan(angle)

if (ViewAngle < INT_ANGLE_180)
    {
    y_yPos  = yBegGlobal + BITMAP_WIDTH;   // Looking down
    y_yNext = BITMAP_WIDTH;        // Positive direction
    }
else
    {
    y_yPos  = yBegGlobal;          // Looking up
    y_yNext = -BITMAP_WIDTH;           // Negative direction
    y_xNext = -y_xNext;
    }

// Calculate the X coordinate for the current square
y_xPos = (((long)y_yPos - (long)yPglobal) * LongInvTanTable[ViewAngle]) + xPglobalHI;

}

//*************************************************************************
//
//*************************************************************************
UINT yRayCast(void)
{
    UINT    Color;
    short     i,j,mx,my;
    short     MapPosn;
    short     yBeg;
    short     BitmapColumn;
    short     xCenter,yCenter,yAdj;
    short     ObjPosn;
    short     oBegX;
    long    xd,yd,ObjDist,sx;

while (1)
    {
    if (y_xPos < 0 || y_xPos > GRID_XMAXLONG ||
    y_yPos < 0 || y_yPos > GRID_YMAX)
    break;

//**********   Y/64 * 64     Fixed point and /64 *****
    MapPosn = (y_yPos & 0xFFC0) + (y_xPos >> (FP_SHIFT+6));


    if ((Color = ObjGrid[MapPosn]) != 0)
    {
    Color &= 0x7F;
    if (!ObjectExist(Color))
        ObjectsSeen[FoundObjectCount++] = Color;

    }


// Check for a wall being struck
    if ((Color = yGridGlobal[MapPosn]) != 0)
    {
    yMapPosn = MapPosn;     // Hold onto map position
    LastX1   = y_xPos;
    iLastY   = y_yPos;

    if ((Color & 0xFF) == DOOR_YCODE)    // Is this a door?
        {
        yd = ((y_xPos >> FP_SHIFT) & GRID_MASK);  // Calc top side of square
        xd = yd + BITMAP_WIDTH;           // And bottom side of square
        ObjDist = (y_xPos + (y_xNext >> 1)) >> FP_SHIFT;
        if (ObjDist < yd || ObjDist > xd)         // Is door visible?
        {
        y_xPos += y_xNext;      // No, continue on with ray cast
        y_yPos += y_yNext;
        continue;
        }

        LastX1 = y_xPos + (y_xNext >> 1);   // Adjust coordinates so door is
        iLastY += (y_yNext >> 1);       // Halfway into wall
        }

    if (Color & DOOR_TYPE_SECRET)
        {
        if (ySecretColumn != 0)
        {
        sx = ySecretColumn * LongInvTanTable[ViewAngle];
        ObjDist = (y_xPos + sx) >> FP_SHIFT;
        yd = ((y_xPos >> FP_SHIFT) & GRID_MASK);  // Get the top side
        xd = yd + BITMAP_WIDTH;           // And the bottom side
        if (ObjDist < yd || ObjDist > xd)     // Is door visible?
            {
            y_xPos += y_xNext;          // Nope, continue casting
            y_yPos += y_yNext;          // the ray as before
            continue;
            }
        LastX1 = y_xPos + sx;
        iLastY += ySecretColumn;
        }

        }

    return(Color);
    }

    y_xPos += y_xNext;          // Next calculated X value for delta Y
    y_yPos += y_yNext;          // Next fixed value of 64 or -64

    }

return(0);      // Return here if no Y wall is found
}

//*************************************************************************
//
//*************************************************************************
UINT OldyRay(void)
{
    UINT    Color;
    short     i,j,mx,my;
    short     MapPosn;
    short     yBeg;
    long    yPos,yNext;
    short     BitmapColumn;
    short     xCenter,yCenter,yAdj;
    short     ObjPosn;
    short     oBegX;
    long    xPos;
    long    xNext;
    long    xd,yd,ObjDist,sx;

xNext = xNextTable[ViewAngle];  // Pre-calc'd value of BITMAP_WIDTH / tan(angle)

if (ViewAngle < INT_ANGLE_180)
    {
    yPos  = yBegGlobal + BITMAP_WIDTH;   /* Looking down       */
    yNext = BITMAP_WIDTH;      /* Positive direction */
    }
else
    {
    yPos  = yBegGlobal;          /* Looking up         */
    yNext = -BITMAP_WIDTH;     /* Negative direction */
    xNext = -xNext;
    }

/* Calculate the X coordinate for the current square */
xPos = (((long)yPos - (long)yPglobal) * LongInvTanTable[ViewAngle]) + xPglobalHI;

while (1)
    {
    if (xPos < 0 || xPos > GRID_XMAXLONG ||
    yPos < 0 || yPos > GRID_YMAX)
    break;

/***********   Y/64 * 64     Fixed point and /64 ******/
//  MapPosn = ((yPos / BITMAP_WIDTH) * GRID_WIDTH) + (xPos >> (FP_SHIFT+BITMAP_SHIFT));
//  MapPosn = ((yPos & GRID_MASK) >> 1) + (xPos >> (FP_SHIFT+BITMAP_SHIFT));
    MapPosn = (yPos & 0xFFC0) + (xPos >> (FP_SHIFT+6));


    if ((Color = ObjGrid[MapPosn]) != 0)
    {
    Color &= 0x7F;
    if (!ObjectExist(Color))
        ObjectsSeen[FoundObjectCount++] = Color;

    }


/** Check for a wall being struck **/
    if ((Color = yGridGlobal[MapPosn]) != 0)
    {
    yMapPosn = MapPosn;     /* Hold onto map position */
    LastX1   = xPos;
    iLastY   = yPos;

    if ((Color & 0xFF) == DOOR_YCODE)    /* Is this a door? */
        {
        yd = ((xPos >> FP_SHIFT) & GRID_MASK);  /* Calc top side of square   */
        xd = yd + BITMAP_WIDTH;   /* And bottom side of square */
        ObjDist = (xPos + (xNext >> 1)) >> FP_SHIFT;
        if (ObjDist < yd || ObjDist > xd)   /* Is door visible? */
        {
        xPos += xNext;      /* No, continue on with ray cast */
        yPos += yNext;
        continue;
        }

        LastX1 = xPos + (xNext >> 1);   /* Adjust coordinates so door is */
        iLastY += (yNext >> 1);     /* Halfway into wall         */
        }

    if (Color & DOOR_TYPE_SECRET)
        {
        if (ySecretColumn != 0)
        {
        sx = ySecretColumn * LongInvTanTable[ViewAngle];
        ObjDist = (xPos + sx) >> FP_SHIFT;
        yd = ((xPos >> FP_SHIFT) & GRID_MASK);    /* Get the top side    */
        xd = yd + BITMAP_WIDTH;       /* And the bottom side */
        if (ObjDist < yd || ObjDist > xd)   /* Is door visible? */
            {
            xPos += xNext;          /* Nope, continue casting */
            yPos += yNext;          /* the ray as before      */
            continue;
            }
        LastX1 = xPos + sx;
        iLastY += ySecretColumn;
        }

        }

    return(Color);
    }

    xPos += xNext;      /* Next calculated X value for delta Y */
    yPos += yNext;      /* Next fixed value of 64 or -64       */

    }

return(0);      /* Return here if no Y wall is found */
}


/****************************************************************************
**                                     **
****************************************************************************/
UINT xRayMulti(UINT MinDist,short MinHeight)
{
    UINT    Color;
    short     i,j,mx,my;
    short     TablePosn;
    short     MapPosn,CurPosn;
    short     xBeg;
    long    xPos,xNext;
    short     BitmapColumn;
    short     xCenter,yCenter,xAdj;
    short     ObjPosn;
    short     oBegX,oBegY;
    long    yPos;
    long    yNext;
    long    xd,yd,sy;
    long    ObjDist;

yNext = yNextTable[ViewAngle];  /* PreCalc'd value of BITMAP_WIDTH * Tan(angle) */

if (ViewAngle > INT_ANGLE_270 || ViewAngle < INT_ANGLE_90)
    {
    xPos  = xBegGlobal + BITMAP_WIDTH;   /* Looking to the right */
    xNext = BITMAP_WIDTH;      /* Positive direction   */
    }
else
    {
    xPos  = xBegGlobal;          /* Looking to the left  */
    xNext = -BITMAP_WIDTH;     /* Negative direction   */
    yNext = -yNext;
    }

/* Calculate the Y coordinate for the current square */
yPos = (((long)xPos - (long)xPglobal) * LongTanTable[ViewAngle]) + yPglobalHI;

while (1)
    {
    if (xPos < 0 || xPos > GRID_XMAX ||
    yPos < 0 || yPos > GRID_YMAXLONG)
    break;

/**************   Fixed point     Y/64 * 64 X / 64 ***********/
    MapPosn = ((yPos >> FP_SHIFT) & 0xFFC0) + (xPos >> 6);

/* Check to see if a wall is being struck by the ray */
    if ((Color = xGridGlobal[MapPosn]) & WALL_TYPE_MULTI)
    {
    if ((Color & 0xFF) > MinHeight)
        {
        xd = xPos - xPglobal;
        yd = InvCosTable[ViewAngle] >> 4;
        if (MinDist < ((xd * yd) >> 10))
        {
        xMapPosn = MapPosn;     /* Hold onto the map location */
        iLastX   = xPos;
        LastY1   = yPos;
        return(Color);
        }
        }
    }

    xPos += xNext;  /* Next X coordinate (fixed at 64 or -64)   */
    yPos += yNext;  /* Next calculated Y coord for a delta of X */
    }

return(0);      /* Return that no wall was found */
}

/****************************************************************************
**                                     **
****************************************************************************/
UINT yRayMulti(UINT MinDist,short MinHeight)
{
    UINT    Color;
    short     i,j,mx,my;
    short     MapPosn;
    short     yBeg;
    long    yPos,yNext;
    short     BitmapColumn;
    short     xCenter,yCenter,yAdj;
    short     ObjPosn;
    short     oBegX;
    long    xPos;
    long    xNext;
    long    xd,yd,ObjDist,sx;

xNext = xNextTable[ViewAngle];  /* Pre-calc'd value of BITMAP_WIDTH / tan(angle) */

if (ViewAngle < INT_ANGLE_180)
    {
    yPos  = yBegGlobal + BITMAP_WIDTH;   /* Looking down       */
    yNext = BITMAP_WIDTH;      /* Positive direction */
    }
else
    {
    yPos  = yBegGlobal;          /* Looking up         */
    yNext = -BITMAP_WIDTH;     /* Negative direction */
    xNext = -xNext;
    }

/* Calculate the X coordinate for the current square */
xPos = (((long)yPos - (long)yPglobal) * LongInvTanTable[ViewAngle]) + xPglobalHI;

while (1)
    {
    if (xPos < 0 || xPos > GRID_XMAXLONG ||
    yPos < 0 || yPos > GRID_YMAX)
    break;

/***********   Y/64 * 64     Fixed point and /64 ******/
    MapPosn = (yPos & 0xFFC0) + (xPos >> (FP_SHIFT+6));

/** Check for a wall being struck **/
    if ((Color = yGridGlobal[MapPosn]) & WALL_TYPE_MULTI)
    {
    if ((Color & 0xFF) > MinHeight)
        {
        xd = yPos - yPglobal;
        yd = InvCosTable[ViewAngle] >> 4;
        if (MinDist < ((xd * yd) >> 10))
        {
        yMapPosn = MapPosn;     /* Hold onto the map location */
        LastX1   = xPos;
        iLastY   = yPos;
        return(Color);
        }
        }
    }

    xPos += xNext;      /* Next calculated X value for delta Y */
    yPos += yNext;      /* Next fixed value of 64 or -64       */

    }

return(0);      /* Return here if no Y wall is found */
}


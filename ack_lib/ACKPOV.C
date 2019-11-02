// Source file ACKPOV.C - Player and Object Movement routines
// (c) 1995 ACK Software (Lary Myers)

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

//************************************************************************
// Internal function called by AckMovePOV(). Checks the passed X and Y
// coordinates of the player against the object coordinates to see if the player will 
// encouner an object.
//************************************************************************
short AckCheckObjPosn(short xPlayer,short yPlayer, short oIndex)
{
    short       i,result,maxObj;
    short       MapPosn;
    NEWOBJECT   **oList;
    NEWOBJECT   *oPtr;

result = POV_NOTHING;                                           // Initialize to nothing found 
MapPosn = (yPlayer & 0xFFC0) + (xPlayer >> 6);     // Calculate grid square the player will be in
maxObj = aeGlobal->MaxObjects;                               // Total number of objects used
oList = &aeGlobal->ObjList[0];                                   // Reference the list of objects

for (i = 0; i < maxObj; i++)                                         // Loop and check each object in the list
    {
    oPtr = oList[i];                                                         // Point to current object
    if (oPtr == NULL)                                                    // No object here; skip to next object in list
    	continue;

    if (!oPtr->Active || oPtr->Flags & OF_PASSABLE)  // Object is not active or is passable
    	continue;                                                           // Skip to next object in list

    if (MapPosn == oPtr->mPos && i != oIndex)       // Object is found in the player's grid position
    	{
    	LastObjectHit = i;                                         // Store the number of the object found
    	return(POV_OBJECT);                                //  Return flag to indicate an object is found
    	}
    }
return(result);
}

//************************************************************************
// Internal function called by AckMovePOV() to see if a wall or object is located in
// the player's current grid square. This function checks for walls or objects in the
// x-plane using the xGrid array. The bitmap code for the wall or object is returned.
// A value returned of 0 indicates that no wall or object is present or that the wall
// is passable.
//************************************************************************
USHORT GetWallX(short mPos)
{
    USHORT    mCode;

mCode = xGridGlobal[mPos];           // Get bitmap code at specified map position
if (mCode & WALL_TYPE_PASS)  // Passable walls can be walked through
    mCode = 0;                                   
return(mCode);
}

//************************************************************************
// Internal function called by AckMovePOV() to see if a wall or object is located in
// the player's current grid square. This function checks for walls or objects in the
// y-plane using the yGrid array. The bitmap code for the wall or object is returned.
// A value returned of 0 indicates that no wall or object is present or that the wall
// is passable.
//************************************************************************
USHORT GetWallY(short mPos)
{
    USHORT    mCode;

mCode = yGridGlobal[mPos];            // Get bitmap code at specified map position
if (mCode & WALL_TYPE_PASS)   // Passable walls can be walked through
    mCode = 0;
return(mCode);
}

//************************************************************************
// Moves the POV based on Angle for Amount. After moving but prior to
// returning the position of the POV is check for collisions.
//************************************************************************
short AckMovePOV(short Angle,short Amount)
{
    short   x1,y1,HitResult;            // New coordinate position
    short   xp,yp;                      // Starting player coordinates
    short   xLeft,xRight,yTop,yBottom;  // Coordinates for grid square
    short   mPos;               // Map position for xGrid[], yGrid[]
    USHORT  mCodeX,mCodeY;      // Return codes for x,y wall arrays

HitResult = POV_NOTHING;        // We haven't hit anything yet
xp = aeGlobal->xPlayer;         // Get the current x,y player coordinates
yp = aeGlobal->yPlayer;

xLeft = xp & 0xFFC0;            // Determine coordinates of the boundaries
yTop = yp & 0xFFC0;             // of the grid square we're in.
xRight = xLeft + GRID_SIZE;
yBottom = yTop + GRID_SIZE;

// Calculate the x,y distance of movement using the angle and distance
// x1,y1 = the new coordinate position of the player.
x1 = xp + (long)((CosTable[Angle] * Amount) >> FP_SHIFT);
y1 = yp + (long)((SinTable[Angle] * Amount) >> FP_SHIFT);
// Calculate current map position for the xGrid[] and yGrid[] arrays
mPos = yTop + (xp >> 6); // Current Map Posn

// It's time to see what happens when we move
if (x1 < xp)				        // Are we moving left?
    {
    if (GetWallX(mPos))			    // Wall found in current square (left edge)
    	{
    	if (x1 < xLeft || abs(x1-xLeft) < 28)   // We crossed the wall or we're too close
    	    {
    	    x1 = xp;                // Use the previous x position
    	    HitResult = POV_SLIDEX;   // We're possibly sliding along the left x wall
    	    }
    	}
    }

if (x1 > xp)				        // Are we moving right?
    {
    if (GetWallX(mPos+1))		    // Wall found in current square (right edge)
        {
    	if (x1 > xRight || abs(xRight-x1) < 28) // We crossed the wall or we're too close
    	    {
    	    x1 = xp;                // Use the previous x position
    	    HitResult = POV_SLIDEX; // We're possibly sliding along the right x wall
    	    }
    	}
    }

if (y1 < yp)				        // Are we moving up?
    {
    if (GetWallY(mPos))			    //	Wall found in current square (top edge)
    	{
    	if (y1 < yTop || abs(y1-yTop) < 28) // We crossed the wall or we're too close
    	    {
    	    y1 = yp;                // Use the previous y position
    	    HitResult = POV_SLIDEY; // We're possibly sliding along the top wall
    	    }
    	}
    }

if (y1 > yp)				        // Are we moving down?
    {
    if (GetWallY(mPos+GRID_WIDTH))  // Wall found in current square (bottom edge)
    	{
    	if (y1 > yBottom || abs(yBottom-y1) < 28) // We crossed the wall or we're too close
    	    {
    	    y1 = yp;                // Use the previous y position
    	    HitResult = POV_SLIDEY;    // We're sliding along the bottom wall
    	    }
    	}
    }

// A wall or object hasn't been hit yet--we must look further.
// The current grid sqaure will be divided into four regions:
//   A = top left; B = top right; C = bottom left; D = bottom right
// Each of these regions will be checked to see if the player's new position (x1,y1)
// is close to a wall or object that borders one of these regions.
// Each grid square is 64x64 units, so each region to check is 32x32 units.
if (!HitResult)
    {                       // Check region A--top left area of grid
    if (y1 < (yTop+32))     // New y position falls in top half
    	{
    	if (x1 < (xLeft+32))    // New x position falls in left half
    	    {
    	    mCodeX = GetWallX(mPos-GRID_WIDTH); // Check adjacent x wall (to left)
    	    mCodeY = GetWallY(mPos-1);          // Check adjacent y wall (above)

    	    if (mCodeX && y1 < (yTop+28))   // Adjacent x wall found and new y coord
        		{                           // is within 28 units
        		if (x1 < (xLeft+28))        // New x coord. is within 28 units of edge
        		    {
        		    if (xp > (xLeft+27))    // Previous x position was outside range
            			{
            			x1 = xp;            // Use previous x position
            			HitResult = POV_SLIDEX;
            			}
        		    else
            			{
            			y1 = yp;            // Use previous y position
            			HitResult = POV_SLIDEY;
            			}
        		    }
        		}

    	    if (mCodeY && x1 < (xLeft+28))  // Adjacent y wall found and new x coord.
        		{                           // is within 28 units
        		if (y1 < (yTop+28))         // New y coord. is within 28 units of edge
        		    {
        		    if (yp > (yTop+27))     // Previous y position was outside range
            			{
            			y1 = yp;            // Use previous y position
            			HitResult = POV_SLIDEY;
            			}
        		    else
            			{
            			x1 = xp;            // Use previous x position
            			HitResult = POV_SLIDEX;
            			}
        		    }
        		}
    	    }
        // Check region B--top right area
        if (x1 > (xRight-32) && !HitResult)// New x is at top right
            {
            mCodeX = GetWallX(mPos+1-GRID_WIDTH);   // Check adjacent x wall (to right)
            mCodeY = GetWallY(mPos+1);              // Check adjacent y wall (above)

            if (mCodeX && y1 < (yTop+28))           // Adjacent x wall found
                {
        		if (x1 > (xRight-28))
        		    {
        		    if (xp < (xRight-27))
            			{
            			x1 = xp;                // Use previous x position
            			HitResult = POV_SLIDEX;
            			}
        		    else
            			{
            			y1 = yp;                // Use previous y position
            			HitResult = POV_SLIDEY;
            			}
        		    }
        		}

    	    if (mCodeY && x1 > (xRight-28))     // Adjacent y wall found
        		{
        		if (y1 < (yTop+28))
        		    {
        		    if (yp > (yTop+27))
            			{
            			y1 = yp;                // Use previous y position
            			HitResult = POV_SLIDEY;
            			}
        		    else
            			{
            			x1 = xp;                // Use previous x position
            			HitResult = POV_SLIDEX;
            			}
        		    }
        		}
    	    }
    	}
    // Check region C--bottom left area
    if (y1 > (yTop+32) && !HitResult)   // We are below upper half of square
    	{
    	if (x1 < (xLeft+32))		    // and on the left half of square
    	    {
    	    mCodeX = GetWallX(mPos+GRID_WIDTH);     // Check adjacent x wall (to left)
    	    mCodeY = GetWallY(mPos-1+GRID_WIDTH);   // Check adjacent y wall (below)

    	    if (mCodeX && y1 > (yBottom-28))    // Adjacent x wall found
        		{
        		if (x1 < (xLeft+28))
        		    {
        		    if (xp > (xLeft+27))
            			{
            			x1 = xp;            // Use previous x position
            			HitResult = POV_SLIDEX;
            			}
        		    else
            			{
            			y1 = yp;            // Use previous y position
            			HitResult = POV_SLIDEY;
            			}
        		    }
        		}

	    if (mCodeY && x1 < (xLeft+28))      // Adjacent y wall found
    		{
    		if (y1 > (yBottom-28))
    		    {
    		    if (yp < (yBottom-27))
        			{
        			y1 = yp;                // Use previous y position
        			HitResult = POV_SLIDEY;
        			}
    		    else
        			{
        			x1 = xp;                // Use previous x position
        			HitResult = POV_SLIDEX;
        			}
    		    }
    		}
	    }
        // Check region D--bottom right area
    	if (x1 > (xRight-32) && !HitResult)	// Check right side of square
    	    {
    	    mCodeX = GetWallX(mPos+1+GRID_WIDTH);   // Check adjacent x wall (to right)
    	    mCodeY = GetWallY(mPos+1+GRID_WIDTH);   // Check adjacent y wall (below)

    	    if (mCodeX && y1 > (yBottom-28))        // Adjacent x wall found
        		{
        		if (x1 > (xRight-28))
        		    {
        		    if (xp < (xRight-27))
            			{
            			x1 = xp;                    // Use previous x position
            			HitResult = POV_SLIDEX;
            			}
           		    else
            			{
            			y1 = yp;                    // Use previous y position
            			HitResult = POV_SLIDEY;
            			}
        		    }
        		}

    	    if (mCodeY && x1 > (xRight-28))     // Adjacent y wall found
        		{
        		if (y1 > (yBottom-28))
        		    {
        		    if (yp < (yBottom-27))
            			{
            			y1 = yp;                        // Use previous y position
            			HitResult = POV_SLIDEY;
            			}
        		    else
            			{
            			x1 = xp;                        // Use previous x position
            			HitResult = POV_SLIDEX;
            			}
        		    }
        		}
    	    }
    	}
    }

if (AckCheckObjPosn(x1,y1,0))         // We've hit an object--not a wall
    return(POV_OBJECT);

if (HitResult == POV_SLIDEX && y1 == yp)    // We've hit an x wall and we're not sliding
    HitResult = POV_XWALL;

if (HitResult == POV_SLIDEY && x1 == xp)    // We've hit a y wall and we're not sliding
    HitResult = POV_YWALL;

aeGlobal->xPlayer = x1;     // Update player's new x,y position
aeGlobal->yPlayer = y1;

return(HitResult);
}


//************************************************************************
// Moves an object based on Angle and Amount then checks for collision
// with other objects AND the POV.
//************************************************************************
short AckMoveObjectPOV(short ObjIndex,short Angle,short Amount)
{
    short       xp,yp,x1,y1,HitResult,oNum;
    USHORT      mCodeX,mCodeY;
    short       xLeft,xRight,yTop,yBottom,mPos;
    short       MapPosn,PlayerPosn;
    NEWOBJECT   **oList;
    NEWOBJECT	*oPtr;

oList = &aeGlobal->ObjList[0];          // Reference the start of the object list
oPtr = oList[ObjIndex];                       // Set a pointer to the object being moved

if (oPtr == NULL)                               // no object is available to move; we're done
    return(0);

xp = oPtr->x;                        // Get the current x,y coordinate of the object
yp = oPtr->y;
// Calculate the new x,y, cordinates of the object (after moving)
x1 = xp + (short)((CosTable[Angle] * Amount) >> FP_SHIFT);
y1 = yp + (short)((SinTable[Angle] * Amount) >> FP_SHIFT);

xLeft = xp & 0xFFC0;                                  // Determine the coordinates of the grid square the
xRight = xLeft + GRID_SIZE - 1;                 // object is currently in
yTop = yp & 0xFFC0;
yBottom = yTop + GRID_SIZE - 1;
mPos = yTop + (xp >> 6);              // Calculate the map position of the grid square the object is in
MapPosn = (y1 & 0xFFC0) + (x1 >> 6);    // Calculate the map position of the grid square the 
                                                                  // object  is moving to

// Check to see if the object will encouner another object while moving
oNum = AckCheckObjPosn(x1,y1,ObjIndex);
if (oNum > 0)                                                            // Yes, return falg to indicate object found
    return(POV_OBJECT);

HitResult = POV_NOTHING;                    // Nothing found yet, initialize flag
if (x1 < xp)				    // Are we moving left?
    {
    if (GetWallX(mPos))			    // Wall found in current square (left edge)
    	{
    	if (x1 < xLeft || abs(x1-xLeft) < 28)	// We crossed the wall or we're too close
    	    {
    	    x1 = xp;                 // Use the previous x position
    	    HitResult = POV_SLIDEX;     // We're possibly sliding along the left x wall
    	    }
    	}
    }

if (x1 > xp)				    // Are we moving right?
    {
    if (GetWallX(mPos+1))		     // Wall found in current square (right edge)
    	{
    	if (x1 > xRight || abs(xRight-x1) < 28) // We crossed the wall or we're too close
    	    {
    	    x1 = xp;                // Use the previous x position
    	    HitResult = POV_SLIDEX;        // We're possibly sliding along the right x wall
    	    }
    	}
    }

if (y1 < yp)				    // Are we moving up?
    {
    if (GetWallY(mPos))			    // Wall found in current square (top edge)
    	{
    	if (y1 < yTop || abs(y1-yTop) < 28) // We crossed the wall or we're too close
    	    {
    	    y1 = yp;                       // Use the previous y position
    	    HitResult = POV_SLIDEY;     // We're possibly sliding along the top wall
    	    }
    	}
    }

if (y1 > yp)				     // Are we moving down?
    {
    if (GetWallY(mPos+GRID_WIDTH))	    // Wall found in current square (bottom edge)
    	{
    	if (y1 > yBottom || abs(yBottom-y1) < 28) // We crossed the wall or we're too close
    	    {
    	    y1 = yp;                     // Use the previous y position
    	    HitResult = POV_SLIDEY;       // We're sliding along the bottom wall
    	    }
    	}
    }

if (!HitResult)				// Nothing hit yet, look further
    {
    if (y1 < (yTop+32))			// We are above upper half of square
    	{
    	if (x1 < (xLeft+32))		// and on the left half of square
    	    {
    	    mCodeX = GetWallX(mPos-GRID_WIDTH);  // Check adjacent x wall (to left)
    	    mCodeY = GetWallY(mPos-1);         // Check adjacent y wall (above)

    	    if (mCodeX && y1 < (yTop+28))         // Adjacent x wall found and new y coord
        		{                                                  // is within 28 units
        		if (x1 < (xLeft+28))                     // New x coord. is within 28 units of edge
        		    {
        		    if (xp > (xLeft+27))                 // Previous x position was outside range
            			{
            			x1 = xp;                          // Use previous x position
            			HitResult = POV_SLIDEX;
            			}
        		    else
            			{
            			y1 = yp;                         // Use previous y position
            			HitResult = POV_SLIDEY;
            			}
        		    }
        		}

    	    if (mCodeY && x1 < (xLeft+28))  // Adjacent y wall found and new x coord.
        		{                                           // is within 28 units
        		if (y1 < (yTop+28))              // New y coord. is within 28 units of edge
        		    {
        		    if (yp > (yTop+27))          // Previous y position was outside range
            			{
            			y1 = yp;                   // Use previous y position
            			HitResult = POV_SLIDEY;
            			}
        		    else
            			{
            			x1 = xp;               // Use previous x position
            			HitResult = POV_SLIDEX;
            			}
        		    }
        		}
    	    }

    	if (x1 > (xRight-32) && !HitResult)	// New x is at top right 
    	    {
    	    mCodeX = GetWallX(mPos+1-GRID_WIDTH);      // Check adjacent x wall (to right)
    	    mCodeY = GetWallY(mPos+1);                              // Check adjacent y wall (above)

    	    if (mCodeX && y1 < (yTop+28))                      // Adjacent x wall found
        		{
        		if (x1 > (xRight-28))
        		    {
        		    if (xp < (xRight-27))
            			{
            			x1 = xp;         // Use previous x position
            			HitResult = POV_SLIDEX;
            			}
        		    else
            			{
            			y1 = yp;           // Use previous y position
            			HitResult = POV_SLIDEY;
            			}
        		    }
        		}

    	    if (mCodeY && x1 > (xRight-28))         // Adjacent y wall found
        		{
        		if (y1 < (yTop+28))
        		    {
        		    if (yp > (yTop+27))
            			{
            			y1 = yp;                     // Use previous y position
            			HitResult = POV_SLIDEY;
            			}
        		    else
            			{
            			x1 = xp;
            			HitResult = POV_SLIDEX;
            			}
        		    }
        		}
    	    }
    	}

    if (y1 > (yTop+32) && !HitResult)	// We are below upper half of square
    	{
    	if (x1 < (xLeft+32))		// and on the left half of square
    	    {
    	    mCodeX = GetWallX(mPos+GRID_WIDTH);
    	    mCodeY = GetWallY(mPos-1+GRID_WIDTH);

    	    if (mCodeX && y1 > (yBottom-28))
        		{
        		if (x1 < (xLeft+28))
        		    {
        		    if (xp > (xLeft+27))
            			{
            			x1 = xp;
            			HitResult = POV_SLIDEX;
            			}
        		    else
            			{
            			y1 = yp;
            			HitResult = POV_SLIDEY;
            			}
        		    }
        		}

    	    if (mCodeY && x1 < (xLeft+28))
        		{
        		if (y1 > (yBottom-28))
        		    {
        		    if (yp < (yBottom-27))
            			{
            			y1 = yp;
            			HitResult = POV_SLIDEY;
            			}
        		    else
            			{
            			x1 = xp;
            			HitResult = POV_SLIDEX;
            			}
        		    }
        		}
    	    }

    	if (x1 > (xRight-32) && !HitResult)	// on right side of square
    	    {
    	    mCodeX = GetWallX(mPos+1+GRID_WIDTH);
    	    mCodeY = GetWallY(mPos+1+GRID_WIDTH);

    	    if (mCodeX && y1 > (yBottom-28))
        		{
        		if (x1 > (xRight-28))
        		    {
        		    if (xp < (xRight-27))
            			{
            			x1 = xp;
            			HitResult = POV_SLIDEX;
            			}
        		    else
            			{
            			y1 = yp;
            			HitResult = POV_SLIDEY;
            			}
        		    }
        		}

    	    if (mCodeY && x1 > (xRight-28))
        		{
        		if (y1 > (yBottom-28))
        		    {
        		    if (yp < (yBottom-27))
            			{
            			y1 = yp;
            			HitResult = POV_SLIDEY;
            			}
        		    else
            			{
            			x1 = xp;
            			HitResult = POV_SLIDEX;
            			}
        		    }
        		}
    	    }
    	}
    }

oPtr->x = x1;           // Update the new x,y coordinates for the object
oPtr->y = y1;
oPtr->mPos = MapPosn;   // Update the grid map position for the object

PlayerPosn = (aeGlobal->yPlayer & 0xFFC0) + (aeGlobal->xPlayer >> 6);
if (MapPosn == PlayerPosn)
    return(POV_PLAYER);

return(HitResult);
}

//************************************************************************
// Checks the list of objects used in an application and sets up the current bitmap for
// each object that can be animated. Object animation is performed by displaying
// different bitmaps for an object in sequence.
//************************************************************************
void AckCheckObjectMovement(void)
{
    short       i,maxObj;
    short       dx;
    NEWOBJECT   **oList;
    NEWOBJECT	*oPtr;

maxObj = aeGlobal->MaxObjects;         // Get the number of objects used
oList = &aeGlobal->ObjList[0];             // Reference the list of objects

for (i = 1; i < maxObj; i++)                     // Loop to check each object in the list
    {
    oPtr = oList[i];                                    // Access current object in list
    if (oPtr == NULL)                              // No object here; skip
    	continue;

    if (!oPtr->Active)                                // Object is not active; skip
    	continue;

    if (!oPtr->Speed)                                 // Object has no speed setting; skip
    	continue;

    if (!(oPtr->Flags & OF_ANIMATE))   // Object is not set up for animation
    	continue;

    dx = oPtr->CurrentBm + 1;                               // Use the next bitmap
    if (dx >= oPtr->Maxbm)                                    // We're at the end of the list of bitmaps
    	{
    	if (oPtr->Flags & OF_ANIMONCE)         // Object should only be animated once
    	    {
    	    oPtr->Flags &= ~OF_ANIMATE;         // Reset flags to indicate that we're done
    	    oPtr->Flags |= OF_ANIMDONE;           // animating the object
    	    dx = oPtr->CurrentBm;                           // Keep current bitmap number
    	    }
    	else
    	    dx = 0;                                                    // Start at the beginning of the set of bitmaps
    	}
    oPtr->CurrentBm = dx;                                        // Store the next bitmap as the current one
    }
}

// **** End of Source ****


// This source file contains the internal functions needed to add objects
// to the slice structures as a view is being built.
// (c) 1995 ACK Software (Lary Myers)
#include <stdio.h>
#include <string.h>

#include "ack3d.h"      // Main ACK-3D internal and interface data structures
#include "ackeng.h"     // Internal data structures and constants
#include "ackext.h"     // Defines external (global) variables

extern  short   gWinStartX;   // Global variables to define the left and
extern  short   gWinEndX;     // right edge of the viewport
// A function pointer to refernce the actual routine used to build a wall slice
extern  void    (*WallMaskRtn)(void);

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Internal function called by FindObject(). Your programs may call this
// function if they need to calculate the angle between two points. dx and
// dy represent the deltas between the two points. (i.e. dx = x1 - x and
// dy = y1 - y)
//
// Quadrants
//  2 | 3       If the object is in quadrants 0 or 2, we need
// ---+---      to add the resulting angle to the quad value less
//  1 | 0       than the resulting angle. If the object is in
//              quadrants 1 or 3, we need to subtract the
//              resulting angle from the next higher quadrant
//              value. This is because quads 1 and 3 are negative
//              values returned from arctan, while 0 and 2 are
//              positive.
//
// The angle between the two points is determined by using the formula:
// tan (angle) = dy/dx. The look-up table LongTanTable[] is used to
// access tangent values of angles.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckGetObjectAngle(long dx,long dy)
{
    short   i,quadrant,objAngle;
    short   Beg;
    long    avalue;

if (dx == 0 || dy == 0)    // Test to see if angle is 0, 90, 180, or 270
    {
    if (dx == 0)     // Distance is directly up or down
        {
        if (dy < 0)  // Distance is straight up
            return(INT_ANGLE_270);
        return(INT_ANGLE_90);
        }
    if (dx < 0)                  // dy = 0; distance is directly left or right
        return(INT_ANGLE_180);
    return(0);
    }

// Need to determine which quadrant is involved
quadrant = 0;          // Set to quad 0 as default
if (dx < 0 && dy > 0)  // We're in quad 1
    quadrant = INT_ANGLE_180;
else
    {
    if (dx < 0 && dy < 0)           // We're in quad 2
        quadrant = INT_ANGLE_270;
    else
        {
        if (dx > 0 && dy < 0)       // We're in quad 3
            quadrant = INT_ANGLE_360;
        }
    }

// Get the absolute values to use for our ratio
if (dy < 0)
    dy = -dy;
if (dx < 0)
    dx = -dx;

//=======================================================================
// Next we need to convert dy into the same fixed point representation
// as used in our tangent table. Then, we divide dy by dx (rise/run)
// to get the ratio so we can determine the tangent of the angle between
// the two pints. We use the ratio to search the tangent table
// and the index that is returned tells us what the actual angle is.
// We only need to check angles from 0 to 90 degrees. Later, the angle
// will be adjusted to take into account which quadrant we are in.
//=======================================================================
dy = dy << FP_SHIFT;    // Make the dividend the same fixed point as the table
avalue = dy / dx;       // Get our ratio to search for
                        // This ratio tells us the tangent of the angle
if (LongTanTable[INT_ANGLE_90-1] <= avalue)   // Angle is 89 degrees
    return(INT_ANGLE_90-1);
objAngle = 0;           // Initialize angle to 0

//=============================================================================
// Now we use a binary lookup trick to speed up the search. This invloves
// a test to see if the angle is between o and 45 degrees or between 45 and
// 90 degrees. Then, we search the list sequentially to find the first value
// higher than our ratio.
//=============================================================================
Beg = 0;    // Assume midpoint between 0 and 45 degrees
if (LongTanTable[INT_ANGLE_45] < avalue)
    {
    if (LongTanTable[360] < avalue)
        Beg = 360;                    // Use angle of 360
    else
        Beg = INT_ANGLE_45;           // Midpoint between 45 and 90 degrees
    }

// Loop to check the tan table and find the correct angle
for (i = Beg; i < INT_ANGLE_90; i++)
    {
    if (LongTanTable[i] > avalue)  // We've passed by the angle
        {
        objAngle = i - 1;    // Get the correct angle
        break;
        }
    }

if (objAngle < 0)    // Adjust for angle=0
    objAngle = 0;

//============================================================================
// Now we adjust the resulting angle based on the quadrant. If we are in
// quad 0 we do nothing. If we're in quads 1 and 3 we subtract the angle from
// the next higher quad angle. If we're in quad 2 we add the angle to the next
// lower quad angle to get the actual angle (0-1800) between the points.
//============================================================================
if (quadrant)
    {
    if (quadrant != INT_ANGLE_270)
        objAngle = quadrant - objAngle;
    else
        objAngle += INT_ANGLE_180;
    }

// Returns the angle between the two points. This value is mainly used for
// determining the angle between the POV and an object, but it could
// be used for generic purposes as well.
return(objAngle);
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Internal function that returns the square root of a long value.
// This function is called by Find)bject().
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short long_sqrt(long v)
{
    short     i;
    unsigned  short result,tmp;
    unsigned  long low,high;

if (v <= 1L) return((unsigned)v);   // Value is less than 1; return value
low = v;                            // Initialize starting variables
high = 0L;
result = 0;

for (i = 0; i < 16; i++)
    {
    result += result;
    high = (high << 2) | ((low >>30) & 0x3);
    low <<= 2;                                // Shift left by 2
    tmp = result + result + 1;
    if (high >= tmp)
        {
        result++;
        high -= tmp;
        }
    }
return(result);
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Internal function called by AckBuildView() which checks the list of
// objects found during the ray cast process and places the object slices
// into the wall slices.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
void FindObject(void)
{
    short   i,j,StartX,EndX;
    short   oCount;
    short   minAngle,maxAngle,objAngle;
    short   objColumn;
    USHORT  distance;
    long    dx,dy;
    short   count,SaveCenter;
    short   ObjNum,oQuad,pQuad,numsides,afact;
    short   NewX,NewY,LightFlag;
    short   MaxOpp,Column,ColBeg,ColEnd;
    short   wt,ObjIndex;
    short   vidwt,vidht,hoff;
    short   MaxObjs;
    short   SliceLen;
    USHORT  BmpColumn;
    long    xp,yp;
    short   wht;
    UCHAR   *wall,*ScreenBuffer;
    UCHAR   *pTable;
    UCHAR   **omaps;
    SLICE   *sa,*sa2,*saNext;
    UCHAR   *bmpFlags;
    NEWOBJECT **oList;
    NEWOBJECT *oPtr;

if (FoundObjectCount)       // Make sure objects were found during ray casting
    {
    oList = &aeGlobal->ObjList[0];  // Get pointer to the array of objects
    StartX = gWinStartX;            // Starting column of view
    EndX = gWinEndX;                // Ending column of view

    minAngle = PlayerAngle - (INT_ANGLE_32 + 10);   // Starting angle of view
    if (minAngle < 0)               // Check for wrap-around at angle 0
        minAngle += INT_ANGLE_360;
    maxAngle = PlayerAngle + (INT_ANGLE_32 + 10);   // Ending angle of view
    if (maxAngle >= INT_ANGLE_360)  // Check for wrap-around at angle 360
        maxAngle -= INT_ANGLE_360;

    TotalObjects = 0;              // Stores nmber of objects in view
    SliceLen = sizeof(SLICE) - 9;  // Amount of slice we'll move later

// Loop and process each object in the view. This invloves setting up
// a few arrays to store the object number, the distance from the player
// to the object, the viewing angle, and the view column where the object
// will be displayed.

    for (oCount = 0; oCount < FoundObjectCount; oCount++)
        {
        i = ObjectsSeen[oCount];    // Get index to possible object
        oPtr = oList[i];            // Pointer to object structure
        if (oPtr == NULL)           // Make sure it's a valid object
            continue;
        if (!oPtr->Active)          // Make sure it's visible
            continue;

        dx = oPtr->x - xPglobal;    // Get the delta x,y between the
        dy = oPtr->y - yPglobal;    // object and the player

        // Calculate the angle the object is relative to the player
        if ((objAngle = AckGetObjectAngle(dx,dy)) < 0)
            continue;        // Negative angle means it can't be seen

        // Here we determine if the POV is looking toward the right or
        // the left and the actual column of the view (from 0 to view width)
        // where the object would be seen.
        if (minAngle > maxAngle)    // If looking towards the right
            {
            if (objAngle >= minAngle)  // Cal. view column of object
                objColumn = objAngle - minAngle;
            else
                objColumn = (objAngle+INT_ANGLE_360) - minAngle;
            }
        else
            {
            objColumn = objAngle - minAngle; // Calc. view column of object
            }

        // Get the distance to the object
        distance = long_sqrt((dx * dx) + (dy * dy));
        // No need to check further if it's too far away
        if (distance >= MaxDistance)
            continue;

        // Place the objects in the correct order so further ones are behind
        j = TotalObjects;    // Current number of objects we've found
        if (j)
            {
            // Sort the objects found by distance so that further ones
            // are drawn BEFORE closer ones.
            for (count = 0; count < TotalObjects; count++)
                {
                if (distance <= ObjRelDist[count])
                    {
                    for (j = TotalObjects; j > count; j--)
                        {
                        ObjRelDist[j] = ObjRelDist[j-1];
                        ObjNumber[j]  = ObjNumber[j-1];
                        ObjColumn[j]  = ObjColumn[j-1];
                        ObjAngle[j]   = ObjAngle[j-1];
                        }
                    j = count;
                    count = TotalObjects;
                    }
                }
            }

        // Hold onto relavant data for the object found
        ObjNumber[j]  = i;             // Store the object number
        ObjRelDist[j] = distance;      // Store the distance to the object
        ObjColumn[j]  = objColumn;     // Store view column where object resides
        ObjAngle[j] = objAngle;        // Store the viewing angle
        TotalObjects++;                // Bump the count of objects in view
        ObjRelDist[TotalObjects] = 0L; // Set to relative dist. in next object to 0
        }

    // Didn't find any objects on the above pass, so we're done
    if (!TotalObjects)
        return;

    omaps = &aeGlobal->oMaps[0]; // Bitmaps used for objects
    pQuad = PlayerAngle / INT_ANGLE_45; // Quadrant POV is facing

// Check each object in the list to be displayed and get the object's
// bitmap. Also, calulate the width and height of the object.
// This loop also checks to see if an object has multiple sides
// and it determines which bitmap should be used to display the object.
    for (i = 0; i < TotalObjects; i++)
        {
        ObjIndex = ObjNumber[i];    // Actual object found
        oPtr = oList[ObjIndex];     // Pointer to object structure
        if (oPtr == NULL)           // Again check for a null object
            continue;
        // Current bitmap for the object (this number can change if the
        // object is animated)
        ObjNum = oPtr->CurrentBitmaps[oPtr->CurrentBm];
        distance = ObjRelDist[i];   // Get relative distance to object
        // Make sure distance is within a reasonable entry in our
        // pre-calculated table
        if (distance >= (MAX_DISTANCE - 10))
            distance = MAX_DISTANCE-11;

        // Get the width of the object
        wt = DistanceTable[distance];   // Adjust the width using the distance
        // Keep the width of the object reasonable
        if (wt > 300)           // The object is too wide
            continue;           // Skip over
        if (wt < 6) wt = 6;    // Adjust if too small

        // Get the scale factor which was pre-calculated based on
        // distance in AckInitialize() function
        yp = AdjustTable[distance];
        xp = 0;        // First col of the object to display

        NewX = ObjColumn[i]; // View column where object resides

        // Check if object has multiple sides. If so we need to determine
        // the correct bitmap to display based on the angle between the
        // POV and the object. We'll perform a trick here by breaking down
        // the problem into quadrants and then use the quadrant to determine
        // which side we're facing. The object itself is facing a certain
        // angle (stored in the Dir field of the object structure) so this
        // needs to be taken into account as well.
        if (oPtr->Flags & OF_MULTIVIEW)
            {
            afact = oPtr->aFactor;   // Get the angles per side of object
            numsides = oPtr->Sides;  // Get total sides for this object
            pQuad = ObjAngle[i] / afact;  // Get the quadrant from POV to object
            oQuad = oPtr->Dir / afact;    // Get the quadrant it wants to face

            // The difference between the POV-Object angle and the angle the
            // object is facing determines the actual side of the object that
            // can currently be seen
            j = (pQuad - oQuad) + (numsides >> 1);

            // Check for wrap-around and keep within range
            if (j >= numsides)
                j -= numsides;
            // Check wrap-around in both directions
            if (j < 0)
                j += numsides;

            // Calculate which bitmap set we should use (each side could
            // have multiple bitmaps for animation)
            j *= oPtr->BitmapsPerView;
            j += oPtr->CurrentBm;
            // Get the actual bitmap for this side and animation
            ObjNum = oPtr->CurrentBitmaps[j];
            }

        // Done processing multiple sides. Next, find the
        // ending column based on the starting column plus the scaled
        // width of the object.
        ColEnd = NewX + wt;
        // Finally get the pointer to the actual bitmap
        wall = omaps[ObjNum];
        // Pick up the transparent flags at end of bitmap
        bmpFlags = &wall[BITMAP_SIZE];
        j = distance;

        // Loop from starting column to ending column and fold in the
        // object into the appropriate slice structure.
        for (Column = NewX - wt; Column < ColEnd; Column++)
            {
            // Make sure column is within view width
            if (Column >= StartX && Column <= EndX)
                {
                // Scale bitmap column down from fixed point
                BmpColumn = xp >> FP_SHIFT;
                if (bmpFlags[BmpColumn])    // If transparent column
                    goto keepgoing;         // Ouch! But it works

                j = distance;
                // Account for fisheye effect
                dy = ViewCosTable[Column] >> 2;
                dx = distance * dy;
                // Now we strip off somemore decimal points and check round-up
                dy = dx >> 12;
                if (dx - (dy << 12) >= 4096)
                    dy++;
                if (dy > 32L)
                    j = dy;

                // Now we pick up the slice for this column and insert sort
                // the object slice based on the distance. This allows objects
                // to appear between walls at various distances, behind
                // transparent walls, and so on.
                sa = &Slice[Column];    // Access the corresponding slice
                if (sa->Active)         // Multiple slices for this column?
                    {
                    while (sa != NULL)
                        {
                        if (j <= sa->Distance)
                            {
                            sa2 = sa;
                            while (sa2->Next != NULL) // Go to end of slices
                                sa2 = sa2->Next;
                            saNext = sa2->Prev;
                            while (sa2 != sa) // Move slice down to create
                                {             // a space for new slice
                                memcpy(sa2,saNext,sizeof(SLICE)-9);
                                sa2->Active = saNext->Active;
                                sa2 = sa2->Prev;
                                saNext = saNext->Prev;
                                }
                            // Fill in the slice structure with the
                            // info about the object
                            sa->Distance = distance;
                            sa->bNumber  = ObjNum;
                            sa->bColumn  = BmpColumn;
                            sa->bMap     = omaps;
                            sa->Active   = 1;
                            sa->Type     = ST_OBJECT;
                            sa->Fnc      = WallMaskRtn;
                            break;
                            }
                        if (!sa->Active)
                            break;
                        sa = sa->Next;
                        }
                    }
                else // Only one slice is used for this column (typical)
                    {
                    if (j <= sa->Distance) // Only put it in if object is
                        {                  // closer than current slice
                        sa->Active = 1;
                        saNext = sa->Next;
                        memcpy(saNext,sa,sizeof(SLICE)-9);
                        sa->Distance = distance;
                        sa->bColumn  = BmpColumn;
                        sa->bNumber  = ObjNum;
                        sa->bMap     = omaps;
                        sa->Type     = ST_OBJECT;
                        sa->Fnc      = WallMaskRtn;
                        saNext->Active = 0;
                        }
                    }
                }
    keepgoing:
            xp += yp;    // Advance the next column to display (scaling)
            }
        }
    }
}
// **** End of Source ****

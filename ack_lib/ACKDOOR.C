// This source file contains the functions needed to process doors.
// (c) 1995 ACK Software (Lary Myers)
#include <stdlib.h>
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

extern DOORS *gDoor;

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Check all the active doors to see what current state they are in. If a
// door's column offset is non-zero, it is either opening or closing,
// so the speed of the door is added in and the door is checked to
// see if it is fully opened or fully closed.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
void CheckDoors (void)
{
    short   i, MapPosn, mPos, mPos1;
    short   mx, my, DeltaGrid;
    short   xPlayer, yPlayer;
    DOORS   *dPtr;

xPlayer = xPglobal;
yPlayer = yPglobal;
dPtr = &gDoor[0];

for (i = 0; i < MAX_DOORS; i++)
    {
    if (dPtr->ColOffset)    // Door is not closed
        {  // Add speed to door column offset
        dPtr->ColOffset += dPtr->Speed;
        mPos = dPtr->mPos;       // Get grid position of door
        mPos1 = dPtr->mPos1;

        // Door is closing and is visible. Put old codes back to make non-passable
        if (dPtr->Speed < 1 && dPtr->ColOffset < 65)
            {  // Get corner of grid
            MapPosn = (yPlayer & 0xFFC0) + (xPlayer >> BITMAP_SHIFT);
            // Is player in same grid square that the door is in?
            if (MapPosn == mPos || MapPosn == mPos1)
                {
                dPtr->ColOffset -= dPtr->Speed;  // Reduce column offset
                continue;
                }
            // Is the door along an x wall
            if (dPtr->Type == DOOR_XCODE)
                {  // Store bitmap ids for doors
                xGridGlobal[mPos] = dPtr->mCode;
                xGridGlobal[mPos1] = dPtr->mCode1;
                }
            else
                {  // Door is along a y wall
                yGridGlobal[mPos] = dPtr->mCode;
                yGridGlobal[mPos1] = dPtr->mCode1;
                }

            // Door is close enough to fully closed to get rid
            // of the door from the array
            if (dPtr->ColOffset < 3)
                {
                dPtr->ColOffset = 0;  // Close door
                dPtr->mPos = -1;
                dPtr->mPos1 = -1;
                dPtr->Flags = 0;
                }
            }
          // Door is fully opened--time to start closing it
        if (dPtr->ColOffset > 0xA0)
            {
            dPtr->Speed = -dPtr->Speed;   // Start to reduce speed
            dPtr->Flags &= ~DOOR_OPENING; // Set flag to indicate door closing
            dPtr->Flags |= DOOR_CLOSING;
            }
        }
    dPtr++;    // Advance to check next door
    }

//=============================================================================
// Now check for any action occuring in a secret door. This is currently
// setup to handle only one door at a time in the X and Y directions, but
// it should be fairly straightforward to use a list of doors, similiar
// to normal doors, to handle more than one.
//=============================================================================
if (xSecretColumn)
    {
    if (xSecretColumn > 0)    // See if the door is to the right of us
        {
        mPos = xSecretmPos1;
        DeltaGrid = -1;
        xSecretColumn += aeGlobal->DoorSpeed;
        }
    else
        {
        mPos = xSecretmPos;
        DeltaGrid = 0;
        xSecretColumn -= aeGlobal->DoorSpeed;
        }

    my = mPos & 0xFFC0;
    mx = (mPos - my) << 6;

    if (abs (xSecretColumn) > BITMAP_WIDTH)   // Beyond one grid square
        {
        mx += xSecretColumn;
        my = my + (mx >> BITMAP_SHIFT);
        if (xGridGlobal[my])  // No further, an obstruction
            {
            xGridGlobal[xSecretmPos] = 0;
            xGridGlobal[xSecretmPos1] = 0;
            my += DeltaGrid;
            xGridGlobal[my] = aeGlobal->NonSecretCode;
            xGridGlobal[my + 1] = aeGlobal->NonSecretCode;
            yGridGlobal[my] = aeGlobal->NonSecretCode;
            yGridGlobal[my + GRID_WIDTH] = aeGlobal->NonSecretCode;
            xSecretColumn = 0;
            }
        else
            {
            if (my != mPos)
                {
                xGridGlobal[xSecretmPos] = 0;
                xGridGlobal[xSecretmPos1] = 0;
                if (xSecretColumn > 0)
                    {
                    xSecretColumn -= (BITMAP_WIDTH - 1);
                    xGridGlobal[my] = DOOR_TYPE_SECRET + aeGlobal->NonSecretCode;
                    xSecretmPos1 = my;
                    my--;
                    xGridGlobal[my] = DOOR_TYPE_SECRET + aeGlobal->NonSecretCode;
                    xSecretmPos = my;
                    }
                else
                    {
                    xSecretColumn += (BITMAP_WIDTH - 1);
                    xGridGlobal[my] = DOOR_TYPE_SECRET + aeGlobal->NonSecretCode;
                    xGridGlobal[my + 1] = DOOR_TYPE_SECRET + aeGlobal->NonSecretCode;
                    xSecretmPos = my;
                    xSecretmPos = my + 1;
                    }
                }
            }                   // End if (xGrid[my]) ... else ...
        }                       // End if (abs(xSecretColumn) > GRID_SIZE)
    }                           // End if (xSecretColumn)

//=============================================================================
// Perform same process on a secret door that may be moving in the Y
// direction. The same door can move either way, depending on which
// angle the player struck it at.
//=============================================================================
if (ySecretColumn)
    {
    if (ySecretColumn > 0)
        {
        mPos = ySecretmPos1;
        DeltaGrid = -GRID_WIDTH;
        ySecretColumn += aeGlobal->DoorSpeed;
        }
    else
        {
        mPos = ySecretmPos;
        DeltaGrid = 0;
        ySecretColumn -= aeGlobal->DoorSpeed;
        }

    my = mPos & 0xFFC0;
    mx = (mPos - my) << 6;

    if (abs (ySecretColumn) > BITMAP_WIDTH)
        {
        my += ySecretColumn;
        my = (my & 0xFFC0) + (mx >> 6);
        if (yGridGlobal[my])
            {
            yGridGlobal[ySecretmPos] = 0;
            yGridGlobal[ySecretmPos1] = 0;
            my += DeltaGrid;
            xGridGlobal[my] = aeGlobal->NonSecretCode;
            xGridGlobal[my + 1] = aeGlobal->NonSecretCode;
            yGridGlobal[my] = aeGlobal->NonSecretCode;
            yGridGlobal[my + GRID_WIDTH] = aeGlobal->NonSecretCode;
            ySecretColumn = 0;
            }
        else
            {
            if (my != mPos)
                {
                yGridGlobal[ySecretmPos] = 0;
                yGridGlobal[ySecretmPos1] = 0;
                if (ySecretColumn > 0)
                    {
                    ySecretColumn -= (BITMAP_WIDTH - 1);
                    yGridGlobal[my] = DOOR_TYPE_SECRET + aeGlobal->NonSecretCode;
                    ySecretmPos1 = my;
                    my -= GRID_WIDTH;
                    yGridGlobal[my] = DOOR_TYPE_SECRET + aeGlobal->NonSecretCode;
                    ySecretmPos = my;
                    }
                else
                    {
                    ySecretColumn += (BITMAP_WIDTH - 1);
                    yGridGlobal[my] = DOOR_TYPE_SECRET + aeGlobal->NonSecretCode;
                    yGridGlobal[my + GRID_WIDTH] = DOOR_TYPE_SECRET + aeGlobal->NonSecretCode;
                    ySecretmPos = my;
                    ySecretmPos = my + GRID_WIDTH;
                    }
                }
            }
        }
    }

}


//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Internal function called by FindDoorSlot(). This function
// locates a door from its map coordinate and return the index.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short FindDoor (short MapPosn)
{
    short   index;

for (index = 0; index < MAX_DOORS; index++)
    {
    if (MapPosn == gDoor[index].mPos ||
        MapPosn == gDoor[index].mPos1)
        {
        return (index);
        }
    }
return (-1);
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Internal routine called by AckCheckDoorOpen() This function
// finds an empty slot for a door. If the door already occupies a slot
// and it is in a non-closed state, an error is returned.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short FindDoorSlot (short MapPosn)
{
    short   index;

index = FindDoor (MapPosn);
if (index >= 0 && gDoor[index].ColOffset)
    return (-1);

for (index = 0; index < MAX_DOORS; index++)
    {
    if (gDoor[index].mPos == -1)
        return (index);
    }
return (-1);
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Checks directly in front of the POV to see if a door is there. If so,
// and the door is not locked, the door is set to begin opening.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckCheckDoorOpen (short xPlayer, short yPlayer, short PlayerAngle)
{
    short   i,j,DoorCode;

DoorCode = POV_NODOOR;
// Check to see if player is close to a wall
i = AckCheckHit (xPlayer, yPlayer, PlayerAngle);
// A secret door is found along an x wall
if (i == 1 && xGridGlobal[xMapPosn] & DOOR_TYPE_SECRET)
    {
    if (xSecretColumn == 0) // Player is on the right side of door
        {
        DoorCode = POV_XSECRETDOOR;
        // Secret door is locked; can't open
        if (xGridGlobal[xMapPosn] & DOOR_LOCKED)
            return (DoorCode | POV_DOORLOCKED);
        // Get grid map position that corresponds with the x wall
        // position of where the door starts
        xSecretmPos = xMapPosn;
        if (iLastX > xPlayer)   // Door is to the right of player
            {
            xSecretmPos1 = xMapPosn + 1;
            LastMapPosn = xMapPosn;
            xSecretColumn = 1;   // Set to indicate player is on right side
            yGridGlobal[xMapPosn] = yGridGlobal[xMapPosn - GRID_WIDTH];
            }
        else
            { // Door is to the left of the player
            LastMapPosn = xSecretmPos1 = xMapPosn - 1;
            xSecretColumn = -1;
            yGridGlobal[xSecretmPos1] = yGridGlobal[xSecretmPos1 - GRID_WIDTH];
            }
        }
    }

// A secret door is found along a y wall
if (i == 2 && yGridGlobal[yMapPosn] & DOOR_TYPE_SECRET)
    {
    if (ySecretColumn == 0)
        {
        DoorCode = POV_YSECRETDOOR;

        if (yGridGlobal[yMapPosn] & DOOR_LOCKED)
            return (DoorCode | POV_DOORLOCKED);

        ySecretmPos = yMapPosn;
        if (iLastY > yPlayer)
            {
            LastMapPosn = yMapPosn;
            ySecretmPos1 = yMapPosn + GRID_WIDTH;
            xGridGlobal[yMapPosn] = xGridGlobal[yMapPosn - 1];
            ySecretColumn = 1;
            }
        else
            {
            LastMapPosn = ySecretmPos1 = yMapPosn - GRID_WIDTH;
            xGridGlobal[ySecretmPos1] = xGridGlobal[ySecretmPos1 - 1];
            ySecretColumn = -1;
            }
        }
    }

// A sliding or split door is found along an x wall
if (i == 1 && (xGridGlobal[xMapPosn] & (DOOR_TYPE_SLIDE + DOOR_TYPE_SPLIT)))
    {
    j = FindDoorSlot (xMapPosn);
    if (j >= 0)
        {
        DoorCode = POV_XDOOR;

        LastMapPosn = gDoor[j].mPos = xMapPosn;
        if ((short) iLastX > xPlayer)
            i = xMapPosn + 1;
        else
            LastMapPosn = i = xMapPosn - 1;
        if (xGridGlobal[xMapPosn] & DOOR_LOCKED)
            {
            gDoor[j].mPos = -1;
            return (DoorCode | POV_DOORLOCKED);
            }
        // Update door data structure
        gDoor[j].mCode = xGridGlobal[xMapPosn];
        gDoor[j].mCode1 = xGridGlobal[i];
        gDoor[j].mPos1 = i;
        gDoor[j].ColOffset = 1;
        gDoor[j].Speed = aeGlobal->DoorSpeed;
        gDoor[j].Type = DOOR_XCODE;
        gDoor[j].Flags = DOOR_OPENING;
        }
    }

// A sliding or split door is found along a y wall
if (i == 2 && (yGridGlobal[yMapPosn] & (DOOR_TYPE_SLIDE + DOOR_TYPE_SPLIT)))
    {
    j = FindDoorSlot (yMapPosn);
    if (j >= 0)
        {
        DoorCode = POV_YDOOR;
        LastMapPosn = gDoor[j].mPos = yMapPosn;
        if ((short) iLastY > yPlayer)
            i = yMapPosn + GRID_WIDTH;
        else
            LastMapPosn = i = yMapPosn - GRID_WIDTH;

        if (yGridGlobal[yMapPosn] & DOOR_LOCKED)
            {
            gDoor[j].mPos = -1;
            return (DoorCode | POV_DOORLOCKED);
            }

        gDoor[j].mCode = yGridGlobal[yMapPosn];
        gDoor[j].mCode1 = yGridGlobal[i];
        gDoor[j].mPos1 = i;
        gDoor[j].ColOffset = 1;
        gDoor[j].Speed = aeGlobal->DoorSpeed;
        gDoor[j].Type = DOOR_YCODE;
        gDoor[j].Flags = DOOR_OPENING;
        }
    }
return (DoorCode);
}
// **** End of Source ****


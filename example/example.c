/*
 * Example ACK-3D Application
 * --------------------------
 *
 * This is just a bare-bones simple "walkaround" demo application that lets
 * you walk through a map.
 *
 * The ACK-3D demo programs, FDEMO, MALL, and also the Windows demo game
 * "Station Escape", all include a fair bit more complexity and a bunch of
 * hard-coded magic numbers, and other messyness.
 *
 * This example program is intended to be _super_ simple to get into.
 * Unfortunately, ACK-3D requires a non-trivial amount of game resource file
 * loading to be performed to get even a simple level up and running, but
 * those details are mostly re-usable and is kept in a separate source file.
 * This source file just contains the basics to initialize the engine and
 * move a player around a game world.
 */
#include <stdio.h>
#include <conio.h>
#include <malloc.h>
#include <string.h>

#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"
#include "assets.h"

#define KEY_RIGHT     77
#define KEY_UP        72
#define KEY_LEFT      75
#define KEY_DOWN      80
#define KEY_SPACE     57
#define KEY_ESCAPE    1

#define TURN_SPEED       5
#define TURN_SPEED_DECAY 2
#define MOVE_SPEED       4
#define MOVE_SPEED_DECAY 1
#define BASE_MOVE_AMOUNT 5
#define BASE_TURN_AMOUNT INT_ANGLE_2

// ACK3D bitmap loading routines, such as AckReadiff(), will automatically
// populate (and re-populate) this array with palette found in that image
// file. still, this (or any other palette color data) must be manually set
// with AckSetPalette()
extern unsigned char colordat[];

// These are the ranges used for distance shading. Will need to be modified
// for the new color palette used.
// For ACK-3D's distance-based light shading to look best, your palette really
// needs to be designed with this in mind (and your bitmaps too). Many of the
// ACK-3D demo assets don't look so good with it.
ColorRange ranges[64] = {
    1,15,
    16,16,
    32,16,
    48,16,
    64,16,
    80,16,
    96,8,
    104,8,
    112,8,
    120,8,
    136,8,
    144,8,
    152,8,
    160,8,
    168,8,
    176,8,
    184,8,
    192,16,
    208,16,
    224,16,
    240,15,
    0,0
};

// this is the main ACK3D engine structure. holds the map, bitmaps, objects,
// and also the current state of the game world (as far as ACK3D is concerned)
ACKENG *ae;

UCHAR *uiFrame = NULL;
UCHAR *palette = NULL;

// this game loop made semi-overcomplicated by rudimentary turn/movement
// acceleration logic
void GameLoop(void) {
    int newAngle;
    int moveSpeed = 0;
    int moveAmount = 0;
    int turnSpeed = 0;
    int turnAmount = 0;

    AckBuildView();
    AckDisplayScreen();

    while (1) {
        moveAmount = BASE_MOVE_AMOUNT + moveSpeed;
        turnAmount = BASE_TURN_AMOUNT + turnSpeed;

        if (AckKeys[KEY_ESCAPE]) {
            break;
        }

        if (AckKeys[KEY_LEFT]) {
            turnSpeed += TURN_SPEED;
            ae->PlayerAngle -= turnAmount;
            if (ae->PlayerAngle < 0)
                ae->PlayerAngle += INT_ANGLE_360;
        }

        if (AckKeys[KEY_RIGHT]) {
            turnSpeed += TURN_SPEED;
            ae->PlayerAngle += turnAmount;
            if (ae->PlayerAngle >= INT_ANGLE_360)
                ae->PlayerAngle -= INT_ANGLE_360;
        }

        if (AckKeys[KEY_UP]) {
            moveSpeed += MOVE_SPEED;

            // this function handles collision detection for you reasonably
            // well actually. pretty handy
            AckMovePOV(ae->PlayerAngle, moveAmount);
        }

        if (AckKeys[KEY_DOWN]) {
            moveSpeed += MOVE_SPEED;
            newAngle = ae->PlayerAngle + INT_ANGLE_180;
            if (newAngle >= INT_ANGLE_360)
                newAngle -= INT_ANGLE_360;
            AckMovePOV(newAngle, moveAmount);
        }

        if (AckKeys[KEY_SPACE]) {
            AckKeys[KEY_SPACE] = 0;
            AckCheckDoorOpen(ae->xPlayer, ae->yPlayer, ae->PlayerAngle);
        }

        // updates object animation and various object state things
        AckCheckObjectMovement();

        // renders the ack3d world to an offscreen buffer. also updates some
        // ack3d state (such as door opening/closing)
        AckBuildView();

        // copies offscreen buffer to vga framebuffer
        AckDisplayScreen();

        moveSpeed -= MOVE_SPEED_DECAY;
        if (moveSpeed < 0)
            moveSpeed = 0;
        if (moveSpeed > 16)
            moveSpeed = 16;

        turnSpeed -= TURN_SPEED_DECAY;
        if (turnSpeed < 0)
            turnSpeed = 0;
        if (turnSpeed > 16)
            turnSpeed = 16;

    }
}


int main(int argc, char *argv[]) {
    int result;

    printf("Example ACK-3D application\n");

    ae = (ACKENG*)malloc(sizeof(ACKENG));
    memset(ae, 0, sizeof(ACKENG));

    // ack3d renderer viewport size. on 486 and lower-spec machines, you
    // probably do not want this to be at full 320x200 for performance reasons
    // even reducing 320x200 by 20% makes a nice, noticeable difference!
    ae->WinStartX = 16;
    ae->WinStartY = 12;
    ae->WinEndX = 303;
    ae->WinEndY = 158;

    printf("Initializing ACK-3D\n");

    // main ack3d initialization. AckInitialize builds up a bunch of lookup
    // tables and other global engine things like that. kit.ovl contains a
    // bunch of pre-calculate trig lookup tables. if it was not opened before
    // AckInitialize is called, then AckInitialize tries to find a file called
    // trig.dat in the current directory.

    result = AckOpenResource("kit.ovl");
    if (result) {
        printf("Error opening kit.ovl\n");
        return 1;
    }

    result = AckInitialize(ae);
    if (result) {
        printf("AckInitialize failed\n");
        return 1;
    }
    AckCloseResource();

    // the big whammy. pics.dtf has basically all your game assets and takes
    // the longest. ProcessInfoFile walks through a ".inf" file that should
    // have been packed in pics.dtf at the first slot. that file (which is
    // just a text file) has directives to load everything else (map, bitmaps,
    // object definitions, etc)

    printf("Processing pics.dtf\n");
    result = AckOpenResource("pics.dtf");
    if (result) {
        printf("Error opening pics.dtf\n");
        return 1;
    }

    result = ProcessInfoFile(ae);
    if (result) {
        printf("Error while processing pics.dtf\n");
        printf("Result code %d\nLine number: %d\nLine: %s\n", result, LineNumber, LineBuffer);
        return 1;
    }

    printf("Processing scrolling backdrop\n");
    // processes any (optional) scrolling backdrop that was loaded so it is
    // ready to be rendered
    LoadBackDrop(ae);

    // if the resource file specified a screen/ui bitmap, then load it
    if (ResScreenBack) {
        uiFrame = LoadBitmap(ae->bmLoadType, (char*)ResScreenBack);
    }

    // if the resource file specified a palette, then load it
    if (PalResource) {
        palette = LoadPalette(PalResource);
    }

    // we won't be loading anything else from the resource file now, so we
    // can close it
    AckCloseResource();

    // various other flags that can be set too.
    // many of these can be set by the .INF file found in pics.dtf, so
    // setting any of the ACKENG properties here would override the .INF
    // settings.
    ae->xPlayer = 32 *GRID_SIZE+(GRID_SIZE/2);  // x = middle of grid 32
    ae->yPlayer = 32 *GRID_SIZE+(GRID_SIZE/2);  // y = middle of grid 32
    ae->PlayerAngle = 0;

    ae->LightFlag = SHADING_OFF;
    ae->DoorSpeed = 3;

    // Set palette for shading if needed
    if (ae->LightFlag) {
        AckSetupPalRanges(ae, ranges);
    }

    printf("Finished initializing\n");

    // the main ack3d rendering functions all assume that the various fields
    // in ACKENG have been copied into a whole bunch of global variables. this
    // architecture was used for performance reasons.
    // ACKENG pointer fields are simply copied, so modifying the actual data
    // should be fine during runtime (if you were so inclined) without needing
    // to call this function again.
    // other flags/number fields from ACKENG are copied, so if you needed to
    // change this type of thing during runtime, you would need to call this
    // function again for the change to take effect!
    AckRegisterStructure(ae);

    AckSetupKeyboard();
    AckSetVGAmode();

    if (palette) {
        // we loaded an explicitly specified palette from the resource file,
        // so we'll set that one
        AckSetPalette(palette);
    } else {
        // default to whatever the palette was from the last loaded bitmap
        AckSetPalette(colordat);
    }

    // draw the full screen/ui frame. ACK-3D won't overwrite this because
    // our WinStart/WinEnd was set to only draw to the viewport box that our
    // screen/ui frame bitmap should have (so we only need to render this
    // bitmap once)
    if (uiFrame) {
        memcpy((void*)0xA0000, uiFrame+4, 64000);
    }

    GameLoop();

    // anything allocated with AckMalloc can only be free'd with AckFree!
    if (uiFrame)
        AckFree(uiFrame);
    if (palette)
        AckFree(palette);

    AckWrapUp(ae);
    AckSetTextMode();

    if (kbhit())
        getch();

    return 0;
}


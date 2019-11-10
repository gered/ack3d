/*
 * ACK-3D Resource File Processing
 * -------------------------------
 *
 * This source file is basically a copy of the ACKINFO.CPP file from the
 * Windows example project discussed in chapter 14 of the book.
 *
 * There are some slight modifications that I've made though:
 * - A global ACKENG structure is not assumed. You must pass one in to
 *   ProcessInfoFile()
 * - Functions for processing a scrolling backdrop have been copied from the
 *   FDEMO/MALL demo projects and are included here since they are useful.
 * - A function for loading a palette is included.
 */

#include <stdio.h>
#include <io.h>
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

// Globals
int LineNumber;
char LineBuffer[200];
short LastObjectIndex;
int MapResource;
int PalResource;
int ResScreenBack;
int ResScrollBack;

// Creates the correct arrays for displaying a rotating background
void ProcessBackDrop(UCHAR *bPtr) {
    int i, j, pos;
    UCHAR *aPtr;

    for (i = 0; i < 320; i++) {
        pos = i + 4;
        aPtr = BackArray[i];
        for (j = 0; j < 100; j++) {
            *aPtr++ = bPtr[pos];
            pos += 320;
        }
    }

    for (i = 320; i < 640; i++) {
        pos = (i - 320) + 32004;
        aPtr = BackArray[i];
        for (j = 0; j < 100; j++) {
            *aPtr++ = bPtr[pos];
            pos += 320;
        }
    }
}

// Loads a bitmap (wall, object, screen/backdrop, etc) and returns a
// pointer to the bitmap data. The first 4-bytes of the returned pointer
// are the width and height (2 shorts) followed by the raw pixel data.
// The filename passed can be either a string specifying a file or an
// integer specifying a resource from the currently open resource file.
UCHAR* LoadBitmap(UCHAR bitmapType, char *fName) {
    UCHAR *bPtr = NULL;

    switch (bitmapType) {
    case BMLOAD_BBM:
        bPtr = AckReadiff(fName);
        break;
    case BMLOAD_GIF:
        bPtr = AckReadgif(fName);
        break;
    case BMLOAD_PCX:
        bPtr = AckReadPCX(fName);
        break;
    }

    return bPtr;
}

// Loads a 256 color palette and returns a pointer to the raw color data.
UCHAR* LoadPalette(int resource) {
    UCHAR *ptr = NULL;
    FILE *fp;

    fp = fdopen(rsHandle,"rb");
    if (!fp)
        return NULL;

    fseek(fp, rbaTable[(int)resource], SEEK_SET);

    ptr = (UCHAR*)AckMalloc(768);
    fread(ptr, 768, 1, fp);

    return ptr;
}


// Loads a background image and processes the image into the
// separate slices for use at display time. Currently a background image
// can be 640 columns wide. This number can be made dynamic if needbe and would
// need to be changed in the routine below and in the DrawBackground routine
// in the ACK engine.
int LoadBackDrop(ACKENG *ae) {
    UCHAR *bPtr;

    if (ResScrollBack) {
        bPtr = LoadBitmap(ae->bmLoadType, (char*)ResScrollBack);
        if (bPtr == NULL)
            return 8;

        ProcessBackDrop(bPtr);
        AckFree(bPtr);
    }

    return 0;
}


// Reads a text line from the resource file
int ReadLine(void) {
    int len;
    char ch;

    len = 0;
    while (len < 200) {
        if (read(rsHandle, &LineBuffer[len], 1) != 1)
            break;

        ch = LineBuffer[len];
        if (ch == 10)
            continue;

        if (ch == 13)
            break;

        len++;
    }

    LineBuffer[len] = '\0';

    return len;
}

// Skips to the next parameter in a text line
char *GetNextParm(char *s) {
    char ch;

    while (*s) {
        ch = *s++;
        if (ch == ',') {
            while (*s) {
                ch = *s;
                if (ch != ',' && ch != ' ' && ch != '\t')
                    return s;
                s++;
            }
            return NULL;
        }
    }

    return NULL;
}

// Loads a wall bitmap specified in info file
int LoadWall(ACKENG *ae) {
    int wnum, rnum, result;
    long pos;
    char *lb;

    lb = LineBuffer;        // Info file buffer
    wnum = atoi(lb);        // Wall number to load into

    lb = GetNextParm(lb);

    if (lb == NULL)
        return -1;

    rnum = atoi(lb);            // Resource number

    pos = lseek(rsHandle, 0L, SEEK_CUR);
    result = AckLoadWall(ae, (short)wnum, (char *)rnum);
    lseek(rsHandle, pos, SEEK_SET);

    return result;
}

// Loads an object bitmap specified in info file
int LoadObject(ACKENG *ae) {
    int onum, rnum, result;
    long pos;
    char *lb;

    lb = LineBuffer;
    onum = atoi(lb);        // Object bitmap number

    lb = GetNextParm(lb);

    if (lb == NULL)
        return -2;

    rnum = atoi(lb);        // Resource number
    pos = lseek(rsHandle, 0L, SEEK_CUR);
    result = AckLoadObject(ae, (short)onum, (char *)rnum);
    lseek(rsHandle, pos, SEEK_SET);

    return result;
}

// Skip any leading spaces in the string
// NOTE: Actually modifies the string passed!
char *SkipSpaces(char *s) {
    while (*s == ' ' || *s == '\t' || *s == ',')
        strcpy(s, &s[1]);

    return s;
}

// Creates and object of the desired style
int CreateObject(ACKENG *ae) {
    short onum, vnum;
    short result, oType;
    short NumViews, bmPerView;
    USHORT flags;
    char *lb;
    OBJSEQ os;

    lb = LineBuffer;

    if (!strnicmp(lb, "NUMBER:", 7)) {
        lb = &lb[7];
        onum = (short)atoi(lb);
        if (onum < 1 || onum > MAX_OBJECTS)
            return -3;

        result = AckCreateObject(ae, (short)onum);
        if (result)
            return result;

        LastObjectIndex = onum;
        lb = GetNextParm(lb);
        if (lb == NULL)
            return -4;

        ae->ObjList[onum]->Speed = (short)atoi(lb);
        return 0;
    }

    onum = LastObjectIndex;     // Object number

    oType = 0;

    if (!strnicmp(lb, "CREATE:", 7)) {
        oType = NO_CREATE;
        lb = &lb[7];
    }

    if (!strnicmp(lb, "DESTROY:", 8)) {
        oType = NO_DESTROY;
        lb = &lb[8];
    }

    if (!strnicmp(lb, "WALK:", 5)) {
        oType = NO_WALK;
        lb = &lb[5];
    }

    if (!strnicmp(lb, "ATTACK:", 7)) {
        oType = NO_ATTACK;
        lb = &lb[7];
    }

    if (!strnicmp(lb, "INTERACT:", 9)) {
        oType = NO_INTERACT;
        lb = &lb[9];
    }

    if (!oType)
        return -5;

    lb = SkipSpaces(lb);
    if (lb == NULL)
        return -6;

    flags = 0;
    strupr(lb);

    if (strstr(lb, "ANIMATE") != NULL)
        flags |= OF_ANIMATE;

    if (strstr(lb, "MOVEABLE") != NULL)
        flags |= OF_MOVEABLE;

    if (strstr(lb, "PASSABLE") != NULL)
        flags |= OF_PASSABLE;

    if (strstr(lb, "MULTIVIEW") != NULL)
        flags |= OF_MULTIVIEW;

    if (strstr(lb, "SHOWONCE") != NULL)
        flags |= OF_ANIMONCE;

    lb = GetNextParm(lb);
    if (lb == NULL)
        return-5;

    NumViews = (short)atoi(lb);
    if (NumViews < 1)
        return -6;

    lb = GetNextParm(lb);
    if (lb == NULL)
        return -7;

    bmPerView = (short)atoi(lb);
    if (bmPerView < 1)
        return -7;

    vnum = (short)(NumViews * bmPerView);
    if (vnum > MAX_OBJ_BITMAPS)
        return -8;

    lb = GetNextParm(lb);
    if (lb == NULL)
        return -9;

    vnum = 0;

    while (lb != NULL && vnum < MAX_OBJ_BITMAPS) {
        os.bitmaps[vnum++] = (short)atoi(lb);
        lb = GetNextParm(lb);
    }

    os.bmBitmapsPerView = bmPerView;
    os.flags = flags;
    os.MaxBitmaps = bmPerView;
    os.bmSides = NumViews;

    result = (short)AckSetupObject(ae, onum, oType, &os);

    return result;
}

// Reads the ASCII info file and processes the commands.
int ProcessInfoFile(ACKENG *ae) {
    int result;
    int mode;
    long pos;

    LineNumber = 0;
    // Position to start of info file within resource file
    lseek(rsHandle, rbaTable[0], SEEK_SET);

    mode = result = 0;

    while (!result) {
        LineNumber++;
        if (!ReadLine())
            continue;

        if (*LineBuffer == ';')
            continue;

        if (!strnicmp(LineBuffer, "END:", 4))
            break;

        printf(".");

        switch (mode) {
        case 1:     // Read walls
            if (!strnicmp(LineBuffer, "LOADTYPE:", 9)) {
                ae->bmLoadType = (short)atoi(&LineBuffer[9]);   // Sets for GIF or BBM
                break;
            }

            if (!strnicmp(LineBuffer, "ENDBITMAPS:", 11))
                mode = 4;
            else
                result = LoadWall(ae);
            break;

        case 2:     // Object bitmaps
            if (!strnicmp(LineBuffer, "LOADTYPE:", 9)) {    // Sets for GIF or BBM
                ae->bmLoadType = (short)atoi(&LineBuffer[9]);
                break;
            }

            if (!strnicmp(LineBuffer, "ENDBITMAPS:", 11))
                mode = 5;
            else
                result = LoadObject(ae);
            break;

        case 3:     // Create Object
            if (!strnicmp(LineBuffer, "ENDDESC:", 8))
                mode = 5;
            else
                result = CreateObject(ae);
            break;

        case 4:     // Walls topic
            if (!strnicmp(LineBuffer, "BITMAPS:", 8))
                mode = 1;

            if (!strnicmp(LineBuffer, "ENDWALLS:", 9))
                mode = 0;
            break;

        case 5:     // Objects topic
            if (!strnicmp(LineBuffer, "BITMAPS:", 8))
                mode = 2;

            if (!strnicmp(LineBuffer, "OBJDESC:", 8))
                mode = 3;

            if (!strnicmp(LineBuffer, "ENDOBJECTS:", 11))
                mode = 0;
            break;


        default:
            if (!strnicmp(LineBuffer, "WALLS:", 6)) {
                mode = 4;
                break;
            }

            if (!strnicmp(LineBuffer, "OBJECTS:", 8)) {
                mode = 5;
                break;
            }

            if (!strnicmp(LineBuffer, "MAPFILE:", 8)) {
                MapResource = atoi(&LineBuffer[8]);
                pos = lseek(rsHandle, 0L, SEEK_CUR);
                result = AckReadMapFile(ae, (char*)MapResource);
                lseek(rsHandle, pos, SEEK_SET);
                break;
            }

            if (!strnicmp(LineBuffer, "PALFILE:", 8)) {
                PalResource = atoi(&LineBuffer[8]);
                break;
            }

            if (!strnicmp(LineBuffer, "XPLAYER:", 8)) {
                ae->xPlayer = (short)atoi(&LineBuffer[8]);
                break;
            }

            if (!strnicmp(LineBuffer, "YPLAYER:", 8)) {
                ae->yPlayer = (short)atoi(&LineBuffer[8]);
                break;
            }

            if (!strnicmp(LineBuffer, "PLAYERANGLE:", 12)) {
                ae->PlayerAngle = (short)atoi(&LineBuffer[12]);
                break;
            }

            if (!strnicmp(LineBuffer, "SCREENBACK:", 11)) {
                ResScreenBack = atoi(&LineBuffer[11]);
                break;
            }

            if (!strnicmp(LineBuffer, "SCROLLBACK:", 11)) {
                ResScrollBack = atoi(&LineBuffer[11]);
                break;
            }

            if (!strnicmp(LineBuffer, "TOPCOLOR:", 9)) {
                ae->TopColor = (short)atoi(&LineBuffer[9]);
                break;
            }

            if (!strnicmp(LineBuffer, "BOTTOMCOLOR:", 12)) {
                ae->BottomColor = (short)atoi(&LineBuffer[12]);
                break;
            }

            if (!strnicmp(LineBuffer, "SHADING:", 8)) {
                strupr(LineBuffer);
                if (strstr(&LineBuffer[8], "OFF") != NULL)
                    ae->LightFlag = SHADING_OFF;
                else
                    ae->LightFlag = SHADING_ON;
                break;
            }

            if (!strnicmp(LineBuffer, "FLOORS:", 7)) {
                strupr(LineBuffer);
                if (strstr(&LineBuffer[7], "OFF") != NULL)
                    ae->SysFlags |= SYS_SOLID_FLOOR;
                else
                    ae->SysFlags &= ~SYS_SOLID_FLOOR;
                break;
            }

            if (!strnicmp(LineBuffer, "CEILING:", 8)) {
                strupr(LineBuffer);
                if (strstr(&LineBuffer[8], "OFF") != NULL)
                    ae->SysFlags |= SYS_SOLID_CEIL;
                else
                    ae->SysFlags &= ~SYS_SOLID_CEIL;
                break;
            }

            if (!strnicmp(LineBuffer, "SINGLEBITMAP:", 13)) {
                strupr(LineBuffer);
                if (strstr(&LineBuffer[13], "ON") != NULL)
                    ae->SysFlags |= SYS_SINGLE_BMP;
                else
                    ae->SysFlags &= ~SYS_SINGLE_BMP;
                break;
            }

            if (!strnicmp(LineBuffer, "CEILBITMAP:", 11)) {
                ae->CeilBitmap = (short)atoi(&LineBuffer[11]);
                break;
            }

            if (!strnicmp(LineBuffer, "FLOORBITMAP:", 12)) {
                ae->FloorBitmap = (short)atoi(&LineBuffer[12]);
                break;
            }

            if (!strnicmp(LineBuffer, "RESOLUTION:", 11)) {
                Resolution = (short)atoi(&LineBuffer[11]);
                break;
            }

            if (!strnicmp(LineBuffer, "LOADTYPE:", 9)) {
                ae->bmLoadType = (short)atoi(&LineBuffer[9]);   // Sets for GIF or BBM
                break;
            }

            break;
        }
        fflush(stdout);
    }

    printf("\n");

    return result;
}


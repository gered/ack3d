// ACKENG.H header file for supporting raycasting routines.
// This file contains the main data structure named SLICE and definitions
// required to support the raycasting routines. The data structures defined
// in this file are not to be used directly by the programmer who is using
// the functions in the ACK-3D library. To locate data structures for the
// ACK-3D interface, see the file ACK3D.H.

#ifndef ACKENG_H_INCLUDED
#define ACKENG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define TRANS_WALLS     0
#define FLOOR_ACTIVE    1
#define USE_XMS         0       // Set to 0 if XMS not desired

#define MAX_RBA         500     // Number of rba's in resource header

// Fixed point constants used to perform fixed point calculations.
#define FP_SHIFT        16
#define FP_MULT         65536
#define FP_HALF         32768

#define VIEW_WIDTH      320     // The number of columns in a view (screen)
#define MAX_DISTANCE    2048    // The max distance from the POV to a wall slice

#define TYPE_WALL       0
#define TYPE_OBJECT     1
#define TYPE_PALETTE    2

#define MAX_HEIGHT      960     // Maximum height of a wall
#define MIN_HEIGHT      8       // Minimum height of a wall

#define MAX_UPDOWN      30      // Max up or down spots for each level

#define MAP_STARTCODE   0xFC    // Force player to this square
#define MAP_UPCODE      0xFD    // Go up to previous level
#define MAP_DOWNCODE    0xFE    // Go down to next level
#define MAP_GOALCODE    0xFF    // Finish line!

#define ST_WALL         1
#define ST_OBJECT       2

#define COLS_PER_BYTE   1   // Use 1 for normal mode 13h, 4 for modeX
#define BYTES_PER_ROW   320 // Use 320 for normal mode 13h, 80 for modeX
#define DWORDS_PER_ROW  (BYTES_PER_ROW / 4)
#define SCREEN_SIZE     64000

// Holds information for the current wall section found during the raycasting process.
// During the raycasting process, ACK-3D casts out rays, looks for a wall at a
// given screen column position, and if a wall (slice) is found, information about
// the slice is stored in this structure.

typedef struct _slicer {
    UCHAR     **bMap;   // Pointer to wall bitmap found while ray casting
    UCHAR     *mPtr;    // Grid pointer to reference multi-height wall data
    short     bNumber;  // Bitmap number of the wall found
    unsigned short    bColumn;  // Screen column location of the found slice
    short     Distance; // Distance from the POV to the slice
    short     mPos;     // Position of the slice in the associated map
    unsigned char   Type;   // Indicates if the slice is a wall or object
    void    (*Fnc)(void);   // Pointer to a function to draw wall or object
    unsigned char   Active; // Indicates last slice in listif a wall or object is displayable or not
                // The next two pointers are used if the current slice
                // is part of a transparent wall
    struct _slicer *Prev;   // References the wall slice in front of current slice
    struct _slicer *Next;   // References the wall slice behind the current slice
    } SLICE;

#ifdef __cplusplus
};
#endif

#endif


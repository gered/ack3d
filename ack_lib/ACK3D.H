// ACK-3D.H header file for the engine interface.
// This file contains the main data structures and definitions
// required to support the ACK-3D engine. The data structures defined are
// the ones you use to set up the communication links between your application
// and the ACK-3D library. The four main data structures set up in this header file 
// include: ACKENG, DOORS, NEWOBJECT, and OBJSEQ.

// USED TO RID OURSELVES OF THE MANY CASTING PROBLEMS 
#define CAST(t,f) (t)(f)

// Internal definitions used to simplify field declarations. 
typedef unsigned long	ULONG;
typedef unsigned short	USHORT;
typedef unsigned char	UCHAR;

// Error codes returned from ACK-3D functions. You can use these error codes in your
//    applications to debug the function calls.
#define ERR_BADFILE         100 // File not found, usually bad filename
#define ERR_BADCOMMAND      101 // Bad command found in the .INF file
#define ERR_BADOBJNUMBER    102 // Object number out of range
#define ERR_BADSYNTAX       103 // Invalid syntax in the .INF file
#define ERR_LOADINGBITMAP   104 // Error while reading a Bitmap file
#define ERR_BADDIRECTION    105 // Obsolete, was error in direction field of objects
#define ERR_BADSTARTX       106 // Error in initial X position of player
#define ERR_BADSTARTY       107 // Error in initial Y position of player
#define ERR_BADANGLE        108 // Error in initial player angle
#define ERR_BADMAPFILE      109 // Error in map file header
#define ERR_READINGMAP      110 // Error during read of map file
#define ERR_BADPICNAME      111 // Error in bitmap filename
#define ERR_INVALIDFORM     112 // Error in .BBM or .LBM format
#define ERR_NOPBM           113 // Error in .BBM format
#define ERR_BADPICFILE      114 // Error reading a bitmap file
#define ERR_NOMEMORY        115 // Out of memory error
#define ERR_BADPALFILE      116 // Error reading raw palette file
#define ERR_BADWINDOWSIZE   117 // Invalid window size for engine
#define ERR_TOMANYVIEWS     118 // Out of range object views
#define ERR_BADOBJECTNUM    119 // Invalid object index
#define ERR_BADOBJTYPE      120 // Invalid object type

// Defines for angle sizes used with the ACK-3D engine.
// These values are used as table look up indexes. The range is from 0 to 1800
// where each value represents and angle increment of 0.5 degrees;
// 0 = 0 degrees and 1800 = 360 degrees.
#define INT_ANGLE_1     5
#define INT_ANGLE_2     10
#define INT_ANGLE_4     20
#define INT_ANGLE_6     30
#define INT_ANGLE_32    160
#define INT_ANGLE_45    225
#define INT_ANGLE_90    450
#define INT_ANGLE_135   675
#define INT_ANGLE_180   900
#define INT_ANGLE_225   1125
#define INT_ANGLE_270   1350
#define INT_ANGLE_315   1575
#define INT_ANGLE_360   1800

// These values are returned by the AckMovePOV() and AckMoveObjectPOV() functions.
#define POV_NOTHING     0       // Nothing was hit
#define POV_XWALL       1       // An X wall was hit
#define POV_YWALL       2       // A Y wall was hit
#define POV_OBJECT      3       // An object was hit
#define POV_PLAYER      4       // The player was hit by object
#define POV_SLIDEX      5       // Sliding along an X wall
#define POV_SLIDEY      6       // Sliding along a Y wall
#define POV_NODOOR      0       // No door was opened
#define POV_XDOOR       1       // An X door was opened
#define POV_YDOOR       2       // A Y door was opened
#define POV_XSECRETDOOR 3       // An X secret door was opened
#define POV_YSECRETDOOR 4       // A Y secret door was opened
#define POV_DOORLOCKED  0x80    // Bit is on if door is locked

// Defines required to support bitmaps.
// Each bitmap used in ACK-3D is 64x64 pixels in size.
#define BITMAP_WIDTH	64
#define BITMAP_HEIGHT	64
#define BITMAP_SHIFT	6	// Bits to shift for bitmap width
#define BITMAP_SIZE	(BITMAP_WIDTH * BITMAP_HEIGHT)

// Defines used to set up map grid. The grid is used with the ACKENG structure.
// Each grid component is 64x64 units in size.
#define GRID_MASK       0xFFC0  // Use FF80 for 128 and FFC0 for 64
#define GRID_SIZE       64      // The size of an individual grid unit
#define GRID_WIDTH      64
#define GRID_HEIGHT     64
#define GRID_MAX        GRID_WIDTH * GRID_HEIGHT
#define GRID_XMAX       BITMAP_WIDTH * GRID_WIDTH
#define GRID_YMAX       BITMAP_WIDTH * GRID_HEIGHT
#define GRID_XMAXLONG   (GRID_XMAX * FP_MULT)
#define GRID_YMAXLONG   (GRID_YMAX * FP_MULT)
// The main grid array used with the ACKENG structure
#define GRID_ARRAY	(GRID_WIDTH+2) * (GRID_HEIGHT+2)

// Defines to specify maximum sizes for the various components used with
// ACK-3D, including wall bitmaps, light zones, shading palette, views, multi-
// height walls, doors, object bitmaps, and objects.
#define MAX_WALLBMPS    256     // Total wall bitmaps allowed
#define MAX_ZONES       8		// Number of light zones
#define PAL_SIZE        4096    // Shading palette ranges
#define MAX_VIEWS       47      // Total sides to an object
#define MAX_MULTI       3       // Max height for multi-height walls
#define MAX_DOORS       10      // Max number of doors that can be opened or
//                              // closed at one time
#define MAX_OBJBMPS     256     // Total object bitmaps allowed
#define MAX_OBJECTS     254     // Total objects allowed in map

// Defines to set up different wall types.
#define WALL_TYPE_TRANS	0x0800	// Transparent wall
#define WALL_TYPE_MULTI	0x0400	// Wall is 1.5 times high
#define WALL_TYPE_UPPER	0x0200	// Wall is above floor level
#define WALL_TYPE_PASS	0x0100	// Wall can be walked through

#define TYPE_WALL	0
#define TYPE_OBJECT	1

#define RES_LOW     1   // Resolution is low for walls, floor, and ceiling
#define RES_MEDIUM	2   // Resolution is low only for floor and ceiling

//***************************************************************
// Definitions and data structures used to represent and process objects.

// These defines specify the two settings for the Active field in the NEWOBJECT structure.
#define OBJECT_ACTIVE   1		// Active and may be moveable
#define OBJECT_INACTIVE 0		// Won't be considered moveable

// These defines specify values for the Flags field in the NEWOBJECT structure.
// They are used to indicate the type of animation available for an object.
#define OF_PASSABLE     0x80	// Object can be walked thru
#define OF_ANIMATE      0x40	// Object bitmaps are animated
#define OF_MOVEABLE     0x20	// Object will move X,Y
#define OF_MULTIVIEW    0x10	// Object has multiple sides
#define OF_ANIMONCE     0x08	// Animate once then stop
#define OF_ANIMDONE     0x04	// One shot animation is completed

// These defines specify values for the CurrentType field in the NEWOBJECT structure.
// They are used to indicate the current status of the object.
// The names given are arbitrary and can be used for a variety of purposes.
#define NO_CREATE   1
#define NO_DESTROY  2
#define NO_WALK     3
#define NO_ATTACK   4
#define NO_INTERACT 5

// The Object Sequence structure. This structure is used by the NEWOBJECT structure.
#define MAX_OBJ_BITMAPS	32  // Max bitmaps per sequence type
typedef struct {
	UCHAR	flags;                      // Flags for this sequence
	UCHAR	bitmaps[MAX_OBJ_BITMAPS];   // Bitmap numbers in this sequence
	short	bmSides;                    // Number of views in sequence
	short	bmBitmapsPerView;           // Number of bitmaps in each view
	short	AngleFactor;                // Used internally to support the object sequence
	UCHAR	MaxBitmaps;                 // Max bitmaps in this sequence
	} OBJSEQ;

// The defininition of the main object structure--NEWOBJECT.
typedef struct {
	char	Active;             // Determines object status: 0=Inactive, 1=Active
	UCHAR	Flags;              // Misc flags for this object
	char	Speed;              // Speed of obj (used by application)
	short	Dir;                // Direction of obj (used by application)
	short	x;                  // Current x,y location in grid
	short	y;
	short	mPos;               // Current map location in grid
	UCHAR	id;                 // Object id
	short	CurrentType;        // Create, Destroy, etc. is current
	UCHAR	*CurrentBitmaps;    // Current bitmap list
	short	Sides;              // Number of views
	short	aFactor;			// Angle factor
	short	BitmapsPerView;		// Number of bitmaps in each view
	UCHAR	CurrentBm;          // Current bitmap number
	UCHAR	Maxbm;              // Max bitmap number for this view
	OBJSEQ	Create;             // Stores structures for the 5 object sequences
	OBJSEQ	Destroy;
	OBJSEQ	Walk;
	OBJSEQ	Attack;
	OBJSEQ	Interact;
	} NEWOBJECT;

//**************************************************************************
// The definitions and data structure that are used to represent doors.

// The default value for the Speed field in the DOORS structure.
#define DEFAULT_DOOR_SPEED  2
// These defines are used to set the Type field.
// Obsolete door codes since any wall can be a door
#define DOOR_XCODE      60		// Map codes for the various doors
#define DOOR_SIDECODE   61
#define DOOR_YCODE      62

// These two defines are used to set the Flags field in the DOORS structure.
#define DOOR_OPENING        0x80	// Set if door is currently opening
#define DOOR_CLOSING        0x40	// Set if door is currently closing
// Other attributes that can be assigned to the Flags field.
#define DOOR_TYPE_SECRET    0x8000		// Secret door
#define DOOR_LOCKED         0x4000		// Locked door
#define DOOR_TYPE_SLIDE     0x2000		// Sliding door
#define DOOR_TYPE_SPLIT     0x1000		// Split door

// The main DOORS structure.
typedef struct {
	short   mPos;		// Stores position info for a door
	short   mPos1;
	short   mCode; 		// Bitmap ID of the door
	short   mCode1;
	UCHAR   ColOffset;	// Column offset for the door
	char    Speed;		// Speed setting for opening and closing the door
	char    Type;		// Code for the door type
	UCHAR   Flags;		// Door attribute settings
} DOORS;

//***********************************************************************
// The defines and data structure for the main interface structure--ACKENG.

// These defines are used for the LightFlag field in the ACKENG structure.
#define SHADING_ON	1	// Set if distance shading is on
#define SHADING_OFF	0

// These defines are used for the SysFlags field in the ACKENG structure.
#define SYS_SOLID_BACK	0x8000		// On if solid color bkgd vs picture
#define SYS_SOLID_FLOOR 0x4000		// On if solid vs texture floor
#define SYS_SOLID_CEIL	0x2000		// On if solid vs texture ceiling
#define SYS_NO_WALLS	0x1000		// On if walls are NOT to display
#define SYS_SINGLE_BMP	0x0800		// On if 1 bitmap for floor & ceiling

// These defines indicate how bitmaps will be loaded. they are used with the bmLoadType field.
#define BMLOAD_BBM	0	// Bitmaps will be loaded using BBM format
#define BMLOAD_GIF	1	// Bitmaps will be loaded using GIF format
#define BMLOAD_PCX	2	// Bitmaps will be loaded using PCX format

// The main interface structure used between the application and the ACK-3D engine.
// This structure MUST be allocated or defined before any ACK-3D calls are made.
typedef struct {
	USHORT      xGrid[GRID_ARRAY];      // Map for X walls
	USHORT      yGrid[GRID_ARRAY];      // Map for Y walls
	UCHAR       *mxGrid[GRID_ARRAY];	// Wall data for multi-height X walls
	UCHAR       *myGrid[GRID_ARRAY];	// Wall data for multi-height Y walls
	UCHAR       *bMaps[MAX_WALLBMPS];	// Pointers to wall bitmaps
	UCHAR       *oMaps[MAX_OBJBMPS];    // Pointers to object bitmaps

	UCHAR       *ScreenBuffer;      // 64k buffer for screen
	UCHAR       *OverlayBuffer;     // Buffer for compiled overlay
	UCHAR       *BkgdBuffer;        // Buffer for ceiling, floor

	short       xPlayer;            // X value from 0 to 4095--stores current position
	short       yPlayer;            // Y value from 0 to 4095--stores current position
	short       PlayerAngle;        // Angle value from 0 to 1799

	short       DoorSpeed;          // Door open/close speed
	short       NonSecretCode;      //  Wall code for secret door

	UCHAR       TopColor;           // Base color of ceiling
	UCHAR       BottomColor;        // Base color of floor
	UCHAR       FloorBitmap;        // Bitmap number for single floor
	UCHAR       CeilBitmap;         // Bitmap number for single ceiling

	UCHAR       LightFlag;          // 0 = no light shading, 1 = ON
	UCHAR       PalTable[PAL_SIZE]; // 16 zones of 256 colors each

	short       WinStartX;          // Value of left side of viewport
	short       WinStartY;          // Value of top side of viewport
	short       WinEndX;            // Value of right side
	short       WinEndY;            // Value of bottom side
	short       CenterRow;          // Value of (WinEndY-WinStartY)/2
	short       CenterOffset;       // Center row times bytes per row
	short       WinWidth;           // Value of WinEndX - WinStartX
	short       WinHeight;          // Value of WinEndY - WinStartY
	USHORT      WinLength;          // Number of dwords in window
	USHORT      WinStartOffset;     // Value of WinStartY * 320
	USHORT      SysFlags;           // General system flags--determines display attributes
	UCHAR       bmLoadType;         // Bitmap load flags (BBM, GIF, PCX, etc)
	short       MaxObjects;         // Total number of objects in map
	NEWOBJECT   *ObjList[MAX_OBJECTS+1];    // Current objects in map
	DOORS       Door[MAX_DOORS];    // Doors moving at one time
} ACKENG;

// Structure used to build the palette ranges for light shading
// There are 16 color ranges (or zones) each containing 256 colors.
typedef struct {
	unsigned char start;    // Starting color for this range
	unsigned char length;   // Length of range
} ColorRange;

//*************************************************************************
// Function prototypes for the user callable ACK-3D functions.
//*************************************************************************
// The user callable functions defined in ACKINIT.C.
//*************************************************************************
// Reads trig files, builds wall and object maps, and performs the general
// initialization tasks. This function also sets up the distance table used by ACK-3D.
short AckInitialize(ACKENG *ae);

// Opens a resource file and prepares for reading.
short AckOpenResource(char *ResFileName);

// Closes the currently open resource file.
void AckCloseResource(void);

// Reads the map file and sets up the map grids.
short AckReadMapFile(ACKENG *ae,char *MapFileName);

//*************************************************************************
// The user callable functions defined in ACKLDBMP.C.
//*************************************************************************
// Loads a bitmap and places it in either a wall bitmap array or an object bitmap array.
short AckLoadBitmap(ACKENG *ae,short BitmapNumber,short BitmapType,char *bmFileName);

// Loads a wall bitmap and places it into the wall array.
short AckLoadWall(ACKENG *ae,short WallNumber,char *bmFileName);

// Loads an object bitmap and places it into the object array.
short AckLoadObject(ACKENG *ae,short BmpNumber,char *bmFileName);

// Fills in an ObjList structure with the information passed in.
short AckCreateObject(ACKENG *ae,short ObjNumber);

// Sets up an object into one of the pre-defined sequences (CREATE, DESTRY, and so on).
short AckSetObjectType(ACKENG *ae,short oNum,short oType);

// Fills in an object structure with a communication structure passed by the application.
short AckSetupObject(ACKENG *ae,short oNum,short oType,OBJSEQ *os);

//*************************************************************************
// The user callable functions defined in ACKIFF.C, ACKGIF.C, and ACKPCX.C
//*************************************************************************
// Reads in a .LBM or .BBM file and returns a buffer.
UCHAR *AckReadiff(char *FileName);

// Reads a 256 color GIF file and returns a buffer.
UCHAR *AckReadgif(char *FileName);

// Reads in a 256 color PCX file and returns a buffer.
UCHAR *AckReadPCX(char *filename);

//*************************************************************************
// The user callable functions defined in ACKVIEW.C.
//*************************************************************************
// Assigns the current engine structure to various global variables.
// This function MUST be called before AckBuildView() is called.
void AckRegisterStructure(ACKENG *ae);

// Draws the current view into ScreenBuffer.
void AckBuildView(void);

// Returns the angle (0-1919) between two objects.
short AckGetObjectAngle(long DeltaX,long DeltaY);

// Checks if a collision occurs. Used by AckCheckDoorOpen routine.
short AckCheckHit(short xPlayer,short yPlayer,short ViewAngle);


//*************************************************************************
// The user callable functions defined in ACKPOV.C.
//*************************************************************************
// Used by AckMovePOV() and AckMoveObjectPOV() to check collision with objects.
short AckCheckObjPosn(short xPlayer,short yPlayer,short oIndex);

// Runs the list of objects to check movement.
void AckCheckObjectMovement(void);

// Moves the POV by the specified amount at the specified angle.
short AckMovePOV(short Angle,short Amount);

// Moves the object POV by the specified amount at the specified angle.
short AckMoveObjectPOV(short ObjIndex,short Angle,short Amount);

//*************************************************************************
// The user callable function defined in ACKOVER.C.
//*************************************************************************
// Reads overlay file and compiles it into the OverlayBuffer.
short AckCreateOverlay(ACKENG *ae, UCHAR *OverlayScreen);

//*************************************************************************
// The user callable function defined in ACKBKGD.C.
//*************************************************************************
// Builds background buffer from TopColor, BottomColor, and LightFlag.
short AckBuildBackground(ACKENG *ae);

//*************************************************************************
// The user callable function defined in ACKDOOR.C.
//*************************************************************************
// Check if a door in front to open
short AckCheckDoorOpen(short xPlayer,short yPlayer,short PlayerAngle);

//*************************************************************************
// The user callable function defined in ACKWRAP.C.
//*************************************************************************
// Frees memory buffers.
short AckWrapUp(ACKENG *ae);

//*************************************************************************
// The user callable functions defined in ACKUTIL.C.
//*************************************************************************
// Internal memory allocation for development purposes
void *AckMalloc(size_t mSize);

// All memory allocated with AckMalloc() must use AckFree() to free memory.
void AckFree(void *m);

// Reads in a palette file and sets the screen palette.
short AckLoadAndSetPalette(char *FileName);

// Fills in the shading palette table.
void AckSetupPalRanges(ACKENG *ae,ColorRange *ranges);

// Returns the object index number of the last object hit.
short AckGetObjectHit(void);

// Returns the map location of the last wall hit.
short AckGetWallHit(void);

// Removes the specified object from the map.
short AckDeleteObject(ACKENG *ae,short ObjectIndex);

// Sets a new bitmap or changes image in the bitmap array.
short AckSetNewBitmapPtr(short BitmapNumber,UCHAR **Maps,UCHAR *NewBitmap);

// Frees up the memory used by the bitmap.
short AckFreeBitmap(UCHAR *bmType);

//*************************************************************************
// The user callable functions defined in ACKRTN.ASM.
//*************************************************************************
// Sets a previously loaded palette.
void AckSetPalette(UCHAR *PalBuffer);

// Places video in standard 320x200 mode 13h.
void AckSetVGAmode(void);

// Places video in 80x25 color text mode 3.
void AckSetTextmode(void);

// Displays the contents of ScreenBuffer and OverlayBuffer if desired.
short AckDisplayScreen(void);



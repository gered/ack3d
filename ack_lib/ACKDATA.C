#include <stdio.h>

#include "ack3d.h"
#include "ackeng.h"

long scPtr;
UCHAR *bmWall = NULL;

long bmDistance;
long BackDropRows;
USHORT ScreenOffset;

long xPglobal;
long yPglobal;
long xBegGlobal;
long yBegGlobal;

ACKENG *aeGlobal = NULL;
USHORT *xGridGlobal = NULL;
USHORT *yGridGlobal = NULL;
long xPglobalHI;
long yPglobalHI;
ULONG *rbaTable = NULL;
int rsHandle;

long LastX1;
long LastY1;
long iLastX;
long iLastY;

long x_xPos;
long x_yPos;
long x_xNext;
long x_yNext;
long y_xPos;
long y_yPos;
long y_xNext;
long y_yNext;

short MaxDistance;
short LightFlag;
short ErrorCode;

long xMapPosn;
long yMapPosn;

USHORT *Grid = NULL;
USHORT *ObjGrid = NULL;
SLICE Slice[VIEW_WIDTH];
SLICE *sPtr = NULL;

short TotalSpecial;

short DistanceTable[MAX_DISTANCE + 1];
long *AdjustTable = NULL;

short xSecretmPos;
short xSecretmPos1;
short xSecretColumn;

short ySecretmPos;
short ySecretmPos1;
short ySecretColumn;

short TotalSecret;
short ViewColumn;

long *SinTable = NULL;
long *CosTable = NULL;

long *LongTanTable = NULL;
long *LongInvTanTable = NULL;
long InvCosTable[INT_ANGLE_360];
long InvSinTable[INT_ANGLE_360];
long *LongCosTable = NULL;
long *ViewCosTable = NULL;

long *xNextTable = NULL;
long *yNextTable = NULL;

short LastFloorAngle = -1;
short LastFloorX;
short LastFloorY;
short LastMapPosn;
short LastObjectHit;
short TotalObjects;
short FoundObjectCount;
UCHAR ObjectsSeen[MAX_OBJECTS + 1];
short MoveObjectCount;
UCHAR MoveObjectList[MAX_OBJECTS + 1];
UCHAR ObjNumber[MAX_OBJECTS + 1];
USHORT ObjRelDist[MAX_OBJECTS + 1];
short ObjColumn[MAX_OBJECTS + 1];
short ObjAngle[MAX_OBJECTS + 1];
short DirAngle[] =
{INT_ANGLE_270, INT_ANGLE_315, 0,
 INT_ANGLE_45, INT_ANGLE_90,
 INT_ANGLE_135, INT_ANGLE_180,
 INT_ANGLE_225};

UCHAR WorkPalette[768];
UCHAR *BackArray[INT_ANGLE_360];
short Resolution;

long Flooru;
long Floorv;
long Floordu;
long Floordv;
long Floorkx;
long Floorky;
long Floorku;
long Floorkv;
long Floorkdu;
long Floorkdv;
UCHAR *Floorbm = NULL;
UCHAR *Floorscr = NULL;
UCHAR *FloorscrTop = NULL;
UCHAR *Floorptr2 = NULL;
UCHAR *Floors1 = NULL;
UCHAR *Floors2 = NULL;
long Floorht;
long Floorwt;
short Floorvht;
short Flooreht;
long FloorLastbNum;
long FloorLastbm;

short ViewHeight = 31;
short CeilingHeight = 31;
short LastWallHeight;
short PlayerAngle;
short ViewAngle;
USHORT SysFlags;
UCHAR **WallbMaps = NULL;
UCHAR *VidTop = NULL;
UCHAR *VidBottom = NULL;
short BotRowTable[320];
USHORT FloorMap[4096];
USHORT CeilMap[4096];
UCHAR  HitMap[4096];

UINT *VidSeg = (UINT*)0xA0000;
char *scantables[96];
volatile UCHAR   AckKeys[128];   // Buffer for keystrokes
volatile long AckTimerCounter;
volatile short AckTmCount=0;
volatile short AckTmDelay=0;
volatile UCHAR KeyPressed;

// **** End of Data ****


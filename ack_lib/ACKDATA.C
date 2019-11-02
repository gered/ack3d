#include <windows.h>
#include <stdio.h>

#include "ack3d.h"
#include "ackeng.h"

long scPtr;
UCHAR *bmWall;

long bmDistance;
long BackDropRows;
USHORT ScreenOffset;

long xPglobal;
long yPglobal;
long xBegGlobal;
long yBegGlobal;

ACKENG *aeGlobal;
USHORT *xGridGlobal;
USHORT *yGridGlobal;
long xPglobalHI;
long yPglobalHI;
ULONG *rbaTable;
short rsHandle;

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

USHORT *Grid;
USHORT *ObjGrid;
SLICE Slice[VIEW_WIDTH];
SLICE *sPtr;

short TotalSpecial;

short DistanceTable[MAX_DISTANCE + 1];
long *AdjustTable;

short xSecretmPos;
short xSecretmPos1;
short xSecretColumn;

short ySecretmPos;
short ySecretmPos1;
short ySecretColumn;

short TotalSecret;
short ViewColumn;

long *SinTable;
long *CosTable;

long *LongTanTable;
long *LongInvTanTable;
long InvCosTable[INT_ANGLE_360];
long InvSinTable[INT_ANGLE_360];
long *LongCosTable;
long *ViewCosTable;

long *xNextTable;
long *yNextTable;

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
UCHAR *Floorbm;
UCHAR *Floorscr;
UCHAR *FloorscrTop;
UCHAR *Floorptr2;
UCHAR *Floors1;
UCHAR *Floors2;
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
UCHAR **WallbMaps;
UCHAR *VidTop;
UCHAR *VidBottom;
short BotRowTable[320];
USHORT FloorMap[4096];
USHORT CeilMap[4096];
UCHAR  HitMap[4096];

UCHAR *VidSeg;
char *scantables[96];
UCHAR   AckKeys[128];   // Buffer for keystrokes
long AckTimerCounter;

// **** End of Data ****


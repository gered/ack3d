/*         ACK-3D ( Animation Construction Kit 3D )           */

#ifndef ACKEXT_H_INCLUDED
#define ACKEXT_H_INCLUDED

extern  UCHAR       *BackArray[];
extern  long        xPglobal;
extern  long        yPglobal;
extern  long        xBegGlobal;
extern  long        yBegGlobal;
extern  long        BackDropRows;
extern  ACKENG      *aeGlobal;
extern  USHORT      *xGridGlobal;
extern  USHORT      *yGridGlobal;
extern  long        xPglobalHI;
extern  long        yPglobalHI;
extern  ULONG       *rbaTable;

extern  long        bmDistance;


extern  int         rsHandle;
extern  long        LastX1;
extern  long        LastY1;
extern  long        iLastX;
extern  long        iLastY;
extern  short       MaxDistance;
extern  short       ErrorCode;
extern  short       LightFlag;

extern  long        xMapPosn;
extern  long        yMapPosn;

extern  short       DefZone[];
extern  short       AckLightZones[];
extern  UCHAR       *HtTable[];
extern  USHORT      *Grid;
extern  USHORT      *ObjGrid;
extern  UCHAR       HitMap[];
extern  UCHAR       *BitmapXferPtr;
extern  short       TotalSpecial;
extern  short       DistanceTable[];
extern  long        *AdjustTable;
extern  short       xSecretmPos;
extern  short       xSecretmPos1;
extern  short       xSecretColumn;
extern  short       ySecretmPos;
extern  short       ySecretmPos1;
extern  short       ySecretColumn;
extern  short       TotalSecret;
extern  short       ViewColumn;
extern  long        *SinTable;
extern  long        *CosTable;
extern  long        *LongTanTable;
extern  long        *LongInvTanTable;
extern  long        InvCosTable[];
extern  long        InvSinTable[];
extern  long        *LongCosTable;
extern  long        *ViewCosTable;
extern  long        *xNextTable;
extern  long        *yNextTable;
extern  UCHAR       ObjectsSeen[];
extern  UCHAR       MoveObjectList[];
extern  short       TotalObjects;
extern  short       FoundObjectCount;
extern  short       MoveObjectCount;
extern  short       LastObjectHit;
extern  short       LastMapPosn;
extern  UCHAR       ObjNumber[];
extern  USHORT      ObjRelDist[];
extern  short       ObjColumn[];
extern  short       ObjAngle[];
extern  short       DirAngle[];
extern  UCHAR       LightMap[];

extern  USHORT      FloorMap[];
extern  USHORT      CeilMap[];
extern  SLICE       Slice[];
extern  USHORT      ScreenOffset;
extern  short       LastFloorAngle;
extern  short       LastFloorX;
extern  short       LastFloorY;
extern  long        Flooru;
extern  long        Floorv;
extern  long        Floordu;
extern  long        Floordv;
extern  long        Floorkx;
extern  long        Floorky;
extern  long        Floorku;
extern  long        Floorkv;
extern  long        Floorkdu;
extern  long        Floorkdv;
extern  UCHAR       *Floorbm;
extern  UCHAR       *Floorscr;
extern  UCHAR       *FloorscrTop;
extern  UCHAR       *Floorptr2;
extern  UCHAR       *Floors1;
extern  UCHAR       *Floors2;
extern  long        Floorht;
extern  long        Floorwt;
extern  short       Floorvht;
extern  short       Flooreht;
extern  short       ViewAngle;
extern  short       ViewHeight;
extern  short       CeilingHeight;
extern  short       Resolution;
extern  short       LastWallHeight;
extern  short       PlayerAngle;
extern  short       ViewAngle;
extern  USHORT      SysFlags;
extern  SLICE       Slice[];
extern  SLICE       *sPtr;
extern  UCHAR       **WallbMaps;
extern  UCHAR       *VidTop;
extern  UCHAR       *VidBottom;
extern  short       BotRowTable[];
extern  USHORT      FloorMap[];
extern  USHORT      CeilMap[];
extern  char        *scantables[];

extern  volatile UCHAR AckKeys[];
extern  volatile UCHAR KeyPressed;
extern  volatile long AckTimerCounter;
extern  volatile short AckTmCount;
extern  volatile short AckTmDelay;

#endif


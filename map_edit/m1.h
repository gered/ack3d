// Header file for ACK-3D Map Editor
// Author: Lary Myers
// Copyright (c) 1994

#ifndef M1_H_INCLUDED
#define M1_H_INCLUDED

#define RES_SMFONT  0   // Resource ID's
#define RES_MDFONT  1
#define RES_2DSCREEN    2
#define RES_3DSCREEN    3
#define RES_SELSCREEN   4


#define BM_X        272
#define BM_Y        17

#define BM_NUM_X    308
#define BM_NUM_Y    30

#define MAP_X   0
#define MAP_Y   0
#define MAP_X1  256
#define MAP_Y1  160

#define MINI_X  255
#define MINI_Y  135
#define MINI_X1 (MINI_X+63)
#define MINI_Y1 (MINI_Y+63)

#define MAPXY_X     217
#define MAPXY_Y     165
#define MAPXY_X1    231
#define MAPXY_Y1    165

#define CURXY_X     217
#define CURXY_Y     173
#define CURXY_X1    231
#define CURXY_Y1    173

#define GD_FWT      32
#define GD_FHT      32
#define GD_HWT      16
#define GD_HHT      16

#define VM_2D       0   // View mode is 2D
#define VM_3D       1   // View mode is 3D

#define VT_MAP      0   // Viewing map
#define VT_FLOOR    1   // Viewing floor
#define VT_CEIL     2   // Viewing ceiling


#define ED_WALLS    0   // Editing walls
#define ED_OBJECTS  1   // Editing objects

#define AI_UP       1   // Action index's
#define AI_RIGHT    2
#define AI_DOWN     3
#define AI_LEFT     4
#define AI_MINI     5
#define AI_SELECT   6
#define AI_VIEW     7
#define AI_FLOORS   8
#define AI_SAVE     9
#define AI_MAP      10
#define AI_WALLS    11
#define AI_CEILING  12
#define AI_OBJECTS  13
#define AI_OPTIONS  14
#define AI_EDITWIN  15
#define AI_BM_UP    16
#define AI_BM_DOWN  17
#define AI_EXIT     99

#define OPT_NORMAL  0   // Selectable option ID's
#define OPT_SLIDING 1
#define OPT_SPLIT   2
#define OPT_SECRET  3
#define OPT_LOCKED  4
#define OPT_TRANS   5
#define OPT_MULTI   6
#define OPT_RAISED  7
#define OPT_CANCEL  8
#define OPT_FILL    9
#define OPT_BORDER  10
#define OPT_CLEAR   11
#define OPT_PASSABLE    12
#define OPT_PALETTE 13
#define OPT_MAPPAL  14

#define OPT_MAX     15  // Total number of options above


#define BM_COLOR_X  261 // Current bitmap color coordinates
#define BM_COLOR_Y  17
#define BM_COLOR_X1 267
#define BM_COLOR_Y1 48

typedef struct {
    short   Action;
    short   x;
    short   y;
    short   x1;
    short   y1;
    } HS;

typedef struct {
    short   id;
    int xBias;
    int yBias;
    UCHAR   Flags;
    char    *Text;
    } OPTS;

#define OPTF_CHECKED        0x80    // Button will have a checkmark


#define VIDSEG  (UCHAR *)0xA0000

#define MAX_MULTI   3

#define KEY_ESC     0x011B
#define KEY_SPACE   0x3920
#define KEY_F2      0x3C00
#define KEY_F3      0x3D00
#define KEY_UP      0x4800
#define KEY_RIGHT   0x4D00
#define KEY_DOWN    0x5000
#define KEY_LEFT    0x4B00
#define KEY_PGUP    0x4900
#define KEY_PGDN    0x5100

//=============================================================================
// Prototypes;
//=============================================================================

//-----------------------------------------------------------------------------
//  Functions in mouse.c
//-----------------------------------------------------------------------------
short MouseInstalled(void);
void ShowMouse(void);
void HideMouse(void);
short ReadMouseCursor(short *mrow,short *mcol);
short ReadMouseButtons(void);
void SetMouseCursor(short mrow,short mcol);
void SetMouseMinMaxColumns(short mincol,short maxcol);
void SetMouseMinMaxRows(short minrow,short maxrow);
void SetMouseShape(short hsrow,short hscol,char far *mask);
void MouseReleased(void);

//-----------------------------------------------------------------------------
//  Functions in m1.c
//-----------------------------------------------------------------------------
short GetAction(short mx,short my);
short GetSelAction(short mx,short my);
void DrawYline(int x,int y);
void DrawXline(int x,int y);
void DrawGridBlock(int x,int y);
void DrawYbitmap(int x,int y,UCHAR *bm,UCHAR Shade,int TransFlag);
void DrawXbitmap(int x,int y,UCHAR *bm,UCHAR Shade,int TransFlag);
void ClearGridArea(UCHAR *BufPtr);
void RefreshGrid(void);
void DrawGrid3D(void);
void Draw2Dxline(int x,int y,UCHAR color);
void DrawxLineMulti(int x,int y,UCHAR color);
void Draw2Dyline(int x,int y,UCHAR color);
void DrawyLineMulti(int x,int y,UCHAR color);
void DrawGrid2D(void);
void DrawTinyBitmap(int x,int y,UCHAR *bm);
void DrawFloorCeil(void);
void DrawObjects(void);
void DrawGrid(void);
void ShowScreen(char *Name);
void ShowBitmap(int x,int y,UCHAR *bm);
short LoadGridMap(char *Name);
short SaveGridMap(char *Name);
void ShowCurrentBitmap(void);
short GetActionIndex(short action);
void PressButton(short ActionCode);
void ReleaseButton(short ActionCode);
void UpdateScreen(void);
int GetGridPos(short mx,short my);
void PutCode(short mx,short my);
void UpdatePosn(short mx,short my);
void ShowSelBitmaps(UCHAR bCode);
int SelectScreen(void);
UCHAR GetIndexColor(short index);
UCHAR GetIndexColor2(short index);
void DrawBackBox(int x,int y,int x1,int y1);
void ShowButtons(OPTS *op,HS *hs,int x,int y,int xAmt,int yAmt);
int ShowOptions(void);
void FillMap(UCHAR bCode);
void DrawArrow(int x,int y,UCHAR color);
int SelectGridBox(void);
void EditMulti(int mode,short mx,short my);

//-----------------------------------------------------------------------------
//  Functions in m1util.c
//-----------------------------------------------------------------------------
void SoundBeep(void);
short LoadSmallFont(void);
short LoadMedFont(void);
void mdWriteChar(short x,short y,unsigned char ch);
short mdWriteString(short x,short y,char *s);
void smWriteChar(short x,short y,unsigned char ch);
short smWriteString(short x,short y,char *s);
void smWriteHUD(short x,short y,UCHAR color,char *s);
void BlitBlock(int x,int y,int x1,int y1,UCHAR *buf);
void DrawFillBox(int x,int y,int x1,int y1,UCHAR color);
void DrawHorzLine(int x,int y,int x1,UCHAR color);
void DrawVertLine(int x,int y,int y1,UCHAR color);
void DrawBox(int x,int y,int x1,int y1,UCHAR color);
UCHAR GetColor(int x,int y);
void SaveVideo(UCHAR *buf);
void RestoreVideo(UCHAR *buf);
void CreateButton(int x,int y,HS *hs,char *s);
int QueryBox(int x,int y,char *Msg);

//-----------------------------------------------------------------------------
//  Functions in m1read.c
//-----------------------------------------------------------------------------
short LoadDescFile(char *fName);

#endif


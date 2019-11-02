// Header file for 3D Construction Kit
// Started: 01/02/94
//  Author: Lary Myers
//  Module: KIT.H
// (c) CopyRight 1994 All Rights Reserved

#define MODE_GRAPHICS	0x13
#define MODE_TEXT	0x03

#define SCREEN_LENGTH	64000	    // Bytes in one full screen
#define SCREEN_LEN_WORDS 32000	    // Words in one screen
#define SCREEN_LEN_DWORDS 16000	    // DWORDS in one screen
#define SCREEN_PLANES	1	    // Number of planes for this mode
#define SCREEN_WIDTH	320	    // Number of bytes in one row
#define SCREEN_HEIGHT	200	    // Number of rows on screen

#define BUTTON_OK	    0
#define BUTTON_OKCANCEL	    1
#define BUTTON_YESNO	    2
#define BUTTON_ABORTRETRY   3

#define BUTTON_RET_OK	    0
#define BUTTON_RET_CANCEL   1
#define BUTTON_RET_YES	    2
#define BUTTON_RET_NO	    3
#define BUTTON_RET_ABORT    4
#define BUTTON_RET_RETRY    5

// Used in IORTN.C for message boxes
typedef struct {
	short	  x;
	short	  y;
	short	  x1;
	short	  y1;
	short	  index;
	} BUTTON;


// Used in IORTN.C for message boxes
typedef struct {
	short	  Count;
	short	  List[4];
	} INDEXES;


// Used in IORTN.C list boxes
typedef struct _lcb {
    struct  _lcb    *Back;
    struct  _lcb    *Fwd;
	    char    Data[1];
	    } LCB;

typedef struct {
	short	  ux;
	short	  uy;
	short	  ux1;
	short	  uy1;
	short	  dx;
	short	  dy;
	short	  dx1;
	short	  dy1;
	} ARROWRECT;


//=============================================================================
// Prototypes in UTIL.C
//=============================================================================
UINT Inkey(void);
char *GetExtent(char *s);
char *StripEndOfLine(char *s);
char *SkipSpaces(char *s);
char *AddExtent(char *s,char *ext);
char *CopyToComma(char *dest,char *src);
short HasWildCards(char *s);
void SetCurrentPath(char *p);
void InitDiskList(void);
char *GetPath(char *s);
//=============================================================================
// Prototypes in GRAPHICS.C
//=============================================================================
void SetMode(short mode);
void ClearScreen(UCHAR color);
void PutPixel(short x,short y,UCHAR color);
void DrawVerticalLine(short x,short y,short y1,UCHAR color);
void DrawHorizontalLine(short x,short y,short x1,UCHAR color);
void DrawBox(short x,short y,short x1,short y1,UCHAR color);
void DrawRoundBox(short x,short y,short x1,short y1,UCHAR color);
void Draw3DBox(short x,short y,short x1,short y1,UCHAR TopColor,UCHAR BottomColor);
void DrawSolidBox(short x,short y,short x1,short y1,UCHAR color);
void DrawXORBox(short x,short y,short x1,short y1,UCHAR color);
void DrawXORRect(short x,short y,short x1,short y1,UCHAR color);
void DrawLine(short x1,short y1,short x2,short y2,UCHAR color);
void PutBitmap(short x,short y,short width,short height,UCHAR *Bitmap);
UCHAR *GetBitmap(short x,short y,short width,short height,UCHAR *buffer);
UINT GetBufferSize(short width,short height);
void PutBitmapAsIcon(short x,short y,UCHAR *bm);
void PutAckBitmap(short x,short y,UCHAR *bm);
UCHAR *ScreenToBuffer(UCHAR *buffer);
void BufferToScreen(UCHAR *buffer);
void SetPalette(UCHAR *PalBuf);
void ShowChar(short x,short y,char ch);
void ShowString(short x,short y,char *s);
UCHAR *LoadAndShowScreen(char *PicName);
//=============================================================================
// Prototypes in MOUSE.C
//=============================================================================
short MouseInstalled(void);
void ShowMouse(void);
void HideMouse(void);
short ReadMouseCursor(short *mrow,short *mcol);
short ReadMouseButtons(void);
void SetMouseCursor(short mrow,short mcol);
void SetMouseMinMaxColumns(short mincol,short maxcol);
void SetMouseMinMaxRows(short minrow,short maxrow);
void SetMouseShape(short hsrow,short hscol,char *mask);
void MouseReleased(void);
void MouseSetFunction(UCHAR,void *);
//=============================================================================
// Prototypes in READIFF.C
//=============================================================================
UCHAR *Readiff(char *picname);
//=============================================================================
// Prototypes in IO.C
//=============================================================================
void GetFontTable(void);
void ShowChar(short x,short y,char ch);
void ShowString(short x,short y,char *s);
short ShowMessageBox(char *Body,UINT Buttons);
void ShowListBox(short x,short y,short x1,short y1,short index,LCB *lcb);
char *FileBox(char *fName,char *Path,char *rBuf,char *Title,char *ActiveButton);
void TestListBox(void);
short PickList(short x,short y,short x1,short y1,LCB *Anchor);
short CreateSelectPickList(short x,short y,short x1,short y1,short count,char **List,short SortFlag);
short ShowPickList(char **p,short SortFlag);
//=============================================================================
// Prototypes in SETS.C
//=============================================================================
short LoadSet(char *fName);


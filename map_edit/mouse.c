// 3D Construction Kit
// Started: 01/02/94
//  Author: Lary Myers
//  Module: MOUSE.C
// (c) CopyRight 1994 All Rights Reserved

#include <dos.h>
#include <stdio.h>
#include <string.h>

    int MouseModifier = 2;

//=============================================================================
// Check if mouse is installed, returns -1 if it IS installed
//=============================================================================
int MouseInstalled(void)
{
    int     yesno;
    union REGPACK regs;

memset(&regs,0,sizeof(union REGPACK));
intr(0x33,&regs);
yesno = regs.w.ax;

return(yesno);
}

//=============================================================================
// Show the mouse cursor
//=============================================================================
void ShowMouse(void)
{
    union REGPACK regs;

memset(&regs,0,sizeof(union REGPACK));
regs.w.ax = 1;
intr(0x33,&regs);

}

//=============================================================================
// Hide the mouse cursor
//=============================================================================
void HideMouse(void)
{
    union REGPACK regs;

memset(&regs,0,sizeof(union REGPACK));
regs.w.ax = 2;
intr(0x33,&regs);

}

//=============================================================================
// Returns button status, mouse row and column
//=============================================================================
int ReadMouseCursor(int *mrow,int *mcol)
{
    int     bstatus;
    union REGPACK regs;

memset(&regs,0,sizeof(union REGPACK));
regs.w.ax = 3;
intr(0x33,&regs);
bstatus = regs.w.bx;
*mrow = regs.w.dx;
*mcol = regs.w.cx / MouseModifier;

return(bstatus);
}

//=============================================================================
// Returns just the mouse button status
//=============================================================================
int ReadMouseButtons(void)
{
    int     bstatus;
    union REGPACK regs;

memset(&regs,0,sizeof(union REGPACK));
regs.w.ax = 3;
intr(0x33,&regs);
bstatus = regs.w.bx;

return(bstatus);
}

//=============================================================================
// Set mouse cursor to desired row and column
//=============================================================================
void SetMouseCursor(int mrow,int mcol)
{
    union REGPACK regs;

memset(&regs,0,sizeof(union REGPACK));
regs.w.ax = 4;
regs.w.dx = mrow;
regs.w.cx = mcol * MouseModifier;
intr(0x33,&regs);

}

//=============================================================================
// Defines left and right columns for mouse travel
//=============================================================================
void SetMouseMinMaxColumns(int mincol,int maxcol)
{
    union REGPACK regs;

memset(&regs,0,sizeof(union REGPACK));
regs.w.ax = 7;
regs.w.cx = mincol * MouseModifier;
regs.w.dx = maxcol * MouseModifier;
intr(0x33,&regs);

}


//=============================================================================
// Defines top and bottom rows for mouse travel
//=============================================================================
void SetMouseMinMaxRows(int minrow,int maxrow)
{
    union REGPACK regs;

memset(&regs,0,sizeof(union REGPACK));
regs.w.ax = 8;
regs.w.cx = minrow;
regs.w.dx = maxrow;
intr(0x33,&regs);

}

//=============================================================================
// Set shape of mouse cursor. 8 byte mask, hotspot row,col
//=============================================================================
void SetMouseShape(int hsrow,int hscol,char far *mask)
{
    union REGPACK regs;

memset(&regs,0,sizeof(union REGPACK));
regs.w.ax = 9;
regs.w.dx = FP_OFF(mask);
regs.w.es = FP_SEG(mask);
regs.w.bx = hscol;
regs.w.cx = hsrow;
intr(0x33,&regs);

}

//=============================================================================
// Wait for the mouse button to be released
//=============================================================================
void MouseReleased(void)
{

while (ReadMouseButtons());

}


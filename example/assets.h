#ifndef ASSETS_H_INCLUDED
#define ASSETS_H_INCLUDED

#include "ack3d.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int LineNumber;
extern char LineBuffer[];

extern int MapResource;
extern int PalResource;
extern int ResScreenBack;
extern int ResScrollBack;

UCHAR* LoadBitmap(UCHAR bitmapType, char *fName);
UCHAR* LoadPalette(int resource);
int LoadBackDrop(ACKENG *ae);
int ProcessInfoFile(ACKENG *ae);

#ifdef __cplusplus
};
#endif

#endif


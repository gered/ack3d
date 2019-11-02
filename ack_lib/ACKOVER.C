
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <mem.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys\stat.h>

#include "ack3d.h"
#include "ackeng.h"

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Creates an overlay buffer that contains non-transparent information
// about the image. Position and length of the non-transparent areas is
// stored for later processing after the drawing phase. Theoretically the
// amount of information stored in the overlay buffer could exceed the
// actual size of the image.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckCreateOverlay(ACKENG *ae,UCHAR *sBuf)
{
    USHORT    bPos,vPos,vLen;
    USHORT    len,sPos,sPos1;

vLen = (ae->WinEndY - ae->WinStartY) * BYTES_PER_ROW;
vPos = ae->WinStartY * BYTES_PER_ROW;
bPos = 0;
sPos = vPos;

while (vLen > 0)
    {
    if (sBuf[sPos])
	{
	sPos1 = sPos;
	while (vLen > 0 && sBuf[sPos1++])
	    vLen--;

	len = (sPos1 - sPos) - 1;
	(*(short *)&ae->ScreenBuffer[bPos]) = len;
	bPos += 2;
	(*(short *)&ae->ScreenBuffer[bPos]) = sPos;
	bPos += 2;
	memmove(&ae->ScreenBuffer[bPos],&sBuf[sPos],len);
	bPos += len;
	sPos = sPos1;
	}
    else
	{
	sPos++;
	vLen--;
	}
    }

(*(short *)&ae->ScreenBuffer[bPos]) = 0;
bPos += 2;

ae->OverlayBuffer = AckMalloc(bPos);

if (ae->OverlayBuffer != NULL)
    {
    memmove(ae->OverlayBuffer,ae->ScreenBuffer,bPos);
    return(0);
    }

return(ERR_NOMEMORY);
}

// **** End of Source ****


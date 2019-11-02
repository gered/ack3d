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
#include "ackext.h"

typedef struct {
	int	sel;
	int	off;
	} SELOFF;

extern	char	AckKeyboardSetup;
extern	SELOFF	OldKeybdInt;
extern	char	AckTimerSetup;
extern	SELOFF	OldTimerInt;

void AckSetIntVector(int VecNum,int sel,int VecOff);

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Frees up buffers and closes any resource file that may be open.
// After calling this function, do NOT call AckBuildView() or
// AckDisplayScreen()
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckWrapUp (ACKENG * ae)
{

  AckFree (LongTanTable);
  AckFree (LongInvTanTable);
  AckFree (CosTable);
  AckFree (SinTable);
  AckFree (LongCosTable);
  AckFree (xNextTable);
  AckFree (yNextTable);
  AckFree (ViewCosTable);
  AckFree (AdjustTable);

  if (ae->OverlayBuffer != NULL)
    AckFree (ae->OverlayBuffer);
  ae->OverlayBuffer = NULL;

  if (ae->BkgdBuffer != NULL)
    AckFree (ae->BkgdBuffer);
  ae->BkgdBuffer = NULL;

  if (ae->ScreenBuffer != NULL)
    AckFree (ae->ScreenBuffer);
  ae->ScreenBuffer = NULL;

  if (rsHandle)
    {
      close (rsHandle);
      rsHandle = 0;
    }

if (AckKeyboardSetup)
    {
    AckSetIntVector(9,OldKeybdInt.sel,OldKeybdInt.off);
    AckKeyboardSetup = 0;
    }
if (AckTimerSetup)
    {
    AckSetIntVector(0x1C,OldTimerInt.sel,OldTimerInt.off);
    AckTimerSetup = 0;
    }

return (0);
}

// **** End of Source ****

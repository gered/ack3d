#include <stdio.h>
#include <dos.h>
#include <io.h>

#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

extern  char    AckKeyboardSetup;
extern  void (__interrupt __far *OldKeybdInt)();
extern  char    AckTimerSetup;
extern  void (__interrupt __far *OldTimerInt)();

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
    _dos_setvect(0x9, OldKeybdInt);
    AckKeyboardSetup = 0;
    }
if (AckTimerSetup)
    {
    _dos_setvect(0x1C, OldTimerInt);
    AckTimerSetup = 0;
    }

return (0);
}

// **** End of Source ****

#include "ack3d.h"
#include "ackeng.h"


void AckBuildCeilingFloor (UCHAR *, short, short, short, short, short, short);

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Generates a solid floor and ceiling
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckBuildBackground (ACKENG * ae)
{

/* Let the assembly routine do all the hard work */

#if FLOOR_ACTIVE
#else
  AckBuildCeilingFloor (ae->BkgdBuffer,
            ae->LightFlag,
            ae->TopColor,
            ae->BottomColor,
            ae->WinStartY,
            ae->WinEndY,
            ae->CenterRow);
#endif

  return (0);
}

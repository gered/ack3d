
_BORLANDC_  =   1

MACRO ACKPROC   L
IFDEF _BORLANDC_
PROC    _&L  NEAR
ENDIF
IFDEF _WATCOMC_
PROC    &L_  NEAR
ENDIF
    ENDM

MACRO ACKEXT    L
IFDEF _BORLANDC_
    extrn   _&L:NEAR
ENDIF
IFDEF   _WATCOMC_
    extrn   &L_:NEAR
ENDIF
    ENDM


MACRO   ACKPUBS LAB
IFDEF   _BORLANDC_
     public  _&LAB
ENDIF
IFDEF   _WATCOMC_
     public  &LAB_
ENDIF
     ENDM

MACRO   ACKCALL LAB
IFDEF   _BORLANDC_
    call    _&LAB
ENDIF
IFDEF   _WATCOMC_
    call    &LAB_
ENDIF
    ENDM

IFDEF   _BORLANDC_
VIDSEG  equ 0a0000000h
ENDIF

IFDEF   _WATCOMC_
VIDSEG  equ 0a0000h
ENDIF

;; Equates for SLICE struct
sabMap      equ     0
samPtr      equ     4
sabNumber   equ     8
sabColumn   equ     10
saDist      equ     12
samPos      equ     14
saType      equ     16
saFnc       equ     17
saActive    equ     21
saPrev      equ     22
saNext      equ     26
saSize      equ     30      ;size of SLICE structure

; Equates for upper byte of walls
WALL_TYPE_UPPER     equ     0200h
WALL_TYPE_MULTI     equ     0400h
WALL_UPPER_MULTI    equ     0600h
WALL_TYPE_TRANS     equ     0800h
DOOR_TYPE_SECRET    equ     8000h
DOOR_LOCKED     equ     4000h
DOOR_TYPE_SLIDE     equ     2000h
DOOR_TYPE_SPLIT     equ     1000h
DOOR_WALL       equ     3000h

; Equates for LightFlag
SHADING_OFF equ 0
SHADING_ON  equ 1


ST_WALL     equ 1
ST_OBJECT   equ 2

DOOR_XCODE  equ 60
DOOR_YCODE  equ 62

RES_LOW     equ     1

;; Equates for SCOL struct
Vid equ 0
Wall    equ 4
Pal equ 8
dst equ 12
wht equ 14
multi   equ 16
topht   equ 18
botht   equ 20
savwht  equ 22
mulcnt  equ 24
savVid  equ 26
bNum    equ 30
scLen   equ 32  ; length of structure

; Equates for SysFlag
SYS_SOLID_BACK      equ     8000h   ; On if solid color bkgd vs picture
SYS_SOLID_FLOOR     equ     4000h   ; On if solid vs texture floor
SYS_SOLID_CEIL      equ     2000h   ; On if solid vs texture ceiling
SYS_NO_WALLS        equ     1000h   ; On if walls are NOT to display

Color   equ ebp-4
retval  equ ebp-6
MapPosn equ ebp-8
xBeg    equ ebp-10
;xPos    equ     ebp-14
;xNext   equ     ebp-18
;yPos    equ     ebp-22
;yNext   equ     ebp-26
xd  equ ebp-30
yd  equ ebp-34
yTemp   equ ebp-38
sy  equ ebp-42
sx  equ ebp-46

INT_ANGLE_32    equ 160
INT_ANGLE_90    equ 450
INT_ANGLE_135   equ 675
INT_ANGLE_180   equ 900
INT_ANGLE_225   equ 1125
INT_ANGLE_270   equ 1350
INT_ANGLE_360   equ 1800


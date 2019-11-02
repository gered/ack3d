
        IDEAL
        JUMPS
        P386
        P387            ; Allow 386 processor


        MASM
        .MODEL FLAT     ;32-bit OS/2 model

        IDEAL
        include "ackrtn.inc"
    MASM

        extrn   _WallDistTable:dword
        extrn   _FloorMap:word
        extrn   _CeilMap:word
        extrn   _LastWallHeight:word
        extrn   _ViewAngle:word
        extrn   _ScreenOffset:word
        extrn   _xPglobal:dword
        extrn   _yPglobal:dword
        extrn   _xBegGlobal:dword
        extrn   _yBegGlobal:dword
	extrn	_aeGlobal:dword
        extrn   _xGridGlobal:dword
        extrn   _yGridGlobal:dword
        extrn   _xPglobalHI:dword
        extrn   _yPglobalHI:dword
        extrn   _rbaTable:dword
        extrn   _rsHandle:word
        extrn   _LastX1:dword
        extrn   _LastY1:dword
        extrn   _iLastX:dword
        extrn   _iLastY;dword
        extrn   _MaxDistance:word
        extrn   _BackArray:dword
        extrn   _zdTable:dword
        extrn   _ErrorCode:word
        extrn   _xMapPosn:dword
        extrn   _yMapPosn:dword
        extrn   _Grid:dword
        extrn   _ObjGrid:dword
        extrn   _WallbMaps:dword
        extrn   _ViewHeight:word
        extrn   _CeilingHeight:word
        extrn   _gTopColor:byte
        extrn   _gBottomColor:byte
	extrn	_PlayerAngle:word
        extrn   _gScrnBuffer:dword
        extrn   _gBkgdBuffer:dword
        extrn   _gCenterOff:word
        extrn   _gWinStartOffset:dword
        extrn   _gWinHeight:word
        extrn   _gWinEndY:dword
        extrn   _SysFlags:word
        extrn   _sPtr:dword
        extrn   _mxGridGlobal:dword
        extrn   _myGridGlobal:dword

        extrn   _xSecretmPos:word
        extrn   _xSecretmPos1:word
        extrn   _xSecretColumn:word

        extrn   _ySecretmPos:word
        extrn   _ySecretmPos1:word
        extrn   _ySecretColumn:word

        extrn   _TotalSecret:word
        extrn   _ViewColumn:word
        extrn   _SinTable:dword
        extrn   _CosTable:dword
	extrn	_LongTanTable:dword
        extrn   _LongInvTanTable:dword
        extrn   _InvCosTable:byte
        extrn   _InvSinTable:byte
        extrn   _LongCosTable:dword
        extrn   _ViewCosTable:dword
        extrn   _xNextTable:dword
        extrn   _yNextTable:dword

        extrn   _LastMapPosn:word
        extrn   _LastObjectHit:word
        extrn   _TotalObjects:word
        extrn   _FoundObjectCount:word
        extrn   _ObjectsSeen:byte
        extrn   _MoveObjectCount:word
        extrn   _MoveObjectList:byte
        extrn   _ObjNumber:byte
        extrn   _ObjRelDist:byte
        extrn   _ObjColumn:byte

        extrn   _x_xPos:dword
        extrn   _x_yPos:dword
        extrn   _x_xNext:dword
        extrn   _x_yNext:dword
	extrn	_y_xPos:dword
        extrn   _y_yPos:dword
        extrn   _y_xNext:dword
        extrn   _y_yNext:dword

        extrn   _Resolution:word
        extrn   _Flooru:dword
        extrn   _Floorv:dword
        extrn   _Floordu:dword
        extrn   _Floordv:dword
        extrn   _Floorkx:dword
        extrn   _Floorky:dword
        extrn   _Floorku:dword
        extrn   _Floorkv:dword
        extrn   _Floorkdu:dword
        extrn   _Floorkdv:dword
        extrn   _Floorbm:dword
        extrn   _Floorscr:dword
        extrn   _Floors1:dword
        extrn   _Floors2:dword
        extrn   _FloorscrTop:dword
        extrn   _Floorptr2:dword
        extrn   _Floorht:dword
        extrn   _Floorwt:dword
	extrn	_Floorvht:word
        extrn   _Flooreht:word
        extrn   _FloorLastbNum:dword
        extrn   _FloorLastbm:dword

        extrn   _bmDistance:dword
        extrn   _scwht:word
        extrn   _scWall:dword
        extrn   _scPal:dword
        extrn   _scVid:dword
        extrn   _scantables:dword

        ACKEXT  DrawBackDrop
        ACKEXT  ShowCol
        ACKEXT  ShowColMask
	ACKEXT	FindDoor
	ACKEXT	xRayCast
	ACKEXT	yRayCast

	ACKPUBS xxxAckDrawFloor
	ACKPUBS xxxAckDrawFloorOnly
	ACKPUBS xxxAckDrawCeilingOnly

	.DATA

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; Globals used by the AckDrawFloor routine
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
BCOL    dd      ?
HEIGHT  dd      ?
VA      dd      ?
SY      dd      ?
EY      dd      ?
BFSCRN  dd      ?
BCSCRN  dd      ?
FSCRN   dd      ?
CSCRN   dd      ?
CV      dd      ?
SV      dd      ?
BA	dd	?
BA1     dd      ?
ZDPTR   dd      ?
POS     dd      ?
BMPOS   dd      ?
MPOS    dd      ?
MPOSHI  dd      ?
SCANTBL dd	?
LINENUM dd      ?
LASTDIST dd	?
;LASTX   dd      ?
;LASTY   dd      ?
LASTEBP dd      ?
LASTEAX dd      ?
LASTEDX dd	?
WALLDIST dd	?

	.CODE
	IDEAL

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC xxxAckDrawFloor
	 push	 ebp
         push    esi
         push    edi
         push    ebx
         push    ecx
         push    edx

	 movzx	 eax,[word ptr _PlayerAngle]
         mov        ecx,eax
	 sub	    eax,INT_ANGLE_32
         jnc        short adf_20
         add        eax,INT_ANGLE_360

adf_20:
	 mov	    ebx,640
         cdq
    idiv    ebx
    mov     [BCOL],edx

    mov     eax,89
    sub     ax,[word ptr _ViewHeight]
    mov     [HEIGHT],eax

    sub     ecx,INT_ANGLE_32
    jnc     short adf_30
    add     ecx,INT_ANGLE_360

adf_30:
    mov     [VA],ecx
    movzx   eax,[word ptr _gWinHeight]
    sar     eax,1
    movzx   ebx,[word ptr _gWinEndY]
    sub     ebx,eax
	 inc	    ebx
    sub     ebx,5  ; 6
    mov     [EY],ebx
    mov     edi,[_gScrnBuffer]
    movzx   eax,[word ptr _gCenterOff]
    mov	    ebx,eax
    add     eax,1920
    add     eax,edi
    mov     [BFSCRN],eax
    sub     ebx,1600     ;1920
    add     ebx,edi
    mov     [BCSCRN],ebx
    mov     ebp,0
    mov     [LINENUM],ebp
    mov     [LASTDIST],ebp
    mov     ebx,[dword ptr _scantables]
    mov     [SCANTBL],ebx
    mov     ebx,[VA]

adf_loop:
    mov     eax,[_CosTable]
    shl     ebx,2
    mov	    eax,[eax+ebx]
    mov     [CV],eax
    mov	    eax,[_SinTable]
    mov     eax,[eax+ebx]
    mov     [SV],eax

    mov     eax,[dword ptr _WallDistTable+ebp*4]
    mov	    [WALLDIST],eax

    mov     eax,[BCSCRN]
    mov     [CSCRN],eax

    mov     ecx,[EY]
    mov     ebx,[BCOL]
         mov        eax,[_BackArray+ebx*4]
    add     eax,ecx
    mov     [BA],eax
    inc     ebx
    cmp     ebx,640
    jb      short adf_l10
    sub     ebx,ebx

adf_l10:
    mov     eax,[_BackArray+ebx*4]
    add	    eax,ecx
    mov     [BA1],eax
    inc	    ebx
    cmp     ebx,640
    jb      short adf_l20
    sub     ebx,ebx

adf_l20:
	 mov	    [BCOL],ebx
    lea	    esi,[offset _zdTable]
    mov	    ecx,[EY]	    ;Number of rows to draw
    imul    eax,ebp,800

    add     esi,eax
    add     esi,24 ;ebx
    mov     edi,[BFSCRN]
    push    ebp

adf_yloop:
    mov     edx,[esi]
    lea     esi,[esi+4]
    cmp     edx,[WALLDIST]
    jb      short adf_distokay
    lea     edi,[edi+320]
    jmp	    adf_ycont

adf_distokay:
    cmp     edx,[LASTDIST]
    jne     short adf_newdist
;   mov     eax,[LASTX]
;   mov     ebx,[LASTY]
    mov     ebp,[LASTEBP]
    mov     eax,[LASTEAX]
    mov     edx,[LASTEDX]
    jmp	    short adf_samedist

adf_newdist:
    mov     [LASTDIST],edx
    mov     eax,[CV]
    mov     ebx,[SV]
    imul    eax,edx
    imul    ebx,edx
    sar     eax,16
    sar     ebx,16
         mov        edx,[_xPglobal]
    add     eax,edx
    mov     edx,[_yPglobal]
    add     ebx,edx
;   mov	    [LASTX],eax
;   mov     [LASTY],ebx

;adf_samedist:
    mov     edx,ebx
    and     edx,0FC0h
    mov     ebp,eax
    sar     ebp,6
    add     ebp,edx             ;Pos within floor and ceiling maps

    and	    ebx,63
    shl     ebx,6
    and     eax,63
    add     eax,ebx             ;bitmap position

;;  mov     ebx,0

    shl     ebp,1
    mov     [LASTEBP],ebp
    mov     [LASTEAX],eax

    movzx   ebx,[word ptr _FloorMap+ebp]
    mov     edx,[_WallbMaps]
    mov     edx,[edx+ebx*4]
    movzx   edx,[byte ptr edx+eax]
    mov	    ebx,[SCANTBL]
    mov	    dl,[ebx+edx]
    mov	    dh,dl
    mov	    [LASTEDX],edx

adf_samedist:
    mov	    [edi],dx
	 lea	    edi,[edi+320]

    movzx   ebx,[word ptr _CeilMap+ebp]
    mov     ebp,[CSCRN]
    test    bx,bx
    jz      short adf_yback

    mov     edx,[_WallbMaps]
    mov     edx,[edx+ebx*4]
    movzx   eax,[byte ptr edx+eax]
    mov     edx,[SCANTBL]
    mov     al,[edx+eax]
    mov     ah,al
    mov     [ebp],ax

    mov     eax,[LINENUM]
    add	    eax,4
    mov     [LINENUM],eax
         mov        eax,[_scantables+eax]
    mov     [SCANTBL],eax
    lea     ebp,[ebp-320]
    mov     [CSCRN],ebp
    dec     [dword ptr BA]
    dec     [dword ptr BA1]
    dec     ecx
    jnz     adf_yloop

    jmp     short adf_ynext

adf_yback:
    mov     eax,[BA]
    mov     dl,[eax]
    mov     eax,[BA1]
    mov     dh,[eax]
    mov     [ebp],dx

adf_ycont:
    mov     eax,[LINENUM]
    add     eax,4
    mov     [LINENUM],eax
    mov	    eax,[_scantables+eax]
    mov     [SCANTBL],eax
    sub     [dword ptr CSCRN],320
    dec     [dword ptr BA]
    dec     [dword ptr BA1]
    dec     ecx
    jnz     adf_yloop

adf_ynext:
    mov     [dword ptr LINENUM],0
    mov     eax,[dword ptr _scantables]
    mov     [SCANTBL],eax
    mov     ebx,[VA]
    lea     ebx,[ebx+2]
         cmp        ebx,INT_ANGLE_360
    jb      short adf_l90
    sub     ebx,INT_ANGLE_360

adf_l90:
    mov     [VA],ebx                ;Note: EBX is used for VA at top of loop!
    add     [dword ptr BFSCRN],2
    add     [dword ptr BCSCRN],2
    pop     ebp
    lea     ebp,[ebp+2]
    cmp	    ebp,320
    jb      adf_loop


adf_exit:
    pop     edx
    pop     ecx
    pop     ebx
         pop        edi
    pop     esi
    pop     ebp
    ret
    endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC xxxAckDrawFloorOnly
	 push	 ebp
	 push	 esi
	 push	 edi
	 push	 ebx
	 push	 ecx
	 push	 edx

         movzx   eax,[word ptr _PlayerAngle]
         mov        ecx,eax
    sub     eax,INT_ANGLE_32
    jnc     short adfo_20
    add     eax,INT_ANGLE_360

adfo_20:
    mov     ebx,640
    cdq
    idiv    ebx
         mov        [BCOL],edx

    mov     eax,89
    sub     ax,[word ptr _ViewHeight]
    mov     [HEIGHT],eax

    sub     ecx,INT_ANGLE_32
    jnc	    short adfo_30
    add     ecx,INT_ANGLE_360

adfo_30:
    mov     [VA],ecx
    movzx   eax,[word ptr _gWinHeight]
    sar     eax,1
    movzx   ebx,[word ptr _gWinEndY]
    sub     ebx,eax
    inc     ebx
    sub     ebx,5  ; 6
         mov        [EY],ebx
    mov     edi,[_gScrnBuffer]
    movzx   eax,[word ptr _gCenterOff]
    mov     ebx,eax
    add     eax,1920
    add     eax,edi
    mov     [BFSCRN],eax
    sub     ebx,1600     ;1920
    add     ebx,edi
    mov     [BCSCRN],ebx
    mov     ebp,0
    mov     [LINENUM],ebp
    mov     ebx,[dword ptr _scantables]
    mov	    [SCANTBL],ebx
    mov     ebx,[VA]

adfo_loop:
    mov     eax,[_CosTable]
         shl        ebx,2
    mov     eax,[eax+ebx]
    mov     [CV],eax
    mov     eax,[_SinTable]
    mov     eax,[eax+ebx]
    mov     [SV],eax

    mov     eax,[BCSCRN]
    mov     [CSCRN],eax

    mov     ecx,[EY]
    mov     ebx,[BCOL]
    mov     eax,[_BackArray+ebx*4]
    add     eax,ecx
    mov     [BA],eax
    inc     ebx
    cmp     ebx,640
    jb      short adfo_l10
         sub        ebx,ebx

adfo_l10:
    mov     eax,[_BackArray+ebx*4]
    add     eax,ecx
    mov     [BA1],eax
    inc     ebx
    cmp     ebx,640
    jb      short adfo_l20
    sub     ebx,ebx

adfo_l20:
    mov     [BCOL],ebx
    lea     esi,[offset _zdTable]
    mov     ecx,[EY]        ;Number of rows to draw
    imul    eax,ebp,800

    add     esi,eax
         add        esi,24 ;ebx
    mov     edi,[BFSCRN]
    push    ebp

adfo_yloop:
    mov     edx,[esi]
    lea     esi,[esi+4]
    mov	    eax,[CV]
    mov     ebx,[SV]
    imul    eax,edx
    imul    ebx,edx
    sar     eax,16
    sar     ebx,16
    mov     edx,[_xPglobal]
    add     eax,edx
    mov     edx,[_yPglobal]
    add     ebx,edx

         mov        edx,ebx
    and     edx,0FC0h
    mov     ebp,eax
    sar     ebp,6
    add     ebp,edx             ;Pos within floor and ceiling maps

    and     ebx,63
    shl     ebx,6
    and     eax,63
    add     eax,ebx             ;bitmap position

    mov     ebx,0

    shl	    ebp,1
    mov     bx,[word ptr _FloorMap+ebp]
    mov     edx,[_WallbMaps]
    mov     edx,[edx+ebx*4]
    movzx   edx,[byte ptr edx+eax]
         mov        ebx,[SCANTBL]
    mov     dl,[ebx+edx]
    mov     dh,dl
    mov     [edi],dx
    lea     edi,[edi+320]

    mov     eax,[BA]
    mov     dl,[eax]
    mov     eax,[BA1]
    mov     dh,[eax]
    mov     ebp,[CSCRN]
    mov     [ebp],dx

adfo_ycont:
    lea     ebp,[ebp-320]
    mov     [CSCRN],ebp
    mov     eax,[LINENUM]
    add     eax,4
         mov        [LINENUM],eax
    mov	    eax,[_scantables+eax]
    mov     [SCANTBL],eax
    dec     [dword ptr BA]
    dec     [dword ptr BA1]
    dec     ecx
    jnz     adfo_yloop

adfo_ynext:
    mov     [dword ptr LINENUM],0
    mov     eax,[dword ptr _scantables]
    mov     [SCANTBL],eax
    mov     ebx,[VA]
    lea     ebx,[ebx+2]
    cmp     ebx,INT_ANGLE_360
    jb      short adfo_l90
    sub     ebx,INT_ANGLE_360

adfo_l90:
    mov     [VA],ebx                ;Note: EBX is used for VA at top of loop!
    add     [dword ptr BFSCRN],2
    add     [dword ptr BCSCRN],2
    pop     ebp
    lea     ebp,[ebp+2]
    cmp     ebp,320
    jb	    adfo_loop

adfo_exit:
    pop     edx
    pop     ecx
    pop     ebx
    pop     edi
    pop     esi
    pop     ebp
    ret
    endp


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC xxxAckDrawCeilingOnly
	 push	 ebp
	 push	 esi
	 push	 edi
	 push	 ebx
	 push	 ecx
	 push	 edx

	 movzx	 eax,[word ptr _PlayerAngle]
         mov        ecx,eax
         sub        eax,INT_ANGLE_32
         jnc        short adco_20
         add        eax,INT_ANGLE_360

adco_20:
         mov        ebx,640
         cdq
         idiv    ebx
         mov        [BCOL],edx

    mov     eax,89
    sub     ax,[word ptr _ViewHeight]
    mov     [HEIGHT],eax

    sub     ecx,INT_ANGLE_32
    jnc     short adco_30
    add     ecx,INT_ANGLE_360

adco_30:
    mov     [VA],ecx
    movzx   eax,[word ptr _gWinHeight]
    sar     eax,1
    movzx   ebx,[word ptr _gWinEndY]
    sub     ebx,eax
    inc     ebx
    sub     ebx,5  ; 6
    mov     [EY],ebx
         mov        edi,[_gScrnBuffer]
    movzx   eax,[word ptr _gCenterOff]
    mov     ebx,eax
    sub     ebx,1600     ;1920
    add     ebx,edi
    mov     [BCSCRN],ebx
    mov     ebp,0
    mov     [LINENUM],ebp
    mov     ebx,[dword ptr _scantables]
    mov     [SCANTBL],ebx
    mov     ebx,[VA]

adco_loop:
    mov     eax,[_CosTable]
    shl     ebx,2
    mov     eax,[eax+ebx]
    mov     [CV],eax
    mov     eax,[_SinTable]
         mov        eax,[eax+ebx]
    mov     [SV],eax

    mov     eax,[BCSCRN]
    mov     [CSCRN],eax

    mov     ecx,[EY]
    mov     ebx,[BCOL]
    mov     eax,[_BackArray+ebx*4]
    add     eax,ecx
    mov     [BA],eax
    inc     ebx
    cmp     ebx,640
    jb      short adco_l10
    sub     ebx,ebx

adco_l10:
    mov     eax,[_BackArray+ebx*4]
         add        eax,ecx
    mov     [BA1],eax
    inc     ebx
    cmp     ebx,640
    jb      short adco_l20
    sub     ebx,ebx

adco_l20:
    mov     [BCOL],ebx
    lea     esi,[offset _zdTable]
    mov     ecx,[EY]        ;Number of rows to draw
    imul    eax,ebp,800

    add     esi,eax
    add     esi,24 ;ebx
    push    ebp

adco_yloop:
         mov        edx,[esi]
    lea     esi,[esi+4]
    mov     eax,[CV]
    mov     ebx,[SV]
    imul    eax,edx
    imul    ebx,edx
    sar     eax,16
    sar     ebx,16
    mov     edx,[_xPglobal]
    add     eax,edx
    mov     edx,[_yPglobal]
    add     ebx,edx

    mov     edx,ebx
    and     edx,0FC0h
    mov     ebp,eax
    sar     ebp,6
    add     ebp,edx             ;Pos within floor and ceiling maps

    and     ebx,63
    shl     ebx,6
    and     eax,63
    add     eax,ebx             ;bitmap position

    mov     ebx,0

    shl     ebp,1
    movzx   ebx,[word ptr _CeilMap+ebp]
    mov     ebp,[CSCRN]
    test    bx,bx
    jz      short adco_yback

    mov     edx,[_WallbMaps]
    mov     edx,[edx+ebx*4]
    movzx   eax,[byte ptr edx+eax]
    mov     edx,[SCANTBL]
         mov        al,[edx+eax]
    mov     ah,al
    mov     [ebp],ax

    mov     eax,[LINENUM]
    add     eax,4
    mov     [LINENUM],eax
    mov     eax,[_scantables+eax]
    mov     [SCANTBL],eax
    lea     ebp,[ebp-320]
    mov     [CSCRN],ebp
    dec     [dword ptr BA]
    dec     [dword ptr BA1]
    dec     ecx
    jnz     adco_yloop

    jmp     short adco_ynext

adco_yback:
    mov     eax,[BA]
    mov     dl,[eax]
    mov     eax,[BA1]
    mov     dh,[eax]
    mov     [ebp],dx

adco_ycont:
    mov     eax,[LINENUM]
    add     eax,4
    mov     [LINENUM],eax
    mov     eax,[_scantables+eax]
    mov     [SCANTBL],eax
    sub     [dword ptr CSCRN],320
    dec     [dword ptr BA]
         dec        [dword ptr BA1]
    dec     ecx
    jnz     adco_yloop

adco_ynext:
    mov     [dword ptr LINENUM],0
    mov     eax,[dword ptr _scantables]
    mov     [SCANTBL],eax
    mov     ebx,[VA]
    lea     ebx,[ebx+2]
    cmp     ebx,INT_ANGLE_360
    jb      short adco_l90
    sub     ebx,INT_ANGLE_360

adco_l90:
    mov     [VA],ebx                ;Note: EBX is used for VA at top of loop!
    add     [dword ptr BCSCRN],2
    pop     ebp
         lea        ebp,[ebp+2]
    cmp     ebp,320
    jb      adco_loop


adco_exit:
    pop     edx
    pop     ecx
    pop     ebx
    pop     edi
    pop     esi
    pop     ebp
    ret
    endp


    end



        IDEAL
        JUMPS
        P386
        P387            ; Allow 386 processor


        MASM
        .MODEL FLAT     ;32-bit OS/2 model
        .CODE
        IDEAL

        include "ackrtn.inc"

        extrn   _gMultiWalls:word
        extrn   _WallDistTable:dword
        extrn   _BackDropRows:dword
        extrn   _FloorCeilRtn:dword
        extrn   _Resolution:word
        extrn   _ScreenOffset:word
        extrn   _bmDistance:dword
        extrn   _bmWall:dword
        extrn   _scPtr:dword
        extrn   _VidTop:dword
        extrn   _VidBottom:dword
        extrn   _Floors1:dword
        extrn   _Floors2:dword
        extrn   _PlayerAngle:word
        extrn   _BackArray:dword
        extrn   _gWinStartX:word
        extrn   _gWinStartY:word
        extrn   _gWinEndX:word
        extrn   _gWinEndY:word
        extrn   _gWinHeight:word
        extrn   _gWinWidth:word
        extrn   _gCenterRow:word
        extrn   _gCenterOff:word

        extrn   _scVid:dword
        extrn   _scWall:dword
        extrn   _scPal:dword
        extrn   _scdst:word
        extrn   _scwht:word
        extrn   _scmulti:word
        extrn   _sctopht:word
        extrn   _scbotht:word
        extrn   _scsavwht:word
        extrn   _scmulcnt:word
        extrn   _scsavVid:dword
        extrn   _scbNum:word
        extrn   _scMulData:dword
        extrn   _scColumn:dword
        extrn   _WallbMaps:dword

        extrn   _FloorMap:word
        extrn   _CeilMap:word
        extrn   _LastWallHeight:word
        extrn   _ViewAngle:word
        extrn   _ScreenOffset:word
        extrn   _xPglobal:dword
        extrn   _yPglobal:dword
        extrn   _xBegGlobal:dword
        extrn   _yBegGlobal:dword
        extrn   _aeGlobal:dword
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
        extrn   _ErrorCode:word
        extrn   _xMapPosn:dword
        extrn   _yMapPosn:dword
        extrn   _Grid:dword
        extrn   _ObjGrid:dword
        extrn   _ViewHeight:word
        extrn   _CeilingHeight:word
        extrn   _gTopColor:byte
        extrn   _gBottomColor:byte
        extrn   _PlayerAngle:word
        extrn   _gScrnBuffer:dword
        extrn   _gBkgdBuffer:dword
        extrn   _gCenterOff:word
        extrn   _gWinHeight:word
        extrn   _SysFlags:word


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
        extrn   _LongTanTable:dword
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
        extrn   _y_xPos:dword
        extrn   _y_yPos:dword
        extrn   _y_xNext:dword
        extrn   _y_yNext:dword

        extrn   _Slice:dword
        extrn   _sPtr:dword
        extrn   _HitMap:byte

        ACKEXT  xRaySetup
        ACKEXT  yRaySetup
        ACKEXT  BuildSlice
        ACKEXT  xRayCast
        ACKEXT  yRayCast
        ACKEXT  AckDrawFloor
        ACKEXT  AckDrawFloorOnly
        ACKEXT  AckDrawCeilingOnly
        ACKEXT  DrawWalls
        ACKEXT  CheckDoors
        ACKEXT  BuildSliceMulti
        ACKEXT  FindObject

        ACKPUBS xRayCastMulti
        ACKPUBS yRayCastMulti
        ACKPUBS ShowColLow
        ACKPUBS ShowColMaskLow
        ACKPUBS BuildUpView

align 2
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC ShowColLow
        push    ebp
        push    esi
        push    edi
        mov     ebp,eax
        mov     [_scPtr],eax
        mov     edi,[_scVid]
        mov     edx,[_scPal]
        mov     ax,[_scwht]
        mov     [_scsavwht],ax
        movsx   ecx,[_sctopht]
        mov     ebx,[_scWall]
        add     ebx,ecx
        mov     [_bmWall],ebx
        mov     ch,al
        inc     ch
        mov     eax,0
        mov     ebx,0

;--TEST------------------------------------------------------------------------
        test    [word ptr _scbNum],WALL_TYPE_UPPER
        jz      short toprun
        mov     ebp,[_bmDistance]

zztoploop:
        dec     ch
        jz      short zzdomulti
        add     bx,bp
        cmp     bh,cl
        jge     short zzdomulti
        sub     di,320
        jmp     zztoploop

zzdomulti:
        mov     ebp,[_scPtr]
        mov     [_scwht],cx
        mov     [_scsavVid],edi
        jmp     chkmulti

;--TEST------------------------------------------------------------------------
toprun:
        mov     ebp,[_bmDistance]

toploop:
        movsx   eax,bh
        mov     esi,[_bmWall]
        sub     esi,eax
        mov     al,[esi]
        mov     al,[edx+eax]
        mov     ah,al
        mov     [edi],ax
        add     bx,bp
        cmp     bh,cl
        jg      short botrun
        dec     ch
        jz      short botrun
        sub     edi,320
        jmp     toploop

botrun:
        mov     ebp,[_scPtr]
        mov     [_scwht],cx
        mov     [_scsavVid],edi
        mov     [_VidTop],edi
        mov     edi,[_scVid]
        mov     cx,[_scbotht]
        mov     bx,[_scsavwht]
        mov     ch,bl
        mov     ebx,0
        mov     esi,[_bmWall]
        inc     esi
        mov     ebp,[_bmDistance]
        dec     cl

botloop:
        add     edi,320
        movsx   ax,bh
        mov     al,[esi+eax]
        mov     al,[edx+eax]
        mov     ah,al
        mov     [edi],ax
        dec     ch
        jz      short chkmulti
        add     bx,bp
        cmp     bh,cl
        jl      botloop

chkmulti:
        mov     [_VidBottom],edi
        mov     ebp,[_scPtr]
        mov     edi,[_scsavVid]
        cmp     [word ptr _scmulti],0
        jz      alldone
        mov     cx,[_scmulcnt]

        mov     ebx,[_scMulData]        ;ptr to count and wall data
        mov     cl,[ebx]                ;get count of data
        inc     ebx
        mov     al,[ebx]                ;first wall to show
        inc     ebx
        mov     [_scMulData],ebx
        movsx   ebx,al                  ;get wall number
        shl     ebx,2                   ;x4 for index into wall bitmap array
        mov     eax,[_WallbMaps]
        mov     ebx,[eax+ebx]
        add     ebx,[dword ptr _scColumn]   ;add in current column
        add     ebx,63                  ;point to bottom of column
        mov     [_bmWall],ebx

        mov     bx,[_scwht]
        mov     ch,bh
        cmp     ch,0
        jz      short alldone

        mov     ebx,0
        mov     ebp,[_bmDistance]

mulloop:
        sub     edi,320
        movsx   eax,bh
        mov     esi,[_bmWall]
        sub     esi,eax
        mov     al,[esi]
        mov     al,[edx+eax]
        mov     ah,al
        mov     [edi],ax
        dec     ch
        jz      short alldone
        add     bx,bp
        cmp     bh,64
        jge     short nextlevel
        jmp     mulloop

nextlevel:
        dec     cl
        jz      short alldone
        mov     ebx,[_scMulData]
        mov     al,[ebx]            ;next wall number
        inc     ebx
        mov     [_scMulData],ebx
        movsx   ebx,al
        shl     ebx,2
        mov     eax,[_WallbMaps]
        mov     ebx,[eax+ebx]
        add     ebx,[dword ptr _scColumn]   ;add in current column
        add     ebx,63                  ;point to bottom of column
        mov     [_bmWall],ebx
        mov     ebx,0
        jmp     mulloop

alldone:
        mov     [_VidTop],edi
        pop     edi
        pop     esi
        pop     ebp
        ret
        endp


align 2
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC ShowColMaskLow
        push    ebp
        push    esi
        push    edi
        mov     ebp,eax
        mov     [_scPtr],eax
        mov     edi,[_scVid]
        mov     edx,[_scPal]
        mov     ax,[_scwht]
        mov     [_scsavwht],ax
        movsx   ecx,[_sctopht]
        mov     ebx,[_scWall]
        add     ebx,ecx
        mov     [_bmWall],ebx
        mov     ch,al
        mov     eax,0
        mov     ebx,0

;--TEST------------------------------------------------------------------------
        test    [word ptr _scbNum],WALL_TYPE_UPPER
        jz      short m_toprun
        mov     ebp,[_bmDistance]

m_zztoploop:
        dec     ch
        jz      short m_zzdomulti
        add     bx,bp
        cmp     bh,cl
        jge     short m_zzdomulti
        sub     edi,320
        jmp     m_zztoploop

m_zzdomulti:
        mov     ebp,[_scPtr]
        mov     [_scwht],cx
        mov     [_scsavVid],edi
        jmp     m_chkmulti

;--TEST------------------------------------------------------------------------
m_toprun:
        mov     ebp,[_bmDistance]

m_toploop:
        movsx   eax,bh
        mov     esi,[_bmWall]
        sub     esi,eax
        mov     al,[esi]
        or      al,al
        jz      m_blank1
        mov     al,[edx+eax]
        mov     ah,al
        mov     [edi],ax

m_blank1:
        add     bx,bp
        cmp     bh,cl
        jg      short m_botrun
        dec     ch
        jz      short m_botrun
        sub     edi,320
        jmp     m_toploop

m_botrun:
        mov     ebp,[_scPtr]
        mov     [_scwht],cx
        mov     [_scsavVid],edi
        mov     edi,[_scVid]
        mov     cx,[_scbotht]
        mov     bx,[_scsavwht]
        mov     ch,bl
        mov     ebx,0
        mov     esi,[_bmWall]
        inc     esi
        mov     ebp,[_bmDistance]

m_botloop:
        add     edi,320
        movsx   ax,bh
        mov     al,[esi+eax]
        or      al,al
        jz      m_blank2
        mov     al,[edx+eax]
        mov     ah,al
        mov     [edi],ax

m_blank2:
        dec     ch
        jz      short m_chkmulti
        add     bx,bp
        cmp     bh,cl
        jl      m_botloop

m_chkmulti:
        mov     ebp,[_scPtr]
        cmp     [word ptr _scmulti],0
        jz      short m_alldone
        mov     cx,[_scmulcnt]
        mov     bx,[_scwht]
        mov     ch,bh
        cmp     ch,0
        jz      short m_alldone

        mov     edi,[_scsavVid]
        mov     ebx,[_scWall]
        add     ebx,63
        mov     [_bmWall],ebx
        mov     ebx,0
        mov     ebp,[_bmDistance]

m_mulloop:
        sub     edi,320
        movsx   eax,bh
        mov     esi,[_bmWall]
        sub     esi,eax
        mov     al,[esi]
        or      al,al
        jz      m_blank3
        mov     al,[edx+eax]
        mov     ah,al
        mov     [edi],ax

m_blank3:
        dec     ch
        jz      short m_alldone
        add     bx,bp
        cmp     bh,64
        jge     short m_nextlevel
        jmp     m_mulloop

m_nextlevel:
        mov     ebx,0
        dec     cl
        jnz     m_mulloop

m_alldone:
        pop     edi
        pop     esi
        pop     ebp
        ret
        endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC xRayCastMulti
         push    ebp
         mov        ebp,esp
         sub        esp,50
         push    esi
         push    edi
         push    ebx
         push    ecx
    push    edx

    mov     [word ptr retval],0

looptop:

    mov     edx,[dword ptr _x_xPos]
    cmp     edx,large 0
    jl      short getout
    cmp     edx,large 4096
    jg      short getout
    mov     eax,[_x_yPos]
    cmp     eax,large 0
    jl      short getout
    cmp     eax,large 010000000h
    jle     short inbounds

getout:
    jmp     loopdone

inbounds:
         sar        eax,16
    and     ax,-64
         sar        edx,6
    add     ax,dx
    mov     si,ax

    shl     ax,1
    movsx   eax,ax
    mov     ebx,[dword ptr _ObjGrid]
    mov     ax,[word ptr ebx+eax]
    or      ax,ax
    jz      short no_obj

    and     ax,0FFh
    mov     [word ptr Color],ax

    movsx   ecx,[_FoundObjectCount]     ;// Get number of current objects seen
    mov     ebx,ecx
    jcxz    short nofound               ;// None found yet, add this new one
    mov     edi,offset _ObjectsSeen
    repne   scasb                       ;// See if this object already seen
    jz      short no_obj                ;// Yes, ignore this ray then

nofound:
    mov     edi,offset _ObjectsSeen
    mov     [edi+ebx],al
    inc     bx
    mov     [word ptr _FoundObjectCount],bx

no_obj:
         movsx   eax,si

         shl        eax,1
    mov     ebx,[dword ptr _xGridGlobal]
    mov     cx,[ebx+eax]
    or      cx,cx
    jz      short next_square
    test    cx,WALL_UPPER_MULTI
    jz      short next_square
    cmp     cl,[byte ptr _LastWallHeight]
    jbe     short next_square

    mov     [word ptr _xMapPosn],si
    mov     ebx,[dword ptr _x_xPos]
    mov     [dword ptr _iLastX],ebx
    mov     edx,[dword ptr _x_yPos]
    mov     [dword ptr _LastY1],edx

    movsx   eax,cx
    jmp     short xRayDone

next_square:
    mov     eax,[dword ptr _x_xNext]
    add     [dword ptr _x_xPos],eax
    mov     eax,[dword ptr _x_yNext]
    add     [dword ptr _x_yPos],eax
    jmp     looptop

loopdone:
    movsx   eax,[word ptr retval]

xRayDone:
         pop        edx
         pop        ecx
         pop        ebx
         pop        edi
         pop        esi
         mov        esp,ebp
         pop        ebp
         ret
         endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC yRayCastMulti
         push    ebp
         mov        ebp,esp
         sub        esp,50
         push    esi
         push    edi
         push    ebx
         push    ecx
         push    edx
         mov        [word ptr retval],0

y_looptop:
    mov     edx,[dword ptr _y_xPos]
    cmp     edx,large 0
         jl         short y_getout
    cmp     edx,large 010000000h
    jg      short y_getout
    mov     eax,[_y_yPos]
    cmp     eax,large 0
    jl      short y_getout
    cmp     eax,large 4096
    jle     short y_inbounds

y_getout:
    jmp     loopdone

y_inbounds:
    sar     edx,22
    and     ax,-64
    add     ax,dx
    mov     si,ax

    shl     ax,1
    movsx   eax,ax
    mov     ebx,[dword ptr _ObjGrid]
    mov     ax,[ebx+eax]
    or      ax,ax
    jz      short y_no_obj

    and     ax,0FFh
    mov     [word ptr Color],ax

    movsx   ecx,[_FoundObjectCount]
    mov     ebx,ecx
         jcxz    short y_nofound
    mov     edi,offset _ObjectsSeen
    repne   scasb
    jz      short y_no_obj

y_nofound:
    mov     edi,offset _ObjectsSeen
    mov     [edi+ebx],al
    inc     bx
    mov     [word ptr _FoundObjectCount],bx

y_no_obj:
    movsx   eax,si

    shl     eax,1
    mov     ebx,[dword ptr _yGridGlobal]
    mov     cx,[ebx+eax]
         or         cx,cx
    jz      short y_next_square

y_wall_here:
    test    cx,WALL_UPPER_MULTI
    jz      short y_next_square
    cmp     cl,[byte ptr _LastWallHeight]
    jbe     short y_next_square
    mov     [word ptr _yMapPosn],si
    mov     ebx,[dword ptr _y_xPos]
    mov     [dword ptr _LastX1],ebx
    mov     edx,[dword ptr _y_yPos]
    mov     [dword ptr _iLastY],edx
         mov        edx,ebx

    movsx   eax,cx
    jmp     short yRayDone

y_next_square:
    mov     eax,[dword ptr _y_xNext]
    add     [dword ptr _y_xPos],eax
    mov     eax,[dword ptr _y_yNext]
    add     [dword ptr _y_yPos],eax
    jmp     y_looptop

y_loopdone:
    movsx   eax,[word ptr retval]

yRayDone:
    pop     edx
         pop        ecx
         pop        ebx
         pop        edi
         pop        esi
         mov        esp,ebp
         pop        ebp
         ret
         endp


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC CheckHitMap
        mov     edi,offset _HitMap
        mov     edx,edi
        mov     ecx,4096
    mov esi,[dword ptr _ObjGrid]

chmLoop:
        xor     eax,eax
        repe    scasb
        jcxz    chmDone
        mov     ebx,edi     ;get current location
        sub     ebx,edx     ;minus start for actual offset
        dec     ebx         ;minus base 0 offset
        shl     ebx,1       ;times 2 for word array
        mov     ax,[word ptr esi+ebx]
        or      al,al       ;is there an object there?
        jz      chmLoop     ;nope, keep checking
        movzx   ebx,[word ptr _FoundObjectCount]
        mov     [byte ptr _ObjectsSeen+ebx],al
        inc     ebx
        mov     [word ptr _FoundObjectCount],bx
        jmp     chmLoop

chmDone:
        ret
        endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC BuildUpView
        push    ebp     ; Save registers used by this routine
        push    esi
        push    edi
        push    ebx
        push    ecx
        push    edx

        ACKCALL CheckDoors      ; Determine the state of the doors used in the view

        mov     edi,offset _HitMap      ; Access the hit map
        mov     ecx,1024                ; Size of the hit map
        xor     eax,eax                 ; Clear each location
        rep     stosd                   ; Clear out entire hit map

; Check to see if moveable objects are found in the view. If so, access the object list
; and store the objects.
        mov     [word ptr _MaxDistance],0
        movzx   ecx,[word ptr _MoveObjectCount] ; Get # of moveable objects
        mov     [_FoundObjectCount],cx          ; Store # of objects found in view
        jcxz    short buv010                    ; No objects are used, jump ahead
        mov     edi,offset _ObjectsSeen         ; Reference global object list
        mov     esi,offset _MoveObjectList      ; List of movable objects
        mov     ebx,ecx                         ; # of moveable objects
        sar     ecx,2                           ; Divide by 2
        rep     movsd
        mov     ecx,ebx                         ; # of moveable objects
        and     ecx,3
        rep     movsb                           ; Finish copying _ObjectSeen to
                                                ; _MoveObjectList
buv010:
        movzx   esi,[_gWinEndX]         ; Get right side of viewport
        cmp     si,320                  ; Is right coord. = 320?
        jae     short buv020
        inc     esi                     ; Adjust right viewport if not 320

buv020:
        movzx   eax,[_PlayerAngle]      ; Get current player’s angle
        sub     eax,INT_ANGLE_32        ; Check with 32 degree point
        jnc     short buv030            ; Skip if in range
        add     eax,INT_ANGLE_360       ; Add 360 degrees to angle

buv030:
        movzx   ebx,[_gWinStartX]       ; Get left side of viewport
        mov     [_ViewColumn],bx        ; Save location in _ViewColumn
        add     eax,ebx                 ; Add angle to left side
        cmp     eax,INT_ANGLE_360       ; Are we out of range?
        jl      short buv040            ; We’re ok!
        sub     eax,INT_ANGLE_360       ; Reduce angle by 360 degrees

buv040:
        mov     [_ViewAngle],ax         ; Store updated angle

; The start of the main loop that builds individual slices for the view.
; This loop continues until the entire view has been built. Each time through
; the loop, the player’s viewing angle is increased. This continues until a full
; 64 degre range is casted (assuming a full width screen is used).
buv050:
        movzx   ebx,[_ViewColumn]               ; Get left column position
        mov     [dword ptr _WallDistTable+ebx*4],4096   ; Max distance to walls
        mov     eax,offset _Slice               ; Access base adress of Slice structure
        imul    ebx,saSize                      ; Calculate offset into actual slice
        add     eax,ebx                         ; Add offset to base address
        mov     [_sPtr],eax                     ; Set up pointer to actual column slice
        movzx   edi,[_ViewAngle]                ; Use current viewing angle
        ACKCALL xRaySetup                       ; Set up x ray to start casting

buv060:
        ACKCALL yRaySetup                       ; Set up y ray to start casting

buv070:
        mov     [word ptr _LastWallHeight],0    ; For checking mult-height walls
        ACKCALL BuildSlice                      ; Build the current slice
        cmp     [word ptr _gMultiWalls],0       ; Are multi-height walls used?
        jz      short buv080                    ; Nope, no need to check

        cmp     [word ptr _LastWallHeight],200  ; No need to check if > 200
        jg      short buv080

        mov     eax,[_sPtr]                     ; Get base address of column slice 
        cmp     [dword ptr eax+saNext],0        ; Check for any more walls
        je      short buv080
        cmp     [word ptr eax+saDist],96        ; Distance from POV to slice is
        jle     short buv080                    ; less than or equal to 96

        ACKCALL BuildSliceMulti                 ; Build the current slice for
                                                ; a multi-height wall
buv080:
        movzx   eax,[_ViewColumn]               ; Get current column
        inc     eax                             ; Advance to the next column
        inc     edi                             ; Increment the casting angle
        cmp     [word ptr _Resolution],RES_LOW  ; Check for screen resolution
        jne     short buv090
        inc     edi                             ; Increment angle and position
        inc     eax                             ; for higher resolution casting

buv090:
        cmp     edi,INT_ANGLE_360               ; Did we go past 360 degrees?
        jl      short buv100                    ; We’re ok
        sub     edi,INT_ANGLE_360               ; Adjust angle for building next slice

buv100:
        mov     [_ViewAngle],di         ; Save current viewing angle
        mov     [_ViewColumn],ax        ; Save column position
        cmp     eax,esi                 ; Are we done yet?
        jl      buv050                  ; Nope; go build next slice

buv_exit:
        ACKCALL CheckHitMap
        ACKCALL FindObject              ; Update slice structures with objects found
        call    [dword ptr _FloorCeilRtn]       ; Build the floor and ceiling
        ACKCALL DrawWalls                       ; Build the walls

        pop     edx     ; Restore the registers used
        pop     ecx
        pop     ebx
        pop     edi
        pop     esi
        pop     ebp
        ret
        endp

        end


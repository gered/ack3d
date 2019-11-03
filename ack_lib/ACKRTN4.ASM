
IDEAL
JUMPS
P386
P387        ; Allow 386 processor

model flat

dataseg

SVTABLE dd  ?

SAVEVID dd  ?
SAVEROW dd  ?


codeseg


include "ackrtn.inc"

extrn   _BackDropRows:dword
extrn   _PlayerAngle:word
extrn   _BackArray:dword
extrn   _Resolution:word
extrn   _ScreenOffset:word
extrn   _bmDistance:dword
extrn   _bmWall:dword
extrn   _scPtr:dword
extrn   _VidTop:dword
extrn   _VidBottom:dword
extrn   _Floors1:dword
extrn   _Floors2:dword

extrn   _gPalTable:dword
extrn   _gWinStartX:word
extrn   _gWinStartY:word
extrn   _gWinEndX:word
extrn   _gWinEndY:word
extrn   _gWinHeight:word
extrn   _ViewHeight:word
extrn   _SysFlags:word
extrn   _Slice:byte
extrn   _gScrnBuffer:dword
extrn   _gCenterOff:word
extrn   _gCenterRow:word
extrn   _Floorscr:dword
extrn   _gWinStartOffset:dword

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
extrn   _LowerTable:dword
extrn   _gBottomOff:dword
extrn   _LightFlag:word

extrn  ShowCol_:near
extrn  ShowColMask_:near

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
extrn   _Floorwt:dword
extrn   _Floorvht:word
extrn   _Flooreht:word
extrn   _FloorMap:word
extrn   _gScrnBufferCenter:dword
extrn   _gWinHalfHeight:word
extrn   _zdTable:dword
extrn   _CosTable:dword
extrn   _SinTable:dword
extrn   _xPglobal:dword
extrn   _yPglobal:dword
extrn   _WallDistTable:dword
extrn   _CeilMap:word
extrn   _AckTimerCounter:dword

public Mymemset_
public AckSpeedUp_
public AckSlowDown_
public ShowColNS_
public ShowColMaskNS_
public DrawBackDrop_


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
proc Mymemset_ near
    push    edi
    push    ebx
    push    ecx
    push    edx
    mov     edi,eax
    mov     dh,dl
    mov     ax,dx
    shl     eax,16
    mov     ax,dx
    mov     ecx,ebx
    sar     ecx,2
    rep     stosd
    mov     ecx,ebx
    and     ecx,3
    rep     stosb
    pop     edx
    pop     ecx
    pop     ebx
    pop     edi
    ret
    endp


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; void AckSpeedUp(int rate)
;  eax: rate
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
proc AckSpeedUp_ near
     push    ebx
     push    edx
     mov        ebx,eax
    mov     ax,0FFFFH
    xor     dx,dx
    idiv    bx
    mov     bx,ax
    mov     dx,43h
    mov     al,36h
    out     dx,al
    mov     dx,40h
    mov     al,bl   ; ffh = original value
    out     dx,al
    mov     dx,40h
    mov     al,bh   ; 1fh = orignal value
    out     dx,al
    pop     edx
    pop     ebx
    ret
    endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; void AckSlowDown(void)
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
proc AckSlowDown_ near
     push    edx
     mov        dx,43h
     mov        al,36h
     out        dx,al
     mov        dx,40h
     mov        al,0ffh
     out        dx,al
     mov        dx,40h
     mov        al,0ffh
     out        dx,al
     pop        edx
     ret
     endp


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; void DrawBackDrop(void)
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
proc DrawBackDrop_ near
    push    ebp
    push    esi
    push    edi
    push    ebx
    push    ecx
    push    edx
    mov ax,[_PlayerAngle]
    sub ax,INT_ANGLE_32
    jnc ang_okay
    add ax,INT_ANGLE_360

ang_okay:
    mov cx,640
    cwd
    idiv    cx      ; Do mod 640 to get starting posn

;   movsx   ebx,dx
    movzx   ebx,dx
    shl ebx,2       ; x 4 for memory pointers
    mov edx,320
    mov ebp,[_gScrnBuffer]  ; get screen buffer

dbd010:
    mov edi,ebp
    mov esi,[dword ptr _BackArray+ebx]  ;current image pointer
    mov ecx,[_BackDropRows]    ; rows to draw

dbd020:
;   mov al,[esi]
;   mov [edi],al
;   inc esi
;   lea edi,[edi+320]
    movsb
    lea edi,[edi+319]
    dec ecx
    jnz dbd020

    inc ebp         ;next screen column
    lea ebx,[ebx+4]
    cmp ebx,2560        ;see if 640x4 column yet
    jb  short dbd030        ;nope
    mov ebx,0           ;else wrap to 0 column

dbd030:
    dec edx         ;see if done with all columns
    jnz dbd010

    pop edx
    pop ecx
    pop ebx
    pop edi
    pop esi
    pop ebp
    ret
    endp


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; void ShowColNS(void)
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
proc ShowColNS_ near
     push    ebp
    push    esi
    push    edi
     mov        edi,[_scVid]
    mov     ebx,[_scWall]
    mov     ebp,[_bmDistance]
    mov     cx,[_gCenterRow]
    mov     edx,1FFFh
    xor     eax,eax

sns_top:
    mov     al,dh
    sub     edx,ebp
    mov     al,[ebx+eax]
    mov     [edi],al
    jc      short sns_bot
    sub     edi,320
    dec     cx
    jns     sns_top
    xor     ecx,ecx

sns_bot:
    mov     [SAVEVID],edi
    mov     [SAVEROW],ecx

    mov     edi,[_scVid]
    mov     cx,[_gCenterRow]
    mov     edx,2000h

sns_botloop:
    mov     al,dh
    add     edi,320
    mov     al,[ebx+eax]
     add        edx,ebp
    mov     [edi],al
    cmp     dh,64
    jae     short sns_exit
    dec     cx
    jnz     sns_botloop

sns_exit:
    cmp     [word ptr _scmulti],0
    jz      sns_alldone
    mov     esi,[SAVEROW]
    or      si,si
    jz      sns_alldone

    mov     ebx,[_scMulData]        ;ptr to count and wall data
    mov     cl,[ebx]            ;get number of walls to draw
    inc     ebx
    mov     al,[ebx]            ;first wall to show
    inc     ebx
    mov     [_scMulData],ebx
    movzx   ebx,al          ;get wall number
    mov     eax,[_WallbMaps]        ;Get array of bitmaps
    mov     ebx,[eax+ebx*4]     ;Get the bitmap we are using
    mov     eax,[_scColumn]
    add     ebx,eax
    mov     edi,[SAVEVID]
     mov        edx,3FFFh
    mov     ebp,[_bmDistance]
    mov     eax,0

sns_mulloop:
    mov     al,dh
    sub     edi,320
    mov     al,[ebx+eax]
    dec     si
    mov     [edi],al
    jz      short sns_alldone
    sub     edx,ebp
    jnc     sns_mulloop

sns_nextlevel:
    dec     cl              ;Bump wall count
    jz      short sns_alldone       ;br if no more walls
    mov     ebx,[_scMulData]        ;Get pointer to the multi-ht data
    movzx   edx,[byte ptr ebx]      ;next wall number
    inc     ebx             ;Advance for next wall
    mov     [_scMulData],ebx
    mov     ebx,[_WallbMaps]        ;Get wall array
    mov     ebx,[ebx+edx*4]     ;Get wall bitmap to use
    add     ebx,[dword ptr _scColumn]   ;add in current column
    mov     edx,3FFFh
    jmp     sns_mulloop

sns_alldone:

    pop     edi
     pop        esi
     pop        ebp
     ret
     endp


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; void ShowColMaskNS(void)
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
proc ShowColMaskNS_ near
     push    ebp
     push    esi
     push    edi
     mov        edi,[_scVid]
     mov        ebx,[_scWall]
     mov        ebp,[_bmDistance]
     mov        cx,[_gCenterRow]
    mov     edx,1FFFh
    xor     eax,eax

smns_top:
    mov     al,dh
    mov     al,[ebx+eax]
    or      al,al
    jz      short smns_blank
    mov     [edi],al

smns_blank:
    sub     edx,ebp
    jc      short smns_bot
    sub     edi,320
    dec     cx
    jnz     smns_top

smns_bot:
    mov     edi,[_scVid]
    mov     cx,[_gCenterRow]
    mov     edx,2000h

smns_botloop:
    add     edi,320
    mov     al,dh
    mov     al,[ebx+eax]
    or      al,al
    jz      short smns_blank1
    mov     [edi],al

smns_blank1:
    add     edx,ebp
    cmp     dh,64
    jae     short smns_exit
    dec     cx
    jnz     smns_botloop

smns_exit:
    pop     edi
    pop     esi
    pop     ebp
    ret
    endp

    end


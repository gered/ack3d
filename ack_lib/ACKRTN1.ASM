
	IDEAL
	JUMPS
	P386
	P387


	MASM
	.MODEL FLAT

	.DATA

SVTABLE dd  ?
ENDPOS	dd  ?


	.CODE
	IDEAL


	include "ackrtn.inc"

	extrn	_VidSeg:dword
	extrn	_Resolution:word
	extrn	_ScreenOffset:word
	extrn	_bmDistance:dword
	extrn	_bmWall:dword
	extrn	_scPtr:dword
	extrn	_VidTop:dword
	extrn	_VidBottom:dword
	extrn	_Floors1:dword
	extrn	_Floors2:dword

	extrn	_gPalTable:dword
	extrn	_gWinStartX:word
	extrn	_gWinStartY:word
	extrn	_gWinEndX:word
	extrn	_gWinEndY:word
	extrn	_gWinHeight:word
	extrn	_ViewHeight:word
	extrn	_SysFlags:word
	extrn	_Slice:byte
	extrn	_gScrnBuffer:dword
	extrn	_gCenterOff:word
	extrn	_Floorht:word
	extrn	_Floorscr:dword
	extrn	_gWinStartOffset:dword

	extrn	_scVid:dword
	extrn	_scWall:dword
	extrn	_scPal:dword
	extrn	_scdst:word
	extrn	_scwht:word
	extrn	_scmulti:word
	extrn	_sctopht:word
	extrn	_scbotht:word
	extrn	_scsavwht:word
	extrn	_scmulcnt:word
	extrn	_scsavVid:dword
	extrn	_scbNum:word
	extrn	_scMulData:dword
	extrn	_scColumn:dword
	extrn	_WallbMaps:dword
	extrn	_LowerTable:dword
	extrn	_gBottomOff:dword

	ACKEXT	ShowColLow
	ACKEXT	ShowColMaskLow

	ACKPUBS ShowCol
	ACKPUBS ShowColMask
	ACKPUBS DrawWalls


align 2
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC ShowCol
	push	ebp
	push	esi
	push	edi

	mov	edi,[_scVid]		    ;Video buffer position
	mov	ebp,[_scPal]		    ;Pointer to palette for shading

	mov	ax,[_scwht]		    ;Height to use
	mov	[_scsavwht],ax		    ;save for later

	movzx	ecx,[_sctopht]
	mov	ebx,[_scWall]
	add	ebx,ecx
	mov	[_bmWall],ebx
	mov	ch,al
	inc	ch

	mov	esi,[_bmDistance]
	mov	esi,[_LowerTable+esi*4]
	mov	[SVTABLE],esi

	mov	eax,0
	mov	ebx,0

	test	[word ptr _scbNum],WALL_TYPE_UPPER
	jz	short toprun
	mov	edx,[_bmDistance]

zztoploop:
	dec	ch
	jz	short zzdomulti
	sub	di,320
	add	bx,dx
	cmp	bh,cl
	jge	short zzdomulti
	jmp	zztoploop

zzdomulti:
	mov	[_scwht],cx
	mov	[_scsavVid],edi
	jmp	chkmulti

toprun:
	mov	ebx,[_bmWall]

toploop:
	movzx	edx,[word ptr esi]
	cmp	dl,cl
	jg	short botrun
	lea	esi,[esi+2]
	neg	edx
	mov	al,[ebx+edx]
	mov	al,[ebp+eax]
	mov	[edi],al
	add	edi,-320
	dec	ch
	jnz	toploop

botrun:
	mov	[_scwht],cx
	mov	[_scsavVid],edi
	mov	edi,[_scVid]
	mov	cx,[_scbotht]
	mov	dx,[_scsavwht]
	mov	ch,dl
	mov	esi,[SVTABLE]
	inc	ebx
	dec	cl

botloop:
	lea	edi,[edi+320]
	movzx	edx,[word ptr esi]
	cmp	dl,cl
	jge	short chkmulti
	lea	esi,[esi+2]
	mov	al,[ebx+edx]
	mov	al,[ebp+eax]
	mov	[edi],al
	dec	ch
	jnz	botloop

chkmulti:
	mov	edi,[_scsavVid]
	cmp	[word ptr _scmulti],0
	jz	alldone
	mov	cx,[_scmulcnt]

	mov	ebx,[_scMulData]	;ptr to count and wall data
	mov	cl,[ebx]		;get number of walls to draw
	inc	ebx
	mov	al,[ebx]		;first wall to show
	inc	ebx
	mov	[_scMulData],ebx
	movzx	ebx,al			;get wall number
	mov	esi,[_scColumn]		;Current bitmap column to display x 64
	lea	esi,[esi+63]
	mov	[_scColumn],esi		;save for later use
	mov	eax,[_WallbMaps]	;Get array of bitmaps
	mov	ebx,[eax+ebx*4]		;Get the bitmap we are using
	add	ebx,esi			;point to bottom of column

	mov	ax,[_scwht]		;Get height of window
	mov	ch,ah
	cmp	ch,0			;Is there more room to draw?
	jz	short alldone		;br if at top of window

	mov	esi,[SVTABLE]
	mov	eax,0

mulloop:
	movzx	eax,[word ptr esi]	;Get height displacement for this row
	cmp	al,64			;Did we do the entire column?
	jge	short nextlevel		;Yes, see if more walls
	lea	esi,[esi+2]
	neg	eax			;Invert so we can add it below
	movzx	eax,[byte ptr ebx+eax]	;Get the pixel from the bitmap
	mov	al,[ebp+eax]		;Map it to the palette for shading
	mov	[edi],al		;Place it into the video buffer
	dec	ch			;Bump the window height
	jz	short alldone		;br if at the top of the window
	sub	edi,320			;next video row
	jmp	mulloop

nextlevel:
	dec	cl			;Bump wall count
	jz	short alldone		;br if no more walls
	mov	ebx,[_scMulData]	;Get pointer to the multi-ht data
	movzx	eax,[byte ptr ebx]	;next wall number
	inc	ebx			;Advance for next wall
	mov	[_scMulData],ebx
	mov	ebx,[_WallbMaps]	;Get wall array
	mov	ebx,[ebx+eax*4]		;Get wall bitmap to use
	add	ebx,[dword ptr _scColumn]   ;add in current column
	mov	eax,0
	mov	esi,[SVTABLE]
	jmp	mulloop

alldone:
	pop	edi
	pop	esi
	pop	ebp
	ret
	endp


align 2
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC ShowColMask
	push	ebp
	push	esi
	push	edi
	mov	ebp,eax
	mov	[_scPtr],eax
	mov	edi,[_scVid]
	mov	edx,[_scPal]
	mov	ax,[_scwht]
	mov	[_scsavwht],ax
	movzx	ecx,[_sctopht]
	mov	ebx,[_scWall]
	add	ebx,ecx
	mov	[_bmWall],ebx
	mov	ch,al
	inc	ch
	mov	eax,0
	mov	ebx,0

;--TEST------------------------------------------------------------------------
	test	[word ptr _scbNum],WALL_TYPE_UPPER
	jz	short m_toprun
	mov	ebp,[_bmDistance]

m_zztoploop:
	dec	ch
	jz	short m_zzdomulti
	add	bx,bp
	cmp	bh,cl
	jge	short m_zzdomulti
	sub	edi,320
	jmp	m_zztoploop

m_zzdomulti:
	mov	ebp,[_scPtr]
	mov	[_scwht],cx
	mov	[_scsavVid],edi
	jmp	m_chkmulti

;--TEST------------------------------------------------------------------------
m_toprun:
	mov	ebp,[_bmDistance]
	mov	esi,[_bmWall]

m_toploop:
	movzx	eax,bh
	neg	eax
	movzx	eax,[byte ptr esi+eax]
	or	al,al
	jz	short m_blank1
	mov	al,[edx+eax]
	mov	[edi],al

m_blank1:
	add	bx,bp
	cmp	bh,cl
	jg	short m_botrun
	dec	ch
	jz	short m_botrun
	sub	edi,320
	jmp	m_toploop

m_botrun:
	mov	ebp,[_scPtr]
	mov	[_scwht],cx
	mov	[_scsavVid],edi
	mov	edi,[_scVid]
	mov	cx,[_scbotht]
	mov	bx,[_scsavwht]
	mov	ch,bl
	mov	ebx,0
	inc	esi
	mov	ebp,[_bmDistance]
	mov	eax,0
	dec	cl

m_botloop:
	add	edi,320
	mov	al,bh
	mov	al,[esi+eax]
	or	al,al
	jz	short m_blank2
	mov	al,[edx+eax]
	mov	[edi],al

m_blank2:
	dec	ch
	jz	short m_chkmulti
	add	bx,bp
	cmp	bh,cl
	jl	m_botloop

m_chkmulti:
	mov	ebp,[_scPtr]
	cmp	[word ptr _scmulti],0
	jz	short m_alldone
	mov	cx,[_scmulcnt]
	mov	bx,[_scwht]
	mov	ch,bh
	cmp	ch,0
	jz	short m_alldone

	mov	edi,[_scsavVid]
	mov	ebx,[_scWall]
	add	ebx,63
	mov	[_bmWall],ebx
	mov	ebx,0
	mov	ebp,[_bmDistance]
	mov	esi,[_bmWall]

m_mulloop:
	sub	edi,320
	movzx	eax,bh
	neg	eax
	movzx	eax,[byte ptr esi+eax]
	or	al,al
	jz	m_blank3
	mov	al,[edx+eax]
	mov	[edi],al

m_blank3:
	dec	ch
	jz	short m_alldone
	add	bx,bp
	cmp	bh,64
	jge	short m_nextlevel
	jmp	m_mulloop

m_nextlevel:
	mov	ebx,0
	dec	cl
	jnz	m_mulloop

m_alldone:
	pop	edi
	pop	esi
	pop	ebp
	ret
	endp


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; This routine runs through the list of slices and draws the walls.
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC DrawWalls
	 push	 ebp
	 push	 esi
	 push	 edi

	 mov	    ax,[_gWinHeight]
	 shr	    ax,1
	 mov	    [_Floorht],ax
	 mov	    ax,[_ViewHeight]
	 mov	    [_sctopht],ax
	 mov	    dx,64
	 sub	    dx,ax
	 mov	    [_scbotht],dx
	 movsx	 ebx,[word ptr _gWinStartX]
	 movsx	 ecx,[_gCenterOff]
	 add	    ecx,ebx
	 add	    ecx,[_gScrnBuffer]
	 mov	    [_scVid],ecx
	 cmp	    [word ptr _Resolution],1	    ;is this low resolution?
	 je	    stw_lowres			    ;yes, perform faster draw

	 movzx	 eax,[_gWinEndX]
    imul    eax,saSize
    mov	    [ENDPOS],eax
    imul    ebx,saSize

stw010:
    mov	    ebp,offset _Slice		    ; offset to slices array
    add	    ebp,ebx

stw020:
    cmp	    [byte ptr ebp+saActive],0	    ;is this an active slice?
    jz	    short stw030		    ;nope, so it's the last one
    cmp	    [dword ptr ebp+saNext],0	    ;is this the last slice?
    jz	    short stw030		    ;yes, so can't go further
    mov	    ebp,[dword ptr ebp+saNext]	    ;point to next slice
	 jmp	    short stw020

stw030:
    mov	    ax,[word ptr ebp+sabNumber]	    ;bitmap number
    mov	    cx,ax
    and	    cx,0ffh			    ;isolate number from flags
    mov	    [_scmulcnt],cx		    ;save as current multi count
    mov	    [_scbNum],ax		    ;and bitmap number and flags

    mov	    cx,0			    ;set multi-flag to none
    test    ax,WALL_TYPE_MULTI
    jz	    short stw040
    mov	    esi,[dword ptr ebp+samPtr]
    mov	    [_scMulData],esi
    or	    esi,esi			    ;is there multi-ht data?
    jz	    short stw040		    ;br if not
    mov	    cx,1			    ;set multi-flag to yes

stw040:
    mov	    [_scmulti],cx
    mov	    ecx,[dword ptr ebp+sabMap]	;get pointer to bitmaps
    movzx   esi,al			;get low byte of bitmap number
    shl	    esi,2

    mov	    esi,[dword ptr ecx+esi]	;get actual bitmap pointer
    movsx   ecx,[word ptr ebp+saDist]	;distance to slice
    mov	    [_bmDistance],ecx		;save for draw routine
    shr	    ecx,6			 ;bring it down into palette range
    cmp	    ecx,15			 ;check against max palette number
    jbe	    short stw050
    mov	    ecx,15			 ;force to max if above

stw050:
    shl	    ecx,8			 ;x256 for palette entry
    mov	    edi,[_gPalTable]		;pointer to palette table
    add	    edi,ecx
	 movsx	 ecx,[word ptr ebp+sabColumn] ;column of bitmap slice
    shl	    ecx,6			;x64 to get correct row (column)
    add	    esi,ecx			;adjust wall point to correct column
    mov	    [_scColumn],ecx		;save for multi-ht walls

    mov	    [_scWall],esi
    mov	    [_scPal],edi
    mov	    eax,0
    mov	    [_scsavVid],eax		;null out saved video

    mov	    ax,[_Floorht]		;window height / 2 saved earlier
    mov	    [_scwht],ax

    mov	    esi,ebx
    call    [dword ptr ebp+saFnc]
	 mov	    ebx,esi

stw075:
    cmp	    [dword ptr ebp+saPrev],0	;is this the first slice?
    jz	    short stw080		;yes, go to next column
    mov	    ebp,[dword ptr ebp+saPrev]	;pick up previous slice
    jmp	    stw030			;and start again with same column


stw080:
    inc	    [dword ptr _scVid]		;next video position
    add	    ebx,saSize
    cmp	    ebx,[ENDPOS]
    ja	    stw_getout			;yes, get out
    jmp	    stw010

stw_lowres:
    mov	    eax,ebx			    ; current column of display
    imul    eax,saSize			    ; size of slice structure
    mov	    ebp,offset _Slice		    ; offset to slices
    add	    ebp,eax

stwlr020:
    cmp	    [byte ptr ebp+saActive],0	    ;is this an active slice?
    jz	    short stwlr030		    ;nope, so it's the last one
    cmp	    [dword ptr ebp+saNext],0	    ;is this the last slice?
    jz	    short stwlr030		    ;yes, so can't go further
    mov	    ebp,[dword ptr ebp+saNext]	    ;point to next slice
    jmp	    short stwlr020

stwlr030:
    mov	    ax,[word ptr ebp+sabNumber]	    ;bitmap number
    mov	    cx,ax
    and	    cx,0ffh			    ;isolate number from flags
	 mov	    [_scmulcnt],cx		    ;save as current multi count
    mov	    [_scbNum],ax		    ;and bitmap number and flags

    mov	    cx,0			    ;set multi-flag to none
    test    ax,WALL_TYPE_MULTI
    jz	    short stwlr040

    mov	    esi,[dword ptr ebp+samPtr]
    mov	    [_scMulData],esi
    or	    esi,esi			    ;is there multi-height data?
    jz	    short stwlr040		    ;br if not
    mov	    cx,1			    ;set multi-flag to yes

stwlr040:
    mov	    [_scmulti],cx
    mov	    ecx,[dword ptr ebp+sabMap]	;get pointer to bitmaps
    movzx   esi,al			;get low byte of bitmap number
    shl	    esi,2
	 mov	    esi,[dword ptr ecx+esi]	;get actual bitmap pointer
    movsx   ecx,[word ptr ebp+saDist]	;distance to slice
    mov	    [_bmDistance],ecx		 ;save for draw routine
    shr	    ecx,6			 ;bring it down into palette range
    cmp	    ecx,15			 ;check against max palette number
    jbe	    short stwlr050
    mov	    ecx,15			 ;force to max if above

stwlr050:
    shl	    ecx,8			 ;x256 for palette entry
    mov	    edi,[_gPalTable]		;pointer to palette table
    add	    edi,ecx
    movsx   ecx,[byte ptr ebp+sabColumn] ;column of bitmap slice
    shl	    ecx,6			;x64 to get correct row (column)
    add	    esi,ecx			;adjust wall point to correct column
    mov	    [_scColumn],ecx		;save for multi-ht walls

    mov	    [_scWall],esi
	 mov	    [_scPal],edi
    mov	    eax,0
    mov	    [_scsavVid],eax		;null out saved video

    mov	    ax,[_Floorht]		;window height / 2 saved earlier
    mov	    [_scwht],ax

    mov	    esi,ebx
    cmp	    [byte ptr ebp+saType],ST_WALL   ;transparent wall?
    je	    short stwlr060		    ;nope, use solid slice routine
	 ACKCALL	ShowColMaskLow
	 jmp	    short stwlr070

stwlr060:
	 ACKCALL	ShowColLow

stwlr070:
    mov	    ebx,esi
	 cmp	    [dword ptr ebp+saPrev],0	;is this the first slice?
    jz	    short stwlr080		  ;yes, go to next column
    mov	    ebp,[dword ptr ebp+saPrev]	;pick up previous slice
    jmp	    stwlr030			  ;and start again with same column


stwlr080:
    add	    [dword ptr _scVid],2	;next video position
    inc	    ebx				;next column
    inc	    ebx				;next column
    cmp	    bx,[word ptr _gWinEndX]	;are we at the end of the window?
    ja	    short stw_getout		      ;yes, get out
    jmp	    stw_lowres

stw_getout:
    pop	    edi
	 pop	    esi
	 pop	    ebp
	 ret
	 endp

	 end


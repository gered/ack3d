
    IDEAL
    JUMPS
    include "prologue.mac"
    P386        ; 386 specific opcodes and shit allowed.
    P387        ; Allow 386 processor


    MASM
    .MODEL FLAT ;32-bit OS/2 model
    .CODE
    IDEAL

    PUBLIC      SetPalette2_
    PUBLIC      SetVGAmode_
    PUBLIC      SetTextMode_
    PUBLIC      inkey_
    PUBLIC      PutHex_

;==============================================================================
; void SetPalette2(unsigned char *PalBuf,short count);
;==============================================================================

Proc    SetPalette2_ near
    push    esi

    mov esi,eax
    mov cx,dx
    mov bx,0
    cld
    mov dx,3C8H
sp210:
    mov al,bl
    out dx,al
    inc dx
    lodsb
    out dx,al
    lodsb
    out dx,al
    lodsb
    out dx,al
    dec dx
    inc bx
    loop    sp210

    pop esi
    ret
    endp


;==============================================================================
; void SetVGAmode(void);
;==============================================================================
Proc    SetVGAmode_  near
    push    ebp
    mov ax,13h
    int 10h     ; Set 320x200x256
    pop ebp
    ret
    endp

;==============================================================================
;
;==============================================================================
Proc    SetTextMode_ near
    push    ebp
    mov ax,3
    int 10h
    pop ebp
    ret
    endp

;==============================================================================
;
;==============================================================================
Proc  inkey_ near
    xor eax,eax
    mov ah,1        ;see if key available
    int 16h
    jz  ink080      ;nope
    xor ax,ax
    int 16h
    jmp short ink090

ink080:
    xor ax,ax
ink090:
    ret
    endp

;==============================================================================
;
;==============================================================================
Proc    HexOut_ near
    and al,15
    cmp al,10
    jb  short hex010
    add al,7

hex010:
    add al,'0'
    stosb
    ret
    endp

;==============================================================================
; void PutHex(char *buf,UINT mCode);
;==============================================================================
Proc    PutHex_ near
    push    edi
    mov edi,eax
    mov eax,edx
    shr al,4
    call    HexOut_
    mov eax,edx
    call    HexOut_
    xor al,al
    stosb
    pop edi
    ret
    endp
    end


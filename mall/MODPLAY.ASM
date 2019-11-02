;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
;        Tiny MOD Player for Watcom C/C++32 and DOS/4GW
;                      Version 2.02a  May 09th, 1994
;
;                      Copyright 1993,94 Carlos Hasan
;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²

ideal
p386
model   flat,c
jumps

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; PUBLICS
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

global  MODPlayModule:near
global  MODStopModule:near
global  MODPlaySample:near
global  MODPlayVoice:near
global  MODStopVoice:near
global  MODSetPeriod:near
global  MODSetVolume:near
global  _GETDS:near

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; EQUATES
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

MAXVOICES       =  8                    ; Max number of voices
MIDCRATE        =  8363                 ; C-2 frequency rate
BUFLEN          =  2048                 ; DMA Buffer Length

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; MOD file structures
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

struc   MODSample                       ; sample header
SampleName      db      22 dup (?)
SampleLength    dw      ?
Finetune        db      ?
Volume          db      ?
RepStart        dw      ?
RepLength       dw      ?
ends    MODSample

struc   MODHeader                       ; file header
SongName    db  20 dup (?)
Samples         MODSample 31 dup (?)
OrderLen        db      ?
ReStart     db  ?
Order       db  128 dup (?)
Sign        dd  ?
ends    MODHeader

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; MOD Player structures
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

struc   Voice
Address     dd  ?,?     ; Current Address
StartAddress    dd  ?       ; Loop Start Address
EndAddress  dd  ?       ; Loop End Address
Frequency   dd  ?       ; Frequency
Volume      db  ?,?     ; Volume
ends    Voice

struc   Track
Period      dw  ?       ; Note Period
Instr       db  ?       ; Instr Number
Volume      db  ?       ; Volume
Effect      dw  ?       ; Effect
ends    Track

struc   Sample
Period          dw      ?               ; Default Period
Volume          dw      ?               ; Default Volume
SampleLength    dd      ?               ; Sample Length
SampleData      dd      ?               ; Sample 8-bit signed Data
ends    Sample

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; DATA
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
dataseg

IOAddr          dw      ?               ; SB I/O Port Address
IRQNum          db      ?               ; SB IRQ Level
DRQNum          db      ?               ; SB DRQ Channel
SBRate          dw      ?               ; SB Sampling Rate

IRQVector       df      ?               ; IRQ Saved Vector

BufSelector     dw      ?               ; DPMI Buffer Selector
VolBuffer       dd      ?               ; Volume Buffer Address
DMABuffer   dd  ?       ; DMA Buffer Address
DMALength       dw      ?               ; DMA Buffer Length

BufferPos       dw      ?               ; Current Buffer Position
BufferLen       dw      ?               ; Current Buffer Length
TimerCount      dw      ?               ; Timer Accumulator
TimerSpeed      dw      ?               ; Timer Speed Value
FreqDivisor     dd      ?               ; Frequency Divisor

ActiveVoices    dw      ?               ; Number of Active Voices
Voices          Voice MAXVOICES dup (?) ; Array of Voices

                label VoiceTable dword  ; Lookup Table
                I=0
                rept    MAXVOICES
                dd      (offset Voices)+I*(size Voice)
                I=I+1
                endm

OrderPos        db      ?               ; Order Position
OrderLen        db      ?               ; Order Length
PatternRow      db      ?               ; Pattern Row
OrderList       db      128 dup (?)     ; Order List
PatternPtr      dd      ?               ; Current Pattern Address
Tempo       db  ?       ; Tempo
TempoCount      db      ?               ; Tempo Internal Counter
BPM     db  ?       ; BPM value

ActiveTracks    dw      ?               ; Number of Active Tracks
Tracks          Track MAXVOICES dup (?) ; Array of Tracks

PatternList     dd      128 dup (?)     ; Pattern List of Pointers
SmpAddress      dd      32 dup (?)      ; Samples Addresses and Volume
SmpStartAddress dd      32 dup (?)
SmpEndAddress   dd      32 dup (?)
SmpVolume       db      32 dup (?)

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; CODE
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
codeseg

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
; MOD Player Stuff
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MODPlayModule - Start playing a modulefile
; In:
;  Song  = Modulefile address
;  Chans = Number of channels
;  Rate  = Sampling Rate
;  Port  = I/O Port
;  IRQ   = IRQ Level
;  DRQ   = DRQ Channel
; Out:
;  EAX   = status
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc    MODPlayModule Song:dword,Chans:dword,Rate:dword,Port:dword,IRQ:dword,DRQ:dword
    pushad
    mov edi,[Song]      ; Load parameters from stack
    mov eax,[Chans]
    mov [ActiveVoices],ax
    mov eax,[Rate]
    mov [SBRate],ax
    mov eax,[Port]
    mov [IOAddr],ax
    mov eax,[IRQ]
    mov [IRQNum],al
    mov eax,[DRQ]
    mov [DRQNum],al
    mov eax,MIDCRATE*428    ; setup frequency divisor
    xor edx,edx
    shld    edx,eax,16
    shl eax,16
    movzx   ebx,[SBRate]
    div ebx
    mov [FreqDivisor],eax
    call    MODSetup        ; setup MOD player stuff
    jc  MODPlayDone
    call    MCInit          ; setup the muti-channel stuff
    jc  MODPlayDone
    mov dl,[BPM]        ; setup the multi-channel timer
    call    MCStartTimer
    call    DMASetBuf       ; setup DMA controller
    call    IRQSetVect      ; setup PIC interrupt
    call    SBOpenDevice        ; setup SB for playback
    jnc MODPlayDone
    call    MODStopModule
MODPlayDone:
    popad
    sbb eax,eax         ; return status in EAX
    ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MODStopModule - Stop playing the current modulefile
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc    MODStopModule
    pushad
    call    SBCloseDevice       ; done SB playback
    call    IRQRestVect     ; done PIC interrupt
    call    DMAReset        ; done DMA controller
    call    MCDone          ; done multi-channel stuff
    popad
    ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MODPlaySample - Play instrument sample
; In:
;  Voice  = Audio channel
;  Instr  = Instrument
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc    MODPlaySample AVoice:dword,AInstr:dword
    pushad
    mov ebx,[AVoice]        ; change the voice sample parameters
    mov eax,[AInstr]
    mov edx,[eax+Sample.SampleData]
    mov esi,[eax+Sample.SampleLength]
    add esi,edx
    mov edi,esi
    call    MCStartVoice
    movzx   ecx,[eax+Sample.Period] ; change the voice frequency
    call    MCSetFrequency
    mov ax,[eax+Sample.Volume]  ; and set the voice volume
    call    MCSetVolume
    popad
    ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MODPlayVoice - Set the voice instrument, period and volume
; In:
;  Voice  = Audio channel
;  Instr  = Instrument Number
;  Period = Amiga Period
;  Volume = Volume
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc    MODPlayVoice AVoice:dword,AInstr:dword,APeriod:dword,AVolume:dword
    pushad
    mov ebx,[AVoice]        ; change the voice sample parameters
    mov eax,[AInstr]
    mov edx,[4*eax+SmpAddress]
    mov esi,[4*eax+SmpStartAddress]
    mov edi,[4*eax+SmpEndAddress]
    call    MCStartVoice
    mov ecx,[APeriod]       ; change the voice frequency
    call    MCSetFrequency
    mov eax,[AVolume]        ; and set the voice volume
    call    MCSetVolume
    popad
    ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MODStopVoice - Turn off the specified voice
; In:
;  Voice = Audio channel
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc    MODStopVoice AVoice:dword
    pushad
    mov ebx,[AVoice]        ; set the voice volume to zero
    xor al,al
    call    MCSetVolume
    popad
    ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MODSetPeriod - Changes the voice period value
; In:
;  Voice  = Audio channel
;  Period = Amiga Period
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc    MODSetPeriod AVoice:dword,APeriod:dword
    pushad
    mov ebx,[AVoice]
    mov ecx,[APeriod]
    call    MCSetFrequency
    popad
    ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MODSetVolume - Set the voice volume
; In:
;  Voice  = Audio channel
;  Volume = New Volume
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc    MODSetVolume AVoice:dword,AVolume:dword
    pushad
    mov ebx,[AVoice]
    mov eax,[AVolume]
    call    MCSetVolume
    popad
    ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MODSetup - Setup all the MOD player stuff
; In:
;  EDI = modulefile address
; Out:
;  CF = status
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MODSetup:
    pushad              ; check for valid modfile formats
    mov [ActiveTracks],4
    cmp [edi+MODHeader.Sign],'.K.M'
    je  MODParse
        cmp     [edi+MODHeader.Sign],'4TLF'
        je      MODParse
        mov     [ActiveTracks],6
        cmp     [edi+MODHeader.Sign],'NHC6'
        je      MODParse
        mov     [ActiveTracks],8
        cmp     [edi+MODHeader.Sign],'NHC8'
        je      MODParse
        stc
        jmp     MODSetupDone
MODParse:
        mov     [OrderPos],0FFh         ; setup variables
        mov     [PatternRow],40h
        mov     [Tempo],6
        mov     [TempoCount],0
    mov [BPM],125
        mov     al,[edi+MODHeader.OrderLen]
        mov     [OrderLen],al

        mov     ecx,80h                 ; copy order list
    xor ebx,ebx
        xor     ah,ah
MODCopyOrder:
    mov al,[edi+ebx+MODHeader.Order]
        mov     [ebx+OrderList],al
        cmp     al,ah
        jb      MODCopyNext
        mov     ah,al
MODCopyNext:
        inc     ebx
        loop    MODCopyOrder

        movzx   ecx,ah                  ; copy pattern pointers
        inc     ecx
        xor     ebx,ebx
        movzx   eax,[ActiveTracks]
        shl     eax,8
        mov     esi,edi
        add     esi,size MODHeader
MODCopyPatterns:
        mov     [4*ebx+PatternList],esi
        add     esi,eax
        inc     ebx
        loop    MODCopyPatterns

        mov     ecx,31                  ; setup samples data
        lea     edi,[edi+MODHeader.Samples]
    xor ebx,ebx
        inc     ebx
MODCopySamples:
        mov     al,[edi+MODSample.Volume]
        mov     [ebx+SmpVolume],al
        movzx   eax,[edi+MODSample.SampleLength]
        movzx   edx,[edi+MODSample.RepLength]
        movzx   ebp,[edi+MODSample.RepStart]
        xchg    al,ah
        xchg    dl,dh
        xchg    ax,bp
        xchg    al,ah
        xchg    ax,bp
        add     eax,eax
        add     edx,edx
        add     ebp,ebp
    cmp edx,2           ; if not looped, then the loop
        ja      MODSetSmpLength         ; start and end are the same.
        xor     edx,edx
        mov     ebp,eax
MODSetSmpLength:
    add edx,ebp
        add     eax,esi
        add     edx,esi
    add ebp,esi
        mov     [4*ebx+SmpAddress],esi
        mov     [4*ebx+SmpEndAddress],edx
        mov     [4*ebx+SmpStartAddress],ebp
        mov     esi,eax
        add     edi,size MODSample
        inc     ebx
        loop    MODCopySamples
        clc
MODSetupDone:
        popad
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MODCallBack - Internal callback routine called by the mixer
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MODCallBack:
        pushad
        dec     [TempoCount]            ; decrease tempo counter
        jle     MODNextRow              ; next row?
        lea     esi,[Tracks]
    xor ebx,ebx
MODUpdateLoop:
        call    MODUpNote               ; update note events
    add esi,size Track
        inc     ebx
        cmp     bx,[ActiveTracks]
        jb      MODUpdateLoop
        popad
        ret
MODNextRow:
        mov     al,[Tempo]              ; reload tempo counter
        mov     [TempoCount],al
        inc     [PatternRow]            ; next pattern row
        cmp     [PatternRow],40h
        jb      MODDontWrap             ; next order?
        xor     ebx,ebx
        mov     [PatternRow],bl         ; reset the pattern row
        mov     bl,[OrderPos]
        inc     bl                      ; update the order position
    cmp bl,[OrderLen]
        jb      MODSetOrder
        xor     bl,bl
MODSetOrder:
        mov     [OrderPos],bl
    mov bl,[ebx+OrderList]  ; get the pattern from the order list
        mov     edi,[4*ebx+PatternList]
        mov     [PatternPtr],edi        ; set the pattern address
MODDontWrap:
        mov     edi,[PatternPtr]        ; read the pattern row...
        lea     esi,[Tracks]
        xor     ebx,ebx
MODGetNoteLoop:
        call    MODGetNote
        add     esi,size Track
        add     edi,4
        inc     ebx
        cmp     bx,[ActiveTracks]
        jb      MODGetNoteLoop
        mov     [PatternPtr],edi
        popad
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MODGetNote - Read the note event from the pattern sheet
; In:
;  EBX = voice number
;  ESI = track address
;  EDI = pattern data address
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MODGetNote:
    mov al,[edi+2]      ; get the sample number
    shr al,4
    mov ah,[edi]
        and     ah,0F0h
        or      al,ah
        test    al,al                   ; nonzero value?
        je      MODGetPeriod
        mov     [esi+Track.Instr],al    ; set new sample instrument
        movzx   eax,al                  ; and set the track volume
        mov     al,[eax+SmpVolume]
        mov     [esi+Track.Volume],al
        call    MCSetVolume
MODGetPeriod:
        mov     ax,[edi]                ; get the period Amiga value
        xchg    al,ah
        and     ax,0FFFh
        test    ax,ax
    je  MODGetEffect        ; nonzero value?
        mov     [esi+Track.Period],ax   ; set new period value
        movzx   ecx,ax
        call    MCSetFrequency
        push    esi edi                 ; set track instrument data
        movzx   eax,[esi+Track.Instr]
        mov     edx,[4*eax+SmpAddress]
        mov     esi,[4*eax+SmpStartAddress]
    mov edi,[4*eax+SmpEndAddress]
    call    MCStartVoice
        pop     edi esi
MODGetEffect:
        mov     ax,[edi+2]              ; get the effect value
        xchg    al,ah
        and     ax,0FFFh
        mov     [esi+Track.Effect],ax
        cmp     ah,0Ch                  ; dispatch effects
        je      EFXSetVolume
        cmp     ah,0Fh
        je      EFXSetSpeed
        cmp     ah,0Bh
        je      EFXJumpPos
        cmp     ah,0Dh
        je      EFXBreak
    cmp ah,09h
        je      EFXSampleOffset
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MODUpNote - Updates the current track's note event
; In:
;  EBX = voice number
;  ESI = track address
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MODUpNote:
    mov ax,[esi+Track.Effect]   ; dispatch the effects
    cmp ah,0Ah
    je  EFXSlideVolume
    ret


;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
; MOD Player Effects Stuff
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;   Only a small set of effects are implemented.
;   The registers EBX,ESI,EDI must be preserved.
;   The effect parameters are passed through the registers:
;       EBX - voice number
;   ESI - track address
;       AL  - effect parameter
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; EFXSetVolume - changes the track volume.
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
EFXSetVolume:
        call    MCSetVolume
EFXNone:
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; EFXSetSpeed - changes the current tempo or BPM rate.
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
EFXSetSpeed:
        test    al,al
        je      EFXNone
        cmp     al,20h
        jae     EFXSetBPM
        mov     [Tempo],al
        mov     [TempoCount],al
        ret
EFXSetBPM:
    mov [BPM],al
        mov     dl,al
        call    MCStartTimer
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; EFXJumpPos - jump to the spec order position.
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
EFXJumpPos:
        dec     al
        mov     [OrderPos],al
        mov     [PatternRow],40h
    ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; EFXBreak - breaks the current pattern.
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
EFXBreak:
        mov     [PatternRow],40h
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; EFXSlideVolume - slides the volume up or down.
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
EFXSlideVolume:
        mov     ah,al
        mov     al,[esi+Track.Volume]
        test    ah,0F0h
        je      EFXVolumeDown
        shr     ah,4
        add     al,ah
        cmp     al,40h
        jbe     EFXVolSlided
        mov     al,40h
EFXVolSlided:
        mov     [esi+Track.Volume],al
    call    MCSetVolume
        ret
EFXVolumeDown:
        sub     al,ah
        jge     EFXVolSlided
        xor     al,al
        jmp     EFXVolSlided

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; EFXSampleOffset - Set the sample offset
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
EFXSampleOffset:
    xor edx,edx
        mov     dh,al
        push    esi edi
        movzx   eax,[esi+Track.Instr]
        add     edx,[4*eax+SmpAddress]
        mov     esi,[4*eax+SmpStartAddress]
        mov     edi,[4*eax+SmpEndAddress]
        call    MCStartVoice
        pop     edi esi
        ret


;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
; MultiChannel Mixer Stuff
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MCInit - Initializes the multi-channel stuff for playback
; Out:
;  CF = status
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MCInit:
    pushad
    xor ax,ax           ; initialize variables
    mov [BufferPos],ax
        mov     [TimerCount],ax
        mov     [TimerSpeed],ax
        mov     [BufSelector],ax
        mov     ax,0100h                ; DPMI allocate DOS memory block
        mov     bx,(BUFLEN/16)+(420h)   ; (2K=DMA buffer 16K=VolBuffer)
        int     31h
        jc      MCInitDone
        mov     [BufSelector],dx        ; save DOS memory block selector
        mov     ecx,BUFLEN
        movzx   edi,ax
        shl     edi,4                   ; get the memory block linear address
        mov     esi,edi
        add     esi,ecx
        shl     ax,4                    ; check for cross pages in
        neg     ax                      ; the DMA buffer memory area
        cmp     ax,cx
        jae     MCAlignVolBuffer
        mov     esi,edi
    add edi,4200h
MCAlignVolBuffer:                       ; VolBuffer must be page aligned
        add     esi,0FFh                ; (256 bytes = 1 page)
        and     esi,not 0FFh
        mov     [VolBuffer],esi         ; save the addresses and length
        mov     [DMABuffer],edi
        mov     [DMALength],cx
        push    es                      ; clear DMA buffer
        mov     ax,ds
        mov     es,ax
        cld
        mov     al,80h
        rep     stosb
        pop     es
        mov     edi,[VolBuffer]         ; make the volume table
        mov     cx,[ActiveVoices]
        cmp     cx,4
        setae   cl
        xor     bx,bx
MCDoVolBuffer:
        mov     al,bl
        imul    bh
        sar     ax,cl
        mov     [edi],ah
    inc edi
        inc     bl
        jne     MCDoVolBuffer
        inc     bh
        cmp     bh,40h
        jbe     MCDoVolBuffer
        clc
MCInitDone:
        popad
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MCDone - Shutdown the multi-channel stuff
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MCDone:
        pushad
        mov     ax,0101h                ; DPMI free DOS memory block
        mov     dx,[BufSelector]
        test    dx,dx
        je      MCDoneExit
        int     31h
        mov     [BufSelector],0
MCDoneExit:
        popad
    ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MCStartVoice - Set the voice sample instrument parameters
; In:
;  EBX = voice number
;  EDX = start address
;  ESI = loop start
;  EDI = loop end
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MCStartVoice:
        push    ebx
        mov     ebx,[4*ebx+VoiceTable]
        mov     [ebx+Voice.Address],edx
        mov     [ebx+Voice.StartAddress],esi
        mov     [ebx+Voice.EndAddress],edi
        pop     ebx
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MCSetFrequency - Set the voice frequency counter
; In:
;  EBX = voice number
;  ECX = frequency (Amiga value)
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MCSetFrequency:
        push    eax
        push    ebx
        push    edx
        jecxz   MCSetFreqDone
        mov     ebx,[4*ebx+VoiceTable]
        mov     eax,[FreqDivisor]
        xor     edx,edx
        div     ecx
        mov     [ebx+Voice.Frequency],eax
MCSetFreqDone:
        pop     edx
        pop     ebx
        pop     eax
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MCSetVolume - set the voice volume
; In:
;  EBX = voice number
;  AL = volume
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MCSetVolume:
    push    ebx
        mov     ebx,[4*ebx+VoiceTable]
        mov     [ebx+Voice.Volume],al
        pop     ebx
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MCStartTimer - Set the timer frequency
; In:
;  DL = timer frequency in BPMs
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MCStartTimer:
        push    ax
        push    cx
        push    dx
        mov     ch,dl                   ; set the timer frequency
        xor     cl,cl                   ; at 24/60*BPM hertz
        mov     ax,[SBRate]
        mov     dx,0280h
        mul     dx
        div     cx
        mov     [TimerSpeed],ax
        pop     dx
        pop     cx
    pop ax
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MCPolls - Polling routine for the multi-channel stuff
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MCPolls:
        movzx   edi,[BufferPos]         ; get the buffer address
        add     edi,[DMABuffer]
        movzx   ecx,[DMALength]         ; get the buffer length
        shr     ecx,1
        mov     [BufferLen],cx
        xor     [BufferPos],cx
        push    edi es                  ; clear the buffer
        mov     ax,ds
        mov     es,ax
        mov     al,80h
        cld
        rep     stosb
        pop     es edi
MCPollLoop:
        cmp     [TimerCount],0          ; time to call the MOD callback?
        jg      MCPollChunk
        call    MODCallBack
    mov ax,[TimerSpeed]     ; update timer accumulator
        add     [TimerCount],ax
MCPollChunk:
        mov     ax,[BufferLen]          ; get the chunk length
        mov     cx,[TimerCount]
        add     cx,63                   ; round up to 64 bytes to avoid
        and     cx,not 63               ; small chunks in the mixing
        cmp     ax,cx
        jle     MCPollVoices
        mov     ax,cx
MCPollVoices:
        sub     [BufferLen],ax          ; decrease counters
        sub     [TimerCount],ax
        movzx   ecx,ax                  ; mixes all the voices
        lea     ebx,[Voices]
        mov     dx,[ActiveVoices]
MCVoicesLoop:
        push    ebx
        push    ecx
        push    edx
        push    edi
        call    MCMixVoice
        pop     edi
        pop     edx
    pop ecx
        pop     ebx
        add     ebx,size Voice
        dec     dx
        jg      MCVoicesLoop
        add     edi,ecx
        cmp     [BufferLen],0           ; more samples left?
        jg      MCPollLoop
MCPollDone:
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; MCMixVoice - Mixes the Voice's sample instrument
; In:
;  EBX = Voice address
;  EDI = Buffer address
;  ECX = Number of samples
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MCMixVoice:
    push    ebx

; Modify the mixing chunk of code

    mov eax,[ebx+Voice.EndAddress]
    mov [dword ds:MCMixCode0],eax
        mov     [dword ds:MCMixCode2],eax
        sub     eax,[ebx+Voice.StartAddress]
        mov     [dword ds:MCMixCode1],eax

; Load the sample address and frequency values

        mov     esi,[ebx+Voice.Address]
        mov     ebp,[ebx+4+Voice.Address]
        mov     eax,[ebx+Voice.Frequency]
        xor     edx,edx
        shld    edx,eax,16
        shl     eax,16

; Load the volume table address

        mov     bh,[ebx+Voice.Volume]
    and ebx,0FF00h
        add     ebx,[VolBuffer]

; Start of the mixing chunk of code (non pure code)

MCMixLoop:
        cmp     esi,12345678h
MCMixCode0 = $-4
        jb      MCMixCont
        sub     esi,12345678h
MCMixCode1 = $-4
        cmp     esi,12345678h
MCMixCode2 = $-4
        jae     MCMixBreak
MCMixCont:
        mov     bl,[esi]
        mov     bl,[ebx]
        add     [edi],bl
        add     ebp,eax
        adc     esi,edx
        inc     edi
        dec     ecx
        jg      MCMixLoop
MCMixBreak:

; Updates the sample current address

        pop     ebx
        mov     [ebx+Voice.Address],esi
        mov     [ebx+4+Voice.Address],ebp
        ret


;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
; Sound Blaster 1.5 Stuff
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; SBWrite - write command or data byte to the DSP chip
; In:
;  AL = data
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
SBWrite:
    push    eax
    push    ecx
    push    edx
    mov dx,[IOAddr]
    add dx,0Ch
    mov ecx,10000h      ; wait until the DSP is ready
        mov     ah,al                   ; to receive the data byte
SBWriteWait:
        in      al,dx
        and     al,80h
        loopnz  SBWriteWait
        mov     al,ah                   ; write data byte
        out     dx,al
        pop     edx
        pop     ecx
        pop     eax
    ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; SBResetDSP - reset the Sound Blaster DSP chip
; Out:
;  CF = status
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
SBResetDSP:
        pusha
        mov     dx,[IOAddr]
        add     dx,06h
        mov     al,01h                  ; write 01h to the reset register
        out     dx,al
        in      al,dx                   ; delay
        in      al,dx
        in      al,dx
        in      al,dx
        mov     al,00h                  ; write 00h to the reset register
        out     dx,al
        add     dx,08h                  ; wait until data is available
        mov     ecx,10000h
SBReadWait:
        in      al,dx
        and     al,80h
    loopz   SBReadWait
        sub     dx,04h
        in      al,dx                   ; read the DSP data byte
        cmp     al,0AAh
        clc
        je      SBResetDone             ; check SB DSP signature
        stc
SBResetDone:
        popa
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; SBOpenDevice - Initialize and start the SB card for playback
; In:
;  IOAddr = I/O Port Address
;  IRQNum = IRQ Level
;  DRQNum = DMA Channel
;  SBRate = Sampling Rate
; Out:
;  CF = initialization status
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
SBOpenDevice:
    pusha
    call    SBResetDSP      ; check for a SB card
    jc  SBOpenDone
        mov     al,0D1h
        call    SBWrite                 ; turn on SB speaker
        mov     al,40h
        call    SBWrite                 ; set playback sampling rate
        mov     ax,1000
        mul     ax
        div     [SBRate]
        neg     ax
        call    SBWrite
        mov     al,14h                  ; set the 8-bit DMA buffer length
        call    SBWrite                 ; and start the playback transfer
        mov     ax,[DMALength]
        shr     ax,1
        dec     ax
        call    SBWrite
        mov     al,ah
        call    SBWrite
        clc                             ; set successful status
SBOpenDone:
        popa
    ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; SBCloseDevice - shutdown the SB playback
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
SBCloseDevice:
        pusha
        call    SBResetDSP              ; reset the SB DSP chip
        mov     al,0D3h
        call    SBWrite                 ; turn off the SB speaker
        popa
        ret

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
; DMA Controller Stuff
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; DMAReset - reset the DMA channel
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
DMAReset:
        pusha
        mov     al,[DRQNum]             ; reset the DMA channel
        or      al,04h
    out 0Ah,al
        popa
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; DMASetBuf = set the DMA channel parameters for playback
; In:
;  DMABuffer = DMA Buffer Address
;  DMALength = DMA Buffer Length
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
DMASetBuf:
        pushad
        mov     cl,[DRQNum]             ; get the DMA channel number
        mov     al,cl
        or      al,04h                  ; reset the DMA channel
        out     0Ah,al
        out     0Ch,al                  ; clear DMA flip-flop register
        mov     al,cl
        or      al,58h                  ; set the DMA autoinit mode
        out     0Bh,al
        movzx   dx,cl                   ; set the DMA buffer address
        add     dx,dx
        mov     eax,[DMABuffer]
        out     dx,al
    mov al,ah
        out     dx,al
        inc     dx                      ; set the DMA buffer length
        mov     ax,[DMALength]
        dec     ax
        out     dx,al
        mov     al,ah
        out     dx,al
        mov     edx,82818387h           ; set the DMA buffer page
        shl     cl,3
        shr     edx,cl
        xor     dh,dh
        shr     cl,3
        shr     eax,16
        out     dx,al
        mov     al,cl                   ; set the DMA channel
        out     0Ah,al
        popad
        ret

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
; PIC IRQ Stuff
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; IRQGetIntNum - Returns the PIC interrupt base
; In:
;  AL = IRQ level
; Out:
;  AL = PIC interrupt number
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
IRQGetIntNum:
        push    dx
        push    ax bx cx                ; get the virtual master and slave
        mov     ax,0400h                ; PIC base interrupts in DX
        int     31h
    pop cx bx ax
    cmp al,08h          ; get the PIC base interrupt for
    jb  IRQGetDone      ; the IRQ number in AL
    mov dh,dl
    sub al,08h
IRQGetDone:
    add al,dh
    pop dx
    ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; IRQRestVect - Restores the previous IRQ vector
; In:
;  IRQNum  = IRQ level
;  IRQVect = IRQ Vector
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
IRQRestVect:
        pushad
    push    ds
    in  al,0A1h         ; disable the IRQ interrupt using
    mov ah,al           ; the PIC interrupt mask registers
    in  al,21h
    mov dx,01h
    mov cl,[IRQNum]
    shl dx,cl
        or      ax,dx
        out     21h,al
        mov     al,ah
        out     0A1h,al
        mov     ah,25h                  ; restore the IRQ vector using
        mov     al,cl                   ; the DOS/4GW INT 21h stuff
        call    IRQGetIntNum
        lds     edx,[IRQVector]
        xor     ebx,ebx
        mov     bx,ds
        or      ebx,edx
        test    ebx,ebx
        je      IRQRestDone
        int     21h
IRQRestDone:
        pop     ds
        xor     ebx,ebx
    mov [dword IRQVector],ebx
        mov     [word 4+IRQVector],bx
        popad
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; IRQSetVect - Set the new IRQ driver handler
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
IRQSetVect:
        pushad
        push    ds es
        mov     ah,35h                  ; get and saves the IRQ vector
        mov     al,[IRQNum]             ; using the DOS/4GW INT 21h stuff
        call    IRQGetIntNum
        int     21h
        mov     [dword IRQVector],ebx
        mov     [word IRQVector+4],es
        mov     ah,25h                  ; set the new IRQ vector using
        mov     al,[IRQNum]             ; the DOS/4GW INT 21h stuff
        call    IRQGetIntNum
        lea     edx,[IRQHandler]
        push    cs
        pop     ds
        int     21h
    pop es ds
        in      al,0A1h                 ; enable this IRQ using the
        mov     al,ah                   ; PIC interrupt mask registers
        in      al,21h
        mov     dx,01h
        mov     cl,[IRQNum]
        shl     dx,cl
        not     dx
        and     ax,dx
        out     21h,al
        mov     al,ah
        out     0A1h,al
        popad
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; IRQHandler - Hardware IRQ handler
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
IRQHandler:
    pushad              ; pushes all the registers
    push    ds
    mov ax,DGROUP       ; load the DS selector
    mov ds,ax
    call    _GETDS
    mov dx,[IOAddr]     ; send ack to the SB DSP chip
        add     dx,0Eh
        in      al,dx
        mov     al,14h                  ; restart DMA transfer
        call    SBWrite
        mov     ax,[DMALength]
        shr     ax,1
        dec     ax
        call    SBWrite
        mov     al,ah
        call    SBWrite
        call    MCPolls                 ; fill the samples buffer
        mov     al,20h
        cmp     [IRQNum],08h            ; send ack to 8259 PIC controllers
        jb      IRQAckPIC
        out     0A0h,al
IRQAckPIC:
        out     20h,al
        pop     ds
        popad                           ; restores all the registers
        iretd

end


        .486p

_TEXT   segment dword public use32 'CODE'
        assume  cs:_TEXT,ds:_TEXT

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; INCLUDE
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

        include main.inc
        include gfx.inc
        include load.inc

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; SYMBOLS
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

        public init_vesa

        public cred
        public pic0p
        public pic1p
        public pic2p
        public pic3p
        public endpic

        public se3p

        extrn xmpInit_:near
        extrn xmpPlay_:near
        extrn xmpStop_:near

        extrn _psp:word
        extrn mxmdata:byte
        extrn hand:dword

        extrn screen:dword
        extrn xmpGetPos_:near
        extrn initsync:near
        extrn getsync:near

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; DATA
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

msg0    db 13,10," ş error - VESA BIOS not detected !",13,10,7,"$"
msg1    db 13,10," ş mode 640x480x256 not available !",13,10,7,"$"
msg2    db 13,10," ş error loading file !",13,10,7,"$"
msg3    db 13,10," ş not enough xms memory !",13,10,7,"$"

cr1a    label byte
        include cr1.inc

cr2a    label byte
        include cr2.inc

cr3a    label byte
        include cr3.inc

epic    label byte
        include endpic.inc

cr0m    dd offs cr1a
        dd 0
        dd 1,1

cr1m    dd offs cr1a
        dd 113+74*320
        dd 79,58

cr2m    dd offs cr2a
        dd 101+62*320
        dd 105,81

cr3m    dd offs cr3a
        dd 100+65*320
        dd 114,82

file0   db "c:\morph\gfx\raw\artemen.raw",0
file1   db "c:\morph\gfx\raw\zdziwko.raw",0
file2   db "c:\morph\gfx\raw\girl.raw",0
file3   db "c:\morph\gfx\raw\pikpoke.raw",0
file4   db "c:\morph\gfx\raw\sfx1.raw",0
music   db "halein.mxm",0

adr0    dd 0
han0    dd 0
adr1    dd 0
han1    dd 0

scra    dd 0
shan    dd 0

info    db 256 dup (0)
temp    db 768 dup (0)

gran    dd 0

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; CODE
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; Entry To ASM Code
; In:
;   CS - Code Selector    Base: 00000000h - Limit: 4G
;   DS - Data Selector    Base: 00000000h - Limit: 4G
;   ES - PSP Selector     Base: PSP Seg   - Limit: 100h
;   FS - ?
;   GS - ?
;   SS - Data Selector    Base: 00000000h - Limit: 4G
;   ESP -> STACK segment
;   Direction Flag - ?
;   Interrupt Flag - ?
;
;   All Other Registers Are Undefined!
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

init_vesa:
        lea edi,info
        shld eax,edi,28
        and di,0fh
        mov reg_es,ax
        mov reg_di,di
        mov reg_ax,4f00h
        dos_int 10h
        cmp dptr info,"ASEV"
        lea ebp,msg0
        jne exit
        lea edi,info
        shld eax,edi,28
        and di,0fh
        mov reg_es,ax
        mov reg_di,di
        mov reg_cx,101h
        mov reg_ax,4f01h
        dos_int 10h
        cmp reg_ax,4fh
        lea ebp,msg1
        jne exit
        movzx eax,bptr info+4
        shl eax,10
        mov gran,eax
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; CREDITS
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

cred:   call xmpGetPos_

        cmp ax,0020h
        jg cr0p
        lea ebp,cr0m
        jmp cr4p

cr0p:   cmp ax,0120h
        jg cr1p
        lea ebp,cr1m
        jmp cr4p

cr1p:   cmp ax,0220h
        jg cr2p
        lea ebp,cr2m
        jmp cr4p

cr2p:   cmp ax,0320h
        jg cr3p
        lea ebp,cr3m
        jmp cr4p

cr3p:   cmp ax,0420h
        jg cr4p
        lea ebp,cr0m

cr4p:   mov esi,[ebp]
        mov edi,[ebp+4]
        add edi,screen
        mov ebx,320
        sub ebx,[ebp+8]
        mov ecx,[ebp+12]
sc0p:   mov edx,[ebp+8]
sc1p:   lodsb
        or al,al
        jz sc2p
        mov [edi],al
sc2p:   inc edi
        dec edx
        jnz sc1p
        add edi,ebx
        dec ecx
        jnz sc0p

        xor eax,eax
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; PICTURE #0
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

pic0p:  lea edx,file0
        call _loadfile
        lea ebp,msg2
        jc exit 
        mov adr0,edx
        mov han0,esi

        lea esi,temp
        call set_pal

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

        mov esi,adr0
        add esi,768
        mov edi,0a0000h
        mov ecx,16000
        rep movsd

        mov ecx,63
mp_1:   call v_sync
        mov dx,3c8h
        mov al,0
        out dx,al
        inc dx
        mov esi,adr0
        mov edi,768
mp_2:   mov al,cl
        cmp bptr [esi],cl
        jle mp_3
        mov al,[esi]
mp_3:   out dx,al
        inc esi
        dec edi
        jnz mp_2
        dec ecx
        jns mp_1

wa0p:   call xmpGetPos_
        cmp ax,0435h
        jl wa0p

        call zero

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

        mov edi,han0
        call free_mem
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; PICTURE #1
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

pic1p:  lea edx,file1
        call _loadfile
        lea ebp,msg2
        jc exit 
        mov adr0,edx
        mov han0,esi

        lea edx,file4
        call _loadfile
        lea ebp,msg2
        jc exit
        mov adr1,edx
        mov han1,esi

        mov ecx,64000
        call get_mem
        lea ebp,msg3
        jc exit
        mov scra,ebx
        mov shan,esi

        mov edi,ebx
        xor eax,eax
        mov ecx,16000
        rep stosd
        mov edi,0a0000h
        xor eax,eax
        mov ecx,16000
        rep stosd

        mov esi,adr0
        call set_pal

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

        call initsync

        xor ebx,ebx
go0p:   mov esi,adr0
        mov edi,scra
        mov ebp,adr1
        xor edx,edx
        mov ecx,64000-1
go1p:   mov dl,[ebp+ecx]
        cmp edx,ebx
        jg go2p
        mov al,[esi+ecx+768]
        mov [edi+ecx],al
go2p:   dec ecx
        jns go1p
        call v_sync
        call copy
        call getsync
        add ebx,eax
        call xmpGetPos_
        cmp ax,0b00h
        jl go0p

        mov esi,adr0
        add esi,768
        mov edi,scra
        mov ecx,16000
        rep movsd

        call v_sync
        call copy

wa1p:   call xmpGetPos_
        cmp ax,0c00h
        jl wa1p

        call initsync

        xor ebx,ebx
go3p:   mov esi,adr1
        mov edi,scra
        xor edx,edx
        mov ecx,64000-1
go4p:   mov dl,[esi+ecx]
        cmp edx,ebx
        jg go5p
        mov bptr [edi+ecx],0
go5p:   dec ecx
        jns go4p
        call v_sync
        call copy
        call getsync
        add ebx,eax
        call xmpGetPos_
        cmp ax,0d00h
        jl go3p

        mov edi,scra
        xor eax,eax
        mov ecx,16000
        rep stosd

        call v_sync
        call copy

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

        mov edi,han0
        call free_mem
        mov edi,han1
        call free_mem
        mov edi,shan
        call free_mem
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; PICTURE #2
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

pic2p:  mov ax,4f02h
        mov bx,101h
        int 10h

        lea edx,file2
        call _loadfile
        lea ebp,msg0
        jc exit
        mov adr0,edx
        mov han0,esi

        lea esi,temp
        call set_pal

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

        call show
        call fade

wa2p:   call xmpGetPos_
        cmp ax,1330h
        jl wa2p

        call zero

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

        mov ax,13h
        int 10h

        mov edi,han0
        call free_mem
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; PICTURE #3
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

pic3p:  mov ax,4f02h
        mov bx,101h
        int 10h

        lea edx,file3
        call _loadfile
        lea ebp,msg0
        jc exit
        mov adr0,edx
        mov han0,esi

        lea esi,temp
        call set_pal

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

        call show
        call fade

        lea edx,music
        call _loadfile
        lea ebp,msg2
        jc exit
        mov hand,esi
        mov esi,edx

        lea ebx,mxmdata
        mov ecx,65536
        movzx eax,_psp
        call xmpInit_

        mov ecx,260
wa3p:   call v_sync
        dec ecx
        jnz wa3p

        xor eax,eax
        call xmpPlay_
        call zero

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

        mov ax,13h
        int 10h

        mov edi,han0
        call free_mem
        ret

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; END PICTURE
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

endpic: call xmpGetPos_

        cmp ax,0200h
        jl se3p

        lea esi,epic
        mov edi,screen
        add edi,6+5*320
        mov ecx,14
se0p:   mov edx,97
se1p:   lodsb
        or al,al
        jz se2p
        mov [edi],al
se2p:   inc edi
        dec edx
        jnz se1p
        add edi,320-97
        dec ecx
        jnz se0p
        mov eax,1
        ret

se3p:   xor eax,eax
        ret

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

fade:   mov ebx,63
        mov ecx,64
pr_1:   call v_sync
        mov dx,3c8h
        mov al,0
        out dx,al
        inc dx
        mov esi,adr0
        mov edi,768
pr_2:   lodsb
        sub al,bl
        or al,al
        jns pr_3
        xor al,al
pr_3:   out dx,al
        dec edi
        jnz pr_2
        dec ebx
        dec ecx
        jnz pr_1
        ret

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

zero:   xor ebx,ebx
        mov ecx,64
pr_4:   call v_sync
        mov dx,3c8h
        mov al,0
        out dx,al
        inc dx
        mov esi,adr0
        mov edi,768
pr_5:   lodsb
        sub al,bl
        or al,al
        jns pr_6
        xor al,al
pr_6:   out dx,al
        dec edi
        jnz pr_5
        inc ebx
        dec ecx
        jnz pr_4
        ret

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

show:   mov esi,adr0
        add esi,768
        mov ebp,640*480
        xor edx,edx
move:   xor ebx,ebx
        mov ax,4f05h
        int 10h
        mov edi,0a0000h
        mov ecx,gran
        shr ecx,2
        rep movsd
        inc edx
        sub ebp,gran
        jg move
        ret

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

copy:   mov esi,scra
        mov edi,0a0000h
        mov ecx,16000
        rep movsd
        ret

_TEXT   ends
        end

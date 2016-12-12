
	.486p

_TEXT	segment dword public use32 'CODE'
	assume	cs:_TEXT,ds:_TEXT

start:	jmp main
	db "WATCOM"

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; INCLUDE
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

	include main.inc
	include gfx.inc

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; SYMBOLS
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

	public _psp
	public mxmdata
	public hand

	extrn _openlib:near
	extrn _closelib:near
	extrn _openfile:near

	extrn xmpInit_:near
	extrn xmpPlay_:near
	extrn xmpStop_:near

	extrn engine:near

	extrn init_vesa:near
	extrn cred:near
	extrn pic0p:near
	extrn pic1p:near
	extrn pic2p:near
	extrn pic3p:near
	extrn endpic:near
	extrn se3p:near

	extrn bump:near

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; DATA
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

msg0	db 13,10," ş error loading music !",13,10,7,"$"
msg1	db 13,10,"Artemen - demo by Pulp for GP3",13,10
	db 13,10,"1. Gravis UltraSound",13,10
	db "2. Sound Blaster 16",13,10
	db "3. No Sound",13,10,"$"
msg2	db 13,10,"SoundBlaster init failed - EMS error !",13,10,7,"$"

msg3 label byte
 db 13,10,13,10
 db "				  ùaùrùtùeùmùeùn   bùyù",13,10
 db 13,10
 db "		       ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ",13,10
 db "		       ÛßßßßßßßßÛÛßßßßÛßßßßÛ ÜÜÜÜ      ÜÜÜÜÜÜÜÜ  Û",13,10
 db "		       Û    Û	 Û    Û    Û ÛÛÛÛ      ÛÛÛÛ ÛÛÛÛ Û",13,10
 db "		       Û    Û	 Û    Û    Û ÛÛÛÛ      ÛÛÛÛ ÛÛÛÛ Û",13,10
 db "		       Û    Û	 Û    Û    Û ÛÛÛÛ      ÛÛÛÛ ÛÛÛÛ Û",13,10
 db "		       Û    Û	 Û    Û    Û ÛÛÛÛ      ÛÛÛÛ ÛÛÛÛ Û",13,10
 db "		       Û    Û	 Û    Û    Û ÛÛÛÛ ÛÛÛÛ ÛÛÛÛ ÛÛÛÛ Û",13,10
 db "		       Û    Û	 Û    Û    Û ÛÛÛÛ ÛÛÛÛ ÛÛÛÛ ÛÛÛÛ Û",13,10
 db "		       Û    ÜÜÜÜÛÛÜ   ß   ÜÛ ßÛÛÛÜÛÛÛß ÛÛÛÛßßßß  Û",13,10
 db "		       ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÛ",13,10
 db 13,10
 db 13,10,13,10
 db "			     code....................morph",13,10
 db "			     sound..........dave,hexachord",13,10
 db "			     pixels.......kurczak,pickpoke",13,10
 db "			     pmode/w..................tran",13,10
 db "$"

_psp	dw 0

data	db "artemen.dat",0

music	db "vague.mxm",0
hand	dd 0

wld0	db "virt0.wld",0
trj0	db "virt0.trj",0
dat0	db "virt0.dat",0

wld1	db "virt1.wld",0
trj1	db "virt1.trj",0
dat1	db "virt0.dat",0

wld2	db "virt2.wld",0
trj2	db "virt2.trj",0
dat2	db "virt3.dat",0

wld3	db "virt3.wld",0
trj3	db "virt3.trj",0
dat3	db "virt3.dat",0

wld4	db "virt4.wld",0
trj4	db "virt4.trj",0
dat4	db "virt0.dat",0

stru0	dd offs wld0	;.wld file
	dd offs trj0	;.trj file
	dd offs dat0	;.dat file
	dd 160		;zoom factor
	dd 2048 	;z clip
	dd offs cred	;co-routine
	db 62,3-2	;music sync

stru1	dd offs wld1	;.wld file
	dd offs trj1	;.trj file
	dd offs dat1	;.dat file
	dd 256		;zoom factor
	dd 2048 	;z clip
	dd offs se3p	;co-routine
	db 0,10 	;music sync

stru2	dd offs wld2	;.wld file
	dd offs trj2	;.trj file
	dd offs dat2	;.dat file
	dd 180		;zoom factor
	dd 1024 	;z clip
	dd offs se3p	;co-routine
	db 62,23	;music sync

stru3	dd offs wld3	;.wld file
	dd offs trj3	;.trj file
	dd offs dat3	;.dat file
	dd 260		;zoom factor
	dd 768		;z clip
	dd offs se3p	;co-routine
	db 60,34	;music sync

stru4	dd offs wld4	;.wld file
	dd offs trj4	;.trj file
	dd offs dat4	;.dat file
	dd 120		;zoom factor
	dd 1024 	;z clip
	dd offs endpic	;co-routine
	db 0,18 	;music sync

mxmdata db 4000h dup (0)

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; CODE
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; Entry To ASM Code
; In:
;   CS - Code Selector	  Base: 00000000h - Limit: 4G
;   DS - Data Selector	  Base: 00000000h - Limit: 4G
;   ES - PSP Selector	  Base: PSP Seg   - Limit: 100h
;   FS - ?
;   GS - ?
;   SS - Data Selector	  Base: 00000000h - Limit: 4G
;   ESP -> STACK segment
;   Direction Flag - ?
;   Interrupt Flag - ?
;
;   All Other Registers Are Undefined!
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

main:	sti
	cld

	mov _psp,es

	push ds
	pop es

	;lea edx,msg1
	;mov ah,9
	;int 21h
go0p:	;mov ah,7
	;int 21h
	;cmp al,"1"
	;je go1p
	;cmp al,"2"
	;je go2p
	;cmp al,"3"
	;je go1p
	;jmp go0p

go1p:	lea edx,data
	call _openlib

	finit
	call init_vesa

	mov ax,13h
	int 10h

	lea ebx,music
	call _openfile
	lea ebp,msg0
	jc exit
	mov hand,esi
	mov esi,edx

	lea ebx,mxmdata
	mov ecx,65536
	movzx eax,_psp
	call xmpInit_

	xor eax,eax
	call xmpPlay_

	in al,21h
	or al,2
	out 21h,al

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	lea esi,stru0
	call engine
	;call pic0p
	;lea esi,stru1
	;call engine
	;call pic1p
	;call bump
	;call pic2p
	;lea esi,stru2
	;call engine
	;lea esi,stru3
	;call engine
	;call xmpStop_
	;mov edi,hand
	;call free_mem
	;call pic3p
	;lea esi,stru4
	;call engine
	;call xmpStop_
	;mov edi,hand
	;call free_mem

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	in al,21h
	and al,0fdh
	out 21h,al

	mov ax,3
	int 10h

	call _closelib

	lea ebp,msg3
	jmp exit

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

go2p:	lea ebp,msg2
	jmp exit

_TEXT	ends

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; STACK
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
stack	segment para stack 'STACK'
	db 1000h dup(?)
stack	ends

	end start

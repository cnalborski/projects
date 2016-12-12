;Flat Mode version 1.0 by Nuke/Absence (a.k.a. Cezary Nalborski)

ext_min         =       64 ;minimum free extended memory (kbytes)

code            segment public
                assume  cs:code

;----------------------------------------------------------------------------

GDT_ptr         dw      0fh
                dw      offset GDT,0

GDT             dq      0
                db      0ffh,0ffh,0,0,0,92h,0cfh,0

driver          dd      0 ;XMS driver address

errmsg0         db      13,10,"Flat Mode error message:",13,10,"  $"
errmsg1         db      "þ 386 or better processor not detected !",13,10,7,"$"
errmsg2         db      "þ CPU is already in Virtual-8086 mode !",13,10,7,"$"
errmsg3         db      "þ XMS driver not detected !",13,10,7,"$"
errmsg4         db      "þ Not enough extended memory !",13,10,7,"$"
errmsg5         db      "þ Extended memory allocation failure !",13,10,7,"$"

;----------------------------------------------------------------------------
; flat_start (enable 4gb address space)
;----------------------------------------------------------------------------

                public  flat_start

flat_start:     mov     ax,cs
                mov     ds,ax

                pushf
                pushf
                pop     ax
                xor     ah,40h
                push    ax
                popf
                pushf
                pop     bx
                popf
                cmp     ah,bh ;check for Nested Task flag - only on 386+
                lea     dx,errmsg1
                jne     error

                .386p
                smsw    ax
                .386
                and     al,1 ;Virtual-8086 mode ?
                lea     dx,errmsg2
                jnz     error

                mov     ax,4300h
                int     2fh
                cmp     al,80h ;XMS driver present ?
                lea     dx,errmsg3
                jne     error

                mov     ax,4310h
                int     2fh
                mov     word ptr driver,bx
                mov     word ptr driver+2,es

                mov     ah,8
                call    driver
                sub     ax,64 ;free_size-HMA_size
                jnc     okay
                xor     ax,ax
okay:           cmp     ax,ext_min
                lea     dx,errmsg4
                jb      error

                mov     eax,code
                shl     eax,4
                add     dword ptr GDT_ptr+2,eax

                cli

                .386p
                lgdt    fword ptr GDT_ptr
                mov     eax,cr0
                or      al,1
                mov     cr0,eax
                jmp     $+2
                mov     bx,8
                mov     ds,bx
                mov     es,bx
                mov     fs,bx
                mov     gs,bx
                and     al,not 1
                mov     cr0,eax
                jmp     $+2
                .386

                sti
                ret

;----------------------------------------------------------------------------
; xms_alloc
; in:
;   dx - size (kbytes)
; out:
;   cf - set if error
;   eax - linear pointer to memory
;   cx - handle
;----------------------------------------------------------------------------

                public  xms_alloc

xms_alloc:      mov     ah,09h
                call    cs:driver
                or      ax,ax
                jz      alloc_error
                mov     cx,dx
                mov     ah,0ch
                call    cs:driver
                or      ax,ax
                jz      alloc_error
                mov     ax,dx
                shl     eax,16
                mov     ax,bx
                clc
                ret
alloc_error:    stc
                ret

;----------------------------------------------------------------------------
; xms_free
; in:
;   dx - handle
; out:
;   cf - set if error
;----------------------------------------------------------------------------

                public  xms_free

xms_free:       mov     ah,0dh
                call    cs:driver
                or      ax,ax
                jz      free_error
                mov     ah,0ah
                call    cs:driver
                or      ax,ax
                jz      free_error
                clc
                ret
free_error:     stc
                ret

;----------------------------------------------------------------------------
; error
; in:
;   dx - message offset
;----------------------------------------------------------------------------

error:          push    dx
                mov     ax,3
                int     10h
                lea     dx,errmsg0
                mov     ah,9
                int     21h
                pop     dx
                mov     ah,9
                int     21h
                mov     ax,4cffh
                int     21h

code            ends
                end
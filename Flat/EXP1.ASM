;Flat Mode version 1.0 - example #1

code            segment public
                assume  cs:code

                include flat.inc

message         db      " Flat Mode version 1.0 exaple #1 "
len             =       $-message

start:          call    flat_start

                mov     ax,3
                int     10h

                mov     ax,cs
                mov     ds,ax

                xor     ax,ax
                mov     es,ax

                mov     ah,5fh
                lea     si,message
                .386
                mov     edi,0b8000h
                mov     cx,len
write:          lodsb
                stos    word ptr es:[edi]
                loop    write

                mov     ah,7
                int     21h

                mov     ax,4c00h
                int     21h

code            ends
                end     start

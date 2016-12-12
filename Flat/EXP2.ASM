;Flat Mode version 1.0 - example #2

code            segment public
                assume  cs:code

                include flat.inc

handle          dw      0

start:          call    flat_start

                cld

                mov     ax,13h
                int     10h

                mov     dx,64
                call    xms_alloc
                mov     cs:handle,cx

                xor     ax,ax
                mov     ds,ax
                mov     es,ax

                .386
                mov     esi,eax
                mov     edi,eax
                mov     eax,80000000h
                mov     ecx,16000
                rep     stos dword ptr es:[edi]

                mov     edi,0a0000h
                mov     ecx,16000
                rep     movs dword ptr es:[edi],ds:[esi]

                mov     ah,7
                int     21h

                mov     dx,cs:handle
                call    xms_free

                mov     ax,3
                int     10h

                mov     ax,4c00h
                int     21h

code            ends
                end     start
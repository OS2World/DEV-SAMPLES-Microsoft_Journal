;bootdd.asm - Reboot Device Driver example

;The driver installs and waits for an ioctl call.  When the call is
;received - it reboots the machine.

.286

            extrn DosPutMessage:far

CR          equ   0Dh
LF          equ   0Ah
CON         equ   0

DAW_CHAR    equ   8000h
DAW_LEVEL   equ   0080h
DAW_IOCTL   equ   0040h

INIT_CMD    equ   0
IOCTL_CMD   equ   10h


_DATA       segment word public 'DATA'

devhdr      dd    -1
devatt      dw    DAW_CHAR or DAW_LEVEL or DAW_IOCTL
strat       dw    offset strategy
ipc         dw    0
devname     db    'REBOOT$ '
reserved    db    8 DUP (0)

devhlp      dd    ?     ;devhlp entry
reboot      dd    ?     ;reboot vector

initmsg     db    LF,'Reboot Driver "REBOOT$" successfully installed!',CR,LF
initlen     equ   $-initmsg

failmsg     db    LF,'Reboot Driver "REBOOT$" failed to install!',CR,LF
faillen     equ   $-failmsg


_DATA       ends


            assume cs:_TEXT,ds:_DATA

_TEXT       segment word public 'CODE'

strategy    proc far

            mov      di,es:[bx+2]      ;get command

            cmp      di,INIT_CMD       ;init command?
            jne      ioctl
            call     init
            jmp      short sdone

ioctl:      cmp      di,IOCTL_CMD      ;ioctl command?
            jne      serr
            jmp      dword ptr [reboot]      ;reboot machine

serr:       mov      ax,8103h          ;bad command - return error

sdone:      mov      es:[bx+3],ax
            ret

strategy    endp


init        proc near

            mov      ax, es:[bx+14]       ;save devhlp address
            mov      word ptr devhlp, ax
            mov      ax, es:[bx+16]
            mov      word ptr devhlp+2,ax

            push     bx                ;save interesting registers
            push     es
            push     dx
            mov      al,5              ;get reboot vector
            mov      dl,24H
            call     devhlp            ;GetDosVar
            jc       fail              ;GetDosVar failed - quit install.

            mov      es, ax                     ;place reboot vector in
            mov      ax,  word ptr es:[bx]      ;data segment
            mov      word ptr reboot, ax
            mov      bx,  word ptr es:[bx+2]
            mov      word ptr reboot+2, bx

fail:       pop      dx                ;restore regs
            pop      es
            pop      bx
            jnc      success           ;no carry - GetDosVar succeeded
                                       ;and reboot vector saved

            push     CON               ;print failure message
            push     faillen
            push     ds
            push     offset _DATA:failmsg
            call     DosPutMessage

;return zeros for end of code and data segments

            mov      word ptr es:[bx+14], offset _TEXT:0
            mov      word ptr es:[bx+16], offset _DATA:0

            mov      ax,8103h          ;return failure
            jmp      short idone

success:    push     CON               ;print appropriate message
            push     initlen
            push     ds
            push     offset _DATA:initmsg
            call     DosPutMessage

;return end of code and data segments

            mov      word ptr es:[bx+14], offset _TEXT:init
            mov      word ptr es:[bx+16], offset _DATA:initmsg

            mov      ax, 0100h         ;return successful init

idone:      ret

init        endp

_TEXT       ends
            end

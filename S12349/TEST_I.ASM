        PAGE    ,132
        TITLE   Test Device Driver
        SUBTTL  SAMPLE - Initialization routine
.286P                   ; Enable 80286 protect-mode instructions
;============================================================================
;
; Register conventions:
;       DS:SI and SS:BP are set up by the strategy routine before dispatching
;       to the appropriate handler.
;
;       Each handler must preserve SS:BP and return control to an Exit label,
;       but may trash DS:SI, ES:DI, and even SP.  See SAMPExit for details.
;
;============================================================================

        INCLUDE SAMPDEFS.INC                    ; which includes some other .incs

STDOUT   EQU   1

HIGH_DATA       SEGMENT WORD PUBLIC 'FAR_DATA'

HIGH_DATA       ENDS


HIGH_CODE       SEGMENT WORD PUBLIC 'FAR_CODE'

extrn     HighTimer:FAR

HIGH_CODE       ENDS


MAIN_DATA SEGMENT WORD PUBLIC 'DATA'
END_Of_DATA     LABEL   BYTE        ; Everything past here will be given
                                    ; back to system at end of init

extrn  DevHlp:DWORD                 ; these variables are located in the data

SignonMsg     DB  13d, 10d, 'Example Multi-Segmented Device Driver',13d, 10d
len           DW  $ - SignonMsg

MAIN_DATA       ENDS


MAIN_CODE       SEGMENT WORD PUBLIC 'CODE'
         ASSUME CS:MAIN_CODE,DS:MAIN_DATA,ES:NOTHING,SS:NOTHING

END_OF_CODE     LABEL   BYTE        ; Everything past here will be given
                                    ; back to system at end of init


;============================================================================
;
; - SAMPINIT -
;
;       One time intialization code, called at system boot time.
;
; The Init routine will be called to set up the DevHlp address
; and do whatever hardware initialization is necessary.
;
;       Entry:  Local data contains the address of the command packet
;
;       Exit:   Return to the correct exit entry point based on whether
;               we were able to install or not.
;
        PUBLIC  Do_Init

Do_Init PROC NEAR
        ASSUME  DS:MAIN_DATA
;
; Output Sign-on Message
;

extrn   DOSPUTMESSAGE:FAR
extrn   TimerHandler:FAR
;extrn   Handler:NEAR
extrn   SAMPEXIT0:NEAR

;
;
        push    WORD PTR    STDOUT              ;File handle - standard output
        push    len                             ;Length of message
        push    ds                              ;Selector of message
        push    OFFSET  SignonMsg               ;Address of message
        call    DOSPUTMESSAGE                   ;AX destroyed

; save DevHlp address in the variable DevHlp

        les     bx, [bp].LD_PTRSAVE             ; ES:BX -> request packet
        mov     ax, word ptr es:[bx].InitDevHlp._off
        mov     word ptr DevHlp._off, ax
        mov     ax, word ptr es:[bx].InitDevHlp._seg
        mov     word ptr DevHlp._seg, ax
;
; indicate the end of code and end of data segments in the request packet

        mov     ax,OFFSET MAIN_CODE:END_OF_CODE ; Set up end-of-code
        mov     cx,OFFSET MAIN_DATA:END_Of_DATA ; and end of data

        mov     es:[bx].InitEcode,ax            ; save end marks in packet
        mov     es:[bx].InitEdata,cx

; lock the extra high data and code segments in the memory

IFDEF DEBUG
        int     3
ENDIF

        mov     ax, SEG HIGH_DATA
        mov     bh, 01h                         ; long term lock
;       mov     bh, 03h                         ; long term lock
        mov     bl, 0
        DevHelp DevHlp_Lock
        jnc     init1

IFDEF DEBUG
        int     3
ENDIF

init1:
        mov     ax, SEG HIGH_CODE
        mov     bh, 01h                         ; long term lock
;       mov     bh, 03h                         ; long term lock
        mov     bl, 0
        DevHelp DevHlp_Lock
        jnc     init2

IFDEF DEBUG
        int     3
ENDIF

init2:
        mov     ax, OFFSET MAIN_CODE:TimerHandler
        mov     bx, 0FFFFh
        DevHelp DevHlp_TickCount
        jnc     fine

IFDEF DEBUG
        int     3
ENDIF

fine:

;
; We are done with the INIT so exit.
;
                                        ; Done, no error.
        jmp     SAMPEXIT0               ; A routine in test.asm

; ----------------------
;
; There's been an error.  Right now, the only possibility is that
; it's from DevHlp_GetDOSVar. In any case, we assume it serious enough that
; we can't install, so to tell OS/2 that we:
;
;   - set the number of units to 0
;   - set the ending code segment offset to 0
;   - set the ending data segment offset to 0
;   - set the error bit in the status word
;
; We jump to SAMPEXIT9, which sets the error bit in the status word.  I'm told
; that the actual error code doesn't matter, just the error bit; but "general
; failure" seems the closest, so what the heck.
;
INIT_ERROR:

        LDS     BX,[BP].LD_PTRSAVE          ; DS:BX -> request block
        XOR     AX,AX
        MOV     [BX].INITCUNIT,AL          ; Set #units to 0.
        MOV     [BX].InitEcode,AX
        MOV     [BX].InitEdata,AX
extrn      SAMPEXIT9:NEAR
                                            ; Done, error = "general failure"

        JMP     SAMPEXIT9                   ; A routine in test.asm

Do_Init ENDP

MAIN_CODE       ENDS
                END

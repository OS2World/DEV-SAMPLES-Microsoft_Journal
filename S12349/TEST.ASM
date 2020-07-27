        PAGE    ,132
        TITLE   Test Device Driver
        SUBTTL  MAIN MODULE
.286P                   ; Enable 80286 protect-mode instructions


        INCLUDE SAMPDEFS.INC                    ; which includes some other .incs

MAIN_DATA       SEGMENT WORD PUBLIC 'DATA'

MAIN_DATA       ENDS


MAIN_CODE       SEGMENT WORD PUBLIC 'CODE'

MAIN_CODE       ENDS


HIGH_DATA       SEGMENT WORD PUBLIC 'FAR_DATA'

HIGH_DATA       ENDS


HIGH_CODE       SEGMENT WORD PUBLIC 'FAR_CODE'

extrn  HighTimer:FAR

HIGH_CODE       ENDS

MAIN_DATA SEGMENT WORD PUBLIC 'DATA'
         ASSUME CS:MAIN_CODE,DS:MAIN_DATA,ES:NOTHING,SS:NOTHING

;====================================================
;
; Device Header -
;
;====================================================

IFDEF DEBUG
        PUBLIC  SAMP_DEV
ENDIF

SAMP_DEV LABEL WORD
        DW      -1,-1                               ; SYSINIT handles the segment part
        DW      DEV_CHAR_DEV OR DEVLEV_1 OR DEV_30 OR DEV_GIOCTL
                                                    ; Use equates in DEVHDR.INC
                                                    ; to define Attribute word
        DW      OFFSET MAIN_CODE:STRAT              ; offset to strategy routine
                                                    ; entry point
        DW      0                                   ; Reserved
        DB      'TEST$   '                          ; Name of device
DPS     DB      8 DUP (?)                           ; More reserved

; End of the device header.
; 
;============================================================================

;
;  data structure used by this device driver
;

        EVEN
DISPTAB LABEL WORD

        DW      OFFSET MAIN_CODE:SAMP_INIT       ;0: Init
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;1: Media Check (Block Devices Only)
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;2: Build BPB (Block Devices Only)
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;3: Reserved (used to be Ioctl Input)
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;4: Input (Read)
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;5: Non-Destructive Input, no wait.
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;6: Input Status
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;7: Input Flush
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;8: Output (Write)
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;9: Output (Write) with verify
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;10: Output Status
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;11: Output Flush
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;12: Reserved (used to be Ioctl Output)
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;13: Device Open
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;14: Device Close
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;15: Removable Media
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;16: Generic Ioctl
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;17: Reset Media
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;18: Get Logical Drive Map
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;19: Set Logical Drive Map
        DW      OFFSET MAIN_CODE:SAMP_EXIT       ;20: DeInstall

MAXCMD = (($ - DispTab)/2) - 1

DevHlp        DD   0
HighDS        DW   SEG HIGH_DATA

PUBLIC        DevHlp

MAIN_DATA       ENDS


page


MAIN_CODE       SEGMENT WORD PUBLIC 'CODE'
         ASSUME CS:MAIN_CODE,DS:MAIN_DATA,ES:NOTHING,SS:NOTHING

;============================================================================
;
; - SAMP_STRAT -
;
;       Device Driver strategy entry point.
;       Parameters      - es:bx = Pointer to the request block
;

STRAT       PROC    FAR
        ASSUME  DS:MAIN_DATA

IFDEF      DEBUG
               INT     3
ENDIF

        SUB     SP,SIZE LOCALDATA               ; reserve space for LocalData (Defined in .INC file)
        MOV     BP,SP                           ; ...referenced with positive offsets
        MOV     [BP].LD_PTRSAVE._OFF,BX         ; Save the virtual address  (ES:BX)
        MOV     [BP].LD_PTRSAVE._SEG,ES         ; ...of the Request Packet.
        MOV     AL,ES:[BX].PKTCMD               ; Get command from packet
        CBW
        CMP     AL,MAXCMD                       ; Make sure its allowed
        JA      SAMPEXIT1                        ; Jump to exit if bad command.

        ADD     AX,AX                           ; Convert to table index
                                                ; (Word sized instead of byte sized)

        MOV     BX,AX                           ; Move to BX and...
        JMP     DISPTAB[BX]                     ;Go execute command

;
; On entry to each of the command handlers:
;
;       DS                 = Driver data
;       SS:BP              = LocalData structure
;       SS:[BP].LD_PTRSAVE = Far pointer to the request block, which looks like:
;
;               13-BYTE request header
;                   BYTE    length of packet in bytes
;                   BYTE    subunit number of BLOCK device (unused by char dev.)
;                   BYTE    command code
;                   WORD    status word (returned by DD)
;                   DWORD   reserved
;                   DWORD   request queue link
;               n WORDS command specific data
;

STRAT       ENDP

page
;============================================================================
;
; - SAMP_EXIT - All routines return through this path
;
; On transfer to a label within SAMPExit:
;       ss:bp              = LocalData structure
;       ss:[bp].ld_ptrsave = Far pointer to the request block
;
;   Notice that the stack doesn't have to be balanced when you get to here.
; You can be down 6 pushes deep, and jump to here, just so long as SS:BP
; points to the LocalData structure.  SP will be set from BP.
;============================================================================


        PUBLIC  SAMP_EXIT,SAMPEXIT0,SAMPEXIT1,SAMPEXIT9

SAMP_EXIT PROC FAR
    ASSUME DS:MAIN_DATA

SAMPEXIT0:                                      ;0100h Done, no error
        MOV     AX,STDON
        JMP     SHORT  FINAL_EXIT

SAMPEXIT1:                                      ;8103h Done, error = "unknown command"


SAMPEXIT9:                                      ;810Ch Done, error = "general failure"

        MOV     AX,STDON+STERR+ERROR_I24_GEN_FAILURE

; 
; All exits come to this point before returning.
;
FINAL_EXIT:

        LDS     BX,[BP].LD_PTRSAVE              ;Load packet address back into DS:BX
        MOV     [BX].PKTSTATUS,AX               ;Mark operation complete

        MOV     SP,BP                           ;Re-Load SP form BP (clean up stack)
        ADD     SP,SIZE LOCALDATA               ;And de-allocate local data space


        RET
SAMP_EXIT ENDP


;==============================================================================
;
;
;
;  Timer Handler
;
;
;
;==============================================================================


TimerHandler   PROC    FAR
               PUBLIC  TimerHandler

    pusha                                         ;
    push    ds
    mov     ds, HighDS                            ; DS -> extra data segment

    call    HighTimer                             ; HighTimer is in extra code

    pop     ds                                    ; segment (test_hi.asm)

    popa
    ret

TimerHandler   ENDP


;==============================================================================
;
;
;
;  SAMP_INIT
;
;
;
;==============================================================================

IFDEF DEBUG
        PUBLIC  SAMP_INIT
ENDIF


SAMP_INIT PROC NEAR
        ASSUME  DS:MAIN_DATA

extrn   Do_Init:near
        jmp    Do_Init         ; A routine in test_i.asm

SAMP_INIT        ENDP


MAIN_CODE       ENDS
                END

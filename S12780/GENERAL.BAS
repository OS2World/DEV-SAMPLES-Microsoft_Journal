'============================================================================
'
'     GENERAL.BAS - General Routines for the User Interface Toolbox in
'           Microsoft BASIC 7.1, Professional Development System
'              Copyright (C) 1987-1990, Microsoft Corporation
'
'  NOTE:    This sample source code toolbox is intended to demonstrate some
'           of the extended capabilities of Microsoft BASIC 7.1 Professional
'           Development system that can help to leverage the professional
'           developer's time more effectively.  While you are free to use,
'           modify, or distribute the routines in this module in any way you
'           find useful, it should be noted that these are examples only and
'           should not be relied upon as a fully-tested "add-on" library.
'
'  PURPOSE: These are the general purpose routines needed by the other
'           modules in the user interface toolbox.
'
'  NOTE: These routines have been modified to support under OS/2 both
'        full-screen and windowed command prompts using OS/2 API functions
'        to provide similar effect to the DOS based code.
'
'  All sections of code that have been modified will have a comment
'  preseeding the modifications in the following manner :
'
'  '| --- Modified to support OS/2 changes ---
'  '|
'  '| Description of changes
'  '| ----------------------
'  '|
'
'  THIS IS SAMPLE CODE AND IS NOT TO BE CONSIDERED A COMPLETE BUG FREE
'  PACKAGE.  THIS CODE IS DESIGNED SPECIFICALLY TO RUN UNDER OS/2 PROTECTED
'  MODE.  THE ORIGINAL CODE HAS NOT BEEN MODIFIED IN ANY WAY, EXCEPT TO
'  PROVIDE THIS FUNCTIONALITY.
'
'
'============================================================================

DEFINT A-Z

'$INCLUDE: 'general.bi'
'$INCLUDE: 'mouse.bi'

'| --- Modified to support OS/2 changes ---
'|
'| Description of changes
'| ----------------------
'|
'| Added module global variables to be used by the OS/2 Keyboard handlers
'| for GetShiftState function.
'|

COMMON SHARED /APIKeys/ KeyBoardOpen   as integer
COMMON SHARED /APIKeys/ KeyBoardHandle as integer

'| Added TYPE...END TYPE's to be used with the OS/2 API calls

TYPE tCell
    Char as string * 1
    Attr as string * 1
END TYPE

TYPE tKbdPeek
    chChar     as string * 1
    chScan     as string * 1
    Status     as string * 1
    Reserved   as string * 1
    ShiftState as integer
    TimeStamp  as long
END TYPE

Type tKbdGetStatus
    cb as integer
    mask as integer
    turnaround as integer
    interim as integer
    state as integer
end type

'| DECLARE Statements for OS/2 Keyboard API functions

DECLARE FUNCTION KbdPeek%      ( SEG   pvData  AS tKbdPeek, _
                                 BYVAL hDevice as integer   _
                               )

DECLARE FUNCTION KbdFreeFocus% ( BYVAL hDevice as integer )

DECLARE FUNCTION KbdGetFocus%  ( BYVAL fWait   as integer, _
                                 BYVAL hDevice as integer  _
                               )

DECLARE FUNCTION kbdOpen%      ( SEG hDevice as integer )

DECLARE FUNCTION KbdGetStatus% ( SEG   pvData  as tKbdGetStatus, _
                                 BYVAL hDevice as Integer        _
                               )


'| Declare Statement for OS/2 Video I/O API functions

DECLARE FUNCTION VioScrollUp%  ( BYVAL ULRow       as Integer, _
                                 BYVAL ULCOL       as Integer, _
                                 BYVAL LRRow       as Integer, _
                                 BYVAL LRCol       as Integer, _
                                 BYVAL Lines       as Integer, _
                                 SEG   Cell        as tCell,   _
                                 BYVAL VideoHandle as Integer  _
                               )

DECLARE FUNCTION VioScrollDn%  ( BYVAL ULRow       as Integer, _
                                 BYVAL ULCOL       as Integer, _
                                 BYVAL LRRow       as Integer, _
                                 BYVAL LRCol       as Integer, _
                                 BYVAL Lines       as Integer, _
                                 SEG   Cell        as tCell,   _
                                 BYVAL VideoHandle as Integer  _
                               )



FUNCTION AltToASCII$ (kbd$)
    ' =======================================================================
    ' Converts Alt+A to A,Alt+B to B, etc.  You send it a string.  The right
    ' most character is compared to the string below, and is converted to
    ' the proper character.
    ' =======================================================================
    index = INSTR("xyz{|}~ÄÅ !" + CHR$(34) + "#$%&,-./012ÇÉ", RIGHT$(kbd$, 1))

    IF index = 0 THEN
        AltToASCII = ""
    ELSE
        AltToASCII = MID$("1234567890QWERTYUIOPASDFGHJKLZXCVBNM-=", index, 1)
    END IF

END FUNCTION

SUB Box (row1, col1, row2, col2, fore, back, border$, fillFlag) STATIC

    '=======================================================================
    '  Use default border if an illegal border$ is passed
    '=======================================================================

    IF LEN(border$) < 9 THEN
        t$ = "⁄ƒø≥ ≥¿ƒŸ"
    ELSE
        t$ = border$
    END IF

    ' =======================================================================
    ' Check coordinates for validity, then draw box
    ' =======================================================================

    IF col1 <= (col2 - 2) AND row1 <= (row2 - 2) AND col1 >= MINCOL AND row1 >= MINROW AND col2 <= MAXCOL AND row2 <= MAXROW THEN
        MouseHide
        BoxWidth = col2 - col1 + 1
        BoxHeight = row2 - row1 + 1
        LOCATE row1, col1
        COLOR fore, back
        PRINT LEFT$(t$, 1); STRING$(BoxWidth - 2, MID$(t$, 2, 1)); MID$(t$, 3, 1)
        LOCATE row2, col1
        PRINT MID$(t$, 7, 1); STRING$(BoxWidth - 2, MID$(t$, 8, 1)); MID$(t$, 9, 1);

        FOR a = row1 + 1 TO row1 + BoxHeight - 2
            LOCATE a, col1
            PRINT MID$(t$, 4, 1);

            IF fillFlag THEN
                PRINT STRING$(BoxWidth - 2, MID$(t$, 5, 1));
            ELSE
                LOCATE a, col1 + BoxWidth - 1
            END IF

            PRINT MID$(t$, 6, 1);
        NEXT a
        LOCATE row1 + 1, col1 + 1
        MouseShow
    END IF

END SUB

SUB GetBackground (row1, col1, row2, col2, buffer$) STATIC

    ' =======================================================================
    ' Create enough space in buffer$ to hold the screen info behind the box
    ' Then, call GetCopyBox to store the background in buffer$
    ' =======================================================================

    IF row1 >= 1 AND row2 <= MAXROW AND col1 >= 1 AND col2 <= MAXCOL THEN
        Wid = col2 - col1 + 1
        Hei = row2 - row1 + 1
        size = 4 + (2 * Wid * Hei)
        buffer$ = SPACE$(size)

        CALL GetCopyBox(row1, col1, row2, col2, buffer$)
    END IF

END SUB

'| --- Modified to support OS/2 changes ---
'|
'| Description of changes
'| ----------------------
'|
'| Added OpenKeyboard Function to open the keyboard and reduce the size
'| of GetShiftState function.

Function OpenKeyBoard%

    If NOT KeyBoardOpen then
        Rtn% = KbdOpen% (KeyBoardHandle)
        if NOT Rtn% then
            KeyBoardOpen = TRUE
        End if
    End if

    OpenKeyBoard% = KeyBoardOpen

END Function

'| --- Modified to support OS/2 changes ---
'|
'| Description of changes
'| ----------------------
'|
'| Modified GetShiftState function to work according to the differences
'| between the DOS GetShiftState and the OS/2 equal by functionality.

FUNCTION GetShiftState% (bit%)

    DIM KeyBoardInfo as tKbdPeek
    DIM KeyBoardStatus as tKbdGetStatus

    RtnState% = False

    If NOT KeyBoardOpen then
        rtn% = OpenKeyboard%
    end if

    IF bit% >= 0 and bit% <= 7 then

        rtn% = kbdgetfocus% (iowait%, keyboardhandle)

        if kbdpeek% (keyboardinfo, keyboardhandle) <> 0 then
            'Skip
        elseif keyboardinfo.status = chr$(0) then

            keyboardstatus.cb = len(keyboardstatus)

            if kbdgetstatus (keyboardstatus, keyhandle%) = 0 then
                if keyboardstatus.state <> 0 then
                    ShiftState% = keyboardstatus.state
                end if
            end if

        else
            ShiftState% = keyboardinfo.ShiftState
        end if

        Rtn% = kbdFreeFocus (KeyHandle%)

        if ShiftState% and 2 ^ bit% then
            RtnState% = TRUE
        end if

    end if

    GetShiftState% = RtnState%

END FUNCTION


SUB PutBackground (row, col, buffer$)

    ' =======================================================================
    ' This sub checks the boundries before executing the put command
    ' =======================================================================

    IF row >= 1 AND row <= MAXROW AND col >= 1 AND col <= MAXCOL THEN
        CALL PutCopyBox(row, col, buffer$)
    END IF

END SUB

'| --- Modified to support OS/2 changes ---
'|
'| Description of changes
'| ----------------------
'|
'| Modified Scroll SUB to work with the OS/2 API video functions that scroll
'| the screen.
'|
'| The OS/2 API VIO calls replace the Call Interrupt service routines

SUB scroll (row1, col1, row2, col2, lines, attr)

    ULRow% = Row1
    ULCOl% = Col1

    LRRow% = Row2
    LRCOl% = Col2

    IF ULRow% > LRRow% Then Swap ULRow%, LRRow%

    IF ULCol% > LRCol% Then Swap ULCol%, LRCol%


    IF row1 >= MINROW AND row2 <= MAXROW AND col1 >= MINCOL AND col2 <= MAXCOL THEN

        DIM Cell as TCell

        Cell.Char = chr$(32)
        Cell.Attr = chr$(attr * 16)

        ScrollLines% = lines

        TopRow%   = ULRow% - 1
        LeftCol%  = ULCol% - 1

        BotRow%   = LRRow% - 1
        RightCol% = LRCol% - 1

        IF lines < 0 THEN

            '| Scroll the screen down

            Rtn% = VioScrollDn% ( TopRow%,      _
                                  LeftCol%,     _
                                  BotRow%,      _
                                  RightCol%,    _
                                  ScrollLines%, _
                                  Cell,         _
                                  VideoHandle%  _
                                )

        ELSEIf Lines = 0 Then

            ScrollLines% = BotRow% - TopRow% + 1

            Rtn% = VioScrollDn% ( TopRow%,      _
                                  LeftCol%,     _
                                  BotRow%,      _
                                  RightCol%,    _
                                  ScrollLines%, _
                                  Cell,         _
                                  VideoHandle%  _
                                )


        ELSE

            '| Scroll the screen up

            Rtn% = VioScrollUp% ( TopRow%,      _
                                  LeftCol%,     _
                                  BotRow%,      _
                                  RightCol%,    _
                                  ScrollLines%, _
                                  Cell,         _
                                  VideoHandle%  _
                                )

        END IF

    END IF

END SUB

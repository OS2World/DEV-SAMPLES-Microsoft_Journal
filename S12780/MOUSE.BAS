'============================================================================
'
'    MOUSE.BAS - Mouse Support Routines for the User Interface Toolbox in
'           Microsoft BASIC 7.1, Professional Development System
'              Copyright (C) 1987-1990, Microsoft Corporation
'
' NOTE:     This sample source code toolbox is intended to demonstrate some
'           of the extended capabilities of Microsoft BASIC 7.1 Professional
'           Development system that can help to leverage the professional
'           developer's time more effectively.  While you are free to use,
'           modify, or distribute the routines in this module in any way you
'           find useful, it should be noted that these are examples only and
'           should not be relied upon as a fully-tested "add-on" library.
'
'  PURPOSE: These routines are required for mouse support in the user
'           interface toolbox, but they may be used independently as well.
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
'============================================================================

DEFINT A-Z

'$INCLUDE: 'general.bi'
'$INCLUDE: 'mouse.bi'
'$INCLUDE: 'menu.bi'

COMMON SHARED /uitools/ GloMenu      AS MenuMiscType
COMMON SHARED /uitools/ GloTitle()   AS MenuTitleType
COMMON SHARED /uitools/ GloItem()    AS MenuItemType

'| --- Modified to support OS/2 changes ---
'|
'| Added common and type statements for the OS/2 API MOU fucntions
'|

COMMON SHARED /MOUCALLS/ MouseHandle AS INTEGER

TYPE tMouseEvent
    Action    as Integer
    EventTime as Long
    MouseRow  as Integer
    MouseCol  as Integer
END TYPE

TYPE tPtrRect
    ULRow as Integer
    ULCol as Integer
    LRRow as Integer
    LRCol as Integer
END TYPE

TYPE tCell
    Char as string * 1
    Attr as string * 1
END TYPE

TYPE tMousePointer
    Mask1 as tCell
    Mask2 as tCell
END TYPE

TYPE tMouseScale
    RowScale as Integer
    ColScale as Integer
END TYPE

TYPE tMousePointerInfo
    cb as integer
    col as integer
    row as integer
    colhot as integer
    rowhot as integer
END TYPE

TYPE tMousePosition
    Row as Integer
    Col as Integer
End Type

TYPE tMouseLimits
    LLimit AS Integer
    RLimit AS Integer
    TLimit AS Integer
    BLimit AS Integer
END TYPE

TYPE tMouseQue
    Events as Integer
    MaxEvents as Integer
END TYPE

'| DECLARE Statements for OS/2 Mouse API Functions

DECLARE FUNCTION MouOpen%        ( BYVAL MouDriverAddr&, _
                                   SEG MouseHandle%      _
                                 )

DECLARE FUNCTION MouDrawPtr%     ( BYVAL MouseHandle% )

DECLARE FUNCTION MouSynch% ( BYVAL MouseWait% )

DECLARE FUNCTION MouRemovePtr%   ( SEG PtrRect as tPtrRect, _
                                   BYVAL MouseHandle%       _
                                 )

DECLARE FUNCTION MouReadEventQue% ( SEG MouseQue as tMouseEvent, _
                                    SEG MouseWait%,              _
                                    BYVAL MouseHandle%           _
                                  )

DECLARE FUNCTION MouGetNumButtons% ( SEG MouseButton%,  _
                                     BYVAL MouseHandle% _
                                   )

DECLARE FUNCTION MouSetPtrShape% ( SEG MousePointer as tMousePointer, _
                                   SEG MousePointerInfo as tMousePointerInfo, _
                                   BYVAL MouseHandle%           _
                                 )

DECLARE FUNCTION MouGetPtrShape% ( SEG MousePointer as tMousePointer, _
                                   SEG MousePointerInfo as tMousePointerInfo, _
                                   BYVAL MouseHandle%           _
                                 )


DECLARE FUNCTION MouGetPtrPos% ( SEG MousePosition as tMousePosition, BYVAL MouseHandle% )

DECLARE FUNCTION MouSetDevStatus% ( SEG MouseStatus%, BYVAL MouseHandle% )

DECLARE FUNCTION MouGetScaleFact% ( SEG MouseScale as tMouseScale, BYVAL MouseHandle% )

DECLARE FUNCTION MouSetScaleFact% ( SEG MouseScale as tMouseScale, BYVAL MouseHandle% )

DECLARE FUNCTION MouSetPtrPos% ( SEG MousePosition as tMousePosition, BYVAL MouseHandle% )

DECLARE FUNCTION MouGetNumQueEl% ( SEG MouseQue as tMouseQue, BYVAL MouseHandle% )

DECLARE FUNCTION MouFlushQue% ( BYVAL MouseHandle% )


SUB MouseBorder (row1, col1, row2, col2) STATIC

    ' =======================================================================
    ' Sets max and min bounds on mouse movement both vertically, and
    ' horizontally
    ' =======================================================================

    LCol = Col1 - 1
    RCol = Col2 - 1

    If LCol > RCol Then SWAP LCol, RCol

    TRow = Row1 - 1
    BRow = Row2 - 1

    If TRow > BRow Then SWAP TRow, BRow

    MouseDriver 7, 0, LCol, RCol
    MouseDriver 8, 0, TRow, BRow

END SUB

'| --- Modified to support OS/2 changes ---
'|
'| Modified MouseDriver SUB to process all the Mouse routines, since
'| under DOS a single INTERRUPT all the OS/2 code is located in this
'| one SUB.
'|
'| OS/2 has a function for each specific MOUSE event.

SUB MouseDriver (m0, m1, m2, m3) STATIC

    '| The reference to CALL INTERRUPT needs to be rewritten with
    '| the OS/2 API call that provides the same functionality/information

    STATIC MouseOpen     AS Integer
    STATIC MouseFound    AS Integer
    STATIC MouseHandle   AS Integer
    STATIC MouseLimits   AS tMouseLimits
    STATIC OldMouseEvent AS tMouseEvent

    DIM MouseScale       AS tMouseScale
    DIM MousePointer     AS tMousePointer
    DIM MousePointerInfo AS tMousePointerInfo
    DIM PtrRect          AS tPtrRect
    DIM MouseEvent       AS tMouseEvent
    DIM MousePosition    AS tMousePosition
    DIM HiddenMouseLoc   AS tMousePosition
    DIM QueEvents        AS tMouseEvent
    DIM MouseQue         AS tMouseQue

    If MouseOpen = 0 Then
        MouseName$ = ""
        Rtn% = MouOpen% ( SSEGADD (MouseName$), MouseHandle )

        If Rtn% = 0 Then
            MouseOpen   = TRUE
            MouseFound  = TRUE
            Rtn% = MouGetScaleFact% ( MouseScale, MouseHandle )
            MouseScale.RowScale = 8
            MouseScale.ColScale = 8
            Rtn% = MouSetScaleFact% ( MouseScale, MouseHandle )
            PtrRect.ULRow = 0
            PtrRect.ULCol = 0
            PtrRect.LRRow = 24
            PtrRect.LRCol = 79
            Rtn% = MouRemovePtr% ( PtrRect, MouseHandle )
            MousePosition.Row = 12
            MousePosition.Col = 40
            Rtn% = MouSetPtrPos% ( MousePosition, MouseHandle )
            MouseStatus% = &H0
            Rtn% = MouSetDevStatus%  ( MouseStatus%, MouseHandle )
            Rtn% = MouGetPtrShape%   ( MousePointer, MousePointerInfo, MouseHandle )
            MousePointer.Mask1.Char = Chr$(&HFF)
            MousePointer.Mask1.Attr = Chr$(3)
            MousePointer.Mask2.Char = Chr$(&H0)
            MousePointer.Mask2.Attr = Chr$(9)
            MousePointerInfo.cb     = Len (MousePointer)
            MousePointerInfo.row    = 0
            MousePointerInfo.col    = 0
            MousePointerInfo.colhot = 24
            MousePointerInfo.rowhot = 79
            Rtn% = MouSetPtrShape%   ( MousePointer, MousePointerInfo, MouseHandle )
            MouseLimits.RLimit = 79
            MouseLimits.LLimit = 0
            MouseLimits.TLimit = 0
            MouseLimits.BLimit = 24
        Else
            MouseOpen    = TRUE
            MouseFound   = FALSE
        End If
    End If

    If MouseFound = FALSE then
        Exit Sub
    End If

    SELECT CASE m0

    CASE 0

        '| MouseInit ( 0, 0, 0, 0 )

        Rtn% = MouGetNumButtons% ( MouseButtons%, MouseHandle )
        m1   = MouseButtons%

    CASE 1

        '| MouseShow ( 1, 0, 0, 0 )

        MouseStatus% = &H0
        Rtn% = MouSetDevStatus%  ( MouseStatus%, MouseHandle )
        Rtn% = MouDrawPtr% ( MouseHandle )

    CASE 2

        '| MouseHide ( 2, 0, 0, 0 )

        MouseStatus% = &H0100
        Rtn% = MouSetDevStatus%  ( MouseStatus%, MouseHandle )

    CASE 3

        '| MousePoll ( 3, x, x, x )

        MouseWait% = 1
        Rtn% = MouSynch% ( MouseWait% )
        MouseWait% = 0
        Rtn% = MouReadEventQue% ( MouseEvent, MouseWait%, MouseHandle )

        If MouseEvent.EventTime = 0 Then
            If OldMouseEvent.EventTime = 0 Then
                Rtn% = MouGetPtrPos% ( MousePosition, MouseHandle )
                MouseEvent.Action   = OldMouseEvent.Action
                MouseEvent.MouseCol = MousePosition.col
                MouseEvent.MouseRow = MousePosition.row
            Else
                MouseEvent = OldMouseEvent
            End If
        End If

        Rtn% = MouGetNumQueEl% ( MouseQue, MouseHandle )

        While MouseQue.Events > 1
            Rtn% = MouReadEventQue% ( QueEvents, MouseWait%, MouseHandle )
            Rtn% = MouGetNumQueEl% ( MouseQue, MouseHandle )
        Wend

        m1 = 0

        If MouseEvent.Action and &H0004 Then   m1 = m1 + 2 ^ 0
        If MouseEvent.Action and &H0010 Then   m1 = m1 + 2 ^ 1
        If MouseEvent.Action and &H0040 Then   m1 = m1 + 2 ^ 2
        If MouseEvent.Action and &H0002 Then   m1 = m1 + 2 ^ 0
        If MouseEvent.Action and &H0008 Then   m1 = m1 + 2 ^ 1
        If MouseEvent.Action and &H0020 Then   m1 = m1 + 2 ^ 2

        m2 = MouseEvent.MouseCol
        m3 = MouseEvent.MouseRow

        OldMouseEvent = MouseEvent

        '| The following section of code checks to see if the mouse pointer
        '| is moving out of the Horizontal and Vertical limits.

        InLimits% = FALSE

        If m2 < MouseLimits.LLimit then

            '| Set it to the min.

            InLimits% = TRUE

            MousePosition.Col = MouseLimits.LLimit
            m2 = MouseLimits.LLimit
        ElseIF m2 > MouseLimits.RLimit then

            '| Set it to the Max.

            InLimits% = TRUE

            MousePosition.Col = MouseLimits.RLimit
            m2 = MouseLimits.RLimit
        Else
            MousePosition.Col = m2
        End If

        If m3 < MouseLimits.TLimit then

            '| Set it to the min.

            InLimits% = TRUE

            MousePosition.Row = MouseLimits.TLimit
            m3 = MouseLimits.TLimit

        ElseIF m3 > MouseLimits.BLimit then

            '| Set it to the Max.

            InLimits% = TRUE

            MousePosition.Row = MouseLimits.BLimit
            m3 = MouseLimits.BLimit
        Else
            MousePosition.Row = m3
        End If

        If InLimits% = TRUE Then
            Rtn% = MouSetPtrPos ( MousePosition, MouseHandle )
        End If

    CASE 7

        '| There are not LIMIT Rectangles when using the OS/2 Mouse API
        '| calls, so we have to provide the limits in the code (at this
        '| level)  We are setting the limits for the horizontal coordinates.

        '| We are responsible for making sure the ranges are in correct
        '| order, since the INTERRUPT Service does this, not the software.

        '| This section of code helps provide the INTERRUPT Service of
        '| INT 33h Function 07h "Set horizontal limits for pointer"

        If m2 > m3 then SWAP m2, m3

        MouseLimits.LLimit = m2
        MouseLimits.RLimit = m3

    CASE 8

        '| There are not LIMIT Rectangles when using the OS/2 Mouse API
        '| calls, so we have to provide the limits in the code (at this
        '| level).  We are setting the limits for the Vertical coordinates.

        '| We are responsible for making sure the ranges are in correct
        '| order, since the INTERRUPT Service does this, not the software.

        '| This section of code helps provide the INTERRUPT Service of
        '| INT 33h Function 08h "Set vertical limits for pointer"

        If m2 > m3 then SWAP m2, m3

        MouseLimits.TLimit = m2
        MouseLimits.BLimit = m3

    CASE ELSE

        '| UnKnown Call

    END SELECT

END SUB


SUB MouseHide

    ' =======================================================================
    ' Decrements internal cursor flag
    ' =======================================================================

   MouseDriver 2, 0, 0, 0

END SUB

SUB MouseInit

    ' =======================================================================
    ' Mouse driver's initialization routine
    ' =======================================================================

    MouseDriver 0, 0, 0, 0

END SUB

SUB MousePoll (row, col, lButton, rButton) STATIC

    ' =======================================================================
    ' Polls mouse driver, then sets parms correctly
    ' =======================================================================

    MouseDriver 3, button, col, row
    row = row + 1 '| row / 8 + 1
    col = col + 1 '| col / 8 + 1
                                                
    IF button AND 1 THEN
        lButton = TRUE
    ELSE
        lButton = FALSE
    END IF

    IF button AND 2 THEN
        rButton = TRUE
    ELSE
        rButton = FALSE
    END IF

END SUB

SUB MouseShow

    ' =======================================================================
    ' Increments mouse's internal cursor flag
    ' =======================================================================

    MouseDriver 1, 0, 0, 0

END SUB


'| =========================================================================
'|
'| UIASM.BAS
'|
'| This BASIC source file will contain the BASIC source code for the
'| UIASM.ASM routines provided with the BASIC 7.00 and 7.10 user-toolbox.
'|
'| These routines are currently written an assembler for speed.  This file
'| contains the equvilent BASIC routines using OS/2 VIO API calls.
'|
'| The OS/2 VIO API calls are written in assembler and still provide a
'| comparable exection time.  Even-though the majority if the code has
'| been rewritten into BASIC.
'|
'| The routine names and prototypes are as follows :
'|
'|     GetCopyBox : Gets screen box info and places the info into a string
'|                  variable.
'|
'|       DECLARE SUB GetCopyBox (ULRow%, ULCow%, LRRow%, LRCol%, buffer$)
'|
'|
'|     PutCopyBox : Puts screen box info back on the screen from a string
'|                  variable.
'|
'|       DECLARE SUB PutCopyBox (ULRow%, ULCow%, buffer$)
'|
'|
'|     AttrBox    : Canges the color attibutes of all characters with a box
'|
'|       DECLARE SUB AttrBox    (ULRow%, ULCow%, LRRow%, LRCol%, NewAttr%)
'|
'| In the above parameter list the beginning two characters specify the
'| portion of the rectangle of the box being refered to.
'|
'|      UL = Upper-Left corner of the rectangle/box
'|      LR = Lower-Right corner of the rectangle/box
'|
'| =========================================================================


'| =========================================================================
'|
'| COMMON Statements used by the three routines
'|
'| =========================================================================

COMMON SHARED /VioUIASM/	VideoHandle%


'| =========================================================================
'|
'| DECLARE Statements for the VIO routines
'|
'| =========================================================================

DECLARE FUNCTION VioReadCellStr% ( BYVAL CellAdd&, _
				   SEG	 NumCells%, _
				   BYVAL SRow%,	 _
				   BYVAL SCol%,	 _
				   BYVAL VidHandle% _
				 )

DECLARE FUNCTION VioWrtCellStr%  ( BYVAL CellAdd&,	 _
				   BYVAL	NumCells%,	 _
				   BYVAL	SRow%,		 _
				   BYVAL	SCol%,		 _
				   BYVAL	VidHandle%	 _
				 )


'| =========================================================================

SUB GetCopyBox (ULRow%, ULCol%, LRRow%, LRCol%, ScreenBuffer$)

	'| Subtract 1 from all screen coordinates to reflect the difference
	'| between BASIC's screen BASE 1 and the VIO API BASE 0.
	'|
	'| We also make a copy of the variables, since we do not force the
	'| caller to use BYVAL or by reference calling convention.
	'|
	'| UL = Upper-Left corner of the rectangle/box
	'| LR = Lower-Right corner of the rectangle/box

	URow% = ULRow% - 1
	UCol% = ULCol% - 1

	LRow% = LRRow% - 1
	LCol% = LRCol% - 1

	'| Find width and height of the Box

	BoxHeigth% = LRow% - URow% + 1
	BoxWidth% = LCol% - UCol% + 1

	'| Calculate the actual width of the line before the For-loop
	'| thus performing the operation once.  We need a copy of the
	'| results, since the function call changes the value of
	'| parameter passed to it.

	ActualWidth% = BoxWidth% * 2
	OldBoxWidth% = ActualWidth%

	For i% = URow% to LRow%

		Work$ = Space$ (ActualWidth%)

		'| Read a one line from the Box of BoxWidth% long into
		'| a Work String.  The format of the string is
		'| character + attribute (every two bytes make a cell)

		VIORt% = VioReadCellStr ( SSEGADD (Work$), _
					  ActualWidth%,	   _
					  i%,			   _
					  UCol%,		   _
					  VideoHandle%	   _
					)

		'| VioReadCellStr changes the value "ActualWidth%" to reflect
		'| how many characters where actual placed in to the buffer.
		'|
		'| We reset the value after every call, so we don't lose one
		'| character and attribute each type through the For-Loop.

		ActualWidth% = OldBoxWidth%

		'| We calculate the offset into the string where we
		'| will place the line of text.

		ScrnOff% = ActualWidth% * (i% - URow%) + 3

		'| By using the Mid$ statement, we can place the information
		'| over the existing information with little work on our part.

		Mid$ (ScreenBuffer$, ScrnOff%, Len (Work$) ) = Work$

	Next i%

	'| Store the Width and Height of the BOX in the first
	'| two characters of the buffer.  The Height will be on the
	'| BASE of ONE, and PutCopyBox will convert it to a BASE zero.

	Mid$(ScreenBuffer$, 1, 1) = chr$(BoxWidth%)
	Mid$(ScreenBuffer$, 2, 1) = chr$(BoxHeigth%)

END SUB

SUB PutCopyBox (ULRow%, ULCol%, ScreenBuffer$)

	'| Subtract 1 from all screen coordinates to reflect the difference
	'| between BASIC's screen BASE 1 and the VIO API BASE 0.
	'|
	'| We also make a copy of the variables, since we do not force the
	'| caller to use BYVAL or by reference calling convention.
	'|
	'| UL = Upper-Left corner of the rectangle/box
	'| LR = Lower-Right corner of the rectangle/box

	URow% = ULRow% - 1
	UCol% = ULCol% - 1

	'| The first two bytes are the Width and Heigth used by GetCopyBox.
	'| Where the Heigth is from BASE 1 rather then ZERO

	BoxWidth%  = ASC(Mid$ (ScreenBuffer$, 1, 1))
	BoxHeigth% = ASC(Mid$ (ScreenBuffer$, 2, 1)) - 1

	'| We calculate the actual width of the line before the loop,
	'| rather then inside the loop.

	ActualWidth% = BoxWidth% * 2

    For i% = 0 to BoxHeigth%

		'| Calculate the offset into the string for each line of
		'| the box.

		ScrnOff% = ActualWidth% * i% + 3

		'| The actual row on the screen that we a printing to

		NewRow% = URow% + i%

		'| We extract a line of text with the attibutes in a
		'| similar manner to how we placed them in the string.

		Work$ = Mid$ (ScreenBuffer$, ScrnOff%, ActualWidth%)

		'| Write the line of text and attributes to the screen

		VIORt% = VioWrtCellStr ( SSEGADD (Work$), _
					 ActualWidth%,	  _
					 NewRow%,		  _
					 UCol%,			  _
					 VideoHandle%	  _
				       )

	Next i%

END SUB

SUB AttrBox (ULRow%, ULCol%, LRRow%, LRCol%, NewAttr%)

	'| Subtract 1 from all screen coordinates to reflect the difference
	'| between BASIC's screen BASE 1 and the VIO API BASE 0.
	'|
	'| We also make a copy of the variables, since we do not force the
	'| caller to use BYVAL or by reference calling convention.
	'|
	'| UL = Upper-Left corner of the rectangle/box
	'| LR = Lower-Right corner of the rectangle/box

	URow% = ULRow% - 1
	UCol% = ULCol% - 1

	LRow% = LRRow% - 1
	LCol% = LRCol% - 1

	'| Find width and height of the Box

	BoxHeigth% = LRow% - URow% + 1
	BoxWidth% = LCol% - UCol% + 1

	'| Calculate the actual width of the line before the For-loop
	'| thus performing the operation once.  We need a copy of the
	'| results, since the function call changes the value of
	'| parameter passed to it.

	ActualWidth% = BoxWidth% * 2
	OldBoxWidth% = ActualWidth%

    For i% = URow% to LRow%

		Work$ = Space$ (ActualWidth%)

		'| Read a one line from the Box of BoxWidth% long into
		'| a Work String.  The format of the string is
		'| character + attribute (every two bytes make a cell)

		VIORt% = VioReadCellStr ( SSEGADD (Work$), _
					  ActualWidth%,	   _
					  i%,			   _
					  UCol%,		   _
					  VideoHandle%	   _
					)

		'| VioReadCellStr changes the value "ActualWidth%" to reflect
		'| how many characters where actual placed in to the buffer.
		'|
		'| We reset the value after every call, so we don't lose one
		'| character and attribute each type through the For-Loop.

		ActualWidth% = OldBoxWidth%

		For j% = 2 to ActualWidth% STEP 2

			Mid$(Work$, j%, 1) = chr$ (NewAttr%)

		Next j%

		VIORt% = VioWrtCellStr ( SSEGADD (Work$), _
					 ActualWidth%,	  _
					 i%,			  _
					 UCol%,			  _
					 VideoHandle%	  _
				   )

	Next i%

END SUB

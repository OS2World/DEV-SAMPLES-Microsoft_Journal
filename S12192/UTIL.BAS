'===== UTIL.BAS =====
' This file contains some useful routines for use with
' the MS OS/2 SDK.
CONST TRUE = -1
CONST FALSE = 0
TYPE ADDRESS
     offset  AS INTEGER
     segment AS INTEGER
END TYPE
DEFINT A-Z

DECLARE FUNCTION WDate$ (P1 AS INTEGER)
DECLARE FUNCTION WTime$ (P1 AS INTEGER)
DECLARE FUNCTION RightShift% (P1 AS LONG, P2 AS INTEGER)
DECLARE FUNCTION LeftShift% (P1 AS LONG, P2 AS INTEGER)
DECLARE FUNCTION FileType$ (P1 AS INTEGER)
DECLARE FUNCTION Unsigned& (P1 AS INTEGER)
DECLARE FUNCTION StringPeek% (P1 AS ADDRESS, P2 AS STRING, _
                              P3 AS INTEGER)
DECLARE FUNCTION IntegerPeek% (P1 AS ADDRESS, P2 AS INTEGER)
DECLARE FUNCTION LongPeek% (P1 AS ADDRESS, P2 AS LONG)
DECLARE FUNCTION SinglePeek% (P1 AS ADDRESS, P2 AS SINGLE)
DECLARE FUNCTION DoublePeek% (P1 AS ADDRESS, P2 AS DOUBLE)
DECLARE FUNCTION stringpoke% (info AS ANY, st$, strlen%)

' ERROR HANDLER
ErrorHandler:
   status% = TRUE
   RESUME NEXT

'============================================================
'= DoublePeek : Finds a double at a given segment and offset
'= Arguments
'=      INFO    : structure containing segment and offset
'=      Number# : double to be returned
'=
'= Return
'=     TRUE    : If an error occurs
'=     FALSE   : If everything went OK
FUNCTION DoublePeek% (info AS ADDRESS, number#)
SHARED status%
ON ERROR GOTO ErrorHandler
status% = FALSE
  DEF SEG = info.segment
  i = 0
  WHILE (NOT status%) AND (i < 8)
      hold$ = hold$ + CHR$(PEEK(info.offset + i))
      PRINT PEEK(info.offset + i);
      i = i + 1
  WEND
  DEF SEG
  number# = CVD(hold$)
  DoublePeek% = status%
END FUNCTION

'==========================================================
'= FileType$ : Determine the type of file by its attributes
'= Arguments
'=      attr : Attribute Number
'=
'= Return
'=      String containing the file type
FUNCTION FileType$ (attr)
  SELECT CASE attr
     CASE 0
        FileType$ = "Normal File"
     CASE 1
        FileType$ = "Read-Only File"
     CASE 2
        FileType$ = "Hidden File"
     CASE 4
        FileType$ = "System File"
     CASE &H10
        FileType$ = "Subdirectory"
     CASE &H20
        FileType$ = "File Archive"
     CASE ELSE
        FileType$ = "Unknown Type"
  END SELECT
END FUNCTION

'==========================================================
'= IntegerPeek% : Finds an integer at a given segment and
'=                offset
'= Arguments
'=      INFO    : structure containing segment and offset
'=      Number% : integer to be returned
'=
'= Return
'=      TRUE    : If an error occurs
'=      FALSE   : If everything went OK
FUNCTION IntegerPeek% (info AS ADDRESS, number%)
SHARED status%
ON ERROR GOTO ErrorHandler
status% = FALSE
  DEF SEG = info.segment
  i = 0
  WHILE (NOT status%) AND (i < 2)
      hold$ = hold$ + CHR$(PEEK(info.offset + i))
      i = i + 1
  WEND
  DEF SEG
  number% = CVI(hold$)
  interpeek% = status%
END FUNCTION

'==========================================================
'= LeftShift% : Shift Bits to the left ====================
'= Arguments
'=      Number : Long to be shifted (unsigned integer)
'=      Amount : Amount to be shifted
'=
'= Return
'=      The shifted SIGNED integer
FUNCTION LeftShift% (number&, amount)
  LeftShift = number& * (2 ^ amount)
END FUNCTION

'==========================================================
'= LongPeek% : Finds a long at a given segment and offset
'= Arguments
'=      INFO    : structure containing segment and offset
'=      Number& : long to be returned
'=
'= Return
'=     TRUE    : If an error occurs
'=     FALSE   : If everything went OK
FUNCTION LongPeek% (info AS ADDRESS, number&)
SHARED status%
ON ERROR GOTO ErrorHandler
status% = FALSE
  DEF SEG = info.segment
  i = 0
  WHILE (NOT status%) AND (i < 4)
      PRINT PEEK(info.offset + i)
      hold$ = hold$ + CHR$(PEEK(info.offset + i))
      i = i + 1
  WEND
  DEF SEG
  number& = CVL(hold$)
  LongPeek% = status%
END FUNCTION

'==========================================================
'= RightShift% : Shift bits to the right ==================
'= Arguments
'=      Number : Long to be shifted (unsigned integer)
'=      Amount : Amount to be shifted
'=
'= Return
'=      The shifted SIGNED integer
FUNCTION RightShift% (number&, amount)
   RightShift = number& \ 2 ^ amount
END FUNCTION

'============================================================
'= SinglePeek! : Finds a single at a given segment and offset
'= Arguments
'=      INFO    : structure containing segment and offset
'=      Number! : single to be returned
'=
'= Return
'=     TRUE    : If an error occurs
'=     FALSE   : If everything went OK
FUNCTION SinglePeek% (info AS ADDRESS, number!)
SHARED status%
ON ERROR GOTO ErrorHandler
status% = FALSE
  DEF SEG = info.segment
  i = 0
  WHILE (NOT status%) AND (i < 4)
      hold$ = hold$ + CHR$(PEEK(info.offset + i))
      PRINT PEEK(info.offset + i);
      i = i + 1
  WEND
  DEF SEG
  number! = CVS(hold$)
  SinglePeek% = status%
  PRINT
END FUNCTION

'==========================================================
'= StringPeek% : Given segment and offset create a string
'=               with length STRLEN
'= Arguments
'=      INFO   : structure containing segment and offset
'=      ST$    : String to be returned
'=      STRLEN : Length max length of the string
'=               If the a NULL is found before the counter
'=               is greater than max length, the new length
'=               of the string is returned in STRLEN.
'= Return
'=     TRUE    : If an error occurs
'=     FALSE   : If everything went OK
FUNCTION StringPeek% (info AS ADDRESS, st$, strlen)
DIM null AS STRING * 1
SHARED status%
ON ERROR GOTO ErrorHandler
   null = CHR$(0)
   incr = 0
   st$ = null
   status% = FALSE
   DEF SEG = info.segment
   DO
     c$ = CHR$(PEEK(info.offset + incr))
     st$ = st$ + c$
     incr = incr + 1
   LOOP WHILE ((c$ <> null) AND (incr < strlen) AND (NOT status%))
   strlen = incr
   StringPeek% = status%
END FUNCTION

'==========================================================
'= stringpoke% : Poke a given string into the segment and
'=               offset provided
'= Arguments
'=      INFO   : structure containing the segment and offset
'=      ST$    : the string
'=      Strlen : length of the string
'=
'= Return
'=     TRUE    : If an error occurs
'=     FALSE   : If everything went OK
'=
'= Notes
'=    This function can also be used for placing numbers into
'=    memory. MKx$ can be used to convert the number to a
'=    string.  This string can be passed to the routine.
FUNCTION stringpoke% (info AS ADDRESS, st$, strlen)
DIM null AS STRING * 1
SHARED status%
ON ERROR GOTO ErrorHandler
   incr = 0
   status% = FALSE
   DEF SEG = info.segment
   DO
     POKE info.offset + incr, ASC(MID$(st$, incr + 1, incr + 1))
     incr = incr + 1
   LOOP WHILE ((incr < strlen) AND (NOT status%))
  strlen = incr
  DEF SEG
  stringpoke% = status%
END FUNCTION

'==========================================================
'= Unsigned& : Convert signed integer to unsigned long ====
'= Arguments
'=      NUM% : Signed integer to be converted to unsigned
'=             long
'= Return
'=      Long which is the unsigned integer
FUNCTION Unsigned& (num)
   IF num >= 0 THEN
      Unsigned& = num
   ELSE
      Unsigned& = 65536 + num
   END IF
END FUNCTION

'==========================================================
'= WDate$ : FUNCTION to print file date returned by FindNext
'= Arguments
'=      d%  : Number to be printed as the date
'=
'= Return
'=      String containing the date
FUNCTION WDate$ (d%) STATIC
DIM dl AS LONG
    dl = Unsigned&(d%)
    mn = (RightShift%(dl, 5)) AND (&HF)
    IF mn < 10 THEN
       mn$ = "0" + LTRIM$(STR$(mn))
    ELSE
       mn$ = LTRIM$(STR$(mn))
    END IF
    dy = dl AND (&H1F)
    IF dy < 10 THEN
       dy$ = "0" + LTRIM$(STR$(dy))
    ELSE
       dy$ = LTRIM$(STR$(dy))
    END IF
    yr$ = STR$(RightShift(dl, 9) + 1980)
    WDate$ = mn$ + "/" + dy$ + "/" + LTRIM$(yr$)
END FUNCTION

'==========================================================
'= WTime$ : FUNCTION to print file time returned by FindNext
'= Arguments
'=      d%  : Number to be printed as the time
'=
'= Return
'=      String containing the time
FUNCTION WTime$ (d%)
DIM dl AS LONG
   dl = Unsigned&(d%)
   hr = RightShift%(dl, 11) AND (&H1F)
   IF hr < 10 THEN
      hr$ = "0" + LTRIM$(STR$(hr))
   ELSE
      hr$ = LTRIM$(STR$(hr))
   END IF
   mt = (RightShift%(dl, 5) AND (&H3F))
   IF mt < 10 THEN
      mt$ = "0" + LTRIM$(STR$(mt))
   ELSE
      mt$ = LTRIM$(STR$(mt))
   END IF
   WTime$ = LTRIM$(hr$) + ":" + mt$ + STRING$(5, 32)
END FUNCTION

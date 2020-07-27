include dosmac.inc
include devsym.inc
include devhlp.inc
include error.inc
;
; Copyright (c) Microsoft Corporation,  1988
;

;=========================================================================
;
; Local Data Area
;   LocalData structure is allocated on the stack and intialized by the
;   Strategy routine for each device driver instance.
;
LOCALDATA STRUC
    LD_PTRSAVE 	DD   (?)		; Far pointer to the request header
LOCALDATA ENDS

LONG_PTR STRUC
	_OFF	DW	(?)	;Offset of far pointer
	_SEG	DW	(?)	;Segment of far pointer
LONG_PTR ENDS

PHYSADDR STRUC
	_LO	DW	(?)	;Low word of a 32-bit pointer
	_HI	DW	(?)	;High word of a 32-bit pointer
PHYSADDR ENDS

DEVHELP macro   function
	mov	dl,function
        call    dword ptr [DevHlp]
	endm

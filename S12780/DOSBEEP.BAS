
'| *********************************************************************** |'
'|                                                                         |'
'| Function : DosBeep ( Frequency, Duration )                              |'
'|                                                                         |'
'| Description :                                                           |'
'|                                                                         |'
'| The BASIC SOUND statement is not supported under OS/2, instead we use   |'
'| the OS/2 DOSBEEP function.                                              |'
'|                                                                         |'
'| *********************************************************************** |'


DECLARE FUNCTION DosBeep%      ( BYVAL Freq%, _
                                 BYVAL Durat% _
                               )

SUB BSound (Freq%, Durat%)

    '|
    '| BSOUND is used in the same fashion as the BASIC SOUND statement.
    '|
    '| It will also generate a BASIC "Illegal Function Call" when it
    '| fails, just like the SOUND statement.
    '|

	Rtn% = DosBeep% (Freq%, Durat%)

	IF Rtn% THEN	'| Issue an Invalid Function Call Error
	   ERROR 5	'| when an error occurs just like the
	END IF		'| BASIC SOUND statement.

END SUB

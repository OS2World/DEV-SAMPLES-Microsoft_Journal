; these two extra segments are retained even afetr the initialization is
; completed.


HIGH_DATA       SEGMENT WORD PUBLIC 'FAR_DATA'

Count    DW    0

HIGH_DATA       ENDS



HIGH_CODE       SEGMENT WORD PUBLIC 'FAR_CODE'

HighTimer       PROC    FAR
                PUBLIC  HighTimer

    inc     ds:count                              ; increment the count
                                                  ;
    ret

HighTimer       ENDP

HIGH_CODE       ENDS

                END

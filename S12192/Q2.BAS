'===== Q2.BAS =====
REM $INCLUDE: 'QUEUE.BI'

'Functions to place and retrieve data with shared memory (UTIL.BAS)
DECLARE FUNCTION STRINGPOKE%(_
              P1  AS ADDRESS,_
              P2  AS STRING,_
              P3  AS INTEGER)
DECLARE FUNCTION STRINGPEEK%(_
              P1  AS ADDRESS,_
              P2  AS STRING,_
              P3  AS INTEGER)
DEFINT A-Z
DIM info AS ADDRESS
DIM ep  AS STRING*1

'Queue Name
QNAME$="\QUEUES\NANCYBA"+CHR$(0)

'Shared memory name
DNAME$="\SHAREMEM\NANCYBA"+CHR$(0)

CLS
'Get the shared segment
x=DosGetShrSeg%(varseg(dname$),sadd(dname$),selector)
IF (x) THEN
   Print "An error occurred.  The number is : ";x
   stop
ELSE
   Print "The SEGMENT ";selector;" was CREATED"
END IF
'To open Queue all that is needed is the Queue name
x=DosOpenQueue%(PID,Qhandle,varseg(Qname$),sadd(Qname$))
IF (x) THEN
   Print "An error occurred.  The number is : ";x
   STOP
ELSE
   Print "The QUEUE is OPENED."
   Print "The HANDLE is : ";qhandle
END IF
FOR i = 0 to 6
   info.segment=selector
   info.offset=i*10
   st$="ITEM"+str$(i)
   strlen=len(st$)
   IF (stringpoke%(info,st$,strlen)) THEN
      PRINT "ERROR"
   ELSE
      x=DosWriteQueue%(QHandle,i+5,strlen,_
                       info.segment,info.offset,0)
      IF (x) THEN
         Print "An error occurred.  The number is : ";x
      ELSE
              Print "The Item was written to the QUEUE"
     END IF
   END IF
NEXT i
Print "Enter any key to quit..."
while inkey$="" :WEND
END

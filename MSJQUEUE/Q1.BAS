'===== Q1.BAS =====
REM $include: 'QUEUE.BI'

'Functions to deal with data in shared memory (UTIL.BAS)
DECLARE FUNCTION STRINGPOKE%(_
              P1  AS ADDRESS,_
              P2  AS STRING,_
              P3  AS INTEGER)
DECLARE FUNCTION STRINGPEEK%(_
              P1  AS ADDRESS,_
              P2  AS STRING,_
              P3  AS INTEGER)
DEFINT A-Z
DIM info  AS ADDRESS
DIM ep    AS STRING*1

'Queue name
QNAME$="\QUEUES\NANCYBA"+CHR$(0)

'Shared memory
DNAME$="\SHAREMEM\NANCYBA"+CHR$(0)

QUEUETYPE=0    'First In First Out
CLS

'Create Queue
x=DosCreateQueue%(QHandle,QueueType,varseg(Qname$),sadd(Qname$))
IF (x) THEN
   Print "An error occurred.  The number is : ";x
   stop
ELSE
   Print "The QUEUE ";qname$;" was CREATED"
   PRINT "The HANDLE is : ";qhandle
END IF

'Create shared segment
SIZE=1024
ALLOCFLAGS=7
x=DosAllocShrSeg%(size,varseg(dname$),sadd(dname$),selector)
IF (x) THEN
   Print "An error occurred.  The number is : ";x
   stop
ELSE
   Print "The SEGMENT ";selector;" was CREATED"
END IF
PRINT "WAITING FOR INPUT TO THE QUEUES"
DO
   x=DosQueryQueue%(Qhandle,number)
' Loop until error or number > 0 :
LOOP WHILE ( (x = 0)  and (number = 0) )
IF (x) THEN
   Print "An Error Occurred.  The number is : ";x
ELSE
   PRINT "There are ";number;" items in the QUEUE"
   FOR i = 1 to number
      x=DosReadQueue%(Qhandle,pr&,dlen,item&,0%,0%,_
                      varseg(ep),varptr(ep),1&)
      IF (x) THEN
         Print "An error occurred.  The number is : ";x
      ELSE
         info.segment=item& \ (2^16)
         info.offset=item& mod (2^16)
         IF (stringpeek(info,st$,dlen)) THEN
            PRINT "ERROR"
         ELSE
            Print "The item in the QUEUE was : ";st$
         END IF
      END IF
   NEXT i
END IF
x=DosCloseQueue%(Qhandle)
IF (x) THEN
   Print "An error occurred.  The number is : ";x
ELSE
   Print "The QUEUE was closed."
END IF
END

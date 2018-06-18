'===== Q3.BAS =====
REM $INCLUDE: 'queue.bi'

'Function for dealing with shared memory (UTIL.BAS)
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
QNAME$="\QUEUES\NANCY"+CHR$(0)
QUEUETYPE=0    'FIFO

'Create the Queue
PRINT
PRINT "Creating the Queue"
PRINT
x=DosCreateQueue%(QHandle,QueueType,varseg(Qname$),sadd(Qname$))
IF (x) THEN
   Print "An error occurred.  The number is : ";x
   stop
ELSE
   Print "The QUEUE ";qname$;" was CREATED"
   PRINT "The HANDLE is : ";qhandle
END IF

'Allocate memory for the Queue
PRINT
PRINT "Allocating Memory"
PRINT
SIZE=1024
ALLOCFLAGS=7
x=DosAllocSeg%(size,selector,allocflags)
IF (x) THEN
   Print "An error occurred.  The number is : ";x
   stop
ELSE
   Print "The SEGMENT ";selector;" was CREATED"
END IF

'Place the data into memory and write the item to the Queue
PRINT
PRINT "Writing Data to the Queue
PRINT
FOR i = 0 to 5
   info.segment=selector
   info.offset=i*10
   st$="ITEM"+str$(i)
   strlen=len(st$)
   IF (stringpoke%(info,st$,strlen)) THEN
      PRINT "ERROR"
   ELSE
      x=DosWriteQueue%(QHandle,i+5,strlen,info.segment,info.offset,0)
      IF (x) THEN
         Print "An error occurred.  The number is : ";x
     END IF
   END IF
NEXT i
PRINT
PRINT "Loop until information is in the Queue"
PRINT
'Loop until an error or item in the Queue
DO
   x=DosQueryQueue%(Qhandle,number)
'loop until error or number > 0:
LOOP WHILE ( (x <> 0)  and (number <> 0) )

PRINT
PRINT "Read the Information from the Queue"
PRINT
'If error continue or read from the Queue
IF (x) THEN
   Print "An Error Occurred.  The number is : ";x
ELSE
   PRINT "There are ";number;" items in the QUEUE"
   FOR i = 1 to number
      x=DosReadQueue%(Qhandle,pr&,dlen,_
                      item&,0%,1%,varseg(ep),varptr(ep),1&)
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
PRINT "Hit Any Key to Continue..."
WHILE INKEY$="" : WEND
PRINT
PRINT "Writing More information to the Queue"

'Write more information into the Queue
FOR i = 6 to 11
   info.segment=selector
   info.offset=i*10
   st$="ITEM"+str$(i)
   strlen=len(st$)
   IF (stringpoke%(info,st$,strlen)) THEN
      PRINT "ERROR"
   ELSE
      x=DosWriteQueue%(QHandle,i+5,strlen,info.segment,info.offset,0)
      IF (x) THEN
         Print "An error occurred.  The number is : ";x
     END IF
   END IF
NEXT i
'loop until error or number > 0
DO
   x=DosQueryQueue%(Qhandle,number)
LOOP WHILE ( (x <> 0)  and (number <> 0) )
PRINT
PRINT "Peeking in the Queue (Note: The items aren't removed)"
PRINT

'Look at items in the Queue, but don't remove them
IF (x) THEN
   Print "An Error Occurred.  The number is : ";x
ELSE
   PRINT "There are ";number;" items in the QUEUE"
   FOR i = 1 to number
      x=DosPeekQueue%(Qhandle,pr&,dlen,item&,elm,1%,_
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
PRINT
PRINT "Peek Complete. View number of items in the Queue"
PRINT

'See how many items are left
x=DosQueryQueue%(Qhandle,number)
IF (x) THEN
   Print "An error occurred.  The number is : ";x
ELSE
   PRINT "There are ";number;" items in the QUEUE"
END IF
'Delete the items in the Queue
PRINT
PRINT "Purge the Queue"
PRINT
x=DosPurgeQueue%(Qhandle)
IF (x) THEN
   Print "An error occurred.  The number is : ";x
ELSE
   Print "The QUEUE was PURGED"
END IF

'See how many items are left
x=DosQueryQueue%(Qhandle,number)
IF (x) THEN
   Print "An error occurred.  The number is : ";x
ELSE
   PRINT "There are ";number;" items in the QUEUE"
END IF
PRINT
PRINT "Close the Queue"
PRINT
'Close the Queue
x=DosCloseQueue%(Qhandle)
IF (x) THEN
   Print "An error occurred.  The number is : ";x
ELSE
   Print "The QUEUE was closed."
END IF
END

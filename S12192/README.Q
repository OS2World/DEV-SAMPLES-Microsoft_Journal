This is an example of using OS/2 queues from compiled BASIC. In this 
example, two compiled BASIC processes running in two different OS/2 
screen groups communicate with each other through a queue. The example 
uses the following MS OS/2 API calls:

   DosCreateQueue
   DosOpenQueue
   DosReadQueue
   DosWriteQueue
   DosQueryQueue
   DosCloseQueue
   DosAllocShrSeg
   DosGetShrSeg

This information applies to Microsoft BASIC Compiler Versions 6.00 and
6.00b for MS OS/2, and to Microsoft BASIC Professional Development 
System (PDS) Version 7.00 for MS OS/2. (This information does not 
apply to Microsoft QuickBASIC, which supports only the MS-DOS 
operating system.)

More Information:

The following files make up this example:

1. Q1.BAS - A program that creates/reads to a Queue

2. Q2.BAS - A program that opens/writes to a Queue

3. QUEUE.BI - An include file used by Q1.BAS and Q2.BAS

4. UTIL.BAS - BASIC routines to read/write shared memory

5. README.Q - Text file describing how to compile, link, and run this 
              example.

The following are compile and LINK instructions:

1.  BC q1;

2.  BC q2;

3.  BC util /x;

4.  LINK q1 util;

5.  LINK q2 util;

The following are execution instructions:

1. Create two OS/2 protected mode sessions.

2. Run Q1 in one session. The program performs the following
   sequence of steps:

   a. Create the Queue (DosCreateQueue).

   b. Allocate Shared Segment (DosAllocShrSeg).

   c. Query the Queue until an error or an item is entered
      (DosQueryQueue).

   d. Read the items out of the Queue (DosReadQueue).

   e. Close the Queue (DosCloseQueue).

3. When Q1 is waiting for Queue information, start Q2 in the other
   MS OS/2 protected mode session. The program performs the following
   sequence of steps:

   Get the segment shared in Q1 (DosGetShrSeg)
   Open the Queue (DosOpenQueue)
   Write to the Queue (DosWriteQueue)
   Exit the program

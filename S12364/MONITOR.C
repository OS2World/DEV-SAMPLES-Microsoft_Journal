/* monitor.c  - Switch through screen groups
 *
 *  Operation Note :
 *
 *
 *      In order to successfully switch between PM screen groups, this
 *      monitor MUST be started from within a windowed screen group. It
 *      functions in both detached and foreground modes.
 *
 *  General Monitor Notes:
 *
 *
 *      The first 16 bytes of a monitor buffer are devoted to the
 *      header, as described by the MonBuff structure below.  Prior to
 *      registering, the first word (bufflen) must be set to the size
 *      of the entire buffer itself.  The entire buffer includes the
 *      header (16 bytes) and room for at least one data record (16 bytes
 *      in the case of the keyboard device driver), so the minimum size
 *      for this example would be 32 bytes (see MINBUFFSIZE above);
 *      however, some drivers may have slightly different requirements,
 *      in which case you should use the method described next, rather than
 *      assuming some fixed length.
 *
 *      If you wish to monitor a device driver whose data record size
 *      is unknown, pick a convenient size and attempt a call to DosMonReg.
 *      If it fails due to ERROR_NOT_ENOUGH_MEMORY, examine the 2nd word
 *      of your buffer, since it will now contain the size of the driver's
 *      buffer (ddbufflen in the example).  Allocate buffer(s) which can
 *      hold a data record of that size (in addition to the 16-byte header)
 *      and re-register your buffer(s).  If the same error occurs, then
 *      there is simply insufficient resources to support another monitor.
 *
 *      Note the MonBuff header structure does not define the monitor buffer
 *      beyond the header, because data records (KeyPackets in this example)
 *      are transferred to and from the buffers by DosMonRead and DosMonWrite.
 *      You should not attempt to read or write that area directly, nor should
 *      you modify any part of the header after a successful DosMonReg.
 *
 *      To maintain a rapid flow of data through any chain of monitors,
 *      DosMonRead and DosMonWrite operations should be done without any
 *      intervening slow operations or system calls. If necessary, dedicate
 *      a thread to the monitor stream, and raise its priority.
 *
 * Makefile:
 *
 *     LIB=\lib
 *     INC=\include
 *     OPT=-AL -G2 -Zi -Lp -Ox -Zp -I$(INC)
 *
 *     monitor.exe:   monitor.c monitor
 *                   cl $(OPT) monitor.c
 *
 *
 * To Run:
 *
 *      a) Start in Windowed Screen Group
 *      b) either  i) monitor
 *                ii) detach monitor
 *
 *
 * Created by Microsoft Corp. 1989
 */

#define INCL_DOSPROCESS
#define INCL_SUB
#define INCL_DOSMONITORS
#define INCL_DOSINFOSEG
#define INCL_DOSSEMAPHORES

#include <malloc.h>             /* for malloc() declaration */
#include <os2def.h>
#include <bse.h>


#define THREAD_STK_SIZE 2048            /* Monitor Thread Stack Size */

#define NOPREFERENCE    0               /* DosMonReg constants - */
#define FRONT           1               /* where in the monitor chain */
#define BACK            2               /* do we want to be installed? */

#define WAIT            0               /* DOSMONREAD and KBDCHARIN constants - */
#define NOWAIT          1                /* should we wait for a key? */

#define NOERROR         0               /* DOSEXIT constants - */
#define ERROR           1               /* what to return for error code? */


#define MINBUFFSIZE     64              /* size for monitor buffers */
                                        /* 64 is minimum, 128 recommended */


struct KeyPacket {              /* KBD monitor data record */
        unsigned       mnflags;
        KBDKEYINFO     cp;
        unsigned       ddflags;
};

struct MonBuff {                        /* generic monitor buffer header */
        unsigned        bufflen;
        unsigned        ddbufflen;
        unsigned char   dispatch[12];
};


HKBD    KBDHandle;           /* keyboard handle from monitor open */

struct MonBuff  *InBuff, *OutBuff;     /* buffers for monitor read/writes */

/*                                MonFlag S.C.  Char  Shift NLSSh State   Time Stamp  DDFlag */
struct KeyPacket Alt_Down     = { 0x3800, 0x00, 0x00, 0x40, 0x00, 0x0208, 0x00000000, 0x0007 };
struct KeyPacket Alt_Up       = { 0xb800, 0x00, 0x00, 0x40, 0x00, 0x0000, 0x00000000, 0x0047 };
struct KeyPacket Ctl_Down     = { 0x1d00, 0x00, 0x00, 0x40, 0x00, 0x0104, 0x00000000, 0x0007 };
struct KeyPacket Ctl_Up       = { 0x9d00, 0x00, 0x00, 0x40, 0x00, 0x0000, 0x00000000, 0x0047 };
struct KeyPacket ESC_Down_Alt = { 0x0100, 0x00, 0x01, 0x40, 0x00, 0x0208, 0x00000000, 0x003f };
struct KeyPacket ESC_Up_Alt   = { 0x8100, 0x00, 0x01, 0x40, 0x00, 0x0208, 0x00000000, 0x004c };
struct KeyPacket ESC_Down_Ctl = { 0x0100, 0x00, 0x01, 0x40, 0x00, 0x0104, 0x00000000, 0x003f };
struct KeyPacket ESC_Up_Ctl   = { 0x8100, 0x00, 0x01, 0x40, 0x00, 0x0104, 0x00000000, 0x004c };

unsigned short  BuffLength;     /* chars in monitor read/write buffer */
unsigned long    sem;           /* ram semaphore */

void Session_Switch()
{
        DosSemRequest( &sem,SEM_INDEFINITE_WAIT);  /* Sem Block to make sure */
                                                   /* that block goes out    */
                                                   /* uninterrupted          */

        DosMonWrite ( (PBYTE)OutBuff,           /* Depress the Alt Key */
                      (PBYTE)&Alt_Down, BuffLength );

        DosMonWrite ( (PBYTE)OutBuff,           /* Depress the ESC Key */
                      (PBYTE)&ESC_Down_Alt, BuffLength );

        DosMonWrite ( (PBYTE)OutBuff,           /* Release the ESC Key */
                      (PBYTE)&ESC_Up_Alt, BuffLength );

        DosMonWrite ( (PBYTE)OutBuff,           /* Release the Alt Key */
                      (PBYTE)&Alt_Up, BuffLength );

        DosSemClear( &sem);
}

void Task_Manager()
{
        DosSemRequest( &sem,SEM_INDEFINITE_WAIT);  /* Sem Block to make sure */
                                                   /* that block goes out    */
                                                   /* uninterrupted          */

        DosMonWrite ( (PBYTE)OutBuff,              /* Depress the Ctl Key */
                      (PBYTE)&Ctl_Down, BuffLength );

        DosMonWrite ( (PBYTE)OutBuff,              /* Depress the ESC Key */
                      (PBYTE)&ESC_Down_Ctl, BuffLength );

        DosMonWrite ( (PBYTE)OutBuff,              /* Release the ESC Key */
                      (PBYTE)&ESC_Up_Ctl, BuffLength );

        DosMonWrite ( (PBYTE)OutBuff,              /* Release the Ctl Key */
                      (PBYTE)&Ctl_Up, BuffLength );

        DosSemClear( &sem);
}

void MonChain()
{
        struct KeyPacket keybuff;   /* struct to read a write Kbd monitor data */
        unsigned short  count;      /* chars in monitor read/write buffer */


        count = sizeof(keybuff);    /* get the size of the KBD Monitor Struct */

        for (;;) {                   /* repeat ad noseum */



           DosMonRead ( (unsigned char *)InBuff, WAIT, /* Wait for any Mon input */
                        (unsigned char *)&keybuff, &count );

           DosSemRequest( &sem, SEM_INDEFINITE_WAIT);  /* yes - block the Mon Out */

           DosMonWrite ( (unsigned char *)OutBuff,     /* Pass Mon Packet as is   */
                         (unsigned char *)&keybuff, count );

           DosSemClear( &sem);                          /* re-enable task/session switch */

           DosSleep(100L);                              /* Allow Task Switch */
        }
}

void main()
{
        struct KeyPacket  keybuff;
        PGINFOSEG gdt;
        SEL gdt_descriptor, ldt_descriptor;   /* infoseg descriptors */

        unsigned short  i;                         /* loop counter */

        void far MonChain();            /* address of Monitor Chain Loop */
        char far *MonChainStack;        /* far pointer to Monitor Chain Loop stack */
        unsigned MonChainID;            /* thread ID */
        unsigned rc;                    /* return code */


        /* allocate space for monitor read/write buffers */
        InBuff = (struct MonBuff *)malloc(MINBUFFSIZE);
        OutBuff = (struct MonBuff *)malloc(MINBUFFSIZE);

        if ((InBuff == NULL) || (OutBuff == NULL))
            DosExit ( EXIT_PROCESS, ERROR ); /*terminate all threads and return error */

        /* prepare buffer headers for registration process */
        InBuff->bufflen = MINBUFFSIZE;
        OutBuff->bufflen = MINBUFFSIZE;

        /* obtain a handle for registering buffers */
        rc = DosMonOpen ( "KBD$", &KBDHandle );
        if (rc != NOERROR)
            DosExit ( EXIT_PROCESS, ERROR ); /*terminate all threads and return error */

        /* get the descriptor of global infoseg */
        /* and convert to far pointer so we can */
        /* get ID of current foreground screen group */
        DosGetInfoSeg(&gdt_descriptor, &ldt_descriptor);
        gdt = MAKEPGINFOSEG(gdt_descriptor);

        /* register the buffers to be used for monitoring */
        rc = DosMonReg ( KBDHandle, (PBYTE)InBuff, (PBYTE)OutBuff, FRONT,
                     gdt->sgCurrent);
        if (rc != NOERROR)
            DosExit ( EXIT_PROCESS, ERROR ); /*terminate all threads and return error */

        BuffLength = sizeof(keybuff);

        /* Monitor Loop Thread setup */
        /* Monitors are part of the data flow path through a device */
        /* driver. Monitors should be written so that a thread actually */
        /* performs rapid I/O and passes the data flowing into the */
        /* monitor chain back into this same chain. This imperative so that */
        /* monitor control characters (if not processed by all monitors) are */
        /* available through the monitor chain. */

        sem = 0L;                       /* init semaphore */

        /* allocate stack space for MonChain thread */
        /* since this is written in C, DOSALLOCSEG cannot be used here */

        if ((MonChainStack = malloc(THREAD_STK_SIZE)) == NULL) {

                DosMonClose ( KBDHandle );       /* Close KBD Monitor */
                DosExit ( EXIT_PROCESS, ERROR ); /*terminate all threads and return error */

        }
        MonChainStack += THREAD_STK_SIZE;        /* since stack grows down */

        /* start another thread */
        if (rc = DosCreateThread( MonChain, &MonChainID,
                                  MonChainStack ))  {

                DosMonClose ( KBDHandle );       /* Close KBD Monitor */
                DosExit ( EXIT_PROCESS, ERROR ); /*terminate all threads and return error */

        }

        /* Sample Code for Session Switch */

        for (i = 0; i < 10; ++i) {  /* Session Switch Screen 10 times */

           Session_Switch();        /* Perform a Session Switch       */
           DosSleep(3000L);         /* Sleep for 5 seconds            */

        } /* end for */

        Task_Manager();             /* Call Task Manager              */

        /* close connection with keyboard */
        DosMonClose ( KBDHandle );

        DosExit ( EXIT_PROCESS, NOERROR );
}

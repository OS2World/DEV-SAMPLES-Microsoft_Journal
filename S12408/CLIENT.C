/*  Compilation and  Linking

    cl -Alfw -Zl -Gs -c client.c
    link client,,,netapi nampipes

    Usage:
       server2
    Comments:
       This program shows the usage of Name Pipes, Semaphores, and
       Threads. Specifically:

	  DosOpen
	  DosSetNmpHandState
       1. The pipe is in non-blocked mode.
       2. Hit <return> to terminate.
    DisClaimer:
	   THIS PROGRAM IS FOR DEMONSTRATION ONLY. MICROSOFT MAKES NO WARRANTY
	   , EITHER EXPRESSED OR IMPLIED, AS TO IT'S USABILITY IN ANY GIVEN
	   SITUATION.
*/

#define INCL_BASE
#include <os2.h>

#include <netcons.h>
#include <nmpipe.h>

#include <stdlib.h>
#include <stdio.h>

    /* Client DosOpen Parameters	*/
#define DUMMY_FILE_SIZE 0L
#define NORMAL 0x00
#define RESERVED 0L
#define OPEN_FAIL 0x00
#define OPEN_EXISTS 0x01


    /* PIPE PARAMETERS */
#define INBUFSZ 4096
#define OUTBUFSZ 4096
#define SIZESEND  2040
#define TIMEOUT 0L
#define OPEN_ACCESS_READWRITE 0x0002
#define OPEN_SHARE_DENYNONE   0x0040

#define DEFAULTNAME "\\pipe\\demo"



main (int argc, char * argv[])
{
    unsigned short oWrt, oRd;
    unsigned rtn;
    char *pname;
    USHORT pipeHdl, status;
    char *buf, *msg;
    int i, j, msglen;
    int n = 0;			/* Message Count */

    if (argc >1)
	pname = argv[1];
    else
	pname = DEFAULTNAME;

    msg = (char *)malloc(INBUFSZ);
    buf = (char *)malloc(OUTBUFSZ);


    printf ("***   CLIENT   ***\n\n");

	/* Open the Pipe at the Client end */


    if (rtn=DosOpen (pname, &pipeHdl, &status, DUMMY_FILE_SIZE, NORMAL,
		     OPEN_EXISTS,
		     OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE,
		     RESERVED)){

	    printf ("DosOpen error #%u status%u\n", rtn, status);
	    exit(1);

    }

	    /* Change to Non_blocking  */
     rtn = DosSetNmpHandState(pipeHdl, 0x8100);
     if (rtn)
	    printf("DosSetNmpHandState Error rtn: %i\n", rtn);

      buf[0] = '\0';
      printf("Client >>");
      msglen = strlen(gets(buf));
      if (msglen ==0) {
	 if (rtn=DosClose (pipeHdl))
	    printf ("DosClose error #%u\n", rtn);
	 exit(1);
       }
      buf[msglen] = '\0';


    do {

	n++;
		/* write   message */
	if (rtn = DosWrite (pipeHdl, buf, strlen(buf), &oRd))
	    printf ("Client: DosWrite error #%u\n", rtn);
	if (oRd != msglen)
	    printf ("Client: %i  write error: %i!!!\n", msglen-oRd, n);

		/* read message */
	if (rtn = DosRead (pipeHdl, msg, INBUFSZ-1, &oRd))
	    printf( "DosRead error #%u\n", rtn);

	if (!rtn) {
	    printf("Message from Server: ");
	    for (j=0; j < oRd; )
		     printf("%c",  msg[j++]);
	    printf("\n");

	}
	msg[0] = '\0';
	buf[0] = '\0';
	printf("Client >>");
	msglen = strlen(gets(buf));
	printf("\n");

	if (msglen ==0) {
	    if (rtn=DosClose (pipeHdl))
		printf ("DosClose error #%u\n", rtn);
	    exit(1);
	}
	buf[msglen] = '\0';


    } while ( 1 );

    if (rtn=DosClose (pipeHdl))
	printf ("DosClose error #%u\n", rtn);
}

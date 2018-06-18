/*  Compilation and  Linking

    cl -Alfw -Zi -Gs -c server.c
    link server,,,netapi  nampipes

    Usage:
       server3
    Comments:
       This program shows the usage of Name Pipes, Semaphores, and
       DosMuxSemWait.	Specifically:

	  DosCreateSem,
	  DosSetNmPipeSem,
	  DosSemSet,
	  DosConnectNmPipe,
	  DosMuxSemWait,

	  DosQNmPipeSemState
	  DosDisconnectNmPipe,
	  DosSemClear,
	  DosOpen

       1. The pipe is in non-blocked mode.
       2. Hit space bar to stop program.
       3. Run server on one machine and test it by running
	  number of clients (local or remote)
    DisClaimer:
	   THIS PROGRAM IS FOR DEMONSTRATION ONLY. MICROSOFT MAKES NO WARRANTY
	   , EITHER EXPRESSED OR IMPLIED, AS TO IT'S USABILITY IN ANY GIVEN
	   SITUATION.
*/




#define INCL_BASE
#define INCL_DOSFILEMGR

#define INCL_DOSSEMAPHORES
#define INCL_DOSERRORS

#include <os2.h>

#include <netcons.h>
#include <nmpipe.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


    /* PIPE PARAMETERS */
#define POMODE	0x0002 /* DUPLEX */
#define PPMODE NP_RMESG | NP_ICOUNT | NP_WMESG | NP_NBLK  /* NON-BLOCKED */
#define INBUFSZ 4096
#define OUTBUFSZ 4096
#define SIZESEND  2040
#define TIMEOUT 0L
#define NUMPIPES 3
USHORT	usPipeIndex;		    /* pipe instance counter */
#define DEFAULTNAME "\\pipe\\demo"

    /* SEMAPHORE DECLARATIONS */

USHORT usSemIndex;
DEFINEMUXSEMLIST(MuxList, NUMPIPES) /* Used with DosMuxSemWait()  */


    /* BUFFER FOR  NPSS STRUCTURES  */
#define bufsize 3*sizeof(struct npss)
         /* bufsize must be a multiple of the size of npss, 
              because multiple npss's can be returned.          */

unsigned char buffer[bufsize][10];
unsigned short buflen = bufsize;
struct npss *npss_check = (struct npss *)buffer;
         /* Doing type casts like this and those below is
              recommended. Use the /W3 compile switch and add
              type casts until the warning messages are all gone
              to save potential grief debugging at run time.    */
 


main (int argc, char *argv[]) {

    unsigned rc;
    unsigned short oWrt, oRd;
    int  i,j, msglen;

			     /* Buffers */
    char *sndbuf,
	 *rtnbuf,
	 *pname;
    unsigned srvPipeHdl[NUMPIPES];	/*  Pipe Handle */

    static char SemName[20] = "\\SEM\\READ.OK";
    char NewName[20], *pCh;
    char cSem[2];
	/* get pipe name */
    if (argc >1)
	pname = argv[1];
    else
	pname = DEFAULTNAME;




    /*	allocate buffers for receiving/sending data */
    sndbuf = (char *)malloc(OUTBUFSZ);
    rtnbuf = (char *)malloc(OUTBUFSZ);
    cSem[0] = 'A';

    printf ("***   SERVER  STARTED   ***\n\n");


	/* Make a Pipe and Set Semaphore for Each Instance */

    MuxList.cmxs = NUMPIPES;
    for (usPipeIndex=0; usPipeIndex <NUMPIPES; usPipeIndex++) {

	   printf("Making the Pipe %i \n", usPipeIndex);

	   rc = DosMakeNmPipe (pname, &srvPipeHdl[usPipeIndex],
					POMODE, PPMODE,
					INBUFSZ, OUTBUFSZ, TIMEOUT);
	   if (rc)
	      printf ("DosMakeNmPipe error #%u\n", rc);

	 /* add pipe instance to semaphore names to generate unique names */

	    strcpy(NewName, SemName);
	    cSem[0] += usPipeIndex;
	    pCh = strcat(NewName, cSem);

	   /* create a semaphore */

	    rc	= DosCreateSem(CSEM_PUBLIC,
			       &MuxList.amxs[usPipeIndex].hsem,
			       pCh);
	    if (rc)
		  printf("ERROR - DosCreateSem, rc = %d\n", rc);

 
	  /*  set the semaphore to pipe */

	   rc = DosSetNmPipeSem(srvPipeHdl[usPipeIndex],
				(long)MuxList.amxs[usPipeIndex].hsem,
				usPipeIndex);
	   if (rc)
	       printf("ERROR - DosSetNmPipeSem, rc = %d\n", rc);
 
	  /* set the semaphore */

	   rc = DosSemSet(MuxList.amxs[usPipeIndex].hsem);
	   if (rc)
	       printf("ERROR - DosSemSet, rc = %d\n", rc);

	  /* open a connection	*/

	   rc = DosConnectNmPipe(srvPipeHdl[usPipeIndex]);

    }

 do  {
	  /*  Wait on Semaphores */


	rc = DosMuxSemWait(&usSemIndex, &MuxList, 5000L);

	if (ERROR_SEM_TIMEOUT==rc)
	      printf("Sem timed out \n");
	else if (rc!=0)
	      printf("ERROR - DosSemWait, rc = %d\n", rc);
 
	else {	    /* Some semaphore was cleared */

	  rc = DosQNmPipeSemState((long) MuxList.amxs[usSemIndex].hsem,
				(char far *)buffer,
				buflen);
	  if (rc)
	    printf("ERROR - DosQNmPipeSemState, rc = %d\n", rc);

	 /* We may have more than one npss structure returned. So let's
	    check each one.  */

	  npss_check = (struct npss *)buffer;

	  for (i=0; i<3; i++) {



	    if (NPSS_EOI == npss_check->npss_status) {	  /* End of Information */

	    /*	EOF   */

	    /* The possible values for npss_info->npss_status are
              correctly defined in C:\LANMAN\NETSRC\H\NMPIPE.H,
              but are wrong in the documentation.               */
	     }

	     if (NPSS_RDATA==npss_check->npss_status) {     /* Read the Data */


	      if (rc = DosRead (srvPipeHdl[usSemIndex],
				rtnbuf, INBUFSZ-1, &oRd))
		 printf ("Server: DosRead error #%u\n", rc );

	      if (oRd !=0) {				   /* Message */
	       printf("Message pipe: %i ", usSemIndex);
	       for (j=0; j < oRd; )
		     printf("%c",  rtnbuf[j++]);
	       printf("\n");
	       rtnbuf[0] ='\0';
	      }
	     }

	     if (NPSS_WSPACE == npss_check->npss_status) {  /* write the data */
	      printf("Server>>");
	      scanf("%s", sndbuf);
	      msglen = strlen(sndbuf);

	      sndbuf[msglen] = '\0';
	      if (rc = DosWrite (srvPipeHdl[usSemIndex],
				 sndbuf, strlen(sndbuf), &oWrt))
		 printf( "Server: DosWrite Error #%u\n", rc);
	      sndbuf[0] ='\0';
	     }

	    if (NPSS_CLOSE==npss_check->npss_status) {	   /* Close the Pipe */

	      if (rc = DosDisconnectNmPipe (srvPipeHdl[usSemIndex]))
		  printf ("DosDisConnectNmPipe error #%u\n", rc);

	      printf("Server: closing pipe number %i\n", usSemIndex);

	      /* Now Reopen the connection for next Client */
	      rc = DosConnectNmPipe(srvPipeHdl[usSemIndex]);
	      if (rc)
		 printf("DosConnectNmPipe error #%u\n", rc);
	    }

	    npss_check += 1;
	  }  /* for */
	}    /* else */

     }	while (1);	     /* end of semaphore loop */



 
}

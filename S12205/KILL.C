/*
 *  This program provides the command line ability to kill another process.
 *  The other process does not need to be a child of this process.  The
 *  other process may be active in a screen group or detached.
 *
 *  This program demonstrates the use of:
 *    DosKillProcess
 *
 *  Compile and Link instructions:
 *
 *    cl -c -Zi -Od kill.c
 *    link kill/CO;
 *
 *  Execution insturctions:
 *
 *    kill <process ID number> (press return key)
 *
 *  This software is provided for demonstration purposes only.
 *  Microsoft makes no warranty, either express or implied as
 *  to its usability in any given situation.
*/

#define INCL_BASE
#include <os2.h>
#include <stdio.h>

main(argc, argv)
unsigned short argc;
char **argv;
{

  int rc;
					/* make sure a process ID supplied   */
  if (argc > 1) {

    rc = DosKillProcess(DKP_PROCESSTREE,
			atoi(argv[1]));
    if (rc) {
      printf("ERROR - DosKillProcess, rc == %d\n", rc);
    }
  }
  else {
    printf("KILL <process ID number> (press return key)\n");
  }
}

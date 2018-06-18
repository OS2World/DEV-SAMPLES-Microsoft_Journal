/*
 *  This program is to demonstrate the use of NetAuditWrite.
 *  This program will generate an audit record not defined by
 *  the base LAN Manager product.
 *
 *  LANMAN.INI parameter audit MUST be set to YES
 *	  audit = yes
 *
 *  This program demonstrates the use of the following API
 *    NetAuditWrite
 *
 *  Compile and Link instructions:
 *
 *    cl -c -Zi -Od netaudit.c
 *    link netaudit/CO,,,netapi netoem;
 *
 *  Execution instructions:
 *
 *    netaudit [<message>]
 *	if message is supplied then that is written into
 *	the audit trail, else a default message is written.
 *
 *  This software is provided for demonstration purposes only.
 *  Microsoft makes no warranty, either express or implied as
 *  to its usability in any given situation.
*/

#define INCL_BASE
#include <os2.h>
#include <stdio.h>

/*
 *  Always include the netcons file prior to other network libraries
*/

#include <netcons.h>
#include <audit.h>
					/*  size of message buffer	     */
#define DEFAULTMESSAGESIZE	    1024
					/*  buffer for user message	     */
char bMessage[DEFAULTMESSAGESIZE];
					/*  default message		     */
#define DEFAULTMESSAGE		    "Audit Message: This is just text"
					/*  pointer to message to write      */
char * psMessage;
					/*  audit type			     */
#define DEFAULTAUDITTYPE	    0x0999


/**********   PROGRAM STARTS HERE  *****************************************/

main(argc, argv)
unsigned short argc;
char **argv;
{

  int rc;

  int j;
					/* if there is a message on the      */
					/* command line then put it in the   */
					/* bMessage area		     */
  if (argc > 1) {
					/* first make sure bMessage is clear */
    strcpy(bMessage, "");
					/* now loop through args,	     */
    for (j = 1; j < argc; j++) {
					/* concatenating as we go	     */
      strcat(bMessage, argv[j]);
      strcat(bMessage, " ");
    }
					/* set pointer to bMessage buffer    */
    psMessage = bMessage;
  }
					/* if there is no message, then use  */
					/* the default message		     */
  else {
    psMessage = DEFAULTMESSAGE;
  }
					/* write message to audit log	     */

  rc = NetAuditWrite(DEFAULTAUDITTYPE,
		     psMessage,
		     strlen(psMessage),
		     NULL,
		     NULL);
  if (rc) {
    printf("ERROR - NetAuditWrite, rc == %d\n", rc);
    exit(0);
  }

  printf("Message successfully written to audit log\n");

}

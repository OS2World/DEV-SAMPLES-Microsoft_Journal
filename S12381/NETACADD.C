/*  Compilation and  Linking

    cl -Alfw -Zl -Gs -c netacadd.c
    link netacadd,,,netapi

    Usage:
       netacadd -n<number of users> -s<\\servername>
    Comments:
	This program demonstrates the usage of NetAccessAdd.
	1.   The following parameter should be set in lanman.ini
	      [server]
	      security = user
	2.   All users for whom an access record will be added by netacadd.c
	     should be added in users account before running the program.
	     (Net admin ->Accounts->add)

    DisClaimer:
	   THIS PROGRAM IS FOR DEMONSTRATION ONLY. MICROSOFT MAKES NO WARRANTY
	   , EITHER EXPRESSED OR IMPLIED, AS TO IT'S USABILITY IN ANY GIVEN
	   SITUATION.
*/




#define INCL_BASE
#include <os2.h>

#include <netcons.h>
#include <access.h>
#include <neterr.h>

#include <stdio.h>
#include <malloc.h>

/*
 * Defines
 */
#define ckerr(rcode,mess) { \
    if (rcode) { \
        printf (error, __LINE__, #rcode, rcode, mess);  \
        exit (rcode); \
        } \
    }

#define MAXLISTSIZE	10
#define AUDIT		0x01
#define NOAUDIT 	0x00
#define SERVERNAME	"\\\\SERVER"

typedef struct access_info_1 InfAccess;
typedef struct access_list   InfList;

/*
 * Global Values
 */
char  error[] = "ERROR at line %d, %s = %d, %s";
char  MEMERR[]= "Malloc failed. Out of memory?";

 /*** main
 *
 *
 */
 main (int argc, char *argv[])
 {

 INT suser, i;
 CHAR server[UNLEN+2], c;
 CHAR *buffer;
 USHORT buflen;
 SHORT level=1 ;			    /* Level 1 */
 InfAccess *access_header;		    /* pointer to access_info_1 */
 InfList   *access_list;		    /* pointer to access_list	*/
 USHORT rc;
 CHAR	Resource[20];			    /* Resource Name		*/
    /*
    *	Default Values
    */
    suser = MAXLISTSIZE;
    strcpy(server, SERVERNAME);

    /***
    *  Find out Number of Users and Server Name  from Command Line
    */

    if (argc > 1) {

       for (i = 1; i <argc ; i++) {
	 if (argv[i][0]=='-') {
	    switch ( argv[i][1] ) {

	       case 'n': sscanf(argv[i], "%c%c%i",&c,&c, &suser);

			 break;
	       case 's': sscanf(argv[i], "%c%c%s",&c,&c, server);


			 break;

	       default : printf("Usage: netacadd -n -s\n");
			 printf("Taking default values\n");
			 break;
	     }

	  }
	}

     }
     /*
     *	Allocate buffer for access_info_1 and acces_list
     *	Assign values to the structure fields
     */

     buflen = sizeof(InfAccess) + sizeof(InfList) * suser;
     buffer = (CHAR  *) malloc(buflen);
     if (buffer==NULL) {
	    printf("%s", MEMERR);
	    exit(0);
      }
     access_header = (InfAccess *) buffer;
     access_list   = (InfList *) (buffer + sizeof(InfAccess));

     printf("Name of the Resource:");
     scanf("%s", Resource);
     c = getchar();
     printf("\n");
     access_header->acc1_resource_name = (CHAR *) Resource;

     printf("\n Auditing on this resource:[y]");

     if ((c = getchar())=='n')
	access_header->acc1_attr = NOAUDIT;
     else
	access_header->acc1_attr = AUDIT;

     access_header->acc1_count = suser;

     /*
      *   Get User Names
     */


     for (i=0; i< suser; i++) {

	printf("[%i] User Name: ", i+1);
	scanf("%s", access_list->acl_ugname);
	printf("\n");
	/*
	 *  Assume ACCESS_READ permissions.
	*/

	access_list->acl_access = ACCESS_READ;
	access_list += 1;
     }

     rc =  NetAccessAdd( (CHAR FAR *) server,	       /* Name of the server */
			 level, 		       /* Level of detail    */
			 (CHAR FAR *) buffer,	       /* Buffer	     */
			 buflen);		       /* Buffer Length      */
     if (rc)
	   ckerr(rc, "NetAccessAdd");

     printf("User Access addedd successfully\n");


}

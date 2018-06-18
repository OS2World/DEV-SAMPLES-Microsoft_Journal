#define INCL_BASE

#include <os2.h>

#include <stdio.h>
#include <conio.h>

void main(void);

void main(void)
{
   HFILE hf;
   USHORT usAction,retcode;

   printf("\n Hit a key to reboot...");
   getch();
   retcode = DosOpen("REBOOT$",                /* open reboot device  */
       &hf,                                    /* address of device handle */
       &usAction,                              /* action taken */
       0L,                                     /* size of new file */
       0,                                      /* normal file attribute */
       1,                                      /* open existing file */
       0x0011,                                 /* write-only, exclusive */
       0L);                                    /* reserved */

   if (!retcode)
      DosDevIOCtl(0L,0L,0,0,hf);         /* pass user IOCtl Cat 80h */
                                               /* Func 41h */
   else
      printf("\n DosOpen on REBOOT$ failed... rc = %u\n",retcode);
}

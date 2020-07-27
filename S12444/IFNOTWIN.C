/****************************************************************************/
/*                                                                          */
/*                 Copyright (c) Microsoft Corp.  1987, 1990                */
/*                           All Rights Reserved                            */
/*                                                                          */
/****************************************************************************/
/*
 * IFWIN - IFNOTWIN
 *
 * Executes a program based on whether it is running in a vio window or not.
 *
 * Two versions of this program can be created based on the #define IFWIN.
 * If this symbol is defined, a program will be created that only runs a
 * specified program if it is running in a vio window.  Without this symbol
 * defined, a program is created that will only run the specified program
 * if it is running in a full screen session.
 *
 * Author: Byron Dazey
 *
 */

#include <stdio.h>
#include <string.h>
#define INCL_DOSINFOSEG
#define INCL_DOSPROCESS
#include <os2.h>

USHORT main(USHORT, NPSZ *);
VOID ShowUsage(VOID);

PLINFOSEG plis;
SEL gisSel;
SEL lisSel;
RESULTCODES resc;
CHAR ArgBuf[512];
NPCH pch;
USHORT i;

NPSZ pszUsage[] = {
    "",
    "Usage:",
#ifdef IFWIN
    "  IFWIN pgmname [arguments]",
    "",
    "    pgmname   - Program to start if executing in a vio window.",
#else
    "  IFNOTWIN pgmname [arguments]",
    "",
    "    pgmname   - Program to start if NOT executing in a vio window.",
#endif
    "                The .EXE extension must be specified.",
    "    arguments - Optional parameters to pgmname.",
    "",
    "  To execute an internal command such as \"copy\", use the following",
    "  for the arguments: \"cmd.exe /c copy a b\"",
    NULL
};

USHORT main(argc, argv)
USHORT argc;
NPSZ *argv;
{
    register rc;

    if (argc < 2) {
        ShowUsage();
        return 1;
    }

    DosGetInfoSeg(&gisSel, &lisSel);
    plis = MAKEPLINFOSEG(lisSel);

#ifdef IFWIN
    if (plis->typeProcess == PT_WINDOWABLEVIO) {
#else
    if (plis->typeProcess == PT_FULLSCREEN) {
#endif

        strcpy(ArgBuf, argv[1]);
        pch = ArgBuf + strlen(ArgBuf);
        pch++;
        *pch = 0;

        for (i = 2; i < argc; i++) {
            if (i > 2)
                strcat(pch, " ");

            strcat(pch, argv[i]);
        }

        *(pch + strlen(pch) + 1) = 0;

        rc = DosExecPgm(NULL,   /*fail name buffer                          */
                0,              /*fail name buffer length                   */
                EXEC_ASYNC,     /*start it asynchronuously                  */
                ArgBuf,         /*pass it the arguments                     */
                NULL,           /*don't change the environment              */
                &resc,          /*result code buffer                        */
                argv[1]);       /*pass it the program name to start         */

        if (rc) {
            printf("Exec failed (rc = %u).\n\a", rc);
            return 1;
        }
    }

    return 0;
}

VOID ShowUsage()
{
    NPSZ *p;

    p = pszUsage;
    while (*p)
        puts(*p++);
}

/*
 * 
 *  HUGE.C -- demonstrate OS/2 huge memory usage
 *  
 *  This example illustrates allocating a huge segment, addressing
 *  the segment (it accesses each selector in the huge segment), and
 *  how to manually determine the huge segment shift factor. The
 *  following OS/2 APIs are illustrated here:
 *  
 *      DosGetInfoSeg()
 *      DosGetHugeShift()
 *      DosAllocHuge()
 *      DosFreeSeg()
 *  
 *  This example allocates a huge segment. It writes a value to all
 *  bytes in all segments of the huge segment. Then it checks if all
 *  bytes in all segments contain the value that was written
 *  previously. It then frees the huge segment and exits. This
 *  example is intended to show how to manage and access huge
 *  segments - it doesn't perform any useful function.
 *  
 *  If macro USE_HUGE_KEYWORD is #defined, then the Microsoft
 *  C compiler 5.1's "huge" keyword will be used to tell the C
 *  compiler to generate code to calculate "jumps" between the huge
 *  segments. This will simplify the applications's code required
 *  (and the compiler will generate the code at a lower level). If
 *  this macro is not #defined, then the code will not use this code
 *  generation ability of the language and will instead manually
 *  calculate the jumps.
 *  
 *  Note that the "huge" keyword, while supported by some other DOS 
 *  and OS/2 C compilers, is not standard C language keyword. Also,
 *  for informative use only, this code uses Microsoft C #pragmas, 
 *  which may not be implemented on other C compilers.
 *  
 *  To compile this program, type:  cl /W3 /AL /Lp /G2 huge.c
 *  
 *  Copyright (C) 1989 Microsoft Corporation 
 *  
 *  This software is provided for demonstration purposes only.
 *  Microsoft makes no warranty, either express or implied as 
 *  to its usability in any given situation.
 *  
 */

/* ------------------------------------------------------------------------ */

/* compile-time build options */

/* allow C to use 'huge' keyword */
/* #define USE_HUGE_KEYWORD */

/* let the builder know how the sources are being compiled */

#ifdef USE_HUGE_KEYWORD
    #pragma message("fyi: compiling WITH support for C 'huge' keyword")
#else
    #pragma message("fyi: compiling withOUT support for C 'huge' keyword")
#endif

/* ------------------------------------------------------------------------ */

/* include files */

#define INCL_DOS
#define INCL_DOSMEMMGR
#include <os2.h>

#include <stdio.h>           /* for printf() */
#include <stdlib.h>          /* for exit() */
 
/* ------------------------------------------------------------------------ */

/* function prototypes */

INT main(void);
void TestError(USHORT usError, PSZ pszApiName);

/* ------------------------------------------------------------------------ */

/* constants */

#define MAX_SEGS     3       /* maximum number of huge segments (arbitrary) */
#define TEST_DATA   42       /* byte-sized data for test (arbitrary) */
#define PART_SEG     0       /* no partial segment */
#define MAX_SEGSIZE  0xFFFF  /* max size of an OS/2 1.XX segment (65KB) */

/* ------------------------------------------------------------------------ */

/* the main program */
 
INT main(void)
{
 
    SEL sel;                 /* selector */
    USHORT usError;          /* API return code */
#ifdef USE_HUGE_KEYWORD
    CHAR huge *fpchBuffer;   /* pointer to a huge segment */
    CHAR huge *fpchFirstSeg; /* pointer to the first huge segment selector */
    LONG j;                  /* segment offset loop index */
#else
    USHORT usHugeShift;      /* huge memory model shift count */
    USHORT usAltHugeShift;   /* huge shift count from GINFOSEG */
    USHORT usHugeIncrement;  /* the huge segment increment value */
    SEL selGlobalSeg;        /* GDT selector */
    SEL selLocalSeg;         /* LDT selector */
    GINFOSEG FAR *pgis;      /* far pointer to GINFOSEG structure */
    CHAR far *fpchBuffer;    /* pointer to a huge segment */
    CHAR far *fpchFirstSeg;  /* pointer to the first huge segment selector */
    INT i;                   /* huge segment loop index */
    INT j;                   /* segment offset loop index */
#endif /* USE_HUGE_KEYWORD */

#ifndef USE_HUGE_KEYWORD
    /* get and display the huge shift count (DosGetHugeShift method) */
    printf("computing huge shift count value...\n");
    usError = DosGetHugeShift(&usHugeShift);
    TestError(usError, "DosGetHugeShift");
    printf("DosGetHugeShift() reports huge shift count = %d\n", usHugeShift);

    /* get and display the huge shift count (GINFOSEG.cHugeShift method) */
    usError = DosGetInfoSeg(&selGlobalSeg, &selLocalSeg);
    TestError(usError, "DosGetInfoSeg");
    pgis = MAKEPGINFOSEG(selGlobalSeg);
    usAltHugeShift = pgis->cHugeShift;

    /* verify that both huge shift counts are the same */
    if (usAltHugeShift != usAltHugeShift)
    {
        printf("DosGetInfoSeg() reports huge shift count = %d\n", 
            usAltHugeShift);
        printf("error: the shift counts from both methods don't agree!\n");
        exit(1);
    }
#endif /* USE_HUGE_KEYWORD */

    /* allocate a huge segment */
    printf("allocating huge segment (%d selectors)...\n", MAX_SEGS);
    usError = DosAllocHuge(MAX_SEGS, PART_SEG, &sel, MAX_SEGS, SEG_NONSHARED);
    TestError(usError, "DosAllocHuge");

    /* compute the address to the first huge segment */
    printf("computing address to the first huge segment...\n");
    fpchFirstSeg = MAKEP(sel, 0);
    fpchBuffer = MAKEP(sel, 0);
    printf("pointer to first segment = %04X\n", fpchFirstSeg);

#ifndef USE_HUGE_KEYWORD
    /* compute the huge segment shift value */
    usHugeIncrement = 1 << usHugeShift;
    printf("the huge segment increment value is %d\n", usHugeIncrement);
#endif /* USE_HUGE_KEYWORD */

    /* data write verification test */
    printf("writing test data to segments...\n");
#ifdef USE_HUGE_KEYWORD
    /* loop through each byte in meta-huge segment */
    printf("loop through %lu bytes...\n", (LONG)(MAX_SEGS * MAX_SEGSIZE));
    for (j = 0L; j < (LONG)(MAX_SEGS * MAX_SEGSIZE); j++)
    {
        fpchBuffer[j] = TEST_DATA; /* test writing to segment */
    }
    printf("%lu bytes written\n", (LONG)(MAX_SEGS * MAX_SEGSIZE));
#else
    /* loop through each selector in huge segment */
    for (fpchBuffer = fpchFirstSeg, i = 0; i < MAX_SEGS; i++)
    {
        fpchBuffer += usHugeIncrement;
        printf("writing to selector #%d (%04Xh)...\n", i, fpchBuffer);
        /* loop through each byte in current selector */
        for (j = 0; j < MAX_SEGSIZE; j++)
        {
            fpchBuffer[j] = TEST_DATA; /* test writing to segment */
        }
    }
#endif /* USE_HUGE_KEYWORD */

    /* data read verification test */
    printf("reading test data back from the segments...\n");
#ifdef USE_HUGE_KEYWORD
    /* loop through each byte in meta-huge segment */
    for (j = 0L; j < (LONG)(MAX_SEGS * MAX_SEGSIZE); j++)
    {
        if (fpchBuffer[j] != TEST_DATA) /* test reading from segment */
        {
            printf("error: unexpected value in huge segment!\n");
        }
    }
    printf("%lu bytes read\n", (LONG)(MAX_SEGS * MAX_SEGSIZE));
#else
    /* loop through each selector in huge segment */
    for (fpchBuffer = fpchFirstSeg, i = 0; i < MAX_SEGS; i++)
    {
        fpchBuffer += usHugeIncrement;
        printf("reading from selector #%d (%04Xh)...\n", i, fpchBuffer);
        /* loop through each byte in current selector */
        for (j = 0; j < MAX_SEGSIZE; j++)
        {
            if (fpchBuffer[j] != TEST_DATA) /* test reading from segment */
            {
                printf("error: unexpected value in huge segment!\n");
            }
        }
    }
#endif /* USE_HUGE_KEYWORD */

    /* free the huge segment */
    usError = DosFreeSeg(sel);
    TestError(usError, "DosFreeSeg");

    exit(0);
    return (0);

} /* main */

/* ------------------------------------------------------------------------ */

/* test an OS/2 API return code; choke and die if unsuccessful */

void TestError(USHORT usError, PSZ pszApi)
{

    if (usError)
    {
        printf("error: %s() returned error code %u!\n", pszApi, usError);
        exit(1);
    }

} /* TestError */

/* ------------------------------------------------------------------------ */

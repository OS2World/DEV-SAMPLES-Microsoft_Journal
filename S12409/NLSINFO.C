/*
 *
 *  SCCSID: @(#)nlsinfo.c -- display localization information
 * 
 *  This program displays most of the NLS (national language
 *  support) features of OS/2. It uses the Family API services
 *  DosGetCntryInfo(), DosGetCollate(), and DosGetCp() to obtain 
 *  various locale-dependent information. 
 *  
 *  In addition to the information returned by these three FAPI
 *  services, this program also has created a table of information
 *  based on DOS and OS/2 user documentation to give more 
 *  information about the localized environment. For example, see 
 *  the COUNTRYNAME table. This table can easily become outdated,
 *  as the table is not in the operating system; refer to MS-DOS 
 *  and OS/2 user documentation for the latest information. 
 *  
 *  The compile instructions for this program are:
 *  
 *      cl /Lp /Fb /W3 nlsinfo.c
 *  
 *  which will result in a dual-mode NLSINFO.EXE file which can 
 *  be run in DOS (3.3 or above) real mode or OS/2 protect mode.
 *  
 *  The runtime command line usage of this program is:
 *  
 *      nlsinfo <countrycode> <codepage>
 * 
 *  where <countrycode> is a valid country code and <codepage>
 *  is a valid codepage. If these values are both omitted, the
 *  program uses the default values of 0 for each, which uses
 *  the current country and codepage information for the current
 *  process. See the batch files NLSTEST.CMD and NLSTEST.BAT for 
 *  examples of how to invoke NLSINFO.EXE.
 *
 *  [Remember, when using DOS, the program NLSFUNC must be running
 *  if you want to obtain NLS-related information for any country 
 *  but the current (default) one. No similar precaution is 
 *  required when running OS/2.]
 *
 *  Copyright (C) 1988 Microsoft Corporation 
 *
 *  This software is provided for demonstration purposes only.
 *  Microsoft makes no warranty, either express or implied as
 *  to its usability in any given situation.
 *
 */

/* ------------------------------------------------------------------------ */

/* include files */

#define INCL_DOS
#define INCL_DOSNLS
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------------------ */

/* hard-coded country ASCIIZ names (not accessable from an OS/2 API) */

typedef struct {
    USHORT usCountry;                                   /* the country code */
    USHORT usCodepagePrimary;         /* country's default/primary codepage */
    USHORT usCodepageSecondary;      /* country's backup/secondary codepage */
    PSZ pszName;                                        /* the country name */
} COUNTRYNAME;

/*
 * 
 * NOTE: Many of the countries in this list (especially the far and 
 *       middle eastern Europe countries) are not supported in the 
 *       USA domestic releases of OS/2 and DOS. They are listed 
 *       because some localized versions of DOS 4.0 and OS/2 may 
 *       support these countries. Refer to the OS/2 documentation 
 *       for a list of the supported countries.
 *
 *       In this table, a value of 9999 is used internally to 
 *       this program as an invalid country; this value does not 
 *       have this special significance by OS/2 or DOS.
 *
 */

COUNTRYNAME vctrynm[] =
{
    {   0, 437, 850, "United States"             },    
    {   1, 437, 850, "United States"             },    
    {   2, 863, 850, "Canada (French)"           },
    {   3, 850, 437, "Latin America"             },
    {  31, 437, 850, "Netherlands"               },
    {  32, 850, 437, "Belgium"                   },
    {  33, 437, 850, "France"                    },
    {  34, 850, 437, "Spain"                     },
    {  39, 437, 850, "Italy"                     },
    {  41, 850, 437, "Switzerland"               },
    {  44, 437, 850, "United Kingdom"            },
    {  45, 850, 865, "Denmark"                   },
    {  46, 437, 850, "Sweden"                    },
    {  47, 850, 865, "Norway"                    },
    {  49, 437, 850, "Germany"                   },
    {  61, 437, 850, "Australia"                 },
    {  81, 932, 437, "Japan"                     },
    {  82, 934, 437, "Korea"                     },
    {  86, 936, 437, "Peoples Republic of China" },
    {  88, 938, 437, "Taiwan"                    },
    { 351, 850, 860, "Portugal"                  },
    { 358, 850, 437, "Finland"                   },
    { 785, 864, 850, "Saudi Arabia"              },
    { 972, 862, 850, "Israel"                    },
    {9999, 000, 000, "Unknown"                   }
};

#define iMAXCOUNTRYNAME (sizeof(vctrynm) / sizeof(COUNTRYNAME))
#define iINVALID_COUNTRYNAME 9999        /* invalid country name/code */

/* ------------------------------------------------------------------------ */

/* function prototypes */

int main(int argc, char *argv[]);
void DisplayCountryInfo(COUNTRYINFO ctryi);
void TestErrorCode(USHORT usError, char *pszApiName);
void DisplayCollateInfo(CHAR acCollate[], USHORT usSize);
void ValidateCountryCode(USHORT usCountry);
void ValidateCodepage(USHORT usCodepage);
COUNTRYNAME FindCountryName(USHORT usCountry);
void DisplayCountryName(USHORT usCountry);

/* ------------------------------------------------------------------------ */

/*
 *
 * main()
 *
 * The main program.
 *
 */

int main(int argc, char *argv[])
{

    USHORT usErrorCode;       /* generic OS/2 API return code */
    COUNTRYCODE ctryc;        /* DosGetCntryInfo() input buffer */
    COUNTRYINFO ctryi;        /* DosGetCntryInfo() output buffer */
    USHORT usCtryI;           /* sizeof the COUNTRYINFO structure */
    CHAR acCollateTbl[256];   /* collating table buffer */
    USHORT usCollateTblSize;  /* size of collating table buffer */
    USHORT rgusCpList[50];    /* the codepage table */
    USHORT cbCpList;          /* the size of the codepage table (bytes) */
    USHORT usActiveCp;        /* the active codepage */
    int i;                    /* loop index (for codepage table) */

    /* specify the country/codepage to use */
    if (argc == 3)
    {
        /* user entered country and codepage on command line */
        ctryc.country = atoi(argv[1]);
        ctryc.codepage = atoi(argv[2]);
        ValidateCountryCode(ctryc.country);
        ValidateCodepage(ctryc.codepage);
    }
    else if (argc == 1)
    {
        /* user didn't enter anything; silently assume defaults */
        ctryc.country = 0;                       /* use the default country */
        ctryc.codepage = 0;                     /* use the current codepage */
    }
    else
    {
        /* user is confused; give them help then terminate */
        printf("usage: nlsinfo <countrycode> <codepage>\n");
        exit(1);
    }

    printf("\n\n%s NLS information for codepage %d and country %d.\n\n",
        (_osmajor < 10 ? "DOS" : "OS/2"), ctryc.codepage, ctryc.country);

    /* get the current codepage */
    usErrorCode = DosGetCp(sizeof(rgusCpList), rgusCpList, &cbCpList);
    TestErrorCode(usErrorCode, "DosGetCp");

    /* get/validate the active codepage */
    usActiveCp = rgusCpList[0];
    printf("Active codepage:     %d\n", usActiveCp);
    ValidateCodepage(usActiveCp);

    printf("Prepared codepages:  ");
    /* loop through each codepage, a word at a time */
    for (i = 0; i < (cbCpList/2); i++)
    {
        printf("%d  ", rgusCpList[i]);
        ValidateCodepage(rgusCpList[i]);
    }
    printf("\n\n");

    /* get/display country information */
    usErrorCode = DosGetCtryInfo(sizeof(COUNTRYINFO), &ctryc, &ctryi, &usCtryI);
    TestErrorCode(usErrorCode, "DosGetCtryInfo");
    DisplayCountryName(ctryi.country);          /* get/display country name */
    DisplayCountryInfo(ctryi);

    /* get/display collating table information */
    usErrorCode = DosGetCollate(sizeof(acCollateTbl), &ctryc, acCollateTbl,
        &usCollateTblSize);
    TestErrorCode(usErrorCode, "DosGetCollate");
    DisplayCollateInfo(acCollateTbl, usCollateTblSize);

    /* get/display dbcs table information */

    /* get/display codepage information */

    exit(0);
    return (0);

} /* main */

/* ------------------------------------------------------------------------ */

/*
 *
 * DisplayCountryInfo()
 *
 * Displays information obtained from the DosGetCtryInfo API.
 *
 */

void DisplayCountryInfo(COUNTRYINFO ctryi)
{

    /* display the country information */
    printf("Country code:        %03d\n", ctryi.country);

    printf("Date separator:      %s\n",   ctryi.szDateSeparator);

    switch (ctryi.fsDateFmt)
    {
        case DATEFMT_MM_DD_YY:
            printf("Date format:         MM%sDD%sYY\n",
                   ctryi.szDateSeparator, ctryi.szDateSeparator,
                   ctryi.szDateSeparator);
            break;

        case DATEFMT_DD_MM_YY:
            printf("Date format:         DD%sMM%sYY\n",
                   ctryi.szDateSeparator, ctryi.szDateSeparator,
                   ctryi.szDateSeparator);
            break;

        case DATEFMT_YY_MM_DD:
            printf("Date format:         YY%sMM%sDD\n",
                   ctryi.szDateSeparator, ctryi.szDateSeparator,
                   ctryi.szDateSeparator);
            break;
    }

    printf("Time separator:      %s\n",   ctryi.szTimeSeparator);

    printf("Clock type:          %s\n",
         (ctryi.fsTimeFmt == 0x0001 ? "12 hour clock" : "24 hour clock"));

    printf("Currency symbol:     %s\n",   ctryi.szCurrency);

    printf("Currency places:     %d\n",   ctryi.cDecimalPlace);

/*
 * 
 * NOTE: The following switch statement needs to be modified to 
 *       support the CURRENCY_FOLLOW and CURRENCY_SPACE naming 
 *       conventions as defined in BSEDOS.H. It works as it is, 
 *       but would be better if using the provided #definitions.
 *
 */
    switch (ctryi.fsCurrencyFmt)
    {
        case 0:
            printf("Currency rule:       currency symbol precedes the value\n");
            printf("Currency rule:       no spaces between symbol and value\n");
            break;

        case 1:
            printf("Currency rule:       currency symbol follows the value\n");
            printf("Currency rule:       no spaces between symbol and value\n");
            break;

        case 2:
            printf("Currency rule:       currency symbol precedes the value\n");
            printf("Currency rule:       one space between symbol and value\n");
            break;

        case 3:
            printf("Currency rule:       currency symbol follows the value\n");
            printf("Currency rule:       one space between symbol and value\n");
            break;

        case CURRENCY_DECIMAL:
            printf("Currency rule:       currency symbol replaces decimal separator\n");
            break;
    }

    printf("Decimal separator:   %s\n",   ctryi.szDecimal);

    printf("Thousands separator: %s\n",   ctryi.szThousandsSeparator);

    printf("Data separator:      %s\n",   ctryi.szDataSeparator);

} /* DisplayCountryInfo */

/* ------------------------------------------------------------------------ */

/*
 *
 * DisplayCollateInfo()
 *
 * Displays information obtained from the DosGetCollate API.
 *
 */

void DisplayCollateInfo(CHAR acCollate[], USHORT usSize)
{

    register int i;

    printf("\nCollating table has %d characters:\n\n", usSize);
    for (i = 0; i < usSize; i++)
    {
        printf("%4d%s", acCollate[i],
            (i % 8 == 7 || i == usSize - 1) ? "\n" : ", "); /* see K&R p48 */
    }

} /* DisplayCollateInfo */

/* ------------------------------------------------------------------------ */

/*
 *
 * DisplayCountryName()
 *
 * Displays information obtained from our hard-coded table.
 *
 */

void DisplayCountryName(USHORT usCountry)
{

    COUNTRYNAME ctrynm;

    /* display the country name from our lookup table */
    ctrynm = FindCountryName(usCountry);
    if (usCountry == iINVALID_COUNTRYNAME)
    {
        printf("This country's name is unknown!\n");
    }
    else
    {
        printf("Country name:        %s\n", ctrynm.pszName);
        printf("Primary codepage:    %03d\n", ctrynm.usCodepagePrimary);
        printf("Secondary codepage:  %03d\n", ctrynm.usCodepageSecondary);
    }

} /* DisplayCountryName */

/* ------------------------------------------------------------------------ */

/*
 *
 * FindCountryName()
 *
 * Find the country name/mnemonic from our hard-coded table.
 *
 */

COUNTRYNAME FindCountryName(USHORT usCountry)
{
    register int i;

    /* this lookup algorithm is unefficient... */
    for (i = 0; i < iMAXCOUNTRYNAME; i++)
    {
        if (usCountry == vctrynm[i].usCountry)
        {
            return (vctrynm[i]);
        }
    }

    return (vctrynm[iINVALID_COUNTRYNAME]);

} /* FindCountryName */

/* ------------------------------------------------------------------------ */

/*
 *
 * ValidateCountryCode()
 *
 * Verifies that the given country code is a valid one.
 *
 */

void ValidateCountryCode(USHORT usCountry)
{

    switch (usCountry)
    {
        case   0: /* this is only here since 0 means default */
        case   1:
        case   2:
        case   3:
        case  31:
        case  32:
        case  33:
        case  34:
        case  39:
        case  41:
        case  44:
        case  45:
        case  46:
        case  47:
        case  49:
        case  61:
        case  81:
        case  82:
        case  86:
        case  88:
        case 351:
        case 358:
        case 785:
        case 972:
            /* these are all known country codes */
            return;
            break;

        default:
            printf("Error, invalid country code %d\n", usCountry);
            exit(1);
            break;
    }

} /* ValidateCountryCode */

/* ------------------------------------------------------------------------ */

/*
 *
 * ValidateCodepage()
 *
 * Verifies that the given codepage is a valid one.
 *
 */

void ValidateCodepage(USHORT usCodepage)
{

    switch (usCodepage)
    {
        case   0: /* this is only here since 0 means default */
        case 437:
        case 850:
        case 860:
        case 862:
        case 863:
        case 864:
        case 865:
        case 932:
        case 934:
        case 936:
        case 938:
            /* these are all known codepages */
            return;
            break;

        default:
            printf("Error, invalid codepage %d\n", usCodepage);
            exit(1);
            break;
    }

} /* ValidateCodepage */

/* ------------------------------------------------------------------------ */

/*
 *
 * TestErrorCode()
 *
 * Tests an OS/2 API return code, and terminates if nonzero/unsuccessful.
 *
 */

void TestErrorCode(USHORT usError, char *pszApiName)
{

    if (usError != 0)
    {
        printf("  %s failed with code %d!\n", pszApiName, usError);
        exit(1);
    }

} /* TestErrorCode */

/* ------------------------------------------------------------------------ */

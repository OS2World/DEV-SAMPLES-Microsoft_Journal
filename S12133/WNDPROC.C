#define INCL_WIN
#define INCL_GPI


#include <os2.h>
#include "APP.h"
#include <stddef.h>
#include <string.h>

extern char szAppName[];
extern char szEdit[];
LONG   FontsAvail=30;
extern HAB    hAB;
HPS    hPS;
FONTMETRICS fm[30];
FATTRS	 vfat;
FONTMETRICS FontMetrics;
OWNERITEM FAR * oi;
int i=0;

MRESULT FAR PASCAL APPWndProc( hWnd, message, mp1, mp2 )
HWND    hWnd;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{

    switch( message )
    {
    case WM_CREATE:
	    hPS = WinGetPS (hWnd);
	/*load courier font set */
	    GpiLoadFonts( hAB, (PSZ) "c:\\os2\\dll\\courier.fon");
	/*query all available private fonts */
	    GpiQueryFonts (hPS, (ULONG)QF_PRIVATE,
			   NULL, &FontsAvail, (LONG) sizeof(FONTMETRICS), fm);

	/*find suitable fixed pitch font */
	    while((i<=FontsAvail))
	    {

	       if((fm[i].fsDefn & 0x8000) && (fm[i].fsType & FATTR_TYPE_FIXED))
		  break;
	       i++;
	    }

      /*prepare FATTRS stuct for later use in WM_DRAWITEM processing in
	GpiCreateLogFont call			       */
	    vfat.usRecordLength  =  sizeof(FATTRS) ;
	    vfat.lMatch 	 =  fm[i].lMatch;
	    vfat.fsSelection	 =  fm[i].fsSelection;
	    strcpy(vfat.szFacename, fm[i].szFacename) ;
	    vfat.idRegistry	 =  fm[i].idRegistry ;
	    vfat.usCodePage	 =  850;
	    vfat.lMaxBaselineExt =  fm[i].lMaxBaselineExt;
	    vfat.lAveCharWidth	 =  fm[i].lAveCharWidth;
	    vfat.fsType 	 =  FATTR_TYPE_FIXED;
	    vfat.fsFontUse	 =  0;

     /* Get metrics  */
	    GpiCreateLogFont( hPS, (PSTR8) szAppName,(LONG)i+1, &vfat );
	    GpiSetCharSet(hPS,(LONG)i+1);
	    GpiQueryFontMetrics (hPS, (long) sizeof FontMetrics, &FontMetrics);
	    GpiSetCharSet(hPS,0L);
	    WinReleasePS (hPS);
            return (0);



    case WM_CLOSE:
	hPS = WinGetPS (hWnd);

	GpiDeleteSetId ( hPS,(LONG)2);
	WinReleasePS (hPS);

        WinPostMsg( hWnd, WM_QUIT, 0L, 0L );
        break;

    case WM_COMMAND:
        switch (LOUSHORT(mp1)) {

            case IDITEM:
                APPCommand( hWnd, message, mp1, mp2 );
                break;

            case IDENTER:
		if (WinDlgBox( HWND_DESKTOP, hWnd, (PFNWP)EditDlg, NULL, IDD_INPUT,
		    NULL )) {
			WinMessageBox( HWND_DESKTOP, hWnd,
				       szEdit,
				       "Edit Control", NULL,
				       MB_OK | MB_ICONEXCLAMATION );
                }
                break;

            case IDABOUT:
		WinDlgBox( HWND_DESKTOP, hWnd, (PFNWP)About, NULL, IDD_ABOUT,
		    NULL );
                break;

            default:
                break;
            }
            break;


    case WM_PAINT:
        APPPaint( hWnd, message, mp1, mp2 );
        break;

    case WM_ERASEBACKGROUND:
	return TRUE;
	break;

    case WM_DRAWITEM:
       oi = mp2;
    /*create log font and set char set for Presentation Space of Listbox (oi->hps)*/
       GpiCreateLogFont(oi->hps, (PSTR8) szAppName,(LONG)i+1, &vfat );
       GpiSetCharSet(oi->hps,(LONG)i+1);
    /*return false to tell listbox proc to draw items  */
       return(FALSE);



    default:
        return WinDefWindowProc( hWnd, message, mp1, mp2 );
        break;

    }

    return( 0L );
}
/*
*/

/*
*/

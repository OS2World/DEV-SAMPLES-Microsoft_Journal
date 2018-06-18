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
LONG lCurrentId;
CHAR pszBuf[20];
LONG	 lLength;
POINTL	 pPoint;
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

	       if((fm[i].fsDefn & 0x8000) && (fm[i].fsType & 0x0001))
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
        WinPostMsg( hWnd, WM_QUIT, 0L, 0L );
        break;

    case WM_COMMAND:
        switch (LOUSHORT(mp1)) {

	    case IDADDSTRINGS:
		AddStrings( hWnd, message, mp1, mp2 );
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

    case WM_MEASUREITEM:
     /*   Set the item height in the listbox */
     /*   Note that the LOUSHORT of mp1 is the listbox ID */

	return(FontMetrics.lMaxBaselineExt);


    case WM_DRAWITEM:
       oi = mp2;
    /* If desired char set not allready selected, create log font and set */
    /* char set for Presentation Space of Listbox (oi->hps)*/

       if(!(GpiQueryCharSet(oi->hps) == i+1))
       {
	GpiCreateLogFont(oi->hps, (PSTR8) szAppName,(LONG)i+1, &vfat );
	GpiSetCharSet(oi->hps,(LONG)i+1);
       }
       if (oi->fsState == oi->fsStateOld)
       {
	WinFillRect(oi->hps,&(oi->rclItem),CLR_WHITE);
	GpiSetBackMix(oi->hps,BM_OVERPAINT);
	GpiSetColor(oi->hps,CLR_BLUE);
	lLength = WinSendMsg(oi->hwnd,LM_QUERYITEMTEXT,(MPARAM)MAKEULONG(oi->idItem,20),
		   (MPARAM)pszBuf);
	pPoint.x = oi->rclItem.xLeft;
	pPoint.y = oi->rclItem.yBottom + 1;
	GpiCharStringAt(oi->hps,/*&(oi->rclItem.xLeft)*/ &pPoint,lLength +1,pszBuf);
	return(TRUE);
       }
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

#define INCL_GPI
#define INCL_WIN

#include <os2.h>
#include <stddef.h>
#include "stdlib.h"
#include "hello.h"
#include "arcp.h"

VOID SetDialogItemLong(HWND,USHORT,LONG);
LONG GetDialogItemLong(HWND,USHORT);

PFNWP pfnOldFrameProc;
RECTL  rRect;

MRESULT FAR PASCAL ArcParamDlg2( HWND , USHORT , MPARAM , MPARAM  );

HWND   EP,EQ,ER,ES;
ARCPARAMS   AP;


/* This is the proc for the inner dialog box.  It contains the controls
   etc.*/

MRESULT FAR PASCAL ArcParamDlg2( hWndDlg, message, mp1, mp2)
HWND   hWndDlg;
USHORT message;
MPARAM mp1;
MPARAM mp2;
{

switch( message )
    {
    case WM_COMMAND:
        switch( SHORT1FROMMP(mp1) )
        {
	  case DID_OK:
	    /* must dismiss parent dialog*/
	    WinDismissDlg( WinQueryWindow(hWndDlg,QW_PARENT,FALSE), TRUE );
            break;

        default:
            return( FALSE );
        }
        break;

    default:
	return( WinDefDlgProc( hWndDlg, message, mp1, mp2) );
    }

return( FALSE );
}



/* Outer frame (dlgbox) dialog procedure.  The icon will actually belong
   to this window.  Note: During WM_INITDLG time I set the icon and
   load the inner dialog box which contains the controls. */

MRESULT FAR PASCAL ArcParamDlg( HWND hWnd, USHORT message, MPARAM mp1, MPARAM mp2 )
{

    switch ( message )
    {
    HPOINTER hptr;
    SWP      swp;
	case WM_INITDLG:
	    hptr = WinLoadPointer(HWND_DESKTOP,NULL,IDD_ARCICO);
	    WinSendMsg(hWnd,WM_SETICON,(MPARAM)hptr,0L);

	    WinLoadDlg( hWnd, hWnd, ArcParamDlg2, NULL, IDD_ARCPARAM2, NULL );
	    /* The following code is there to get the client initially sized.*/
	    /* Get a copy of the current client window size and style flags */
	    WinQueryWindowPos( hWnd, &swp );
	    /* Size and position this new client window using above info. */
	    WinSetWindowPos( hWnd, swp.hwndInsertBehind, swp.x, swp.y, swp.cx+1, swp.cy+1, swp.fs | SWP_MOVE | SWP_SIZE | SWP_ZORDER );

	    break;


        default:
	    return( WinDefDlgProc(hWnd, message, mp1, mp2 ));
    }
    return (FALSE);
}

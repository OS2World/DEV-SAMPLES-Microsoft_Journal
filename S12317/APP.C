#define INCL_GPI
#define INCL_WIN


/**********************************************************************
*								      *
*								      *
*  This program illustrates how to Change the Font used in a List Box *
*								      *
*  This would be useful if your application needed to have list boxes *
*  that display information in columns or if you just wanted to use a *
*  a non-proportional font.					      *
*								      *
*  This is accomplished with the following steps:		      *
*								      *
*  1. Query the available fonts and select one that you wish to use.  *
*								      *
*  2. Use the style LS_OWNERDRAW when creating your list-box control. *
*  This style will cause a WM_MEASUREITEM message to be sent to the   *
*  owner when the listbox is created and a WM_DRAWITEM message to be  *
*  sent to the owner of the control whenever a list-box item is about *
*  to be drawn. 						      *
*								      *
*  3. In the message processing for the WM_MEASUREITEM message you    *
*  return the height of the listbox items.			      *
*								      *
*  4. In the message processing for the WM_DRAWITEM, you need to set  *
*  the font selection to the desired font and return FALSE. The mp2   *
*  parameter will contain the address of the OWNERITEM structure      *
*  associated with the item that needs drawing. This structure	      *
*  contains the PS for the list box, among other things.	      *
*								      *
***********************************************************************/


#include <os2.h>
#include <stddef.h>
#include "APP.h"

HAB     hAB;
HMQ     hMsgQ;
HWND    hwndApp;
HWND    hwndAppFrame;
HWND	hwndKid;
HWND	hwndKidFrame;
HWND	hwndList;
extern FONTMETRICS FontMetrics;

/* Write-once global variables */
char    szAppName[10];
char	szKidName[10];
ULONG	style = FCF_STANDARD;
ULONG	KidCtlData = FCF_TITLEBAR | FCF_SYSMENU;

SHORT cdecl main(  )
{
    QMSG    qMsg;

    hAB   = WinInitialize(NULL);
    hMsgQ = WinCreateMsgQueue( hAB, 0 );

    WinLoadString( hAB, NULL, IDSNAME, sizeof(szAppName), (NPCH)szAppName );
    WinLoadString( hAB, NULL, IDSKIDNAME, sizeof(szKidName), szKidName );

    if ( !WinRegisterClass( hAB, (NPCH)szAppName, (PFNWP)APPWndProc,
            CS_CLIPCHILDREN | CS_SIZEREDRAW, NULL ) )
        return( FALSE );


    /* Create a window instance of class szAppName */
    hwndAppFrame = WinCreateStdWindow(
        HWND_DESKTOP,     /* specify desktop as parent window           */
	FS_ICON | FS_ACCELTABLE | WS_VISIBLE,
	&style,
        (NPCH)szAppName,  /* window class name                          */
        (NPCH)szAppName,  /* name appearing in window caption           */
        0L,
        NULL,             /*  use current executable module id          */
	ID_APP, 	  /*  menu id and accelerator id and icon id	*/
        (HWND FAR *)&hwndApp  /* window handle                          */
        );


//                      Add the list box to the client window
//
/* note the LS_OWNERDRAW style.  This will cause a WM_DRAWITEM message
   to be sent to the owner of the listbox			       */
    hwndList = WinCreateWindow (hwndApp, WC_LISTBOX, NULL,
			       WS_VISIBLE | LS_OWNERDRAW, 0, 0, 0, 0,
			       hwndApp, HWND_BOTTOM, LID_LIST1, NULL, NULL);
    WinSetWindowPos (hwndList, HWND_BOTTOM, 10, 10, (short) 100,
			     (short) 100, SWP_SIZE | SWP_MOVE | SWP_SHOW);





    while( WinGetMsg( hAB, (PQMSG)&qMsg, (HWND)NULL, 0, 0 ) )
    {
        WinDispatchMsg( hAB, (PQMSG)&qMsg );
    }

    WinDestroyWindow( hwndAppFrame );
    WinDestroyMsgQueue( hMsgQ );
    WinTerminate( hAB );
}
/*
*/

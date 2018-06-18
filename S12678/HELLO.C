/*
    NoClose - An example to show how to write a 1.21 application that
	      can't ever be closed except by a system shutdown. The is useful
	      for controlling shells, etc.

	This sample shows how to make an unkillable 1.21 application
    similar to the way the Print Manager is implemented. This is guaranteed
    to work in OS/2 1.21, but it uses undocumented features which may change
    in future versions.


    Ed Mills
    Microsoft
    August 1990
    */

#define INCL_BASE
#define INCL_PM
#include <os2.h>
#include <string.h>
#include "hello.h"

HAB     hAB;
HMQ     hmqHello;
HWND    hwndHello;
HWND    hwndHelloFrame;

CHAR    szClassName[] = "Hello";
CHAR    szMessage[]   = "Hello World";

VOID PASCAL FAR SignalHandler(USHORT, USHORT);


/***************************** Private Function ****************************\
*
* Hello World Main procedure
*
* Effects: Set globals   - hAB        Handle to Application Anchor Block
*                        - hmqHello   Handle to application window msg     queue
*
* Warnings: None
*
\***************************************************************************/

int cdecl main( )
{
    QMSG	qmsg;
    ULONG	ctldata;
    HWND	hWndSysMenu;
    HWND	hWndSysSubMenu;
    MENUITEM	miSysMenu;
    SHORT	idSysMenu;
    USHORT	iSepPos, iSepID;
    HWND far *	pShellWindow;	     /* Exported by OS2SM.DLL */
    USHORT	pid, tid;
    SWCNTRL	sw;
    HMODULE	hmod;


    /* Set a signal handler to trap SIG_KILLPROCESS so that we can't
       be killed when the user hits a CTRL-C or CTRL-BREAK from a
       windowed command shell, or kills our parent, etc.

       By not specifying SIGA_ACKNOWLEDGE, we prevent the signal from
       occuring a second time.
       */
    if (DosSetSigHandler(SignalHandler, NULL, NULL,
			 SIGA_ACCEPT, SIG_KILLPROCESS)) {
	DosBeep(1000, 200);
	return(1);
	}

    hAB = WinInitialize(NULL);

    hmqHello = WinCreateMsgQueue(hAB, 0);

    /* Don't allow the task manager to shut this queue down. */
    WinCancelShutdown(hmqHello, TRUE );


    if (!WinRegisterClass( hAB,
                           (PCH)szClassName,
                           (PFNWP)HelloWndProc,
                           CS_SYNCPAINT | CS_SIZEREDRAW,
                           0))
        return( 0 );

    ctldata = FCF_STANDARD & ~FCF_MENU & ~FCF_ACCELTABLE & ~FCF_TASKLIST;

    hwndHelloFrame = WinCreateStdWindow( HWND_DESKTOP,
					 0L,
  					 &ctldata,
                                         (PCH)szClassName,
					 "Unkillable Application",
                                         0L,
                                         (HMODULE)NULL,
                                         HELLOICON,
                                         (HWND FAR *)&hwndHello );

    /* Get handle of the [-] System Menu */
    hWndSysMenu = WinWindowFromID (hwndHelloFrame, FID_SYSMENU);

    /* Get the id of the zeroth item of the system menu */
    idSysMenu = (SHORT) WinSendMsg (hWndSysMenu,
				    MM_ITEMIDFROMPOSITION, 0L, 0L);

    /* Get the MENUITEM structure for the system menu */
    WinSendMsg (hWndSysMenu,
		MM_QUERYITEM,
		(MPARAM)MAKEULONG (idSysMenu, FALSE),
		(MPARAM)(PMENUITEM)&miSysMenu);

    /* Get handle of the system sub menu */
    hWndSysSubMenu = miSysMenu.hwndSubMenu;

    /* Remove the close seperator. */
    iSepPos = (USHORT)WinSendMsg(hWndSysSubMenu,
				 MM_ITEMPOSITIONFROMID,
				 MPFROM2SHORT((USHORT)SC_CLOSE, (BOOL)FALSE),
				 0L) - 1;
    iSepID = (USHORT)WinSendMsg(hWndSysSubMenu,
				MM_ITEMIDFROMPOSITION,
				MPFROMSHORT((SHORT)iSepPos),
				0L);
    WinSendMsg(hWndSysSubMenu,
	       MM_REMOVEITEM,
	       MPFROM2SHORT((USHORT)iSepID, (BOOL)FALSE),
	       0L);

    /* Remove the sysmenu close. */
    WinSendMsg(hWndSysSubMenu,
	       MM_REMOVEITEM,
	       MPFROM2SHORT((USHORT) SC_CLOSE, (BOOL)FALSE),
	       0L);


    /* Open it iconic. */
    WinSetWindowPos(hwndHelloFrame, HWND_TOP, 0, 0, 0, 0,
		    SWP_MINIMIZE | SWP_SHOW);


    /* Now add the unkillable switch list entry for this application.
       Note that this works but uses all sorts of undocumented stuff
       which may change in the future.
       We probably also want to use a system semaphore or such to
       prevent more than one of these from running at a time.
       */

    /* Get the handle to the shell. */
    DosLoadModule(NULL, 0, "OS2SM", &hmod);
    DosGetProcAddr(hmod, "SHELLWINDOW", &pShellWindow);
    DosFreeModule(hmod);

    WinQueryWindowProcess(hwndHelloFrame, &pid, &tid);

    strcpy(sw.szSwtitle, "Unkillable");
    sw.hwnd	     = hwndHelloFrame;
    sw.hwndIcon      = (ULONG)NULL;
    sw.hprog         = (ULONG)NULL;
    sw.idProcess     = pid;
    sw.idSession     = (USHORT)0;
    sw.uchVisibility = SWL_VISIBLE;
    sw.fbJump        = SWL_JUMPABLE;

#define SH_SWITCHLIST	    0x0080
#define SWL_NOTERMINATE     4

    /* Add us to the list. */
    WinPostMsg(*pShellWindow, SH_SWITCHLIST, (MPARAM)SWL_NOTERMINATE,
            (MPARAM)WinAddSwitchEntry(&sw));


    /* Poll messages from event queue */
    while (WinGetMsg( hAB, (PQMSG)&qmsg, (HWND)NULL, 0, 0 ) )
	WinDispatchMsg( hAB, (PQMSG)&qmsg );


    WinDestroyWindow( hwndHelloFrame );
    WinDestroyMsgQueue( hmqHello );
    WinTerminate( hAB );
}



/***************************** Private Function ****************************\
*
* Procedure which processes window messages
*
* Effects: None
*
* Warnings: None
*
\***************************************************************************/

MRESULT EXPENTRY _loadds HelloWndProc( hWnd, msg, mp1, mp2 )
HWND   hWnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    HPS    hPS;
    POINTL pt;
    CHARBUNDLE cb;

    switch (msg)
    {
    case WM_CLOSE:
        WinPostMsg( hWnd, WM_QUIT, 0L, 0L );
        break;

    case WM_PAINT:
        hPS = WinBeginPaint( hWnd, (HPS)NULL, (PWRECT)NULL );
        pt.x = pt.y = 0L;
        cb.lColor = CLR_BLACK;
        GpiSetAttrs(hPS, PRIM_CHAR, CBB_COLOR, 0L, (PBUNDLE)&cb);

        GpiCharStringAt( hPS, (PPOINTL)&pt, (LONG)sizeof(szMessage)-1,
                        (PCH)szMessage );
        WinEndPaint( hPS );
        break;

    case WM_ERASEBACKGROUND:
        return( TRUE );
        break;

    default:
        return( WinDefWindowProc( hWnd, msg, mp1, mp2 ) );
        break;
    }
    return(0L);
}


VOID PASCAL FAR SignalHandler(usSigArg, usSigNum)
USHORT usSigArg;    /* furnished by DosFlagProcess if appropriate */
USHORT usSigNum;    /* signal number being processed              */
{
    /* We just ignore the signal. */
    return;

usSigArg;   /* These are just to shut up the compiler. */
usSigNum;
}

/*
 * sample: attempt to make child windows of HWND_DESKTOP invisible.
 *	   a dialogue box to exit sample is invoked, and refers to
 *	   a readme file attached which reports the strange behaviors
 *	   we have encountered.
 */
#define INCL_WIN
#include <os2.h>
#include <string.h>

char szBuf[80];
RECTL rOldSize;

SHORT cdecl main(  )
{
    HAB   hAB;
    HMQ   hMsgQ;
    HENUM henum;
    HWND  next;
    HWND  hwndClient;

    hAB   = WinInitialize(NULL);
    hMsgQ = WinCreateMsgQueue( hAB, 0 );

    /*
     * To disable the Task Manager window, our solution is to
     * obtain its hwnd via enumerating the child windows of
     * HWND_DESKTOP and make it invisible via WinSetWindowPos().
     */


     henum = WinBeginEnumWindows( HWND_DESKTOP );

     while( (next = WinGetNextWindow(henum)) != NULL ) {
	   WinLockWindow( next, FALSE );  /* Unlock the windows locked above */
	   /* next is the window handle of direct children of HWND_DESKTOP */
	   /* Now we'll see if there is client window and what it's class is */
	   hwndClient = WinWindowFromID( next, FID_CLIENT );
	   if (hwndClient) {
	       WinQueryClassName( hwndClient, sizeof( szBuf ), szBuf );
	       /* Hide the program starter */
	       if (!strcmp( "StarterWindow", szBuf)) {
		  WinQueryWindowRect(next,&rOldSize);
		  WinSetWindowPos(next,HWND_BOTTOM,
				  rOldSize.xLeft,rOldSize.yBottom,
				  rOldSize.xRight,rOldSize.yTop,
				  SWP_MINIMIZE | SWP_MOVE | SWP_SIZE);
	       }
	   }
     }

     WinEndEnumWindows( henum );

     WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
		    "OK to Exit... please read README ", "WHA?",
		    NULL, MB_OK );

     WinDestroyMsgQueue( hMsgQ );
     WinTerminate( hAB );
     return 0;
} /* main */

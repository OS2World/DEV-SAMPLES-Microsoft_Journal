#define INCL_PM

#include <os2.h>
#include "stdio.h"
#include <stddef.h>
#include "hello.h"


HAB     hAB;
HMQ     hmqHello;
HWND    hwndHello;
HWND    hwndHelloFrame;
HWND	hwndInfo;
CHAR    szClassName[] = "Hello";
CHAR    szMessage[]   = "Hello World";

PFNWP pfnHelloWndProc;

int cdecl main( )
{
    QMSG qmsg;
    ULONG ctldata;

    hAB = WinInitialize(0);

    hmqHello = WinCreateMsgQueue(hAB, 0);

    if (!WinRegisterClass( hAB,(PCH)szClassName,(PFNWP)HelloWndProc,
			   CS_SYNCPAINT | CS_SIZEREDRAW,
                           0))
        return( 0 );

    if (!WinRegisterClass( hAB,(PCH)"InfoClass",(PFNWP)InfoWndProc,
                           CS_SYNCPAINT | CS_SIZEREDRAW | CS_CLIPSIBLINGS,
                           0))
        return( 0 );

    ctldata = FCF_STANDARD &~FCF_ACCELTABLE;

    hwndHelloFrame = WinCreateStdWindow( HWND_DESKTOP,
					 WS_VISIBLE,
					 &ctldata,
                                         (PCH)szClassName,
					 (PCH) NULL,
                                         0L,
                                         (HMODULE)NULL,
                                         ID_HELLO,
                                         (HWND FAR *)&hwndHello );

    hwndInfo=WinCreateWindow(hwndHelloFrame,
		    "InfoClass",
		    NULL,
		    0,
		    0,0,
		    0,0,
		    hwndHelloFrame,
                    WinWindowFromID(hwndHelloFrame, FID_MINMAX),
		    ID_INFOWINDOW,
		    NULL,
		    NULL);

    pfnHelloWndProc=WinSubclassWindow(hwndHelloFrame, (PFNWP) FrameSubProc);

    WinSendMsg(hwndHelloFrame, WM_UPDATEFRAME, 0L,0L);

    WinShowWindow( hwndHelloFrame, TRUE );

    while( WinGetMsg( hAB, (PQMSG)&qmsg, (HWND)NULL, 0, 0 ) )
    {
        WinDispatchMsg( hAB, (PQMSG)&qmsg );
    }

    WinDestroyWindow( hwndHelloFrame );
    WinDestroyMsgQueue( hmqHello );
    WinTerminate( hAB );
}

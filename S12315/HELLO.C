#define INCL_PM

#include <os2.h>
#include "stdio.h"
#include <stddef.h>
#include "hello.h"


HAB     hAB;
HMQ     hmqHello;
HWND    hwndHello;
HWND    hwndHelloFrame;

CHAR    szClassName[] = "Hello";
CHAR    szScrollClass[] = "scroller";
CHAR    szMessage[]   = "Hello World";

int     nFontHeight,nFontWidth;


int cdecl main( )
{
    QMSG qmsg;
    ULONG ctldata;

    hAB = WinInitialize(NULL);

    hmqHello = WinCreateMsgQueue(hAB, 0);

    if (!WinRegisterClass( hAB,(PCH)szClassName,(PFNWP)HelloWndProc,
			   CS_SYNCPAINT | CS_SIZEREDRAW,
                           0))
        return( 0 );

    ctldata = FCF_STANDARD & ~FCF_ACCELTABLE;

    hwndHelloFrame = WinCreateStdWindow( HWND_DESKTOP,
					 WS_VISIBLE  | FS_ICON,
					 &ctldata,
                                         (PCH)szClassName,
                                         (PCH)szMessage,
                                         0L,
                                         (HMODULE)NULL,
                                         ID_HELLO,
                                         (HWND FAR *)&hwndHello );

    WinShowWindow( hwndHelloFrame, TRUE );

    while( WinGetMsg( hAB, (PQMSG)&qmsg, (HWND)NULL, 0, 0 ) )
    {
        WinDispatchMsg( hAB, (PQMSG)&qmsg );
    }

    WinDestroyWindow( hwndHelloFrame );
    WinDestroyMsgQueue( hmqHello );
    WinTerminate( hAB );
}

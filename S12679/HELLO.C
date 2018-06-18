/****************************** Module Header ******************************\
* Module Name: HELLO.C
*
* The sample shows how to guarantee that an application will return to
* the full screen screen group which started it once the application
* terminates.
*
* Ed Mills
* August 1990
*
* Created by Microsoft Corp., 1988
*
\***************************************************************************/

#define INCL_PM
#include <os2.h>
#include "hello.h"

HAB     hAB;
HMQ     hmqHello;
HWND    hwndHelloFrame;


/***************************** Private Function ****************************\
*
* Hello World Main procedure
*
* Effects: Set globals   - hAB        Handle to Application Anchor Block
*			 - hmqHello   Handle to application window msg queue
*
* Warnings: None
*
\***************************************************************************/

int cdecl main( )
{
    hAB = WinInitialize(NULL);

    hmqHello = WinCreateMsgQueue(hAB, 0);

    hwndHelloFrame = WinCreateWindow(HWND_DESKTOP,
				     /* It must be a frame. */
				     WC_FRAME,
				     NULL,
				     /* It must be visible. */
				     WS_VISIBLE,
				     /* This keeps it from ever showing. */
				     0, 0, 0, 0,
				     NULL,
				     HWND_TOP,
				     NULL, NULL, NULL);


    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
		  "This is just a simple message box", "FULLSCRN",
		  1717, MB_OK);


    WinDestroyWindow( hwndHelloFrame );

    WinDestroyMsgQueue( hmqHello );
    WinTerminate( hAB );
    return(0);
}

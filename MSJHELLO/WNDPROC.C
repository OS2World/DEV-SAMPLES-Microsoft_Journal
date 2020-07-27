#define INCL_PM
#include <os2.h>
#include <stddef.h>

#include "hello.h"
#include <stdio.h>


extern void HelloPaint(HWND);

HAB  hAB;
HPS  hNPS;
SIZEL SL;
POINTL pt;
char M[80];
long res[20];


MRESULT FAR PASCAL HelloWndProc( hWnd, msg, mp1, mp2 )
HWND   hWnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
switch (msg)
  {
   case WM_CREATE:
      {
       HDC  hDC = WinOpenWindowDC(hWnd);
       SL.cx = SL.cy = 0;
       hNPS = GpiCreatePS(hAB,hDC,&SL,PU_PELS |
		       GPIF_DEFAULT | GPIT_MICRO |
		       GPIA_ASSOC);
      }
       break;

   case WM_DESTROY:
      GpiDestroyPS(hNPS);
      break;

   case WM_CLOSE:
      WinPostMsg( hWnd, WM_QUIT, 0L, 0L );
      break;

   case WM_COMMAND:
	HelloCommand(hWnd,SHORT1FROMMP(mp1));
	break;

   case WM_PAINT:
        HelloPaint(hWnd);
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

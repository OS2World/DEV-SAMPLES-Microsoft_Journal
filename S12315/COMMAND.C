#define INCL_PM

#include <os2.h>
#include <stddef.h>
#include "stdio.h"
#include "hello.h"
#include "arcp.h"


extern CHAR    szFileName[80] ;
extern CHAR    szScrollClass[];
extern HWND    hwndHelloFrame;
extern HAB     hAB;


extern ULONG FAR PASCAL ArcParamDlg(HWND,USHORT,MPARAM,MPARAM);


BOOL HelloCommand( hWnd, Menu)
HWND	hWnd;
USHORT Menu;
{
switch (Menu)
  {
   case IDM_OPEN:
    if(WinDlgBox(hWnd ,hWnd ,  (PFNWP)ArcParamDlg, NULL,
			  IDD_ARCPARAM, NULL ) == FALSE)
       return(TRUE);
    WinInvalidateRect(hWnd,NULL,TRUE);
    WinUpdateWindow(hWnd);
    break;

   case IDM_CLOSE:
    break;

   default:
     return(FALSE);
     break;
  }
return(TRUE);
}


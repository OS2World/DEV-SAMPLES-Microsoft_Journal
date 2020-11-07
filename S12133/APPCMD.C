#define INCL_WIN

#include <os2.h>
#include <stddef.h>
#include "APP.h"
#include <string.h>

extern HWND hwndList;
int index;
char szEdit[256] = "This is a test";
char temp[15];

MRESULT FAR PASCAL APPCommand( hWnd, message, mp1, mp2 )
HWND    hWnd;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{


		    index = strlen ("Window");
		    strncpy (temp, "Window", index);
		    temp [index] = '\0';
		    WinSendMsg (hwndList, LM_INSERTITEM, MPFROM2SHORT (LIT_SORTASCENDING, 0),
				MPFROMP (temp));
		    index = strlen ("PM Land");
		    strncpy (temp, "PM Land", index);
		    temp [index] = '\0';

		    WinSendMsg (hwndList, LM_INSERTITEM, MPFROM2SHORT (LIT_SORTASCENDING, 0),
				MPFROMP (temp));

		   return (0);


}
/*
*/


MRESULT FAR PASCAL About( hWndDlg, message, mp1, mp2 )
HWND    hWndDlg;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{
    switch( message )
    {
      case WM_COMMAND:
        switch( LOUSHORT( mp1 ) )
        {
          case DID_OK:
            WinDismissDlg( hWndDlg, TRUE );
            break;

          default:
            break;
        }
        break;

      default:
	return WinDefDlgProc( hWndDlg, message, mp1, mp2 );
    }
    return FALSE;
}
/*
*/

MRESULT FAR PASCAL EditDlg( hWndDlg, message, mp1, mp2 )
HWND    hWndDlg;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{
    switch( message )
    {
      case WM_INITDLG:
	  WinSetDlgItemText( hWndDlg, IDEDIT, szEdit );
          break;

      case WM_COMMAND:
          switch( LOUSHORT( mp1 ) ) {
              case DID_OK:
		  WinQueryDlgItemText( hWndDlg, IDEDIT, 256, szEdit );
                  WinDismissDlg( hWndDlg, TRUE );
                  break;

              case DID_CANCEL:
                  WinDismissDlg( hWndDlg, FALSE );
                  break;

              default:
                  break;
          }
          break;

      default:
        return( WinDefDlgProc( hWndDlg, message, mp1, mp2 ) );
    }
    return FALSE;
}
/*
*/

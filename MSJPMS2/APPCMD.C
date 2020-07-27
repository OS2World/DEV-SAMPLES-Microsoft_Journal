#define INCL_WIN

#include <os2.h>
#include <stddef.h>
#include "APP.h"
#include <string.h>

extern HWND hwndList;
int index;
char szEdit[256] = "This is a test";
char temp[15];


/******************************************************************/
/*  This function adds two strings to the listbox.		  */
/******************************************************************/
MRESULT FAR PASCAL ADDStrings( hWnd, message, mp1, mp2 )
HWND    hWnd;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{


       index = strlen ("String1");
       strncpy (temp, "String1", index);
       temp [index] = '\0';
       WinSendMsg (hwndList, LM_INSERTITEM, MPFROM2SHORT (LIT_SORTASCENDING, 0),
		   MPFROMP (temp));
       index = strlen ("String2");
       strncpy (temp, "String2", index);
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

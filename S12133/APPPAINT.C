#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stddef.h>
#include "APP.h"

extern char     szAppName[];

VOID FAR PASCAL APPPaint( hWnd, message, mp1, mp2 )
HWND   hWnd;
USHORT message;
MPARAM  mp1;
MPARAM  mp2;
{
    HPS         hPS;
    RECTL	rc, rcClip;

    hPS = WinBeginPaint( hWnd, NULL, &rcClip );

    WinQueryWindowRect( hWnd, &rc );
    WinFillRect( hPS, &rc, CLR_WHITE );

    WinEndPaint( hPS );

    return;
}
/*
*/

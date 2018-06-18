#define INCL_PM
#include <os2.h>
#include <stddef.h>
#include <string.h>
#include "hello.h"


/*	For info on using these routines to display debugging output
	on a secondary monitor, see the Software Library file PMUTILS.ARC
	and acompanying KnowledgeBase article Q43027.

#define DEBUG
*/

#ifdef DEBUG
#include "utils.h"
#endif

extern HWND	hwndHelloFrame;
extern PFNWP	pfnHelloWndProc;
extern BOOL bHelp;

extern void HelloPaint(HWND);
extern HWND hwndInfo;
char buffer[256];

void DisplaySelection(HWND hWnd, SHORT idItem, BOOL fPost, HWND hwndMenu)
{
    switch (idItem) {
        case (-1):
	    strcpy(buffer, "");
	    break;

	case 50:  /* Help item ??????????? */
	    strcpy(buffer, "Help item selected");
	    break;

	case 0x2000:
	    strcpy(buffer, "First action bar item");
	    break;

	case ID_DISABLED:
	    strcpy(buffer, "Disabled item, you can't select this one!!!");
	    break;

	case IDM_NEWMENU:
	    strcpy(buffer, "This selection adds an additional menu to the action bar");
	    break;

        case ID_FIRSTITEM:    /* newly added items to pulldown menu */
	    strcpy(buffer, "first newly added item to pulldown menu");
	    break;

        case ID_NEWITEM:
	    strcpy(buffer, "newly added item");
	    break;

	case ID_ADDTOSYSMENU:
	    strcpy(buffer, "add new item to system menu");
	    break;
	case ID_DELSYS:
	    strcpy(buffer, "remove newly added item(s) from system menu");
	    break;
	case 1000:
	    strcpy(buffer, "New action bar item");
	    break;


	default:		     /* show id of menu item selected */
	    strcpy(buffer, "some new item");
	    break;
	}
    WinSendMsg(hwndInfo, WM_UPDATESTRING, buffer,0L);
}


MRESULT EXPENTRY HelloWndProc( hWnd, msg, mp1, mp2 )
HWND   hWnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
USHORT idItem;
BOOL fPost;
HWND hwndMenu;

switch (msg) {
    case WM_HELP:
        DosBeep(500,50);
        break;

    case WM_MENUSELECT:
	idItem = SHORT1FROMMP(mp1);	     /* item identifier */
	fPost = (BOOL) SHORT2FROMMP(mp1);    /* post flag	*/
	hwndMenu = LONGFROMMP(mp2);	     /* menu window	*/

	DisplaySelection(hWnd, idItem, fPost, hwndMenu);

        return ( (MRESULT) TRUE);  /* process the command as usual */
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


    default:
	return( WinDefWindowProc( hWnd, msg, mp1, mp2 ) );
	break;
    }
return(0L);
}


MRESULT EXPENTRY FrameSubProc( hWnd, msg, mp1, mp2 )
HWND   hWnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
USHORT cFrameCtls;
USHORT iswpNext;
USHORT chwnd;
PSWP pswp;
SWP swpClient;

switch (msg) {
    case WM_QUERYFRAMECTLCOUNT:
        cFrameCtls=(USHORT) (ULONG) (pfnHelloWndProc)(hWnd, WM_QUERYFRAMECTLCOUNT, mp1,mp2);
	/* add one for our info window */
        if (!bHelp) return( (MRESULT) cFrameCtls);   /* new control is disabled, so don't add
                                             it to the SWPs */
        return ((MRESULT) (cFrameCtls+1) );
	break;

    case WM_FORMATFRAME:
	pswp=(PSWP) PVOIDFROMMP(mp1);

	/* The other controls normally by calling the frame window proc */

        iswpNext=chwnd= (USHORT) (ULONG) ((pfnHelloWndProc)(hWnd, WM_FORMATFRAME, mp1,mp2));

        if (!bHelp) return( (MRESULT)chwnd);   /* new control is disabled, so don't add it
                                        to the SWPs */

	swpClient=pswp[iswpNext-1];	 /* remember client's SWP */
	iswpNext--;

	/* set up SWP structure for the new control */

	pswp[iswpNext].fs=SWP_SHOW | SWP_SIZE | SWP_MOVE;
                                       /* keep it inside the client area */
        pswp[iswpNext].cy= (CY_INFOWINDOW<swpClient.cy ? CY_INFOWINDOW : swpClient.cy);
	pswp[iswpNext].cx=swpClient.cx;
	pswp[iswpNext].x=swpClient.x;
	pswp[iswpNext].y=swpClient.y;
	pswp[iswpNext].hwndInsertBehind=0;
	pswp[iswpNext].hwnd=hwndInfo;

#ifdef DEBUG
	Debug("info window");
	Debug("pswp.x=%d, .y=%d, .cx=%d, .cy=%d",
	pswp[iswpNext].x,pswp[iswpNext].y,pswp[iswpNext].cx,pswp[iswpNext].cy);
#endif

	iswpNext++;

	pswp[iswpNext]=swpClient;    /* keep client at end of list */

	chwnd++;
				     /* shrink client by space taken by info window */
	pswp[iswpNext].y=pswp[iswpNext-1].cy+pswp[iswpNext-1].y;
	pswp[iswpNext].cy-=pswp[iswpNext-1].cy;

        return( (MRESULT) chwnd);
	break;

    default:
	return( (pfnHelloWndProc)(hWnd, msg, mp1,mp2) );
    }
}


MRESULT EXPENTRY InfoWndProc( hWnd, message, mp1, mp2 )
HWND    hWnd;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{
HPS	    hps;
RECTL	    rc;
RECTL rclClient, rect;
POINTL ptlStart;
switch( message ) {

/* WM_UPDATESTRING sent to window to update its text
   (PCH)    mp1 - info string to display
*/
    case WM_UPDATESTRING:
	hps=WinGetPS(hWnd);

	ptlStart.x=10;
	ptlStart.y=2;

	WinQueryWindowRect(hWnd, &rclClient);

	rect.yTop=rclClient.yTop-1; /* subtract one so it doesn't blast the border */
	rect.yBottom=0;
	rect.xLeft=0;
	rect.xRight=rclClient.xRight;

	WinFillRect(hps, &rect, CLR_BLUE);

	GpiCharStringAt(hps, &ptlStart, strlen(mp1), mp1);
	WinReleasePS(hps);

	break;

    case WM_PAINT:
	WinQueryWindowRect( hWnd, &rc );
	hps = WinBeginPaint( hWnd, NULL, NULL );
	WinFillRect( hps, &rc, CLR_BLUE);

	rc.yTop--;	/* since the rectangle is exclusive on the top/right */
	rc.xRight--;

	/* Draw a line at the top of the info box */
	ptlStart.x=rc.xLeft;
	ptlStart.y=rc.yTop;
	GpiMove(hps,&ptlStart);

	ptlStart.x=rc.xRight;
	ptlStart.y=rc.yTop;
	GpiLine(hps, &ptlStart);

	WinEndPaint( hps );
	break;

    default:
	return WinDefWindowProc( hWnd, message, mp1, mp2 );
	break;
    }
return( 0L );
}

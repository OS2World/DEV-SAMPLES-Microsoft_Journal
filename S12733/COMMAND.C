#define INCL_DOS
#define INCL_PM

#include <os2.h>
#include <stddef.h>
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "hello.h"

/* The ALLOCED macro is used to determine whether to dynamically
allocate the menu template structure or whether to just get it from a
global variable */

#define ALLOCED 0
BOOL bHelp=TRUE;

extern HWND    hwndHelloFrame;
extern HWND    hwndInfo;

HWND	    hSubMenu, hWndSysSubMenu;
MENUCREATETEMPLATE  mt;
MENUCREATETEMPLATE far * pmt;

USHORT	    ActionBarID = 1000;
BOOL	    bPopCreated;    /* flag to only allow one popup menu at a time */

void HelloCommand( hWnd, Menu)
HWND	hWnd;
USHORT Menu;
{
char  buffer[10];
HWND hWndSysMenu,hMenu;
MENUITEM    mi,miSysMenu;
SHORT	    idSysMenu;
HBITMAP hBitmap;
HPS hpsMenu;

switch (Menu) {
    case IDM_DELPOPUP:
	/* can't delete it if it hasn't been created */

	if (bPopCreated) {
	    hMenu = WinWindowFromID(hwndHelloFrame,FID_MENU);
	    WinSendMsg(hMenu,MM_DELETEITEM, MPFROM2SHORT((USHORT) ActionBarID, (BOOL) FALSE),0L);
	    bPopCreated=FALSE;
	    }
	break;

    case IDM_NEWMENU:
	if (!bPopCreated) {

#if ALLOCED
	    SEL sel;
	    DosAllocSeg(100, &sel, 0);
	    pmt=MAKEP(sel,0);

	    pmt->size = 0;	   /* length of menu template ???????? */
	    pmt->version = 0;	   /* template version. currently 0 */
	    pmt->codepage = 850;   /* codepage used for text items within the menu
				      (but not any submenus) */
	    pmt->mnemonic = 0;	   /* set to 0 for OS/2 v1.1 */
	    pmt->itemcount = 1;    /* one new submenu item */

	    pmt->item[0].afStyle     = MIS_TEXT; /* text item */
	    pmt->item[0].afAttribute = 0;    /* no initial attributes */

                                        /* produces WM_COMMAND message with ID_SUBMENU value */
            pmt->item[0].id          = ID_FIRSTITEM;
	    strcpy(pmt->item[0].text, "New menu item");
#else
				 /* initialize popup menu template; use defaults */
	    mt.size = 0;	 /* length of menu template ???????? */
	    mt.version = 0;	 /* template version. currently 0 */
	    mt.codepage = 850;	 /* codepage used for text items within the menu
				   (but not any submenus) */
            mt.mnemonic = 0;     /* set to 0 for OS/2 v1.1 */
	    mt.itemcount = 1;	 /* one new submenu item */

	    mt.item[0].afStyle	   = MIS_TEXT; /* text item */
	    mt.item[0].afAttribute = 0;    /* no initial attributes */
                                           /* produces WM_COMMAND message with ID_FIRSTITEM value */
            mt.item[0].id          = ID_FIRSTITEM;
	    strcpy(mt.item[0].text, "New menu item");
#endif
	    /* get a handle to the windows menu */
	    hMenu = WinWindowFromID(hwndHelloFrame,FID_MENU);

	    /* create a popup menu using template information */

#if ALLOCED
	    hSubMenu = WinCreateWindow(HWND_OBJECT,WC_MENU,NULL,WS_CLIPSIBLINGS | WS_SAVEBITS,
			  0,0,0,0,
			hMenu,HWND_BOTTOM,0x3000,(PCH) pmt,0);
	    DosFreeSeg(sel);
#else
	    hSubMenu = WinCreateWindow(HWND_OBJECT,WC_MENU,NULL,WS_CLIPSIBLINGS | WS_SAVEBITS,
			  0,0,0,0,
			hMenu,HWND_BOTTOM,0x3000,(PCH)&mt,0);
#endif

	    hpsMenu=WinGetPS(hMenu);
            hBitmap=GpiLoadBitmap(hpsMenu, (HMODULE) 0, 13, 0L, 0L);
	    WinReleasePS(hpsMenu);

	    mi.iPosition = 1;		      /* append item to end */
	    mi.afStyle	 = MIS_BITMAP | MIS_SUBMENU;  /* text and menu style */
	    mi.afAttribute = 0; 		    /* no initial attributes */
	    mi.id	 = ActionBarID; 	    /* ID of action bar menu item */
	    mi.hwndSubMenu = hSubMenu;		    /* window handle of popup menu */
	    mi.hItem	 = hBitmap;		       /* handle for owner or bitmap item */

	    /* insert the new action bar item */
	    WinSendMsg(hMenu,MM_INSERTITEM,(MPARAM) &mi,NULL);

/*	    The following line adds the text to the action bar item,  not
/*	    needed when the action bar item is a bitmap
/*
/*		 (MPARAM)(PCH)itoa(ActionBarID++,buffer,10)
*/
	    bPopCreated=TRUE;
	    }

	break;

    case ID_FIRSTITEM:
	mi.iPosition = MIT_END; 	 /* append item to end */
	mi.afStyle   = MIS_TEXT;	 /* text style */
	mi.afAttribute = 0;		 /* no initial attributes */
        mi.id        = ID_NEWITEM;             /* ID of action bar menu item */
	mi.hwndSubMenu = NULL;
        mi.hItem     = 0;                /* handle for owner or bitmap item */

	WinSendMsg(hSubMenu,MM_INSERTITEM,(MPARAM)&mi,
		  (MPARAM)(PCH)"Newly added item");
	break;

    case ID_ADDTOSYSMENU:
	/* Get handle of the [-] System Menu */
	hWndSysMenu = WinWindowFromID (hwndHelloFrame, FID_SYSMENU);

	/* Get the id of the zeroth item of the system menu */
        idSysMenu = (SHORT) (LONG) WinSendMsg (hWndSysMenu,
					MM_ITEMIDFROMPOSITION, 0L, 0L);

	/* Get the MENUITEM structure for the system menu */
	WinSendMsg (hWndSysMenu,
		    MM_QUERYITEM,
		    (MPARAM)MAKEULONG (idSysMenu, FALSE),
		    (MPARAM)(PMENUITEM)&miSysMenu);

	/* Get handle of the system sub menu */
	hWndSysSubMenu = miSysMenu.hwndSubMenu;

	mi.iPosition=0;     /* add to top of system menu */
	mi.afStyle=MIS_TEXT;
	mi.afAttribute=0;
	mi.id=101;
	mi.hwndSubMenu=NULL;
        mi.hItem = 0;

	WinSendMsg(hWndSysSubMenu,
		   MM_INSERTITEM,
		   (MPARAM) &mi,
		   MPFROMP((PSZ) "Item text")
		  );
	break;

    case ID_DELSYS:
	 WinSendMsg(hWndSysSubMenu,
		    MM_DELETEITEM,
		    MPFROM2SHORT((USHORT) 101, (BOOL) FALSE),
		    0L);    /* not used, must be zero */
	 break;

    case ID_TOGGLEHELP:
        if (bHelp) {
            /* Help is on now, turn it off */
            bHelp=FALSE;
            WinSetParent(hwndInfo, HWND_OBJECT, FALSE);
            WinSendMsg(hwndHelloFrame, WM_UPDATEFRAME, FCF_MENU,0L);
            }
        else {
            /* Help is off now, turn it on */
            bHelp=TRUE;
            WinSetParent(hwndInfo, hwndHelloFrame, FALSE);
            WinSendMsg(hwndHelloFrame, WM_UPDATEFRAME, 0L,0L);
            }

        break;


    default:			 /* show id of menu item selected */
	MessageInt(hWnd,Menu,(PCH)"menuitem selected");
	break;
  }
}


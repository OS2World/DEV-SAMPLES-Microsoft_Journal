/*----------------------------------------------------------------------------------
   PresParm.C -- A Program that Demonstrates Combo Boxes and Presentation Parameters
  ---------------------------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include "PresParm.h"
#include <stdlib.h>

#define RGB(r,g,b) (((ULONG) ((ULONG)(r) << 8 | (ULONG)(g)) << 8) | (ULONG)(b))

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY GetPresParamsDlgProc (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY AboutboxDlgProc (HWND, USHORT, MPARAM, MPARAM) ;

MRESULT EXPENTRY GetPresParamsDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {

     /* Handle to List Box which is being updated */
     static HWND   hwndListBox ;

     /* used for indexing */
	    int i ;

     /* temporary storage variables  */

	    USHORT  usTempRed, usTempGreen, usTempBlue, usTempListIndex;
	    char szTempTitle [25] ;

     /* Used for storing values between changes */

     static ULONG TempPresParam[3] ;

     /* static variables to save variable values  */

     static USHORT  usRed = 0, usGreen = 0, usBlue = 0, usListIndex = 0 ;
     static char szTitle [25] = "Sample Title:";
	    LONG   clr;

     static ULONG idPresParam[3] = {PP_FOREGROUNDCOLOR,
				    PP_BACKGROUNDCOLOR,
				    PP_HILITEBACKGROUNDCOLOR} ;

     static ULONG PresParam[3] = {RGB(0,0,0),
				  RGB(255,255,255),
				  RGB(0,0,0)} ;

     switch (msg)
	  {

	  case WM_INITDLG:

	   /* Initialize static text string */

	       WinSetDlgItemText (hwnd, DID_TITLE, szTitle) ;

	  /* Initialize drop down combo box */

	       WinSetDlgItemText (hwnd, DID_EDITLIST, szTitle) ;

	       WinSendDlgItemMsg (hwnd, DID_EDITLIST, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "Demo Strings:")) ;
	       WinSendDlgItemMsg (hwnd, DID_EDITLIST, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "Test Strings:")) ;
	       WinSendDlgItemMsg (hwnd, DID_EDITLIST, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "List Strings:")) ;

	  /* Initialize drop down list box */

	       WinSendDlgItemMsg (hwnd, DID_LISTCOMBO, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "List Box Text")) ;
	       WinSendDlgItemMsg (hwnd, DID_LISTCOMBO, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "Background")) ;
	       WinSendDlgItemMsg (hwnd, DID_LISTCOMBO, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "Selection")) ;
	       WinSendDlgItemMsg (hwnd, DID_LISTCOMBO,
				  LM_SELECTITEM,
				  MPFROMSHORT ((SHORT) usListIndex),
				  MPFROMSHORT ((SHORT) TRUE) ) ;

	  /* Initialize Sample Strings */

	       WinSendDlgItemMsg (hwnd, DID_LIST, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "String 1")) ;
	       WinSendDlgItemMsg (hwnd, DID_LIST, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "String 2")) ;
	       WinSendDlgItemMsg (hwnd, DID_LIST, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "String 3")) ;
	       WinSendDlgItemMsg (hwnd, DID_LIST, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "String 4")) ;
	       WinSendDlgItemMsg (hwnd, DID_LIST, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "String 5")) ;
	       WinSendDlgItemMsg (hwnd, DID_LIST, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "String 6")) ;
	       WinSendDlgItemMsg (hwnd, DID_LIST, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "String 7")) ;
	       WinSendDlgItemMsg (hwnd, DID_LIST, LM_INSERTITEM,
				  MPFROMSHORT ((SHORT) LIT_END),
				  MPFROMP ((PSZ) "String 8")) ;

	 /* Initialize Edit Fields */

	       WinSetDlgItemShort (hwnd, DID_RED, usRed, FALSE) ;
	       WinSetDlgItemShort (hwnd, DID_GREEN, usGreen, FALSE) ;
	       WinSetDlgItemShort (hwnd, DID_BLUE, usBlue, FALSE) ;

	 /*    Set Presentation Parameters on list box with Id of DID_LIST   */

	       hwndListBox = WinWindowFromID (hwnd, DID_LIST) ;

	       for (i = 0 ; i <3 ; i++ )
		   WinSetPresParam (hwndListBox,
				   idPresParam[i],
				   (LONG) sizeof (LONG),
				   (PVOID) &PresParam[i]) ;

	  /* Update temporary storage */

	       for (i = 0 ; i <3 ; i++ )
		   TempPresParam[i] = PresParam[i] ;

	       return 0 ;


	  case WM_COMMAND:
	       switch (COMMANDMSG(&msg)->cmd)
		    {
		    case DID_UPDATE:

		    /* Update Display Text */

			 WinQueryDlgItemText (hwnd, DID_EDITLIST, 25, szTempTitle) ;
			 WinSetDlgItemText (hwnd, DID_TITLE, szTempTitle) ;

		    /* Get new color value */

			 WinQueryDlgItemShort (hwnd, DID_RED, &usTempRed, FALSE) ;
			 WinQueryDlgItemShort (hwnd, DID_GREEN, &usTempGreen, FALSE) ;
			 WinQueryDlgItemShort (hwnd, DID_BLUE, &usTempBlue, FALSE) ;

			 clr = RGB (usTempRed, usTempGreen, usTempBlue) ;

		    /* Query type of PresParam to change */

			 usTempListIndex = (USHORT) WinSendDlgItemMsg (hwnd, DID_LISTCOMBO,
						       LM_QUERYSELECTION,
						       MPFROMSHORT ((SHORT) LIT_FIRST), 0L) ;

		    /* Update PresParams */

			 WinSetPresParam (hwndListBox,
					   idPresParam[usTempListIndex],
					   (LONG) sizeof (LONG),
					   (PVOID) &clr ) ;

		    /* Save value between Update messages */

			 TempPresParam[usTempListIndex] = clr ;

		    /* Return to the system */

			 return 0 ;

		    case DID_OK:

		    /* Make final display and variable updates */

			 WinQueryDlgItemText (hwnd, DID_EDITLIST, 25, szTitle) ;
			 WinSetDlgItemText (hwnd, DID_TITLE, szTitle) ;


			 WinQueryDlgItemShort (hwnd, DID_RED, &usRed, FALSE) ;
			 WinQueryDlgItemShort (hwnd, DID_GREEN, &usGreen, FALSE) ;
			 WinQueryDlgItemShort (hwnd, DID_BLUE, &usBlue, FALSE) ;


			 usListIndex = (USHORT) WinSendDlgItemMsg (hwnd, DID_LISTCOMBO,
						      LM_QUERYSELECTION,
						      MPFROMSHORT ((SHORT) LIT_FIRST), 0L) ;

			 clr = RGB (usRed, usGreen, usBlue) ;

			 WinSetPresParam (hwndListBox,
					   idPresParam[usListIndex],
					   (LONG) sizeof (LONG),
					   (PVOID) &clr ) ;

			 TempPresParam[usListIndex] = clr ;

			 for (i = 0 ; i <3 ; i++ )
			     PresParam[i] = TempPresParam[i] ;

			 WinDismissDlg (hwnd, TRUE) ;
			 return 0 ;

		    case DID_CANCEL:

			 WinDismissDlg (hwnd, FALSE) ;
			 return 0 ;
		    }
	       break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }



MRESULT EXPENTRY AboutboxDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {

     switch (msg)
	  {

	  case WM_INITDLG:

	       return 0 ;


	  case WM_COMMAND:
	       switch (COMMANDMSG(&msg)->cmd)
		    {

		    case DID_OK:

			 WinDismissDlg (hwnd, FALSE) ;
			 return 0 ;
		    }
	       break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }


MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     HPS hps ;
     RECTL rcl ;

     switch (msg)
	  {

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
		    case IDM_PLAY:
			 WinDlgBox (HWND_DESKTOP, hwnd, GetPresParamsDlgProc,
				    NULL, ID_RESOURCE, NULL) ;
			 return 0 ;

		    case IDM_ABOUT:
			 WinDlgBox (HWND_DESKTOP, hwnd, AboutboxDlgProc,
				    NULL, ID_ABOUT, NULL) ;

			 return 0 ;


		    case IDM_QUIT:
			 WinSendMsg (hwnd, WM_CLOSE, 0L, 0L) ;
			 return 0 ;

		    }
	       break ;

	  case WM_PAINT:
	       hps = WinBeginPaint (hwnd, NULL, &rcl) ;
	       WinFillRect (hps, &rcl , CLR_WHITE) ;
	       WinEndPaint (hps) ;
	       return 0 ;

          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }





int main (void)
     {
     static CHAR  szClientClass [] = "PresParm" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
				 FCF_SHELLPOSITION | FCF_TASKLIST |
				 FCF_MENU ;

       HAB	    hab ;
       HMQ	    hmq ;
       HWND	    hwndClient, hwndFrame ;
       QMSG	    qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (
                    hab,                // Anchor block handle
                    szClientClass,      // Name of class being registered
                    ClientWndProc,      // Window procedure for class
                    CS_SIZEREDRAW,      // Class style
                    0) ;                // Extra bytes to reserve

     hwndFrame = WinCreateStdWindow (
                    HWND_DESKTOP,       // Parent window handle
                    WS_VISIBLE,         // Style of frame window
                    &flFrameFlags,      // Pointer to control data
                    szClientClass,      // Client window class name
                    NULL,               // Title bar text
                    0L,                 // Style of client window
                    NULL,               // Module handle for resources
		    ID_RESOURCE,	// ID of resources
                    &hwndClient) ;      // Pointer to client window handle

     WinSendMsg (hwndFrame, WM_SETICON,
                 WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                 NULL) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

/**************************************************************************\
 *                                                                        *
 * Base.c                                                                 *
 *                                                                        *
 * This application does the following:                                   *
 *                                                                        *
 *      1. Changes the system menu icon to the child system icon.         *
 *                                                                        *
 *      2. Changes the system menu item's mnemonics to use CTRL+ key      *
 *         combinations rather than the usual ALT+ key combinations.      *
 *                                                                        *
 * Author: Petrus Wong                                                    *
 *                                                                        *
 * History: created 7-10-90                                               *
 *                                                                        *
 **************************************************************************/
#define INCL_WIN
#include <os2.h>
#include "base.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "Base" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST|
                                 FCF_ACCELTABLE ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient, hwndSysMenu ;
     QMSG         qmsg ;
     HBITMAP      hbmChildSysMenu ;
     ULONG        flStyle ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (
                    hab,                // Anchor block handle.
                    szClientClass,      // Name of class being registered.
                    ClientWndProc,      // Window procedure for class.
                    CS_SIZEREDRAW,      // Class style.
                    0) ;                // Extra bytes to reserve.

     hwndFrame = WinCreateStdWindow (
                    HWND_DESKTOP,       // Parent window handle.
                    0L,                 // Style of frame window.
                    &flFrameFlags,      // Pointer to control data.
                    szClientClass,      // Client window class name.
                    NULL,               // Title bar text.
                    0L,                 // Style of client window.
                    NULL,               // Module handle for resources.
                    IDR_DOC,            // ID of resources.
                    &hwndClient) ;      // Pointer to client window handle.

     WinSendMsg (hwndFrame, WM_SETICON,
                 WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                 NULL) ;

    /*
     * Get the child system bitmap and then
     * load in the document window's system menu.
     */
     hbmChildSysMenu = WinGetSysBitmap(HWND_DESKTOP, SBMP_CHILDSYSMENU);
     hwndSysMenu = WinLoadMenu(hwndFrame, (HMODULE)NULL, IDM_DOCSYSMENU);

    /*
     * Make it look like a normal system menu to the frame manager so
     * that it gets formatted correctly.
     */
     flStyle = WinQueryWindowULong(hwndSysMenu, QWL_STYLE);
     WinSetWindowULong(hwndSysMenu, QWL_STYLE, flStyle | MS_TITLEBUTTON);
     WinSetWindowUShort(hwndSysMenu, QWS_ID, FID_SYSMENU);

    /*
     * Set the bitmap to the SBMP_CHILDSYSMENU bitmap.
     */
     WinSendMsg(hwndSysMenu, MM_SETITEMHANDLE, (MPARAM)SC_DOCSYSMENU,
            (MPARAM)hbmChildSysMenu);

     WinShowWindow(hwndFrame, TRUE);

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR szText [] = "Note the system menu icon and its mnemonics have changed" ;
     HPS         hps;
     RECTL       rcl ;

     switch (msg)
	  {
          case WM_CREATE:
               DosBeep (261, 100) ;
               DosBeep (330, 100) ;
               DosBeep (392, 100) ;
               DosBeep (523, 500) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               WinQueryWindowRect (hwnd, &rcl) ;

               WinDrawText (hps, -1, szText, &rcl, CLR_NEUTRAL, CLR_BACKGROUND,
                            DT_CENTER | DT_VCENTER | DT_ERASERECT) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               DosBeep (523, 100) ;
               DosBeep (392, 100) ;
               DosBeep (330, 100) ;
               DosBeep (261, 500) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

/**************************************************************************\
 *                                                                        *
 * Base.c                                                                 *
 *                                                                        *
 * This application does the following:                                   *
 *                                                                        *
 *      1. Puts up the original dialog box without resizing and           *
 *         repositioning it when it is called the first time.             *
 *                                                                        *   
 *         It also puts up a full screen dialog box the second time it    *
 *         is called and thereafter.                                      * 
 *                                                                        *
 *      2. Allocates heap for the list box control in the dialog box the  *
 *          first time it is called and reuse it for subsequent calls.    *
 *                                                                        *
 * Author: Petrus Wong                                                    *
 *                                                                        *
 * History: created 8-16-90                                               *
 *                                                                        *
 **************************************************************************/
#define INCL_WIN
#define INCL_GPI

#define INCL_SUB
#define INCL_DOS
#define INCL_BASE
#define INCL_DOSERRORS


#include <os2.h>
#include "base.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY DlgProc (HWND, USHORT, MPARAM, MPARAM);

SEL   sel;
HHEAP hHeap;
HWND  hwndFrame, hwndClient;
HWND hwndList ;
static BOOL  fFirstCall = TRUE;         // Flags if the dialog box is 
                                        // called the first time.

int main (void)
     {
     static CHAR  szClientClass [] = "Base" ;
     static ULONG flFrameFlags = FCF_SIZEBORDER | FCF_MENU | FCF_SYSMENU |
                                 FCF_TASKLIST | FCF_TITLEBAR |
                                 FCF_MINMAX | FCF_SHELLPOSITION;

     HAB          hab ;
     HMQ          hmq ;
     QMSG         qmsg ;


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
                    WS_VISIBLE,         // Style of frame window.
                    &flFrameFlags,      // Pointer to control data.
                    szClientClass,      // Client window class name.
                    NULL,               // Title bar text.
                    0L,                 // Style of client window.
                    NULL,               // Module handle for resources.
                    2,                  // ID of resources.
                    &hwndClient) ;      // Pointer to client window handle.


     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow(hwndList) ;
     WinDestroyHeap(hHeap) ;
     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     HPS          hps;
     RECTL        rect;

     switch (msg)
          {
          case WM_CREATE: {
               register HWND   hwndDlg;
               register USHORT wResult;

               /* Puts up the original dialog box for the first time. */
               /*      No resizing and repositioning.                 */
               if (!(hwndDlg = WinLoadDlg( HWND_DESKTOP, hwnd,
                                           DlgProc, NULL, ID_DIALOG, NULL)))
                  {
                   /* Cannot create the dialog box, sound the alarm.  */
                   DosBeep(375, 1000);
                   DosBeep(375, 100);
                   DosBeep(375, 1000);
                  }
               wResult = WinProcessDlg(hwndDlg) ;

               if (hwndDlg != NULL)
                  WinDestroyWindow(hwndDlg) ;

               fFirstCall = FALSE ;              // Reset the flag to signal
                                                 //  it has been called the
                                                 //  first time.

               break ;
               }

          case WM_PAINT:

               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               WinQueryWindowRect (hwnd, &rect);
               WinFillRect (hps, &rect, CLR_WHITE);
               WinEndPaint (hps) ;
               return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
               {
               case IDM_BEEPHIGH: {
                    register HWND   hwndDlg;
                    register USHORT wResult;

                    if (!(hwndDlg = WinLoadDlg( HWND_DESKTOP, hwnd,
                                           DlgProc, NULL, ID_DIALOG, NULL)))
                      {
                        /* Cannot create the dialog box, sound the alarm.  */
                        DosBeep(375, 1000);
                        DosBeep(375, 100);
                        DosBeep(375, 1000);
                      }
                    wResult = WinProcessDlg(hwndDlg) ;

                    if (hwndDlg != NULL)
                      WinDestroyWindow(hwndDlg) ;

                    return (wResult) ;
                    }

               case IDM_BEEPLOW:
                      DosBeep (400, 175);
                      return 0;
               }
               break;

          case WM_DESTROY:
             /*  DosExit (EXIT_PROCESS, 0); */
               return 0;

          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY DlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static USHORT flag = 0;
     HWND flaghwnd;

     switch (msg)
          {
          case WM_INITDLG: {
               SWP swp;
               LONG cx, cy ;

               if (!fFirstCall)     // Puts up a full screen sized dialog
                                    // box.
                     {
                        /* Finds out the size and position of the list box.
                         */
                        WinQueryWindowPos(WinWindowFromID (hwnd, IDD_LIST),
                                          &swp);

                        /* Finds out the size of the screen. */
                        cx = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
                        cy = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);

                        /* We want to reuse the old list box so trash this
                           one.                                            */
                        WinDestroyWindow (WinWindowFromID (hwnd, IDD_LIST));

                        /* Revive the old list box.                        */
                        WinSetOwner(hwndList, hwnd);
                        WinSetParent(hwndList, hwnd, FALSE);

                        /* Position and resize the dialog box to that of
                           the screen.
                         */
                        WinSetWindowPos(hwnd, HWND_TOP,
                                        0, 0, cx, cy,
                                        SWP_SIZE | SWP_MOVE | SWP_SHOW );

                        /* Position the list box item. */
                        WinSetWindowPos(hwndList, HWND_TOP,
                                        swp.x, swp.y, swp.cx, swp.cy,
                                        SWP_MOVE | SWP_SHOW );

                        WinSendMsg(WinWindowFromID(hwnd, IDD_LIST),
                               LM_INSERTITEM,
                               MPFROMSHORT ((SHORT) LIT_END),
                               MPFROMP ((PSZ) "DEF"));

                      }
                  else {            // Shows the original sized dialog box.
                    WinSendMsg(WinWindowFromID(hwnd, IDD_LIST),
                               LM_INSERTITEM,
                               MPFROMSHORT ((SHORT) LIT_END),
                               MPFROMP ((PSZ) "ABC"));
                       }
               return 0 ;
               }

          case WM_COMMAND:
               switch (SHORT1FROMMP (mp1))
               {
               case IDD_OK:

                      hwndList = WinWindowFromID (hwnd, IDD_LIST);

                      /* Save the list box for future use, then destroy
                         the dialog box.                                */
                      WinSetParent(hwndList, HWND_OBJECT, FALSE) ;
                      WinSetOwner(hwndList, NULL);
                      WinDismissDlg(hwnd, TRUE);
                      fFirstCall = FALSE ;
                      return 0;
               }
               break ;

          case WM_CONTROLHEAP:
              /* Take care of the heap allocation for the list box 
                  ourselves. */
              /* The same heap is reused, ie. only created the first time. */

              flaghwnd = HWNDFROMMP (mp2);
              if ((flaghwnd == WinWindowFromID(hwnd, IDD_LIST)) &&
                  (fFirstCall))
              {
               if (!DosAllocSeg ((60 * 1024), &sel, 0))
               {
                  hHeap = WinCreateHeap ( sel, (40 * 1024),
                                                0, 0, 128, 0);

                  if (hHeap)
                  {
                     return (hHeap);
                  }
               }
               break;
              }
              break;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }

/**************************************************************************\
 *                                                                        *
 * Base.c                                                                 *
 *                                                                        *
 * This application does the following:                                   *
 *                                                                        *   
 *   Creates a "kid" frame inside the parent frame, and draws a bitmap    *
 *   in the client area of both parent and "kid".                         *
 *                                                                        *
 *   It serves to demonstrates that by taking care of the WM_PAINT        *
 *   message in both the client areas of "kid" and parent, the clients    *
 *   will be redrawn correctly when repainting is required.               *
 *                                                                        *
 *                                                                        *
 * Author: Petrus Wong                                                    *
 *                                                                        *
 * History: created 8-20-90                                               *
 *                                                                        *
 **************************************************************************/
#define INCL_WIN
#include <os2.h>
#include "base.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY ChildWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "Base" ;
     static CHAR  szChildClass [] = "Kid" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient, hwndChildFrame, hwndChildClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;
     WinRegisterClass (hab, szChildClass, ChildWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
                                     0L, NULL, 0, &hwndClient) ;
     flFrameFlags &= ~FCF_SHELLPOSITION ;
     hwndChildFrame = WinCreateStdWindow (hwndClient, 0L,
                                     &flFrameFlags, szChildClass, "Kid",
                                     0L, NULL, 0, &hwndChildClient) ;
     WinSetWindowPos(hwndChildFrame, HWND_TOP, 0, 0, 300, 200,
                     SWP_SHOW | SWP_SIZE | SWP_MOVE );

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ChildWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static SHORT cxClient, cyClient ;
     HBITMAP      hbm ;
     HPS          hps ;
     POINTL       ptl ;

     switch (msg)
          {
          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_PAINT:

               /* By taking care of the drawing in here, the bitmap or 
                  the client will be redrawn when it receives this 
                  message.  */
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               hbm = GpiLoadBitmap (hps, NULL, IDB_HELLO,
                                    (LONG) cxClient, (LONG) cyClient) ;
               if (hbm)
                    {
                    ptl.x = 0 ;
                    ptl.y = 0 ;

                    WinDrawBitmap (hps, hbm, NULL, &ptl,
                                   CLR_NEUTRAL, CLR_BACKGROUND, DBM_NORMAL) ;

                    GpiDeleteBitmap (hbm) ;               
                    }
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static SHORT cxClient, cyClient ;
     HBITMAP      hbm ;
     HPS          hps ;
     POINTL       ptl ;

     switch (msg)
          {
          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_PAINT:

               /* By taking care of the drawing in here, the bitmap or 
                  the client will be redrawn when it receives this 
                  message.    */
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               hbm = GpiLoadBitmap (hps, NULL, IDB_HELLO,
                                    (LONG) cxClient, (LONG) cyClient) ;
               if (hbm)
                    {
                    ptl.x = 0 ;
                    ptl.y = 0 ;

                    WinDrawBitmap (hps, hbm, NULL, &ptl,
                                   CLR_NEUTRAL, CLR_BACKGROUND, DBM_NORMAL) ;

                    GpiDeleteBitmap (hbm) ;               
                    }
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

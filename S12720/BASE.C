/*  base code for PM applications  */

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "base.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY AboutDlgProc  (HWND, USHORT, MPARAM, MPARAM);

HWND   hwndFrame, hwndClient;

int main (void)
   {
   static CHAR  szClientClass [] = "base";
   static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU    |
                               FCF_SIZEBORDER    | FCF_MINMAX     |
                               FCF_SHELLPOSITION | FCF_TASKLIST   |
                               FCF_MENU          | FCF_ACCELTABLE |
                               FCF_ICON ;
   HAB    hab;
   HMQ    hmq;

   QMSG   qmsg;

   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, 0);

   WinRegisterClass (
                 hab,             // Anchor Block handle
                 szClientClass,   // Name of class being registered
                 ClientWndProc,   // Window procedure for class
                 CS_SIZEREDRAW,   // Class style
                 0);              // Extra bytes to reserve

   hwndFrame = WinCreateStdWindow (
                 HWND_DESKTOP,    // Parent of Window handle
                 WS_VISIBLE,      // Style of frame window
                 &flFrameFlags,   // Pointer to control data
                 szClientClass,   // Client window class name
                 NULL,            // Title bar text
                 0L,              // Style of Client Window
                 (HMODULE) NULL,  // Module of handle for resources
                 ID_RESOURCE,     // ID of resources
                 &hwndClient);    // Pointer to client window handle

   WinSetWindowText (hwndFrame, "Base Code for PM Apps");


   while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
      WinDispatchMsg (hab, &qmsg);

   WinDestroyWindow (hwndFrame);
   WinDestroyMsgQueue (hmq);
   WinTerminate (hab);
   return 0;
   }

/********************** ClientWndProc **************************/

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
   {
   HPS    hps;
   RECTL  rcl;
   HWND   hwndMinMax;

   switch (msg)
     {
     case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
           {
           case IDM_MENU1:

             /*
              *  Get window handle of minmax buttons, and destroy it.
              */

             hwndMinMax = WinWindowFromID (hwndFrame, FID_MINMAX);

			 if ( hwndMinMax == NULL )
				break;

             WinDestroyWindow (hwndMinMax);

             /*
              *  need to send update frame message to update the internal
              *  swp structures for the frame.
              */

             WinSendMsg (hwndFrame, WM_UPDATEFRAME, MPFROM2SHORT(FCF_MINMAX, 0), 0L);
             break;

           case IDM_ABOUT:
              WinDlgBox(HWND_DESKTOP, hwnd, AboutDlgProc, (HMODULE) NULL, IDD_ABOUT, NULL);
              break;
           }
        break;
     case WM_PAINT:
        hps = WinBeginPaint (hwnd, NULL, NULL);
        GpiErase (hps);
        WinQueryWindowRect (hwnd, &rcl);
        WinEndPaint (hps);
        return 0;

     case WM_HELP:
        WinMessageBox (HWND_DESKTOP, hwnd, "Help Message Box", "PM Base Code",
                       0, MB_OK | MB_ICONASTERISK );
        break;
     case WM_DESTROY:
        return 0;
     }
   return WinDefWindowProc (hwnd, msg, mp1, mp2);
   }

MRESULT EXPENTRY AboutDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
   {

   switch (msg)
     {
     case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd)
           {
           case DID_OK:
           case DID_CANCEL:
              WinDismissDlg (hwnd, TRUE);
              break;

           }
        break;

     default:
        return (WinDefDlgProc (hwnd, msg, mp1, mp2));

     }
   }

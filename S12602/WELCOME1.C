#define INCL_BASE
#define INCL_PM

#include <os2.h>
#include "welcome1.h"
#include <string.h>
#include <stdio.h>

// #define DEBUG

#ifdef DEBUG

/* For details on using additional debugging output routines for PM
   programs see KnowledgeBase article q43027 and softlib file pmutils.arc

   If you define DEBUG, you will need to add utils.lib to the LINK line in
   the MAKEFILE.
*/

#include "\pmapps\utils\utils.h"
#endif

#define SZREQUESTDATA "data from WM_DDE_REQUEST"

BOOL DisplayabData (HWND , PDDESTRUCT) ;
void DisplayusFormat (HWND, USHORT) ;
void DisplayDdeMsgType (HWND, USHORT) ;
void DisplaywStatus (HWND, USHORT) ;

PDDESTRUCT MakeDDESegment (HWND, PSZ, USHORT, USHORT, PVOID, USHORT) ;
MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

BOOL fQuiet=FALSE;  /* flag to turn off message boxes */

HWND hwndServer ;
HWND hwndClient ;
BOOL fInitSucceed;
HWND hwndWelcomeClient;
HWND hwndFrame;

char ThreadStack[4096];

int main (void)
     {
     static CHAR  szClientClass [] = "Welcome1" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
				 FCF_SHELLPOSITION | FCF_TASKLIST |
				 FCF_MENU ;
     HAB          hab ;
     HMQ          hmq ;
     QMSG         qmsg ;

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
                    0,                 // Module handle for resources
		    ID_RESOURCE,	// ID of resources
                    &hwndWelcomeClient) ;      // Pointer to client window handle

     WinSendMsg (hwndFrame, WM_SETICON,
                 WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                 0L) ;

     while (WinGetMsg (hab, &qmsg, 0L, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }


MRESULT EXPENTRY GetValueDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {

     static HWND   hwndButton ;
     SHORT	   sLen ;
     static char   szBuffer [SZBUF_SIZE] = {0L} ;
     static CHAR FAR * szString ;

     switch (msg)
	  {
	  case WM_INITDLG:
	       WinAlarm (HWND_DESKTOP, WA_NOTE) ;

	       szString = PVOIDFROMMP (mp2) ;

               WinSendDlgItemMsg(hwnd,
                                 IDD_CRITEREA,
                                 EM_SETTEXTLIMIT,
                                 MPFROMSHORT( (SHORT) SZBUF_SIZE),
                                 0L);

	       WinSetDlgItemText (hwnd, IDD_TEXT, szString) ;

	       hwndButton = WinWindowFromID (hwnd, IDD_CRITEREA) ;
	       WinSetDlgItemText (hwnd, IDD_CRITEREA, "") ;

	       return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
		    case IDD_OK:

			 sLen = WinQueryWindowTextLength (hwndButton) + 1 ;
			 WinQueryWindowText (hwndButton, sLen, szString) ;
			 WinDismissDlg (hwnd, TRUE) ;
			 return 0 ;

		    case IDD_CANCEL:

			 WinDismissDlg (hwnd, FALSE) ;
                         return 0 ;
                    }
               break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }


PDDESTRUCT MakeDDESegment (hwndDest, pszItemName, fsStatus, usFormat, pabData, usDataLen )
HWND     hwndDest ;
PSZ      pszItemName ;
USHORT   fsStatus ;
USHORT   usFormat ;
PVOID    pabData ;
USHORT   usDataLen ;

{
PDDESTRUCT pddes ;
USHORT     usItemLen ;
SEL        selBuf ;

    if (pszItemName != 0L)
       usItemLen = strlen (pszItemName) + 1 ;
    else
       usItemLen = 0 ;

    if (! DosAllocSeg (sizeof (DDESTRUCT) + usItemLen + usDataLen,
                       &selBuf, SEG_GIVEABLE))
        {
        pddes = SELTOPDDES (selBuf) ;
        pddes -> cbData = usDataLen - 1; /* subtract 1 to make it work ??????? */
        pddes -> fsStatus = fsStatus ;
        pddes -> usFormat = usFormat ;
        pddes -> offszItemName = sizeof (DDESTRUCT) ;

        if ((usDataLen) && (pabData))
            pddes->offabData = sizeof (DDESTRUCT) + usItemLen ;
        else
            pddes -> offabData = sizeof (DDESTRUCT) ;

        if (pszItemName != NULL)
            strcpy (DDES_PSZITEMNAME (pddes) , pszItemName) ;

        memcpy (DDES_PABDATA (pddes), pabData, usDataLen) ;
        }

    return (pddes) ;
}


/*  ===================    Spawn Excel thread  ======================== */

/*  This thread spawns a copy of Excel using DosStartSession and then
    waits until Excel adds itself to the task list before initiating a DDE
    conversation with Excel.  Its kind of a hack in the way it determines
    that Excel is ready for DDE since Excel doesn't provide a more formal
    interface to let other apps know when it is ready

    =================================================================== */


VOID SpawnExcel(VOID)
{
STARTDATA stdata;
USHORT idSession;
USHORT pid;
PDDESTRUCT pddes;
SEL sel;
PSWBLOCK pswblk;
USHORT cbItems, cbBuf,us;
BOOL found;

HAB hab;
HMQ hmq;

hab=WinInitialize(0);
hmq=WinCreateMsgQueue(hab, 0);

DosBeep(440,500);

stdata.Length = sizeof(stdata);
stdata.Related = FALSE;   /* unrelated session */
stdata.FgBg = FALSE;
stdata.TraceOpt = 0;

stdata.PgmTitle = "John's Excel"; /* don't put 'Microsoft Excel' here or
                                     you'll not be able to tell when the
                                     real one is started.  */

stdata.PgmName = "excel.exe";
stdata.PgmInputs = NULL;
stdata.TermQ = 0;
stdata.Environment = 0;
stdata.InheritOpt = 0;
stdata.SessionType = 3;  /* PM api */
stdata.IconFile = NULL;
stdata.PgmHandle = 0;
stdata.PgmControl = 32768;
stdata.InitXPos = 0;
stdata.InitYPos = 0;
stdata.InitXSize = 0;
stdata.InitYSize = 0;

DosStartSession(&stdata, &idSession, &pid);

#ifdef DEBUG
    Debug("Excel spawned");
#endif

found = FALSE;

/* HACK ALERT.  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

/* The following hack is needed since Excel can't handle it if we just
try to call WinDdeInitiate() repeatedly until we get a
WM_DDE_INITIATEACK message.  Instead, we enumerate the switch list and
wait for the name 'Microsoft Excel' to appear, then we *assume* that
Excel is ready so we cross our fingers and try the WinDdeInitiate.
This works under OS/2 1.2 with Excel 2.2a dated March 9,1990 and it is
probably OK with other versions of OS/2 and Excel but since it is a hack,
things might change in the future but since both Excel and 1.2 are
released this should be OK for quite a while.

-John Bartlow 26-Apr-1990
*/


while (!found)
    {

    /* It would be best to move the DosAllocSeg/FreeSeg out of this
        while() loop, but then how would you know how much memory to
        allocate since it changes as apps come to life ?????
    */

    cbItems = WinQuerySwitchList(hab, NULL, 0);   /* gets num. of items */
    cbBuf = (cbItems * sizeof(SWENTRY)) + sizeof(HSWITCH);
    DosAllocSeg(cbBuf, &sel, SEG_NONSHARED);      /* allocates buffer   */
    pswblk = (PSWBLOCK) MAKEP(sel, 0);

    WinQuerySwitchList(hab, pswblk, cbBuf);       /* gets struct. array */

    us=0;

    for (us=0; us < cbItems; us++)
        {
#ifdef DEBUG
        Debug("Task title = %s",pswblk->aswentry[us].swctl.szSwtitle);
#endif

        if (!strcmp( pswblk->aswentry[us].swctl.szSwtitle,"Microsoft Excel"))
            break;
        }

    if ( ( us < cbItems) &&
         ( !strcmp( pswblk->aswentry[us].swctl.szSwtitle,"Microsoft Excel")) )
        {
        found=TRUE;
        break;
        }
    else
        {
        DosSleep(1000L);   /* give Excel some more time */
        }

    DosFreeSeg(sel);
    }

/* End HACK ALERT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */

/* We hope its OK to try the DdeInitiate now */

WinDdeInitiate (hwndWelcomeClient, "Excel", "System");

pddes = MakeDDESegment (hwndServer, 0, 0,
                        DDEFMT_TEXT, "[NEW()]", strlen ("[NEW()]") + 1) ;

WinDdePostMsg (hwndServer, hwndWelcomeClient, WM_DDE_EXECUTE, pddes, TRUE) ;
DosFreeSeg (PDDESTOSEL(pddes)) ;
}


MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
    {
    static CHAR szString  [128] ;
    static CHAR szString1 [128] ;
    static CHAR szString2 [128] ;
    static CHAR szData    [128] ;
    static CHAR szText [] = "Welcome" ;

    PDDESTRUCT pddes ;
    PDDEINIT   pddei ;

    HPS         hps;
    RECTL       rcl ;

    switch (msg)
        {
        case WM_DDE_INITIATEACK:
            hwndServer = HWNDFROMMP (mp1) ;

            fInitSucceed=TRUE;   /* we got an INITIATEACK from Excel */

            DosBeep(800,1000);
            DosBeep(400,1000);
            DosBeep(200,1000);

            DisplayDdeMsgType (hwnd, msg) ;
            WinInvalidateRect (hwnd, 0L, FALSE) ;
            return (0) ;

        case WM_DDE_TERMINATE:
            DisplayDdeMsgType (hwnd, msg) ;
            if (hwndServer)
               WinDdePostMsg (hwndServer, hwnd, WM_DDE_TERMINATE, 0L, TRUE) ;
            if (hwndClient)
               WinDdePostMsg (hwndClient, hwnd, WM_DDE_TERMINATE, 0L, TRUE) ;
            return (0) ;

        case WM_DDE_ACK:
            pddes = (PDDESTRUCT) LONGFROMMP (mp2) ;
            DisplayDdeMsgType (hwnd, msg) ;
            DisplaywStatus (hwnd, pddes->fsStatus) ;
            DosFreeSeg (PDDESTOSEL (pddes)) ;
            return (0) ;

        case WM_DDE_DATA:
            pddes = (PDDESTRUCT) LONGFROMMP (mp2) ;
            DisplayDdeMsgType (hwnd, msg) ;
            DisplaywStatus (hwnd, pddes->fsStatus) ;
            DisplayusFormat (hwnd, pddes->usFormat) ;
            DisplayabData (hwnd, pddes) ;
            DosFreeSeg (PDDESTOSEL (pddes)) ;
            pddes = MakeDDESegment (hwndServer, szString, DDE_FACK, 0, NULL, 0) ;
            WinDdePostMsg (hwndServer, hwnd, WM_DDE_ACK, pddes, TRUE) ;
            return (0) ;

        case WM_DDE_INITIATE:

             pddei = (PDDEINIT) LONGFROMMP (mp2) ;

             if ( !strcmp(pddei->pszAppName,"Welcome1"))
             {
                if (!strcmp(pddei->pszTopic,"System"))
                {
                   WinDdeRespond ((HWND) (LONGFROMMP (mp1)), hwnd,
                                  pddei->pszAppName, pddei->pszTopic) ;

                   hwndClient = HWNDFROMMP (mp1) ;
                   WinInvalidateRect (hwnd, NULL, FALSE) ;

                }
             }
             DisplayDdeMsgType (hwnd, msg) ;

             if (!fQuiet)
                {
                WinMessageBox(HWND_DESKTOP,
                             hwnd,
                             pddei->pszAppName,
                             pddei->pszTopic,
                             0,
                             MB_OK);
                }
             DosFreeSeg (PDDEITOSEL(pddei)) ;

             break ;

        case WM_DDE_EXECUTE:
            pddes = (PDDESTRUCT) LONGFROMMP (mp2) ;
            DisplayDdeMsgType (hwnd, msg) ;
            if ( (ULONG) pddes->offabData - (ULONG) pddes->offszItemName < pddes->cbData)
                DisplayabData (hwnd, pddes) ;

            strcpy (szString, (PSZ) ( (LONG) pddes + pddes->offabData)) ;
            DosFreeSeg (PDDESTOSEL (pddes)) ;

            pddes = MakeDDESegment ((HWND) LONGFROMMP (mp1), 0L,
                                     DDE_FACK,  DDEFMT_TEXT,
                                     szString, strlen (szString) + 1) ;
            if (pddes)
                {
                WinDdePostMsg ((HWND) mp1, hwnd, WM_DDE_ACK,
                              pddes,
                              TRUE);
#ifdef DEBUG
                Debug("Posted WM_DDE_ACK message");
#endif
                }
            return(TRUE);

            break ;

        case WM_DDE_REQUEST:
            DisplayDdeMsgType (hwnd, msg) ;

            pddes = (PDDESTRUCT) LONGFROMMP (mp2) ;

            strcpy (szString, (PSZ) ( (LONG) pddes + pddes->offszItemName)) ;

            if (pddes->usFormat == DDEFMT_TEXT)
                {
                pddes = MakeDDESegment(hwndClient,
                                       szString,
                                       DDE_FRESPONSE,
                                       DDEFMT_TEXT,
                                       SZREQUESTDATA,
                                       strlen (SZREQUESTDATA) + 1);

                WinDdePostMsg (hwndClient, hwnd, WM_DDE_DATA, pddes, TRUE) ;

                DosFreeSeg (PDDESTOSEL (pddes)) ;
                }
            else
                {
                /* We don't support this format, so respond with a negative ACK */

                pddes = MakeDDESegment(hwndClient,
                                       szString,
                                       0,           /* negative ACK */
                                       0,
                                       "",
                                       strlen ("") + 1);

                WinDdePostMsg (hwndClient, hwnd, WM_DDE_ACK, pddes, TRUE) ;

                }

            break ;

        case WM_DDE_ADVISE:
        case WM_DDE_UNADVISE:
        case WM_DDE_POKE:

        case WM_DDE_LAST:
            DisplayDdeMsgType (hwnd, msg) ;
            break ;

        case WM_COMMAND:
            switch (COMMANDMSG(&msg)->cmd)
                {
                case IDM_QUIET:
                    if (!fQuiet)
                        {
                        WinSendMsg(WinWindowFromID( hwndFrame, FID_MENU),
                                   MM_SETITEMATTR,
                                   MPFROM2SHORT((USHORT) IDM_QUIET, (BOOL) TRUE),
                                   MPFROM2SHORT((USHORT) MIA_CHECKED, (USHORT) MIA_CHECKED));
                        fQuiet=TRUE;
                        }
                    else
                        {
                        WinSendMsg(WinWindowFromID( hwndFrame, FID_MENU),
                                   MM_SETITEMATTR,
                                   MPFROM2SHORT((USHORT) IDM_QUIET, (BOOL) TRUE),
                                   MPFROM2SHORT((USHORT) MIA_CHECKED, (USHORT) 0));
                        fQuiet=FALSE;
                        }
                    break;

                case IDM_EXCEL:
                    {
                    TID tidThread;

                    DosCreateThread(SpawnExcel, &tidThread, &ThreadStack[4095]);
                    }
                    break;

                case IDM_CREATE:
                    strcpy (szString1,"App Name?") ;
                    if (!WinDlgBox (HWND_DESKTOP, hwnd, GetValueDlgProc,
                               (HMODULE) 0, DID_GETVALUE, (PVOID) szString1))
                         return(0);

                    strcpy (szString2,"Topic Name?") ;
                    if (!WinDlgBox (HWND_DESKTOP, hwnd, GetValueDlgProc,
                               (HMODULE) 0, DID_GETVALUE, (PVOID) szString2))
                         return(0);


                    WinDdeInitiate (hwnd, szString1, szString2) ;
                    return 0 ;

                case IDM_RUN:
                    strcpy (szData,"Execute String?") ;
                    if (!WinDlgBox (HWND_DESKTOP, hwnd, GetValueDlgProc,
                               (HMODULE) 0, DID_GETVALUE, (PVOID) szData))
                         return(0);

                    pddes = MakeDDESegment (hwndServer, 0L, 0,
                                            DDEFMT_TEXT, szData, strlen (szData) + 1) ;
                    WinDdePostMsg (hwndServer, hwnd, WM_DDE_EXECUTE, pddes, TRUE) ;
                    return 0 ;

                case IDM_NUMBER:
                    strcpy (szString,"Data Location?") ;
                    WinDlgBox (HWND_DESKTOP, hwnd, GetValueDlgProc,
                               0, DID_GETVALUE, (PVOID) szString) ;
                    pddes = MakeDDESegment (hwndServer,
                                            szString,
                                             0,  DDEFMT_TEXT, szString, strlen (szString) + 1) ;
                    WinDdePostMsg (hwndServer, hwnd, WM_DDE_REQUEST, pddes, TRUE) ;
                    return 0 ;

                case IDM_REPLY:
                case IDM_COMPOSE:
                    strcpy (szString,"Data Location?") ;
                    WinDlgBox (HWND_DESKTOP, hwnd, GetValueDlgProc,
                              (HMODULE) 0, DID_GETVALUE, (PVOID) szString) ;

                    strcpy (szData,"Data?") ;
                    WinDlgBox (HWND_DESKTOP, hwnd, GetValueDlgProc,
                              (HMODULE) 0, DID_GETVALUE, (PVOID) szData) ;

                    pddes = MakeDDESegment (hwndServer,
                                            szString,
                                            0,  DDEFMT_TEXT,
                                            szData, strlen (szData)+1);

                    WinDdePostMsg (hwndServer, hwnd, WM_DDE_POKE, pddes, TRUE) ;
                    return 0 ;

                case IDM_ENDSERVER:
                    if (hwndServer)
                    {
                       pddes = MakeDDESegment (hwndServer,
                                               0L,
                                               0, 0, NULL, 0) ;
                       WinDdePostMsg (hwndServer, hwnd, WM_DDE_TERMINATE, 0L, TRUE) ;
                       hwndServer = (HWND) 0;
                       WinInvalidateRect (hwnd, 0L, 0L) ;
                    }
                    break ;

                case IDM_ENDCLIENT:
                    if (hwndClient)
                    {
                      pddes = MakeDDESegment (hwndClient,
                                              0L,
                                              0, 0, NULL, 0) ;
                      WinDdePostMsg (hwndClient, hwnd, WM_DDE_TERMINATE, 0L, TRUE) ;
                      hwndClient = 0L ;
                      WinInvalidateRect (hwnd, NULL, 0) ;
                    }
                    return 0 ;


                case IDM_AUTHOR:
                    return 0 ;

                case IDM_PARENT:
                    return 0 ;

                case IDM_CHILDREN:
                    return 0 ;

                case IDM_DATE:
                    return 0 ;
                }
            break ;

        case WM_CREATE:
             strcpy (szString, "Hello") ;
             hwndServer = 0L ;
             hwndClient = 0L ;
             return 0 ;

        case WM_PAINT:
             hps = WinBeginPaint (hwnd, 0L, 0L) ;
             WinQueryWindowRect (hwnd, &rcl) ;
             sprintf(szString,"Server Window Handle: %u",hwndServer) ;
             WinDrawText (hps, -1, (PSZ) szString, &rcl, CLR_NEUTRAL, CLR_BACKGROUND,
                          DT_CENTER | DT_VCENTER | DT_ERASERECT) ;
             sprintf(szString,"Client Window Handle: %u",hwndClient) ;
             WinDrawText (hps, -1, szString, &rcl, CLR_NEUTRAL, CLR_BACKGROUND,
             DT_CENTER  | DT_BOTTOM) ;
             WinEndPaint (hps) ;
             return 0 ;

        case WM_DESTROY:
             return 0 ;
        }
    return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
    }

void DisplaywStatus (HWND hwnd, USHORT fsStatus)
     {
     static CHAR szString [80] ;

     switch (fsStatus)
	 {

	 case DDE_FACK:
	      strcpy (szString, "DDE_FACK") ;
	      break ;


	 case DDE_FACKREQ:
	      strcpy (szString, "DDE_FACKREQ") ;
	      break ;

	 case DDE_FBUSY:
	      strcpy (szString, "DDE_FBUSY") ;
	      break ;

	 case DDE_FNODATA:
	      strcpy (szString, "DDE_FNODATA") ;
	      break ;

	 case DDE_FRESPONSE:
	      strcpy (szString, "DDE_FRESPONSE") ;
	      break ;

	 case DDE_NOTPROCESSED:
	      strcpy (szString, "DDE_NOTPROCESSED") ;
	      break ;

	 case DDE_FRESERVED:
	      strcpy (szString, "DDE_FRESERVED") ;
	      break ;

	 case DDE_FAPPSTATUS:
	      strcpy (szString, "DDE_FAPPSTATUS") ;
	      break ;

	 default:
	      strcpy (szString, "DDE_UNKNOWN") ;
	      break ;

	 }

         if (!fQuiet)
            {
            WinMessageBox(HWND_DESKTOP,
                       hwnd,
                       szString,
                       "fsStatus",
                       0,
                       MB_OK);
            }
     }

void DisplayDdeMsgType (HWND hwnd, USHORT msg)
     {
     static CHAR szString [80] ;

     switch (msg)
	  {
	  case WM_DDE_INITIATEACK:
	       strcpy (szString, "WM_DDE_INITIATEACK") ;
	       break ;

	  case WM_DDE_TERMINATE:
	       strcpy (szString, "WM_DDE_TERMINATE") ;
	       break ;

	  case WM_DDE_ACK:
	       strcpy (szString, "WM_DDE_ACK") ;
	       break ;

	  case WM_DDE_DATA:
	       strcpy (szString, "WM_DDE_DATA") ;
	       break ;

	  case WM_DDE_INITIATE:
	       strcpy (szString, "WM_DDE_INITIATE") ;
	       break ;

	  case WM_DDE_REQUEST:
	       strcpy (szString, "WM_DDE_REQUEST") ;
	       break ;

	  case WM_DDE_ADVISE:
	       strcpy (szString, "WM_DDE_ADVISE") ;
	       break ;

	  case WM_DDE_UNADVISE:
	       strcpy (szString, "WM_DDE_UNADVISE") ;
	       break ;

	  case WM_DDE_POKE:
	       strcpy (szString, "WM_DDE_POKE") ;
	       break ;

	  case WM_DDE_EXECUTE:
	       strcpy (szString, "WM_DDE_EXECUTE") ;
	       break ;

	  case WM_DDE_LAST:
	       strcpy (szString, "WM_DDE_LAST") ;
	       break ;

	  default:
	       strcpy (szString, "WM_DDE_UNKNOWN") ;
	       break ;
	  }

         if (!fQuiet)
            {
            WinMessageBox(HWND_DESKTOP,
                         hwnd,
                         szString,
                         "Message Received",
                         0,
                         MB_OK);
            }
    }

void DisplayusFormat (HWND hwnd, USHORT usFormat)
     {
     static CHAR szString [80] ;

     switch (usFormat)
        {
        case DDEFMT_TEXT:
            strcpy (szString, "DDEFMT_TEXT") ;
            break;

        default:
            strcpy (szString, "DDEFMT_UNKNOWN") ;
            break;

        }

        if (!fQuiet)
            {
            WinMessageBox(HWND_DESKTOP,
                          hwnd,
                          szString,
                          "DDE FORMAT",
                          0,
                          MB_OK);
            }
     }

BOOL DisplayabData (HWND hwnd, PDDESTRUCT pddes)
     {
     PSZ pszString ;

     pszString = (PSZ) ( (LONG) pddes + pddes->offabData) ;
     if (!fQuiet)
        {
        WinMessageBox(HWND_DESKTOP,
                     hwnd,
                     pszString,
                     "DDE DATA",
                     0,
                     MB_OK);
        }
     return TRUE ;

     }

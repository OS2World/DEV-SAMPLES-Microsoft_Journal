#define YEAH_PROCS
#define INCL_PM
#define INCL_WINDDE
#define INCL_WINTRACKRECT
#include <os2.h>
#include "server.h"

HAB hab;
HWND hwndServer;
HWND hwndServerFrame;
HMQ hmqServer;

HCHNL hchnlStockInfo;
HCHNL hchnlSystem;

HWND hwndClient;
HWND hwndSystem;
char szServerClass[] = "Server";
char szAppName[] = "Server";
char szTopic[] = "StockInfo";
char szSystem[] = "System";
#define iStockMac 5
BOOL fWaitAck[iStockMac];
BOOL fStockAdvised[iStockMac];
int cfFormat[iStockMac];
int cfCSV;
BOOL fQuit;

int cScroll=8;
char szMarquee[256];

char *rgszItem[] = {  "IBM", "T", "GM", "INDU", "MSFT" };

/* These are sample strings that are exact duplicates of what the
	real Server Application would send out.  Here is the format:

	name,dataaone,datatwo,datathree,datafour,voludme,exg,n,b,e

	name is 3 or 5 letter name of symbol
	dataone is decimal number of last sale
	datatwo is decimal number of previous close
	datathree is decimal number of high
	datafour is decimal number o low.
	volume is volume
	exg is exchange where traded.  Also signifies open or close.
	n is news indicator
	b is real time and background indicator
	e is exchange where listed
	a is optional indicator
	[usually applications ignore the last four fields]

   NOTE:  dataone..datafour must be padded with leading spaces to the
	 hundreds digit.


*/

#define ichMax 11
char *szstock[] = {  "IBM ,120.2,120.0,121.3,119.6,940K,P,N,Y,N",
		     "T   , 28.0, 27.5, 28.6, 27.4,1.40M,X,N,Y,N",
		     "GM  , 83.6, 83.5, 84.7, 82.4,390K,B,N,Y,N",
		     "INDU, 76.6, 76.5, 77.7, 76.4,220K,B,N,Y,N",
		     "MSFT, 49.5, 48.5, 50.5, 48.4,890K,B,N,Y,N"
		   };


/************* PROCEDURE DECLARATIONS   */

MRESULT FAR PASCAL ServWndProc(HWND, USHORT, MPARAM, MPARAM);
unsigned random();

/************* COMMAND DISPATCH TABLE   */

int (*rgpfnCmds[CCMDS])() = {
	NULL, NULL, NULL, NULL,         /* File menu */
	NULL, NULL, NULL, NULL, NULL    /* Edit menu */
};

#define toupper(ch) ((ch)&0xdf)

/**************************************/

VOID DoCommand(cmd)
int cmd;
{
	int (*pfn)();

/*
	Lo byte of cmd value is index into dispatch table
	Hi byte of cmd is menu index, or whatever.  Hi byte
	can be used in a switch here to do common processing for
	related commands.
*/
	if ((pfn = rgpfnCmds[LOBYTE(cmd)]) != NULL)
		(*pfn)();
}

BOOL ServerInit()
{
	HATOMTBL hatomtbl;
	
	hab = WinInitialize(0);

	hmqServer = WinCreateMsgQueue(hab, 0);

	if (!WinRegisterClass(hab, szServerClass, (PFNWP)ServWndProc, 0L, 0))
		return(FALSE);

	hatomtbl = WinQuerySystemAtomTable();
	cfCSV = (int)WinAddAtom(hatomtbl, (PSZ)"Csv");

	randomize(42);

	LpszCopy((PSZ)"Market Open...................................................................", (PSZ)szMarquee);

	return(TRUE);
}

int Os2Main(argc, argv)
int argc;
char **argv;
{
	int dx, dy;
	long lgrbitCtl;
	QMSG qmsg;
	HCHNL HchnlCreate();
	HPS hps;
	FONTMETRICS fm;

	if (!ServerInit()) {
		WinAlarm(HWND_DESKTOP, WA_ERROR);
		return(0);
	}

	lgrbitCtl = FCF_DLGBORDER|FCF_TASKLIST;

	hwndServerFrame = WinCreateStdWindow(HWND_DESKTOP,
			0L,
			&lgrbitCtl,
			szServerClass, NULL,
			0L,
			(HMODULE)NULL, 0,
			(HWND FAR *)&hwndServer);

	hps = WinGetPS(hwndServer);
	GpiQueryFontMetrics(hps, (long)sizeof(FONTMETRICS), &fm);
	dx = fm.lAveCharWidth*40;
	dy = fm.lXHeight*4;
	WinReleasePS(hps);
	WinSetWindowPos(hwndServerFrame, HWND_TOP, 10, 10, dx, dy, SWP_SIZE|SWP_MOVE|SWP_ZORDER);

	/* create windows for dde communication */
	hchnlStockInfo = HchnlCreate(szTopic);
	hchnlSystem = HchnlCreate(szSystem);
	if (hwndServer == NULL || hchnlStockInfo == NULL || hchnlSystem == NULL)
		return(0);


	WinShowWindow(hwndServerFrame, TRUE);
	WinShowWindow(hwndServer, TRUE);
	WinSetFocus(HWND_DESKTOP, hwndServer);

	while (WinGetMsg(hab, (PQMSG)&qmsg, NULL, 0, 0)) {
		WinDispatchMsg(hab, (PQMSG)&qmsg);
	}

	WinDestroyWindow(hwndServerFrame);

	WinDestroyMsgQueue(hmqServer);
	WinTerminate(hab);

	DosExit(EXIT_PROCESS, 0);
}

/********** Server Window Procedure **************/

MRESULT FAR PASCAL ServWndProc(hwnd, msg, mp1, mp2)
HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
	int cf;
	int iStock;
	int ret;
	HPS hps;
	RECTL rclPaint;
	VOID ServerPaint();
	DDESTRUCT far *LpddeAlloc();
#define lpdde ((DDESTRUCT far *)mp2)
#define lpddeinit ((DDEINIT far *)mp2)
#define hchnlClient ((HWND)mp1)

	switch (msg) {
	case WM_PAINT:
		hps = WinBeginPaint(hwnd, (HPS)NULL, &rclPaint);
		ServerPaint(hwnd, hps);
		WinEndPaint(hps);
		return(0L);

	case WM_COMMAND:
		DoCommand(LOUSHORT(mp1));
		return(0L);

	case WM_TIMER:
		GotTimer();
		return(0L);
		
	case WM_CLOSE:
	case WM_BUTTON1DBLCLK:
		if (!hwndClient && !hwndSystem)
			goto GoodNight;

		if (fQuit)
			return(0L);
		
		fQuit=TRUE;
		if (hwndClient != NULL)
			WinDdePostMsg(hwndClient, hchnlStockInfo, WM_DDE_TERMINATE, 0L, TRUE);
		if (hwndSystem != NULL)
			WinDdePostMsg(hwndClient, hchnlSystem, WM_DDE_TERMINATE, 0L, TRUE);
		break;

	case WM_BUTTON1DOWN:
		return((ULONG)WinSendMsg(hwndServerFrame, WM_TRACKFRAME,
                    (MPARAM)(((long)mp2&0xffff) | TF_MOVE), (MPARAM)0L));

	case WM_DDE_INITIATE:

		if ((CchLpszLen(lpddeinit->pszAppName) == 0 ||
		     FLpszEq(lpddeinit->pszAppName, (PSZ)szAppName)) && !fQuit)
			{
			if ((CchLpszLen(lpddeinit->pszTopic) == 0 ||
			     FLpszEq(lpddeinit->pszTopic, (PSZ)szSystem)) &&
			    hwndSystem == NULL)
				{
				WinDdeRespond(hchnlClient, hchnlSystem, (PSZ)szAppName, (PSZ)szSystem);
				hwndSystem = hchnlClient;
				}
			if ((CchLpszLen(lpddeinit->pszTopic) == 0 ||
			     FLpszEq(lpddeinit->pszTopic, (PSZ)szTopic)) &&
			    hwndClient == NULL)
				{
				WinDdeRespond(hchnlClient, hchnlStockInfo, szAppName, szTopic);
				hwndClient = hchnlClient;
				}
			}
		break;
	case WM_DDE_TERMINATE:
		if (hwnd == hchnlStockInfo)
			{
			WinStopTimer(hab, hwndServer, 0xedf);
			if (!fQuit)
				WinDdePostMsg(hwndClient, hwnd, WM_DDE_TERMINATE, 0L, TRUE);
			hwndClient = NULL;
			}
		if (hwnd == hchnlSystem)
			{
			if (!fQuit)
				WinDdePostMsg(hwndSystem, hwnd, WM_DDE_TERMINATE, 0L, TRUE);
			hwndSystem = NULL;
			}
		if (fQuit && hwndClient == NULL && hwndSystem == NULL)
			{
GoodNight:
			WinDestroyWindow(hwndServerFrame);
			WinDestroyMsgQueue(hmqServer);
			WinTerminate(hab);
			DosExit(EXIT_PROCESS, 0);
			}
		break;

	case WM_DDE_ACK:
		for (iStock=0; iStock<iStockMac; iStock++)
			if (FLpszEq(DDES_PSZITEMNAME(lpdde), (PSZ)rgszItem[iStock]))
				fWaitAck[iStock]=FALSE;
		break;

	case WM_DDE_ADVISE:
		/* Note this code Assumes CF_CSV is the requested
		* Clipboard format.  Ideally one should check first,
		* by checking the wFormat field of the Advise block
		* which is passed as a handle in the loword of lParam.
		*/
		if  (hwnd == hchnlSystem || fQuit)
			break; /* shouldn't we nack here? */
		cf = lpdde->usFormat;
		ret=0;
		for (iStock=0; iStock<iStockMac; iStock++)
			if (FLpszEq(DDES_PSZITEMNAME(lpdde), (PSZ)rgszItem[iStock]) &&
				(cf == cfCSV || cf == DDEFMT_TEXT))
				{
				fStockAdvised[iStock]=TRUE;
				fWaitAck[iStock]=FALSE;
				cfFormat[iStock]=cf;
				ret = DDE_FACK;
				break;
				}
		lpdde->fsStatus = ret;
		WinDdePostMsg(hchnlClient, hwnd, WM_DDE_ACK, lpdde, TRUE);
		if (ret)
			WinStartTimer(hab, hwndServer, 0xedf, 250);
		break;

	case WM_DDE_REQUEST:
		if (hwnd == hchnlSystem && fQuit)
			break;
		cf = lpdde->usFormat;
		for (iStock=0; iStock<iStockMac; iStock++)
			if (FLpszEq(DDES_PSZITEMNAME(lpdde), (PSZ)rgszItem[iStock]) && (cf == cfCSV || cf == DDEFMT_TEXT))
				{
				DosFreeSeg(PDDESTOSEL(lpdde));
				lpdde = LpddeAlloc(rgszItem[iStock], 50);
				/* mark data as ack to request */
				lpdde->fsStatus = DDE_FRESPONSE;
				lpdde->usFormat = cf;
				LpszCopy((PSZ)szstock[iStock], DDES_PABDATA(lpdde));
				WinDdePostMsg(hwndClient, hchnlStockInfo, WM_DDE_DATA, lpdde, TRUE);
				break;
				}
		if (iStock == iStockMac) /* Nack request if no item found */
			{
			lpdde->fsStatus = 0;
			WinDdePostMsg(hchnlClient, hwnd, WM_DDE_ACK, lpdde, TRUE);
			}
		break;

	case WM_DDE_UNADVISE:
		if (hwnd == hchnlSystem || fQuit)
			break;
		ret=0;
		for (iStock=0; iStock<iStockMac; iStock++)
			if (FLpszEq(DDES_PSZITEMNAME(lpdde), (PSZ)rgszItem[iStock]))
				{
				fStockAdvised[iStock]=FALSE;
				ret = DDE_FACK;
				break;
				}
		lpdde->fsStatus = ret;
		WinDdePostMsg(hchnlClient, hwnd, WM_DDE_ACK, lpdde, TRUE);
		break;

	default:
DefProc:
		return(WinDefWindowProc(hwnd, msg, mp1, mp2));
	}
#undef lpdde
#undef hchnlClient
}

VOID ServerPaint(hwnd, hps)
HWND hwnd;
HPS hps;
{
	RECTL rcl;
	CHARBUNDLE charb;
	
	GpiQueryAttrs(hps, PRIM_CHAR, CBB_BOX, &charb);
	charb.sizfxCell.cy *= 4;
	charb.sizfxCell.cx *= 4;
	GpiSetAttrs(hps, PRIM_CHAR, CBB_BOX, CBB_SET, &charb);
	WinQueryWindowRect(hwnd, &rcl);
	WinDrawText(hps, -1, szMarquee, &rcl, SYSCLR_WINDOWTEXT,
			SYSCLR_WINDOW, DT_ERASERECT);
}



/* GetQuote:

   Assumes number is found between 2 commas, can have leading spaces.

   Number returned is 10x real #.
   ie  XXXX,123.3,XXXX will be returned as 1233
*/

int GetQuote(nStock,nData)
int nStock, nData;

{
	char *cT;
	int  nValue = 0;

	cT = szstock[nStock];
	while( *cT && nData > 0)
		{
		if (*cT == ',')
			nData--;
		cT++;
		}

	while ( *cT == ' ')    /* skip whspace */
		cT++;

	while ( *cT != ',')    /* parse number */
		{
		if(*cT == '.')
			cT++;
		nValue = 10 * nValue + *cT - '0';
		cT++;
		}

	return nValue;
}




/* ChangeQuote:

  Assumes:
	nValue < 10000;

  Note to avoid using FP, nValue is 10x what the real stock price is.

*/

void ChangeQuote(nStock, nData, nValue)
int nStock, nData, nValue;
{
	char *cT;
	int nBase;
	int nPrevValue;

	cT = szstock[nStock];
	while(*cT && nData > 0)	  /* Make cT point to char after comma */
		{
		if (*cT == ',')
			nData--;
		cT++;
		}

	while( *cT != ',')		  /* blank out field */
		*cT++ = (*cT == '.' ? '.' : ' ');

	cT--;
	while (nValue)
		{
		if (*cT == '.')
			cT--;
		*cT-- = nValue % 10 + '0';
		nValue /= 10;
		}
}

GotTimer()
{
	DDESTRUCT far *lpdde;
	int iStock;
	int nQuote;
	int ich, ichT;
	DDESTRUCT far *LpddeAlloc();

	if (cScroll<ichMax)
		{
		LpszCopy((PSZ)szMarquee+1, (PSZ)szMarquee);
		WinInvalidateRect(hwndServer, NULL, FALSE);
		cScroll++;
		return;
		}

	iStock = random() % iStockMac;

	if (!fStockAdvised[iStock] || fWaitAck[iStock])
		return;

	cScroll = 0;
	nQuote = GetQuote(iStock,1) + (random()&0x07) - 0x03;
	if(iStock == 4) 				    /* msft */
		nQuote++;
	ChangeQuote(iStock,1,nQuote);

	lpdde = LpddeAlloc(rgszItem[iStock], 50);
	lpdde->fsStatus = DDE_FACKREQ;
	lpdde->usFormat = cfFormat[iStock];

	LpszCopy((PSZ)szstock[iStock], DDES_PABDATA(lpdde));

	WinDdePostMsg(hwndClient, hchnlStockInfo, WM_DDE_DATA, lpdde, TRUE);
	fWaitAck[iStock]=TRUE;
	for (ich=0, ichT=CchLpszLen((PSZ)szMarquee); ich<ichMax; ich++,ichT++)
		{
		szMarquee[ichT] = szstock[iStock][ich];
		if (szMarquee[ichT] == ',')
			szMarquee[ichT] = ' ';
		}
	szMarquee[ichT] = 0;
}

HCHNL HchnlCreate(sz)
char *sz;
{
	return(WinCreateWindow(HWND_OBJECT, szServerClass, &sz[1],
			0L, 0, 0, 0, 0,
			HWND_DESKTOP, HWND_TOP, 23, NULL, NULL));
}

DDESTRUCT far *LpddeAlloc(sz, cbData)
char *sz;
int cbData;
{
	int cbBase;
	SEL sel;
	DDESTRUCT far *lpdde;

	cbBase = CchLpszLen((PSZ)sz)+1+sizeof(DDESTRUCT);
	DosAllocSeg(cbBase+cbData, &sel, SEG_GIVEABLE);
	lpdde = MAKEP(sel, 0);
	lpdde->cbData = cbBase+cbData;
	lpdde->fsStatus = 0;
	lpdde->usFormat = 0;
	lpdde->offszItemName = sizeof(DDESTRUCT);
	lpdde->offabData = cbBase;
	LpszCopy((char far *)sz, DDES_PSZITEMNAME(lpdde));
	return(lpdde);
}

CchLpszLen(lpch)
char far *lpch;
{
	int cch;
	
	for (cch=0; *lpch++; cch++)
		;
	return(cch);
}

LpszCopy(lpchFrom, lpchTo)
char far *lpchFrom;
char far *lpchTo;
{
	while (*lpchTo++ = *lpchFrom++)
		;
}

FLpszEq(lpch, lpch2)
char far *lpch;
char far *lpch2;
{
	for (; *lpch && toupper(*lpch2) == toupper(*lpch); lpch++, lpch2++)
		;
	return(toupper(*lpch) == toupper(*lpch2));
}

/*
 *	Global data
 */

static unsigned seed = 0;
static unsigned rgrnd[55];
static int j, k;



/***    random2 - returns a random unsigned integer
 *
 *      This is a simple linear congruence RNG.
 *
 *	random2()
 *
 *	Entry:	None
 *	Exit:	returns - 16-bit unsigned pseudo-random number
 *
 *	The actual formula used is:
 *	
 *		seed = (25173 * seed + 13849) % 65536;
 *
 *	Since the modulus corresponds nicely with our word size, we
 *	don't have to do the division.
 */

unsigned random2()
{
    	return(seed = 25173 * seed + 13849);
}

/***    randomize - initialize RNG
 *
 *	Here we initialize our additive random number generator (RNG).
 *
 *	randomize(u)
 *
 *	Entry:	u - random number seed
 *	Exit:	None
 *
 *	We just fill up our rgrnd array with pseudo-random numbers
 *	(generated with a congruence RNG) and then initialize a couple
 *	of pointers into the arrays (cf. Knuth).
 */

randomize(u)
unsigned u;
{
     	seed = u;	/* seed random2() */
	for (u = 0; u < 55; u++)
		rgrnd[u] = random2();
    	j = 23; k = 54;
}

/***    random - return a random number
 *
 *      This is our magic additive RNG.  Cf. Knuth and randomize()
 *	above for further info.
 *
 *	random(umax)
 *
 *	Entry:	umax - range random number should be in
 *	Exit:	returns - random number within [0..umax)
 */

unsigned random(umax)
unsigned umax;
{
	unsigned u;

    	u = (rgrnd[k] += rgrnd[j]);
    	if (--j < 0)
    		j = 54;
    	if (--k < 0)
    		k = 54;
	/* u is a random unsigned integer in the interval [0..65536).
	   Multiplying by umax will give us a 32-bit value with the
	   high 16 bits in the range [0..umax), which is just what we 
	   want. */
    	return((unsigned)(((long)u*umax)>>16));
}

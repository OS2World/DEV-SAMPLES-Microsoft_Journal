typedef int far *LPINT;


/* this is the simple case of the definition of this structure.  A more
    detailed case is included in QuickHelp in PM SDK 1.1 */

typedef struct _POPUPITEM {
  USHORT afStyle;
  USHORT afAttribute;
  USHORT id;
  char	 text[80];   /* should only be the size of each text string */
} POPUPITEM;

typedef struct _MENUCREATETEMPLATE {
  USHORT    size;
  USHORT    version;
  USHORT    codepage;
  USHORT    mnemonic;
  USHORT    itemcount;
  POPUPITEM item[1];	/* multiple items are allowed, item count = # of items */
} MENUCREATETEMPLATE;


/* constants for the info window */

#define CY_INFOWINDOW 15
#define ID_INFOWINDOW    0x100     /* ID for new frame control */

/* menu and icon id */
#define ID_HELLO 1


/* petzold file dialog id */

/*  ???????????????????????????????????????

#define IDD_OPEN         1

#define IDD_PATH         10
#define IDD_FILEEDIT     11
#define IDD_DIRLIST      12
#define IDD_FILELIST     13

#define MAXEDITCHARS    16000
*/



#define IDM_NEWMENU  0x0100
#define IDM_DELPOPUP 0x0101

#define ID_ADDTOSYSMENU  0x0151
#define ID_DELSYS    0x0152
#define ID_DISABLED  0x0153
#define ID_TOGGLEHELP 0x0154

#define ID_FIRSTITEM 6000
#define ID_NEWITEM   6001

void MainCommand(HWND,USHORT);

/* COMMAND.C */
void MakePopup(HWND);
void HelloCommand(HWND,USHORT);

/* COMMON.C */
void MessageInt(HWND,int,PCH);

/* HELLO.C */
int cdecl main(void);

/* PAINT.C */
void HelloPaint(HWND);

/* WNDPROC.C */
MRESULT EXPENTRY HelloWndProc( HWND, USHORT, MPARAM, MPARAM );
MRESULT EXPENTRY FrameSubProc( HWND, USHORT, MPARAM, MPARAM );
MRESULT EXPENTRY InfoWndProc( HWND, USHORT, MPARAM, MPARAM );

/* user defined messages */
#define WM_UPDATESTRING WM_USER+1

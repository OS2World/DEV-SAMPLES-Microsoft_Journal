typedef int far *LPINT;


/* menu and icon id */
#define ID_HELLO 1


/* petzold file dialog id */
#define IDD_OPEN         1

#define IDD_PATH         10
#define IDD_FILEEDIT     11
#define IDD_DIRLIST      12
#define IDD_FILELIST     13


#define MAXEDITCHARS    16000


/* window extra definitions */
#define EXTRABYTES    4
#define TOPLINE       0
#define BUFFER        2
#define IDM_OPEN  0x0100
#define IDM_CLOSE 0x0101

void MainCommand(HWND,USHORT);


/* COMMAND.C */
void MakePopup(HWND);
BOOL HelloCommand(HWND,USHORT);

/* COMMON.C */
void MessageInt(HWND,int,PCH);

/* HEAD.C */
MRESULT EXPENTRY OpenDlgProc( HWND, USHORT, MPARAM, MPARAM );
SHORT ParseFileName (CHAR *, CHAR *);
VOID FillDirListBox(HWND,char *);
VOID FillFileListBox(HWND);

/* HELLO.C */
int cdecl main(void);

/* PAINT.C */
void HelloPaint(HWND);

/* SCROLL.C */
MRESULT FAR PASCAL ScrollWndProc( HWND, USHORT, MPARAM, MPARAM );
int MakeList(SEL,int);
void CreateScrollWnd(HWND);
MRESULT VScroll(HWND,LONG);

/* SPAINT.C */
DrawStrings(HPS,	POINTL,PCH,int,int,int,int);
PCH  ForwardWalk(PCH,LPINT);
void ScrollPaint(HWND);

/* WNDPROC.C */
MRESULT FAR PASCAL HelloWndProc( HWND, USHORT, MPARAM, MPARAM );


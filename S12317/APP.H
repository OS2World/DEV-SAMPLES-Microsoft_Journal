/* String table constants */
#define IDSNAME         1
#define IDSKIDNAME	2

/* Resource Ids */
#define ID_APP          1
#define IDD_ABOUT       2
#define IDD_INPUT       3

/* Menu constants */
#define IDADDSTRINGS	1001
#define IDENTER         1002
#define IDABOUT         1006

/* Input Dialog Box */
#define IDEDIT          300

/*control ID*/
#define LID_LIST1	30

/* Function prototypes */
SHORT cdecl main                 ( void );
MRESULT FAR PASCAL APPWndProc    ( HWND, USHORT, MPARAM, MPARAM );
MRESULT FAR PASCAL KidWndProc	 ( HWND, USHORT, MPARAM, MPARAM );
VOID    FAR PASCAL APPPaint      ( HWND, USHORT, MPARAM, MPARAM );
MRESULT FAR PASCAL AddStrings	 ( HWND, USHORT, MPARAM, MPARAM );
MRESULT FAR PASCAL About	 ( HWND, USHORT, MPARAM, MPARAM );
MRESULT FAR PASCAL EditDlg	 ( HWND, USHORT, MPARAM, MPARAM );

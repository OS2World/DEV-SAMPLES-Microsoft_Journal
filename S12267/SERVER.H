/****** Resource IDs *****/

#define IDR_GENAPP    1

/****** Menu command IDs *****/
/* (by convention, hi byte is menu index, lo byte is dispatch table index) */

/* File menu */

#define CMD_NEW     0x0000
#define CMD_OPEN    0x0001
#define CMD_SAVE    0x0002
#define CMD_SAVEAS  0x0003

/* Edit menu */
#define CMD_UNDO    0x0104
#define CMD_CUT     0x0105
#define CMD_COPY    0x0106
#define CMD_PASTE   0x0107
#define CMD_CLEAR   0x0108

/* Number of menu commands */
#define CCMDS       (4+5)

typedef HWND HCHNL;

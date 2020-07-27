#include <os2.h>
#include <stddef.h>
#include "stdlib.h"
#include "hello.h"


void MessageInt(hWnd,i,lpText)
HWND hWnd;
int i;
PCH lpText;
{
char tmp[8];

itoa(i,tmp,10);
WinMessageBox(HWND_DESKTOP,hWnd,(PCH)tmp,lpText, NULL,MB_OK|MB_ICONEXCLAMATION);
}

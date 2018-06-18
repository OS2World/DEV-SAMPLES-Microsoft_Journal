#define INCL_PM

#include <os2.h>
#include <stddef.h>
#include "hello.h"


HPS  hNPS;


void HelloPaint(hWnd)
HWND hWnd;
{
POINTL pt;
RECTL  rect;

hNPS = WinBeginPaint(hWnd,hNPS,NULL);

WinQueryWindowRect(hWnd,&rect);
WinFillRect(hNPS,&rect,CLR_WHITE); /* fill back ground */

WinEndPaint(hNPS);
}

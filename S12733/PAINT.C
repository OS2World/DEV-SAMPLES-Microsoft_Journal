#define INCL_PM

#include <os2.h>
#include <stddef.h>
#include <string.h>
#include "hello.h"

extern char szMessage[];

void HelloPaint(hWnd)
HWND hWnd;
{
HPS    hPS;
RECTL  rect;

hPS = WinBeginPaint( hWnd, (HPS)NULL,&rect);

WinFillRect(hPS,&rect,CLR_GREEN);

rect.xLeft=10;
rect.xRight=200;
rect.yBottom=2;
rect.yTop=30;

#define TEXT "This is some text"

WinDrawText(hPS, strlen(TEXT), TEXT, &rect, CLR_BLACK, CLR_WHITE, DT_LEFT | DT_BOTTOM);

WinEndPaint(hPS);
}

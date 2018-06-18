#define DEVESC_STARTDOC 	     8150L
#define DEVESC_ENDDOC		     8151L
#define DEVESC_NEWFRAME         16300L
LONG APIENTRY DevEscape(HDC hdc, LONG lCode, LONG lInCount, PBYTE pbInData,
			PLONG plOutCount, PBYTE pbOutData);

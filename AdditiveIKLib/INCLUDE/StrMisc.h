#ifndef STRMISCH
#define STRMISCH

#include <Windows.h>
#include <gdiplus.h>

#ifdef STRMISCCPP

	int ShortenNameW(const WCHAR* srcname, WCHAR* dstname, int dstbufleng, int cpmaxleng);

	int DrawGdiplusButton(Gdiplus::Image* srcimage, HDC srchdc,
		int drawposx, int drawposy, int drawwidth, int drawheight, float srcalpha);
	int DrawGdiplusButtonStretch(Gdiplus::Image* srcimage, HDC srchdc,
		int drawposx, int drawposy,
		int srcw, int srch, int dstw, int dsth, float srcalpha);
	
	bool CalcTextExtent(HDC hdc, const WCHAR* srcname, SIZE* pspacesize, SIZE* pnamesize);

	bool DrawGdiText(HDC hDC, const WCHAR* srctext, 
		int posx, int posy, int labelheight, 
		COLORREF textcol, bool underlineflag);

#else

	extern int ShortenNameW(const WCHAR* srcname, WCHAR* dstname, int dstbufleng, int cpmaxleng);

	extern int DrawGdiplusButton(Gdiplus::Image* srcimage, HDC srchdc,
		int drawposx, int drawposy, int drawwidth, int drawheight, float srcalpha);
	extern int DrawGdiplusButtonStretch(Gdiplus::Image* srcimage, HDC srchdc,
		int drawposx, int drawposy,
		int srcw, int srch, int dstw, int dsth, float srcalpha);

	extern bool CalcTextExtent(HDC hdc, const WCHAR* srcname, SIZE* pspacesize, SIZE* pnamesize);

	extern bool DrawGdiText(HDC hDC, const WCHAR* srctext,
		int posx, int posy, int labelheight,
		COLORREF textcol, bool underlineflag);

#endif

#endif
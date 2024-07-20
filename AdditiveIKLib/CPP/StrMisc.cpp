#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#define STRMISCCPP
#include <StrMisc.h>

#define DBGH
#include <dbg.h>


//GUI表示文字数制限
//名前を最大cpmaxleng文字までコピーする
int ShortenNameW(const WCHAR* srcname, WCHAR* dstname, int dstbufleng, int cpmaxleng)
{
	if (!srcname || !dstname) {
		_ASSERT(0);
		return 1;
	}
	if (dstbufleng <= 0) {
		_ASSERT(0);
		return 1;
	}
	if ((cpmaxleng <= 0) || (cpmaxleng >= 1024)) {
		_ASSERT(0);
		return 1;
	}
	int nameleng = (int)wcslen(srcname);
	if ((nameleng <= 0) || (nameleng >= 1024)) {
		_ASSERT(0);
		return 1;
	}

	WCHAR filename[1024] = { 0L };
	WCHAR printname[1024] = { 0L };
	ZeroMemory(filename, sizeof(WCHAR) * 1024);
	ZeroMemory(printname, sizeof(WCHAR) * 1024);

	wcscpy_s(filename, 1024, srcname);
	int cpleng;
	if (nameleng <= cpmaxleng) {
		cpleng = nameleng;
	}
	else {
		cpleng = cpmaxleng;
	}
	wcsncpy_s(printname, 1024, filename, cpleng);

	if (cpleng < dstbufleng) {
		wcscpy_s(dstname, dstbufleng, printname);
	}
	else {
		_ASSERT(0);
		return 1;
	}
	return 0;
}


int DrawGdiplusButton(Gdiplus::Image* srcimage, HDC srchdc,
	int drawposx, int drawposy, int drawwidth, int drawheight, float srcalpha)
{
	if (!srcimage) {
		_ASSERT(0);
		return 1;
	}
	if (srchdc == NULL) {
		_ASSERT(0);
		return 1;
	}

	Gdiplus::Graphics* gdipg = new Gdiplus::Graphics(srchdc);
	if (gdipg) {
		Gdiplus::ImageAttributes attr;
		Gdiplus::ColorMatrix cmat = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Red
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // Green
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Blue
			0.0f, 0.0f, 0.0f, srcalpha, 0.0f,   // Alpha (70%)
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f    // must be 1
		};
		attr.SetColorMatrix(&cmat);
		gdipg->DrawImage(srcimage, Gdiplus::Rect(drawposx, drawposy, drawwidth, drawheight),
			0, 0, drawwidth, drawheight,
			Gdiplus::UnitPixel, &attr, NULL, NULL);
		delete gdipg;
	}
	return 0;
}

int DrawGdiplusButtonStretch(Gdiplus::Image* srcimage, HDC srchdc,
	int dstx, int dsty,
	int srcw, int srch, int dstw, int dsth, float srcalpha)
{
	if (!srcimage) {
		_ASSERT(0);
		return 1;
	}
	if (srchdc == NULL) {
		_ASSERT(0);
		return 1;
	}
	if ((srcw <= 0) || (srch <= 0) || (dstw <= 0) || (dsth <= 0)) {
		_ASSERT(0);
		return 1;
	}


	Gdiplus::Graphics* gdipg = new Gdiplus::Graphics(srchdc);
	if (gdipg) {
		Gdiplus::ImageAttributes attr;
		Gdiplus::ColorMatrix cmat = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Red
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // Green
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Blue
			0.0f, 0.0f, 0.0f, srcalpha, 0.0f,   // Alpha (70%)
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f    // must be 1
		};
		attr.SetColorMatrix(&cmat);
		gdipg->DrawImage(srcimage, Gdiplus::Rect(dstx, dsty, dstw, dsth),
			0, 0, srcw, srch,
			Gdiplus::UnitPixel, &attr, NULL, NULL);
		delete gdipg;
	}
	return 0;
}

bool CalcTextExtent(HDC hdc, const WCHAR* srcname, SIZE* pspacesize, SIZE* pnamesize)
{
	if (!srcname || !pspacesize || !pnamesize) {
		_ASSERT(0);
		return false;
	}
	if (*srcname == 0L) {
		_ASSERT(0);
		return false;
	}

	int namelen = (int)wcslen(srcname);
	if (namelen >= 1024) {
		_ASSERT(0);
		return false;
	}

	int loopcount = 0;
	int spacecount = 0;
	while (loopcount < namelen) {
		WCHAR chkwc = *(srcname + loopcount);
		if ((chkwc == TEXT(' ')) || (chkwc == TEXT('　'))) {
			spacecount++;
		}
		else {
			break;
		}
		loopcount++;
	}

	bool result1 = true;
	bool result2 = true;
	if (spacecount > 0) {
		WCHAR strspace[1024] = { 0L };
		ZeroMemory(strspace, sizeof(WCHAR) * 1024);
		wcsncpy_s(strspace, 1024, srcname, spacecount);
		result1 = GetTextExtentPoint32W(hdc, strspace, spacecount, pspacesize);
	}
	else {
		pspacesize->cx = 0;
		pspacesize->cy = 0;
	}

	WCHAR strname[1024] = { 0L };
	ZeroMemory(strname, sizeof(WCHAR) * 1024);
	wcscpy_s(strname, 1024, srcname + spacecount);
	int wccount = (int)wcslen(strname);
	result2 = GetTextExtentPoint32W(hdc, strname, wccount, pnamesize);

	if (result1 && result2) {
		return true;
	}
	else {
		_ASSERT(0);
		return false;
	}
}


bool DrawGdiText(HDC hDC, const WCHAR* srctext,
	int posx, int posy, int labelheight,
	COLORREF textcol, bool underlineflag)
{
	if (!hDC || !srctext) {
		_ASSERT(0);
		return false;
	}

	if (underlineflag == false) {
		SetTextColor(hDC, textcol);
		TextOut(hDC,
			posx, posy,
			srctext, (int)wcslen(srctext));
	}
	else {

		SIZE spacesize, namesize;
		spacesize.cx = 0;
		spacesize.cy = 0;
		namesize.cx = 0;
		namesize.cy = 0;
		bool result0;
		result0 = CalcTextExtent(hDC, srctext, &spacesize, &namesize);

		//オレンジの下線マーク
		int pos01x = posx + spacesize.cx;
		int pos02x = pos01x + namesize.cx;
		int pos01y = posy + (int)(labelheight * 0.85);
		int pos02y = posy + labelheight;

		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 128, 0));
		DeleteObject(SelectObject(hDC, hPen));
		HBRUSH hBrush = CreateSolidBrush(RGB(255, 128, 0));
		DeleteObject(SelectObject(hDC, hBrush));
		
		Rectangle(hDC, pos01x, pos01y, pos02x, pos02y);

		SetTextColor(hDC, textcol);
		TextOut(hDC,
			posx, posy,
			srctext, (int)wcslen(srctext));
	}


	return true;
}

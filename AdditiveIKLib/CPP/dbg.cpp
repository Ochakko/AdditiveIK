#include "stdafx.h"
//#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <windows.h>

#include <crtdbg.h>

#ifdef DBGH
#undef DBGH
#endif
#include <dbg.h>

#include <GlobalVar.h>
#include <InfoWindow.h>

//Global Func
void OutputToInfoWnd(int infocolor, const WCHAR* lpFormat, ...)
{
	if (g_infownd) {
		int ret;
		va_list Marker;
		//unsigned long wleng, writeleng;
		WCHAR outchar[INFOWINDOWLINEW - 10];

		ZeroMemory(outchar, sizeof(WCHAR) * (INFOWINDOWLINEW - 10));

		va_start(Marker, lpFormat);
		ret = vswprintf_s(outchar, INFOWINDOWLINEW - 10, lpFormat, Marker);
		va_end(Marker);

		if (ret < 0)
			return;

		WCHAR strlineno[10] = { 0L };
		if (g_infownd->IsFirstOutput()) {
			swprintf_s(strlineno, L"L%d", g_infownd->GetDataIndex());
		}
		else {
			swprintf_s(strlineno, L"L%d", g_infownd->GetDataIndex() + 1);
		}

		g_infownd->OutputInfo(infocolor, L"%s: %s", strlineno, outchar);
		g_infownd->UpdateWindow();

	}
}

int DbgOut( const WCHAR* lpFormat, ... )
{
	if( !dbgfile ){
		return 0;
	}

	int ret;
	va_list Marker;
	size_t wleng;
	DWORD writeleng;
	WCHAR outchar[7000];
			
	ZeroMemory( outchar, sizeof( WCHAR ) * 7000 );

	va_start( Marker, lpFormat );
	ret = vswprintf_s( outchar, 7000, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 )
		return 1;

	outchar[7000 - 1] = 0L;
	wleng = wcslen( outchar );
	if ((wleng > 0) && (wleng < 7000)) {
		WriteFile(dbgfile, outchar, (DWORD)(sizeof(WCHAR) * wleng), &writeleng, NULL);
	}
	
	return 0;
	
}

int DbgOutB( void* lpvoid, int datsize )
{
	if( !dbgfile )
		return 0;

	unsigned long writeleng;

	WriteFile( dbgfile, lpvoid, datsize, &writeleng, NULL );
	if( datsize != (long)writeleng ){
		return 1;
	}

	return 0;

}

void ErrorMessage(const WCHAR* szMessage, HRESULT hr)
{
    if(HRESULT_FACILITY(hr) == FACILITY_WINDOWS)
		hr = HRESULT_CODE(hr);

	WCHAR* szError;
	if(FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&szError, 0, NULL) != 0)
	{
		DbgOut( L"ErrorMessage : %s: %s\r\n", szMessage, szError);

		MessageBox( NULL, szError, szMessage, MB_OK );

		LocalFree(szError);
	}else{
		DbgOut( L"ErrorMessage : Error number not found\r\n");
	}
}

int OpenDbgFile(int srcappcnt)
{
	WCHAR dbgfilename[MAX_PATH] = { 0L };
	WCHAR appfolder[MAX_PATH] = { 0L };
	bool resgetappfolder = GetAppFolderPathOchakkoLAB(appfolder, MAX_PATH);
	if (!resgetappfolder || (appfolder[0] == 0L)) {
		_ASSERT(0);
		return 1;
	}
	swprintf_s(dbgfilename, MAX_PATH, L"%sdbg%d.txt", appfolder, srcappcnt);//複数のAdditiveIKを立ち上げるため

	if (!dbgfile) {
		//dbgfile = CreateFile(L"dbg.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
		dbgfile = CreateFile(dbgfilename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if (dbgfile == INVALID_HANDLE_VALUE) {
			return 1;
		}
		SetEndOfFile(dbgfile);
		DbgOut(L"InitFunc: dbgfile created\r\n");
	}

	if (!infofile) {

		/* 現在時刻表示:UTC+時差 */
		SYSTEMTIME st;
		GetLocalTime(&st);

		WCHAR infofilename[MAX_PATH] = { 0L };
		swprintf_s(infofilename, MAX_PATH, L"%sinfo%d_%4u_%02u_%02u_%02u_%02u_%02u.txt",
			appfolder,
			srcappcnt,//複数のAdditiveIKを立ち上げるため
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond);

		infofile = CreateFile(infofilename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if (infofile == INVALID_HANDLE_VALUE) {
			return 1;
		}
		SetEndOfFile(infofile);
		DbgOut(L"InitFunc: infofile created\r\n");
	}

	return 0;
}

int CloseDbgFile()
{
	if(dbgfile){		
		DbgOut( L"CloseDbgFile\r\n" );

		FlushFileBuffers( dbgfile );
		SetEndOfFile( dbgfile );
		CloseHandle( dbgfile );
		dbgfile = 0;
	}

	if (infofile) {
		if (g_infownd) {
			int linenum = g_infownd->GetStrNum();
			if (linenum > 0) {
				int lineno;
				for (lineno = 0; lineno < linenum; lineno++) {
					WCHAR strline[INFOWINDOWLINEW] = { 0L };
					int ret;
					ret = g_infownd->GetStr(lineno, INFOWINDOWLINEW, strline);
					wcscat_s(strline, INFOWINDOWLINEW, L"\r\n");

					strline[INFOWINDOWLINEW - 1] = 0L;
					DWORD wleng = (unsigned long)wcslen(strline);
					DWORD writeleng = 0;
					WriteFile(infofile, strline, sizeof(WCHAR) * wleng, &writeleng, NULL);
				}
				FlushFileBuffers(infofile);
				SetEndOfFile(infofile);
			}
			CloseHandle(infofile);
			infofile = 0;
		}
	}

	return 0;
}

int DbgWrite2File( const char* lpFormat, ... )
{
	if( dbgfile == INVALID_HANDLE_VALUE ){
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[2048];
			
	ZeroMemory( outchar, 2048 );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, 2048, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 )
		return 1;

	outchar[2048 - 1] = 0;
	wleng = (unsigned long)strlen( outchar );
	WriteFile( dbgfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		return 1;
	}

	return 0;	
}

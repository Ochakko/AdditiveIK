#pragma once
#ifndef DSUPDATEUNDERTRACKINGH
#define DSUPDATEUNDERTRACKINGH

#include <stdio.h>

#include <windows.h>
#include <wchar.h>
#include <coef.h>

class CDSUpdateUnderTracking
{
public:
	CDSUpdateUnderTracking();
	~CDSUpdateUnderTracking();

	int CreateDSUpdateUnderTracking(HINSTANCE srchinstance);



	int OutputInfo(WCHAR* lpFormat, ...);


	void UpdateWindow();
	void UpdateWindowFunc();

	void OnPaint();
	//void onMouseWheel(const OrgWinGUI::MouseEvent& e);

	HWND GetHWnd() { return m_hWnd; };
	int GetDataIndex() { return m_dataindex; };
	bool IsFirstOutput() { return m_isfirstoutput; };
	int GetStrNum();
	int GetStr(int srcindex, int srcoutleng, WCHAR* strout);

	HCURSOR GetMHCursor() {
		return m_mhcursor;
	};
	HCURSOR GetArrowCursor() {
		return m_arrowcursor;
	};
	void SetArrowCursor(HCURSOR srccursor) {
		m_arrowcursor = srccursor;
	};

private:
	void InitParams();
	void DestroyObjs();

private:
	HWND m_hWnd;
	HWND m_hParentWnd;
	HCURSOR m_mhcursor;
	HCURSOR m_arrowcursor;
	//OrgWinGUI::HDCMaster* m_hdcM;

	//WCHAR m_stroutput[DSUpdateUnderTrackingLINEH][DSUpdateUnderTrackingLINEW];
	WCHAR* m_stroutput;
	
	int m_dataindex;//データ格納位置
	int m_viewindex;//表示開始位置
	bool m_isfirstoutput;//最初のデータ格納かどうか
};

#endif
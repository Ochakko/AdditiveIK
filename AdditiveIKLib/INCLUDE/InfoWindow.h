#pragma once
#ifndef INFOWINDOWH
#define INFOWINDOWH

#include <stdio.h>

#include <windows.h>
#include <wchar.h>
#include <coef.h>

#include <OrgWindow.h>

#include <map>

//1行の長さ
#define INFOWINDOWLINEW	300

//保持する行数
//#define INFOWINDOWLINEH	500000
#define INFOWINDOWLINEH 6000

//表示する行数
#define INFOWINDOWLINEVIEW 4
#define INFOWINDOWLINEVIEW4K 10

class CInfoWindow
{
public:
	CInfoWindow();
	~CInfoWindow();

	int CreateInfoWindow(HWND srcparentwnd, int srcposx, int srcposy, int srcwidth, int srcheight);
	int OutputInfo(int infocolor, const WCHAR* lpFormat, ...);


	void UpdateWindow();
	void UpdateWindowFunc();

	void OnPaint();
	void onMouseWheel(const OrgWinGUI::MouseEvent& e);

	HWND GetHWnd() { return m_hWnd; };
	int GetDataIndex() { return m_dataindex; };
	bool IsFirstOutput() { return m_isfirstoutput; };
	int GetStrNum();
	int GetStr(int srcindex, int srcoutleng, WCHAR* strout);

private:
	void InitParams();
	void DestroyObjs();
	void InitInfoColor();

private:
	HWND m_hWnd;
	HWND m_hParentWnd;
	OrgWinGUI::HDCMaster* m_hdcM;

	//WCHAR m_stroutput[INFOWINDOWLINEH][INFOWINDOWLINEW];
	WCHAR* m_stroutput;
	//std::map<int, int> m_infocolorvec;
	std::vector<int> m_infocolorvec;

	int m_dataindex;//データ格納位置
	int m_viewindex;//表示開始位置
	bool m_isfirstoutput;//最初のデータ格納かどうか
};

#endif
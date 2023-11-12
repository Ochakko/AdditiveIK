#pragma once
#ifndef INFOWINDOWH
#define INFOWINDOWH

#include <stdio.h>

#include <windows.h>
#include <wchar.h>
#include <coef.h>

#include <OrgWindow.h>

//1�s�̒���
#define INFOWINDOWLINEW	300

//�ێ�����s��
//#define INFOWINDOWLINEH	500000
#define INFOWINDOWLINEH 6000

//�\������s��
#define INFOWINDOWLINEVIEW 4
#define INFOWINDOWLINEVIEW4K 10

class CInfoWindow
{
public:
	CInfoWindow();
	~CInfoWindow();

	int CreateInfoWindow(HWND srcparentwnd, int srcposx, int srcposy, int srcwidth, int srcheight);
	int OutputInfo(const WCHAR* lpFormat, ...);


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

private:
	HWND m_hWnd;
	HWND m_hParentWnd;
	OrgWinGUI::HDCMaster* m_hdcM;

	//WCHAR m_stroutput[INFOWINDOWLINEH][INFOWINDOWLINEW];
	WCHAR* m_stroutput;
	
	int m_dataindex;//�f�[�^�i�[�ʒu
	int m_viewindex;//�\���J�n�ʒu
	bool m_isfirstoutput;//�ŏ��̃f�[�^�i�[���ǂ���
};

#endif
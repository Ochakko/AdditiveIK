#include "stdafx.h"

#include <windows.h>

#define DECSETDLGPOSH
#include "SetDlgPos.h"

#define DBGH
#include <dbg.h>
#include <crtdbg.h>

using namespace std;


int SetDlgPosDesktopCenter(HWND hDlgWnd, HWND hWndInsertAfter)
{
	int lefttopposx = 0;
	int lefttopposy = 0;
	int setposx = 0;
	int setposy = 0;

	//POINT ptCursor;
	//GetCursorPos(&ptCursor);
	////::ScreenToClient(s_3dwnd, &ptCursor);

	HWND desktopwnd;
	desktopwnd = ::GetDesktopWindow();
	if (desktopwnd) {
		RECT desktoprect;
		::GetClientRect(desktopwnd, &desktoprect);
		lefttopposx = (desktoprect.left + desktoprect.right) / 2;
		lefttopposy = (desktoprect.top + desktoprect.bottom) / 2;

		//2023/01/26 dlg�T�C�Y���l�����ā@�f�X�N�g�b�v�����ɔz�u
		RECT dlgrect;
		::GetClientRect(hDlgWnd, &dlgrect);
		setposx = max(60, (lefttopposx - (dlgrect.right - dlgrect.left) / 2));
		setposy = max(60, (lefttopposy - (dlgrect.bottom - dlgrect.top) / 2));
		setposx = min(max(60, desktoprect.right - 60), setposx);
		setposy = min(max(60, desktoprect.bottom - 60), setposy);

		SetWindowPos(hDlgWnd, hWndInsertAfter, setposx, setposy, 0, 0, SWP_NOSIZE);
	}
	else {
		lefttopposx = 0;
		lefttopposy = 0;
		setposx = 0;
		setposy = 0;
	}

	//RECT dlgrect;
	//GetWindowRect(fgwnd, &dlgrect);
	//SetCursorPos(dlgrect.left + 25, dlgrect.top + 10);
	
	//2023/01/25
	//���̊֐��́@�w�肵�Ă���E�C���h�E���t�H�A�O�����h�ɂȂ����Ƃ��ɂ��Ă΂��
	//�@�\�̃{�^�����������ہ@�}�E�X���A�b�v����O�ɌĂ΂�邱�Ƃ�����
	//�}�E�X�𓮂����Ɓ@�{�^�����@�\���Ȃ����Ƃ�����̂Ł@SetCursorPos�̓R�����g�A�E�g
	//SetCursorPos(posx + 25, posy + 10);

	//2023/01/26
	//SetCursorPos�ɂ���
	//���̊֐����t�H�A�O�����h�C�x���g����Ăяo�����̂Ł@SetCursorPos���R�����g�A�E�g����
	//���̊֐��̊O�Ł@�_�C�A���O�\������SetCursorPos���邱�Ƃ͑��v

	return 0;
}

int SetDlgPosCursorPos(HWND hDlgWnd, HWND hWndInsertAfter)
{
	int posx = 0;
	int posy = 0;

	POINT ptCursor;
	GetCursorPos(&ptCursor);
	////::ScreenToClient(s_3dwnd, &ptCursor);

	posx = ptCursor.x;
	posy = ptCursor.y;

	SetWindowPos(hDlgWnd, hWndInsertAfter, posx, posy, 0, 0, SWP_NOSIZE);

	return 0;
}




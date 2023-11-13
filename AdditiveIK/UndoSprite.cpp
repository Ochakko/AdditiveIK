#include "stdafx.h"

//#include <vector>
//#include <map>
#include <string>

#include <Coef.h>
#include <GlobalVar.h>
//#include <ChaVecCalc.h>
#include <MySprite.h>

#include "UndoSprite.h"


#define DBGH
#include <dbg.h>

#include <crtdbg.h>


CUndoSprite::CUndoSprite()
{
	InitParams();
}
CUndoSprite::~CUndoSprite()
{
	DestroyObjs();
}
void CUndoSprite::InitParams()
{
	//bool m_createdflag;
	//CMySprite* m_sprite[12];
	//int m_indexR[4];
	//int m_indexW[4];
	//POINT m_pointR[4];
	//POINT m_pointW[4];

	m_createdflag = false;
	ZeroMemory(m_sprite, sizeof(CMySprite*) * 12);
	ZeroMemory(m_indexR, sizeof(int) * 4);
	ZeroMemory(m_indexW, sizeof(int) * 4);
	ZeroMemory(m_pointR, sizeof(POINT) * 4);
	ZeroMemory(m_pointW, sizeof(POINT) * 4);

	int posno;
	for (posno = 0; posno < 4; posno++) {
		m_posR[posno] = ChaVector3(0.0f, 0.0f, 0.0f);
		m_posW[posno] = ChaVector3(0.0f, 0.0f, 0.0f);
	}
	m_size = ChaVector2(1.0f, 1.0f);

}
void CUndoSprite::DestroyObjs()
{
	DestroySprites();
	InitParams();
}

int CUndoSprite::CreateSprites(ID3D12Device* pdev, WCHAR* mpath)
{

	DestroySprites();

	if (!pdev || !mpath) {
		_ASSERT(0);
		return 1;
	}
	
	m_sprite[0] = new CMySprite(pdev);
	_ASSERT(m_sprite[0]);
	CallF(m_sprite[0]->Create(mpath, L"0.png", 0, 0), return 1);

	m_sprite[1] = new CMySprite(pdev);
	_ASSERT(m_sprite[1]);
	CallF(m_sprite[1]->Create(mpath, L"1.png", 0, 0), return 1);

	m_sprite[2] = new CMySprite(pdev);
	_ASSERT(m_sprite[2]);
	CallF(m_sprite[2]->Create(mpath, L"2.png", 0, 0), return 1);

	m_sprite[3] = new CMySprite(pdev);
	_ASSERT(m_sprite[3]);
	CallF(m_sprite[3]->Create(mpath, L"3.png", 0, 0), return 1);

	m_sprite[4] = new CMySprite(pdev);
	_ASSERT(m_sprite[4]);
	CallF(m_sprite[4]->Create(mpath, L"4.png", 0, 0), return 1);

	m_sprite[5] = new CMySprite(pdev);
	_ASSERT(m_sprite[5]);
	CallF(m_sprite[5]->Create(mpath, L"5.png", 0, 0), return 1);

	m_sprite[6] = new CMySprite(pdev);
	_ASSERT(m_sprite[6]);
	CallF(m_sprite[6]->Create(mpath, L"6.png", 0, 0), return 1);

	m_sprite[7] = new CMySprite(pdev);
	_ASSERT(m_sprite[7]);
	CallF(m_sprite[7]->Create(mpath, L"7.png", 0, 0), return 1);

	m_sprite[8] = new CMySprite(pdev);
	_ASSERT(m_sprite[8]);
	CallF(m_sprite[8]->Create(mpath, L"8.png", 0, 0), return 1);

	m_sprite[9] = new CMySprite(pdev);
	_ASSERT(m_sprite[9]);
	CallF(m_sprite[9]->Create(mpath, L"9.png", 0, 0), return 1);

	m_sprite[10] = new CMySprite(pdev);
	_ASSERT(m_sprite[10]);
	CallF(m_sprite[10]->Create(mpath, L"R_32x32.png", 0, 0), return 1);

	m_sprite[11] = new CMySprite(pdev);
	_ASSERT(m_sprite[11]);
	CallF(m_sprite[11]->Create(mpath, L"W_32x32.png", 0, 0), return 1);




	m_createdflag = true;
	return 0;
}
void CUndoSprite::DestroySprites()
{
	m_createdflag = false;

	int spriteno;
	for (spriteno = 0; spriteno < 12; spriteno++) {
		CMySprite* cursp = m_sprite[spriteno];
		if (cursp) {
			delete cursp;
		}
	}
}
int CUndoSprite::SetParams(int srcmainwidth, int srcmainheight)
{
	if (!m_createdflag) {
		_ASSERT(0);
		return 1;
	}

	//float spawidth = 32.0f;
	//float spaheight = 32.0f;
	float spawidth = 16.0f;
	float spaheight = 16.0f;
	int spashift = 1;

	//m_pointR[0].x = 175;
	m_pointR[0].x = 180;
	m_pointR[0].y = 40;
	{
		int spacnt;
		for (spacnt = 1; spacnt < 4; spacnt++) {
			m_pointR[spacnt].x = m_pointR[spacnt - 1].x + (int)spawidth + spashift;
			m_pointR[spacnt].y = m_pointR[0].y;
		}
	}

	m_pointW[0].x = m_pointR[3].x + (int)spawidth + spashift + 5;
	m_pointW[0].y = 40;
	{
		int spacnt;
		for (spacnt = 1; spacnt < 4; spacnt++) {
			m_pointW[spacnt].x = m_pointW[spacnt - 1].x + (int)spawidth + spashift;
			m_pointW[spacnt].y = m_pointR[0].y;
		}
	}


	m_size = ChaVector2(spawidth / (float)srcmainwidth * 2.0f, spawidth / (float)srcmainheight * 2.0f);


	{
		int spacnt;
		for (spacnt = 0; spacnt < 4; spacnt++) {
			m_posR[spacnt].x = (float)(m_pointR[spacnt].x) / ((float)srcmainwidth / 2.0f) - 1.0f;
			m_posR[spacnt].y = -((float)(m_pointR[spacnt].y) / ((float)srcmainheight / 2.0f) - 1.0f);
			m_posR[spacnt].z = 0.0f;

			m_posW[spacnt].x = (float)(m_pointW[spacnt].x) / ((float)srcmainwidth / 2.0f) - 1.0f;
			m_posW[spacnt].y = -((float)(m_pointW[spacnt].y) / ((float)srcmainheight / 2.0f) - 1.0f);
			m_posW[spacnt].z = 0.0f;


			//ChaVector2 dispsize = ChaVector2(spawidth / (float)s_mainwidth * 2.0f, spawidth / (float)s_mainheight * 2.0f);
			//if (m_sprite[spacnt]) {
			//	CallF(s_spundo[spacnt].sprite->SetPos(disppos), return 1);
			//	CallF(s_spundo[spacnt].sprite->SetSize(dispsize), return 1);
			//}
			//else {
			//	_ASSERT(0);
			//}
		}
	}




	return 0;


}
int CUndoSprite::Render(RenderContext* pd3dImmediateContext, int undoR, int undoW)
{
	if (!pd3dImmediateContext) {
		_ASSERT(0);
		return 1;
	}
	if ((undoR < 0) || (undoR >= UNDOMAX)) {
		_ASSERT(0);
		return 1;
	}
	if ((undoW < 0) || (undoW >= UNDOMAX)) {
		_ASSERT(0);
		return 1;
	}

	if (!m_createdflag) {
		_ASSERT(0);
		return 1;
	}


	m_indexR[0] = 10;//R_32x32.png
	m_indexW[0] = 11;//W_32x32.png

	int decindex;
	int curR = undoR;
	int curW = undoW;
	for (decindex = 0; decindex < 3; decindex++) {
		if ((curR < 0) || (curR >= UNDOMAX)) {
			_ASSERT(0);
			return 1;
		}
		if ((curW < 0) || (curW >= UNDOMAX)) {
			_ASSERT(0);
			return 1;
		}

		int divval = 1;
		int divno;
		for (divno = (2 - decindex); divno > 0; divno--) {
			divval *= 10;
		}

		int tmpindexR = curR / divval;
		if ((tmpindexR >= 0) && (tmpindexR <= 9)) {
			m_indexR[decindex + 1] = curR / divval;//int
		}
		else {
			_ASSERT(0);
			m_indexR[decindex + 1] = 0;
		}
		curR = curR - (m_indexR[decindex + 1] * divval);

		
		int tmpindexW = curW / divval;
		if ((tmpindexW >= 0) && (tmpindexW <= 9)) {
			m_indexW[decindex + 1] = curW / divval;//int
		}
		else {
			_ASSERT(0);
			m_indexW[decindex + 1] = 0;//int
		}
		curW = curW - (m_indexW[decindex + 1] * divval);
	}

	int spacnt;
	for (spacnt = 0; spacnt < 4; spacnt++) {
		CMySprite* spriteR;
		CMySprite* spriteW;

		if ((m_indexR[spacnt] >= 0) && (m_indexR[spacnt] < 12)) {
			spriteR = m_sprite[m_indexR[spacnt]];
			if (spriteR) {
				CallF(spriteR->SetPos(m_posR[spacnt]), return 1);
				CallF(spriteR->SetSize(m_size), return 1);
				spriteR->OnRender(pd3dImmediateContext);
			}
			else {
				_ASSERT(0);
				return 1;
			}
		}
		else {
			_ASSERT(0);
			return 1;
		}

		if ((m_indexW[spacnt] >= 0) && (m_indexW[spacnt] < 12)) {
			spriteW = m_sprite[m_indexW[spacnt]];
			if (spriteW) {
				CallF(spriteW->SetPos(m_posW[spacnt]), return 1);
				CallF(spriteW->SetSize(m_size), return 1);
				spriteW->OnRender(pd3dImmediateContext);
			}
			else {
				_ASSERT(0);
				return 1;
			}
		}
		else {
			_ASSERT(0);
			return 1;
		}

	}


	return 0;
}









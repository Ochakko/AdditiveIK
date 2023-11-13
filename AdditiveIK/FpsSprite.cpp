#include "stdafx.h"

//#include <vector>
//#include <map>
#include <string>

#include <Coef.h>
#include <GlobalVar.h>
//#include <ChaVecCalc.h>
#include <MySprite.h>

#include "FpsSprite.h"


#define DBGH
#include <dbg.h>

#include <crtdbg.h>


CFpsSprite::CFpsSprite()
{
	InitParams();
}
CFpsSprite::~CFpsSprite()
{
	DestroyObjs();
}
void CFpsSprite::InitParams()
{
	m_createdflag = false;
	ZeroMemory(m_sprite, sizeof(CMySprite*) * 11);
	ZeroMemory(m_index, sizeof(int) * 6);
	ZeroMemory(m_point, sizeof(POINT) * 6);

	int posno;
	for (posno = 0; posno < 6; posno++) {
		m_pos[posno] = ChaVector3(0.0f, 0.0f, 0.0f);
	}
	m_size_label = ChaVector2(1.0f, 1.0f);
	m_size_num = ChaVector2(1.0f, 1.0f);

}
void CFpsSprite::DestroyObjs()
{
	DestroySprites();
	InitParams();
}

int CFpsSprite::CreateSprites(ID3D12Device* pdev, WCHAR* mpath)
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
	CallF(m_sprite[10]->Create(mpath, L"fps_64x32.png", 0, 0), return 1);



	m_createdflag = true;
	return 0;
}
void CFpsSprite::DestroySprites()
{
	m_createdflag = false;

	int spriteno;
	for (spriteno = 0; spriteno < 11; spriteno++) {
		CMySprite* cursp = m_sprite[spriteno];
		if (cursp) {
			delete cursp;
		}
	}
}
int CFpsSprite::SetParams(int srcmainwidth, int srcmainheight)
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
	m_point[0].x = 180 - (int)spawidth - spashift;
	m_point[0].y = 10;

	m_point[1].x = m_point[0].x + (int)spawidth * 2 + spashift;
	m_point[1].y = m_point[0].y;

	{
		int spacnt;
		for (spacnt = 2; spacnt < 6; spacnt++) {
			m_point[spacnt].x = m_point[spacnt - 1].x + (int)spawidth + spashift;
			m_point[spacnt].y = m_point[0].y;
		}
	}

	m_size_label = ChaVector2((spawidth * 2) / (float)srcmainwidth * 2.0f, spawidth / (float)srcmainheight * 2.0f);
	m_size_num = ChaVector2(spawidth / (float)srcmainwidth * 2.0f, spawidth / (float)srcmainheight * 2.0f);


	{
		int spacnt;
		for (spacnt = 0; spacnt < 6; spacnt++) {
			m_pos[spacnt].x = (float)(m_point[spacnt].x) / ((float)srcmainwidth / 2.0f) - 1.0f;
			m_pos[spacnt].y = -((float)(m_point[spacnt].y) / ((float)srcmainheight / 2.0f) - 1.0f);
			m_pos[spacnt].z = 0.0f;

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
int CFpsSprite::Render(RenderContext* pd3dImmediateContext, int srcfps)
{
#define FPSDISPMAX	99999


	if (!pd3dImmediateContext) {
		_ASSERT(0);
		return 1;
	}
	if ((srcfps < 0) || (srcfps >= FPSDISPMAX)) {
		_ASSERT(0);
		return 1;
	}

	if (!m_createdflag) {
		_ASSERT(0);
		return 1;
	}


	m_index[0] = 10;//fps_64x32.png

	int decindex;
	int curval = srcfps;
	for (decindex = 0; decindex < 5; decindex++) {

		if ((curval < 0) || (curval >= FPSDISPMAX)) {
			_ASSERT(0);
			return 1;
		}

		int divval = 1;
		int divno;
		for (divno = (4 - decindex); divno > 0; divno--) {
			divval *= 10;
		}

		int tmpindex = curval / divval;
		if ((tmpindex >= 0) && (tmpindex <= 9)) {
			m_index[decindex + 1] = curval / divval;//int
		}
		else {
			_ASSERT(0);
			m_index[decindex + 1] = 0;
		}
		curval = curval - (m_index[decindex + 1] * divval);
	}

	int spacnt;
	for (spacnt = 0; spacnt < 6; spacnt++) {
		CMySprite* sprite;

		if ((m_index[spacnt] >= 0) && (m_index[spacnt] < 11)) {
			sprite = m_sprite[m_index[spacnt]];
			if (sprite) {
				CallF(sprite->SetPos(m_pos[spacnt]), return 1);
				if (spacnt == 0) {
					CallF(sprite->SetSize(m_size_label), return 1);
				}
				else {
					CallF(sprite->SetSize(m_size_num), return 1);
				}
				sprite->OnRender(pd3dImmediateContext);
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









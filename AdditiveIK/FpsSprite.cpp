#include "stdafx.h"

//#include <vector>
//#include <map>
#include <string>

#include <Coef.h>
#include <GlobalVar.h>

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
	//ZeroMemory(m_sprite, sizeof(CMySprite*) * 11);
	ZeroMemory(m_texture, sizeof(Texture*) * 11);
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

int CFpsSprite::CreateSprites(WCHAR* mpath)
{

	DestroySprites();

	if (!mpath) {
		_ASSERT(0);
		return 1;
	}


	WCHAR filename[11][MAX_PATH] = {
		L"MameMedia\\0.png",
		L"MameMedia\\1.png",
		L"MameMedia\\2.png",
		L"MameMedia\\3.png",
		L"MameMedia\\4.png",
		L"MameMedia\\5.png",
		L"MameMedia\\6.png",
		L"MameMedia\\7.png",
		L"MameMedia\\8.png",
		L"MameMedia\\9.png",
		L"MameMedia\\fps_64x32.png"
	};

	char cpath[MAX_PATH];
	char cfxpath[MAX_PATH];
	//char cfilepath[MAX_PATH];
	char cbasedir[MAX_PATH] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, g_basedir, -1, cbasedir, MAX_PATH, NULL, NULL);
	strcpy_s(cpath, MAX_PATH, cbasedir);
	char* clasten = 0;
	char* clast2en = 0;
	clasten = strrchr(cpath, '\\');
	if (!clasten) {
		_ASSERT(0);
		PostQuitMessage(1);
		return S_FALSE;
	}
	*clasten = 0;
	clast2en = strrchr(cpath, '\\');
	if (!clast2en) {
		_ASSERT(0);
		PostQuitMessage(1);
		return S_FALSE;
	}
	*clast2en = 0;
	strcat_s(cpath, MAX_PATH, "\\Media\\");
	SpriteInitData spriteinitdata;
	strcpy_s(cfxpath, MAX_PATH, cpath);
	strcat_s(cfxpath, MAX_PATH, "Shader\\preset\\InstancedSprite.fx");
	spriteinitdata.m_fxFilePath = cfxpath;
	spriteinitdata.m_width = 256;//仮　ファイルから読込時は上書きされる
	spriteinitdata.m_height = 256;//仮　ファイルから読込時は上書きされる
	spriteinitdata.m_alphaBlendMode = AlphaBlendMode_Trans;

	bool screenvertexflag = true;//!!!!!!!!!!!!


	int spno;
	for (spno = 0; spno < 11; spno++) {
		WCHAR filepath[MAX_PATH];
		wcscpy_s(filepath, MAX_PATH, mpath);
		wcscat_s(filepath, MAX_PATH, filename[spno]);
		m_texture[spno] = new Texture();
		m_texture[spno]->InitFromWICFile(filepath);
		spriteinitdata.m_textures[0] = m_texture[spno];
		m_sprite[spno].Init(spriteinitdata);
	}

	m_createdflag = true;//SetParamsよりは前でセット

	SetParams();

	return 0;
}
void CFpsSprite::DestroySprites()
{
	m_createdflag = false;

	int spriteno;
	for (spriteno = 0; spriteno < 11; spriteno++) {
		Texture* curtex = m_texture[spriteno];
		if (curtex) {
			delete curtex;
			m_texture[spriteno] = nullptr;
		}
	}
}
int CFpsSprite::SetParams()
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

	m_size_label = ChaVector2((spawidth * 2), spawidth);
	m_size_num = ChaVector2(spawidth, spawidth);


	{
		int spacnt;
		for (spacnt = 0; spacnt < 6; spacnt++) {
			m_pos[spacnt].x = (float)(m_point[spacnt].x);
			m_pos[spacnt].y = (float)(m_point[spacnt].y);
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
int CFpsSprite::DrawScreen(RenderContext& rc, int srcfps)
{
#define FPSDISPMAX	99999

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

	int instanceno[11];
	ZeroMemory(instanceno, sizeof(int) * 11);

	int spacnt;
	for (spacnt = 0; spacnt < 6; spacnt++) {
		//CMySprite* sprite;

		if ((m_index[spacnt] >= 0) && (m_index[spacnt] < 11)) {
			//sprite = m_sprite[m_index[spacnt]];

			ChaVector2 cursize;
			if (spacnt == 0) {
				cursize = m_size_label;
			}
			else {
				cursize = m_size_num;
			}
			m_sprite[m_index[spacnt]].UpdateScreen(instanceno[m_index[spacnt]], m_pos[spacnt], cursize);
			//m_sprite[m_index[spacnt]].DrawScreen(rc);

			(instanceno[m_index[spacnt]])++;
		}
		else {
			_ASSERT(0);
			return 1;
		}
	}

	int spno;
	for (spno = 0; spno < 11; spno++) {
		m_sprite[spno].DrawScreen(rc);
	}

	return 0;
}









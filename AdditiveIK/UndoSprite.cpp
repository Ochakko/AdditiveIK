#include "stdafx.h"

//#include <vector>
//#include <map>
#include <string>

#include <Coef.h>
#include <GlobalVar.h>

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
	m_createdflag = false;
	//ZeroMemory(m_sprite, sizeof(CMySprite*) * 12);
	ZeroMemory(m_texture, sizeof(Texture*) * 12);
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

int CUndoSprite::CreateSprites(WCHAR* mpath)
{

	DestroySprites();

	if (!mpath) {
		_ASSERT(0);
		return 1;
	}
	

	WCHAR filename[12][MAX_PATH] = {
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
		L"MameMedia\\R_32x32.png",
		L"MameMedia\\W_32x32.png"
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
	for (spno = 0; spno < 12; spno++) {
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
void CUndoSprite::DestroySprites()
{
	m_createdflag = false;

	int spriteno;
	for (spriteno = 0; spriteno < 12; spriteno++) {
		m_sprite[spriteno].DestroyObjs();

		Texture* curtex = m_texture[spriteno];
		if (curtex) {
			delete curtex;
			m_texture[spriteno] = nullptr;
		}
	}
}

int CUndoSprite::SetParams()
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


	m_size = ChaVector2(spawidth, spawidth);


	{
		int spacnt;
		for (spacnt = 0; spacnt < 4; spacnt++) {
			m_posR[spacnt].x = (float)(m_pointR[spacnt].x);
			m_posR[spacnt].y = (float)(m_pointR[spacnt].y);
			m_posR[spacnt].z = 0.0f;

			m_posW[spacnt].x = (float)(m_pointW[spacnt].x);
			m_posW[spacnt].y = (float)(m_pointW[spacnt].y);
			m_posW[spacnt].z = 0.0f;
		}
	}

	return 0;


}
int CUndoSprite::DrawScreen(RenderContext* rc, int undoR, int undoW)
{
	if (!rc) {
		_ASSERT(0);
		return 1;
	}

	if ((undoR < 0) || (undoR >= UNDOMAX)) {
		//_ASSERT(0);
		return 1;
	}
	if ((undoW < 0) || (undoW >= UNDOMAX)) {
		//_ASSERT(0);
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


	int instanceno[12];
	ZeroMemory(instanceno, sizeof(int) * 12);

	ChaVector4 colmult = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);

	int spacnt;
	for (spacnt = 0; spacnt < 4; spacnt++) {

		if ((m_indexR[spacnt] >= 0) && (m_indexR[spacnt] < 12)) {
			m_sprite[m_indexR[spacnt]].UpdateScreen(instanceno[m_indexR[spacnt]], m_posR[spacnt], m_size, colmult);
			//m_sprite[m_indexR[spacnt]].DrawScreen(rc);

			(instanceno[m_indexR[spacnt]])++;
		}
		else {
			_ASSERT(0);
			return 1;
		}

		if ((m_indexW[spacnt] >= 0) && (m_indexW[spacnt] < 12)) {
			m_sprite[m_indexW[spacnt]].UpdateScreen(instanceno[m_indexW[spacnt]], m_posW[spacnt], m_size, colmult);
			//m_sprite[m_indexW[spacnt]].DrawScreen(rc);

			(instanceno[m_indexW[spacnt]])++;
		}
		else {
			_ASSERT(0);
			return 1;
		}
	}


	int spno;
	for (spno = 0; spno < 12; spno++) {
		m_sprite[spno].DrawScreen(rc);
	}


	return 0;
}









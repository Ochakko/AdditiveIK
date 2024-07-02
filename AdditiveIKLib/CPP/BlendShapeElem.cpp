#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#include <BlendShapeElem.h>

#include <Model.h>
#include <MQOObject.h>
#include <MQOMaterial.h>

#include <GlobalVar.h>

#define DBGH
#include <dbg.h>

using namespace std;

CBlendShapeElem::CBlendShapeElem() 
{
	Init();
}
CBlendShapeElem::~CBlendShapeElem() 
{
	Init();
}
int CBlendShapeElem::SetBlendShape(CModel* srcmodel, CMQOObject* srcmqoobj, int srcchannelindex) 
{
	if (!srcmodel || !srcmqoobj || (srcchannelindex < 0)) {
		_ASSERT(0);
		validflag = false;
		return 1;
	}
	model = srcmodel;
	mqoobj = srcmqoobj;
	channelindex = srcchannelindex;

	int error0 = 0;
	std::string strshapename = mqoobj->GetShapeName(channelindex, &error0);
	if (error0 != 0) {
		_ASSERT(0);
		validflag = false;
		return 1;
	}

	char shapename[256] = { 0 };
	strcpy_s(shapename, 256, strshapename.c_str());
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, shapename, -1, targetname, 256);

	validflag = true;
	return 0;
}

void CBlendShapeElem::Init() 
{
	validflag = false;
	model = nullptr;
	mqoobj = nullptr;
	ZeroMemory(targetname, sizeof(WCHAR) * 256);
	channelindex = -1;
}

CBlendShapeElem CBlendShapeElem::operator= (CBlendShapeElem m) 
{
	this->validflag = m.validflag;
	this->model = m.model;
	this->mqoobj = m.mqoobj;
	if (m.targetname[0] != 0L) {
		wcscpy_s(this->targetname, 256, m.targetname);
	}
	else {
		ZeroMemory(this->targetname, sizeof(WCHAR) * 256);
	}
	this->channelindex = m.channelindex;

	return *this;
}



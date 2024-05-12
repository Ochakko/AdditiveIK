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

#include <GrassElem.h>
#include <ChaScene.h>
#include <Model.h>
#include <mqomaterial.h>

#include <GlobalVar.h>

#define DBGH
#include <dbg.h>

using namespace std;


CGrassElem::CGrassElem(CModel* srcmodel)
{
	InitParams();
	m_grass = srcmodel;
	m_grass->SetGrassFlag(true);
	m_grass->SetInstancingNum(GRASSINDEXMAX);
	_ASSERT(m_grass);
}

CGrassElem::~CGrassElem()
{
	DestroyObjs();
}

void CGrassElem::InitParams()
{
	m_grass = nullptr;
	ClearGrassMat();

}

void CGrassElem::DestroyObjs()
{
	InitParams();
}

int CGrassElem::SetInstancingParams(ChaMatrix matVP)
{
	if (!GetGrass()) {
		_ASSERT(0);
		return 1;
	}

	GetGrass()->ResetInstancingParams();

	int grassnum = GetGrassNum();
	int grassindex;
	for (grassindex = 0; grassindex < grassnum; grassindex++) {
		if (grassindex < GRASSINDEXMAX) {
			ChaVector4 grassmaterial = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);
			GetGrass()->SetInstancingParams(grassindex, GetGrassMat(grassindex), matVP, grassmaterial);
		}
	}

	return 0;
}

int CGrassElem::RenderInstancingModel(ChaScene* srcchascene)
{
	if (!GetGrass()) {
		_ASSERT(0);
		return 1;
	}

	int grassnum = GetGrassNum();
	if (grassnum <= 0) {
		return 0;
	}

	int lightflag = 1;
	ChaVector4 diffusemult = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);
	bool forcewithalpha = true;
	int btflag = 0;
	bool zcmpalways = false;
	bool zenable = true;

	if (grassnum > 0) {
		srcchascene->RenderInstancingModel(GetGrass(), forcewithalpha, lightflag, diffusemult, btflag,
			zcmpalways, zenable,
			RENDERKIND_INSTANCING_TRIANGLE);
	}

	return 0;
}

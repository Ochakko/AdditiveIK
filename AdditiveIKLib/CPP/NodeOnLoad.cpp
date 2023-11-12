#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#include <GlobalVar.h>


#include <NodeOnLoad.h>

#include <Model.h>
#include <mqomaterial.h>
#include <mqoobject.h>

#define DBGH
#include <dbg.h>

#include <Bone.h>
#include <MQOFace.h>
#include <BtObject.h>

#include <InfScope.h>
#include <MotionPoint.h>

#include <ChaVecCalc.h>

#include <RigidElem.h>
#include <EngName.h>
//#include <BoneProp.h>

using namespace std;


CNodeOnLoad::CNodeOnLoad(FbxNode* srcnode)
{
	InitParams();
	SetNode(srcnode);
}
CNodeOnLoad::~CNodeOnLoad()
{
	DestroyObjes();
}

void CNodeOnLoad::InitParams()
{
	m_type = NOL_NONE;
	m_pnode = 0;
	m_bone = 0;
	m_mqoobject = 0;
	m_bindmat.SetIdentity();
	m_childonload.clear();
}
void CNodeOnLoad::DestroyObjes()
{
	InitParams();
}



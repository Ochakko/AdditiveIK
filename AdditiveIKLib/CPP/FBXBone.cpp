#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <FBXBone.h>

#define	DBGH
#include <dbg.h>

#include <Bone.h>

#include <crtdbg.h>


CFBXBone::CFBXBone()
{
	InitParams();
}

CFBXBone::~CFBXBone()
{
	DestroyObjs();
}

int CFBXBone::InitParams()
{
	m_type = FB_NORMAL;
	m_bone = 0;
	m_pbe = 0;
	m_skelnode = 0;
	m_bunkinum = 0;
	m_axisq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	m_parent = 0;
	m_child = 0;
	m_brother = 0;

	return 0;
}

int CFBXBone::DestroyObjs()
{

	return 0;
}

int CFBXBone::AddChild( CFBXBone* childptr )
{

	if( !m_child ){
		m_child = childptr;
		m_child->m_parent = this;
	}else{
		CFBXBone* broptr = m_child;
		if (broptr){
			while (broptr->m_brother){
				broptr = broptr->m_brother;
			}
			broptr->m_brother = childptr;
			broptr->m_brother->m_parent = this;
		}
	}

	return  0;
}


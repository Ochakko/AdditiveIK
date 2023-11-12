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

#include <GlobalVar.h>

#include <RigidElem.h>
//#include <Bone.h>
//#include <quaternion.h>

#define DBGH
#include <dbg.h>

using namespace std;
/*
extern float g_kinit;
extern float g_initmass;
extern float g_l_dmp;
extern float g_a_dmp;
extern float g_initcuslk;
extern float g_initcusak;
*/


static std::vector<CRigidElem*> s_rigidelempool;//allocate MPPOOLBLKLEN motoinpoints at onse and pool 


CRigidElem::CRigidElem()
{
	InitParams();
}
CRigidElem::~CRigidElem()
{
	DestroyObjs();
}

int CRigidElem::InitParams()
{
	//not use at allocated
	m_useflag = 0;//0: not use, 1: in use
	m_indexofpool = 0;
	m_allocheadflag = 0;//1: head pointer at allocated


	m_forbidrotflag = 0;

	m_dampanim_l = 0.0f;
	m_dampanim_a = 0.0f;

	m_boneleng = 0.0f;
	m_coltype = COL_CAPSULE_INDEX;
	m_skipflag = 1;

	m_sphrate = 0.6f;
	m_boxzrate = 0.6f;

	m_cylileng = 0.0f;
	m_sphr = 0.0f;
	m_boxz = 0.0f;

	//m_impulse = ChaVector3( 0.0f, 0.0f, 0.0f );
	
	m_l_kindex = 2;
	m_a_kindex = 2;

	m_l_damping = g_l_dmp;
	m_a_damping = g_a_dmp;

	m_mass = g_initmass;

	m_bone = 0;
	m_endbone = 0;

	m_cus_lk = g_initcuslk;
	m_cus_ak = g_initcusak;


	m_groupid = 2;
	m_coliids.clear();
	m_coliids.push_back( 1 );
	m_myselfflag = 1;

	m_restitution = 0.5f;
	m_friction = 0.5f;

	m_btg = -0.09f;

	return 0;
}
int CRigidElem::DestroyObjs()
{
	m_coliids.clear();
	return 0;
}

int CRigidElem::GetColiID()
{
	int retid = 0;

	int idnum = (int)m_coliids.size();
	int ino;
	for( ino = 0; ino < idnum; ino++ ){
		int curid = 1 << (m_coliids[ ino ] - 1);
		retid |= curid;
	}

	if( m_myselfflag == 1 ){
		int curid = 1 << (m_groupid - 1);
		retid |= curid;
	}

	return retid;
}

float CRigidElem::GetBoneLeng()
{
	if (!m_bone || !m_endbone){
		m_boneleng = 0.0f;
		return 0.0f;
	}

	ChaVector3 centerA, parentposA, childposA, aftparentposA, aftchildposA;
	parentposA = m_bone->GetJointFPos();
	ChaMatrix tmpim = m_bone->GetInitMat();
	ChaVector3TransformCoord(&aftparentposA, &parentposA, &tmpim);
	childposA = m_endbone->GetJointFPos();
	ChaMatrix tmpendim = m_endbone->GetInitMat();
	ChaVector3TransformCoord(&aftchildposA, &childposA, &tmpendim);
	ChaVector3 diffA = childposA - parentposA;
	m_boneleng = (float)ChaVector3LengthDbl(&diffA);

	return m_boneleng;

}

ChaMatrix CRigidElem::GetCapsulematForColiShape(bool limitdegflag, int calczeroframe, int multworld)//default of multworld = 1
{
	//######################################################################################
	//2023/01/18
	//RigidBody�̌`��̕\���p�̎p���v�Z��dir2xflag = true�ōs���@(���[�V�����Ƃ͎����قȂ�)
	//######################################################################################

	ChaMatrix retmat;
	retmat.SetIdentity();

	if (m_endbone && m_endbone->GetParent(false)) {
		bool dir2xflag = true;
		m_endbone->GetParent(false)->CalcAxisMatX_RigidBody(limitdegflag, dir2xflag, 0, m_endbone, &retmat, calczeroframe);
	}
	else {
		retmat.SetIdentity();
	}

	return retmat;
}





//static func
CRigidElem* CRigidElem::GetNewRigidElem()
{
	static int s_befheadno = -1;
	static int s_befelemno = -1;

	int curpoollen;
	curpoollen = (int)s_rigidelempool.size();


	//if ((s_befheadno != (s_rigidelempool.size() - 1)) || (s_befelemno != (REPOOLBLKLEN - 1))) {//�O�񃊃��[�X�����|�C���^���Ō���ł͂Ȃ��ꍇ

	//�O�񃊃��[�X�����|�C���^�̎��̃����o�[���`�F�b�N���Ė��g�p�������烊���[�X
		int chkheadno;
		chkheadno = s_befheadno;
		int chkelemno;
		chkelemno = s_befelemno + 1;
		//if ((chkheadno >= 0) && (chkheadno >= curpoollen) && (chkelemno >= REPOOLBLKLEN)) {
		if ((chkheadno >= 0) && (chkheadno < (curpoollen - 1)) && (chkelemno >= REPOOLBLKLEN)) {//2021/08/21
			chkelemno = 0;
			chkheadno++;
		}
		if ((chkheadno >= 0) && (chkheadno < curpoollen) && (chkelemno >= 0) && (chkelemno < REPOOLBLKLEN)) {
			CRigidElem* currigidelemhead = s_rigidelempool[chkheadno];
			if (currigidelemhead) {
				CRigidElem* chkrigidelem;
				chkrigidelem = currigidelemhead + chkelemno;
				if (chkrigidelem) {
					if (chkrigidelem->GetUseFlag() == 0) {
						int saveindex = chkrigidelem->GetIndexOfPool();
						int saveallochead = chkrigidelem->IsAllocHead();
						chkrigidelem->InitParams();
						chkrigidelem->SetUseFlag(1);
						chkrigidelem->SetIndexOfPool(saveindex);
						chkrigidelem->SetIsAllocHead(saveallochead);

						s_befheadno = chkheadno;
						s_befelemno = chkelemno;
						return chkrigidelem;
					}
				}
			}
		}

		//if ((chkheadno >= 0) && (chkheadno < curpoollen)) {
			//�v�[����擪���猟�����Ė��g�p���݂���΂���������[�X
		int rigidelemno;
		for (rigidelemno = 0; rigidelemno < curpoollen; rigidelemno++) {
			CRigidElem* currigidelemhead = s_rigidelempool[rigidelemno];
			if (currigidelemhead) {
				int elemno;
				for (elemno = 0; elemno < REPOOLBLKLEN; elemno++) {
					CRigidElem* currigidelem;
					currigidelem = currigidelemhead + elemno;
					if (currigidelem->GetUseFlag() == 0) {
						int saveindex = currigidelem->GetIndexOfPool();
						int saveallochead = currigidelem->IsAllocHead();
						currigidelem->InitParams();
						currigidelem->SetUseFlag(1);
						currigidelem->SetIndexOfPool(saveindex);
						currigidelem->SetIsAllocHead(saveallochead);

						s_befheadno = rigidelemno;
						s_befelemno = elemno;
						return currigidelem;
					}
				}
			}
		}
		//}
	//}

	//���g�prigidelem��pool�ɖ��������ꍇ�A�A���P�[�g���ăA���P�[�g�����擪�̃|�C���^�������[�X
	CRigidElem* allocrigidelem;
	allocrigidelem = new CRigidElem[REPOOLBLKLEN];
	if (!allocrigidelem) {
		_ASSERT(0);

		s_befheadno = -1;
		s_befelemno = -1;

		return 0;
	}
	int allocno;
	for (allocno = 0; allocno < REPOOLBLKLEN; allocno++) {
		CRigidElem* curallocrigidelem = allocrigidelem + allocno;
		if (curallocrigidelem) {
			//int indexofpool = curpoollen + allocno;
			int indexofpool = curpoollen;//pool[indexofpool] 2021/08/19
			curallocrigidelem->InitParams();
			curallocrigidelem->SetUseFlag(0);
			curallocrigidelem->SetIndexOfPool(indexofpool);

			if (allocno == 0) {
				curallocrigidelem->SetIsAllocHead(1);
			}
			else {
				curallocrigidelem->SetIsAllocHead(0);
			}
		}
		else {
			_ASSERT(0);

			s_befheadno = -1;
			s_befelemno = -1;

			return 0;
		}
	}
	s_rigidelempool.push_back(allocrigidelem);//allocate block(�A���P�[�g���̐擪�|�C���^)���i�[

	allocrigidelem->SetUseFlag(1);


	s_befheadno = (int)s_rigidelempool.size() - 1;
	if (s_befheadno < 0) {
		s_befheadno = 0;
	}
	s_befelemno = 0;

	return allocrigidelem;
}

//static func
void CRigidElem::InvalidateRigidElem(CRigidElem* srcrigidelem)
{
	if (!srcrigidelem) {
		_ASSERT(0);
		return;
	}

	int saveindex = srcrigidelem->GetIndexOfPool();
	int saveallochead = srcrigidelem->IsAllocHead();

	srcrigidelem->DestroyObjs();

	srcrigidelem->InitParams();
	srcrigidelem->SetUseFlag(0);
	srcrigidelem->SetIsAllocHead(saveallochead);
	srcrigidelem->SetIndexOfPool(saveindex);
}

//static func
void CRigidElem::InitRigidElems()
{
	s_rigidelempool.clear();
}

//static func
void CRigidElem::DestroyRigidElems() {
	int rigidelemallocnum = (int)s_rigidelempool.size();
	int rigidelemno;
	for (rigidelemno = 0; rigidelemno < rigidelemallocnum; rigidelemno++) {
		CRigidElem* delrigidelem;
		delrigidelem = s_rigidelempool[rigidelemno];
		//if (delrigidelem && (delrigidelem->IsAllocHead() == 1)) {
		if (delrigidelem) {
			delete[] delrigidelem;
		}
	}
	s_rigidelempool.clear();
}

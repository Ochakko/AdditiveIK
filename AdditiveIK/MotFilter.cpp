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


#define DBGH
#include <dbg.h>

#include <Model.h>
#include <MotionPoint.h>
#include <Bone.h>
//#include <BoneProp.h>

#include "FilterType.h"
#include "FilterDlg.h"
#include "MotFilter.h"

using namespace std;


//extern bool g_underIKRot;
//extern bool g_edgesmp;

CMotFilter::CMotFilter()
{
	InitParams();
}

CMotFilter::~CMotFilter()
{
	DestroyObjs();
}

void CMotFilter::InitParams()
{
	m_filtertype = 0;
	m_filtersize = 0;

	m_eul = 0;
	m_smootheul = 0;
	m_tra = 0;
	m_smoothtra = 0;

}

void CMotFilter::DestroyObjs()
{
	if (m_eul){
		delete[] m_eul;
	}

	if (m_smootheul){
		delete[] m_smootheul;
	}

	if (m_tra){
		delete[] m_tra;
	}

	if (m_smoothtra){
		delete[] m_smoothtra;
	}

	m_eul = 0;
	m_smootheul = 0;
	m_tra = 0;
	m_smoothtra = 0;

}

int CMotFilter::GetFilterType()
{
	CFilterDlg dlg;
	int ret = (int)dlg.DoModal();
	if (ret == IDOK){
		m_filtertype = dlg.GetFilterType();
		m_filtersize = dlg.GetFilterSize();
	}

	return ret;
}


/***********************************************************

�@�@�t�B���^�T�C�Y���傫���قǁA
  �@��������̃t���[���Ԃŕ������Ȃ����B

   �E�ړ����ρF
	�ʏ�̐��`�ɂ�镽�����B

	�E���d�ړ����ρF
	���X�ɏd�݂����������Ă����������B
	�ړ����ς����A�����s�[�N���c��₷���B

	�E�K�E�V�A���F
	�K�E�V�A���t�B���^�ɂ�镽�����B
	���̃��[�V�����̍Č������傫���B�i�Ƃ����Ȃ��j
						   �@
***********************************************************/
int CMotFilter::Filter(bool edgesmp, bool limitdegflag, 
	CModel* srcmodel, CBone* srcbone,
	int srcopekind, int srcmotid, int srcstartframe, int srcendframe)
{
	if (!srcmodel || !srcbone){
		return 0;//!!!!!!!!!!
	}
	if ((srcopekind != 1) && (srcopekind != 2) && (srcopekind != 3)) {
		return 0;
	}

	DestroyObjs();//!!!!!!!!!!

	int dlgret = GetFilterType();
	if (dlgret != IDOK){
		return 0;//!!!!!!!!!!!!!!!
	}

	CallFilterFunc(edgesmp, limitdegflag, 
		srcmodel, srcbone, srcopekind, srcmotid, srcstartframe, srcendframe);

	::MessageBox(NULL, L"�����������s���܂����B", L"�����I��", MB_OK);

	return 0;
}


int CMotFilter::FilterNoDlg(bool edgesmp, bool limitdegflag, 
	CModel* srcmodel, CBone* srcbone,
	int srcopekind, int srcmotid, int srcstartframe, int srcendframe)
{
	if (!srcmodel || !srcbone) {
		return 0;//!!!!!!!!!!
	}
	if ((srcopekind != 1) && (srcopekind != 2) && (srcopekind != 3)) {
		return 0;
	}

	DestroyObjs();//!!!!!!!!!!

	
	//m_filtertype = AVGF_GAUSSIAN;
	//int framenum = srcendframe - srcstartframe + 1;
	//if (framenum <= 3) {
	//	return 0;
	//}
	//m_filtersize = max(3, (framenum / 10));


	//2023/02/27
	//�������̕����`���ς��Ȃ�
	//m_filtertype = AVGF_MOVING;
	//m_filtersize = 9;


	//2023/08/09 NoDlg�̏ꍇ�́@m_filtertype, m_filtersize�͑O����s���̒l���g�p����
	//m_filtertype = AVGF_WEIGHTED_MOVING;
	if (m_filtertype == 0) {
		m_filtertype = AVGF_GAUSSIAN;
	}
	if (m_filtersize == 0) {
		m_filtersize = 5;
	}
	
	CallFilterFunc(edgesmp, limitdegflag, 
		srcmodel, srcbone, srcopekind, srcmotid, srcstartframe, srcendframe);

	return 0;
}

int CMotFilter::CallFilterFunc(bool edgesmp, bool limitdegflag, 
	CModel* srcmodel, CBone* srcbone,
	int srcopekind, int srcmotid, int srcstartframe, int srcendframe)
{
	int frameleng = srcendframe - srcstartframe + 1;
	int bufleng = frameleng + m_filtersize * 2;//�T���v�����O�͗��[��m_fitersize���]�v��

	m_eul = new ChaVector3[bufleng];
	if (!m_eul) {
		_ASSERT(0);
		return 1;
	}
	m_smootheul = new ChaVector3[frameleng];
	if (!m_smootheul) {
		_ASSERT(0);
		return 1;
	}

	m_tra = new ChaVector3[bufleng];
	if (!m_tra) {
		_ASSERT(0);
		return 1;
	}
	m_smoothtra = new ChaVector3[frameleng];
	if (!m_smoothtra) {
		_ASSERT(0);
		return 1;
	}

	ZeroMemory(m_eul, sizeof(ChaVector3) * bufleng);//�T���v�����O�͗��[��m_fitersize���]�v��
	ZeroMemory(m_tra, sizeof(ChaVector3) * bufleng);//�T���v�����O�͗��[��m_fitersize���]�v��

	ZeroMemory(m_smootheul, sizeof(ChaVector3) * frameleng);//���ʂ�startframe-->endframe
	ZeroMemory(m_smoothtra, sizeof(ChaVector3) * frameleng);//���ʂ�startframe-->endframe


	if (srcopekind == 1) {
		//all joints
		//bool saveunderik = g_underIKRot;
		//g_underIKRot = true;
		bool saveunderik = srcmodel->GetUnderIKRot();
		srcmodel->SetUnderIKRot(true);

		bool firstbroflag = false;
		FilterReq(edgesmp, limitdegflag, srcmodel, 
			srcmodel->GetTopBone(false), srcmotid, srcstartframe, srcendframe, firstbroflag);
		
		//g_underIKRot = saveunderik;
		srcmodel->SetUnderIKRot(saveunderik);
	}
	else if (srcopekind == 2) {
		//selecting joint
		if (srcbone->IsSkeleton()) {
			//bool saveunderik = g_underIKRot;
			//g_underIKRot = true;
			bool saveunderik = srcmodel->GetUnderIKRot();
			srcmodel->SetUnderIKRot(true);

			int result = FilterFunc(edgesmp, limitdegflag,
				srcmodel, srcbone, srcmotid, srcstartframe, srcendframe);

			//g_underIKRot = saveunderik;
			srcmodel->SetUnderIKRot(saveunderik);

			if (result != 0) {
				_ASSERT(0);
				return 1;//!!!!!!!!!!!!!
			}
		}
	}
	else if (srcopekind == 3) {
		//selecting joint and deeper
		//bool saveunderik = g_underIKRot;
		//g_underIKRot = true;
		bool saveunderik = srcmodel->GetUnderIKRot();
		srcmodel->SetUnderIKRot(true);

		bool firstbroflag = false;
		FilterReq(edgesmp, limitdegflag, 
			srcmodel, srcbone, srcmotid, srcstartframe, srcendframe, firstbroflag);

		//g_underIKRot = saveunderik;
		srcmodel->SetUnderIKRot(saveunderik);
	}
	else {
		_ASSERT(0);
		return 1;
	}

	DestroyObjs();

	return 0;
}


int CMotFilter::FilterFunc(bool edgesmp, bool limitdegflag,
	CModel* srcmodel, CBone* curbone, int srcmotid, int srcstartframe, int srcendframe)
{
	if (!srcmodel || !curbone) {
		_ASSERT(0);
		return 1;
	}

	if (curbone->IsSkeleton()) {
		int frameleng = srcendframe - srcstartframe + 1;
		int bufleng = frameleng + m_filtersize * 2;//�T���v�����O�͗��[��m_fitersize���]�v��

		int motionleng;
		MOTINFO* curmi = srcmodel->GetMotInfo(srcmotid);
		if (curmi) {
			motionleng = IntTime(curmi->frameleng);
		}
		else {
			motionleng = frameleng;
			_ASSERT(0);
			return 1;
		}


		ChaVector3 tmp_vec3;
		ChaVector3 tmp_pos3;

		int frame;

		if (curbone) {

			ZeroMemory(m_eul, sizeof(ChaVector3) * bufleng);//�T���v�����O�͗��[��m_fitersize���]�v��
			ZeroMemory(m_tra, sizeof(ChaVector3) * bufleng);//�T���v�����O�͗��[��m_fitersize���]�v��

			ZeroMemory(m_smootheul, sizeof(ChaVector3) * frameleng);//���ʂ�startframe-->endframe
			ZeroMemory(m_smoothtra, sizeof(ChaVector3) * frameleng);//���ʂ�startframe-->endframe

			int bufindex = 0;
			for (frame = (srcstartframe - m_filtersize); frame <= (srcendframe + m_filtersize); frame++) {
				int smpframe;
				if (edgesmp == false) {
					//���[�����̃T���v�����O�@���R�[�@�ҏW�̈�̊O��(�����)�T���v�����O
					if ((frame >= 1) && (frame < motionleng)) {
						smpframe = frame;
					}
					else if (frame < 1) {
						smpframe = 1;
					}
					else if (frame >= motionleng) {
						smpframe = motionleng - 1;
					}
					else {
						_ASSERT(0);
						return 1;
					}
				}
				else {
					//2023/03/05
					//��Ɂ@�ʒu�R���X�g���C���g�̎��́@�I�v�V�����Ƃ��Ďg�p����\��
					//���[�����̃T���v�����O�@�Œ�[�@�ҏW�̈�̊O�̓T���v�����O���Ȃ�
					if (frame < srcstartframe) {
						smpframe = srcstartframe;
					}
					else if (frame > srcendframe) {
						smpframe = srcendframe;
					}
					else {
						smpframe = frame;
					}
				}
				ChaVector3 cureul = curbone->CalcLocalEulXYZ(limitdegflag, -1, srcmotid, (double)smpframe, BEFEUL_BEFFRAME);// axiskind = -1 --> m_anglelimit�̍��W�n
				ChaVector3 curtra = curbone->CalcLocalTraAnim(limitdegflag, srcmotid, (double)smpframe);

				*(m_eul + bufindex) = cureul;
				*(m_tra + bufindex) = curtra;
				bufindex++;
			}


			//����������
			switch (m_filtertype) {
			case AVGF_NONE:
				//���������Ȃ�
			{
				MoveMemory((void*)m_smootheul, (void*)(m_eul + m_filtersize), sizeof(ChaVector3) * frameleng);
				MoveMemory((void*)m_smoothtra, (void*)(m_tra + m_filtersize), sizeof(ChaVector3) * frameleng);
			}
			break;
			case AVGF_MOVING:
				//�ړ�����
			{
				int srcindex;
				int dstindex = 0;
				for (srcindex = m_filtersize; srcindex < (frameleng + m_filtersize); srcindex++) {
					tmp_vec3 = ChaVector3(0.0f, 0.0f, 0.0f);
					tmp_pos3 = ChaVector3(0.0f, 0.0f, 0.0f);
					int addcount = 0;
					for (int k = -m_filtersize; k <= m_filtersize; k++) {
						int smpframe = srcindex + k;
						if ((smpframe >= 0) && (smpframe < bufleng)) {
							tmp_vec3 += m_eul[smpframe];
							tmp_pos3 += m_tra[smpframe];
							addcount++;
						}
						else {
							_ASSERT(0);
							return 1;
						}
					}
					if (addcount > 0) {
						if ((dstindex >= 0) && (dstindex < frameleng)) {
							m_smootheul[dstindex] = tmp_vec3 / (float)addcount;
							m_smoothtra[dstindex] = tmp_pos3 / (float)addcount;
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
					dstindex++;
				}
			}
			break;

			case AVGF_WEIGHTED_MOVING:
				//���d�ړ�����
			{
				double denomVal = 0.0;
				float weightVal = 0.0;

				int sumd = 0;
				int coef = 0;
				for (int i = 1; i <= m_filtersize; i++) {
					sumd += i;
				}
				denomVal = 1.0 / (double)sumd;

				int srcindex;
				int dstindex = 0;
				for (srcindex = m_filtersize; srcindex < (frameleng + m_filtersize); srcindex++) {
					tmp_vec3 = ChaVector3(0.0f, 0.0f, 0.0f);
					tmp_pos3 = ChaVector3(0.0f, 0.0f, 0.0f);
					for (int k = -m_filtersize; k <= m_filtersize; k++) {
						if (k <= 0) {
							coef = k + m_filtersize;
						}
						else {
							coef = m_filtersize - k;
						}
						weightVal = (float)((double)denomVal / 2.0 * (double)coef);

						int smpframe = srcindex + k;
						if ((smpframe >= 0) && (smpframe < bufleng)) {
							tmp_vec3 = tmp_vec3 + m_eul[smpframe] * weightVal;
							tmp_pos3 = tmp_pos3 + m_tra[smpframe] * weightVal;
						}
						else {
							_ASSERT(0);
							return 1;
						}
					}
					if ((dstindex >= 0) && (dstindex < frameleng)) {
						m_smootheul[dstindex] = tmp_vec3;
						m_smoothtra[dstindex] = tmp_pos3;
					}
					else {
						_ASSERT(0);
						return 1;
					}
					dstindex++;
				}
			}
			break;

			case AVGF_GAUSSIAN:
				//�K�E�V�A�� 
			{
				double normalizeVal = 0.0;
				float normalDistVal = 0.0;

				//int N = m_filtersize - 1;
				int sum = 0;
				int r = 0;
				for (int i = 0; i <= m_filtersize; i++) {
					sum += Combi(m_filtersize, i);
				}
				normalizeVal = 1.0 / (double)sum;

				int srcindex;
				int dstindex = 0;
				for (srcindex = m_filtersize; srcindex < (frameleng + m_filtersize); srcindex++) {
					tmp_vec3 = ChaVector3(0.0f, 0.0f, 0.0f);
					tmp_pos3 = ChaVector3(0.0f, 0.0f, 0.0f);
					for (int k = -m_filtersize; k <= m_filtersize; k++) {
						if (k <= 0) {
							r = k + m_filtersize;
						}
						else {
							r = m_filtersize - k;
						}
						normalDistVal = (float)((double)normalizeVal / 2.0 *
							(double)Combi(m_filtersize, r));

						int smpframe = srcindex + k;
						if ((smpframe >= 0) && (smpframe < bufleng)) {
							tmp_vec3 = tmp_vec3 + m_eul[smpframe] * normalDistVal;
							tmp_pos3 = tmp_pos3 + m_tra[smpframe] * normalDistVal;
						}
						else {
							_ASSERT(0);
							return 1;
						}
					}
					if ((dstindex >= 0) && (dstindex < frameleng)) {
						m_smootheul[dstindex] = tmp_vec3;
						m_smoothtra[dstindex] = tmp_pos3;
					}
					else {
						_ASSERT(0);
						return 1;
					}
					dstindex++;
				}
			}
			break;

			default:
				break;
			}

			for (frame = srcstartframe; frame <= srcendframe; frame++) {
				ChaMatrix befwm = curbone->GetWorldMat(limitdegflag, srcmotid, (double)frame, 0);
				//curbone->SetWorldMatFromEulAndTra(limitdegflag, 1, befwm, m_smootheul[frame - srcstartframe], m_smoothtra[frame - srcstartframe], srcmotid, (double)frame);
				//curbone->SetWorldMatFromEul(0, 1, m_smootheul[frame - srcstartframe], srcmotid, (double)frame);


				//�ύX�O��scalevec���擾
				ChaMatrix beflocalmat;
				beflocalmat.SetIdentity();
				if (curbone->GetParent(false)) {
					ChaMatrix parmat;
					parmat = curbone->GetParent(false)->GetWorldMat(limitdegflag, srcmotid, (double)frame, 0);
					beflocalmat = befwm * ChaMatrixInv(parmat);
				}
				else {
					beflocalmat = befwm;
				}
				ChaVector3 befsvec, beftvec;
				ChaMatrix befrmat;
				GetSRTMatrix(beflocalmat, &befsvec, &befrmat, &beftvec);

				int inittraflag = 0;
				int setchildflag = 1;//!!!
				curbone->SetWorldMatFromEulAndScaleAndTra(limitdegflag, inittraflag, setchildflag,
					befwm, m_smootheul[frame - srcstartframe],
					befsvec, m_smoothtra[frame - srcstartframe],
					srcmotid, (double)frame);
			}
		}
	}
	return 0;
}

void CMotFilter::FilterReq(bool edgesmp, bool limitdegflag, 
	CModel* srcmodel, CBone* curbone, 
	int srcmotid, int srcstartframe, int srcendframe, bool broflag)
{
	if (curbone) {

		if (curbone->IsSkeleton()) {
			int result = FilterFunc(edgesmp, limitdegflag,
				srcmodel, curbone, srcmotid, srcstartframe, srcendframe);
			if (result != 0) {
				_ASSERT(0);
				return;//!!!!!!!!!!!!!
			}
		}

		if (curbone->GetChild(false)){
			bool broflag2 = true;
			FilterReq(edgesmp, limitdegflag, 
				srcmodel, curbone->GetChild(false), srcmotid, srcstartframe, srcendframe, broflag2);
		}
		if (curbone->GetBrother(false) && broflag){
			FilterReq(edgesmp, limitdegflag, 
				srcmodel, curbone->GetBrother(false), srcmotid, srcstartframe, srcendframe, broflag);
		}
	}
}



//�K�E�V�A���t�B���^�p
int CMotFilter::Combi(int n, int r) {
	if (n < 0){
		_ASSERT(0);
		return 0;
	}
	if ((r < 0) || (r > n)){
		_ASSERT(0);
		return 0;
	}
	if (n == r){
		return 1;
	}
	if (r == 0){
		return 1;
	}

	int val0 = n;
	int val1 = r;
	int calccntmax = r;
	int calccnt;
	for (calccnt = calccntmax; calccnt >= 2; calccnt--){
		val0 = val0 * (val0 - 1);
		val1 = val1 * (val1 - 1);
	}

	if (val1 != 0){
		return val0 / val1;
	}
	else{
		return 1;
	}
}


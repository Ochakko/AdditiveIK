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

#include <ChaCamera.h>

#include <Model.h>
#include <ChaCalcFunc.h>
#include <ChaScene.h>
#include <FbxMisc.h>

#define DBGH
#include <dbg.h>

using namespace std;

extern ChaScene* g_chascene;


ChaCamera::ChaCamera()
{
	InitParams();
}
ChaCamera::~ChaCamera()
{
	DestroyObjs();
}

void ChaCamera::InitParams()
{
	m_camEye = ChaVector3(0.0f, 0.0f, 0.0f);
	m_camtargetpos = ChaVector3(0.0f, 0.0f, 0.0f);
	m_befcamEye = ChaVector3(0.0f, 0.0f, 0.0f);
	m_befcamtargetpos = ChaVector3(0.0f, 0.0f, 0.0f);
	m_cameraupdir = ChaVector3(0.0f, 1.0f, 0.0f);
	m_camdist = 50.0f;
	m_initcamdist = 50.0f;
	m_maxcamdist = 20000.0f;
	m_fovy = (float)(PI / 4.0);
	m_projnear = 0.01f;
	m_projfar = m_initcamdist * 100.0f;
	m_fAspectRatio = 1.0f;

	m_cameraheight = 200.0f;
	m_cameraheightflag = 0;
	m_cameragmodel = nullptr;

	m_camtargetflag = 0;
	m_camtargetOnceflag = 0;
	m_camtargetdisp = false;//カメラターゲット位置にマニピュレータを表示するかどうかのフラグ
	m_moveeyepos = false;//s_sidemenu_camdistSlider動作の種類　true:eyeposが動く、false:targetposが動く

	m_cameramodel = nullptr;
	m_cameraHasChildFlag = false;//１つでもCameraの子供モデルがあればTRUE. OnUserFrameMove()でセット.

	m_twistcameraFlag = false;
	m_cameradollyFlag = false;


	m_cameraframe = 0.0;
	m_cameraanimmode = 0;//0: OFF, 1:ON, 2:ON and RootMotionOption ON
	m_cameraInheritMode = CAMERA_INHERIT_ALL;

	m_cammvstep = 500.0f;

	for (int refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
		m_refposView[refposindex].SetIdentity();
	}
	m_refposStartIndex = REFPOSMAXNUM;//リングバッファ保存位置　初回のProcessRefPosView()で0になるように初期化
	m_refposRecordCount = -1;//30フレームごとにm_refposViewを更新するためのカウンタ
}

void ChaCamera::DestroyObjs()
{

}

int ChaCamera::ChangeCameraDist(float newcamdist, bool moveeyeposflag, bool calledbyslider)
{
	float savecamdist = m_camdist;
	{
		m_camdist = (float)fmin(m_maxcamdist, newcamdist);//2024/06/04 最大値でクランプしてからtargetを再計算する

		if (m_camdist >= 1.0f) {
			ChaVector3 newcampos;// = m_camtargetpos + camvec * m_camdist;

			ChaVector3 camvec = m_camEye - m_camtargetpos;
			ChaVector3Normalize(&camvec, &camvec);

			ChaVector2 camvecXZ;
			camvecXZ.x = m_camEye.x - m_camtargetpos.x;
			camvecXZ.y = m_camEye.z - m_camtargetpos.z;
			ChaVector2Normalize(&camvecXZ, &camvecXZ);

			if (moveeyeposflag == true) {//2024/02/26

				if (!m_camtargetOnceflag && m_camtargetflag) {
					float tempcamposx = m_camtargetpos.x + camvecXZ.x * m_camdist;
					float tempcamposy = m_camEye.y;
					float tempcamposz = m_camtargetpos.z + camvecXZ.y * m_camdist;

					//2025/10/04 カメラ酔い防止策　カメラの位置は徐々に変える
					newcampos.x = m_camEye.x + (tempcamposx - m_camEye.x) * 0.0060f * (float)g_dspeed;
					newcampos.y = m_camEye.y + (tempcamposy - m_camEye.y) * 0.0060f * (float)g_dspeed;// *0.010f;
					newcampos.z = m_camEye.z + (tempcamposz - m_camEye.z) * 0.0060f * (float)g_dspeed;
				}
				else {
					newcampos = m_camtargetpos + camvec * m_camdist;
				}

				if ((m_cameraheightflag == 1) && (m_cameragmodel != nullptr) && moveeyeposflag) {
					camvec.y = 0.0f;

					ChaVector3 startglobal = newcampos + ChaVector3(0.0f, (m_cameraheight + 1.0f), 0.0f);
					ChaVector3 endglobal = newcampos - ChaVector3(0.0f, (m_cameraheight + 1.0f), 0.0f);

					ChaVector3 gpos = newcampos;
					int hitflag = m_cameragmodel->CollisionPolyMesh3_Ray(
						false,
						startglobal, endglobal, &gpos, true);
					if (hitflag != 0) {
						//newcampos = gpos;
						//newcampos.y += m_cameraheight;

						newcampos.x = gpos.x;
						newcampos.y = m_camEye.y + (gpos.y + m_cameraheight - m_camEye.y) * 0.0060f * (float)g_dspeed;//徐々に変化するように
						newcampos.z = gpos.z;
					}
				}
				else {
					ChaVector3Normalize(&camvec, &camvec);
					newcampos = m_camtargetpos + camvec * m_camdist;
				}

				//ChaVector3 diffvec = newcampos - m_camtargetpos;
				//m_camdist = (float)ChaVector3LengthDbl_2D(&diffvec);


				m_befcamEye = m_camEye;
				//m_camEye = m_camtargetpos + camvec * m_camdist;
				m_camEye = newcampos;
			}
			else {
				m_befcamtargetpos = m_camtargetpos;
				m_camtargetpos = m_camEye - camvec * m_camdist;
			}
		}
		else {

			//2023/03/23
			//カメラ位置がターゲットに近づきすぎた場合　止めないで　ターゲット位置を視線方向に延長するように

			ChaVector3 camvec2 = m_camtargetpos - m_camEye;
			//ChaVector3Normalize(&camvec2, &camvec2);

			float savedist3 = (float)fmin(m_maxcamdist, (savecamdist * 3.0f));//2024/06/04

			m_befcamEye = m_camEye;
			m_befcamtargetpos = m_camtargetpos;

			if (moveeyeposflag == true) {//2024/02/26
				if ((m_cameraheightflag == 1) && (m_cameragmodel != nullptr) && moveeyeposflag) {
					camvec2.y = 0.0f;
					ChaVector3Normalize(&camvec2, &camvec2);
				}
				else {
					ChaVector3Normalize(&camvec2, &camvec2);
				}
				m_camtargetpos = m_camEye + camvec2 * savedist3;
				m_camEye = m_camtargetpos - camvec2 * savedist3;
			}
			else {
				ChaVector3Normalize(&camvec2, &camvec2);
				m_camEye = m_camtargetpos + camvec2 * savedist3;
				m_camtargetpos = m_camEye - camvec2 * savedist3;
			}
			m_camdist = savedist3;

		}
	}

	return 0;
}

double ChaCamera::CalcCameraDist()
{
	ChaVector3 diffv = m_camEye - m_camtargetpos;
	float newcamdist;
	if ((m_cameraheightflag == 1) && (m_cameragmodel != nullptr) && m_moveeyepos) {
		newcamdist = (float)ChaVector3LengthDbl_2D(&diffv);
	}
	else {
		newcamdist = (float)ChaVector3LengthDbl(&diffv);
	}

	return newcamdist;
}

double ChaCamera::UpdateCameraDist()
{
	m_befcamEye = m_camEye;

	float newcamdist = (float)CalcCameraDist();
	ChangeCameraDist(newcamdist, true, false);
	return newcamdist;
}

void ChaCamera::UpdateCameraAnimFrame(double cameranextframe)
{
	SetCameraFrame(cameranextframe);

	if (m_cameramodel) {
		m_cameramodel->SetCameraMotionFrame(m_cameramodel->GetCameraMotionId(), cameranextframe);
	}
}

void ChaCamera::OnCameraMenu(int cameramotid)
{
	if ((m_cameramodel != nullptr) && m_cameramodel->IsCameraLoaded()) {
		//fbxにカメラが在る場合
		ChaVector3 camdir;
		camdir.SetParams(0.0f, 0.0f, 1.0f);
		m_cameramodel->GetCameraProjParams(cameramotid, &m_projnear, &m_projfar, &m_fovy, &m_camEye, &camdir, &m_cameraupdir);

		m_initcamdist = (float)fmax(0.1f, fmin(m_maxcamdist, m_projfar));
		m_camtargetpos = m_camEye + camdir * m_initcamdist;

		m_camdist = m_initcamdist;

		m_befcamEye = m_camEye;
		m_befcamtargetpos = m_camtargetpos;
	}
}

void ChaCamera::OnFramePreviewCamera(double nextcameraframe)
{
	if (m_cameramodel != nullptr) {
		m_cameramodel->GetCameraAnimParams(nextcameraframe, m_camdist,
			&m_camEye, &m_camtargetpos, &m_cameraupdir, 0, m_cameraInheritMode);//m_camdist
		
		m_cameraframe = nextcameraframe;

		UpdateCameraDist();
	}
}

void ChaCamera::SetCamera6Angle(int srcangleid)
{
	ChaVector3 weye, wdiff;
	weye = m_camEye;
	wdiff = m_camtargetpos - weye;
	float camdist = (float)ChaVector3LengthDbl(&wdiff);

	ChaVector3 neweye;
	float delta = 0.10f;

	bool setflag = false;

	if (srcangleid == 1) {
		neweye.x = m_camtargetpos.x;
		neweye.y = m_camtargetpos.y;
		neweye.z = m_camtargetpos.z - camdist;

		m_befcamEye = m_camEye;
		m_camEye = neweye;

		setflag = true;
	}
	else if (srcangleid == 2) {
		neweye.x = m_camtargetpos.x;
		neweye.y = m_camtargetpos.y;
		neweye.z = m_camtargetpos.z + camdist;

		m_befcamEye = m_camEye;
		m_camEye = neweye;

		setflag = true;
	}
	else if (srcangleid == 3) {
		neweye.x = m_camtargetpos.x - camdist;
		neweye.y = m_camtargetpos.y;
		neweye.z = m_camtargetpos.z;

		m_befcamEye = m_camEye;
		m_camEye = neweye;

		setflag = true;
	}
	else if (srcangleid == 4) {
		neweye.x = m_camtargetpos.x + camdist;
		neweye.y = m_camtargetpos.y;
		neweye.z = m_camtargetpos.z;

		m_befcamEye = m_camEye;
		m_camEye = neweye;
	}
	else if (srcangleid == 5) {
		neweye.x = m_camtargetpos.x;
		neweye.y = m_camtargetpos.y + camdist;
		neweye.z = m_camtargetpos.z + delta;

		m_befcamEye = m_camEye;
		m_camEye = neweye;

		setflag = true;
	}
	else if (srcangleid == 6) {
		neweye.x = m_camtargetpos.x;
		neweye.y = m_camtargetpos.y - camdist;
		neweye.z = m_camtargetpos.z - delta;

		m_befcamEye = m_camEye;
		m_camEye = neweye;

		setflag = true;
	}

	if (setflag) {
		UpdateCameraDist();
	}
}

void ChaCamera::OnCameraAnimMouseMoveDist()
{
	if (m_cameramodel != nullptr) {
		ChaVector3 tmpcamtarget;
		m_cameramodel->GetCameraAnimParams(m_cameraframe,
			m_camdist,
			&m_camEye, &tmpcamtarget, &m_cameraupdir,
			0, m_cameraInheritMode);//g_camdist

		if (m_camtargetflag) {
			//g_camtargetposはそのまま
		}
		else {
			m_camtargetpos = tmpcamtarget;
		}
	}
}

void ChaCamera::ProcessRefPosView()
{
	m_refposRecordCount++;
	if (m_refposRecordCount >= REFPOSRECORDINTERVAL) {
		//30フレームに1回だけ更新する
		m_refposRecordCount = 0;

		m_refposStartIndex++;
		if (m_refposStartIndex >= REFPOSMAXNUM) {
			m_refposStartIndex = 0;
		}
		m_refposView[m_refposStartIndex].SetParams(g_camera3D->GetViewMatrix(false));
	}

}
ChaMatrix ChaCamera::GetRefPosView(int srcrefposindex)
{
	if ((srcrefposindex < 0) || (srcrefposindex >= REFPOSMAXNUM)) {
		ChaMatrix dummy;
		dummy.SetIdentity();
		return dummy;
	}

	int viewindex = m_refposStartIndex - srcrefposindex;
	if (viewindex >= REFPOSMAXNUM) {
		viewindex -= REFPOSMAXNUM;
	}
	if (viewindex < 0) {
		viewindex += REFPOSMAXNUM;
	}

	return m_refposView[viewindex];
}

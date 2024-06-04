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

#include <Bone.h>

#define DBGH
#include <dbg.h>

#include <CameraFbx.h>
#include <GlobalVar.h>


using namespace std;


CCameraFbx::CCameraFbx()
{
	InitParams();
}
CCameraFbx::~CCameraFbx()
{
	DestroyObjs();
}

int CCameraFbx::InitParams()
{
	m_loadedflag = false;

	m_cameranode.clear();
	m_cameramotion.clear();

	m_time = 0.0;

	m_pscene = 0;
	ZeroMemory(m_animname, sizeof(char) * 256);

	return 0;
}
int CCameraFbx::DestroyObjs()
{
	vector<CAMERANODE*>::iterator itrcameranode;
	for (itrcameranode = m_cameranode.begin(); itrcameranode != m_cameranode.end(); itrcameranode++) {
		CAMERANODE* curcn = *itrcameranode;
		if (curcn) {
			free(curcn);
		}
	}
	m_cameranode.clear();
	m_cameramotion.clear();

	InitParams();

	return 0;
}

int CCameraFbx::Clear()
{
	InitParams();
	return 0;
}


CAMERANODE* CCameraFbx::FindCameraNodeByNode(FbxNode* srcnode)
{
	vector<CAMERANODE*>::iterator itrcameranode;
	for (itrcameranode = m_cameranode.begin(); itrcameranode != m_cameranode.end(); itrcameranode++) {
		CAMERANODE* curcn = *itrcameranode;
		if (curcn) {
			if (curcn->pnode && curcn->pbone && (curcn->pnode == srcnode)) {
				return curcn;
			}
		}
	}
	return 0;
}
CAMERANODE* CCameraFbx::FindCameraNodeByBone(CBone* srcbone)
{
	vector<CAMERANODE*>::iterator itrcameranode;
	for (itrcameranode = m_cameranode.begin(); itrcameranode != m_cameranode.end(); itrcameranode++) {
		CAMERANODE* curcn = *itrcameranode;
		if (curcn) {
			if (curcn->pnode && curcn->pbone && (curcn->pbone == srcbone)) {
				return curcn;
			}
		}
	}
	return 0;
}

CAMERANODE* CCameraFbx::FindCameraNodeByMotId(int srcmotid)
{
	map<int, CAMERANODE*>::iterator itrcameranode;
	itrcameranode = m_cameramotion.find(srcmotid);
	if (itrcameranode != m_cameramotion.end()) {
		CAMERANODE* retcameranode = itrcameranode->second;
		if (retcameranode && retcameranode->IsValid()) {
			return retcameranode;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	}
	else {
		return 0;
	}
}

int CCameraFbx::AddFbxCamera(FbxNode* pNode, CBone* pbone)
{

	//#############################################################################################################################
	//使い方
	//　!!!!!!!! カメラオブジェクトの名前と　アニメーション(名前にCameraを含む)の名前を一致させることが読み込み条件 !!!!!!!!!!!!!!
	//#############################################################################################################################


	if (!pNode || !pbone) {
		_ASSERT(0);
		return 1;
	}

	CAMERANODE* chkcameranode = FindCameraNodeByNode(pNode);
	if (chkcameranode) {
		//既に存在するのでリターン
		_ASSERT(0);
		return 0;
	}

	CAMERANODE* newcameranode = (CAMERANODE*)malloc(sizeof(CAMERANODE));
	if (!newcameranode) {
		_ASSERT(0);
		return 1;
	}
	newcameranode->Init();
	m_cameranode.push_back(newcameranode);//!!!!!!!!!



	newcameranode->pnode = pNode;
	newcameranode->pbone = pbone;
	newcameranode->pcamera = pNode->GetCamera();//fbxsdk func


	if (newcameranode->pcamera) {



		FbxVector4 upVector = newcameranode->pcamera->UpVector.Get();     // アップベクトル
		FbxDouble aspectHeight = newcameranode->pcamera->AspectHeight.Get(); // アスペクト高
		FbxDouble aspectWidth = newcameranode->pcamera->AspectWidth.Get();  // アスペクト幅

		//FbxDouble nearZ = newcameranode->pcamera->GetNearPlane();     // near平面距離
		//FbxDouble farZ = newcameranode->pcamera->GetFarPlane();      // far平面距離
		FbxDouble nearZ = newcameranode->pcamera->NearPlane.Get();     // near平面距離
		FbxDouble farZ = newcameranode->pcamera->FarPlane.Get();      // far平面距離
		
		FbxDouble aspectRatio = aspectWidth / aspectHeight; // アスペクト比
		FbxDouble inch_mm = 25.4;    // インチ→ミリ
		FbxDouble filmHeight = newcameranode->pcamera->FilmHeight.Get();   // フィルム高（インチ）
		FbxDouble focalLength = newcameranode->pcamera->FocalLength.Get(); // 合焦距離（ミリ）
		FbxDouble filmHeight_mm = inch_mm * filmHeight;


		//FbxDouble fovY = atan2(filmHeight_mm, 2.0 * focalLength);
		FbxDouble fovY_Degree = newcameranode->pcamera->ComputeFieldOfView(focalLength);//degree
		FbxDouble fovY = fovY_Degree * 3.14159265358979 / 180.0;


		//newcameranode->upVector = ChaVector3((float)upVector[0], (float)upVector[1], (float)upVector[2]);     // アップベクトル
		newcameranode->aspectHeight = aspectHeight; // アスペクト高
		newcameranode->aspectWidth = aspectWidth;  // アスペクト幅
		newcameranode->nearZ = nearZ;     // near平面距離
		newcameranode->farZ = farZ;      // far平面距離
		newcameranode->aspectRatio = aspectRatio; // アスペクト比
		newcameranode->inch_mm = inch_mm;    // インチ→ミリ
		newcameranode->filmHeight = filmHeight;   // フィルム高（インチ）
		newcameranode->focalLength = focalLength; // 合焦距離（ミリ）
		newcameranode->filmHeight_mm = filmHeight_mm;
		newcameranode->fovY = fovY;
		newcameranode->fovY_Degree = fovY_Degree;


		//################################################################################
		//カメラがアニメーションを持たない場合のために 0フレームの位置と向きを計算しておく
		//################################################################################
		FbxTime timezero;
		timezero.SetSecondDouble(m_time);
		FbxVector4 fbxpos = newcameranode->pcamera->EvaluatePosition(timezero);
		newcameranode->position = ChaVector3(fbxpos);

		FbxAMatrix fbxcameramat = newcameranode->pnode->EvaluateGlobalTransform(timezero, FbxNode::eSourcePivot, true, true);
		ChaMatrix cameramat = ChaMatrixFromFbxAMatrix(fbxcameramat);
		newcameranode->worldmat = cameramat;//!!!!!!


		ChaMatrix rotmat = cameramat;
		CQuaternion cameraq;
		cameraq.RotationMatrix(rotmat);
		//ChaVector3 firstdir = ChaVector3(1.0f, 0.0f, 0.0f);
		//ChaVector3 cameradir = ChaVector3(1.0f, 0.0f, 0.0f);
		ChaVector3 dirvec0;
		ChaVector3 dirvec;
		dirvec0 = ChaVector3(1.0f, 0.0f, 0.0f);
		dirvec = ChaVector3(1.0f, 0.0f, 0.0f);
		cameraq.Rotate(&dirvec, dirvec0);
		ChaVector3Normalize(&dirvec, &dirvec);
		newcameranode->dirvec = dirvec;


		//ChaVector3 firstupdir = ChaVector3(0.0f, 1.0f, 0.0f);
		//ChaVector3 cameraupdir = ChaVector3(0.0f, 1.0f, 0.0f);
		ChaVector3 firstupdir = ChaVector3(0.01f, 0.99f, 0.0f);
		ChaVector3 cameraupdir = ChaVector3(0.01f, 0.99f, 0.0f);
		cameraq.Rotate(&cameraupdir, firstupdir);
		ChaVector3Normalize(&cameraupdir, &cameraupdir);
		newcameranode->upvec = cameraupdir;




		m_loadedflag = true;

	}


	return 0;

}

CAMERANODE* CCameraFbx::GetCameraNode(int cameramotid)
{
	CAMERANODE* curcamera = FindCameraNodeByMotId(cameramotid);
	if (!curcamera) {
		return 0;
	}
	if (!curcamera->IsValid()) {
		//必要な情報が読み込めていない場合　エラー
		return 0;
	}

	return curcamera;
}



int CCameraFbx::PreLoadFbxAnim(CBone* srcbone, int srcmotid)
{
	
	//この時点では　CAMERANODE*とsrcmotidは紐づいていない
	CAMERANODE* cameranode = FindCameraNodeByBone(srcbone);
	if (!cameranode) {
		_ASSERT(0);
		return 1;
	}
	if (!cameranode->IsValid()) {
		_ASSERT(0);
		return 1;
	}


	//2023/06/27
	//CAMERA_INHERIT_CANCEL_NULL2の位置補正用
	//2023/07/08
	//!!!!!  カレントモーションがカメラのモーションの時に計算する  !!!!!
	FbxTime fbxtime0;
	fbxtime0.SetSecondDouble(0.0);
	FbxVector4 lcltime0 = cameranode->pnode->EvaluateLocalTranslation(fbxtime0, FbxNode::eSourcePivot, true, true);
	ChaVector3 chalcltime0 = ChaVector3(lcltime0, false);
	ChaVector3 charotpiv = ChaVector3(cameranode->pnode->GetRotationPivot(FbxNode::eSourcePivot));
	ChaVector3 nodepos = ChaMatrixTraVec(srcbone->GetNodeMat());
	cameranode->adjustpos = charotpiv - chalcltime0;



	//###################
	//cameramotionに登録
	//###################
	CAMERANODE* chkcameranode = FindCameraNodeByMotId(srcmotid);
	if (!chkcameranode) {
		m_cameramotion[srcmotid] = cameranode;
	}


	return 0;
}



ChaMatrix CCameraFbx::GetCameraNodeMat(int cameramotid)
{
	//CalcENullMatrixに掛けるInvNodeMat用


	ChaMatrix nodemat;
	nodemat.SetIdentity();

	if (!IsLoaded()) {
		nodemat.SetIdentity();
		return nodemat;
	}
	if (cameramotid < 0) {
		_ASSERT(0);
		nodemat.SetIdentity();
		return nodemat;
	}


	CAMERANODE* curcamera = GetCameraNode(cameramotid);
	if (!curcamera) {
		_ASSERT(0);
		nodemat.SetIdentity();
		return nodemat;
	}

	CBone* curbone = curcamera->pbone;
	if (curbone) {
		nodemat = curbone->GetNodeMat();
		return nodemat;
	}
	else {
		nodemat.SetIdentity();
		return nodemat;

	}
}


ChaMatrix CCameraFbx::GetCameraTransformMat(int cameramotid, double nextframe, int inheritmode, 
	bool calcbynode, bool setmotionpoint)
{
	//InvNodeMatが掛かっていない　wmではなくtransformmat


	ChaMatrix transformmat;
	transformmat.SetIdentity();

	double roundingframe = RoundingTime(nextframe);

	if (!IsLoaded()) {
		transformmat.SetIdentity();
		return transformmat;
	}
	if (cameramotid < 0) {
		_ASSERT(0);
		transformmat.SetIdentity();
		return transformmat;
	}


	CAMERANODE* curcamera = GetCameraNode(cameramotid);
	if (!curcamera) {
		_ASSERT(0);
		transformmat.SetIdentity();
		return transformmat;
	}

	
	//2024/01/31 NotRoundingTime
	m_time = nextframe;

	FbxTime fbxtime;
	fbxtime.SetSecondDouble(m_time / 30.0);


	FbxNode* cameranode = curcamera->pnode;//IsValid()で非Nullチェック済
	CBone* camerabone = curcamera->pbone;//IsValid()で非Nullチェック済
	if (!camerabone->GetParent(false)) {
		_ASSERT(0);
		transformmat.SetIdentity();
		return transformmat;
	}


	MOTINFO camerami = camerabone->GetParModel()->GetMotInfo(cameramotid);


	if (calcbynode == false) {

		//########################
		//読込格納済のモーションを返す
		//########################

		//##################################################################
		//calcbynode == falseの場合は　プレビュー時に使用するのでフレーム間を補間する
		//##################################################################

		ChaMatrix cameramat;
		cameramat.SetIdentity();
		ChaMatrix cameramat1;
		cameramat1.SetIdentity();
		ChaMatrix cameramat2;
		cameramat2.SetIdentity();

		CMotionPoint* curmp;
		curmp = camerabone->GetMotionPoint(cameramotid, roundingframe);
		if (curmp) {
			cameramat1 = curmp->GetWorldMat();
		}
		else {
			_ASSERT(0);
			cameramat1.SetIdentity();
		}

		double nextroundingframe = roundingframe + 1.0;
		if (nextroundingframe < camerami.frameleng) {
			CMotionPoint* curmp2;
			curmp2 = camerabone->GetMotionPoint(cameramotid, nextroundingframe);
			if (curmp2) {
				cameramat2 = curmp2->GetWorldMat();
			}
			else {
				_ASSERT(0);
				cameramat2.SetIdentity();
			}

			double t = (m_time - roundingframe);
			cameramat = cameramat1 + (cameramat2 - cameramat1) * (float)t;
		}
		else {
			cameramat = cameramat1;
		}

		return cameramat;
	}
	else {

		//#####################################
		//fbxのモーションを返す 必要に応じて格納する
		//#####################################

		ChaMatrix localnodemat, localnodeanimmat;
		localnodemat.SetIdentity();
		localnodeanimmat.SetIdentity();
		bool bindposeflag = false;
		camerabone->CalcLocalNodePosture(bindposeflag, 0, cameramotid, roundingframe, &localnodemat, &localnodeanimmat);

		ChaMatrix parentGlobalNodeMat, parentLocalNodeMat, parentLocalNodeAnimMat;
		parentGlobalNodeMat.SetIdentity();
		parentLocalNodeMat.SetIdentity();
		parentLocalNodeAnimMat.SetIdentity();
		if (camerabone->GetParent(false) && camerabone->GetParent(false)->IsNull()) {
			//eNullノードのアニメーションに対応するために　timeはroundingframe
			camerabone->GetParent(false)->CalcLocalNodePosture(bindposeflag, 0, cameramotid, roundingframe, &parentLocalNodeMat, &parentLocalNodeAnimMat);

			////parentGlobalNodeMat = camerabone->GetParent(false)->GetENullMatrix(roundingframe);//global
			parentGlobalNodeMat = camerabone->GetParent(false)->GetTransformMat(cameramotid, roundingframe, true);//global
		}
		else {
			parentGlobalNodeMat.SetIdentity();
			parentLocalNodeMat.SetIdentity();
		}

		ChaVector3 nodepos = ChaMatrixTraVec(camerabone->GetNodeMat());

		ChaMatrix adjusttra;
		adjusttra.SetIdentity();
		adjusttra.SetTranslation(curcamera->adjustpos);


		switch (inheritmode) {
		case CAMERA_INHERIT_ALL:
			//transformmat = nodemat * cammat;
			transformmat = localnodeanimmat * parentGlobalNodeMat;//ParentRot有り
			break;

		case CAMERA_INHERIT_CANCEL_NULL1:
			//transformmat = nodemat * cammat * ChaMatrixInv(rootmat);
			transformmat = localnodeanimmat;//前 ##########
			break;

		case CAMERA_INHERIT_CANCEL_NULL2:
			//################################################################################################
			//UnityにおいてはRootMotionチェックオン. Mayaにおいてはトランスフォームの継承チェックオフ　に相当
			//################################################################################################
			//transformmat = cammat * ChaMatrixInv(lcltramat) * ChaMatrixInv(rootmat);
			//transformmat = localnodeanimmat * adjusttra * parentLocalNodeMat;//###################### ParentRot無し TheHunt Street1 Camera1

			//2023/07/24
			//CalcLocalNodePosture()変更により　Rot無しを明示的に数式にする必要
			//( CalcLocalNodePosture()のnodematは　GetRotationActiveの値により回転を持つことがある )
			transformmat = localnodeanimmat * adjusttra * ChaMatrixTra(parentLocalNodeMat);

			break;

		default:
			_ASSERT(0);
			transformmat = localnodeanimmat;
			break;
		}
		//ChaMatrix retmat;
		//if (multInvNodeMat) {
		//	retmat = ChaMatrixInv(GetCameraNodeMat(cameramotid)) * transformmat;
		//}
		//else {
		//	retmat = transformmat;
		//}

		if (setmotionpoint) {
			int existflag = 0;
			CMotionPoint* cameramp = camerabone->AddMotionPoint(cameramotid, roundingframe, &existflag);
			if (cameramp) {
				cameramp->SetWorldMat(transformmat);
				cameramp->SetLocalMat(localnodeanimmat);
			}
			else {
				_ASSERT(0);
			}

			int existflag2 = 0;
			CMotionPoint* enullmp = camerabone->GetParent(false)->AddMotionPoint(cameramotid, roundingframe, &existflag2);
			if (enullmp) {
				enullmp->SetWorldMat(parentGlobalNodeMat);
				enullmp->SetLocalMat(parentLocalNodeAnimMat);
			}
			else {
				_ASSERT(0);
			}

		}



		return transformmat;
	}
}



//#################################################################
//inheritmode  0: writemode, 1:inherit all, 2:inherit cancel and Lclcancel
//#################################################################
int CCameraFbx::GetCameraAnimParams(int cameramotid, double nextframe, double camdist, 
	ChaVector3* pEyePos, ChaVector3* pTargetPos, ChaVector3* pcamupvec, ChaMatrix* protmat, int inheritmode)
{

	//if (!pEyePos || !pTargetPos || (cameramotid <= 0)) {
	if (!pEyePos || !pTargetPos || !pcamupvec) {//2023/05/29 cameramotid <= 0のときには　zeroframeカメラ位置をセット
		//###################################################
		//protmatがNULLの場合も許可　rotmatをセットしないだけ
		//###################################################
		_ASSERT(0);
		return 1;
	}


	ChaVector3 zeropos = ChaVector3(0.0f, 0.0f, 0.0f);
	//double roundingframe = RoundingTime(nextframe);

	if (IsLoaded()) {

		CAMERANODE* curcamera = GetCameraNode(cameramotid);//カメラがあるがカメラアニメが無い場合にもnullを返す

		if (curcamera && (cameramotid > 0)) {



			//if (roundingframe != 0.0) {
			if (nextframe != 0.0) {
				bool multInvNodeMat = false;
				ChaMatrix transformmat;
				transformmat.SetIdentity();
				bool calcbynode = false;
				bool setmotionpoint = false;
				transformmat = GetCameraTransformMat(cameramotid, nextframe, inheritmode,
					calcbynode, setmotionpoint);


				//##############
				//カメラの位置
				//##############
				//ChaMatrix transformmat = nodemat * cammat * ChaMatrixInv(lcltramat) * ChaMatrixInv(rootmat);	 
				//ChaVector3TransformCoord(pEyePos, &nodepos, &transformmat);
				ChaVector3TransformCoord(pEyePos, &zeropos, &transformmat);

				//##############
				//カメラの向き
				//##############
				ChaVector3 dirvec0;
				ChaVector3 dirvec;
				dirvec0 = ChaVector3(-1.0f, 0.0f, 0.0f);
				dirvec = ChaVector3(-1.0f, 0.0f, 0.0f);

				ChaMatrix convmat = ChaMatrixRot(transformmat);
				ChaVector3TransformCoord(&dirvec, &dirvec0, &convmat);
				ChaVector3Normalize(&dirvec, &dirvec);


				//###############
				//カメラの注視点
				//###############
				*pTargetPos = *pEyePos + dirvec * camdist;


				//#######################
				//カメラのアップベクトル
				//#######################
				//2023/06/25
				//ChaVector3 upvec = ChaMatrixRot(convmat).GetRow(1);
				//ChaVector3Normalize(&upvec, &upvec);
				//*pcamupvec = upvec;
				ChaVector3 firstupdir = ChaVector3(0.01f, 0.99f, 0.0f);
				ChaVector3 cameraupdir = ChaVector3(0.01f, 0.99f, 0.0f);
				ChaVector3TransformCoord(&cameraupdir, &firstupdir, &convmat);
				ChaVector3Normalize(&cameraupdir, &cameraupdir);
				*pcamupvec = cameraupdir;


				if (protmat) {
					*protmat = convmat;
				}
			}
			else {
				//cameraanimがある場合にも　0フレームは別計算　ZeroFrameCamera

				*pEyePos = curcamera->position;
				*pTargetPos = *pEyePos + curcamera->dirvec * camdist;
				*pcamupvec = curcamera->upvec;

				if (protmat) {
					*protmat = ChaMatrixRot(curcamera->worldmat);
				}
			}
		}
		else {
			//ZeroFrameCamera

			//モーションを持たない複数カメラ切り替えに対して　現在GUIが未対応　最初のカメラの0フレームの位置と向きを返す
			CAMERANODE* curcamera0 = 0;
			curcamera0 = GetFirstValidCameraNode();//anim無しのカメラ用
			if (!curcamera0) {
				//何もしない
				return 0;
			}

			*pEyePos = curcamera0->position;
			*pTargetPos = *pEyePos + curcamera0->dirvec * camdist;
			*pcamupvec = curcamera0->upvec;

			if (protmat) {
				*protmat = ChaMatrixRot(curcamera0->worldmat);
			}
		}
	}
	else {
		//何もしない
	}


	return 0;
}


ChaVector3 CCameraFbx::CalcCameraFbxEulXYZ(int cameramotid, double srcframe)
{
	ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);


	CAMERANODE* curcamera = FindCameraNodeByMotId(cameramotid);
	if (!curcamera) {
		//fbxにカメラが含まれていない場合　処理せずリターン
		_ASSERT(0);
		return cureul;
	}
	if (!curcamera->IsValid()) {
		//必要な情報が読み込めていない場合　エラー
		_ASSERT(0);
		return cureul;
	}


	double roundingframe = RoundingTime(srcframe);
	m_time = roundingframe;
	FbxTime fbxtime;
	fbxtime.SetSecondDouble(m_time / 30.0);
	ChaVector3 zeropos = ChaVector3(0.0f, 0.0f, 0.0f);

	int notmodify180flag;
	if (roundingframe <= 1.0) {
		notmodify180flag = 1;
	}
	else {
		notmodify180flag = 0;
	}


	if (IsLoaded()) {
		if (cameramotid > 0) {
			FbxNode* cameranode = curcamera->pnode;
			CBone* camerabone = curcamera->pbone;
			if (cameranode && camerabone) {

				EFbxRotationOrder rotationorder;
				cameranode->GetRotationOrder(FbxNode::eSourcePivot, rotationorder);

				FbxVector4 orgfbxeul = cameranode->EvaluateLocalRotation(fbxtime, FbxNode::eSourcePivot, true, true);
				cureul = ChaVector3((float)orgfbxeul[0], (float)orgfbxeul[1], (float)orgfbxeul[2]);

			}
		}
	}

	return cureul;
}





//CCameraFbx CCameraFbx::operator= (CCameraFbx srccamera)
//{
//	m_loadedflag = srccamera.m_loadedflag;
//
//	m_position = srccamera.m_position;
//	m_dirvec = srccamera.m_dirvec;
//	m_worldmat = srccamera.m_worldmat;
//
//	m_upVector = srccamera.m_upVector;
//	m_aspectHeight = srccamera.m_aspectHeight;
//	m_aspectWidth = srccamera.m_aspectWidth;
//	m_nearZ = srccamera.m_nearZ;
//	m_farZ = srccamera.m_farZ;
//	m_aspectRatio = srccamera.m_aspectRatio;
//	m_inch_mm = srccamera.m_inch_mm;
//	m_filmHeight = srccamera.m_filmHeight;
//	m_focalLength = srccamera.m_focalLength;
//	m_filmHeight_mm = srccamera.m_filmHeight_mm;
//	m_fovY = srccamera.m_fovY;
//	m_fovY_Degree = srccamera.m_fovY_Degree;
//
//
//	return *this;
//}


bool CCameraFbx::IsLoaded()
{
	return m_loadedflag;
}

CAMERANODE* CCameraFbx::GetFirstValidCameraNode()
{
	//anim無しカメラ用
	std::vector<CAMERANODE*>::iterator itrcameranode;
	for (itrcameranode = m_cameranode.begin(); itrcameranode != m_cameranode.end(); itrcameranode++) {
		CAMERANODE* curcn = *itrcameranode;
		if (curcn) {
			if (curcn->IsValid()) {
				return curcn;//!!!!!!!!!!!
			}
		}
	}

	return 0;//みつからなかった場合
}




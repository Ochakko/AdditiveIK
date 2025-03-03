#include "stdafx.h"
#include <windows.h>
#include <math.h>


//#ifdef CONVD3DX11
//#include <d3dcommon.h>
//#include <dxgi.h>
//#include <d3d10_1.h>
//#include <d3d10.h>
//#include <d3dcompiler.h>
//#include <d3dx10.h>
//#endif

#include <ChaCalcFunc.h>
#include <GlobalVar.h>
#include <Model.h>
#include <Bone.h>
#include <MotionPoint.h>
#include <EditRange.h>
#include <RIgidElem.h>
#include <Collision.h>
#include <FbxMisc.h>

#define DBGH
#include <dbg.h>

#include "../Examples/CommonInterfaces/CommonExampleInterface.h"
#include "../Examples/CommonInterfaces/CommonGUIHelperInterface.h"
#include "../BTSRC/BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "../BTSRC/BulletCollision/CollisionShapes/btCollisionShape.h"
#include "../BTSRC/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h"


#include "../BTSRC/LinearMath/btTransform.h"
#include "../BTSRC/LinearMath/btHashMap.h"
//#include "../Examples/MultiThreadedDemo/CommonRigidBodyMTBase.h"
//#include "../Examples/CommonInterfaces/CommonMultiBodyBase.h"

#include "../Examples/CommonInterfaces/CommonRigidBodyBase.h"
#include "../Examples/MultiThreadedDemo/CommonRigidBodyMTBase.h"


#include "../BTSRC/btBulletDynamicsCommon.h"
#include "../BTSRC/LinearMath/btIDebugDraw.h"

#include <math.h>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

#include <crtdbg.h>

using namespace std;


//extern bool g_wmatDirectSetFlag;//!!!!!!!!!!!!
//extern bool g_underIKRot;
extern bool g_x180flag;
extern CRITICAL_SECTION g_CritSection_FbxSdk;


#define EULLIMITPLAY	1


int ChaCalcFunc::ModifyEuler360(ChaVector3* eulerA, ChaVector3* eulerB, int notmodify180flag, float throundX, float throundY, float throundZ)
{
	//#########################################################
	// 2022/12/04
	//+-180dgreeに制限せずに　オイラー角を連続させるための関数
	//#########################################################

	//###########################################################################################
	//2023/02/04
	//当たり前のことだが　XYZEul(180, 0, 180)とXYZEul(0, 0, 0)は違う姿勢
	//360度のプラスマイナスは有りだが　180度のプラスマイナスは　違う姿勢にすること
	//ノイズ対策として+-180度は有り得るが
	//同じ姿勢の別表現としての+-180度は　XYZEul(0, 180, 0)をXYZEul(180, 0, 180)にする以外に思いつかない
	//360のプラスマイナスに戻して　後処理として補正を行う
	//###########################################################################################


	float tmpX0, tmpY0, tmpZ0;
	if (notmodify180flag == 0) {
		if (eulerA->x >= 0.0f) {
			tmpX0 = eulerA->x + 360.0f * GetRoundThreshold((eulerB->x - eulerA->x) / 360.0f, throundX);
		}
		else {
			tmpX0 = eulerA->x - 360.0f * GetRoundThreshold((eulerA->x - eulerB->x) / 360.0f, throundX);
		}
		if (eulerA->y >= 0.0f) {
			tmpY0 = eulerA->y + 360.0f * GetRoundThreshold((eulerB->y - eulerA->y) / 360.0f, throundY);
		}
		else {
			tmpY0 = eulerA->y - 360.0f * GetRoundThreshold((eulerA->y - eulerB->y) / 360.0f, throundY);
		}
		if (eulerA->z >= 0.0f) {
			tmpZ0 = eulerA->z + 360.0f * GetRoundThreshold((eulerB->z - eulerA->z) / 360.0f, throundZ);
		}
		else {
			tmpZ0 = eulerA->z - 360.0f * GetRoundThreshold((eulerA->z - eulerB->z) / 360.0f, throundZ);
		}


		////角度変化の大きさ
		//double s0 = ((double)eulerB->x - eulerA->x) * ((double)eulerB->x - eulerA->x) +
		//	((double)eulerB->y - eulerA->y) * ((double)eulerB->y - eulerA->y) +
		//	((double)eulerB->z - eulerA->z) * ((double)eulerB->z - eulerA->z);
		//double s1 = ((double)eulerB->x - tmpX0) * ((double)eulerB->x - tmpX0) +
		//	((double)eulerB->y - tmpY0) * ((double)eulerB->y - tmpY0) +
		//	((double)eulerB->z - tmpZ0) * ((double)eulerB->z - tmpZ0);
		//
		//if (s0 <= s1) {
		//	//そのまま
		//}
		//else {
		//	eulerA->x = tmpX0;
		//	eulerA->y = tmpY0;
		//	eulerA->z = tmpZ0;
		//}

		eulerA->x = tmpX0;
		eulerA->y = tmpY0;
		eulerA->z = tmpZ0;
	}
	else {
		//そのまま
	}

	////############################################################################################
	////Q2EulXYZにaxisqを指定して呼び出した場合
	////invaxisq * *this * axisqによって　１８０度分オイラー角が回転することがあるので対策
	//// ただし　befframeが0フレームの場合には　１８０度分回転チェックはしない(１８０度回転を許す)
	////############################################################################################
	//if (notmodify180flag == 0) {
	//	float thdeg = 165.0f;
	//	if ((tmpX0 - eulerB->x) >= thdeg) {
	//		tmpX0 -= 180.0f;
	//	}
	//	if ((eulerB->x - tmpX0) >= thdeg) {
	//		tmpX0 += 180.0f;
	//	}

	//	if ((tmpY0 - eulerB->y) >= thdeg) {
	//		tmpY0 -= 180.0f;
	//	}
	//	if ((eulerB->y - tmpY0) >= thdeg) {
	//		tmpY0 += 180.0f;
	//	}

	//	if ((tmpZ0 - eulerB->z) >= thdeg) {
	//		tmpZ0 -= 180.0f;
	//	}
	//	if ((eulerB->z - tmpZ0) >= thdeg) {
	//		tmpZ0 += 180.0f;
	//	}
	//}

	return 0;
}
int ChaCalcFunc::GetRoundThreshold(float srcval, float degth)
{
	//GetRound()では　180度以上のずれを１回転で補正していた
	//ChaGetRoundThreshold()では　１回転よりどれだけ小さい角度で一回転とみなすか(使う側で足すのは３６０度単位なので姿勢は変わらない)を指定する(軸ごとに)

	float th360;
	th360 = degth / 360.0f;

	if (srcval > 0.0f) {
		return (int)(srcval + th360);
	}
	else {
		return (int)(srcval - th360);
	}

	return 0;
}
int ChaCalcFunc::GetBefNextMP(CBone* srcbone, int srcmotid, double srcframe, CMotionPoint** ppbef, CMotionPoint** ppnext, int* existptr, bool onaddmotion)
{


	if (!srcbone || !ppbef || !ppnext || !existptr) {
		_ASSERT(0);
		return 1;
	}
	EnterCriticalSection(&(srcbone->m_CritSection_GetBefNext));


	//2023/04/28 2023/05/23
	if (srcbone->IsNotSkeleton() && srcbone->IsNotCamera() && srcbone->IsNotNull()) {
		if (ppbef) {
			*ppbef = 0;
		}
		if (ppnext) {
			*ppnext = 0;
		}
		if (existptr) {
			*existptr = 0;
		}
		LeaveCriticalSection(&(srcbone->m_CritSection_GetBefNext));
		return 0;
	}



	CMotionPoint* pbef = 0;
	//CMotionPoint* pcur = m_motionkey[srcmotid -1];
	CMotionPoint* pcur = 0;
	//std::map<int, std::vector<CMotionPoint*>>::iterator itrvecmpmap;


	int curframeindex = IntTime(srcframe);
	int nextframeindex = curframeindex + 1;
	int mpmapleng = 0;//2022/11/01 STLのsize()は重いらしいので変数に代入して使いまわし

	*existptr = 0;

	//if ((srcmotid <= 0) || (srcmotid > srcbone->GetMotionKeySize())) {
	if (srcmotid <= 0) {//2024/06/10
		//AddMotionPointから呼ばれるときに通る場合は正常
		*ppbef = 0;
		*ppnext = 0;
		//_ASSERT(0);
		LeaveCriticalSection(&(srcbone->m_CritSection_GetBefNext));
		return 0;
	}
	else {
		if (curframeindex < 0) {
			*ppbef = 0;
			*ppnext = 0;

			//if (srcmotid >= 1) {
			//	m_cachebefmp[srcmotid - 1] = NULL;
			//}
			////_ASSERT(0);
			LeaveCriticalSection(&(srcbone->m_CritSection_GetBefNext));
			return 0;
		}
		else {
			pcur = srcbone->GetMotionKey(srcmotid);
		}
	}

	bool getbychain;
	getbychain = onaddmotion;


	//if (getbychain == false) {
	//	//get by indexed のフラグ指定の場合にもindexedの準備が出来ていない場合はget by chainで取得する
	//	if (srcbone->GetInitIndexedMotionPointSize() <= srcmotid) {//エントリーがまだ無いとき
	//		getbychain = true;
	//	}
	//	else {
	//		getbychain = !(srcbone->ExistInitIndexedMotionPoint(srcmotid));
	//	}
	//}
	//if (getbychain == false) {
	//	//indexのframe長のチェック
	//	mpmapleng = srcbone->GetIndexedMotionPointFrameLeng(srcmotid);
	//	if ((mpmapleng <= 0) || (curframeindex >= mpmapleng)) {// ##### 2023/10/17_1  "&&" になっていた #####
	//		getbychain = true;
	//	}
	//}


	//std::vector<CMotionPoint*> mpvec;
	//mpvec.clear();
	//srcbone->GetIndexedMotionPointVec(srcmotid, mpvec);
	//if (getbychain == false) {
	//	if (mpvec.empty()) {
	//		getbychain = true;
	//	}
	//	else {
	//		mpmapleng = (int)mpvec.size();
	//		if ((mpmapleng <= 0) || (curframeindex >= mpmapleng)) {
	//			getbychain = true;
	//		}
	//	}
	//}


	//2023/10/27　1.2.0.27 RC11
	// 最適化
	//indexedMotionPointの内　ここで使用するのは　モーションポイント３つだけ
	//vector<>全てをコピーする必要は無い
	CMotionPoint* pcurframemp = 0;
	CMotionPoint* pnextframemp = 0;
	CMotionPoint* pendframemp = 0;
	mpmapleng = 0;
	int result1 = srcbone->GetIndexedMotionPoint3(srcmotid, curframeindex, nextframeindex, &pcurframemp, &pnextframemp, &pendframemp, &mpmapleng);
	if (result1 != 0) {//正常時0, エラー時1, empty時は2
		getbychain = true;//index使用準備が出来ていないのでチェインを使用
	}
	else if ((mpmapleng <= 0) || (curframeindex >= mpmapleng)) {
		getbychain = true;//index使用準備が出来ていないのでチェインを使用
	}




	if (getbychain == true) {
#ifdef USE_CACHE_ONGETMOTIONPOINT__
				//キャッシュをチェックする


		if ((srcmotid >= 1) && (srcmotid <= MAXMOTIONNUM)) {
			CMotionPoint* chkcache = srcbone->GetMPCache(srcmotid);//2023/10/27 引数としてはsrcmotid　内部で[srcmotid - 1]でデータ参照
			if (chkcache &&
				(chkcache->GetUseFlag() == 1) &&
				//((m_cachebefmp[srcmotid - 1])->GetFrame() <= (srcframe + 0.0001))) {
				(chkcache->GetFrame() <= ((double)curframeindex + 0.0001))) {//2022/12/26
				//高速化のため途中からの検索にする
				
				pcur = chkcache;
			}
			else {
				//上方でセットしたsrcbone->GetMotionKey(srcmotid);のまま
				int dbgflag0 = 1;
			}
		}
#endif

		if (!pcur) {
			int dbgflag1 = 1;
		}


		while (pcur) {

			if (IsJustEqualTime(pcur->GetFrame(), srcframe)) {//ジャスト判定　ジャストの場合補間無し
				//if ((pcur->GetFrame() >= ((double)curframeindex - 0.0001)) && (pcur->GetFrame() <= ((double)curframeindex + 0.0001))) {//2022/12/26 これでは補間が効かない
				*existptr = 1;
				pbef = pcur;
				break;
			}
			else if (pcur->GetFrame() > srcframe) {//過ぎてしまった場合
				//else if (pcur->GetFrame() > ((double)curframeindex + 0.0001)) {//2022/12/26
				*existptr = 0;
				break;
			}
			else {
				//for loop
				pbef = pcur;
				pcur = pcur->GetNext();
			}
		}
		*ppbef = pbef;//過ぎてしまった１つ前のモーションポイントをセット

		if (*existptr) {
			*ppnext = pbef->GetNext();
		}
		else {
			*ppnext = pcur;
		}

#ifdef USE_CACHE_ONGETMOTIONPOINT__
		//m_cachebefmp = pbef;
		if ((srcmotid >= 1) && (srcmotid <= MAXMOTIONNUM)) {

			if (*ppbef) {
				//キャッシュはインデックス作成前の段階で　MotionPointへアクセスする場合に　高速化の助けになる
				//InitMpFrame()のマルチスレッドからアクセスすることを想定して
				//INITMP_THREADSNUM * 2個分prevへ遡ってからキャッシュへセットする
				CMotionPoint* chkmp = *ppbef;
				CMotionPoint* pcache = *ppbef;
				int backno;
				const int backnum = 2;
				for (backno = 0; backno < backnum; backno++) {//どのくらい異なる範囲からアクセスするかによる　backnumはとりあえずの値
					if (chkmp) {
						pcache = chkmp;
					}
					else {
						break;
					}
					chkmp = chkmp->GetPrev();
				}
				srcbone->SetMPCache(srcmotid, chkmp);//2023/10/27 引数としてはsrcmotid　内部で[srcmotid - 1]でデータセット
			}
			else {
				//srcbone->SetMPCache(srcmotid - 1, NULL);
			}

			//if (*ppbef) {
			//	if ((*ppbef)->GetPrev()) {
			//		m_cachebefmp[srcmotid - 1] = (*ppbef)->GetPrev();
			//	}
			//	else {
			//		m_cachebefmp[srcmotid - 1] = (*ppbef);
			//	}
			//}
			//else {
			//	//m_cachebefmp[srcmotid - 1] = m_motionkey[srcmotid - 1];
			//	m_cachebefmp[srcmotid - 1] = NULL;
			//}
		}
#endif


	}
	else {

		//### 2022/11/01 ################
		//最初の方でチェック済なので不要
		//###############################
		//if ((srcmotid <= 0) || (srcmotid > m_indexedmotionpoint.size())) {
		//	//AddMotionPointから呼ばれるときに通る場合は正常
		//	*ppbef = 0;
		//	*ppnext = 0;
		//	//_ASSERT(0);
		//	return 0;
		//}
		//else {
		//	itrvecmpmap = m_indexedmotionpoint.find(srcmotid);
		//	if (itrvecmpmap == m_indexedmotionpoint.end()) {
		//		*ppbef = 0;
		//		*ppnext = 0;
		//		//_ASSERT(0);
		//		return 0;

		//	}
		//}

		//CMotionPoint* testmp = (itrvecmpmap->second)[curframeindex];


		if (curframeindex < mpmapleng) {
			//*ppbef = srcbone->GetIndexedMotionPoint(srcmotid, curframeindex);
			//*ppbef = mpvec[curframeindex];
			*ppbef = pcurframemp;
		}
		else {
			if (mpmapleng >= 1) {
				//*ppbef = srcbone->GetIndexedMotionPoint(srcmotid, mpmapleng - 1);
				//*ppbef = mpvec[mpmapleng - 1];
				*ppbef = pendframemp;
			}
			else {
				*ppbef = 0;
			}
		}

		if (*ppbef) {
			double mpframe = (*ppbef)->GetFrame();
			//if ((mpframe >= ((double)curframeindex - 0.0001)) && (mpframe <= ((double)curframeindex + 0.0001))) {

			//2023/08/02
			//補間計算時には
			//GetBefNextMPには(m_curmp)以外の場合　端数在りの時間を渡す　justの計算も端数在りでする
			//if分を以下のようにしないと　モーションによっては　0.007倍速などでカクカクする　変更前でもモーション時間がたまたまintの場合には滑らかだった
			//上のは補間計算時の話　モーションのキーの時間はintに揃えてリサンプリングして読み込んでいる
			//モーションデータをインデックス化していない場合の滑らか処理の修正は　上の方のコードで2022/12/26に修正済　今回の修正はインデックス化されたデータについての修正
			if (IsJustEqualTime(mpframe, srcframe)) {
				*existptr = 1;
			}
			else {
				*existptr = 0;
			}


			if (nextframeindex < mpmapleng) {
				//*ppnext = srcbone->GetIndexedMotionPoint(srcmotid, nextframeindex);
				//*ppnext = mpvec[nextframeindex];
				*ppnext = pnextframemp;
			}
			else {
				if (mpmapleng >= 1) {
					//*ppnext = srcbone->GetIndexedMotionPoint(srcmotid, mpmapleng - 1);
					*ppnext = pendframemp;
				}
				else {
					*ppnext = 0;
				}

			}
		}
		else {
			*ppnext = 0;
			*existptr = 0;
		}
	}

	LeaveCriticalSection(&(srcbone->m_CritSection_GetBefNext));

	return 0;
}

int ChaCalcFunc::IKRotateOneFrame(CModel* srcmodel, int limitdegflag, int wallscrapingikflag, CEditRange* erptr,
	int keyno, CBone* rotbone, CBone* parentbone,
	int srcmotid, double curframe, double startframe, double applyframe,
	CQuaternion rotq0, bool keynum1flag, bool postflag, bool fromiktarget,
	ChaMatrix* srcapplymat
)
{

	//for return value
	int ismovable = 1;

	//if (!srcmodel || !erptr || !rotbone || !parentbone) {
	if (!srcmodel || !rotbone || !parentbone) {
		_ASSERT(0);
		return 0;//not move
	}

	bool underfootrig = srcmodel->GetUnderFootRig();//FootRig時にはinfooutしないように


	if (rotbone->IsNotSkeleton() && !rotbone->IsNullAndChildIsCamera()) {
		return 0;//not move
	}

	CQuaternion qForRot;
	CQuaternion qForHipsRot;

	if (fromiktarget == true) {
		qForRot = rotq0;
		qForHipsRot = rotq0;

		//bool calcaplyflag = false;
		//CalcQForRot(limitdegflag, calcaplyflag, 
		//	srcmotid, curframe, applyframe, rotq0,
		//	parentbone, parentbone,
		//	&qForRot, &qForHipsRot);

		//IKTragetの場合には
		//0.080で刻んで　徐々に近づける
		//近づきが足りない場合は　処理後に　ConstExecuteボタンを押す
		CQuaternion endq;
		CQuaternion curqForRot;
		CQuaternion curqForHipsRot;
		endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		qForRot.Slerp2(endq, 0.080f, &curqForRot);
		curqForHipsRot = curqForRot;
		bool infooutflag = !underfootrig;
		ismovable = rotbone->RotAndTraBoneQReq(limitdegflag, wallscrapingikflag, 0, RoundingTime(startframe),
			infooutflag, 0, srcmotid, curframe, curqForRot, curqForHipsRot, fromiktarget);

		////bool infooutflag = true;
		////parentbone->RotAndTraBoneQReq(limitdegflag, 0, RoundingTime(startframe),
		////	infooutflag, 0, srcmotid, curframe, qForRot, qForHipsRot, fromiktarget);

	}
	else if (keynum1flag) {
		qForRot = rotq0;
		qForHipsRot = rotq0;

		//bool calcaplyflag = false;
		//CalcQForRot(limitdegflag, calcaplyflag, 
		//	srcmotid, curframe, applyframe, rotq0,
		//	parentbone, parentbone,
		//	&qForRot, &qForHipsRot);

		bool infooutflag = !underfootrig;
		ismovable = rotbone->RotAndTraBoneQReq(limitdegflag, wallscrapingikflag, 0, RoundingTime(startframe),
			infooutflag, 0, srcmotid, curframe, qForRot, qForHipsRot, fromiktarget);

		if ((fromiktarget != true) && (postflag != true) && erptr) {
			IKTargetVec(srcmodel, limitdegflag, wallscrapingikflag, erptr, srcmotid, curframe, postflag);
		}
	}
	else {
		if (g_pseudolocalflag == 1) {
			bool calcaplyflag = true;
			CalcQForRot(limitdegflag, calcaplyflag,
				srcmotid, curframe, applyframe, rotq0,
				rotbone, parentbone,
				&qForRot, &qForHipsRot, srcapplymat, fromiktarget);
		}
		else {
			qForRot = rotq0;
			qForHipsRot = rotq0;
		}

		double changerate;
		//if ((srcapplymat != nullptr) && (fromiktarget == false)) {
		//	changerate = (double)(*(g_motionbrush_value + (int)curframe));
		//}
		//else {
		//	changerate = 1.0;
		//}
		changerate = 1.0;


		bool infooutflag;
		if (IsEqualRoundingTime(curframe, applyframe)) {
			infooutflag = !underfootrig;
		}
		else {
			infooutflag = false;
		}

		//double firstframe = 0.0;
		//if (keyno == 0) {
		//	firstframe = curframe;
		//}
		//if (g_absikflag == 0) {
		if (g_slerpoffflag == 0) {
			CQuaternion endq;
			CQuaternion curqForRot;
			CQuaternion curqForHipsRot;
			endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);


			curqForRot = qForRot;
			curqForHipsRot = qForHipsRot;

			//addrotだけにウェイトを掛ける必要があるので　ここではslerpしない
			//qForRot.Slerp2(endq, 1.0 - changerate, &curqForRot);
			//qForHipsRot.Slerp2(endq, 1.0 - changerate, &curqForHipsRot);

			//curqForRot = qForRot * changerate + endq * (1.0 - changerate);
			//curqForRot.normalize();
			//curqForHipsRot = qForHipsRot * changerate + endq * (1.0 - changerate);
			//curqForHipsRot.normalize();


			ismovable = rotbone->RotAndTraBoneQReq(limitdegflag, wallscrapingikflag, 0, RoundingTime(startframe),
				infooutflag, 0, srcmotid, curframe, curqForRot, curqForHipsRot, fromiktarget);
		}
		else {
			ismovable = rotbone->RotAndTraBoneQReq(limitdegflag, wallscrapingikflag, 0, RoundingTime(startframe),
				infooutflag, 0, srcmotid, curframe, qForRot, qForHipsRot, fromiktarget);
		}
		//}
		//else {
		//	if (keyno == 0) {
		//		ismovable = rotbone->RotAndTraBoneQReq(limitdegflag, 0, RoundingTime(startframe),
		//			infooutflag, 0, srcmotid, curframe, qForRot, qForHipsRot, fromiktarget);
		//	}
		//	else {
		//		rotbone->SetAbsMatReq(limitdegflag, 0, srcmotid, curframe, firstframe);
		//	}
		//}


		if ((fromiktarget != true) && (postflag != true) && erptr) {
			IKTargetVec(srcmodel, limitdegflag, wallscrapingikflag, erptr, srcmotid, curframe, postflag);
		}
	}

	return ismovable;
}


int ChaCalcFunc::CalcQForRot(bool limitdegflag, bool calcaplyflag,
	int srcmotid, double srcframe, double srcapplyframe, CQuaternion srcaddrot,
	CBone* srcrotbone, CBone* srcaplybone,
	CQuaternion* dstqForRot, CQuaternion* dstqForHipsRot,
	ChaMatrix* srcapplymat, bool srcfromiktarget
)
{
	if (!srcrotbone || !srcaplybone || !dstqForRot || !dstqForHipsRot) {
		//srcapplymatはnullptrも有
		_ASSERT(0);
		return 1;
	}

	if (srcrotbone->IsNotSkeleton() && !srcrotbone->IsNullAndChildIsCamera()) {
		dstqForRot->SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		dstqForHipsRot->SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		return 0;
	}

	double roundingframe = RoundingTime(srcframe);
	double roundingapplyframe = RoundingTime(srcapplyframe);


	ChaMatrix currotmat;
	ChaMatrix invcurrotmat;
	//if (srcaplybone != nullptr) {
	//	currotmat = srcaplybone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
	//	//currotmat.SetTranslationZero();
	//	invcurrotmat = ChaMatrixInv(currotmat);
	//	//invcurrotmat.SetTranslationZero();
	//}
	//else {
		currotmat = srcrotbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		//currotmat.SetTranslationZero();
		invcurrotmat = ChaMatrixInv(currotmat);
		//invcurrotmat.SetTranslationZero();
	//}






	ChaMatrix aplyparrotmat, invaplyparrotmat;
	ChaMatrix curparmat, invcurparmat;
	//aplyparrotmat = srcrotbone->GetWorldMat(limitdegflag, srcmotid, roundingapplyframe, 0);
	////aplyparrotmat.SetTranslationZero();
	//invaplyparrotmat = ChaMatrixInv(aplyparrotmat);
	////invaplyparrotmat.SetTranslationZero();

	if (srcapplymat == nullptr) {
		if (srcaplybone != nullptr) {
			aplyparrotmat = srcaplybone->GetWorldMat(limitdegflag, srcmotid, roundingapplyframe, 0);
			//aplyparrotmat.SetTranslationZero();
			invaplyparrotmat = ChaMatrixInv(aplyparrotmat);
			//invaplyparrotmat.SetTranslationZero();

			curparmat = srcaplybone->GetWorldMat(limitdegflag, srcmotid, RoundingTime(srcframe), nullptr);
			invcurparmat = ChaMatrixInv(curparmat);
		}
		else {
			//aplyparrotmat.SetIdentity();
			//invaplyparrotmat.SetIdentity();
			aplyparrotmat = srcrotbone->GetWorldMat(limitdegflag, srcmotid, roundingapplyframe, 0);
			//aplyparrotmat.SetTranslationZero();
			invaplyparrotmat = ChaMatrixInv(aplyparrotmat);
			//invaplyparrotmat.SetTranslationZero();

			curparmat.SetIdentity();;
			invcurparmat.SetIdentity();
		}
	//	if (srcrotbone) {
	//		aplyparrotmat = srcrotbone->GetWorldMat(limitdegflag, srcmotid, roundingapplyframe, 0);
	//		aplyparrotmat.SetTranslationZero();
	//		invaplyparrotmat = ChaMatrixInv(aplyparrotmat);
	//		invaplyparrotmat.SetTranslationZero();
	//	}
	//	else {
	//		aplyparrotmat.SetIdentity();
	//		invaplyparrotmat.SetIdentity();
	//	}
	}
	else {
		aplyparrotmat = *srcapplymat;
		//aplyparrotmat.SetTranslationZero();
		invaplyparrotmat = ChaMatrixInv(aplyparrotmat);
		//invaplyparrotmat.SetTranslationZero();

		if (srcaplybone != nullptr) {
			curparmat = srcaplybone->GetWorldMat(limitdegflag, srcmotid, RoundingTime(srcframe), nullptr);
			invcurparmat = ChaMatrixInv(curparmat);
		}
		else {
			curparmat.SetIdentity();;
			invcurparmat.SetIdentity();
		}
	}


	//CQuaternion invcurparrotq, aplyparrotq, invaplyparrotq, curparrotq;
	//invcurparrotq.RotationMatrix(invcurparrotmat);
	//aplyparrotq.RotationMatrix(aplyparrotmat);
	//invaplyparrotq.RotationMatrix(invaplyparrotmat);
	//curparrotq.RotationMatrix(curparrotmat);


	//意味：RotBoneQReq()にrotqを渡し　currentworldmatの後ろに　invpivot * rotq * pivotを掛ける
	//つまり　A = currentworldmat, B = localq.MakeRotMatX()とすると A * (invA * B * A)
	ChaMatrix transmat2ForRot;
	ChaMatrix transmat2ForHipsRot;

	//hisp移動はうまくいくが　回転がおかしい 　hips以外は良い
	//transmat2 = invcurparrotmat * aplyparrotmat * localq.MakeRotMatX() * invaplyparrotmat * curparrotmat;//bef

	//hips回転はうまくいくが　移動がおかしい
	//transmat2 = localq.MakeRotMatX();//for hips edit

	//####################################################################
	//ToDo : RotQBoneReq2()を作って　引数として上記２つの回転情報を渡す
	//####################################################################

	//if (calcaplyflag == true) {
	//	transmat2ForRot = invcurparrotmat * aplyparrotmat * srcaddrot.MakeRotMatX() * invaplyparrotmat * curparrotmat;//bef
	//}
	//else {
	//	transmat2ForRot = invcurparrotmat * srcaddrot.MakeRotMatX() * curparrotmat;//bef
	//	//transmat2ForRot = srcaddrot.MakeRotMatX();//bef
	//}


	//##################################
	//ブラシのウェイトはAddRotにだけ適用する
	//##################################
	CQuaternion addrotq2;
	double changerate;
	if ((srcapplymat != nullptr) && (srcfromiktarget == false)) {
		changerate = (double)(*(g_motionbrush_value + (int)srcframe));
		CQuaternion endq;
		endq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		srcaddrot.Slerp2(endq, 1.0 - changerate, &addrotq2);
	}
	else {
		changerate = 1.0;
		addrotq2 = srcaddrot;
	}

	//if (IsEqualRoundingTime(srcframe, srcapplyframe) == false) {
	ChaMatrix nodemat;
	ChaMatrix invnodemat;
	if (srcaplybone != nullptr) {
		nodemat = srcaplybone->GetNodeMat();
		invnodemat = ChaMatrix(nodemat);
	}
	else {
		nodemat = srcrotbone->GetNodeMat();
		invnodemat = ChaMatrix(nodemat);
	}


	//transmat2ForRot = invcurrotmat * aplyparrotmat * invnodemat * addrotq2.MakeRotMatX() * nodemat * invaplyparrotmat * currotmat;
	//transmat2ForRot = invcurrotmat * nodemat * aplyparrotmat * addrotq2.MakeRotMatX() * invaplyparrotmat * currotmat;
	
	//if (IsInitRot(currotmat) == false) {
	//	if (IsInitRot(aplyparrotmat) == false) {
	//		transmat2ForRot = invcurrotmat * aplyparrotmat * addrotq2.MakeRotMatX() * invaplyparrotmat * currotmat;
	//	}
	//	else {
	//		transmat2ForRot = invcurrotmat * addrotq2.MakeRotMatX() * currotmat;
	//	}
	//}
	//else {
	//	if (IsInitRot(aplyparrotmat) == false) {
	//		transmat2ForRot = aplyparrotmat * addrotq2.MakeRotMatX() * invaplyparrotmat;
	//	}
	//	else {
	//		transmat2ForRot = addrotq2.MakeRotMatX();
	//	}
	//}

	ChaMatrix rotmat0 = srcrotbone->GetWorldMat(limitdegflag, srcmotid, srcframe, nullptr);
	ChaMatrix invrotmat0 = ChaMatrixInv(rotmat0);

	//transmat2ForRot = addrotq2.MakeRotMatX() * currotmat;

	//transmat2ForRot = invaplyparrotmat * addrotq2.MakeRotMatX() * aplyparrotmat * currotmat;
	//transmat2ForRot = aplyparrotmat * addrotq2.MakeRotMatX() * invaplyparrotmat * currotmat;
	//transmat2ForRot = addrotq2.MakeRotMatX() * rotmat0 * invaplyparrotmat * currotmat;

	//if (srcapplymat != nullptr) {
		//transmat2ForRot = invcurrotmat * aplyparrotmat * addrotq2.MakeRotMatX() * invaplyparrotmat * currotmat;
		transmat2ForRot = rotmat0 * invaplyparrotmat * addrotq2.MakeRotMatX() * aplyparrotmat * invrotmat0;
	//}
	//else {
	//	//transmat2ForRot = invcurrotmat * nodemat * addrotq2.MakeRotMatX() * currotmat;
	//}

	dstqForRot->RotationMatrix(transmat2ForRot);

	//transmat2ForHipsRot = srcaddrot.MakeRotMatX();//for hips edit
	//dstqForHipsRot->RotationMatrix(transmat2ForHipsRot);
	*dstqForHipsRot = addrotq2;

	dstqForRot->normalize();
	dstqForHipsRot->normalize();


	return 0;

}

bool ChaCalcFunc::CalcAxisAndRotForIKRotateAxis(CModel* srcmodel, int limitdegflag,
	CBone* parentbone, CBone* firstbone,
	int srcmotid, double curframe, ChaVector3 targetpos,
	ChaVector3 srcikaxis,
	ChaVector3* dstaxis, float* dstrotrad)
{
	//return nearflag : too near to move


	//########################################################
	//2023/03/24
	//model座標系で計算：modelのWorldMatの影響を無くして計算
	//########################################################


	if (!srcmodel || !parentbone || !firstbone || !dstaxis || !dstrotrad) {
		_ASSERT(0);
		return true;
	}

	if (!firstbone->GetParent(false)) {
		_ASSERT(0);
		return true;
	}

	ChaMatrix invmodelwm = ChaMatrixInv(srcmodel->GetWorldMat());

	ChaVector3 ikaxis = srcikaxis;//!!!!!!!!!!!!
	ChaVector3Normalize(&ikaxis, &ikaxis);

	ChaVector3 modelparentpos, modelchildpos;
	{
		ChaVector3 parentworld;
		parentworld = parentbone->GetWorldPos(limitdegflag, srcmotid, curframe);
		ChaVector3TransformCoord(&modelparentpos, &parentworld, &invmodelwm);

		ChaMatrix parentmat = firstbone->GetParent(false)->GetWorldMat(limitdegflag, srcmotid, curframe, 0);// *GetWorldMat();
		ChaVector3 tmpfirstfpos = firstbone->GetJointFPos();
		ChaVector3TransformCoord(&modelchildpos, &tmpfirstfpos, &parentmat);
	}

	ChaVector3 childtotarget = targetpos - modelchildpos;
	double distance = ChaVector3LengthDbl(&childtotarget);
	//if (distance <= 0.10f) {
	//	return true;
	//}


	ChaVector3 parbef, chilbef, tarbef;
	parbef = modelparentpos;
	CalcShadowToPlane(modelchildpos, ikaxis, modelparentpos, &chilbef);
	CalcShadowToPlane(targetpos, ikaxis, modelparentpos, &tarbef);

	ChaVector3 vec0, vec1;
	vec0 = chilbef - parbef;
	ChaVector3Normalize(&vec0, &vec0);
	vec1 = tarbef - parbef;
	ChaVector3Normalize(&vec1, &vec1);

	ChaVector3 rotaxis2;
	ChaVector3Cross(&rotaxis2, (const ChaVector3*)&vec0, (const ChaVector3*)&vec1);
	ChaVector3Normalize(&rotaxis2, &rotaxis2);

	float rotdot2, rotrad2;
	rotdot2 = ChaVector3Dot(&vec0, &vec1);
	rotdot2 = fmin(1.0f, rotdot2);
	rotdot2 = fmax(-1.0f, rotdot2);
	rotrad2 = (float)acos(rotdot2);

	*dstaxis = rotaxis2;
	*dstrotrad = rotrad2;

	return false;
}


int ChaCalcFunc::RotAndTraBoneQReq(CBone* srcbone, bool limitdegflag, int wallscrapingikflag, 
	int* onlycheckptr, 
	double srcstartframe, bool infooutflag, CBone* parentbone, int srcmotid, double srcframe,
	CQuaternion qForRot, CQuaternion qForHipsRot, bool fromiktarget)
{
	//######################################
	//IK用.　RetargetはRotBoneQReq()を使用
	//hipsはtanimを qForHipsRot で回転する
	//hips以外はtanimを qForRot　で回転する
	//######################################

	//###################################################################
	//onlycheckptr != NULLの場合には
	//SetWorldMatをonlycheckで呼び出して　回転可能かどうかだけを調べる
	//初回呼び出し時のCBoneに対してだけチェックをして　直ちにリターンする
	//###################################################################

	if (!srcbone) {
		_ASSERT(0);
		return 1;
	}


	int ismovable = 1;//for return value

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (srcbone->IsNotSkeleton() && !srcbone->IsNullAndChildIsCamera()) {
		return 0;
	}

	CMotionPoint* curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		//_ASSERT(0);
		return 0;
	}

	ChaMatrix currentbefwm;
	ChaMatrix currentnewwm;
	currentbefwm.SetIdentity();
	currentnewwm.SetIdentity();
	currentbefwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);

	//初回呼び出し
	bool ishipsjoint;
	ishipsjoint = srcbone->IsHipsBone();


	ChaMatrix currentwm;
	//limitedworldmat = GetLimitedWorldMat(srcmotid, srcframe);//ここをGetLimitedWorldMatにすると１回目のIKが乱れる。２回目のIK以降はOK。
	currentwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
	ChaMatrix parentwm;
	CQuaternion parentq;
	CQuaternion invparentq;
	if (srcbone->GetParent(false)) {
		parentwm = srcbone->GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		parentq.RotationMatrix(parentwm);
		invparentq.RotationMatrix(ChaMatrixInv(parentwm));
	}
	else {
		parentwm.SetIdentity();
		parentq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		invparentq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	}

	//Get startframeframe traanim : SRT保存はCModel::IKRotate* から呼び出すCBone::SaveSRT()で行っている
	//ChaVector3 startframetraanim.SetParams(0.0f, 0.0f, 0.0f);
	ChaMatrix startframetraanimmat;
	startframetraanimmat.SetIdentity();
	{
		//CMotionPoint* zeromp = GetMotionPoint(srcmotid, 0.0);
		CMotionPoint* startframemp = srcbone->GetMotionPoint(srcmotid, RoundingTime(srcstartframe));
		if (startframemp) {
			ChaMatrix smat0, rmat0, tmat0, tanimmat0;
			smat0.SetIdentity();
			rmat0.SetIdentity();
			tmat0.SetIdentity();
			tanimmat0.SetIdentity();
			//CModel::IKRotate* から呼び出したCBone::SaveSRT()で保存したSRTを取得
			startframemp->GetSaveSRTandTraAnim(&smat0, &rmat0, &tmat0, &tanimmat0);
			startframetraanimmat = tanimmat0;
		}
		else {
			startframetraanimmat.SetIdentity();
		}
	}
	//ChaMatrix currenttraanimmat;
	//curmp->GetSaveSRTandTraAnim(0, 0, 0, &currenttraanimmat);



	ChaMatrix newwm;
	newwm.SetIdentity();


	//###########################################################################################################
	//2022/12/29 Memo
	//Hipsのときには　追加分の回転を後ろから掛ける
	//その際にTraAnimよりも後ろから掛けることにより TraAnimを回転する
	// 
	//Hips以外の時には　qForRotの内容にトリックがあって　追加分の回転を "実質的には"前から掛けている
	//この場合　TraAnimだけを別途回転してセットする必要がある
	// 
	//なぜ　Hips以外の時にHipsと同じシンプルな式を使えないかというと
	//体全体を回転した時などに　体に対する回転の向きを維持する必要があるので　後ろから掛けることが出来ないため
	//###########################################################################################################


	//####################################################################################################
	//2024/04/26 AdditiveIK 1.0.0.18へ向けて
	//Hipsの場合にも　ジャンプ中に前転させる場合においては　Hips以外と同様(移動はそのままで回転だけする)に処理する必要があった
	//Hipsかつ　g_rotateanimがtrueのときのみ変更前と同様に　追加分の回転を後ろから掛ける
	//その際にTraAnimよりも後ろから掛けることにより TraAnimを回転する
	//####################################################################################################


	if ((ishipsjoint == true) && g_rotatetanim) {//2024/04/26 && g_rotatetanim

		//############
		//Hips Joint
		//############


		//#############################################################################################################################
		//2022/12/27
		//hispについて　移動も回転するには　について
		//InvCurNodeTra * curS * curR * CurNodeTra * TAnim * ParentWM に対して　回転qForHipsRotを加え　curTAnimも回転するには
		//イメージとしては　curwmの親の位置に　qForHipsRot処理を加えるイメージ
		//実際には
		//curTAnimとCurNodeTraは両方とも移動成分のみであるから可換であるから
		//(InvCurNodeTra * curS * curR * TAnim * CurNodeTra) * InvCurNodeTra * qForHipsRot * CurNodeTra * ParentWM
		//currentwm * InvCurNode * qForHipsRot * CurNodeTra * ParentWM
		//#############################################################################################################################
		//newwm = currentwm * ChaMatrixInv(parentwm) *
		//	ChaMatrixInv(ChaMatrixTra(GetNodeMat())) * qForHipsRot.MakeRotMatX() * ChaMatrixTra(GetNodeMat()) *
		//	parentwm;

		newwm = currentwm * ChaMatrixInv(parentwm) *
			ChaMatrixInv(ChaMatrixTra(srcbone->GetNodeMat())) * ChaMatrixInv(startframetraanimmat) * qForHipsRot.MakeRotMatX() * ChaMatrixTra(srcbone->GetNodeMat()) * startframetraanimmat *
			parentwm;

		if (onlycheckptr) {
			bool directsetflag = false;
			int onlycheckflag = 1;
			ismovable = srcbone->SetWorldMat(limitdegflag, wallscrapingikflag, directsetflag, infooutflag, 0,
				srcmotid, roundingframe, newwm, onlycheckflag, fromiktarget);
			*onlycheckptr = ismovable;
			//if (ismovable == 0) {
			//	return curmp;// not movableの場合は　印を付けて　直ちにリターンする
			//}
			return ismovable;//onlycheckptr != NULLの場合は　初回呼び出しでmovableチェックして直ちにリターン
		}
		else {
			bool directsetflag = false;
			int onlycheckflag = 0;
			int setchildflag = 1;
			ismovable = srcbone->SetWorldMat(limitdegflag, wallscrapingikflag, directsetflag, infooutflag, setchildflag,
				srcmotid, roundingframe, newwm, onlycheckflag, fromiktarget);
		}
		currentnewwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);

	}
	else {

		//###############################################
		//other joints !!!! traanimを qForRot で回転する
		//###############################################


			////以下３行　hipsと同じようにすると　traanimが設定してあるジョイントで　回転軸がマニピュレータと合わない
			//newwm = currentwm * ChaMatrixInv(parentwm) *
			//	ChaMatrixInv(ChaMatrixTra(GetNodeMat())) * ChaMatrixInv(startframetraanimmat) * qForRot.MakeRotMatX() * ChaMatrixTra(GetNodeMat()) * startframetraanimmat *
			//	parentwm;



		//########################################################################
		//2023/01/14
		//指のRigでテストしたところ 制限角度有りの場合に　traanimが不正になった
		//２段階に分けて計算することにより解決
		//########################################################################

		//#############################################################
		//２段階処理の１段目：回転だけを変更して確定するための　１段目
		//#############################################################
			////calc new local rot
		ChaMatrix newlocalrotmatForRot;
		ChaMatrix smatForRot, rmatForRot, tmatForRot, tanimmatForRot;
		newlocalrotmatForRot.SetIdentity();
		smatForRot.SetIdentity();
		rmatForRot.SetIdentity();
		tmatForRot.SetIdentity();
		tanimmatForRot.SetIdentity();
		newlocalrotmatForRot = srcbone->CalcNewLocalRotMatFromQofIK(limitdegflag, srcmotid, roundingframe, qForRot, &smatForRot, &rmatForRot, &tanimmatForRot);

		ChaMatrix newtanimmatrotated;
		newtanimmatrotated = tanimmatForRot;//１段目では　traanimを 回転しない

		////	//traanimを 回転しないとき
		////	newlocalrotmatForHipsRot = newlocalrotmatForRot;
		////	newtanimmatrotated = tanimmatForRot;


		//#### SRTAnimからローカル行列組み立て ####
		ChaMatrix newlocalmat;
		newlocalmat = ChaMatrixFromSRTraAnim(true, true, srcbone->GetNodeMat(),
			&smatForRot, &newlocalrotmatForRot, &newtanimmatrotated);//ForRot
		//newwm = newlocalmat * parentwmForRot;//globalにする
		if (srcbone->GetParent(false)) {
			newwm = newlocalmat * parentwm;//globalにする
		}
		else {
			newwm = newlocalmat;
		}

		if (onlycheckptr) {
			bool directsetflag = false;
			int onlycheckflag = 1;
			ismovable = srcbone->SetWorldMat(limitdegflag, wallscrapingikflag, directsetflag, infooutflag, 0,
				srcmotid, roundingframe, newwm, onlycheckflag, fromiktarget);
			*onlycheckptr = ismovable;
			//if (ismovable == 0) {
			//	return curmp;// not movableの場合は　印を付けて　直ちにリターンする
			//}


			//onlycheckの場合は　ここまで
			return ismovable;

		}
		else {
			bool directsetflag = false;
			int onlycheckflag = 0;
			int setchildflag = 1;//2023/02/12 ２段目の前に再帰する必要
			ismovable = srcbone->SetWorldMat(limitdegflag, wallscrapingikflag, directsetflag, infooutflag, setchildflag,
				srcmotid, roundingframe, newwm, onlycheckflag, fromiktarget);
		}
		currentnewwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);

		////#####################################################################################################
		////２段階処理の２段目：角度制限オンオフを考慮し　回転を確定させた後　移動アニメを回転するための　２段目
		////#####################################################################################################


			//2023/01/22 制限角度と一緒に使うと　操作ごとに誤差が蓄積するので　オプションにした
		if ((ismovable == 1) && g_rotatetanim) {
			////calc new local rot
			ChaMatrix tmplocalmat;
			tmplocalmat = currentnewwm * ChaMatrixInv(parentwm);

			//ChaMatrix newlocalrotmatForRot2;
			ChaMatrix smatForRot2, rmatForRot2, tmatForRot2, tanimmatForRot2;
			//newlocalrotmatForRot2.SetIdentity();
			smatForRot2.SetIdentity();
			rmatForRot2.SetIdentity();
			tmatForRot2.SetIdentity();
			tanimmatForRot2.SetIdentity();

			//newlocalrotmatForRot = CalcNewLocalRotMatFromQofIK(srcmotid, roundingframe, qForRot, &smatForRot, &rmatForRot, &tanimmatForRot);
			GetSRTandTraAnim(tmplocalmat, srcbone->GetNodeMat(), &smatForRot2, &rmatForRot2, &tmatForRot2, &tanimmatForRot2);


			//２段目では　確定した回転によりtraanimを回転する
			ChaMatrix newtanimmatrotated2;
			newtanimmatrotated2 = srcbone->CalcNewLocalTAnimMatFromSRTraAnim(rmatForRot2,
				smatForRot, rmatForRot, tanimmatForRot, ChaMatrixTraVec(startframetraanimmat));

			////	//traanimを 回転しないとき
			////	newlocalrotmatForHipsRot = newlocalrotmatForRot;
			////	newtanimmatrotated = tanimmatForRot;


			//#### SRTAnimからローカル行列組み立て ####
			ChaMatrix newlocalmat2;
			newlocalmat2 = ChaMatrixFromSRTraAnim(true, true, srcbone->GetNodeMat(),
				&smatForRot, &rmatForRot2, &newtanimmatrotated2);//ForRot
			//newwm = newlocalmat * parentwmForRot;//globalにする
			if (srcbone->GetParent(false)) {
				newwm = newlocalmat2 * parentwm;//globalにする
			}
			else {
				newwm = newlocalmat2;
			}

			bool directsetflag2 = true;
			int setchildflag2 = 1;//2023/02/12
			int onlycheck2 = 0;
			srcbone->SetWorldMat(limitdegflag, wallscrapingikflag, directsetflag2, infooutflag, setchildflag2,
				srcmotid, roundingframe, newwm, onlycheck2, fromiktarget);
			currentnewwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		}

	}

	curmp->SetAbsMat(srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp));

	//2023/02/12 setchildflag = 1で処理することに
	//if (ismovable == 1) {
	//	if (m_child && curmp) {
	//		bool setbroflag2 = true;
	//		m_child->UpdateParentWMReq(limitdegflag, setbroflag2, srcmotid, roundingframe,
	//			currentbefwm, currentnewwm);
	//	}
	//	if (m_brother && parentbone) {
	//		bool setbroflag3 = true;
	//		m_child->UpdateParentWMReq(limitdegflag, setbroflag3, srcmotid, roundingframe,
	//			srcbefparentwm, srcnewparentwm);
	//	}
	//}

	return ismovable;
}

int ChaCalcFunc::IKTargetVec(CModel* srcmodel, bool limitdegflag, int wallscrapingikflag, 
	CEditRange* erptr, int srcmotid, double srcframe, bool postflag)
{
	if (!srcmodel || !erptr) {
		_ASSERT(0);
		return 1;
	}

	int iktargetbonevecsize = (int)srcmodel->GetIKTargetBoneVecSize();

	int targetno;
	for (targetno = 0; targetno < iktargetbonevecsize; targetno++) {
		CBone* srcbone = srcmodel->GetIKTargetBone(targetno);
		if (srcbone && srcbone->IsSkeleton() && srcbone->GetParent(false) && srcbone->GetIKTargetFlag()) {
			ChaVector3 iktargetpos = srcbone->GetIKTargetPos();//model座標系
			int calccount;
			const int calccountmax = 30;
			for (calccount = 0; calccount < calccountmax; calccount++) {
				int maxlevel = 0;
				IKRotateForIKTarget(srcmodel, limitdegflag, wallscrapingikflag, erptr, srcbone->GetBoneNo(), srcmotid,
					iktargetpos, maxlevel, srcframe, postflag);
			}
		}
	}

	return 0;
}

int ChaCalcFunc::IKRotateForIKTarget(CModel* srcmodel, bool limitdegflag, int wallscrapingikflag, 
	CEditRange* erptr,
	int srcboneno, int srcmotid, ChaVector3 targetpos, int maxlevel, double directframe, bool postflag)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}


	double curframe = directframe;

	CBone* firstbone = srcmodel->GetBoneByID(srcboneno);
	if (!firstbone) {
		_ASSERT(0);
		//g_underIKRot = false;//2023/01/14 parent limited or not
		return -1;
	}
	if (firstbone->IsNotSkeleton()) {
		return -1;
	}


	bool ishipsjoint = false;
	if (firstbone->GetParent(false)) {
		ishipsjoint = firstbone->GetParent(false)->IsHipsBone();
	}
	else {
		ishipsjoint = false;
	}

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);


	//if (postflag && (directframe == applyframe)) {
	//	return srcboneno;
	//}


	CBone* curbone = firstbone;
	CBone* lastpar = curbone;
	srcmodel->SetBefEditMat(limitdegflag, erptr, curbone, maxlevel);
	CBone* editboneforret = 0;
	if (firstbone->GetParent(false)) {
		editboneforret = firstbone->GetParent(false);
	}
	else {
		editboneforret = firstbone;
	}


	//For IKTraget
	//カメラ軸回転とカメラ軸に垂直な軸回転と　２回実行する
	int calcnum = 3;

	int calccnt;
	for (calccnt = 1; calccnt <= calcnum; calccnt++) {
		curbone = firstbone;
		lastpar = curbone;

		int levelcnt = 0;
		float currate;
		//currate = g_ikrate;
		currate = 0.750f;

		while (curbone && lastpar && lastpar->GetParent(false) && ((maxlevel == 0) || (levelcnt < maxlevel)))
		{

			//IKTarget()でフラグがリセットされるので　ループ先頭で　セットし直し
			//g_underIKRot = true;

			//CBone* parentbone = curbone->GetParent();
			CBone* parentbone = lastpar->GetParent(false);
			if (parentbone && parentbone->IsSkeleton() && (curbone->GetJointFPos() != parentbone->GetJointFPos())) {
				//UpdateMatrix(limitdegflag, &m_matWorld, &m_matVP);//curmp更新

				CRigidElem* curre = srcmodel->GetRigidElem(lastpar->GetBoneNo());
				if (curre && (curre->GetForbidRotFlag() != 0)) {

					//_ASSERT(0);

					//回転禁止の場合処理をスキップ
					if (parentbone) {
						lastpar = parentbone;
					}
					levelcnt++;
					currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
					continue;
				}

				ChaVector3 rotaxis2;
				float rotrad2;
				//if ((calccnt % 2) != 0) {
				//	CalcAxisAndRotForIKRotate(limitdegflag,
				//		parentbone, firstbone,
				//		curframe, targetpos,
				//		&rotaxis2, &rotrad2);
				//}
				//else {
				//	CalcAxisAndRotForIKRotateVert(limitdegflag,
				//		parentbone, firstbone,
				//		curframe, targetpos,
				//		&rotaxis2, &rotrad2);
				//}


				ChaMatrix parentnodemat;
				parentnodemat = parentbone->GetNodeMat();
				ChaMatrix parentnoderot;
				parentnoderot = ChaMatrixRot(parentnodemat);
				ChaVector3 ikaxis;
				if ((calccnt % 3) == 0) {
					ikaxis = parentnoderot.GetRow(0);
				}
				else if ((calccnt % 2) == 0) {
					ikaxis = parentnoderot.GetRow(1);
				}
				else {
					ikaxis = parentnoderot.GetRow(2);
				}
				bool nearflag = CalcAxisAndRotForIKRotateAxis(srcmodel, limitdegflag,
					parentbone, firstbone,
					srcmotid, curframe, targetpos,
					ikaxis,
					&rotaxis2, &rotrad2);

				//rotrad2 *= currate;

				double firstframe = 0.0;
				if ((nearflag == false) && (fabs(rotrad2) > 1.0e-4)) {
					CQuaternion rotq0;
					rotq0.SetAxisAndRot(rotaxis2, rotrad2);

					//parentbone->SaveSRT(limitdegflag, m_curmotinfo->motid, startframe, endframe);
					// 
					//保存結果は　CBone::RotAndTraBoneQReqにおいてしか使っておらず　startframeしか使っていない
					parentbone->SaveSRT(limitdegflag, srcmotid, startframe);
					ChaMatrix saveapplyframemat;
					if (parentbone->GetParent(false)) {
						saveapplyframemat = parentbone->GetParent(false)->GetWorldMat(limitdegflag, srcmotid, RoundingTime(applyframe), 0);//2025/02/24
					}
					else {
						saveapplyframemat = parentbone->GetWorldMat(limitdegflag, srcmotid, RoundingTime(applyframe), 0);//2025/02/24
					}
					//saveapplyframemat = parentbone->GetWorldMat(limitdegflag, srcmotid, RoundingTime(applyframe), 0);//2025/02/24

					//IKRotateは壁すりIKで行うので　回転可能かどうかのチェックはここではしない


					int keyno = 0;
					bool keynum1flag = false;
					bool fromiktarget = true;
					IKRotateOneFrame(srcmodel, limitdegflag, wallscrapingikflag, 
						erptr,
						keyno,
						parentbone, 
						//parentbone, 
						parentbone->GetParent(false),
						srcmotid, curframe, startframe, applyframe,
						rotq0, keynum1flag, postflag, fromiktarget, &saveapplyframemat);
					keyno++;

					//if (g_applyendflag == 1) {
					//	//curmotinfo->curframeから最後までcurmotinfo->curframeの姿勢を適用
					//	int tolast;
					//	for (tolast = (int)m_curmotinfo->curframe + 1; tolast < m_curmotinfo->frameleng; tolast++) {
					//		(m_bonelist[0])->PasteRotReq(limitdegflag, m_curmotinfo->motid, m_curmotinfo->curframe, tolast);
					//	}
					//}
				}

			}

			if (parentbone) {
				lastpar = parentbone;

				//コンストレイント用回転も　IKStopで止める必要有
				//体の中心まで回転を伝えた方が　コンストレイントしやすいが
				//shoulderのIKStopで回転を止めない場合
				//右手と左手のコンストレイント順番により　どちらかにしか拘束できなくなる
				//check ikstopflag
				if (parentbone->GetIKStopFlag()) {
					break;
				}
			}


			levelcnt++;

			//currate = (float)pow((double)g_ikrate, (double)g_ikfirst * (double)levelcnt);
		}

		//絶対モードの場合
		if ((calccnt == calcnum) && g_absikflag && lastpar) {
			AdjustBoneTra(srcmodel, limitdegflag, wallscrapingikflag, erptr, lastpar, srcmotid);
		}
	}

	//if( lastpar ){
	//	return lastpar->GetBoneNo();
	//}else{
	//	return srcboneno;
	//}


	//g_underIKRot = false;//2023/01/14 parent limited or not
	if (editboneforret)
	{
		return editboneforret->GetBoneNo();
	}
	else {
		return srcboneno;
	}

}


int ChaCalcFunc::AdjustBoneTra(CModel* srcmodel, bool limitdegflag, int wallscrapingikflag, CEditRange* erptr, CBone* lastpar, int srcmotid)
{
	int keynum = erptr->GetKeyNum();
	double startframe = erptr->GetStartFrame();
	double endframe;
	//if (g_applyendflag == 1) {
	//	endframe = m_curmotinfo->frameleng - 1.0;
	//}
	//else {
	endframe = erptr->GetEndFrame();
	//}

	if (lastpar && (keynum >= 2)) {
		int keyno = 0;
		double curframe;
		for (curframe = RoundingTime(startframe); curframe <= endframe; curframe += 1.0) {
			if (keyno >= 1) {
				CMotionPoint* pcurmp = 0;
				int curmotid = srcmotid;
				pcurmp = lastpar->GetMotionPoint(curmotid, curframe);
				if (pcurmp) {
					ChaVector3 orgpos;
					ChaVector3 tmplastfpos = lastpar->GetJointFPos();
					ChaMatrix tmpbefeditmat = pcurmp->GetBefEditMat();
					ChaVector3TransformCoord(&orgpos, &tmplastfpos, &tmpbefeditmat);

					ChaVector3 newpos;
					ChaMatrix tmpwm = lastpar->GetWorldMat(limitdegflag, curmotid, curframe, pcurmp);
					ChaVector3TransformCoord(&newpos, &tmplastfpos, &tmpwm);

					ChaVector3 diffpos;
					diffpos = orgpos - newpos;

					CEditRange tmper;
					KeyInfo tmpki;
					tmpki.time = curframe;
					list<KeyInfo> tmplist;
					tmplist.push_back(tmpki);
					tmper.SetRange(tmplist, curframe);
					//FKBoneTra( 0, &tmper, lastpar->GetBoneNo(), diffpos );
					FKBoneTra(srcmodel, limitdegflag, wallscrapingikflag, 
						1, &tmper, lastpar->GetBoneNo(), srcmotid, diffpos);//2022/11/07 FKBoneTra内でframeno loopしないように　onlyoneflag = 1
				}
			}
			keyno++;
		}
	}

	return 0;
}


int ChaCalcFunc::FKBoneTra(CModel* srcmodel, bool limitdegflag, int wallscrapingikflag, 
	int onlyoneflag, CEditRange* erptr,
	int srcboneno, int srcmotid, ChaVector3 addtra, double onlyoneframe)
{

	if (!srcmodel || (srcboneno < 0)) {
		_ASSERT(0);
		return 1;
	}

	CBone* firstbone = srcmodel->GetBoneByID(srcboneno);
	if (!firstbone) {
		_ASSERT(0);
		return 1;
	}
	if (firstbone->IsNotSkeleton()) {
		return 1;
	}

	CBone* curbone = firstbone;
	srcmodel->SetBefEditMatFK(limitdegflag, erptr, curbone);

	CBone* lastpar = firstbone->GetParent(false);

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);
	if (onlyoneflag == 0) {
	}
	else {
		startframe = onlyoneframe;
		endframe = onlyoneframe;
	}


	curbone = firstbone;
	double firstframe = 0.0;

	ChaMatrix dummyparentwm;
	dummyparentwm.SetIdentity();//Req関数の最初の呼び出し時は　Identityを渡せばよい

	if (keynum >= 2) {
		//float changerate = 1.0f / (float)(endframe - startframe + 1);

		int keyno = 0;
		double curframe;
		for (curframe = startframe; curframe <= endframe; curframe += 1.0) {
			double changerate;
			//if( curframe <= applyframe ){
			//	changerate = 1.0 / (applyframe - startframe + 1);
			//}else{
			//	changerate = 1.0 / (endframe - applyframe + 1);
			//}
			changerate = (double)(*(g_motionbrush_value + (int)curframe));


			if (keyno == 0) {
				firstframe = curframe;
			}
			if (g_absikflag == 0) {
				if (g_slerpoffflag == 0) {
					//double currate2;
					//if( curframe <= applyframe ){
					//	currate2 = changerate * (curframe - startframe + 1);
					//}else{
					//	currate2 = changerate * (endframe - curframe + 1);
					//}
					//ChaVector3 curtra;
					//curtra = addtra * (float)currate2;
					ChaVector3 curtra;
					curtra = addtra * (float)changerate;

					//currate2 = changerate * keyno;
					//ChaVector3 curtra;
					//curtra = (1.0 - currate2) * addtra;

					curbone->AddBoneTraReq(limitdegflag, wallscrapingikflag, 0, srcmotid, curframe, curtra, dummyparentwm, dummyparentwm);
				}
				else {
					curbone->AddBoneTraReq(limitdegflag, wallscrapingikflag, 0, srcmotid, curframe, addtra, dummyparentwm, dummyparentwm);
				}
			}
			else {
				if (keyno == 0) {
					curbone->AddBoneTraReq(limitdegflag, wallscrapingikflag, 0, srcmotid, curframe, addtra, dummyparentwm, dummyparentwm);
				}
				else {
					curbone->SetAbsMatReq(limitdegflag, wallscrapingikflag, 0, srcmotid, curframe, firstframe);
				}
			}

			bool postflag = false;
			IKTargetVec(srcmodel, limitdegflag, wallscrapingikflag, erptr, srcmotid, curframe, postflag);


			keyno++;

		}
	}
	else {
		curbone->AddBoneTraReq(limitdegflag, wallscrapingikflag, 0, srcmotid, startframe, addtra, dummyparentwm, dummyparentwm);

		bool postflag = false;
		IKTargetVec(srcmodel, limitdegflag, wallscrapingikflag, erptr, srcmotid, startframe, postflag);
	}


	return curbone->GetBoneNo();
}


int ChaCalcFunc::FKBoneTraOneFrame(CModel* srcmodel, bool limitdegflag, int wallscrapingikflag, 
	CEditRange* erptr, int srcboneno, int srcmotid, double srcframe, ChaVector3 addtra)
{

	if (!srcmodel || (srcboneno < 0)) {
		_ASSERT(0);
		return 1;
	}

	CBone* firstbone = srcmodel->GetBoneByID(srcboneno);
	if (!firstbone) {
		_ASSERT(0);
		return 1;
	}
	if (firstbone->IsNotSkeleton()) {
		return 1;
	}

	CBone* curbone = firstbone;
	srcmodel->SetBefEditMatFK(limitdegflag, erptr, curbone);

	CBone* lastpar = firstbone->GetParent(false);

	int keynum;
	double startframe, endframe, applyframe;
	erptr->GetRange(&keynum, &startframe, &endframe, &applyframe);


	//########### applyframeは　Post処理しない ###########
	if (RoundingTime(srcframe) == RoundingTime(applyframe)) {
		return 0;
	}


	curbone = firstbone;
	double firstframe = 0.0;

	ChaMatrix dummyparentwm;
	dummyparentwm.SetIdentity();//Req関数の最初の呼び出し時は　Identityを渡せばよい

	if (keynum >= 2) {
		//float changerate = 1.0f / (float)(endframe - startframe + 1);

		int keyno = 0;
		double curframe = srcframe;
		//for (curframe = startframe; curframe <= endframe; curframe += 1.0) {
			double changerate;
			//if( curframe <= applyframe ){
			//	changerate = 1.0 / (applyframe - startframe + 1);
			//}else{
			//	changerate = 1.0 / (endframe - applyframe + 1);
			//}
			changerate = (double)(*(g_motionbrush_value + (int)curframe));


			//if (keyno == 0) {
			//	firstframe = curframe;
			//}
			if (g_absikflag == 0) {
				if (g_slerpoffflag == 0) {
					//double currate2;
					//if( curframe <= applyframe ){
					//	currate2 = changerate * (curframe - startframe + 1);
					//}else{
					//	currate2 = changerate * (endframe - curframe + 1);
					//}
					//ChaVector3 curtra;
					//curtra = addtra * (float)currate2;
					ChaVector3 curtra;
					curtra = addtra * (float)changerate;

					//currate2 = changerate * keyno;
					//ChaVector3 curtra;
					//curtra = (1.0 - currate2) * addtra;

					curbone->AddBoneTraReq(limitdegflag, wallscrapingikflag, 0, srcmotid, curframe, curtra, dummyparentwm, dummyparentwm);
				}
				else {
					curbone->AddBoneTraReq(limitdegflag, wallscrapingikflag, 0, srcmotid, curframe, addtra, dummyparentwm, dummyparentwm);
				}
			}
			else {
				if (keyno == 0) {
					curbone->AddBoneTraReq(limitdegflag, wallscrapingikflag, 0, srcmotid, curframe, addtra, dummyparentwm, dummyparentwm);
				}
				else {
					curbone->SetAbsMatReq(limitdegflag, wallscrapingikflag, 0, srcmotid, curframe, firstframe);
				}
			}

			bool postflag = false;
			IKTargetVec(srcmodel, limitdegflag, wallscrapingikflag, erptr, srcmotid, curframe, postflag);


			keyno++;

		//}
	}
	//else {
	//	curbone->AddBoneTraReq(limitdegflag, 0, srcmotid, startframe, addtra, dummyparentwm, dummyparentwm);
	//	bool postflag = false;
	//	IKTargetVec(srcmodel, limitdegflag, erptr, srcmotid, startframe, postflag);
	//}


	return curbone->GetBoneNo();
}

CMotionPoint* ChaCalcFunc::AddBoneTraReq(CBone* srcbone, bool limitdegflag, int wallscrapingikflag, 
	CMotionPoint* parmp, int srcmotid, double srcframe, ChaVector3 srctra, ChaMatrix befparentwm, ChaMatrix newparentwm)
{
	if (!srcbone) {
		_ASSERT(0);
		return 0;
	}

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (srcbone->IsNotSkeleton()) {
		//_ASSERT(0);
		return 0;
	}

	int existflag = 0;
	//CMotionPoint* curmp = AddMotionPoint( srcmotid, srcframe, &existflag );
	//if( !curmp || !existflag ){
	CMotionPoint* curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		_ASSERT(0);
		return 0;
	}

	ChaMatrix currentbefwm;
	ChaMatrix currentnewwm;
	currentbefwm.SetIdentity();
	currentnewwm.SetIdentity();
	currentbefwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);


	bool infooutflag = false;

	//curmp->SetBefWorldMat( curmp->GetWorldMat() );
	if (parmp) {
		//ChaMatrix invbefpar;
		//ChaMatrix tmpparbefwm = parmp->GetBefWorldMat();//!!!!!!! 2022/12/23 引数にするべき
		//ChaMatrixInverse( &invbefpar, NULL, &tmpparbefwm );
		//ChaMatrix tmpmat = curmp->GetWorldMat() * invbefpar * parmp->GetWorldMat();
		ChaMatrix tmpmat = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp) * ChaMatrixInv(befparentwm) * newparentwm;
		bool directsetflag = true;
		int onlycheck = 0;
		bool fromiktarget = false;
		srcbone->SetWorldMat(limitdegflag, wallscrapingikflag, directsetflag, infooutflag, 0, srcmotid, roundingframe, tmpmat, onlycheck, fromiktarget);

		currentnewwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
	}
	else {
		ChaMatrix tramat;
		tramat.SetIdentity();//2023/02/12
		ChaMatrixTranslation(&tramat, srctra.x, srctra.y, srctra.z);
		ChaMatrix tmpmat = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp) * tramat;
		bool directsetflag = true;
		int onlycheck = 0;
		bool fromiktarget = false;
		srcbone->SetWorldMat(limitdegflag, wallscrapingikflag, directsetflag, infooutflag, 0, srcmotid, roundingframe, tmpmat, onlycheck, fromiktarget);

		currentnewwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
	}

	curmp->SetAbsMat(srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp));

	if (srcbone->GetChild(false)) {
		srcbone->GetChild(false)->AddBoneTraReq(limitdegflag, wallscrapingikflag, 
			curmp, srcmotid, roundingframe, srctra, currentbefwm, currentnewwm);
	}
	if (srcbone->GetBrother(false) && parmp) {
		srcbone->GetBrother(false)->AddBoneTraReq(limitdegflag, wallscrapingikflag, 
			parmp, srcmotid, roundingframe, srctra, befparentwm, newparentwm);
	}
	return curmp;

}



int ChaCalcFunc::CalcBoneEul(CModel* srcmodel, bool limitdegflag, int srcmotid)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}

	if (srcmodel->GetNoBoneFlag() == true) {
		return 0;
	}

	if (srcmotid >= 0) {
		map<int, CBone*>::iterator itrbone;
		for (itrbone = srcmodel->GetBoneListBegin(); itrbone != srcmodel->GetBoneListEnd(); itrbone++) {
			CBone* curbone = itrbone->second;
			if (curbone) {
				MOTINFO curmi = srcmodel->GetMotInfo(srcmotid);
				if (curmi.motid > 0) {
					CalcBoneEulOne(srcmodel, limitdegflag, curbone, curmi.motid, 0.0, curmi.frameleng - 1.0);
				}
			}
		}
	}

	return 0;
}
int ChaCalcFunc::CalcBoneEulOne(CModel* srcmodel, bool limitdegflag, CBone* curbone, int srcmotid, double startframe, double endframe)
{
	if (!srcmodel || !curbone) {
		_ASSERT(0);
		return 1;
	}

	if (curbone->IsSkeleton() || curbone->IsNullAndChildIsCamera()) {
		ChaVector3 cureul;
		cureul.SetParams(0.0f, 0.0f, 0.0f);
		int paraxiskind = -1;
		double srcframe;
		for (srcframe = RoundingTime(startframe); srcframe <= RoundingTime(endframe); srcframe += 1.0) {
			cureul = curbone->CalcLocalEulXYZ(limitdegflag, paraxiskind, srcmotid, srcframe, BEFEUL_BEFFRAME);
			////curbone->SetLocalEul(limitdegflag, srcmotid, srcframe, cureul, 0);

			////ChaVector3 srceul = curbone->GetLocalEul(limitdegflag, srcmotid, srcframe, 0);

			//2023/11/07 
			// GetLocalEulではなくCalcLocalEulXYZを呼び出すことにしたので　ModifyEulerについてはCalcLocalEulXYZで呼び出されている
			// よってModifyEuler360をコメントアウト
			//BEFEUL befeul = curbone->GetBefEul(limitdegflag, srcmotid, srcframe);
			//int notmodify180flag = curbone->GetNotModify180Flag(srcmotid, srcframe);
			////ModifyEuler360(&cureul, &(befeul.befframeeul), notmodify180flag, 15.0f, 15.0f, 15.0f);//1.2.0.27, 1.2.0.28, 1.2.0.29
			//ModifyEuler360(&cureul, &(befeul.befframeeul), notmodify180flag, 1.0f, 1.0f, 1.0f);


			//ModifyEuler360(&srceul, &(befeul.befframeeul), notmodify180flag, 91.0f, 181.0f, 181.0f);
			//ModifyEuler360(&srceul, &(befeul.befframeeul), notmodify180flag, 91.0f, 91.0f, 91.0f);
			//ModifyEuler360(&srceul, &(befeul.befframeeul), notmodify180flag, 45.0f, 45.0f, 45.0f);
			//ModifyEuler360(&srceul, &(befeul.befframeeul), notmodify180flag, 1.0f, 1.0f, 1.0f);
			//ModifyEuler360(&srceul, &(befeul.befframeeul), notmodify180flag, 180.0f, 180.0f, 180.0f);
			curbone->SetLocalEul(limitdegflag, srcmotid, srcframe, cureul, 0);
		}
	}
	return 0;
}

ChaVector3 ChaCalcFunc::CalcLocalEulXYZ(CBone* srcbone, bool limitdegflag, int axiskind,
	int srcmotid, double srcframe, tag_befeulkind befeulkind, ChaVector3* directbefeul)
{
	if (!srcbone) {
		_ASSERT(0);
		return ChaVector3(0.0f, 0.0f, 0.0f);
	}
	if (!srcbone->GetParModel()) {
		_ASSERT(0);
		return ChaVector3(0.0f, 0.0f, 0.0f);
	}

	//###################################################################################################################
	//2022/12/17
	//この関数の呼び出し元でLimitEul()をする
	//Parentの姿勢に関しては計算済のGetParent()->GetWorldMat()を使用 : curwmに掛かっているのはLimitedではないparentwm
	//モーション全体のオイラー角計算し直しは　この関数ではなく　UpdateMatrixを使用
	//###################################################################################################################

	double roundingframe = RoundingTime(srcframe);


	//for debug
	//if ((roundingframe == 2.0) && (g_limitdegflag == true) && (strstr(GetBoneName(), "Root") != 0)) {
	//	_ASSERT(0);
	//}


	//axiskind : BONEAXIS_*  or  -1(CBone::m_anglelimit.boneaxiskind)

	ChaVector3 cureul;
	cureul.SetParams(0.0f, 0.0f, 0.0f);
	BEFEUL befeul;
	befeul.Init();


	if (srcbone->IsNotSkeleton() && srcbone->IsNotCamera() && srcbone->IsNotNull()) {
		return cureul;
	}
	if (!srcbone->GetParModel()) {
		_ASSERT(0);
		return cureul;
	}


	const WCHAR* bonename = srcbone->GetWBoneName();
	//if (wcscmp(bonename, L"RootNode") == 0){
	//	return cureul;//!!!!!!!!!!!!!!!!!!!!!!!!
	//}

	if (befeulkind == BEFEUL_BEFFRAME) {
		//1つ前のフレームのEULはすでに計算されていると仮定する。
		//bool limitdegOnLimitEul = false;//2023/02/07 befeulはunlimited. 何回転もする場合にオーバー１８０度の角度で制限するために.
		//befeul = srcbone->GetBefEul(limitdegOnLimitEul, srcmotid, roundingframe);

		//2023/10/18
		//limitdeg trueのときのために　limitdegflagはそのまま渡す必要
		//１８０度オーバー対策としては　PostIKの後に　CalcBoneEulを呼ぶ
		//befeul = srcbone->GetBefEul(limitdegflag, srcmotid, roundingframe);

		//2023/10/25
		//3回転して１回転の制限角度にする場合などに　制限中のはずの角度が小さい角度で評価されないように　&& 
		//limitdeg trueのときにも　180度オーバーの角度で制限を掛ける
		////if (g_underCalcEul) {
		if (srcbone && srcbone->GetParModel() && srcbone->GetParModel()->GetUnderCalcEul()) {
			befeul = srcbone->GetBefEul(limitdegflag, srcmotid, roundingframe);
		}
		else {
			bool befeullimitdegflag = false;
			befeul = srcbone->GetBefEul(befeullimitdegflag, srcmotid, roundingframe);
		}


	}
	else if ((befeulkind == BEFEUL_DIRECT) && directbefeul) {
		befeul.befframeeul = *directbefeul;
		befeul.currentframeeul = *directbefeul;
	}

	int isfirstbone = 0;
	int isendbone = 0;


	CBone* parentbone = srcbone->GetParent(false);//!!!!!!!!!!


	if (parentbone && parentbone->IsSkeleton()) {
		isfirstbone = 0;
	}
	else {
		isfirstbone = 1;
	}

	if (srcbone->GetChild(false) && srcbone->GetChild(false)->IsSkeleton()) {
		if (srcbone->GetChild(false)->GetChild(false) && srcbone->GetChild(false)->GetChild(false)->IsSkeleton()) {
			isendbone = 0;
		}
		else {
			isendbone = 1;
		}
	}
	else {
		isendbone = 1;
	}



	int notmodify180flag = srcbone->GetNotModify180Flag(srcmotid, roundingframe);
	CQuaternion axisq;
	axisq.RotationMatrix(srcbone->GetNodeMat());

	CQuaternion eulq;

	if (srcbone->IsSkeleton()) {

		//###########################
		//skeletonの場合
		//###########################
		CMotionPoint* curmp = 0;
		curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
		if (curmp) {
			ChaMatrix curwm;
			curwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp);

			if (parentbone) {
				isfirstbone = 0;

				ChaMatrix parentwm, eulmat;

				//parentがeNullの場合はある
				if (parentbone->IsSkeleton()) {
					parentwm = parentbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
					eulq = ChaMatrix2Q(ChaMatrixInv(parentwm)) * ChaMatrix2Q(curwm);
				}
				else if (parentbone->IsNull() || parentbone->IsCamera()) {
					//2023/05/16 eNullにもIdentity以外のNodeMatが設定されたため修正
					//parentwm = ChaMatrixInv(parentbone->GetNodeMat()) * parentbone->GetENullMatrix();//ENullMatrixにはNodeMatが掛かっている

					//2023/06/26 書き出し時にworldmat (InvNodeMat * EvaluateGlobalTransform)にenullの　回転の　影響は入っていない? NodeMatに入っている？　？？？
					//モデルのeNullをY180度回転したモデルの読み書き読み書き読みテストで確認
					//下記のように変更しないと　eNullをY180度回転したモデルの読み書き読み時にオイラー角表現が変質し　読み書き読み書き読みテストで　モデル向きが反対を向く
					//eulq = ChaMatrix2Q(curwm);




					//2023/06/29 eNullもアニメーション可能にしたので
					//GetENullMatrixを修正してCalcEnullMatReqで計算するようにしたところが2023/06/26から変わったところ
					//SetWorldMat()時には　回転計算用のローカル行列取得時に　parenetがeNullの場合関してもGetWorldMat[invNode * CalcENullMat]を使用
					//移動計算時には　CalcFbxLocalMatrix内にて　parentがeNullの場合　GetENullMatrixを使用
					//
					//nullの　回転の　影響は入っていない? NodeMatに入っている？に関して
					//eNullがアニメーションしない場合には　eNullの初期行列はNodeMatに含まれる　eNullのアニメーション分はInvNode * ENullMat = Indentityとなる
					//eNullがアニメーションする場合には　eNullのアニメーション分はInvNode * ENullMat != Indentityとなり後ろに掛ける　
					//補足：NodeMatはジョイントの位置である　NodeMatを途中で変えることはジョイント位置を途中で変えることであり　通常NodeMatは変えない
					parentwm = parentbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
					eulq = ChaMatrix2Q(ChaMatrixInv(parentwm)) * ChaMatrix2Q(curwm);
				}
				else {
					eulq = ChaMatrix2Q(curwm);
				}
			}
			else {
				isfirstbone = 1;
				eulq = ChaMatrix2Q(curwm);
			}
		}
		else {
			//_ASSERT(0);
			eulq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		}

		eulq.Q2EulXYZusingQ(srcbone->GetParModel()->GetUnderIKRot(), srcbone->GetParModel()->GetUnderRetarget(),
			&axisq, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);

	}
	else if (srcbone->IsCamera()) {

		//########################
		//カメラの場合
		//########################
		if (srcbone->GetFbxNodeOnLoad()) {
			EnterCriticalSection(&g_CritSection_FbxSdk);
			FbxTime fbxtime0;
			fbxtime0.SetSecondDouble(0.0);
			FbxDouble3 fbxLclRot = srcbone->GetFbxNodeOnLoad()->EvaluateLocalRotation(fbxtime0, FbxNode::eSourcePivot, true, true);
			cureul.SetParams((float)fbxLclRot[0], (float)fbxLclRot[1], (float)fbxLclRot[2]);
			//####  rotorder注意  #####
			LeaveCriticalSection(&g_CritSection_FbxSdk);
		}

		//cureul.SetParams(0.0f, 0.0f, 0.0f);

		//CMotionPoint* curmp = 0;
		//curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
		//if (curmp) {
		//
		//	EFbxRotationOrder rotationorder;
		//	srcbone->GetFbxNodeOnLoad()->GetRotationOrder(FbxNode::eSourcePivot, rotationorder);
		//
		//	ChaMatrix localmat = curmp->GetLocalMat();
		//	eulq = ChaMatrix2Q(localmat);
		//	eulq.Q2EulXYZusingMat(
		//		IntRotationOrder(rotationorder),
		//		0,
		//		ChaVector3(0.0f, 0.0f, 0.0f), &cureul, 0, 0, 1);
		//}
		//else {
		//	cureul.SetParams(0.0f, 0.0f, 0.0f);
		//}

		//if (srcbone->GetParModel() && srcbone->GetParModel()->IsCameraLoaded()) {
		//
		//	EnterCriticalSection(&g_CritSection_FbxSdk);
		//	cureul = srcbone->GetParModel()->CalcCameraFbxEulXYZ(srcmotid, roundingframe);
		//	//####  rotorder注意  #####
		//	LeaveCriticalSection(&g_CritSection_FbxSdk);
		//	//cureul.SetParams(0.0f, 0.0f, 0.0f);
		//
		//}
		//else {
		//	_ASSERT(0);
		//	cureul.SetParams(0.0f, 0.0f, 0.0f);
		//}
	}
	else if (srcbone->IsNull()) {

		//###########
		//eNullの場合
		//###########

		if (srcbone->IsNullAndChildIsCamera()) {

			//##############################
			//子供にeCameraを持つeNullの場合
			//##############################

			CMotionPoint* curmp = 0;
			curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
			if (curmp) {
				
				EFbxRotationOrder rotationorder;
				srcbone->GetFbxNodeOnLoad()->GetRotationOrder(FbxNode::eSourcePivot, rotationorder);

				ChaMatrix localmat = curmp->GetLocalMat();
				eulq = ChaMatrix2Q(localmat);

				int cameranotmodify180flag = 1;
				eulq.Q2EulXYZusingMat(
					IntRotationOrder(rotationorder),//!!!!!! eCamera* --> ROTORDER_*への変換
					0,
					ChaVector3(0.0f, 0.0f, 0.0f), &cureul, cameranotmodify180flag);
			}
			else {
				cureul.SetParams(0.0f, 0.0f, 0.0f);
			}


			//CMotionPoint* curmp = 0;
			//curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
			//if (curmp) {
			//	
			//	EFbxRotationOrder rotationorder;
			//	srcbone->GetFbxNodeOnLoad()->GetRotationOrder(FbxNode::eSourcePivot, rotationorder);
			//	int orgrotationorder = IntRotationOrder(rotationorder);
			//
			//	FbxTime fbxtime;
			//	fbxtime.SetSecondDouble(roundingframe / 30.0);
			//	FbxAMatrix fbxlocalmat;
			//	fbxlocalmat = srcbone->GetFbxNodeOnLoad()->EvaluateLocalTransform(fbxtime, FbxNode::eSourcePivot, true, true);
			//	ChaMatrix localmat = ChaMatrixFromFbxAMatrix(fbxlocalmat);
			//	eulq = ChaMatrix2Q(localmat);
			//	eulq.Q2EulXYZusingMat(orgrotationorder, 
			//		//&axisq, 
			//		0,
			//		ChaVector3(0.0f, 0.0f, 0.0f), &cureul, 0, 0, 1);
			//
			//}
			//else {
			//	cureul.SetParams(0.0f, 0.0f, 0.0f);
			//}
		}
		else {

			//##############################
			//子供にeCameraを持たないeNullの場合
			//##############################

			if (srcbone->GetFbxNodeOnLoad()) {
				EnterCriticalSection(&g_CritSection_FbxSdk);
				FbxTime fbxtime;
				fbxtime.SetSecondDouble(roundingframe / srcbone->GetParModel()->GetFbxTimeScale());
				FbxVector4 orgfbxeul = srcbone->GetFbxNodeOnLoad()->EvaluateLocalRotation(fbxtime, FbxNode::eSourcePivot, true, true);
				cureul.SetParams(orgfbxeul, false);
				//####  rotorder注意  #####
				LeaveCriticalSection(&g_CritSection_FbxSdk);
				return cureul;
			}
			else {
				return cureul;
			}
		}
	}
	else {
		return cureul;
	}

	return cureul;


	//CMotionPoint* curmp;
	//curmp = GetMotionPoint(srcmotid, roundingframe);
	//if (curmp){
	//	ChaVector3 oldeul = curmp->GetLocalEul();
	//	if (IsSameEul(oldeul, cureul) == 0){
	//		return cureul;
	//	}
	//	else{
	//		return oldeul;
	//	}
	//}
	//else{
	//	return cureul;
	//}
}

int ChaCalcFunc::SetWorldMat(CBone* srcbone, bool limitdegflag, int wallscrapingikflag, bool directsetflag,
	bool infooutflag, int setchildflag,
	int srcmotid, double srcframe, ChaMatrix srcmat, int onlycheck, bool fromiktarget)
{
	double roundingframe = RoundingTime(srcframe);

	if (!srcbone) {
		_ASSERT(0);
		return 1;
	}

	//2023/04/28
	if (srcbone->IsNotSkeleton() && !srcbone->IsNullAndChildIsCamera()) {
		return 0;
	}


	//if pose is change, return 1 else return 0
	CMotionPoint* curmp;
	curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		return 0;
	}

	curmp->SetCalcLimitedWM(0);//2023/01/14 limited　フラグ　リセット


	int ismovable = 0;


	//変更前を保存
	ChaMatrix saveworldmat;
	ChaVector3 saveeul;
	saveworldmat = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp);
	saveeul = srcbone->GetLocalEul(limitdegflag, srcmotid, roundingframe, curmp);


	//if ((directsetflag == false) && (g_underRetargetFlag == false)){

	if (directsetflag == false) {
		ChaMatrix beflocalmat;
		ChaMatrix newlocalmat;
		beflocalmat.SetIdentity();
		newlocalmat.SetIdentity();
		if (srcbone->GetParent(false)) {
			//eNullもアニメーション可能
			//GetENullMatrixを修正してCalcEnullMatReqで計算するようにしたところが2023/06/26から変わったところ
			//SetWorldMat()時には　回転計算用のローカル行列取得時に　parenetがeNullの場合関してもGetWorldMat[invNode * CalcENullMat]を使用
			//Fbx回転計算時には　CalcLocalEulXYZ()内にて　parentがeNullの場合　invNode * CalcENullMatを使用
			//Fbx移動計算時には　CalcFbxLocalMatrix内にて　parentがeNullの場合　GetENullMatrixを使用

			ChaMatrix currentparentwm;
			currentparentwm = srcbone->GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
			beflocalmat = saveworldmat * ChaMatrixInv(currentparentwm);
			newlocalmat = srcmat * ChaMatrixInv(currentparentwm);
		}
		else {
			beflocalmat = saveworldmat;
			newlocalmat = srcmat;
		}

		//calc bef SRT
		ChaMatrix befrmat;
		befrmat.SetIdentity();
		ChaVector3 befscalevec;
		befscalevec.SetParams(1.0f, 1.0f, 1.0f);
		ChaVector3 beftvec;
		beftvec.SetParams(0.0f, 0.0f, 0.0f);
		GetSRTMatrix(beflocalmat, &befscalevec, &befrmat, &beftvec);

		//calc new SRT
		ChaMatrix newsmat, newrmat, newtmat, newtanimmat;
		newsmat.SetIdentity();
		newrmat.SetIdentity();
		newtmat.SetIdentity();
		newtanimmat.SetIdentity();
		GetSRTandTraAnim(newlocalmat, srcbone->GetNodeMat(), &newsmat, &newrmat, &newtmat, &newtanimmat);

		//calc new eul
		srcbone->SetWorldMat(limitdegflag, srcmotid, roundingframe, srcmat, curmp);//tmp time
		ChaVector3 neweul;
		neweul.SetParams(0.0f, 0.0f, 0.0f);
		neweul = srcbone->CalcLocalEulXYZ(limitdegflag, -1, srcmotid, roundingframe, BEFEUL_BEFFRAME);


		//if (g_limitdegflag == true) {
		if (!srcbone->IsNullAndChildIsCamera() && 
			//(limitdegflag == true) && (srcbone->GetBtForce() == 0)) {//2023/01/28 物理シミュは　自前では制限しない
			(limitdegflag == true)) {//2024/09/13 物理シミュも　自前で制限する
			ismovable = srcbone->ChkMovableEul(neweul);
		}
		else {
			ismovable = 1;
		}

		if (infooutflag == true) {
			OutputToInfoWnd(INFOCOLOR_INFO, L"CBone::SetWorldMat : %s : frame %5.1lf : neweul [%f, %f, %f] : ismovable %d", srcbone->GetWBoneName(), roundingframe, neweul.x, neweul.y, neweul.z, ismovable);
		}

		if (onlycheck == 0) {
			if (ismovable == 1) {
				int inittraflag0 = 0;
				//子ジョイントへの波及は　SetWorldMatFromEulAndScaleAndTra内でしている
				srcbone->SetWorldMatFromEulAndScaleAndTra(limitdegflag, inittraflag0, setchildflag,
					saveworldmat, neweul, befscalevec, ChaMatrixTraVec(newtanimmat), srcmotid, roundingframe);//setchildflag有り!!!!
				//curmp->SetBefWorldMat(saveworldmat);
				//srcbone->SetLocalEul(limitdegflag, srcmotid, roundingframe, neweul, curmp);//<---SetWorldMatFromEulAnd...内でする
				if (limitdegflag == true) {
					curmp->SetCalcLimitedWM(2);
				}
			}
			else {
				if ((wallscrapingikflag == 1) || (fromiktarget == true)) {//PosConstraint用の回転時には　自動で壁すり処理をする
					//############################################
					//　遊び付きリミテッドIK
					//############################################
					ChaVector3 limiteul;
					//bool limitdegOnLimitEul1 = false;//2023/02/07 befeulはunlimited. 何回転もする場合にオーバー１８０度の角度で制限するために.
					//limiteul = LimitEul(neweul, GetBefEul(limitdegOnLimitEul1, srcmotid, roundingframe));
					limiteul = srcbone->LimitEul(neweul);
					int inittraflag0 = 0;
					//子ジョイントへの波及は　SetWorldMatFromEulAndScaleAndTra内でしている
					srcbone->SetWorldMatFromEulAndScaleAndTra(limitdegflag, inittraflag0, setchildflag,
						saveworldmat, limiteul, befscalevec, ChaMatrixTraVec(newtanimmat), srcmotid, roundingframe);//setchildflag有り!!!!
					//srcbone->SetLocalEul(limitdegflag, srcmotid, roundingframe, limiteul, curmp);//<---SetWorldMatFromEulAnd...内でする
					if (limitdegflag == true) {
						curmp->SetCalcLimitedWM(2);
					}

					ismovable = 1;//2024/09/13 壁すりでも動けば1をセット

				}
				else {
					//if (g_underIKRot == true) {
					if (srcbone->GetParModel() && srcbone->GetParModel()->GetUnderIKRot()) {
						srcbone->SetWorldMat(limitdegflag, srcmotid, roundingframe, saveworldmat, curmp);
						srcbone->SetLocalEul(limitdegflag, srcmotid, roundingframe, saveeul, curmp);
						if (limitdegflag == true) {
							curmp->SetCalcLimitedWM(2);
						}
					}
					else {
						ChaVector3 limiteul;
						////bool limitdegOnLimitEul2 = false;//2023/02/07 befeulはunlimited. 何回転もする場合にオーバー１８０度の角度で制限するために.
						////limiteul = LimitEul(neweul, GetBefEul(limitdegOnLimitEul2, srcmotid, roundingframe));

						//limiteul = srcbone->LimitEul(neweul);<--- これだと壁すりになる

						//############################################################################################################
						//2023/10/25
						//角度制限により動かさない場合にbefeul.befframeeulを使うので　ApplyNewLimitsToWM()はマルチスレッド化出来ない
						//############################################################################################################
						BEFEUL befeul = srcbone->GetBefEul(limitdegflag, srcmotid, roundingframe);
						limiteul = befeul.befframeeul;//befframeeulをlimiteulとして使用する

						int inittraflag0 = 0;
						//子ジョイントへの波及は　SetWorldMatFromEulAndScaleAndTra内でしている
						srcbone->SetWorldMatFromEulAndScaleAndTra(limitdegflag, inittraflag0, setchildflag,
							saveworldmat, limiteul, befscalevec, ChaMatrixTraVec(newtanimmat), srcmotid, roundingframe);//setchildflag有り!!!!
						//srcbone->SetLocalEul(limitdegflag, srcmotid, roundingframe, limiteul, curmp);//<---SetWorldMatFromEulAnd...内でする
						if (limitdegflag == true) {
							curmp->SetCalcLimitedWM(2);
						}
					}
				}
			}
		}
		else {
			//only check : 仮セットしていたのを元に戻す
			srcbone->SetWorldMat(limitdegflag, srcmotid, roundingframe, saveworldmat, curmp);
			srcbone->SetLocalEul(limitdegflag, srcmotid, roundingframe, saveeul, curmp);
		}
	}
	else {

		//directsetflag == true

		ismovable = 1;
		if (onlycheck == 0) {
			srcbone->SetWorldMat(limitdegflag, srcmotid, roundingframe, srcmat, curmp);

			//if (g_underCopyW2LW) {
			if (srcbone->GetParModel() && srcbone->GetParModel()->GetUnderCopyW2LW()) {
				//#########################
				//2023/10/25
				//コピー中はそのままセット
				//#########################
				bool unlimiteddegflag = false;
				bool limiteddegflag = true;
				ChaVector3 unlimitedeul = srcbone->GetLocalEul(unlimiteddegflag, srcmotid, roundingframe, curmp);
				srcbone->SetLocalEul(limiteddegflag, srcmotid, roundingframe, unlimitedeul, curmp);
			}
			else {
				ChaVector3 neweul = srcbone->CalcLocalEulXYZ(limitdegflag, -1, srcmotid, roundingframe, BEFEUL_BEFFRAME);
				srcbone->SetLocalEul(limitdegflag, srcmotid, roundingframe, neweul, curmp);
			}

			//else if ((g_underIKRotApplyFrame == true) || (g_underIKRot == false)) {
			//	ChaVector3 neweul = srcbone->CalcLocalEulXYZ(limitdegflag, -1, srcmotid, roundingframe, BEFEUL_BEFFRAME);
			//	srcbone->SetLocalEul(limitdegflag, srcmotid, roundingframe, neweul, curmp);
			//}
			//else {
			//	//#####################################################################################################
			//	//2023/10/19
			//	//PostIK処理の際には　後からCalcBoneEul()を呼ぶので　ここではeulの計算を省略してみる(処理高速化のため)
			//	//#####################################################################################################
			//}

			if (limitdegflag == true) {
				curmp->SetCalcLimitedWM(2);
			}

			if (setchildflag == 1) {
				if (srcbone->GetChild(false)) {
					bool setbroflag = true;
					srcbone->GetChild(false)->UpdateParentWMReq(limitdegflag,
						setbroflag, srcmotid, roundingframe,
						saveworldmat, srcmat);
				}
			}
		}
		else {
			//only check : 仮セットしていたのを元に戻す
			srcbone->SetWorldMat(limitdegflag, srcmotid, roundingframe, saveworldmat, curmp);
			srcbone->SetLocalEul(limitdegflag, srcmotid, roundingframe, saveeul, curmp);
		}
	}


	return ismovable;
}

int ChaCalcFunc::SetBtMatLimited(CBone* srcbone, bool limitdegflag, 
	bool directsetflag, bool setchildflag, ChaMatrix srcmat)
{
	//2024/04/13メモ
	//物理を柔らかく揺らすために剛体の結合を柔らかく設定した
	//剛体の接続を柔らかく設定した影響で物理の角度制限が効かなくなっていた
	//よって一度は取り下げたが自前の角度制限を物理に対しても適用することにした


	if (!srcbone) {
		_ASSERT(0);
		return 1;
	}

	//2023/04/28
	if (srcbone->IsNotSkeleton() && !srcbone->IsNullAndChildIsCamera()) {
		return 0;
	}

	int ismovable = 0;


	//変更前を保存
	ChaVector3 saveeul;
	ChaMatrix saveworldmat = srcbone->GetBtMat(false);//レンダーフレームのbtmat
	saveeul = srcbone->GetBtEul();


	ChaMatrix beflocalmat;
	ChaMatrix newlocalmat;
	beflocalmat.SetIdentity();
	newlocalmat.SetIdentity();
	if (srcbone->GetParent(false)) {
		//eNullもアニメーション可能
		//GetENullMatrixを修正してCalcEnullMatReqで計算するようにしたところが2023/06/26から変わったところ
		//SetWorldMat()時には　回転計算用のローカル行列取得時に　parenetがeNullの場合関してもGetWorldMat[invNode * CalcENullMat]を使用
		//Fbx回転計算時には　CalcLocalEulXYZ()内にて　parentがeNullの場合　invNode * CalcENullMatを使用
		//Fbx移動計算時には　CalcFbxLocalMatrix内にて　parentがeNullの場合　GetENullMatrixを使用

		ChaMatrix befparentwm;
		befparentwm = srcbone->GetParent(false)->GetBtMat(false);
		beflocalmat = saveworldmat * ChaMatrixInv(befparentwm);

		ChaMatrix newparentwm;
		newparentwm = srcbone->GetParent(false)->GetBtMat(true);
		newlocalmat = srcmat * ChaMatrixInv(newparentwm);
	}
	else {
		beflocalmat = saveworldmat;
		newlocalmat = srcmat;
	}

	//calc bef SRT
	ChaMatrix befsmat, befrmat, beftmat, beftanimmat;
	befsmat.SetIdentity();
	befrmat.SetIdentity();
	beftmat.SetIdentity();
	beftanimmat.SetIdentity();
	GetSRTandTraAnim(beflocalmat, srcbone->GetNodeMat(), &befsmat, &befrmat, &beftmat, &beftanimmat);


	//calc new SRT
	ChaMatrix newsmat, newrmat, newtmat, newtanimmat;
	newsmat.SetIdentity();
	newrmat.SetIdentity();
	newtmat.SetIdentity();
	newtanimmat.SetIdentity();
	GetSRTandTraAnim(newlocalmat, srcbone->GetNodeMat(), &newsmat, &newrmat, &newtmat, &newtanimmat);

	////calc new eul
	//srcbone->SetWorldMat(limitdegflag, srcmotid, roundingframe, srcmat, curmp);//tmp time
	//ChaVector3 neweul.SetParams(0.0f, 0.0f, 0.0f);
	//neweul = srcbone->CalcLocalEulXYZ(limitdegflag, -1, srcmotid, roundingframe, BEFEUL_BEFFRAME);

	CQuaternion axisq;
	axisq.RotationMatrix(srcbone->GetNodeMat());
	ChaVector3 neweul;
	neweul.SetParams(0.0f, 0.0f, 0.0f);
	CQuaternion eulq;
	//eulq.RotationMatrix(srcmat);//<--- global !!!
	eulq.RotationMatrix(newlocalmat);//local !!!
	int isfirstbone = 0;
	int isendbone = 0;
	{
		CBone* parentbone = srcbone->GetParent(false);

		if (parentbone && parentbone->IsSkeleton()) {
			isfirstbone = 0;
		}
		else {
			isfirstbone = 1;
		}
		if (srcbone->GetChild(false) && srcbone->GetChild(false)->IsSkeleton()) {
			if (srcbone->GetChild(false)->GetChild(false) && srcbone->GetChild(false)->GetChild(false)->IsSkeleton()) {
				isendbone = 0;
			}
			else {
				isendbone = 1;
			}
		}
		else {
			isendbone = 1;
		}
		//int notmodify180flag = srcbone->GetNotModify180Flag(srcmotid, roundingframe);
		//int notmodify180flag = 1;//!!!!!!!!!!!!!!!
		int notmodify180flag = 0;//!!!!!!!!!!!!!!!
		BEFEUL befeul;
		befeul.Init();
		befeul.befframeeul = saveeul;
		befeul.currentframeeul = saveeul;
		eulq.Q2EulXYZusingQ(true, false, &axisq, befeul, &neweul, isfirstbone, isendbone, notmodify180flag);
	}

	if (limitdegflag == true) {
		ismovable = srcbone->ChkMovableEul(neweul);
	}
	else {
		ismovable = 1;
	}

	if (directsetflag) {
		if (setchildflag == false) {
			srcbone->SetBtMat(srcmat);
			srcbone->SetBtEul(neweul);
		}else{
			srcbone->UpdateCurrentWM(limitdegflag, 0, 0.0, srcmat);
		}
	}
	else {
		if (ismovable == 1) {

			srcbone->SetBtMovable(true);//2024/05/02

			//ChaMatrix setlocalrotmat = eulq.MakeRotMatX();

			//####################################################################################################
			//2024/05/02
			//LimitEul時にはismovable == 1の場合にも25%(%は後でGUIで設定可能に)の変化量に抑えて　パタパタするのを緩和してみる
			// 
			//90fpsでうまくいくように設定したものを30fpsで再生した場合に　"以前は"　パタパタカクカクが目立った
			//今回の修正結果をテストしてみたところ　90fps時よりもLimitRateを大きく設定することと上記２５％補間により　パタパタ緩和
			//
			//上記２５％の部分をDispAndLimitsメニューのスライダーで設定：g_physicalMovableRate
			//####################################################################################################

			CQuaternion saveq;
			saveq.RotationMatrix(beflocalmat);
			CQuaternion calcq1;
			calcq1.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
			//calcq1 = eulq.Slerp(saveq, 100, 50);
			calcq1 = saveq.Slerp(eulq, 100, g_physicalMovableRate);//DispAndLimitsのスライダー設定値g_physicalMovableRate％
			ChaMatrix setlocalrotmat = calcq1.MakeRotMatX();

			ChaMatrix setlocalmat;
			bool sflag = true;
			bool tanimflag = true;
			//setlocalmat = ChaMatrixFromSRTraAnim(sflag, tanimflag, srcbone->GetNodeMat(), &befsmat, &setlocalrotmat, &beftanimmat);
			setlocalmat = ChaMatrixFromSRTraAnim(sflag, tanimflag, srcbone->GetNodeMat(), &befsmat, &setlocalrotmat, &newtanimmat);

			ChaMatrix setmat;
			if (srcbone->GetParent(false)) {
				setmat = setlocalmat * srcbone->GetParent(false)->GetBtMat(true);
			}
			else {
				setmat = setlocalmat;
			}

			if (setchildflag == false) {
				srcbone->SetBtMat(setmat);
				int notmodify180flag3 = 0;//!!!!!!!!!!!!!!!
				BEFEUL befeul3;
				befeul3.Init();
				befeul3.befframeeul = saveeul;
				befeul3.currentframeeul = saveeul;
				ChaVector3 seteul;
				seteul.SetParams(0.0f, 0.0f, 0.0f);
				
				//eulq.Q2EulXYZusingQ(true, false, &axisq, befeul3, &seteul, isfirstbone, isendbone, notmodify180flag3);
				calcq1.Q2EulXYZusingQ(true, false, &axisq, befeul3, &seteul, isfirstbone, isendbone, notmodify180flag3);

				srcbone->SetBtEul(seteul);
			}
			else {
				srcbone->UpdateCurrentWM(limitdegflag, 0, 0.0, setmat);
			}

			//if (setchildflag == false) {
			//	srcbone->SetBtMat(setmat);
			//	srcbone->SetBtEul(neweul);
			//}
			//else {
			//	srcbone->UpdateCurrentWM(limitdegflag, 0, 0.0, setmat);
			//}
		}
		else if (ismovable == 0) {

			srcbone->SetBtMovable(false);//2024/05/02

			//制限値を越えている場合　
			// 　壁すり処理をする
			//	//############################################
			//	//　遊び付きリミテッドIK
			//	//############################################
			ChaVector3 limiteul;
			limiteul = srcbone->LimitEul(neweul);//壁すり
			CQuaternion limitq;
			limitq.SetRotationXYZ(&axisq, limiteul);
			CQuaternion saveq;
			saveq.RotationMatrix(beflocalmat);
			//saveq.SetRotationXYZ(&axisq, saveeul);
			CQuaternion calcq1;
			calcq1 = CQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
			//calcq1 = limitq.Slerp(eulq, 100, g_limitrate);//LimitEulerプレートメニューのlimit rate for physicsのスライダー値(%)
			//calcq1 = eulq.Slerp(limitq, 100, g_limitrate);//2024/04/15 limitrateが実質FreeRateになっていたので修正　新しいlimitrate = (100 - 古いlimitrate)
			//calcq1 = eulq.Slerp(limitq, 100, srcbone->GetLimitRate());//2024/04/17 limitrateはCBoneごとの管理になった

			int limitrate = (int)((double)srcbone->GetLimitRate() * g_physicalLimitScale + 0.0001);
			limitrate = max(0, limitrate);
			limitrate = min(100, limitrate);
			calcq1 = eulq.Slerp(limitq, 100, limitrate);//2024/04/17　fpsに応じて設定するシーンごとのscaleをlimitrateに掛ける

			CQuaternion calcq2;
			calcq2 = CQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
			////calcq2 = calcq1.Slerp(saveq, 100, g_limitrate);
			//calcq2 = calcq1.Slerp(saveq, 100, 50);
			calcq2 = calcq1;

			ChaMatrix setlocalrotmat = calcq2.MakeRotMatX();
			ChaMatrix setlocalmat;
			bool sflag = true;
			bool tanimflag = true;

			//setlocalmat = ChaMatrixFromSRTraAnim(sflag, tanimflag, srcbone->GetNodeMat(), &befsmat, &setlocalrotmat, &beftanimmat);
			setlocalmat = ChaMatrixFromSRTraAnim(sflag, tanimflag, srcbone->GetNodeMat(), &befsmat, &setlocalrotmat, &newtanimmat);

			ChaMatrix setmat;
			if (srcbone->GetParent(false)) {
				setmat = setlocalmat * srcbone->GetParent(false)->GetBtMat(true);
			}
			else {
				setmat = setlocalmat;
			}

			if (setchildflag == false) {
				srcbone->SetBtMat(setmat);
				int notmodify180flag3 = 0;//!!!!!!!!!!!!!!!
				BEFEUL befeul3;
				befeul3.Init();
				befeul3.befframeeul = saveeul;
				befeul3.currentframeeul = saveeul;
				ChaVector3 seteul;
				seteul.SetParams(0.0f, 0.0f, 0.0f);
				calcq2.Q2EulXYZusingQ(true, false, &axisq, befeul3, &seteul, isfirstbone, isendbone, notmodify180flag3);
				srcbone->SetBtEul(seteul);
			}
			else {
				srcbone->UpdateCurrentWM(limitdegflag, 0, 0.0, setmat);
			}
		}
		else if(ismovable == 2) {

			srcbone->SetBtMovable(false);//2024/05/02

			//###############################################
			//2024/04/23
			//回転禁止フラグが設定されている場合
			//前のフレームの姿勢へと制限する
			//###############################################
			CQuaternion saveq;
			saveq.RotationMatrix(beflocalmat);
			//int limitrate = (int)((double)srcbone->GetLimitRate() * g_physicalLimitScale + 0.0001);
			//limitrate = max(0, limitrate);
			//limitrate = min(100, limitrate);
			CQuaternion calcq1;
			calcq1 = CQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
			//calcq1 = eulq.Slerp(saveq, 100, limitrate);
			calcq1 = saveq;

			ChaMatrix setlocalrotmat = calcq1.MakeRotMatX();
			ChaMatrix setlocalmat;
			bool sflag = true;
			bool tanimflag = true;
			//setlocalmat = ChaMatrixFromSRTraAnim(sflag, tanimflag, srcbone->GetNodeMat(), &befsmat, &setlocalrotmat, &beftanimmat);
			setlocalmat = ChaMatrixFromSRTraAnim(sflag, tanimflag, srcbone->GetNodeMat(), &befsmat, &setlocalrotmat, &newtanimmat);

			ChaMatrix setmat;
			if (srcbone->GetParent(false)) {
				setmat = setlocalmat * srcbone->GetParent(false)->GetBtMat(true);
			}
			else {
				setmat = setlocalmat;
			}

			if (setchildflag == false) {
				srcbone->SetBtMat(setmat);
				int notmodify180flag3 = 0;//!!!!!!!!!!!!!!!
				BEFEUL befeul3;
				befeul3.Init();
				befeul3.befframeeul = saveeul;
				befeul3.currentframeeul = saveeul;
				ChaVector3 seteul;
				seteul.SetParams(0.0f, 0.0f, 0.0f);
				calcq1.Q2EulXYZusingQ(true, false, &axisq, befeul3, &seteul, isfirstbone, isendbone, notmodify180flag3);
				srcbone->SetBtEul(seteul);
			}
			else {
				srcbone->UpdateCurrentWM(limitdegflag, 0, 0.0, setmat);
			}

			//srcbone->SetBtMat(saveworldmat);
			//srcbone->SetBtEul(saveeul);				

			//else {
			//	ChaVector3 limiteul;
			//	//############################################################################################################
			//	//2023/10/25
			//	//角度制限により動かさない場合にbefeul.befframeeulを使うので　ApplyNewLimitsToWM()はマルチスレッド化出来ない
			//	//############################################################################################################
			//	BEFEUL befeul = srcbone->GetBefEul(limitdegflag, srcmotid, roundingframe);
			//	limiteul = befeul.befframeeul;//befframeeulをlimiteulとして使用する
			//
			//	int inittraflag0 = 0;
			//	//子ジョイントへの波及は　SetWorldMatFromEulAndScaleAndTra内でしている
			//	srcbone->SetWorldMatFromEulAndScaleAndTra(limitdegflag, inittraflag0, setchildflag,
			//		saveworldmat, limiteul, befscalevec, ChaMatrixTraVec(newtanimmat), srcmotid, roundingframe);//setchildflag有り!!!!
			//	//srcbone->SetLocalEul(limitdegflag, srcmotid, roundingframe, limiteul, curmp);//<---SetWorldMatFromEulAnd...内でする
			//	if (limitdegflag == true) {
			//		curmp->SetCalcLimitedWM(2);
			//	}
			//}
		}
	}

	return ismovable;
}



int ChaCalcFunc::SetWorldMat(CBone* srcbone, bool limitdegflag,
	int srcmotid, double srcframe, ChaMatrix srcmat, CMotionPoint* srcmp)//default : srcmp = 0
{
	double roundingframe = RoundingTime(srcframe);

	if (!srcbone) {
		_ASSERT(0);
		return 1;
	}

	//2023/04/28
	if (srcbone->IsNotSkeleton() && !srcbone->IsNullAndChildIsCamera()) {
		return 0;
	}

	if (srcmp) {
		if (limitdegflag == false) {
			srcmp->SetWorldMat(srcmat);
		}
		else {
			srcmp->SetLimitedWM(srcmat);
			//srcmp->SetCalcLimitedWM(2);
		}
	}
	else {
		CMotionPoint* curmp;
		curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
		if (curmp) {
			if (limitdegflag == false) {
				curmp->SetWorldMat(srcmat);
			}
			else {
				curmp->SetLimitedWM(srcmat);
				//curmp->SetCalcLimitedWM(2);
			}
		}
	}

	return 0;
}

int ChaCalcFunc::SetWorldMatFromEulAndScaleAndTra(CBone* srcbone, bool limitdegflag, 
	int inittraflag, int setchildflag,
	ChaMatrix befwm, ChaVector3 srceul, ChaVector3 srcscale, ChaVector3 srctra, int srcmotid, double srcframe)
{
	if (!srcbone) {
		_ASSERT(0);
		return 1;
	}


	//anglelimitをした後のオイラー角が渡される。anglelimitはCBone::SetWorldMatで処理する。
	//if (!m_child) {
	//	return 0;
	//}

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (srcbone->IsNotSkeleton() && !srcbone->IsNullAndChildIsCamera()) {
		return 0;
	}


	ChaMatrix newscalemat;
	newscalemat.SetIdentity();//2023/02/12 抜け落ちてた
	ChaMatrixScaling(&newscalemat, srcscale.x, srcscale.y, srcscale.z);//!!!!!!!!!!!!  new scale
	ChaMatrix newtramat;
	ChaMatrixIdentity(&newtramat);
	ChaMatrixTranslation(&newtramat, srctra.x, srctra.y, srctra.z);//TraAnimをそのまま

	CMotionPoint* curmp;
	curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		_ASSERT(0);
		return 1;
	}


	//ChaMatrix curwm;
	//curwm = GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp);
	//ChaMatrix curlocalmat;
	//if (GetParent()) {
	//	ChaMatrix parmat;
	//	parmat = GetParent()->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
	//	curlocalmat = curwm * ChaMatrixInv(parmat);
	//}
	//else {
	//	curlocalmat = curwm;
	//}
	//ChaMatrix cursmat, currmat, curtmat, curtanimmat;
	//GetSRTandTraAnim(curlocalmat, GetNodeMat(), &cursmat, &currmat, &curtmat, &curtanimmat);


	ChaMatrix newlocalrotmat = srcbone->CalcLocalRotMatFromEul(srceul);
	ChaMatrix newlocalmat;
	bool sflag = true;
	bool tanimflag = (inittraflag == 0);
	newlocalmat = ChaMatrixFromSRTraAnim(sflag, tanimflag, srcbone->GetNodeMat(), &newscalemat, &newlocalrotmat, &newtramat);

	ChaMatrix newmat;
	if (srcbone->GetParent(false)) {
		ChaMatrix limitedparmat = srcbone->GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		newmat = newlocalmat * limitedparmat;
	}
	else {
		newmat = newlocalmat;
	}

	if (curmp) {
		//curmp->SetBefWorldMat(curmp->GetWorldMat());
		srcbone->SetWorldMat(limitdegflag, srcmotid, roundingframe, newmat, curmp);
		
		//#######################################################################################################################
		//2023/10/19
		//何回転もIKする際の正しいオイラー角は　CalcLocalEulXYZで計算するが　1000フレーム全ボーンIK３回転で３０秒くらい遅くなる
		//Main.cppのPostIK呼び出しの後で　CalcBoneEul()を呼び出してまとめて正しいオイラー角に直すことにした
		//よってここではSetLocalEul(srceul)で良い
		//#######################################################################################################################
		srcbone->SetLocalEul(limitdegflag, srcmotid, roundingframe, srceul, curmp);
		//2023/10/18 befframeをみてオイラー角を計算し直す
		//ChaVector3 neweul = srcbone->CalcLocalEulXYZ(limitdegflag, -1, srcmotid, roundingframe, BEFEUL_BEFFRAME);
		//srcbone->SetLocalEul(limitdegflag, srcmotid, roundingframe, neweul, curmp);


		if (limitdegflag == true) {
			curmp->SetCalcLimitedWM(2);
		}

		if (setchildflag == 1) {
			if (srcbone->GetChild(false)) {
				bool setbroflag = true;
				srcbone->GetChild(false)->UpdateParentWMReq(limitdegflag, 
					setbroflag,
					srcmotid, roundingframe, befwm, newmat);
			}
		}
	}
	else {
		_ASSERT(0);
	}

	return 0;
}


CMotionPoint* ChaCalcFunc::GetMotionPoint(CBone* srcbone, int srcmotid, double srcframe, bool onaddmotion) {
	if (!srcbone) {
		_ASSERT(0);
		return 0;
	}
	EnterCriticalSection(&(srcbone->m_CritSection_GetMotionPoint));

	//存在するときだけ返す。
	CMotionPoint* pbef = 0;
	CMotionPoint* pnext = 0;
	int existflag = 0;
	GetBefNextMP(srcbone, srcmotid, srcframe, &pbef, &pnext, &existflag, onaddmotion);
	if (existflag == 1) {
		LeaveCriticalSection(&(srcbone->m_CritSection_GetMotionPoint));
		return pbef;
	}
	else {
		LeaveCriticalSection(&(srcbone->m_CritSection_GetMotionPoint));
		return 0;
	}
}

ChaMatrix ChaCalcFunc::GetWorldMat(CBone* srcbone, bool limitdegflag,
	int srcmotid, double srcframe, CMotionPoint* srcmp, ChaVector3* dsteul)//default : dsteul = 0
{

	double roundingframe = RoundingTime(srcframe);

	ChaMatrix curmat;
	ChaMatrixIdentity(&curmat);
	if (!srcbone) {
		_ASSERT(0);
		return curmat;
	}


	if (!srcbone->GetParModel()) {
		_ASSERT(0);
		curmat.SetIdentity();
		return curmat;
	}


	//2023/04/28
	if (srcbone->IsNull()) {
		//2023/05/07  
		//return GetENullMatrix();

		//2023/05/09
		//CalcLocalEulXYZのローカル計算部分メモの理由により　eNullのWorldMatはIdentityを返すことにする
		//ただし　fbxのscaleとtra出力時の親行列としてはGetENullMatrix()を使う(CalcFbxLocalMatrix()参照)
		//Rotとtra,scaleの違いは　RotはNodeMat無しでローカル計算して軸としてNodeMatを指定　tra,scaleはローカル計算時にNodeMatを掛ける
		//return curmat;


		//2023/05/16 eNullのNodeMatがIdentityではなくなったため
		//return ChaMatrixInv(GetNodeMat()) * GetENullMatrix(srcframe);//!!!!!!!!!!!!

		//return ChaMatrixInv(GetNodeMat()) * GetTransformMat(srcframe, true);

		//EnterCriticalSection(&g_CritSection_FbxSdk);
		//ChaMatrix retmat = ChaMatrixInv(srcbone->GetNodeMat()) * srcbone->GetTransformMat(0.0, true);//!!!!!  1.2.0.26
		//LeaveCriticalSection(&g_CritSection_FbxSdk);
		//return retmat;
		////return ChaMatrixInv(GetNodeMat()) * GetTransformMat(0.0, true);//!!!!!  1.2.0.26

		//2023/06/27
		//CalcLocalEulXYZ()の検証で　ParentがeNullのときには　parentwmはIdentityにするべきだったので　それに合わせる
		//curmat.SetIdentity();

		if (srcmp) {
			if (limitdegflag == false) {
				curmat = srcmp->GetWorldMat();
				if (dsteul) {
					*dsteul = srcmp->GetLocalEul();
				}
			}
			else {
				curmat = srcmp->GetLimitedWM();
				if (dsteul) {
					*dsteul = srcmp->GetLimitedLocalEul();
				}
			}
		}
		else {
			CMotionPoint* curmp;
			curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
			if (curmp) {
				if (limitdegflag == false) {
					curmat = curmp->GetWorldMat();
					if (dsteul) {
						*dsteul = curmp->GetLocalEul();
					}
				}
				else {
					curmat = curmp->GetLimitedWM();
					if (dsteul) {
						*dsteul = curmp->GetLimitedLocalEul();
					}
				}
			}else{
				curmat.SetIdentity();	
			}
		}
		return curmat;
	}
	else if (srcbone->IsCamera()) {
		//bool multInvNodeMat = true;
		//return GetParModel()->GetCameraTransformMat(srcmotid, srcframe, g_cameraInheritMode, multInvNodeMat);
		//return ChaMatrixInv(GetNodeMat()) * GetTransformMat(0.0, true);//2023/07/05 Cameraの子供のスキンメッシュの形が　読み書き読み書き読みテストで形崩れしないように

		//EnterCriticalSection(&g_CritSection_FbxSdk);
		//ChaMatrix retmat = ChaMatrixInv(srcbone->GetNodeMat()) * srcbone->GetTransformMat(0.0, true);//!!!!!  1.2.0.26
		//LeaveCriticalSection(&g_CritSection_FbxSdk);
		//return retmat;

		if (srcmp) {
			if (limitdegflag == false) {
				curmat = srcmp->GetWorldMat();
				if (dsteul) {
					*dsteul = srcmp->GetLocalEul();
				}
			}
			else {
				curmat = srcmp->GetLimitedWM();
				if (dsteul) {
					*dsteul = srcmp->GetLimitedLocalEul();
				}
			}
		}
		else {
			CMotionPoint* curmp;
			curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
			if (curmp) {
				if (limitdegflag == false) {
					curmat = curmp->GetWorldMat();
					if (dsteul) {
						*dsteul = curmp->GetLocalEul();
					}
				}
				else {
					curmat = curmp->GetLimitedWM();
					if (dsteul) {
						*dsteul = curmp->GetLimitedLocalEul();
					}
				}
			}else{
				curmat.SetIdentity();	
			}
		}
		return curmat;
	}
	else if (srcbone->IsSkeleton()) {
		if (srcmp) {
			if (limitdegflag == false) {
				curmat = srcmp->GetWorldMat();
				if (dsteul) {
					*dsteul = srcmp->GetLocalEul();
				}
			}
			else {
				curmat = srcmp->GetLimitedWM();
				if (dsteul) {
					*dsteul = srcmp->GetLimitedLocalEul();
				}
			}
		}
		else {
			CMotionPoint* curmp;
			curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
			if (curmp) {
				if (limitdegflag == false) {
					curmat = curmp->GetWorldMat();
					if (dsteul) {
						*dsteul = curmp->GetLocalEul();
					}
				}
				else {
					curmat = curmp->GetLimitedWM();
					if (dsteul) {
						*dsteul = curmp->GetLimitedLocalEul();
					}
				}
			}else{
				curmat.SetIdentity();	
			}
		}
		return curmat;

	}
	else {
		curmat.SetIdentity();
		return curmat;//!!!!!!!!!!!!  SkeletonでもNullでもCameraでも無い場合　identityを返す
	}

}

ChaVector3 ChaCalcFunc::LimitEul(CBone* srcbone, ChaVector3 srceul)
{
	if (!srcbone) {
		_ASSERT(0);
		return srceul;
	}

	const float thdeg = 165.0f;

	ChaVector3 reteul;
	reteul.SetParams(0.0f, 0.0f, 0.0f);
	ChaVector3 tmpeul;
	tmpeul.SetParams(0.0f, 0.0f, 0.0f);


	//2023/04/28
	if (srcbone->IsNotSkeleton()) {
		return reteul;
	}


	tmpeul.x = LimitAngle(srcbone, AXIS_X, srceul.x);
	tmpeul.y = LimitAngle(srcbone, AXIS_Y, srceul.y);
	tmpeul.z = LimitAngle(srcbone, AXIS_Z, srceul.z);


	//何回転もする場合に　純粋に角度で制限するためにコメントアウト
	//CQuaternion calcq;
	//calcq.ModifyEuler360(&tmpeul, &srcbefeul, 0);

	reteul = tmpeul;


	return reteul;
}

float ChaCalcFunc::LimitAngle(CBone* srcbone, enum tag_axiskind srckind, float srcval)
{
	if (!srcbone) {
		_ASSERT(0);
		return srcval;
	}


	//2023/04/28
	if (srcbone->IsNotSkeleton()) {
		return 0.0f;
	}


	srcbone->SetAngleLimitOff();
	ANGLELIMIT tmpanglelimit;
	tmpanglelimit.Init();
	bool dummylimitdegflag = false;
	int setchkflag = 0;
	tmpanglelimit = srcbone->GetAngleLimit(dummylimitdegflag, setchkflag);

	if (tmpanglelimit.limitoff[srckind] == 1) {
		return srcval;
	}
	else {
		float newval = srcval;

		float cmpvalupper, cmpvallower;
		cmpvalupper = srcval;
		cmpvallower = srcval;

		if (abs(tmpanglelimit.upper[srckind] - tmpanglelimit.lower[srckind]) > EULLIMITPLAY) {
			//リミット付近でもIKが動くためには遊びの部分が必要

			if (cmpvalupper > tmpanglelimit.upper[srckind]) {
				newval = fmin(cmpvalupper, (float)(tmpanglelimit.upper[srckind] - EULLIMITPLAY));
			}
			if (cmpvallower < tmpanglelimit.lower[srckind]) {
				newval = fmax(cmpvallower, (float)(tmpanglelimit.lower[srckind] + EULLIMITPLAY));
			}
		}
		else {
			//lowerとupperの間がEULLIMITPLAYより小さいとき

			if (cmpvalupper > tmpanglelimit.upper[srckind]) {
				newval = (float)(tmpanglelimit.upper[srckind]);
			}
			if (cmpvallower < tmpanglelimit.lower[srckind]) {
				newval = (float)(tmpanglelimit.lower[srckind]);
			}
		}

		return newval;
	}
}

int ChaCalcFunc::SetLocalEul(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe, ChaVector3 srceul, CMotionPoint* srcmp)
{
	if (!srcbone) {
		_ASSERT(0);
		return 1;
	}

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (srcbone->IsNotSkeleton() && !srcbone->IsNullAndChildIsCamera()) {
		return 0;
	}

	if (srcmp) {
		if (limitdegflag == false) {
			srcmp->SetLocalEul(srceul);
		}
		else {
			srcmp->SetLimitedLocalEul(srceul);
		}
	}
	else {
		CMotionPoint* curmp;
		curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
		if (curmp) {
			if (limitdegflag == false) {
				curmp->SetLocalEul(srceul);
			}
			else {
				curmp->SetLimitedLocalEul(srceul);
			}
		}
		else {
			//_ASSERT(0);
			return 1;
		}
	}

	return 0;
}

ChaVector3 ChaCalcFunc::GetLocalEul(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe, CMotionPoint* srcmp)
{
	double roundingframe = RoundingTime(srcframe);

	ChaVector3 reteul;
	reteul.SetParams(0.0f, 0.0f, 0.0f);
	if (!srcbone) {
		_ASSERT(0);
		return reteul;
	}

	//2023/04/28
	if (srcbone->IsNotSkeleton() && srcbone->IsNotCamera() && !srcbone->IsNullAndChildIsCamera()) {//2023/06/05 eCamera対応
		return reteul;
	}


	if (srcmp) {
		if (limitdegflag == false) {
			reteul = srcmp->GetLocalEul();
		}
		else {
			//2023/01/31
			//g_limitdegflag == trueの場合
			//GetWorldMat-->GetLimitedWorldMat-->CalcLocalEulAndSetLImitedEul
			//-->GetLocalEul(-->GetLimitedWorldMat)
			//上記のように呼ばれるので　この関数GetLocalEul内でGetLimitedWorldMatを呼び出すことは出来ない

			//if (srcmp->GetCalcLimitedWM() == 2) {
			//	reteul = srcmp->GetLimitedLocalEul();
			//}
			//else {
			//	GetLimitedWorldMat(srcmotid, roundingframe, &reteul);
			//}

			reteul = srcmp->GetLimitedLocalEul();

		}
	}
	else {
		CMotionPoint* curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
		if (curmp) {
			if (limitdegflag == false) {
				reteul = curmp->GetLocalEul();
			}
			else {
				//2023/01/31
				//g_limitdegflag == trueの場合
				//GetWorldMat-->GetLimitedWorldMat-->CalcLocalEulAndSetLImitedEul
				//-->GetLocalEul(-->GetLimitedWorldMat)
				//上記のように呼ばれるので　この関数GetLocalEul内でGetLimitedWorldMatを呼び出すことは出来ない

				//if (curmp->GetCalcLimitedWM() == 2) {
				//	reteul = curmp->GetLimitedLocalEul();
				//}
				//else {
				//	GetLimitedWorldMat(srcmotid, roundingframe, &reteul);
				//}

				reteul = curmp->GetLimitedLocalEul();
			}
		}
		else {
			//_ASSERT(0);
			reteul.SetParams(0.0f, 0.0f, 0.0f);
		}
	}

	return reteul;
}

int ChaCalcFunc::ChkMovableEul(CBone* srcbone, ChaVector3 srceul)
{

	//2023/01/10 : 事前計算しておきたい場合などがあるので　以下３行の条件はこの関数の呼び出し元で行う
	//if (g_limitdegflag == false){
	//	return 1;//movable
	//}

	if (!srcbone) {
		_ASSERT(0);
		return 0;
	}

	//2023/04/28
	if (srcbone->IsNotSkeleton() && !srcbone->IsNullAndChildIsCamera()) {
		return 0;
	}


	//##################################################
	//2024/04/23
	//物理シミュ回転禁止フラグが設定されている場合には　２を返す
	//##################################################
	if (srcbone->GetParent(false)) {
		CRigidElem* curre = srcbone->GetParent(false)->GetRigidElem(srcbone);
		if (curre && curre->GetForbidRotFlag()) {
			return 2;//!!!! 2 !!!!
		}
	}


	srcbone->SetAngleLimitOff();
	ANGLELIMIT tmpanglelimit;
	tmpanglelimit.Init();
	bool dummylimitdegflag = false;
	int setchkflag = 0;
	tmpanglelimit = srcbone->GetAngleLimit(dummylimitdegflag, setchkflag);


	int dontmove = 0;
	int axiskind;

	float chkval[3];
	chkval[0] = srceul.x;
	chkval[1] = srceul.y;
	chkval[2] = srceul.z;

	for (axiskind = AXIS_X; axiskind <= AXIS_Z; axiskind++) {
		if (tmpanglelimit.limitoff[axiskind] == 0) {
			//if (m_anglelimit.via180flag[axiskind] == 0) {
			if ((tmpanglelimit.lower[axiskind] > (int)chkval[axiskind]) || (tmpanglelimit.upper[axiskind] < (int)chkval[axiskind])) {
				dontmove++;
			}
			//}
			//else {
			//	//180度線(-180度線)を越えるように動く場合
			//	if ((m_anglelimit.lower[axiskind] <= (int)chkval[axiskind]) && (m_anglelimit.upper[axiskind] >= (int)chkval[axiskind])) {
			//		dontmove++;
			//	}
			//}
		}
	}

	if (dontmove != 0) {
		return 0;
	}
	else {
		return 1;//movable
	}
}


int ChaCalcFunc::CopyWorldToLimitedWorldOne(CModel* srcmodel, CBone* srcbone, int srcmotid, double srcframe)
{
	if (!srcmodel && !srcbone) {
		return 0;
	}

	if (srcmodel->GetNoBoneFlag() == true) {
		return 0;
	}

	CopyWorldToLimitedWorldReq(srcmodel, srcbone, srcmotid, srcframe);

	return 0;


}

void ChaCalcFunc::CopyWorldToLimitedWorldReq(CModel* srcmodel, CBone* srcbone, int srcmotid, double srcframe)
{
	if (!srcmodel) {
		_ASSERT(0);
		return;
	}

	if (srcbone) {

		if (srcbone->IsSkeleton()) {
			srcbone->CopyWorldToLimitedWorld(srcmotid, srcframe);
		}

		if (srcbone->GetChild(false)) {
			CopyWorldToLimitedWorldReq(srcmodel, srcbone->GetChild(false), srcmotid, srcframe);
		}
		if (srcbone->GetBrother(false)) {
			CopyWorldToLimitedWorldReq(srcmodel, srcbone->GetBrother(false), srcmotid, srcframe);
		}
	}
}

int ChaCalcFunc::CopyWorldToLimitedWorld(CBone* srcbone, int srcmotid, double srcframe)
{
	if (!srcbone) {
		_ASSERT(0);
		return 1;
	}

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28 2023/05/23
	if (srcbone->IsNotSkeleton() && srcbone->IsNotCamera()) {
		return 0;
	}

	CMotionPoint* curmp;
	curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
	if (curmp) {
		ChaMatrix currentwm;
		currentwm = curmp->GetWorldMat();

		ChaMatrix newwm;
		newwm.SetIdentity();
		if (srcbone->GetParent(false)) {
			if (srcbone->GetParent(false)->IsSkeleton()) {
				ChaMatrix unlimitedlocal;
				ChaMatrix parentunlimited;
				ChaMatrix parentlimited;
				unlimitedlocal.SetIdentity();
				parentunlimited.SetIdentity();
				parentlimited.SetIdentity();

				parentunlimited = srcbone->GetParent(false)->GetWorldMat(false, srcmotid, roundingframe, 0);
				parentlimited = srcbone->GetParent(false)->GetWorldMat(true, srcmotid, roundingframe, 0);

				unlimitedlocal = currentwm * ChaMatrixInv(parentunlimited);
				newwm = unlimitedlocal * parentlimited;
			}
			else if (srcbone->GetParent(false)->IsNull() || srcbone->GetParent(false)->IsCamera()) {
				newwm = currentwm;
			}
			else {
				_ASSERT(0);
				newwm = currentwm;
			}
		}
		else {
			newwm = currentwm;
		}

		//bool limitdegflag = true;
		//bool directsetflag = false;
		////bool directsetflag = true;//2023/02/08 copyなのでdirectset.
		//bool infooutflag = false;
		//int setchildflag = 1;//setchildflagは directsetflag == falseのときしか働かない
		//SetWorldMat(limitdegflag, directsetflag, infooutflag, setchildflag, srcmotid, roundingframe, newwm);

		bool limitdegflag = true;
		int wallscrapingikflag = 0;//directsetの場合は関係ない
		srcbone->UpdateCurrentWM(limitdegflag, srcmotid, roundingframe, newwm);

	}
	else {
		_ASSERT(0);
		return 1;
	}

	return 0;


}

void ChaCalcFunc::UpdateCurrentWM(CBone* srcbone, bool limitdegflag, 
	int srcmotid, double srcframe, ChaMatrix newwm)
{
	if (!srcbone) {
		_ASSERT(0);
		return;
	}


	//directsetで　ツリーの姿勢を更新　再帰

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28 2023/05/23
	if (srcbone->IsNotSkeleton() && srcbone->IsNotCamera() && !srcbone->IsNullAndChildIsCamera()) {
		return;
	}


	ChaMatrix befwm;
	ChaMatrix currentlocalmat;
	befwm.SetIdentity();
	//if ((g_previewFlag != 4) && (g_previewFlag != 5)) {
		befwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		if (srcbone->GetParent(false)) {
			ChaMatrix parentwm = srcbone->GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
			currentlocalmat = newwm * ChaMatrixInv(parentwm);
		}
		else {
			currentlocalmat = newwm;
		}
	//}
	//else {
	if ((g_previewFlag == 4) || (g_previewFlag == 5)) {
		befwm = srcbone->GetBtMat(true);
		if (srcbone->GetParent(false)) {
			ChaMatrix parentwm = srcbone->GetParent(false)->GetBtMat(true);
			currentlocalmat = newwm * ChaMatrixInv(parentwm);
		}
		else {
			currentlocalmat = newwm;
		}
	}
	//ChaMatrix befparentwm;
	//befparentwm.SetIdentity();
	//if (GetParent(false)) {
	//	befparentwm = GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
	//}
	//else {
	//	befparentwm.SetIdentity();
	//}

	//if ((g_previewFlag != 4) && (g_previewFlag != 5)) 
	{//btSimuの際にも必要な処理
		bool directsetflag = true;//directset !!!
		int wallscrapingikflag = 0;//directsetのときは関係ない
		bool infooutflag = false;
		int setchildflag = 0;
		int onlycheck = 0;
		bool fromiktarget = false;
		srcbone->SetWorldMat(limitdegflag, wallscrapingikflag, directsetflag, infooutflag, setchildflag,
			srcmotid, roundingframe, newwm, onlycheck, fromiktarget);

		CMotionPoint* curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
		if (curmp) {
			curmp->SetAbsMat(srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp));
		}
	}
	//else {
	if ((g_previewFlag == 4) || (g_previewFlag == 5)) {
		//bool directsetflag = true;
		//bool setchildflag = false;//この後ろの部分でUpdateParentWMReqを明示的に呼び出すので、ここのsetchildflagはfalse
		//srcbone->SetBtMatLimited(limitdegflag, directsetflag, setchildflag, newwm);

		int isfirstbone, isendbone;
		int notmodify180flag = 0;//!!!!!!!!!!!!!!!
		CBone* parentbone = srcbone->GetParent(false);
		if (parentbone && parentbone->IsSkeleton()) {
			isfirstbone = 0;
		}
		else {
			isfirstbone = 1;
		}
		if (srcbone->GetChild(false) && srcbone->GetChild(false)->IsSkeleton()) {
			if (srcbone->GetChild(false)->GetChild(false) && srcbone->GetChild(false)->GetChild(false)->IsSkeleton()) {
				isendbone = 0;
			}
			else {
				isendbone = 1;
			}
		}
		else {
			isendbone = 1;
		}

		CQuaternion axisq;
		axisq.RotationMatrix(srcbone->GetNodeMat());
		ChaVector3 saveeul = srcbone->GetBtEul();
		BEFEUL befeul;
		befeul.Init();
		befeul.befframeeul = saveeul;
		befeul.currentframeeul = saveeul;
		ChaVector3 seteul;
		seteul.SetParams(0.0f, 0.0f, 0.0f);
		CQuaternion setq;
		//setq.RotationMatrix(newwm);//global !!!!
		setq.RotationMatrix(currentlocalmat);//local !!!!
		setq.Q2EulXYZusingQ(true, false, &axisq, befeul, &seteul, isfirstbone, isendbone, notmodify180flag);

		srcbone->SetBtMat(newwm);
		srcbone->SetBtEul(seteul);
	}

	if (srcbone->GetChild(false)) {
		bool setbroflag2 = true;
		UpdateParentWMReq(srcbone->GetChild(false), limitdegflag, 
			setbroflag2, srcmotid, roundingframe,
			befwm, newwm);
	}
	//if (GetBrother() && (setbroflag == true)) {
	//	GetBrother()->UpdateParentWMReq(limitdegflag, setbroflag, srcmotid, roundingframe,
	//		befparentwm, befparentwm);
	//}
}

void ChaCalcFunc::UpdateParentWMReq(CBone* srcbone, bool limitdegflag, 
	bool setbroflag, int srcmotid, double srcframe, ChaMatrix oldparentwm, ChaMatrix newparentwm)
{
	if (!srcbone) {
		_ASSERT(0);
		return;
	}

	//directsetで　parentの姿勢を更新　再帰

	double roundingframe = RoundingTime(srcframe);

	ChaMatrix currentbefwm;
	ChaMatrix currentnewwm;
	currentbefwm.SetIdentity();
	currentnewwm.SetIdentity();

	if (srcbone->IsSkeleton() || srcbone->IsCamera() || //2023/05/23
		srcbone->IsNullAndChildIsCamera()) {
		//if ((g_previewFlag != 4) && (g_previewFlag != 5)) 
		{//btSimuの際にも必要な処理
			currentbefwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
			currentnewwm = currentbefwm * ChaMatrixInv(oldparentwm) * newparentwm;

			bool directsetflag = true;//directset !!!
			int wallscrapingikflag = 0;//directsetの場合は関係ない
			bool infooutflag = false;
			int setchildflag = 0;
			int onlycheck = 0;
			bool fromiktarget = false;
			srcbone->SetWorldMat(limitdegflag, wallscrapingikflag, directsetflag, infooutflag, setchildflag,
				srcmotid, roundingframe, currentnewwm, onlycheck, fromiktarget);

			CMotionPoint* curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
			if (curmp) {
				curmp->SetAbsMat(srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp));
			}
		}
		//else {
		if ((g_previewFlag == 4) || (g_previewFlag == 5)) {
			currentbefwm = srcbone->GetBtMat(true);
			currentnewwm = currentbefwm * ChaMatrixInv(oldparentwm) * newparentwm;

			bool directsetflag = true;
			int wallscrapingikflag = 0;//directsetの場合は関係ない
			bool setchildflag = false;//この後ろの部分でUpdateParentWMReqを明示的に呼び出すので、ここのsetchildflagはfalse
			srcbone->SetBtMatLimited(limitdegflag, directsetflag, setchildflag, currentnewwm);
		}

	}
	else if (srcbone->IsNull()) {
		currentbefwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		currentnewwm = currentbefwm * ChaMatrixInv(oldparentwm) * newparentwm;
	}
	else {
		_ASSERT(0);
		currentbefwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		currentnewwm = currentbefwm * ChaMatrixInv(oldparentwm) * newparentwm;
	}


	if (srcbone->GetChild(false)) {
		bool setbroflag2 = true;
		UpdateParentWMReq(srcbone->GetChild(false), limitdegflag, 
			setbroflag2, srcmotid, roundingframe,
			currentbefwm, currentnewwm);
	}
	if (srcbone->GetBrother(false) && (setbroflag == true)) {
		UpdateParentWMReq(srcbone->GetBrother(false), limitdegflag, 
			setbroflag, srcmotid, roundingframe,
			oldparentwm, newparentwm);
	}

}

int ChaCalcFunc::Motion2Bt(CModel* srcmodel, bool limitdegflag, double nextframe, 
	ChaMatrix* pmView, ChaMatrix* pmProj)//, int updateslot)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}
	if (!pmView || !pmProj) {
		_ASSERT(0);
		return 1;
	}


	//2024/04/06
	// スレッドから呼び出されたCModel::Motion2Bt()からCModel::UpdateMatrixを呼び出していた
	// CModel::UpdateMatrixのChkInViewを複数スレッドから同時に呼び出すことは出来ない
	// CModel::UpdateMatrixのChkInViewをマルチスレッド呼び出しすると即時実行コンピュートシェーダ関連のメモリエラーで落ちる
	//(CBone::UpdateMatrixはコンテクスト限定でマルチスレッド可能)
	//CModel::Motion2Bt()からのCModel::UpdateMatrix呼び出しをやめて、ChaSceneの呼び出し元でCModel::UpdateMatrixを呼び出して済ませることに
	//
	//ChaMatrix mW = srcmodel->GetWorldMat();
	//srcmodel->UpdateMatrix(limitdegflag, &mW, pmView, pmProj, true, 0);//, updateslot);


	if (!srcmodel->GetTopBt()) {
		return 0;
	}
	if (!srcmodel->GetBtWorld()) {
		return 0;
	}


	srcmodel->CalcRigidElemParamsOnBt();

	srcmodel->Motion2BtReq(srcmodel->GetTopBt());


	//if (g_previewFlag == 5){
	//	if (m_topbt){
	//		SetBtEquilibriumPointReq(m_topbt);
	//	}
	//}

	return 0;
}


void ChaCalcFunc::RetargetReq(CModel* srcmodel, CModel* srcbvhmodel, CBone* modelbone,
	double srcframe, CBone* befbvhbone, float hrate, std::map<CBone*, CBone*>& sconvbonemap)
{
	if (!srcmodel || !srcbvhmodel) {
		return;
	}

	if (!modelbone) {
		_ASSERT(0);
		return;
	}

	CBone* bvhbone = sconvbonemap[modelbone];
	if (bvhbone) {
		ConvBoneRotation(srcmodel, srcbvhmodel, 1, modelbone, bvhbone, srcframe, befbvhbone, hrate);
	}

	//2023/03/27 コメントアウト : 対応bvhboneが無い場合は　InitMPの姿勢のままにする
	//else {
	//	ConvBoneRotation(srcmodel, srcbvhmodel, 0, modelbone, 0, srcframe, befbvhbone, hrate);
	//}


	if (modelbone->GetChild(true)) {
		if (bvhbone) {
			RetargetReq(srcmodel, srcbvhmodel, modelbone->GetChild(true), srcframe, bvhbone, hrate, sconvbonemap);
		}
		else {
			RetargetReq(srcmodel, srcbvhmodel, modelbone->GetChild(true), srcframe, befbvhbone, hrate, sconvbonemap);
		}
	}
	if (modelbone->GetBrother(true)) {
		//if (bvhbone){
		//	ConvBoneConvertReq(modelbone->GetBrother(true), srcframe, bvhbone, hrate);
		//}
		//else{
		RetargetReq(srcmodel, srcbvhmodel, modelbone->GetBrother(true), srcframe, befbvhbone, hrate, sconvbonemap);
		//}
	}

}

int ChaCalcFunc::ConvBoneRotation(CModel* srcmodel, CModel* srcbvhmodel, int selfflag,
	CBone* srcbone, CBone* bvhbone, double srcframe, CBone* befbvhbone, float hrate)
{

	//retargetは　unlimitedに対して行い　unlimitedにセットする
	bool limitdegflag = false;
	int wallscrapingikflag = 0;//リターゲットの際には関係ない

	//2023/03/27 : 対応bvhboneが無い場合には　InitMPの姿勢のままにする
	if (!bvhbone) {
		return 0;
	}



	if (selfflag && !bvhbone) {
		_ASSERT(0);
		return 1;
	}
	if ((selfflag == 0) && !befbvhbone) {
		_ASSERT(0);
		return 1;
	}

	if (!srcmodel || !srcbvhmodel || !srcbone) {
		_ASSERT(0);
		return 1;
	}


	//###################################################################
	//2023/02/02
	//GetCurMp().GetWorldMatには　例外的にモデルのworldmatが掛かっている
	//アニメ姿勢の計算には　GetCurMp().GetAnimMat()を使用
	//###################################################################


	double roundingframe = RoundingTime(srcframe);

	int bvhmotid;
	if (!srcbvhmodel->ExistCurrentMotion()) {
		_ASSERT(0);
		return 1;
	}
	bvhmotid = srcbvhmodel->GetCurrentMotID();//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	bool onaddmotion = true;//for getbychain
	CMotionPoint* bvhmp = 0;
	if (bvhbone) {
		//bvhmp = bvhbone->GetCurMp();
		bvhmp = bvhbone->GetMotionPoint(bvhmotid, roundingframe);
		if (!bvhmp) {
			_ASSERT(0);
			return 0;
		}
	}
	else {
		//bvhmp = befbvhbone->GetCurMp();
		return 0;
	}


	if (srcmodel->ExistCurrentMotion()) {
		int modelmotid = srcmodel->GetCurrentMotID();
		CMotionPoint modelmp;
		CMotionPoint* pmodelmp = 0;
		pmodelmp = srcbone->GetMotionPoint(modelmotid, roundingframe, onaddmotion);
		if (pmodelmp) {
			modelmp = *pmodelmp;
		}
		else {
			_ASSERT(0);
			return 1;
		}

		CMotionPoint modelparmp;
		CMotionPoint* pmodelparmp = 0;
		if (srcbone->GetParent(true)) {
			pmodelparmp = srcbone->GetParent(true)->GetMotionPoint(modelmotid, roundingframe, onaddmotion);
			if (pmodelparmp) {
				modelparmp = *pmodelparmp;
			}
		}


		int curboneno = srcbone->GetBoneNo();


		CQuaternion rotq;
		ChaVector3 traanim;

		if (bvhbone) {
			//ChaMatrix curbvhmat;
			//ChaMatrix bvhmat;
			//bvhmat = bvhmp.GetWorldMat();

			//ChaMatrix modelinit, invmodelinit;
			//modelinit = modelmp.GetWorldMat();
			//invmodelinit = modelmp.GetInvWorldMat();

			CBone* modelfirstbone = 0;
			CBone* modeltopbone = srcmodel->GetTopBone();
			CBone* modelhipsbone = 0;
			if (modeltopbone) {
				srcmodel->GetHipsBoneReq(modeltopbone, &modelhipsbone);
				if (modelhipsbone) {
					modelfirstbone = modelhipsbone;
				}
				else {
					modelfirstbone = modeltopbone;
				}
			}

			CBone* bvhfirstbone = 0;
			CBone* bvhtopbone = srcbvhmodel->GetTopBone();
			CBone* bvhhipsbone = 0;
			if (bvhtopbone) {
				srcbvhmodel->GetHipsBoneReq(bvhtopbone, &bvhhipsbone);
				if (bvhhipsbone) {
					bvhfirstbone = bvhhipsbone;
				}
				else {
					bvhfirstbone = bvhtopbone;
				}
			}


			//if (srcbone == srcmodel->GetTopBone()) {//モデル側の最初のボーンの処理時
			//if (modelfirstbone && (srcbone == modelfirstbone)) {//モデル側の最初のボーンの処理時

			//	//firsthipbvhmatとfirsthipmodelmatは　この関数の参照引数　一度セットして使いまわす
			//	
			//	//#######################################################################################
			//	//2022/12/21 ver1.1.0.10へ向けて
			//	//式10033と前提条件を合わせる
			//	//bvh側の0フレーム姿勢がIdentityになるように　InvFirstMat * NodeMat を掛ける
			//	//#######################################################################################
			//	firsthipbvhmat = ChaMatrixInv(bvhbone->GetFirstMat()) * bvhbone->GetNodeMat() * bvhmp.GetAnimMat();
			//	firsthipbvhmat.data[MATI_41] = 0.0f;
			//	firsthipbvhmat.data[MATI_42] = 0.0f;
			//	firsthipbvhmat.data[MATI_43] = 0.0f;

			//	firsthipmodelmat = modelmp.GetWorldMat();
			//	firsthipmodelmat.data[MATI_41] = 0.0f;
			//	firsthipmodelmat.data[MATI_42] = 0.0f;
			//	firsthipmodelmat.data[MATI_43] = 0.0f;
			//}


			//#########################################################################################
			//2023/03/26 ver1.2.0.18へ向けて
			//bvh側model側　両方とも０フレームにアニメが在っても　リターゲットがうまくいくように　修正
			//#########################################################################################
			ChaMatrix bvhparentmat, modelparentmat;
			bvhparentmat.SetIdentity();
			modelparentmat.SetIdentity();
			if (bvhbone->GetParent(true)) {
				bvhparentmat = ChaMatrixInv(bvhbone->GetParent(true)->GetWorldMat(false, bvhmotid, 0.0, 0)) * bvhbone->GetParent(true)->GetWorldMat(false, bvhmotid, roundingframe, 0);
			}
			else {
				//bvhparentmat.SetIdentity();
				bvhparentmat = ChaMatrixInv(bvhbone->GetWorldMat(false, bvhmotid, 0.0, 0)) * bvhbone->GetWorldMat(false, bvhmotid, roundingframe, 0);
			}
			if (srcbone->GetParent(true)) {
				modelparentmat = ChaMatrixInv(srcbone->GetParent(true)->GetWorldMat(false, modelmotid, 0.0, 0)) * srcbone->GetParent(true)->GetWorldMat(false, modelmotid, roundingframe, 0);
			}
			else {
				//modelparentmat.SetIdentity();
				modelparentmat = ChaMatrixInv(srcbone->GetWorldMat(false, modelmotid, 0.0, 0)) * srcbone->GetWorldMat(false, modelmotid, roundingframe, 0);
			}


			//curbvhmat = bvhbone->GetInvFirstMat() * invmodelinit * bvhmat;
			//curbvhmat = bvhbone->GetInvFirstMat() * sinvfirsthipmat * invmodelinit * bvhmat;
			//curbvhmat = sinvfirsthipmat * bvhbone->GetInvFirstMat() * sfirsthipmat * invmodelinit * bvhmat;//1.0.0.26になる前までの式。初期姿勢の変換にbvhの全体回転sfirsthipmatを考慮する。

			//#############################################################################################################################
			//1.0.0.26からは
			//bvhは読み込み時に０フレームアニメがIdentityになるように読み込む。model側はInvJonitPos * AnimMatのように読み込むようにした。
			//model側は０フレーム編集に対応した。
			//以上の変更に対応するためにretargetの数式も修正。
			//#############################################################################################################################

			//###################################################################################################################
			//1.0.0.27からは０フレームアニメの編集に対応。
			//０フレームに対応可能なのは非bvhのモデル。非bvhの場合、０フレームアニメがIdentityになるようには読まない。
			//非bvhの場合にはBindPoseと0フレームアニメの両方が存在する。よって０フレームアニメの編集をして書き出しても正常。
			//一方、bvhの場合、０フレームアニメがIdentityになるように読み込む。そのためリターゲットの数式が簡略化される。
			//###################################################################################################################
			//curbvhmat = sinvfirsthipmat * srcbone->GetFirstMat() * sfirsthipmat * invmodelinit * bvhmat;//式10027_1 うまく行く

			////####################################################################################
			////式10027_1の行列掛け算部分をクォータニオンにしてジンバルロックが起こりにくくしてみる
			////####################################################################################


			//FirstMatについて
			//SetFirstMatは　CBone::InitMP　で行う。InitMPはCModel::AddMotionから呼ばれる。
			//InitMPは最初のモーションの０フレームアニメで新規モーションの全フレームを初期化する。


			////##############################################################################################################################
			////式10032(1033も)  bvh側の０フレーム対応とmodel側の０フレーム対応を修正して　合体！！
			//// 前提１：リターゲット条件は bvh側とmodel側の見かけ上のポーズが同じであること
			//// 前提２：bvh側は０フレーム姿勢がidentity(０フレームにアニメが付いる場合はジョイント位置に落とし込み姿勢はidentity). 
			//// 前提３：model側は０フレームにアニメ成分を残している
			//// 前提２と前提３については　fbxの読み込み方をそのようにしてある(bvh側にはバインドポーズが無いことが多いからこのようにしてある)
			////##############################################################################################################################

			if (modelfirstbone && bvhfirstbone) {

				//#######################################################################################
				//2022/12/21 ver1.1.0.10へ向けて
				//式10033と前提条件を合わせる
				//bvh側の0フレーム姿勢がIdentityになるように　InvFirstMat * NodeMat を掛ける
				//#######################################################################################
				ChaMatrix offsetforbvhmat, offsetformodelmat;
				//offsetforbvhmat = ChaMatrixInv(bvhbone->GetFirstMat()) * bvhbone->GetNodeMat();
				//offsetformodelmat.SetIdentity();

				//#########################################################################################
				//2023/03/26 ver1.2.0.18へ向けて
				//bvh側model側　両方とも０フレームにアニメが在っても　リターゲットがうまくいくように　修正
				//#########################################################################################
				offsetforbvhmat = ChaMatrixInv(bvhbone->GetWorldMat(false, bvhmotid, 0.0, 0));
				offsetformodelmat = ChaMatrixInv(srcbone->GetWorldMat(false, modelmotid, 0.0, 0));


				//######
				//model
				//######
					//model parent
				CQuaternion modelparentQ, invmodelparentQ;
				modelparentQ.RotationMatrix(modelparentmat);
				invmodelparentQ.RotationMatrix(ChaMatrixInv(modelparentmat));

				//model current
				ChaMatrix invmodelcurrentmat;
				CQuaternion invmodelQ;
				invmodelcurrentmat = ChaMatrixInv(offsetformodelmat * modelmp.GetWorldMat());
				invmodelQ.RotationMatrix(invmodelcurrentmat);

				//model zeroframe anim
				ChaMatrix zeroframemodelmat;
				CQuaternion zeroframemodelQ;
				zeroframemodelmat = offsetformodelmat * srcbone->GetCurrentZeroFrameMat(limitdegflag, 1);
				zeroframemodelQ.RotationMatrix(zeroframemodelmat);


				//######
				//bvh
				//######
					//bvh parent
				CQuaternion bvhparentQ, invbvhparentQ;
				bvhparentQ.RotationMatrix(bvhparentmat);
				invbvhparentQ.RotationMatrix(ChaMatrixInv(bvhparentmat));

				//bvh current
				ChaMatrix bvhcurrentmat;
				CQuaternion bvhQ;
				//bvhcurrentmat = offsetforbvhmat * bvhmp.GetAnimMat();
				bvhcurrentmat = offsetforbvhmat * bvhmp->GetWorldMat();
				bvhQ.RotationMatrix(bvhcurrentmat);


				////bvh zeroframe anim
				ChaMatrix zeroframebvhmat;
				CQuaternion invzeroframebvhQ;
				zeroframebvhmat = offsetforbvhmat * bvhbone->GetCurrentZeroFrameMat(limitdegflag, 1);
				invzeroframebvhQ.RotationMatrix(ChaMatrixInv(zeroframebvhmat));


				//10033準備の式
					//ChaMatrix curbvhmat;
					//curbvhmat =
					//	(ChaMatrixInv(firsthipbvhmat) * ChaMatrixInv(bvhbone->GetCurrentZeroFrameMat(1)) * firsthipbvhmat) *
					//	(ChaMatrixInv(firsthipmodelmat) * (ChaMatrixInv(modelmp.GetWorldMat()) * zeroframemodelmat) * firsthipmodelmat) *
					//	bvhmp.GetWorldMat();//2022/10/30 テスト(bvh120, bvh121, Rokoko)済　OK
					//
					//補足：invhips * (inv)zeroframemat * hipsは　model座標系というかhips座標系のzeroframe姿勢の計算
					// 
					// 


				//###############################################################################################
				//2023/03/27 修正：　firsthipbvhmatはbvhparentmatに　firsthipmodelmatはmodelparentmatに置き換え
				//hips座標系ではなく　parent座標系で計算
				//###############################################################################################

				//式10033 以下６行
				ChaMatrix curbvhmat;
				CQuaternion convQ;
				convQ = bvhQ *
					(invmodelparentQ * (zeroframemodelQ * invmodelQ) * modelparentQ) *
					(invbvhparentQ * invzeroframebvhQ * bvhparentQ);
				curbvhmat = convQ.MakeRotMatX();
				//式10033
				//2022/10/30テストの式をクォータニオン(及びクォータニオンの掛け算の順番)にして　ジンバルロックが起こり難いように


				rotq.RotationMatrix(curbvhmat);//回転だけ採用する

				//2023/03/26　補足
				//FKRotate-->RotBoneQReqに回転を渡して　既存の姿勢にrotqを掛けることになる
				//リターゲット結果の側(model側)のモーションは
				//Identityではなく　最初のモーションの０フレームの姿勢で初期化しておく
				//bvh側とmodel側の０フレームの見かけ上の姿勢が同じであることが　リターゲット条件
				//０フレーム姿勢からの変化分を利用して　軸の違いなどを吸収して計算する




				//traanim = bvhbone->CalcLocalTraAnim(bvhmotid, roundingframe);//移動はこちらから取得
				//if (!bvhbone->GetParent(true)) {
				//	ChaVector3 bvhbonepos = bvhbone->GetJointFPos();
				//	ChaVector3 firstframebonepos = bvhbone->GetFirstFrameBonePos();
				//	ChaVector3 firstdiff = firstframebonepos - bvhbonepos;
				//	traanim -= firstdiff;
				//}
				//traanim = traanim * hrate;


				//################################################################################
				//2023/01/08
				//Hipsジョイント以外のTraAnimも有効に
				// 
				//リターゲット条件は　modelとbvhの０フレームの見かけ上の姿勢が同じことであるから
				//リターゲット時にTraAnimとして計算すべきは　０フレームからの変化分である
				//################################################################################



				////for debug
				//{
				//	MOTINFO chkmi = srcbvhmodel->GetCurMotInfo();
				//	if ((bvhbone->IsHipsBone()) && ((roundingframe == 0.0) || (roundingframe == 1.0)) &&
				//		!chkmi.cameramotion && (chkmi.frameleng >= 900.0)) {
				//		int dbgflag1 = 1;
				//	}
				//}



				ChaMatrix bvhsmat, bvhrmat, bvhtmat, bvhtanimmat;
				ChaMatrix bvhsmat0, bvhrmat0, bvhtmat0, bvhtanimmat0;

				//GetWorldMat() : limitedflagをゼロにしておく必要有 !!!!
				if (bvhbone->GetParent(true)) {
					ChaMatrix parentwm = bvhbone->GetParent(true)->GetWorldMat(limitdegflag, bvhmotid, roundingframe, 0);
					//GetSRTandTraAnim(bvhmp.GetAnimMat() * ChaMatrixInv(parentwm), bvhbone->GetNodeMat(),
					//	&bvhsmat, &bvhrmat, &bvhtmat, &bvhtanimmat);
					GetSRTandTraAnim(bvhmp->GetWorldMat() * ChaMatrixInv(parentwm), bvhbone->GetNodeMat(),
						&bvhsmat, &bvhrmat, &bvhtmat, &bvhtanimmat);

					//calc 0 frame
					ChaMatrix parentwm0 = bvhbone->GetParent(true)->GetWorldMat(limitdegflag, bvhmotid, 0.0, 0);
					GetSRTandTraAnim(bvhbone->GetWorldMat(limitdegflag, bvhmotid, 0.0, 0) * ChaMatrixInv(parentwm0), bvhbone->GetNodeMat(),
						&bvhsmat0, &bvhrmat0, &bvhtmat0, &bvhtanimmat0);
				}
				else {
					//GetSRTandTraAnim(bvhmp.GetAnimMat(), bvhbone->GetNodeMat(),
					//	&bvhsmat, &bvhrmat, &bvhtmat, &bvhtanimmat);
					GetSRTandTraAnim(bvhmp->GetWorldMat(), bvhbone->GetNodeMat(),
						&bvhsmat, &bvhrmat, &bvhtmat, &bvhtanimmat);

					//calc 0 frame
					GetSRTandTraAnim(bvhbone->GetWorldMat(limitdegflag, bvhmotid, 0.0, 0), bvhbone->GetNodeMat(),
						&bvhsmat0, &bvhrmat0, &bvhtmat0, &bvhtanimmat0);
				}

				traanim = ChaMatrixTraVec(bvhtanimmat) - ChaMatrixTraVec(bvhtanimmat0);//2023/01/08
				traanim = traanim * hrate;
				int dbgflag1 = 1;
			}
			else {
				rotq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
				traanim.SetParams(0.0f, 0.0f, 0.0f);
			}
		}
		else {
			//rotq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
			//traanim.SetParams(0.0f, 0.0f, 0.0f);

			return 0;
		}

		bool onretarget = true;
		if (bvhbone) {
			int reqflag = 1;//!!!!!!!!! 編集結果を再帰的に子供に伝えるので　bvhboneが無い場合には処理をしないで良い
			int traanimflag = 1;
			srcmodel->FKRotate(limitdegflag, wallscrapingikflag, onretarget, reqflag, bvhbone,
				traanimflag, traanim, roundingframe, curboneno, rotq);
		}
		else {
			//srcmodel->FKRotate(limitdegflag, onretarget, 0, befbvhbone, 0, traanim, roundingframe, curboneno, rotq);
			return 0;
		}
	}

	return 0;
}

CBone* ChaCalcFunc::GetTopBone(CModel* srcmodel, bool excludenullflag)//default : excludenullflag = true
{
	if (!srcmodel) {
		return 0;
	}

	CBone* ptopbone = 0;
	GetTopBoneReq(srcmodel, srcmodel->DirectGetTopBone(), &ptopbone, excludenullflag);
	return ptopbone;
}
void ChaCalcFunc::GetTopBoneReq(CModel* srcmodel, CBone* srcbone, CBone** pptopbone, bool excludenullflag)
{
	if (srcmodel && srcbone && pptopbone && !(*pptopbone)) {

		if (excludenullflag == true) {
			//if ((srcbone->IsSkeleton()) || (srcbone->GetType() == FBXBONE_ROOTNODE)) {
			if (srcbone->IsSkeleton()) {//FBXBONE_ROOTNODEはここでは除外 　FBXBONE_ROOTNODEはGetRootNode()で取得するように
				*pptopbone = srcbone;
				return;
			}
		}
		else {
			*pptopbone = srcbone;
			return;
		}

		if (!(*pptopbone)) {
			if (srcbone->GetBrother(false)) {
				GetTopBoneReq(srcmodel, srcbone->GetBrother(false), pptopbone, excludenullflag);
			}
			if (srcbone->GetChild(false))
			{
				GetTopBoneReq(srcmodel, srcbone->GetChild(false), pptopbone, excludenullflag);
			}
		}
	}
}



void ChaCalcFunc::GetHipsBoneReq(CModel* srcmodel, CBone* srcbone, CBone** dstppbone)
{
	if (!srcmodel) {
		return;
	}

	if (srcmodel->GetNoBoneFlag() == true) {
		*dstppbone = 0;
		return;
	}


	if (srcbone && dstppbone && !(*dstppbone)) {

		if (srcbone->IsHipsBone()) {
			*dstppbone = srcbone;
			return;
		}

		if (!(*dstppbone)) {
			if (srcbone->GetBrother(false)) {
				GetHipsBoneReq(srcmodel, srcbone->GetBrother(false), dstppbone);
			}
			if (srcbone->GetChild(false)) {
				GetHipsBoneReq(srcmodel, srcbone->GetChild(false), dstppbone);
			}
		}
	}
}

int ChaCalcFunc::FKRotate(CModel* srcmodel, bool limitdegflag, int wallscrapingikflag, 
	bool onretarget, int reqflag,
	CBone* bvhbone, int traflag, ChaVector3 traanim, double srcframe, int srcboneno,
	CQuaternion rotq)
{

	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}

	if (srcboneno < 0) {
		_ASSERT(0);
		return 1;
	}

	//CBone* curbone = m_bonelist[srcboneno];
	CBone* curbone = srcmodel->GetBoneByID(srcboneno);
	if (!curbone) {
		_ASSERT(0);
		return 1;
	}
	if (curbone->IsNotSkeleton()) {
		return 1;
	}

	if (!srcmodel->ExistCurrentMotion()) {
		_ASSERT(0);
		return 1;
	}

	int curmotid = srcmodel->GetCurrentMotID();
	double roundingframe = RoundingTime(srcframe);


	//for debug
	{
		MOTINFO chkmi = srcmodel->GetCurMotInfo();
		if ((curbone->IsHipsBone()) && ((roundingframe == 0.0) || (roundingframe == 1.0)) &&
			!chkmi.cameramotion && (chkmi.frameleng >= 900.0)) {
			int dbgflag1 = 1;
		}
	}




	bool onaddmotion = true;//for getbychain
	CBone* parentbone = curbone->GetParent(false);
	CMotionPoint* parmp = 0;
	if (parentbone && parentbone->IsSkeleton()) {
		parmp = parentbone->GetMotionPoint(curmotid, roundingframe, onaddmotion);
	}

	if (reqflag == 1) {
		ChaMatrix dummyparentwm;
		dummyparentwm.SetIdentity();
		bool infooutflag = true;
		curbone->RotBoneQReq(limitdegflag, wallscrapingikflag, 
			infooutflag, 0, curmotid, roundingframe, rotq, dummyparentwm, dummyparentwm,
			bvhbone, traanim);// , setmatflag, psetmat, onretarget);
	}
	else if (bvhbone) {
		ChaMatrix setmat = bvhbone->GetTmpMat();
		curbone->RotBoneQOne(limitdegflag, wallscrapingikflag, 
			parentbone, parmp, curmotid, roundingframe, setmat);
	}

	return curbone->GetBoneNo();
}

CMotionPoint* ChaCalcFunc::RotBoneQReq(CBone* srcbone, bool limitdegflag, int wallscrapingikflag, bool infooutflag,
	CBone* parentbone, int srcmotid, double srcframe,
	CQuaternion rotq, ChaMatrix srcbefparentwm, ChaMatrix srcnewparentwm,
	CBone* bvhbone, ChaVector3 traanim)// , int setmatflag, ChaMatrix* psetmat, bool onretarget)
{
	if (!srcbone) {
		_ASSERT(0);
		return 0;
	}


	//##############################################################
	//Retarget専用. IK用にはRotAndTraBoneQReq()を使用
	//##############################################################

	double roundingframe = RoundingTime(srcframe);

	//2023/04/28
	if (srcbone->IsNotSkeleton()) {
		return 0;
	}


	CMotionPoint* curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		_ASSERT(0);
		return 0;
	}

	ChaMatrix currentbefwm;
	ChaMatrix currentnewwm;
	currentbefwm.SetIdentity();
	currentnewwm.SetIdentity();
	currentbefwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);

	//初回呼び出し

	ChaMatrix newlocalrotmat;
	ChaMatrix smat, rmat, tmat, tanimmat;
	newlocalrotmat.SetIdentity();
	smat.SetIdentity();
	rmat.SetIdentity();
	tmat.SetIdentity();
	tanimmat.SetIdentity();
	newlocalrotmat = srcbone->CalcNewLocalRotMatFromQofIK(limitdegflag, srcmotid, roundingframe, rotq, &smat, &rmat, &tanimmat);

	//ChaMatrix newtanimmatrotated;
	//newtanimmatrotated.SetIdentity();
	//newtanimmatrotated = CalcNewLocalTAnimMatFromQofIK(srcmotid, roundingframe, newlocalrotmat, smat, rmat, tanimmat, parentwm);

	ChaMatrix bvhtraanim;
	bvhtraanim.SetIdentity();
	bvhtraanim.SetTranslation(ChaMatrixTraVec(tanimmat) + traanim);//元のtanim + 引数traanim

	//#### SRTAnimからローカル行列組み立て ####
	ChaMatrix newlocalmat;
	//newlocalmat = ChaMatrixFromSRTraAnim(true, true, GetNodeMat(), &smat, &newlocalrotmat, &newtanimmatrotated);
	newlocalmat = ChaMatrixFromSRTraAnim(true, true, srcbone->GetNodeMat(), &smat, &newlocalrotmat, &bvhtraanim);
	ChaMatrix newwm;
	if (srcbone->GetParent(false)) {
		ChaMatrix parentwm;
		parentwm = srcbone->GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		newwm = newlocalmat * parentwm;//globalにする
	}
	else {
		newwm = newlocalmat;
	}


	bool directsetflag = false;
	int setchildflag = 0;
	int onlycheck = 0;
	bool fromiktarget = false;
	srcbone->SetWorldMat(limitdegflag, wallscrapingikflag, directsetflag, infooutflag, setchildflag,
		srcmotid, roundingframe, newwm, onlycheck, fromiktarget);

	if (bvhbone) {
		//bvhbone->SetTmpMat(tmpmat);
		bvhbone->SetTmpMat(newwm);
	}

	currentnewwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);


	curmp->SetAbsMat(srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, curmp));


	if (srcbone->GetChild(false) && curmp) {
		bool setbroflag2 = true;
		srcbone->GetChild(false)->UpdateParentWMReq(limitdegflag, setbroflag2, srcmotid, roundingframe,
			currentbefwm, currentnewwm);
	}
	//if (GetBrother() && parentbone){
	//	bool setbroflag3 = true;
	//	GetBrother()->UpdateParentWMReq(limitdegflag, setbroflag3, srcmotid, roundingframe,
	//		srcbefparentwm, srcnewparentwm);
	//}
	return curmp;
}

ChaMatrix ChaCalcFunc::CalcNewLocalRotMatFromQofIK(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe, 
	CQuaternion qForRot, ChaMatrix* dstsmat, ChaMatrix* dstrmat, ChaMatrix* dsttanimmat)
{

	double roundingframe = RoundingTime(srcframe);

	ChaMatrix newlocalrotmat;
	newlocalrotmat.SetIdentity();
	if (!srcbone || !dstsmat || !dstrmat || !dsttanimmat) {
		_ASSERT(0);
		return newlocalrotmat;
	}

	//2023/04/28
	if (srcbone->IsNotSkeleton() && !srcbone->IsNullAndChildIsCamera()) {
		if (dstsmat) {
			dstsmat->SetIdentity();
		}
		if (dstrmat) {
			dstrmat->SetIdentity();
		}
		if (dsttanimmat) {
			dsttanimmat->SetIdentity();
		}

		return newlocalrotmat;//!!!!!!!!!!!! return !!!!!!!!!!!!!!!!
	}


	ChaMatrix currentwm;
	//limitedworldmat = GetLimitedWorldMat(srcmotid, roundingframe);//ここをGetLimitedWorldMatにすると１回目のIKが乱れる。２回目のIK以降はOK。
	currentwm = srcbone->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
	ChaMatrix localmat;
	ChaMatrix parentwm;
	localmat.SetIdentity();
	parentwm.SetIdentity();
	CQuaternion parentq;
	CQuaternion invparentq;
	if (srcbone->GetParent(false)) {
		parentwm = srcbone->GetParent(false)->GetWorldMat(limitdegflag, srcmotid, roundingframe, 0);
		parentq.RotationMatrix(parentwm);
		invparentq.RotationMatrix(ChaMatrixInv(parentwm));
		localmat = currentwm * ChaMatrixInv(parentwm);
	}
	else {
		parentwm.SetIdentity();
		parentq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		invparentq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
		localmat = currentwm;
	}

	ChaMatrix smat, rmat, tmat, tanimmat;
	//ChaMatrix zeroposmat;
	//zeroposmat.SetIdentity();
	GetSRTandTraAnim(localmat, srcbone->GetNodeMat(), &smat, &rmat, &tmat, &tanimmat);//#### ローカル行列をSRTTAnim分解 ####
	//GetSRTandTraAnim(localmat, zeroposmat, &smat, &rmat, &tmat, &tanimmat);//#### ローカル行列をSRTTAnim分解 ####


	CQuaternion curq, newq;
	curq.RotationMatrix(rmat);

	CQuaternion globalq;
	globalq = parentq * curq;
	CQuaternion newglobalq;
	newglobalq = qForRot * globalq;//########### 引数rotqはグローバル姿勢 ############
	//newglobalq = globalq * rotq;
	CQuaternion newlocalrotq;
	newlocalrotq = invparentq * newglobalq;
	newlocalrotmat = newlocalrotq.MakeRotMatX();


	if (dstsmat) {
		*dstsmat = smat;
	}
	if (dstrmat) {
		*dstrmat = rmat;
	}
	if (dsttanimmat) {
		*dsttanimmat = tanimmat;
	}

	return newlocalrotmat;
}

ChaMatrix ChaCalcFunc::GetCurrentZeroFrameMat(CBone* srcbone, bool limitdegflag, int updateflag)
{
	if (!srcbone) {
		_ASSERT(0);
		ChaMatrix inimat;
		ChaMatrixIdentity(&inimat);
		return inimat;
	}

	//ZeroFrameの編集前と編集後のポーズのdiffをとる必要がある場合に対応する
	//updateflagが1の場合に最新情報。0の場合に前回の取得情報と同じものを返す。

	//2023/04/28
	if (srcbone->IsNotSkeleton()) {
		ChaMatrix inimat;
		ChaMatrixIdentity(&inimat);
		return inimat;
	}


	//取得時に計算
	
	//m_firstgetflag = 1;
	ChaMatrix firstgetmatrix = srcbone->GetWorldMat(limitdegflag, srcbone->GetCurMotID(), 0.0, 0);
	srcbone->SetFirstGetMatrix(firstgetmatrix);
	srcbone->SetInvFirstGetMatrix(ChaMatrixInv(firstgetmatrix));
	
	return firstgetmatrix;


	//int inverseflag = 0;
	//return GetCurrentZeroFrameMatFunc(limitdegflag, updateflag, inverseflag);

}



int ChaCalcFunc::InitMP(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe)
{
	if (!srcbone) {
		_ASSERT(0);
		return 1;
	}

	//###########################################################
	//InitMP 初期姿勢。リターゲットの初期姿勢に関わる。 
	//最初のモーション(firstmotid)の worldmat(firstanim)で初期化
	//###########################################################

	if (!srcbone->GetParModel()) {
		return 0;
	}
	//2023/04/28
	//if (srcbone->IsNotSkeleton()) {
	//if (srcbone->IsNotSkeleton() && !srcbone->IsNullAndChildIsCamera() && srcbone->IsNotCamera()) {
	//	return 0;
	//}
	if (!srcbone->IsConcerned(srcmotid)) {
		return 0;
	}


	double roundingframe = RoundingTime(srcframe);

	//この関数は処理に時間が掛かる
	//CModel読み込み中で　読み込み中のモーション数が０以外の場合には　InitMPする必要は無い(モーションの値で上書きする)ので　リターンする
	//
	//2022/11/08
	//ただし　RootまたはReferenceが含まれる名前のボーンは　読み込み時に追加することがあるので　RootとReferenceについてはここではリターンしない
	if ((strstr(srcbone->GetBoneName(), "Root") == 0) && (strstr(srcbone->GetBoneName(), "Reference") == 0) &&
		(srcbone->GetParModel()->GetLoadedFlag() == false) && (srcbone->GetParModel()->GetLoadingMotionCount() > 0)) {//2022/10/20
		return 0;
	}

	////firstmpが無い場合のダミーの初期化モーションポイント
	////初期化されたworldmatがあれば良い
	CMotionPoint initmp;
	initmp.InitParams();


	////１つ目のモーションを削除する場合もあるので　motid = 1決め打ちは出来ない　2022/09/13
	////CMotionPoint* firstmp = GetMotionPoint(1, 0.0);//motid == 1は１つ目のモーション

	int firstmotid = 1;
	//bool cameraanimflag = srcbone->GetParModel()->IsCameraMotion(srcmotid);
	//MOTINFO firstmi = srcbone->GetParModel()->GetFirstValidMotInfo(cameraanimflag);//１つ目のモーションを削除済の場合に対応

	//#####################################################################################################
	//2024/06/09 1.0.0.23 RC3
	//srcmotidがカメラアニメかどうかでfirstmiを決めると　カメラアニメの長さを変更した際にカメラアニメのモデルが消えてしまう
	//よってボーンごとにfirstmiを決定する
	//#####################################################################################################
	MOTINFO firstmi;
	if (srcbone->IsSkeleton()) {
		//skeletonの場合　通常モーションを先に調べて　無ければカメラモーションを調べる
		bool cameraanimflag = false;
		firstmi = srcbone->GetParModel()->GetFirstValidMotInfo(cameraanimflag);
		if (firstmi.motid <= 0) {
			cameraanimflag = true;
			firstmi = srcbone->GetParModel()->GetFirstValidMotInfo(cameraanimflag);
		}

		if (firstmi.motid <= 0) {
			//MotionPointが無い場合においても　想定している使い方として　MOTINFOはAddされた状態でRetargetは呼ばれる
			//よってここを通る場合は　想定外エラー
			_ASSERT(0);
			return 1;
		}
		else {
			firstmotid = firstmi.motid;
		}
	}
	else if (srcbone->IsCamera() || srcbone->IsNullAndChildIsCamera()) {
		//カメラの場合　カメラモーションを先に調べて　無ければ通常モーションを調べる
		bool cameraanimflag = true;
		//firstmi = srcbone->GetParModel()->GetFirstValidMotInfo(cameraanimflag);
		//if (firstmi.motid <= 0) {
		//	cameraanimflag = false;
		//	firstmi = srcbone->GetParModel()->GetFirstValidMotInfo(cameraanimflag);
		//}


		//2024/06/27
		//複数カメラがある場合、対象カメラ以外のカメラのモーションポイントは無いのが普通
		//カメラの場合の初期姿勢はinitmpとする


	}
	else {
		_ASSERT(0);
		bool cameraanimflag = false;
		firstmi = srcbone->GetParModel()->GetFirstValidMotInfo(cameraanimflag);
		if (firstmi.motid <= 0) {
			cameraanimflag = true;
			firstmi = srcbone->GetParModel()->GetFirstValidMotInfo(cameraanimflag);
		}

		if (firstmi.motid <= 0) {
			//MotionPointが無い場合においても　想定している使い方として　MOTINFOはAddされた状態でRetargetは呼ばれる
			//よってここを通る場合は　想定外エラー
			_ASSERT(0);
			return 1;
		}
		else {
			firstmotid = firstmi.motid;
		}
	}


	CMotionPoint* curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
	if (!curmp) {
		//2023/10/27
		//モーションの無いfbxを読み込んだ場合　ここではまだモーションポイントは１つも無い
		//モーション無しfbxの最初のInitMP時にも　後方のfirstmpがNULLにならないためには　firstmpセットよりも前に　モーションポイントを作成する必要
		int existflag = 0;
		curmp = srcbone->AddMotionPoint(srcmotid, roundingframe, &existflag);
		if (!curmp) {//2024/06/10
			int dbgflag1 = 1;
		}
	}
	if (curmp) {

		CMotionPoint* firstmp = 0;
		if ((srcbone->IsCamera() || srcbone->IsNullAndChildIsCamera()) || //2024/06/27 カメラの場合もfirstmpはinitmpとする
			(srcbone->GetParModel()->GetLoadedFlag() == false) && (srcbone->GetParModel()->GetLoadingMotionCount() <= 0)) {
			//Motionが１つも無いfbx読み込みのフォロー
			//読み込み中で　fbxにモーションが無い場合　モーションポイントを作成する　それ以外の場合で　モーションポイントが無い場合はエラー
			firstmp = &initmp;
		}
		else {
			firstmp = srcbone->GetMotionPoint(firstmotid, 0.0);
			if (!firstmp) {
				int dbgflag1 = 1;
			}
		}

		if (!firstmp && ((strstr(srcbone->GetBoneName(), "Root") != 0) || (strstr(srcbone->GetBoneName(), "Reference") != 0))) {
			//2022/11/08
			//RootまたはReferenceが含まれる名前のボーンは　読み込み時に追加することがある
			//RootとReferenceボーンの内　モーションポイントが無い場合についても　ここで対応

			firstmp = &initmp;
		}

		//ChaMatrix matforinit;
		//matforinit.SetIdentity();

		if (firstmp) {

			//2024/06/07 skeletonとcameraとenullchildiscamera以外は上記でリターンしている
			//その３種に関しては　Evaluateする必要は無いので　以下コメントアウト
			
			////###############
			////set matforinit 2023/05/15
			////###############
			////if (srcbone->GetParModel()->GetLoadingMotionCount() <= 1) {
			//if (srcbone->HasMotionCurve(srcmotid) &&//2024/05/24
			//	srcbone->IsNotSkeleton() && 
			//	!srcbone->IsNullAndChildIsCamera() && srcbone->IsNotCamera() &&
			//	(srcbone->GetParModel()->GetLoadingMotionCount() <= 1)) {//2023/10/23 skeleton以外の場合
			//	FbxNode* pNode = srcbone->GetFbxNodeOnLoad();
			//	if (pNode) {

			//		EnterCriticalSection(&g_CritSection_FbxSdk);//!!!!!!!!!
			//		FbxAMatrix lGlobalSRT;
			//		FbxTime time0;
			//		time0.SetSecondDouble(0.0);
			//		lGlobalSRT = pNode->EvaluateGlobalTransform(time0, FbxNode::eSourcePivot, true, true);//current animation
			//		ChaMatrix chaGlobalSRT;
			//		chaGlobalSRT = ChaMatrixFromFbxAMatrix(lGlobalSRT);
			//		matforinit = (ChaMatrixInv(srcbone->GetNodeMat()) * chaGlobalSRT);
			//		//matforinit = chaGlobalSRT;
			//		//matforinit = ChaMatrixInv(GetNodeMat());
			//		LeaveCriticalSection(&g_CritSection_FbxSdk);//!!!!!!!!!
			//	}
			//	else {
			//		_ASSERT(0);
			//		matforinit.SetIdentity();
			//	}

			//	//matforinit = firstmp->GetWorldMat();
			//	////matforinit.SetIdentity();
			//}
			//else {
			//	matforinit = firstmp->GetWorldMat();
			//}

			//###########
			//for debug
			//###########
			//if ((srcmotid == 1) && (srcframe == 0.0)) {
			//	char strdbg[1024] = { 0 };
			//	WCHAR wstrdbg[1024] = { 0L };
			//	sprintf_s(strdbg, 1024, "InitMP firstanim firstframe : (%s)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n\t(%.3f, %.3f, %.3f, %.3f)\r\n",
			//		srcbone->GetBoneName(),
			//		matforinit.data[MATI_11], matforinit.data[MATI_12], matforinit.data[MATI_13], matforinit.data[MATI_14],
			//		matforinit.data[MATI_21], matforinit.data[MATI_22], matforinit.data[MATI_23], matforinit.data[MATI_24],
			//		matforinit.data[MATI_31], matforinit.data[MATI_32], matforinit.data[MATI_33], matforinit.data[MATI_34],
			//		matforinit.data[MATI_41], matforinit.data[MATI_42], matforinit.data[MATI_43], matforinit.data[MATI_44]
			//	);
			//	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, strdbg, 1024, wstrdbg, 1024);
			//	DbgOut(wstrdbg);
			//}

			//SetFirstMat(firstanim);//リターゲット時のbvhbone->GetFirstMatで効果


			////SetWorldMat(srcmotid, roundingframe, firstanim, curmp);
			//curmp->SetWorldMat(firstanim);
			//curmp->SetLimitedWM(firstanim);

			curmp->SetLocalMat(firstmp->GetLocalMat());
			curmp->SetWorldMat(firstmp->GetWorldMat());
			curmp->SetLimitedWM(firstmp->GetWorldMat());
			curmp->SetLocalEul(firstmp->GetLocalEul());
			curmp->SetLimitedLocalEul(firstmp->GetLocalEul());
			curmp->SetCalcLimitedWM(2);

			////SetInitMat(xmat);
			//////オイラー角初期化
			//ChaVector3 cureul.SetParams(0.0f, 0.0f, 0.0f);
			//int paraxsiflag = 1;


			////cureul = CalcLocalEulXYZ(0, paraxsiflag, 1, 0.0, BEFEUL_ZERO);
			////cureul = srcbone->CalcLocalEulXYZ(0, paraxsiflag, srcmotid, roundingframe, BEFEUL_BEFFRAME);
			//cureul = firstmp->GetLocalEul();//2023/10/23


			//////１つ目のモーションを削除する場合もあるので　motid = 1決め打ちは出来ない　2022/09/13
			//////ChaVector3 cureul = GetLocalEul(firstmotid, 0.0, 0);//motid == 1は１つ目のモーション
			//////SetLocalEul(srcmotid, roundingframe, cureul, curmp);
			////ChaVector3 cureul = firstmp->GetLocalEul();

			//curmp->SetLocalEul(cureul);
			//curmp->SetLimitedLocalEul(cureul);
			////if (limitdegflag == true) {
			//curmp->SetCalcLimitedWM(2);
			//}


			//2023/02/11
			//GetFbxAnimのif((animno == 0) && (srcframe == 0.0))を通らなかったRootジョイント用の初期化
			if (srcbone->IsNotCamera() && !srcbone->IsNullAndChildIsCamera() && 
				(srcmotid == firstmotid) && (roundingframe == 0.0)) {
				ChaMatrix firstmat;
				firstmat = srcbone->GetNodeMat() * firstmp->GetWorldMat();
				srcbone->SetFirstMat(firstmat);
			}
		}
		else {
			_ASSERT(0);
		}
	}

	////###################################################################################		
	////InitMP 初期姿勢。２つ目以降のモーションの初期姿勢。リターゲットの初期姿勢に関わる。
	////###################################################################################
	//if (newmp && (srcmotid != 1)) {
	//	ChaMatrix xmat = GetFirstMat();
	//	newmp->SetWorldMat(xmat);
	//	//SetInitMat(xmat);
	//	////オイラー角初期化
	//	ChaVector3 cureul.SetParams(0.0f, 0.0f, 0.0f);
	//	int paraxsiflag = 1;
	//	cureul = CalcLocalEulXYZ(paraxsiflag, srcmotid, roundingframe, BEFEUL_ZERO);
	//	SetLocalEul(srcmotid, roundingframe, cureul);
	//}



	//ChaMatrix parfirstmat, invparfirstmat;
	//ChaMatrixIdentity(&parfirstmat);
	//ChaMatrixIdentity(&invparfirstmat);
	//if (parentbone) {
	//	double zeroframe = 0.0;
	//	int existz = 0;
	//	CMotionPoint* parmp = parentbone->AddMotionPoint(motid, zeroframe, &existz);
	//	if (existz && parmp) {
	//		parfirstmat = parmp->GetWorldMat();//!!!!!!!!!!!!!! この時点ではm_matWorldが掛かっていないから後で修正必要かも？？
	//		ChaMatrixInverse(&invparfirstmat, NULL, &parfirstmat);
	//	}
	//	else {
	//		ChaMatrixIdentity(&parfirstmat);
	//		ChaMatrixIdentity(&invparfirstmat);
	//	}
	//}

	//double framecnt;
	//for (framecnt = 0.0; framecnt < animleng; framecnt += 1.0) {
	//	double frame = framecnt;

	//	ChaMatrix mvmat;
	//	ChaMatrixIdentity(&mvmat);

	//	CMotionPoint* pcurmp = 0;
	//	bool onaddmotion = true;
	//	pcurmp = curbone->GetMotionPoint(motid, frame, onaddmotion);
	//	if (!pcurmp) {
	//		int exist2 = 0;
	//		CMotionPoint* newmp = curbone->AddMotionPoint(motid, frame, &exist2);
	//		if (!newmp) {
	//			_ASSERT(0);
	//			return;
	//		}

	//		if (parentbone) {
	//			int exist3 = 0;
	//			CMotionPoint* parmp = parentbone->AddMotionPoint(motid, frame, &exist3);
	//			ChaMatrix tmpmat = parentbone->GetInvFirstMat() * parmp->GetWorldMat();//!!!!!!!!!!!!!!!!!! endjointはこれでうまく行くが、floatと分岐が不動になる。
	//			//newmp->SetBefWorldMat(tmpmat);
	//			newmp->SetWorldMat(tmpmat);//anglelimit無し

	//			//オイラー角初期化
	//			ChaVector3 cureul.SetParams(0.0f, 0.0f, 0.0f);
	//			int paraxiskind = -1;//2021/11/18
	//			//int isfirstbone = 0;
	//			cureul = curbone->CalcLocalEulXYZ(paraxiskind, motid, (double)framecnt, BEFEUL_ZERO);
	//			curbone->SetLocalEul(motid, (double)framecnt, cureul);

	//		}
	//	}
	//}


	return 0;
}

int ChaCalcFunc::InitMP(CModel* srcmodel, bool limitdegflag, CBone* curbone, int srcmotid, double curframe)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}


	//CMotionPoint* pcurmp = 0;
	//pcurmp = curbone->GetMotionPoint(GetCurMotInfo()->motid, curframe);

	//if (pcurmp) {

	//	//pcurmp->SetBefWorldMat(pcurmp->GetWorldMat());

	//	ChaMatrix xmat = curbone->GetFirstMat();
	//	pcurmp->SetWorldMat(xmat);
	//	curbone->SetInitMat(xmat);

	//}
	//else {
	//	CMotionPoint* curmp3 = 0;
	//	int existflag3 = 0;
	//	curmp3 = curbone->AddMotionPoint(GetCurMotInfo()->motid, curframe, &existflag3);
	//	if (!curmp3) {
	//		_ASSERT(0);
	//		return 1;
	//	}
	//	ChaMatrix xmat = curbone->GetFirstMat();
	//	curmp3->SetWorldMat(xmat);
	//	curbone->SetInitMat(xmat);
	//	//_ASSERT( 0 );
	//}

	////オイラー角初期化
	//ChaVector3 cureul.SetParams(0.0f, 0.0f, 0.0f);
	//int paraxsiflag = 1;
	////int isfirstbone = 0;
	//cureul = curbone->CalcLocalEulXYZ(paraxsiflag, GetCurMotInfo()->motid, curframe, BEFEUL_ZERO);
	//curbone->SetLocalEul(GetCurMotInfo()->motid, curframe, cureul);


	//if ((srcmodel->GetNoBoneFlag() == false) && curbone && 
	//	(curbone->IsSkeleton() || curbone->IsCamera() ||  curbone->IsNullAndChildIsCamera())) {
	if((srcmodel->GetNoBoneFlag() == false) && curbone &&
		curbone->IsConcerned(srcmotid)) {//2024/06/10
		InitMP(curbone, limitdegflag, srcmotid, curframe);
	}


	return 0;
}

void ChaCalcFunc::InitMPReq(CModel* srcmodel, bool limitdegflag, CBone* curbone, int srcmotid, double curframe)
{
	if (!srcmodel) {
		_ASSERT(0);
		return;
	}

	if (!curbone) {
		return;
	}
	if (srcmodel->GetNoBoneFlag() == true) {
		return;
	}


	//bool cameraanimflag = srcmodel->IsCameraMotion(srcmotid);
	//char cmpmotionname[256] = { 0 };
	//int result0 = srcmodel->GetMotionName(srcmotid, 256, cmpmotionname);
	//if (result0 != 0) {
	//	_ASSERT(0);
	//	return;
	//}

	//2024/06/10
	//非カメラアニメ時：skeleton or enull, カメラアニメ時：関係するcamera or 関係するeNull
	//if ((!cameraanimflag && (curbone->IsSkeleton() || curbone->IsNull())) || 
	//	(cameraanimflag && (curbone->IsConcernedCamera(cmpmotionname) || curbone->IsConcernedNullAndChildIsCamera(cmpmotionname)))) {
	if (curbone->IsConcerned(srcmotid)) {
		InitMP(srcmodel, limitdegflag, curbone, srcmotid, curframe);
	}

	//if (curbone->IsSkeleton() || curbone->IsCamera() || curbone->IsNullAndChildIsCamera()) {
	//	InitMP(srcmodel, limitdegflag, curbone, srcmotid, curframe);
	//}

	if (curbone->GetChild(false)) {
		InitMPReq(srcmodel, limitdegflag, curbone->GetChild(false), srcmotid, curframe);
	}
	if (curbone->GetBrother(false)) {
		InitMPReq(srcmodel, limitdegflag, curbone->GetBrother(false), srcmotid, curframe);
	}
}

BEFEUL ChaCalcFunc::GetBefEul(CBone* srcbone, bool limitdegflag, int srcmotid, double srcframe)
{
	double roundingframe = RoundingTime(srcframe);

	BEFEUL befeul;
	befeul.Init();

	if (!srcbone) {
		befeul.Init();
		return befeul;
	}


	if (srcbone->IsNotSkeleton() && srcbone->IsNotCamera()) {
		befeul.Init();
		return befeul;
	}

	//###########
	//bef frame
	//###########
	double befframe;
	befframe = roundingframe - 1.0;
	if (roundingframe <= 1.01) {
		//roundingframe が0.0または1.0の場合 
		CMotionPoint* curmp;
		curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
		if (curmp) {
			befeul.befframeeul = srcbone->GetLocalEul(limitdegflag, srcmotid, roundingframe, curmp);
		}
	}
	else {
		CMotionPoint* befmp;
		befmp = srcbone->GetMotionPoint(srcmotid, befframe);
		if (befmp) {
			befeul.befframeeul = srcbone->GetLocalEul(limitdegflag, srcmotid, befframe, befmp);
		}
	}

	//##############
	//current frame
	//##############
	CMotionPoint* curmp;
	curmp = srcbone->GetMotionPoint(srcmotid, roundingframe);
	if (curmp) {
		befeul.currentframeeul = srcbone->GetLocalEul(limitdegflag, srcmotid, roundingframe, curmp);
	}






	////##############################################################################################
	////2023/10/12
	////モデル読込中と　リターゲット中と　ファイル書き出し中だけ　前のフレーム番号のオイラー角を取得
	////それ以外の場合には　カレントフレームの編集前のオイラー角を取得
	////このようにすることで　リターゲット中及びIK中の　１８０度裏返り問題が大きく緩和される
	////(読込中とリターゲット中とファイル書き出し中がifの前半部分　IK中はelse部分)
	////主にbvh121とbvh144でテスト
	////##############################################################################################
	//
	//if ((g_underRetargetFlag == true) || 
	//	(GetParModel() && GetParModel()->GetLoadedFlag() == false) || 
	//	(g_underWriteFbx == true)) {
	//
	//	//1つ前のフレームのEULはすでに計算されていると仮定する。
	//	double befframe;
	//	befframe = roundingframe - 1.0;
	//	if (roundingframe <= 1.01) {
	//		//roundingframe が0.0または1.0の場合 
	//		//befeul.SetParams(0.0f, 0.0f, 0.0f);
	//		CMotionPoint* curmp;
	//		curmp = GetMotionPoint(srcmotid, roundingframe);
	//		if (curmp) {
	//			befeul = GetLocalEul(limitdegflag, srcmotid, roundingframe, curmp);
	//		}
	//	}
	//	else {
	//		CMotionPoint* befmp;
	//		befmp = GetMotionPoint(srcmotid, befframe);
	//		if (befmp) {
	//			befeul = GetLocalEul(limitdegflag, srcmotid, befframe, befmp);
	//		}
	//	}
	//}
	//else {
	//	CMotionPoint* curmp;
	//	curmp = GetMotionPoint(srcmotid, roundingframe);
	//	if (curmp) {
	//		befeul = GetLocalEul(limitdegflag, srcmotid, roundingframe, curmp);
	//	}
	//}
	//
	////if (g_underIKRot == true) {
	////	if (roundingframe <= 1.01) {
	////		befeul.SetParams(0.0f, 0.0f, 0.0f);
	////	}
	////}

	return befeul;
}


//#################################################################
//Following Functions are Called From GlobalFunctions at ChaVecCalc
//#################################################################

CQuaternion ChaCalcFunc::ccfQMakeFromBtMat3x3(btMatrix3x3* eulmat)
{
	CQuaternion retq;
	retq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	if (!eulmat) {
		return retq;
	}

	btVector3 tmpcol[3];
	int colno;
	for (colno = 0; colno < 3; colno++) {
		tmpcol[colno] = eulmat->getColumn(colno);
	}

	ChaMatrix xmat;
	ChaMatrixIdentity(&xmat);

	xmat.data[MATI_11] = tmpcol[0].x();
	xmat.data[MATI_12] = tmpcol[0].y();
	xmat.data[MATI_13] = tmpcol[0].z();

	xmat.data[MATI_21] = tmpcol[1].x();
	xmat.data[MATI_22] = tmpcol[1].y();
	xmat.data[MATI_23] = tmpcol[1].z();

	xmat.data[MATI_31] = tmpcol[2].x();
	xmat.data[MATI_32] = tmpcol[2].y();
	xmat.data[MATI_33] = tmpcol[2].z();

	retq.RotationMatrix(xmat);
	return retq;

}

void ChaCalcFunc::ccfChaMatrixIdentity(ChaMatrix* pdst)
{
	if (!pdst) {
		return;
	}
	pdst->data[MATI_11] = 1.0f;
	pdst->data[MATI_12] = 0.0f;
	pdst->data[MATI_13] = 0.0f;
	pdst->data[MATI_14] = 0.0f;

	pdst->data[MATI_21] = 0.0f;
	pdst->data[MATI_22] = 1.0f;
	pdst->data[MATI_23] = 0.0f;
	pdst->data[MATI_24] = 0.0f;

	pdst->data[MATI_31] = 0.0f;
	pdst->data[MATI_32] = 0.0f;
	pdst->data[MATI_33] = 1.0f;
	pdst->data[MATI_34] = 0.0f;

	pdst->data[MATI_41] = 0.0f;
	pdst->data[MATI_42] = 0.0f;
	pdst->data[MATI_43] = 0.0f;
	pdst->data[MATI_44] = 1.0f;
}

CQuaternion ChaCalcFunc::ccfChaMatrix2Q(ChaMatrix srcmat)//ChaMatrixを受け取って　CQuaternionを返す
{
	CQuaternion retq;
	retq.RotationMatrix(srcmat);
	return retq;
}

ChaMatrix ChaCalcFunc::ccfChaMatrixRot(ChaMatrix srcmat)//回転成分だけの行列にする
{
	ChaMatrix retmat;
	retmat.SetIdentity();

	ChaVector3 rotx, roty, rotz;
	rotx.SetParams(srcmat.data[MATI_11], srcmat.data[MATI_12], srcmat.data[MATI_13]);
	roty.SetParams(srcmat.data[MATI_21], srcmat.data[MATI_22], srcmat.data[MATI_23]);
	rotz.SetParams(srcmat.data[MATI_31], srcmat.data[MATI_32], srcmat.data[MATI_33]);
	ChaVector3Normalize(&rotx, &rotx);
	ChaVector3Normalize(&roty, &roty);
	ChaVector3Normalize(&rotz, &rotz);

	retmat.data[MATI_11] = rotx.x;
	retmat.data[MATI_12] = rotx.y;
	retmat.data[MATI_13] = rotx.z;

	retmat.data[MATI_21] = roty.x;
	retmat.data[MATI_22] = roty.y;
	retmat.data[MATI_23] = roty.z;

	retmat.data[MATI_31] = rotz.x;
	retmat.data[MATI_32] = rotz.y;
	retmat.data[MATI_33] = rotz.z;

	return retmat;

}

ChaMatrix ChaCalcFunc::ccfChaMatrixScale(ChaMatrix srcmat)//スケール成分だけの行列にする
{
	ChaMatrix retmat;
	retmat.SetIdentity();

	ChaVector3 rotx, roty, rotz;
	rotx.SetParams(srcmat.data[MATI_11], srcmat.data[MATI_12], srcmat.data[MATI_13]);
	roty.SetParams(srcmat.data[MATI_21], srcmat.data[MATI_22], srcmat.data[MATI_23]);
	rotz.SetParams(srcmat.data[MATI_31], srcmat.data[MATI_32], srcmat.data[MATI_33]);

	float scalex, scaley, scalez;
	scalex = (float)ChaVector3LengthDbl(&rotx);
	scaley = (float)ChaVector3LengthDbl(&roty);
	scalez = (float)ChaVector3LengthDbl(&rotz);

	retmat.data[MATI_11] = scalex;
	retmat.data[MATI_22] = scaley;
	retmat.data[MATI_33] = scalez;

	return retmat;
}

ChaMatrix ChaCalcFunc::ccfChaMatrixTra(ChaMatrix srcmat)//移動成分だけの行列にする
{
	ChaMatrix retmat;
	retmat.SetIdentity();

	retmat.data[MATI_41] = srcmat.data[MATI_41];
	retmat.data[MATI_42] = srcmat.data[MATI_42];
	retmat.data[MATI_43] = srcmat.data[MATI_43];

	return retmat;
}

ChaVector3 ChaCalcFunc::ccfChaMatrixScaleVec(ChaMatrix srcmat)//スケール成分のベクトルを取得
{
	ChaVector3 rotx, roty, rotz;
	rotx.SetParams(srcmat.data[MATI_11], srcmat.data[MATI_12], srcmat.data[MATI_13]);
	roty.SetParams(srcmat.data[MATI_21], srcmat.data[MATI_22], srcmat.data[MATI_23]);
	rotz.SetParams(srcmat.data[MATI_31], srcmat.data[MATI_32], srcmat.data[MATI_33]);

	float scalex, scaley, scalez;
	scalex = (float)ChaVector3LengthDbl(&rotx);
	scaley = (float)ChaVector3LengthDbl(&roty);
	scalez = (float)ChaVector3LengthDbl(&rotz);

	ChaVector3 retvec;
	retvec.x = scalex;
	retvec.y = scaley;
	retvec.z = scalez;

	return retvec;
}
ChaVector3 ChaCalcFunc::ccfChaMatrixRotVec(ChaMatrix srcmat, int notmodify180flag)//回転成分のベクトルを取得
{
	//ローカルオイラー角を取得するためには
	//srcmatには　GetNodeMat * GetWorldMat * Inv(GetParent()->GetWorldMat) * Inv(GetParent()->GetNodeMat) を渡す
	//ジョイントのオイラー角を取得する場合には　CBone::CalcLocalEulXYZ()を使う

	ChaVector3 reteul;
	reteul.SetParams(0.0f, 0.0f, 0.0f);
	//ChaVector3 befeul.SetParams(0.0f, 0.0f, 0.0f);
	BEFEUL befeul;
	befeul.Init();

	int isfirstbone = 0;
	int isendbone = 0;

	CQuaternion eulq;
	bool underikrot = false;
	bool underretarget = false;
	eulq.Q2EulXYZusingQ(underikrot, underretarget, 0, befeul, &reteul, isfirstbone, isendbone, notmodify180flag);
	//eulq.Q2EulXYZusingMat(ROTORDER_XYZ, 0, befeul, &reteul, isfirstbone, isendbone, notmodify180flag);


	return reteul;
}
ChaVector3 ChaCalcFunc::ccfChaMatrixTraVec(ChaMatrix srcmat)//移動成分のベクトルを取得
{
	ChaVector3 rettra;
	rettra.x = srcmat.data[MATI_41];
	rettra.y = srcmat.data[MATI_42];
	rettra.z = srcmat.data[MATI_43];

	return rettra;
}

void ChaCalcFunc::ccfChaMatrixNormalizeRot(ChaMatrix* pdst)
{
	if (!pdst) {
		return;
	}

	ChaMatrix srcmat = *pdst;

	ChaVector3 vecx, vecy, vecz;
	vecx.x = srcmat.data[MATI_11];
	vecx.y = srcmat.data[MATI_12];
	vecx.z = srcmat.data[MATI_13];

	vecy.x = srcmat.data[MATI_21];
	vecy.y = srcmat.data[MATI_22];
	vecy.z = srcmat.data[MATI_23];

	vecz.x = srcmat.data[MATI_31];
	vecz.y = srcmat.data[MATI_32];
	vecz.z = srcmat.data[MATI_33];

	ChaVector3Normalize(&vecx, &vecx);
	ChaVector3Normalize(&vecy, &vecy);
	ChaVector3Normalize(&vecz, &vecz);

	pdst->data[MATI_11] = vecx.x;
	pdst->data[MATI_12] = vecx.y;
	pdst->data[MATI_13] = vecx.z;

	pdst->data[MATI_21] = vecy.x;
	pdst->data[MATI_22] = vecy.y;
	pdst->data[MATI_23] = vecy.z;

	pdst->data[MATI_31] = vecz.x;
	pdst->data[MATI_32] = vecz.y;
	pdst->data[MATI_33] = vecz.z;


}


//#####################################################################################
//2022/11/06 テスト中　問題発生　表示には問題は出ていなかったが
//３Dオブジェクトをマウスでピックする部分で　ピックの精度が著しく落ちて使えなかった
//なぜかはよくわかっていないが　doubleの AVXを使わないと　精度が間に合わないのが問題？
//この数式のまま　AVX2に移行することは可能だろうか？と考え中
//とりあえずコメントアウト　
//#####################################################################################
//void ChaCalcFunc::ChaMatrixInverse(ChaMatrix* pdst, float* pdet, const ChaMatrix* psrc)
//{
//	//https://lxjk.github.io/2020/02/07/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained-JP.html
//
//	if (!pdst || !psrc) {
//		return;
//	}
//
//	ChaMatrix res;
//	res.SetIdentity();
//
//	if (IsInitMat(*psrc) == 1) {
//		//case src is Identity.
//		pdst->SetIdentity();
//		if (pdet) {
//			*pdet = 1.0f;
//		}
//	}
//	else {
//		// use block matrix method
//		// A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace
//
//		// sub matrices
//		__m128 A = VecShuffle_0101(psrc->mVec[0], psrc->mVec[1]);
//		__m128 B = VecShuffle_2323(psrc->mVec[0], psrc->mVec[1]);
//		__m128 C = VecShuffle_0101(psrc->mVec[2], psrc->mVec[3]);
//		__m128 D = VecShuffle_2323(psrc->mVec[2], psrc->mVec[3]);
//
//#if 0
//		__m128 detA = _mm_set1_ps(psrc->m[0][0] * psrc->m[1][1] - psrc->m[0][1] * psrc->m[1][0]);
//		__m128 detB = _mm_set1_ps(psrc->m[0][2] * psrc->m[1][3] - psrc->m[0][3] * psrc->m[1][2]);
//		__m128 detC = _mm_set1_ps(psrc->m[2][0] * psrc->m[3][1] - psrc->m[2][1] * psrc->m[3][0]);
//		__m128 detD = _mm_set1_ps(psrc->m[2][2] * psrc->m[3][3] - psrc->m[2][3] * psrc->m[3][2]);
//#else
//		// determinant as (|A| |B| |C| |D|)
//		__m128 detSub = _mm_sub_ps(
//			_mm_mul_ps(VecShuffle(psrc->mVec[0], psrc->mVec[2], 0, 2, 0, 2), VecShuffle(psrc->mVec[1], psrc->mVec[3], 1, 3, 1, 3)),
//			_mm_mul_ps(VecShuffle(psrc->mVec[0], psrc->mVec[2], 1, 3, 1, 3), VecShuffle(psrc->mVec[1], psrc->mVec[3], 0, 2, 0, 2))
//		);
//		__m128 detA = VecSwizzle1(detSub, 0);
//		__m128 detB = VecSwizzle1(detSub, 1);
//		__m128 detC = VecSwizzle1(detSub, 2);
//		__m128 detD = VecSwizzle1(detSub, 3);
//#endif
//
//		// let iM = 1/|M| * | X  Y |
//		//                  | Z  W |
//
//		// D#C
//		__m128 D_C = Mat2AdjMul(D, C);
//		// A#B
//		__m128 A_B = Mat2AdjMul(A, B);
//		// X# = |D|A - B(D#C)
//		__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), Mat2Mul(B, D_C));
//		// W# = |A|D - C(A#B)
//		__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), Mat2Mul(C, A_B));
//
//		// |M| = |A|*|D| + ... (continue later)
//		__m128 detM = _mm_mul_ps(detA, detD);
//
//		// Y# = |B|C - D(A#B)#
//		__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), Mat2MulAdj(D, A_B));
//		// Z# = |C|B - A(D#C)#
//		__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), Mat2MulAdj(A, D_C));
//
//		// |M| = |A|*|D| + |B|*|C| ... (continue later)
//		detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));
//
//		// tr((A#B)(D#C))
//		__m128 tr = _mm_mul_ps(A_B, VecSwizzle(D_C, 0, 2, 1, 3));
//		tr = _mm_hadd_ps(tr, tr);
//		tr = _mm_hadd_ps(tr, tr);
//		// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C)
//		detM = _mm_sub_ps(detM, tr);
//
//
//		float checkdetM4[4];
//		_mm_store_ps(checkdetM4, detM);
//		//if (fabs(checkdetM4[3]) >= 1e-5) {
//		if (fabs(checkdetM4[3]) >= FLT_MIN) {//2022/11/23 ChkRayにおいて最小距離より近いものは当たりにしている　1e-5では最小距離の精度が無かった　この値ならOK
//			const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
//			// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
//			__m128 rDetM = _mm_div_ps(adjSignMask, detM);
//
//			X_ = _mm_mul_ps(X_, rDetM);
//			Y_ = _mm_mul_ps(Y_, rDetM);
//			Z_ = _mm_mul_ps(Z_, rDetM);
//			W_ = _mm_mul_ps(W_, rDetM);
//
//			// apply adjugate and store, here we combine adjugate shuffle and store shuffle
//			res.mVec[0] = VecShuffle(X_, Y_, 3, 1, 3, 1);
//			res.mVec[1] = VecShuffle(X_, Y_, 2, 0, 2, 0);
//			res.mVec[2] = VecShuffle(Z_, W_, 3, 1, 3, 1);
//			res.mVec[3] = VecShuffle(Z_, W_, 2, 0, 2, 0);
//
//			*pdst = res;
//		}
//		else {
//			*pdst = *psrc;
//		}
//	}
//}

void ChaCalcFunc::ccfChaMatrixInverse(ChaMatrix* pdst, float* pdet, const ChaMatrix* psrc)
{
	if (!pdst || !psrc) {
		return;
	}

	ChaMatrix res;
	double detA;
	double a11, a12, a13, a14, a21, a22, a23, a24, a31, a32, a33, a34, a41, a42, a43, a44;
	double b11, b12, b13, b14, b21, b22, b23, b24, b31, b32, b33, b34, b41, b42, b43, b44;

	a11 = psrc->data[MATI_11];
	a12 = psrc->data[MATI_12];
	a13 = psrc->data[MATI_13];
	a14 = psrc->data[MATI_14];

	a21 = psrc->data[MATI_21];
	a22 = psrc->data[MATI_22];
	a23 = psrc->data[MATI_23];
	a24 = psrc->data[MATI_24];

	a31 = psrc->data[MATI_31];
	a32 = psrc->data[MATI_32];
	a33 = psrc->data[MATI_33];
	a34 = psrc->data[MATI_34];

	a41 = psrc->data[MATI_41];
	a42 = psrc->data[MATI_42];
	a43 = psrc->data[MATI_43];
	a44 = psrc->data[MATI_44];

	detA = (a11 * a22 * a33 * a44) + (a11 * a23 * a34 * a42) + (a11 * a24 * a32 * a43)
		+ (a12 * a21 * a34 * a43) + (a12 * a23 * a31 * a44) + (a12 * a24 * a33 * a41)
		+ (a13 * a21 * a32 * a44) + (a13 * a22 * a34 * a41) + (a13 * a24 * a31 * a42)
		+ (a14 * a21 * a33 * a42) + (a14 * a22 * a31 * a43) + (a14 * a23 * a32 * a41)
		- (a11 * a22 * a34 * a43) - (a11 * a23 * a32 * a44) - (a11 * a24 * a33 * a42)
		- (a12 * a21 * a33 * a44) - (a12 * a23 * a34 * a41) - (a12 * a24 * a31 * a43)
		- (a13 * a21 * a34 * a42) - (a13 * a22 * a31 * a44) - (a13 * a24 * a32 * a41)
		- (a14 * a21 * a32 * a43) - (a14 * a22 * a33 * a41) - (a14 * a23 * a31 * a42);

	if (pdet) {
		*pdet = (float)detA;
	}


	if (detA == 0.0) {
		*pdst = *psrc;
		return;//!!!!!!!!!!!!!!!!!!!!!!!!!!
	}




	b11 = (a22 * a33 * a44) + (a23 * a34 * a42) + (a24 * a32 * a43) - (a22 * a34 * a43) - (a23 * a32 * a44) - (a24 * a33 * a42);
	b12 = (a12 * a34 * a43) + (a13 * a32 * a44) + (a14 * a33 * a42) - (a12 * a33 * a44) - (a13 * a34 * a42) - (a14 * a32 * a43);
	b13 = (a12 * a23 * a44) + (a13 * a24 * a42) + (a14 * a22 * a43) - (a12 * a24 * a43) - (a13 * a22 * a44) - (a14 * a23 * a42);
	b14 = (a12 * a24 * a33) + (a13 * a22 * a34) + (a14 * a23 * a32) - (a12 * a23 * a34) - (a13 * a24 * a32) - (a14 * a22 * a33);

	b21 = (a21 * a34 * a43) + (a23 * a31 * a44) + (a24 * a33 * a41) - (a21 * a33 * a44) - (a23 * a34 * a41) - (a24 * a31 * a43);
	b22 = (a11 * a33 * a44) + (a13 * a34 * a41) + (a14 * a31 * a43) - (a11 * a34 * a43) - (a13 * a31 * a44) - (a14 * a33 * a41);
	b23 = (a11 * a24 * a43) + (a13 * a21 * a44) + (a14 * a23 * a41) - (a11 * a23 * a44) - (a13 * a24 * a41) - (a14 * a21 * a43);
	b24 = (a11 * a23 * a34) + (a13 * a24 * a31) + (a14 * a21 * a33) - (a11 * a24 * a33) - (a13 * a21 * a34) - (a14 * a23 * a31);

	b31 = (a21 * a32 * a44) + (a22 * a34 * a41) + (a24 * a31 * a42) - (a21 * a34 * a42) - (a22 * a31 * a44) - (a24 * a32 * a41);
	b32 = (a11 * a34 * a42) + (a12 * a31 * a44) + (a14 * a32 * a41) - (a11 * a32 * a44) - (a12 * a34 * a41) - (a14 * a31 * a42);
	b33 = (a11 * a22 * a44) + (a12 * a24 * a41) + (a14 * a21 * a42) - (a11 * a24 * a42) - (a12 * a21 * a44) - (a14 * a22 * a41);
	b34 = (a11 * a24 * a32) + (a12 * a21 * a34) + (a14 * a22 * a31) - (a11 * a22 * a34) - (a12 * a24 * a31) - (a14 * a21 * a32);

	b41 = (a21 * a33 * a42) + (a22 * a31 * a43) + (a23 * a32 * a41) - (a21 * a32 * a43) - (a22 * a33 * a41) - (a23 * a31 * a42);
	b42 = (a11 * a32 * a43) + (a12 * a33 * a41) + (a13 * a31 * a42) - (a11 * a33 * a42) - (a12 * a31 * a43) - (a13 * a32 * a41);
	b43 = (a11 * a23 * a42) + (a12 * a21 * a43) + (a13 * a22 * a41) - (a11 * a22 * a43) - (a12 * a23 * a41) - (a13 * a21 * a42);
	b44 = (a11 * a22 * a33) + (a12 * a23 * a31) + (a13 * a21 * a32) - (a11 * a23 * a32) - (a12 * a21 * a33) - (a13 * a22 * a31);

	res.data[MATI_11] = (float)(b11 / detA);
	res.data[MATI_12] = (float)(b12 / detA);
	res.data[MATI_13] = (float)(b13 / detA);
	res.data[MATI_14] = (float)(b14 / detA);

	res.data[MATI_21] = (float)(b21 / detA);
	res.data[MATI_22] = (float)(b22 / detA);
	res.data[MATI_23] = (float)(b23 / detA);
	res.data[MATI_24] = (float)(b24 / detA);

	res.data[MATI_31] = (float)(b31 / detA);
	res.data[MATI_32] = (float)(b32 / detA);
	res.data[MATI_33] = (float)(b33 / detA);
	res.data[MATI_34] = (float)(b34 / detA);

	res.data[MATI_41] = (float)(b41 / detA);
	res.data[MATI_42] = (float)(b42 / detA);
	res.data[MATI_43] = (float)(b43 / detA);
	res.data[MATI_44] = (float)(b44 / detA);

	*pdst = res;
}


void ChaCalcFunc::ccfChaMatrixTranslation(ChaMatrix* pdst, float srcx, float srcy, float srcz)
{
	if (!pdst) {
		return;
	}

	pdst->data[MATI_41] = srcx;
	pdst->data[MATI_42] = srcy;
	pdst->data[MATI_43] = srcz;
}

void ChaCalcFunc::ccfChaMatrixTranspose(ChaMatrix* pdst, ChaMatrix* psrc)
{
	if (!pdst || !psrc) {
		_ASSERT(0);
		return;
	}

	ChaMatrix savesrc;
	savesrc = *psrc;


	pdst->data[MATI_11] = savesrc.data[MATI_11];
	pdst->data[MATI_12] = savesrc.data[MATI_21];
	pdst->data[MATI_13] = savesrc.data[MATI_31];
	pdst->data[MATI_14] = savesrc.data[MATI_41];

	pdst->data[MATI_21] = savesrc.data[MATI_12];
	pdst->data[MATI_22] = savesrc.data[MATI_22];
	pdst->data[MATI_23] = savesrc.data[MATI_32];
	pdst->data[MATI_24] = savesrc.data[MATI_42];

	pdst->data[MATI_31] = savesrc.data[MATI_13];
	pdst->data[MATI_32] = savesrc.data[MATI_23];
	pdst->data[MATI_33] = savesrc.data[MATI_33];
	pdst->data[MATI_34] = savesrc.data[MATI_43];

	pdst->data[MATI_41] = savesrc.data[MATI_14];
	pdst->data[MATI_42] = savesrc.data[MATI_24];
	pdst->data[MATI_43] = savesrc.data[MATI_34];
	pdst->data[MATI_44] = savesrc.data[MATI_44];

	//float m[4][4];
	//m[0][0] = psrc->data[MATI_11];
	//m[0][1] = psrc->data[MATI_12];
	//m[0][2] = psrc->data[MATI_13];
	//m[0][3] = psrc->data[MATI_14];
	//m[1][0] = psrc->data[MATI_21];
	//m[1][1] = psrc->data[MATI_22];
	//m[1][2] = psrc->data[MATI_23];
	//m[1][3] = psrc->data[MATI_24];
	//m[2][0] = psrc->data[MATI_31];
	//m[2][1] = psrc->data[MATI_32];
	//m[2][2] = psrc->data[MATI_33];
	//m[2][3] = psrc->data[MATI_34];
	//m[3][0] = psrc->data[MATI_41];
	//m[3][1] = psrc->data[MATI_42];
	//m[3][2] = psrc->data[MATI_43];
	//m[3][3] = psrc->data[MATI_44];

	//pdst->data[MATI_11] = m[0][0];
	//pdst->data[MATI_12] = m[1][0];
	//pdst->data[MATI_13] = m[2][0];
	//pdst->data[MATI_14] = m[3][0];

	//pdst->data[MATI_21] = m[0][1];
	//pdst->data[MATI_22] = m[1][1];
	//pdst->data[MATI_23] = m[2][1];
	//pdst->data[MATI_24] = m[3][1];

	//pdst->data[MATI_31] = m[0][2];
	//pdst->data[MATI_32] = m[1][2];
	//pdst->data[MATI_33] = m[2][2];
	//pdst->data[MATI_34] = m[3][2];

	//pdst->data[MATI_41] = m[0][3];
	//pdst->data[MATI_42] = m[1][3];
	//pdst->data[MATI_43] = m[2][3];
	//pdst->data[MATI_44] = m[3][3];

}

double ChaCalcFunc::ccfChaVector3LengthDbl(ChaVector3* v)
{
	if (!v) {
		return 0.0f;
	}

	double leng;
	double mag;
	mag = (double)v->x * (double)v->x + (double)v->y * (double)v->y + (double)v->z * (double)v->z;
	if (mag != 0.0) {
		leng = sqrt(mag);
	}
	else {
		leng = 0.0;
	}
	return leng;

}

double ChaCalcFunc::ccfChaVector3DotDbl(const ChaVector3* psrc1, const ChaVector3* psrc2)
{
	if (!psrc1 || !psrc2) {
		return 0.0f;
	}

	double retval = (double)psrc1->x * (double)psrc2->x + (double)psrc1->y * (double)psrc2->y + (double)psrc1->z * (double)psrc2->z;

	return retval;
}



//double ChaCalcFunc::ccfChaVector3LengthDbl(ChaVector3* v)
//{
//	if (!v){
//		return 0.0f;
//	}
//
//	double leng;
//	double mag;
//	mag = (double)v->x * (double)v->x + (double)v->y * (double)v->y + (double)v->z * (double)v->z;
//	if (mag != 0.0f){
//		leng = (float)sqrt(mag);
//	}
//	else{
//		leng = 0.0;
//	}
//	return leng;
//
//}

void ChaCalcFunc::ccfChaVector3Normalize(ChaVector3* pdst, const ChaVector3* psrc) {
	if (!pdst || !psrc) {
		return;
	}

	//__m128 srcx = _mm_load1_ps(&psrc->x);
	//__m128 srcy = _mm_load1_ps(&psrc->y);
	//__m128 srcz = _mm_load1_ps(&psrc->z);

	//__m128 mulx = _mm_mul_ps(srcx, srcx);
	//__m128 maddxy = _mm_madd_ps(srcy, srcy, mulx);
	//__m128 maddxyz = _mm_madd_ps(srcz, srcz, maddxy);

	//float check[4];
	//_mm_store_ps(check, maddxyz);
	////if (check[0] >= 1e-7) {
	//if (fabs(check[0]) >= FLT_MIN) {//2022/11/23
	//	__m128 invsqrt = _mm_rsqrt_ps(maddxyz);//平方根の逆数

	//	__m128 srcxyz1 = _mm_setr_ps(psrc->x, psrc->y, psrc->z, 0.0f);
	//	__m128 mresult = _mm_mul_ps(srcxyz1, invsqrt);

	//	float result[4];
	//	_mm_store_ps(result, mresult);

	//	pdst->x = result[0];
	//	pdst->y = result[1];
	//	pdst->z = result[2];
	//}
	//else {
	//	//*pdst.SetParams(0.0f, 0.0f, 0.0f);
	//	pdst->x = psrc->x;
	//	pdst->y = psrc->y;
	//	pdst->z = psrc->z;
	//}

	ChaVector3 src = *psrc;
	double mag = (double)src.x * (double)src.x + (double)src.y * (double)src.y + (double)src.z * (double)src.z;
	if (mag != 0.0) {
		double divval = ::sqrt(mag);
		if (divval != 0.0) {
			double tmpx = src.x / divval;
			double tmpy = src.y / divval;
			double tmpz = src.z / divval;
			pdst->x = (float)tmpx;
			pdst->y = (float)tmpy;
			pdst->z = (float)tmpz;
		}
		else {
			pdst->x = src.x;
			pdst->y = src.y;
			pdst->z = src.z;
		}
	}
	else {
		//*pdst.SetParams(0.0f, 0.0f, 0.0f);
		pdst->x = src.x;
		pdst->y = src.y;
		pdst->z = src.z;
	}
}

float ChaCalcFunc::ccfChaVector3Dot(const ChaVector3* psrc1, const ChaVector3* psrc2)
{
	if (!psrc1 || !psrc2) {
		return 0.0f;
	}

	return (psrc1->x * psrc2->x + psrc1->y * psrc2->y + psrc1->z * psrc2->z);

}

void ChaCalcFunc::ccfChaVector3Cross(ChaVector3* pdst, const ChaVector3* psrc1, const ChaVector3* psrc2)
{
	if (!pdst || !psrc1 || !psrc2) {
		return;
	}

	//ChaVector3 v1, v2;
	//v1 = *psrc1;
	//v2 = *psrc2;

	//pdst->x = v1.y * v2.z - v1.z * v2.y;
	//pdst->y = v1.z * v2.x - v1.x * v2.z;
	//pdst->z = v1.x * v2.y - v1.y * v2.x;
	pdst->x = (float)((double)psrc1->y * (double)psrc2->z - (double)psrc1->z * (double)psrc2->y);
	pdst->y = (float)((double)psrc1->z * (double)psrc2->x - (double)psrc1->x * (double)psrc2->z);
	pdst->z = (float)((double)psrc1->x * (double)psrc2->y - (double)psrc1->y * (double)psrc2->x);
}


void ChaCalcFunc::ccfChaVector3TransformCoord(ChaVector3* dstvec, ChaVector3* srcvec, ChaMatrix* srcmat)
{
	if (!dstvec || !srcvec || !srcmat) {
		return;
	}

	////float src4fl[4] = { srcvec->x, srcvec->y, srcvec->z, 1.0f };
	////__m128 ma_col_0 = _mm_load_ps(src4fl);
	//const __m128 ma_col_0 = _mm_setr_ps(srcvec->x, srcvec->y, srcvec->z, 1.0f);


	//__m128 resultX;
	//__m128 resultXY;
	//__m128 resultXYZ;
	//__m128 resultXYZW;
	//resultX = _mm_mul_ps(srcmat->mVec[0], _mm_replicate_x_ps(ma_col_0));
	//resultXY = _mm_madd_ps(srcmat->mVec[1], _mm_replicate_y_ps(ma_col_0), resultX);
	//resultXYZ = _mm_madd_ps(srcmat->mVec[2], _mm_replicate_z_ps(ma_col_0), resultXY);
	//resultXYZW = _mm_madd_ps(srcmat->mVec[3], _mm_replicate_w_ps(ma_col_0), resultXYZ);


	////x/tmpw, y/tmpw, z/tmpw, w/tmpw
	//float checkw4[4];
	//_mm_store_ps(checkw4, resultXYZW);
	////if (fabs(checkw4[3]) >= 1e-5) {
	//if (fabs(checkw4[3]) >= FLT_MIN) {//2022/11/23
	//	const __m128 inverseOne = _mm_setr_ps(1.f, 1.f, 1.f, 1.f);
	//	const __m128 resultw = VecSwizzle1(resultXYZW, 3);
	//	__m128 rDetM = _mm_div_ps(inverseOne, resultw);
	//	resultXYZW = _mm_mul_ps(resultXYZW, rDetM);


	//	float resultfl4[4];
	//	_mm_store_ps(resultfl4, resultXYZW);

	//	dstvec->x = resultfl4[0];
	//	dstvec->y = resultfl4[1];
	//	dstvec->z = resultfl4[2];
	//}
	//else {
	//	//dstvec->x = 0.0f;
	//	//dstvec->y = 0.0f;
	//	//dstvec->z = 0.0f;
	//	*dstvec = *srcvec;
	//}

	double tmpx = (double)srcmat->data[MATI_11] * (double)srcvec->x + (double)srcmat->data[MATI_21] * (double)srcvec->y + (double)srcmat->data[MATI_31] * (double)srcvec->z + (double)srcmat->data[MATI_41];
	double tmpy = (double)srcmat->data[MATI_12] * (double)srcvec->x + (double)srcmat->data[MATI_22] * (double)srcvec->y + (double)srcmat->data[MATI_32] * (double)srcvec->z + (double)srcmat->data[MATI_42];
	double tmpz = (double)srcmat->data[MATI_13] * (double)srcvec->x + (double)srcmat->data[MATI_23] * (double)srcvec->y + (double)srcmat->data[MATI_33] * (double)srcvec->z + (double)srcmat->data[MATI_43];
	double tmpw = (double)srcmat->data[MATI_14] * (double)srcvec->x + (double)srcmat->data[MATI_24] * (double)srcvec->y + (double)srcmat->data[MATI_34] * (double)srcvec->z + (double)srcmat->data[MATI_44];

	if (tmpw != 0.0) {
		dstvec->x = (float)(tmpx / tmpw);
		dstvec->y = (float)(tmpy / tmpw);
		dstvec->z = (float)(tmpz / tmpw);
	}
	else {
		//dstvec->x = 0.0f;
		//dstvec->y = 0.0f;
		//dstvec->z = 0.0f;

		//*dstvec = *srcvec;

		dstvec->x = (float)tmpx;
		dstvec->y = (float)tmpy;
		dstvec->z = (float)tmpz;

	}
}

/*
D3DXVECTOR3 *WINAPI D3DXVec3TransformNormal(D3DXVECTOR3 *pOut,
CONST D3DXVECTOR3 *pV,
CONST D3DXMATRIX *pM
);
パラメータ

pOut
[in, out] 演算結果である D3DXVECTOR3 構造体へのポインタ。
pV
[in] 処理の基になる D3DXVECTOR3 構造体へのポインタ。
pM
[in] 処理の基になる D3DXMATRIX 構造体へのポインタ。
戻り値

トランスフォームされたベクトルの D3DXVECTOR3 構造体へのポインタ。

注意

この関数は、ベクトル pV のベクトル法線(x, y, z, 0) を行列 pM でトランスフォームする。
*/
ChaVector3* ChaCalcFunc::ccfChaVector3TransformNormal(ChaVector3* dstvec, const ChaVector3* srcvec, const ChaMatrix* srcmat)
{
	if (!dstvec || !srcvec || !srcmat) {
		return NULL;
	}

	double tmpx, tmpy, tmpz, tmpw;
	tmpx = (double)srcmat->data[MATI_11] * (double)srcvec->x + (double)srcmat->data[MATI_21] * (double)srcvec->y + (double)srcmat->data[MATI_31] * (double)srcvec->z;// +(double)srcmat->data[MATI_41];
	tmpy = (double)srcmat->data[MATI_12] * (double)srcvec->x + (double)srcmat->data[MATI_22] * (double)srcvec->y + (double)srcmat->data[MATI_32] * (double)srcvec->z;// + (double)srcmat->data[MATI_42];
	tmpz = (double)srcmat->data[MATI_13] * (double)srcvec->x + (double)srcmat->data[MATI_23] * (double)srcvec->y + (double)srcmat->data[MATI_33] * (double)srcvec->z;// + (double)srcmat->data[MATI_43];
	tmpw = (double)srcmat->data[MATI_14] * (double)srcvec->x + (double)srcmat->data[MATI_24] * (double)srcvec->y + (double)srcmat->data[MATI_34] * (double)srcvec->z;// + (double)srcmat->data[MATI_44];

	if (tmpw != 0.0) {
		dstvec->x = (float)(tmpx / tmpw);
		dstvec->y = (float)(tmpy / tmpw);
		dstvec->z = (float)(tmpz / tmpw);
	}
	else {
		//dstvec->x = 0.0f;
		//dstvec->y = 0.0f;
		//dstvec->z = 0.0f;
		*dstvec = *srcvec;
	}

	return dstvec;
}



double ChaCalcFunc::ccfChaVector3LengthSq(ChaVector3* psrc)
{
	if (!psrc) {
		return 0.0f;
	}
	double mag = (double)psrc->x * (double)psrc->x + (double)psrc->y * (double)psrc->y + (double)psrc->z * (double)psrc->z;
	return mag;
}


void ChaCalcFunc::ccfChaMatrixRotationAxis(ChaMatrix* pdst, ChaVector3* srcaxis, float srcrad)
{
	if (!pdst) {
		return;
	}
	CQuaternion q;
	q.SetAxisAndRot(*srcaxis, srcrad);

	*pdst = q.MakeRotMatX();

}

void ChaCalcFunc::ccfChaMatrixScaling(ChaMatrix* pdst, float srcx, float srcy, float srcz)
{
	if (!pdst) {
		return;
	}

	pdst->data[MATI_11] = srcx;
	pdst->data[MATI_22] = srcy;
	pdst->data[MATI_33] = srcz;
}

void ChaCalcFunc::ccfChaMatrixLookAtRH(ChaMatrix* dstviewmat, ChaVector3* camEye, ChaVector3* camtar, ChaVector3* camUpVec)
{
	/*
	zaxis = normal(Eye - At)
	xaxis = normal(cross(Up, zaxis))
	yaxis = cross(zaxis, xaxis)

	xaxis.x           yaxis.x           zaxis.x          0
	xaxis.y           yaxis.y           zaxis.y          0
	xaxis.z           yaxis.z           zaxis.z          0
	-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1
	*/
	ChaVector3 zaxis;
	ChaVector3 yaxis;
	ChaVector3 xaxis;

	zaxis = *camEye - *camtar;
	ChaVector3Normalize(&zaxis, &zaxis);

	ChaVector3Cross(&xaxis, (const ChaVector3*)camUpVec, (const ChaVector3*)&zaxis);
	ChaVector3Normalize(&xaxis, &xaxis);

	ChaVector3Cross(&yaxis, (const ChaVector3*)&zaxis, (const ChaVector3*)&xaxis);
	ChaVector3Normalize(&yaxis, &yaxis);


	float trax, tray, traz;
	trax = -ChaVector3Dot(&xaxis, camEye);
	tray = -ChaVector3Dot(&yaxis, camEye);
	traz = -ChaVector3Dot(&zaxis, camEye);


	dstviewmat->data[MATI_11] = xaxis.x;
	dstviewmat->data[MATI_21] = xaxis.y;
	dstviewmat->data[MATI_31] = xaxis.z;
	dstviewmat->data[MATI_41] = trax;

	dstviewmat->data[MATI_12] = yaxis.x;
	dstviewmat->data[MATI_22] = yaxis.y;
	dstviewmat->data[MATI_32] = yaxis.z;
	dstviewmat->data[MATI_42] = tray;

	dstviewmat->data[MATI_13] = zaxis.x;
	dstviewmat->data[MATI_23] = zaxis.y;
	dstviewmat->data[MATI_33] = zaxis.z;
	dstviewmat->data[MATI_43] = traz;

	dstviewmat->data[MATI_14] = 0.0f;
	dstviewmat->data[MATI_24] = 0.0f;
	dstviewmat->data[MATI_34] = 0.0f;
	dstviewmat->data[MATI_44] = 1.0f;


}

/*
D3DXMATRIX *D3DXMatrixOrthoOffCenterRH(          D3DXMATRIX *pOut,
FLOAT l,
FLOAT r,
FLOAT t,
FLOAT b,
FLOAT zn,
FLOAT zf
);

2/(r-l)      0            0           0
0            2/(t-b)      0           0
0            0            1/(zn-zf)   0
(l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1
*/

ChaMatrix* ChaCalcFunc::ccfChaMatrixOrthoOffCenterRH(ChaMatrix* pOut, float l, float r, float t, float b, float zn, float zf)
{
	if (!pOut) {
		return NULL;
	}

	if ((r == l) || (t == b) || (zn == zf)) {
		ChaMatrixIdentity(pOut);
		return pOut;
	}


	pOut->data[MATI_11] = (float)(2.0 / ((double)r - (double)l));
	pOut->data[MATI_12] = 0.0f;
	pOut->data[MATI_13] = 0.0f;
	pOut->data[MATI_14] = 0.0f;

	pOut->data[MATI_21] = 0.0f;
	pOut->data[MATI_22] = (float)(2.0 / ((double)t - (double)b));
	pOut->data[MATI_23] = 0.0f;
	pOut->data[MATI_24] = 0.0f;

	pOut->data[MATI_31] = 0.0f;
	pOut->data[MATI_32] = 0.0f;
	pOut->data[MATI_33] = (float)(1.0 / ((double)zn - (double)zf));
	pOut->data[MATI_34] = 0.0f;

	pOut->data[MATI_41] = (float)(((double)l + (double)r) / ((double)l - (double)r));
	pOut->data[MATI_42] = (float)(((double)t + (double)b) / ((double)b - (double)t));
	pOut->data[MATI_43] = (float)((double)zn / ((double)zn - (double)zf));
	pOut->data[MATI_44] = 1.0f;

	return pOut;
}


/*
D3DXMATRIX *D3DXMatrixPerspectiveFovRH(          D3DXMATRIX *pOut,
FLOAT fovY,
FLOAT Aspect,
FLOAT zn,
FLOAT zf
);

w       0       0                0
0       h       0                0
0       0       zf/(zn-zf)      -1
0       0       zn*zf/(zn-zf)    0
where:
h is the view space height. It is calculated from
h = cot(fovY/2);

w is the view space width. It is calculated from
w = h / Aspect.
*/

ChaMatrix* ChaCalcFunc::ccfChaMatrixPerspectiveFovRH(ChaMatrix* pOut, float fovY, float Aspect, float zn, float zf)
{
	if (!pOut) {
		return NULL;
	}
	if (zn == zf) {
		ChaMatrixIdentity(pOut);
		return pOut;
	}


	double h, w;
	double c, s;
	c = cos((double)fovY / 2.0);
	s = sin((double)fovY / 2.0);
	if ((s != 0.0) && (Aspect != 0.0f)) {
		h = c / s;
		w = h / (double)Aspect;

		pOut->data[MATI_11] = (float)w;
		pOut->data[MATI_12] = 0.0f;
		pOut->data[MATI_13] = 0.0f;
		pOut->data[MATI_14] = 0.0f;

		pOut->data[MATI_21] = 0;
		pOut->data[MATI_22] = (float)h;
		pOut->data[MATI_23] = 0.0f;
		pOut->data[MATI_24] = 0.0f;

		pOut->data[MATI_31] = 0.0f;
		pOut->data[MATI_32] = 0.0f;
		pOut->data[MATI_33] = (float)((double)zf / ((double)zn - (double)zf));
		pOut->data[MATI_34] = -1.0f;

		pOut->data[MATI_41] = 0.0f;
		pOut->data[MATI_42] = 0.0f;
		pOut->data[MATI_43] = (float)((double)zn * (double)zf / ((double)zn - (double)zf));
		pOut->data[MATI_44] = 0.0f;
	}
	else {
		ChaMatrixIdentity(pOut);
	}

	return pOut;

}



const ChaMatrix* ChaCalcFunc::ccfChaMatrixRotationQuaternion(ChaMatrix* dstmat, CQuaternion* srcq)
{
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);

	if (!dstmat || !srcq) {
		return NULL;
	}

	retmat = srcq->MakeRotMatX();
	*dstmat = retmat;
	return (const ChaMatrix*)dstmat;
}

/*
pOut
[in, out] 演算結果である D3DXMATRIX 構造体へのポインタ。
Yaw
[in] y 軸を中心とするヨー(ラジアン単位)。
Pitch
[in] x 軸を中心とするピッチ(ラジアン単位)。
Roll
[in] z 軸を中心とするロール(ラジアン単位)。
戻り値

指定されたヨー･ピッチ･ロールを持つ D3DXMATRIX 構造体へのポインタ。

注意

この関数の戻り値は、pOut パラメータの戻り値と同じである。したがって、D3DXMatrixRotationYawPitchRoll 関数を別の関数の引数として使える。

トランスフォームの順序は、最初にロール、次にピッチ、最後にヨーである。これは、オブジェクトのローカル座標軸を基準として、z 軸の周囲での回転、x 軸の周囲での回転、y 軸の周囲での回転と同じになる。
*/
ChaMatrix* ChaCalcFunc::ccfChaMatrixRotationYawPitchRoll(ChaMatrix* pOut, float srcyaw, float srcpitch, float srcroll)
{
	if (!pOut) {
		return NULL;
	}

	ChaVector3 axisy;
	axisy.SetParams(0.0f, 1.0f, 0.0f);
	ChaVector3 axisx;
	axisx.SetParams(1.0f, 0.0f, 0.0f);
	ChaVector3 axisz;
	axisz.SetParams(0.0f, 0.0f, 1.0f);

	CQuaternion qyaw, qpitch, qroll;
	qyaw.SetAxisAndRot(axisy, srcyaw);
	qpitch.SetAxisAndRot(axisx, srcpitch);
	qroll.SetAxisAndRot(axisz, srcroll);

	CQuaternion qrot;
	qrot = qyaw * qpitch * qroll;

	*pOut = qrot.MakeRotMatX();
	return pOut;
}

ChaMatrix* ChaCalcFunc::ccfChaMatrixRotationX(ChaMatrix* pOut, float srcrad)
{
	if (!pOut) {
		return NULL;
	}

	ChaVector3 axisx;
	axisx.SetParams(1.0f, 0.0f, 0.0f);
	CQuaternion qx;
	qx.SetAxisAndRot(axisx, srcrad);

	*pOut = qx.MakeRotMatX();
	return pOut;
}

ChaMatrix* ChaCalcFunc::ccfChaMatrixRotationY(ChaMatrix* pOut, float srcrad)
{
	if (!pOut) {
		return NULL;
	}

	ChaVector3 axisy;
	axisy.SetParams(0.0f, 1.0f, 0.0f);
	CQuaternion qy;
	qy.SetAxisAndRot(axisy, srcrad);

	*pOut = qy.MakeRotMatX();
	return pOut;
}

ChaMatrix* ChaCalcFunc::ccfChaMatrixRotationZ(ChaMatrix* pOut, float srcrad)
{
	if (!pOut) {
		return NULL;
	}

	ChaVector3 axisz;
	axisz.SetParams(0.0f, 0.0f, 1.0f);
	CQuaternion qz;
	qz.SetAxisAndRot(axisz, srcrad);

	*pOut = qz.MakeRotMatX();
	return pOut;
}

void ChaCalcFunc::ccfCQuaternionIdentity(CQuaternion* dstq)
{
	if (!dstq) {
		return;
	}
	dstq->x = 0.0f;
	dstq->y = 0.0f;
	dstq->z = 0.0f;
	dstq->w = 1.0f;

}

CQuaternion ChaCalcFunc::ccfCQuaternionInv(CQuaternion srcq)
{
	CQuaternion invq;
	srcq.inv(&invq);
	return invq;
}


ChaMatrix ChaCalcFunc::ccfMakeRotMatFromChaMatrix(ChaMatrix srcmat)
{
	CQuaternion tmpq;
	tmpq.MakeFromD3DXMat(srcmat);
	return tmpq.MakeRotMatX();
}

ChaMatrix ChaCalcFunc::ccfChaMatrixTranspose(ChaMatrix srcmat)
{
	ChaMatrix tmpmat = srcmat;
	ChaMatrix retmat;


	retmat.data[MATI_11] = tmpmat.data[MATI_11];
	retmat.data[MATI_12] = tmpmat.data[MATI_21];
	retmat.data[MATI_13] = tmpmat.data[MATI_31];
	retmat.data[MATI_14] = tmpmat.data[MATI_41];

	retmat.data[MATI_21] = tmpmat.data[MATI_12];
	retmat.data[MATI_22] = tmpmat.data[MATI_22];
	retmat.data[MATI_23] = tmpmat.data[MATI_32];
	retmat.data[MATI_24] = tmpmat.data[MATI_42];

	retmat.data[MATI_31] = tmpmat.data[MATI_13];
	retmat.data[MATI_32] = tmpmat.data[MATI_23];
	retmat.data[MATI_33] = tmpmat.data[MATI_33];
	retmat.data[MATI_34] = tmpmat.data[MATI_43];

	retmat.data[MATI_41] = tmpmat.data[MATI_14];
	retmat.data[MATI_42] = tmpmat.data[MATI_24];
	retmat.data[MATI_43] = tmpmat.data[MATI_34];
	retmat.data[MATI_44] = tmpmat.data[MATI_44];


	return retmat;
}


ChaMatrix ChaCalcFunc::ccfCalcAxisMatX(ChaVector3 vecx, ChaVector3 srcpos, ChaMatrix srcmat)
{
	ChaMatrix retmat;
	retmat.SetIdentity();


	float leng = (float)ChaVector3LengthDbl(&vecx);
	if (leng < 0.00001f) {
		retmat.SetIdentity();
		return retmat;
	}


	ChaVector3Normalize(&vecx, &vecx);

	//###########################################################################################
	//convmatのvecxをbonevecにする　それに合わせて３軸が互いに垂直になるようにvecy, veczを求める
	//###########################################################################################
	ChaVector3 axisx = vecx;
	ChaVector3 axisy0;
	axisy0.SetParams(srcmat.data[MATI_21], srcmat.data[MATI_22], srcmat.data[MATI_23]);
	ChaVector3 axisz0;
	axisz0.SetParams(srcmat.data[MATI_31], srcmat.data[MATI_32], srcmat.data[MATI_33]);

	ChaVector3 axisy1, axisz1;
	ChaVector3Cross(&axisy1, &axisz0, &axisx);
	ChaVector3Normalize(&axisy1, &axisy1);
	ChaVector3Cross(&axisz1, &axisx, &axisy1);
	ChaVector3Normalize(&axisz1, &axisz1);

	//#####################################
	//求めた変換ベクトルで　変換行列を作成
	//#####################################
	retmat.data[MATI_11] = axisx.x;
	retmat.data[MATI_12] = axisx.y;
	retmat.data[MATI_13] = axisx.z;

	retmat.data[MATI_21] = axisy1.x;
	retmat.data[MATI_22] = axisy1.y;
	retmat.data[MATI_23] = axisy1.z;

	retmat.data[MATI_31] = axisz1.x;
	retmat.data[MATI_32] = axisz1.y;
	retmat.data[MATI_33] = axisz1.z;

	//#########################################################
	//位置は　ボーンの親の位置　つまりカレントジョイントの位置
	//#########################################################
	retmat.data[MATI_41] = srcpos.x;
	retmat.data[MATI_42] = srcpos.y;
	retmat.data[MATI_43] = srcpos.z;

	return retmat;

}


//BoneProp

double ChaCalcFunc::ccfvecDotVec(ChaVector3* vec1, ChaVector3* vec2)
{
	return ((double)vec1->x * (double)vec2->x + (double)vec1->y * (double)vec2->y + (double)vec1->z * (double)vec2->z);
}

double ChaCalcFunc::ccflengthVec(ChaVector3* vec)
{
	double mag;
	double leng;

	mag = (double)vec->x * (double)vec->x + (double)vec->y * (double)vec->y + (double)vec->z * (double)vec->z;
	if (mag == 0.0) {
		leng = 0.0;
	}
	else {
		leng = sqrt(mag);
	}
	return leng;
}

double ChaCalcFunc::ccfaCos(double dot)
{
	if (dot > 1.0)
		dot = 1.0;
	else if (dot < -1.0)
		dot = -1.0;

	double rad;
	rad = acos(dot);

	double degree;
	degree = (rad * PAI2DEG);

	return degree;
}

int ChaCalcFunc::ccfvec3RotateY(ChaVector3* dstvec, double deg, ChaVector3* srcvec)
{

	int ret;
	CQuaternion iniq;
	CQuaternion dirq;
	ChaMatrix	dirm;

	ret = dirq.SetRotationXYZ(&iniq, 0, deg, 0);
	_ASSERT(!ret);
	dirm = dirq.MakeRotMatX();

	ChaVector3 tmpsrcvec = *srcvec;//dstvecとsrcvecが同一ポインタの場合に対応

	ChaVector3TransformCoord(dstvec, &tmpsrcvec, &dirm);
	//dstvec->x = (float)((double)dirm.data[MATI_11] * (double)tmpsrcvec.x + (double)dirm.data[MATI_21] * (double)tmpsrcvec.y + (double)dirm.data[MATI_31] * (double)tmpsrcvec.z + (double)dirm.data[MATI_41]);
	//dstvec->y = (float)((double)dirm.data[MATI_12] * (double)tmpsrcvec.x + (double)dirm.data[MATI_22] * (double)tmpsrcvec.y + (double)dirm.data[MATI_32] * (double)tmpsrcvec.z + (double)dirm.data[MATI_42]);
	//dstvec->z = (float)((double)dirm.data[MATI_13] * (double)tmpsrcvec.x + (double)dirm.data[MATI_23] * (double)tmpsrcvec.y + (double)dirm.data[MATI_33] * (double)tmpsrcvec.z + (double)dirm.data[MATI_43]);

	return 0;
}
int ChaCalcFunc::ccfvec3RotateX(ChaVector3* dstvec, double deg, ChaVector3* srcvec)
{

	int ret;
	CQuaternion iniq;
	CQuaternion dirq;
	ChaMatrix	dirm;

	ret = dirq.SetRotationXYZ(&iniq, deg, 0, 0);
	_ASSERT(!ret);
	dirm = dirq.MakeRotMatX();

	ChaVector3 tmpsrcvec = *srcvec;//dstvecとsrcvecが同一ポインタの場合に対応

	ChaVector3TransformCoord(dstvec, &tmpsrcvec, &dirm);
	//dstvec->x = (float)((double)dirm.data[MATI_11] * (double)tmpsrcvec.x + (double)dirm.data[MATI_21] * (double)tmpsrcvec.y + (double)dirm.data[MATI_31] * (double)tmpsrcvec.z + (double)dirm.data[MATI_41]);
	//dstvec->y = (float)((double)dirm.data[MATI_12] * (double)tmpsrcvec.x + (double)dirm.data[MATI_22] * (double)tmpsrcvec.y + (double)dirm.data[MATI_32] * (double)tmpsrcvec.z + (double)dirm.data[MATI_42]);
	//dstvec->z = (float)((double)dirm.data[MATI_13] * (double)tmpsrcvec.x + (double)dirm.data[MATI_23] * (double)tmpsrcvec.y + (double)dirm.data[MATI_33] * (double)tmpsrcvec.z + (double)dirm.data[MATI_43]);


	return 0;
}
int ChaCalcFunc::ccfvec3RotateZ(ChaVector3* dstvec, double deg, ChaVector3* srcvec)
{

	int ret;
	CQuaternion iniq;
	CQuaternion dirq;
	ChaMatrix	dirm;

	ret = dirq.SetRotationXYZ(&iniq, 0, 0, deg);
	_ASSERT(!ret);
	dirm = dirq.MakeRotMatX();

	ChaVector3 tmpsrcvec = *srcvec;//dstvecとsrcvecが同一ポインタの場合に対応

	ChaVector3TransformCoord(dstvec, &tmpsrcvec, &dirm);
	//dstvec->x = (float)((double)dirm.data[MATI_11] * (double)tmpsrcvec.x + (double)dirm.data[MATI_21] * (double)tmpsrcvec.y + (double)dirm.data[MATI_31] * (double)tmpsrcvec.z + (double)dirm.data[MATI_41]);
	//dstvec->y = (float)((double)dirm.data[MATI_12] * (double)tmpsrcvec.x + (double)dirm.data[MATI_22] * (double)tmpsrcvec.y + (double)dirm.data[MATI_32] * (double)tmpsrcvec.z + (double)dirm.data[MATI_42]);
	//dstvec->z = (float)((double)dirm.data[MATI_13] * (double)tmpsrcvec.x + (double)dirm.data[MATI_23] * (double)tmpsrcvec.y + (double)dirm.data[MATI_33] * (double)tmpsrcvec.z + (double)dirm.data[MATI_43]);

	return 0;
}


//int qToEulerAxis(CQuaternion axisQ, CQuaternion* srcq, ChaVector3* Euler)
//{
//
//	int ret;
//	CQuaternion invaxisQ, EQ;
//	ret = axisQ.inv(&invaxisQ);
//	_ASSERT(!ret);
//	EQ = invaxisQ * *srcq * axisQ;
//
//
//	ChaVector3 axisXVec(1.0f, 0.0f, 0.0f);
//	ChaVector3 axisYVec(0.0f, 1.0f, 0.0f);
//	ChaVector3 axisZVec(0.0f, 0.0f, 1.0f);
//
//	ChaVector3 targetVec, shadowVec;
//	ChaVector3 tmpVec;
//
//	EQ.Rotate(&targetVec, axisZVec);
//	shadowVec.x = (float)vecDotVec(&targetVec, &axisXVec);
//	shadowVec.y = 0.0f;
//	shadowVec.z = (float)vecDotVec(&targetVec, &axisZVec);
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler->y = 90.0f;
//	}
//	else {
//		Euler->y = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisXVec) < 0.0) {
//		Euler->y = -Euler->y;
//	}
//
//	/***
//	E3DMultQVec targetQ,0.0,0.0,1.0,targetVec.0,targetVec.1,targetVec.2
//	shadowVec.0= vecDotVec(targetVec,axisXVec)
//	shadowVec.1= 0.0
//	shadowVec.2= vecDotVec(targetVec,axisZVec)
//	if( lengthVec(shadowVec) == 0 ){
//	eulerY= 90.0
//	}else{
//	E3DACos ( vecDotVec(shadowVec,axisZVec)/ lengthVec(shadowVec) ), eulerY		//Y軸回転度数
//	}
//	if( vecDotVec(shadowVec,axisXVec) < 0.0 ){ eulerY= -eulerY }				//右回りに修正
//	***/
//	vec3RotateY(&tmpVec, -Euler->y, &targetVec);
//	shadowVec.x = 0.0f;
//	shadowVec.y = (float)vecDotVec(&tmpVec, &axisYVec);
//	shadowVec.z = (float)vecDotVec(&tmpVec, &axisZVec);
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler->x = 90.0f;
//	}
//	else {
//		Euler->x = (float)aCos(vecDotVec(&shadowVec, &axisZVec) / lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisYVec) > 0.0) {
//		Euler->x = -Euler->x;
//	}
//	/***
//	E3DVec3RotateY targetVec.0,targetVec.1,targetVec.2,(-eulerY),shadowVec.0,shadowVec.1,shadowVec.2
//	if( lengthVec(shadowVec) == 0 ){
//	eulerX= 90.0
//	}else{
//	E3DACos ( vecDotVec(shadowVec,axisZVec)/ lengthVec(shadowVec) ), eulerX		//X軸回転度数
//	}
//	if( vecDotVec(shadowVec,axisYVec) > 0.0 ){ eulerX= -eulerX }				//右回りに修正
//	***/
//
//	EQ.Rotate(&targetVec, axisYVec);
//	vec3RotateY(&tmpVec, -Euler->y, &targetVec);
//	targetVec = tmpVec;
//	vec3RotateX(&tmpVec, -Euler->x, &targetVec);
//	shadowVec.x = (float)vecDotVec(&tmpVec, &axisXVec);
//	shadowVec.y = (float)vecDotVec(&tmpVec, &axisYVec);
//	shadowVec.z = 0.0f;
//	if (lengthVec(&shadowVec) == 0.0) {
//		Euler->z = 90.0f;
//	}
//	else {
//		Euler->z = (float)aCos(vecDotVec(&shadowVec, &axisYVec) / lengthVec(&shadowVec));
//	}
//	if (vecDotVec(&shadowVec, &axisXVec) > 0.0) {
//		Euler->z = -Euler->z;
//	}
//	/***
//	E3DMultQVec targetQ,0.0,1.0,0.0,targetVec.0,targetVec.1,targetVec.2
//	E3DVec3RotateY (targetVec.0+0.0),(targetVec.1+0.0),(targetVec.2+0.0),(-eulerY),targetVec.0,targetVec.1,targetVec.2
//	E3DVec3RotateY targetVec.2,targetVec.0,targetVec.1,(-eulerX),shadowVec.2,shadowVec.0,shadowVec.1
//	　　----> 引数がZ,X,YになっているためRotateXと同じ！！！
//
//	  if( lengthVec(shadowVec) == 0 ){
//	  eulerZ= 90.0
//	  }else{
//	  E3DACos ( vecDotVec(shadowVec,axisYVec)/ lengthVec(shadowVec) ), eulerZ		//Z軸回転度数
//	  }
//	  if( vecDotVec(shadowVec,axisXVec) > 0.0 ){ eulerZ= -eulerZ }				//右回りに修正
//
//	  ***/
//	return 0;
//}



//int GetRound(float srcval)
//{
//	//if (srcval > 0.0f) {
//	//	return (int)(srcval + 0.5f);
//	//}
//	//else {
//	//	return (int)(srcval - 0.5f);
//	//}
//
//
//}

//int modifyEuler(ChaVector3* eulerA, ChaVector3* eulerB)
//{
//
//	//オイラー角Aの値をオイラー角Bの値に近い表示に修正
//	float tmpX1, tmpY1, tmpZ1;
//	float tmpX2, tmpY2, tmpZ2;
//	float s1, s2;
//
//	//予想される角度1
//	tmpX1 = eulerA->x + 360.0f * GetRound((eulerB->x - eulerA->x) / 360.0f);
//	tmpY1 = eulerA->y + 360.0f * GetRound((eulerB->y - eulerA->y) / 360.0f);
//	tmpZ1 = eulerA->z + 360.0f * GetRound((eulerB->z - eulerA->z) / 360.0f);
//
//	//予想される角度2
//	tmpX2 = 180.0f - eulerA->x + 360.0f * GetRound((eulerB->x + eulerA->x - 180.0f) / 360.0f);
//	tmpY2 = eulerA->y + 180.0f + 360.0f * GetRound((eulerB->y - eulerA->y - 180.0f) / 360.0f);
//	tmpZ2 = eulerA->z + 180.0f + 360.0f * GetRound((eulerB->z - eulerA->z - 180.0f) / 360.0f);
//
//	//角度変化の大きさ
//	s1 = (eulerB->x - tmpX1) * (eulerB->x - tmpX1) + (eulerB->y - tmpY1) * (eulerB->y - tmpY1) + (eulerB->z - tmpZ1) * (eulerB->z - tmpZ1);
//	s2 = (eulerB->x - tmpX2) * (eulerB->x - tmpX2) + (eulerB->y - tmpY2) * (eulerB->y - tmpY2) + (eulerB->z - tmpZ2) * (eulerB->z - tmpZ2);
//
//	//変化の少ない方に修正
//	if (s1 < s2) {
//		eulerA->x = tmpX1; eulerA->y = tmpY1; eulerA->z = tmpZ1;
//	}
//	else {
//		eulerA->x = tmpX2; eulerA->y = tmpY2; eulerA->z = tmpZ2;
//	}
//
//	return 0;
//}

int ChaCalcFunc::ccfIsInitRot(ChaMatrix srcmat)
{
	int retval = 0;

	float d11, d12, d13;
	float d21, d22, d23;
	float d31, d32, d33;

	d11 = srcmat.data[MATI_11] - 1.0f;
	d12 = srcmat.data[MATI_12] - 0.0f;
	d13 = srcmat.data[MATI_13] - 0.0f;

	d21 = srcmat.data[MATI_21] - 0.0f;
	d22 = srcmat.data[MATI_22] - 1.0f;
	d23 = srcmat.data[MATI_23] - 0.0f;

	d31 = srcmat.data[MATI_31] - 0.0f;
	d32 = srcmat.data[MATI_32] - 0.0f;
	d33 = srcmat.data[MATI_33] - 1.0f;

	//float dmin = 0.000001f;
	float dmin = 0.000010f;

	if ((fabs(d11) <= dmin) && (fabs(d12) <= dmin) && (fabs(d13) <= dmin) &&
		(fabs(d21) <= dmin) && (fabs(d22) <= dmin) && (fabs(d23) <= dmin) &&
		(fabs(d31) <= dmin) && (fabs(d32) <= dmin) && (fabs(d33) <= dmin)) {
		retval = 1;
	}

	return retval;
}

int ChaCalcFunc::ccfIsInitMat(ChaMatrix srcmat)
{
	ChaMatrix mat1;
	mat1.SetIdentity();
	return IsSameMat(srcmat, mat1);
}

int ChaCalcFunc::ccfIsSameMat(ChaMatrix srcmat1, ChaMatrix srcmat2)
{

	int retval = 0;
	ChaMatrix diffmat;
	diffmat = srcmat1 - srcmat2;

	//float dmin = 0.000001f;
	float dmin = 0.000010f;

	if ((fabs(diffmat.data[MATI_11]) <= dmin) && (fabs(diffmat.data[MATI_12]) <= dmin) && (fabs(diffmat.data[MATI_13]) <= dmin) && (fabs(diffmat.data[MATI_14]) <= dmin) &&
		(fabs(diffmat.data[MATI_21]) <= dmin) && (fabs(diffmat.data[MATI_22]) <= dmin) && (fabs(diffmat.data[MATI_23]) <= dmin) && (fabs(diffmat.data[MATI_24]) <= dmin) &&
		(fabs(diffmat.data[MATI_31]) <= dmin) && (fabs(diffmat.data[MATI_32]) <= dmin) && (fabs(diffmat.data[MATI_33]) <= dmin) && (fabs(diffmat.data[MATI_34]) <= dmin) &&
		(fabs(diffmat.data[MATI_41]) <= dmin) && (fabs(diffmat.data[MATI_42]) <= dmin) && (fabs(diffmat.data[MATI_43]) <= dmin) && (fabs(diffmat.data[MATI_44]) <= dmin)
		) {
		retval = 1;
	}

	return retval;
}

int ChaCalcFunc::ccfIsSameEul(ChaVector3 srceul1, ChaVector3 srceul2)
{
	int retval = 0;

	ChaVector3 diffeul;
	diffeul = srceul1 - srceul2;

	//float dmin = 0.000015f;
	//float dmin = 0.00010f;//(degree)
	//float dmin = 0.00050f;//(degree)
	//float dmin = 0.000010f;//(degree)
	float dmin = 0.10f;//(degree)

	if ((fabs(diffeul.x) <= dmin) && (fabs(diffeul.y) <= dmin) && (fabs(diffeul.z) <= dmin)) {
		retval = 1;
	}

	return retval;
}

//void SetAngleLimitOff(ANGLELIMIT* dstal)
//{
//	if (dstal) {
//		int axiskind;
//		for (axiskind = AXIS_X; axiskind < AXIS_MAX; axiskind++) {
//			if ((dstal->lower[axiskind] == -180) && (dstal->upper[axiskind] == 180)) {
//				dstal->limitoff[axiskind] = 1;
//			}
//			else {
//				dstal->limitoff[axiskind] = 0;
//			}
//		}
//	}
//}
//void InitAngleLimit(ANGLELIMIT* dstal)
//{
//	dstal->boneaxiskind = BONEAXIS_CURRENT;
//
//	if (dstal) {
//		int axiskind;
//		for (axiskind = AXIS_X; axiskind < AXIS_MAX; axiskind++) {
//			dstal->limitoff[axiskind] = 0;
//			dstal->via180flag[axiskind] = 0;
//			dstal->lower[axiskind] = -180;
//			dstal->upper[axiskind] = 180;
//		}
//		SetAngleLimitOff(dstal);
//	}
//}



bool ChaCalcFunc::ccfIsJustEqualTime(double srctime1, double srctime2)
{
	double difftime = srctime1 - srctime2;
	if ((difftime >= -0.0001) && (difftime <= 0.0001)) {
		return true;
	}
	else {
		return false;
	}
}

double ChaCalcFunc::ccfRoundingTime(double srctime)
{
	return (double)((int)(srctime + 0.0001));
}
int ChaCalcFunc::ccfIntTime(double srctime)
{
	return (int)(srctime + 0.0001);
}

bool ChaCalcFunc::ccfIsEqualRoundingTime(double srctime1, double srctime2)
{
	return (RoundingTime(srctime1) == RoundingTime(srctime2));
}



double ChaCalcFunc::ccfVecLength(ChaVector3 srcvec)
{
	double tmpval = (double)srcvec.x * (double)srcvec.x + (double)srcvec.y * (double)srcvec.y + (double)srcvec.z * (double)srcvec.z;
	if (tmpval > 0.0) {
		return sqrt(tmpval);
	}
	else {
		return 0.0;
	}
}



void ChaCalcFunc::ccfGetSRTMatrix(ChaMatrix srcmat, ChaVector3* svecptr, ChaMatrix* rmatptr, ChaVector3* tvecptr)
{
	if (!svecptr || !rmatptr || !tvecptr) {
		_ASSERT(0);
		return;
	}

	svecptr->SetParams(1.0f, 1.0f, 1.0f);
	ChaMatrixIdentity(rmatptr);
	tvecptr->SetParams(0.0f, 0.0f, 0.0f);

	ChaMatrix tmpmat1 = srcmat;

	*tvecptr = tmpmat1.GetTranslation();

	tmpmat1.SetTranslationZero();

	ChaVector3 vec1, vec2, vec3;
	vec1 = tmpmat1.GetRow(0);
	vec2 = tmpmat1.GetRow(1);
	vec3 = tmpmat1.GetRow(2);

	double len1, len2, len3;
	len1 = VecLength(vec1);
	len2 = VecLength(vec2);
	len3 = VecLength(vec3);

	//次のコメントアウトコードでは縮小出来ない
	//if ((fabs(len1) - 1.0) < 0.0001) {
	//	len1 = 1.0;//誤差で急に大きさが変わるのを防止
	//}
	//if (fabs(len1) < 0.0001) {
	//	len1 = 0.0001;//0scale禁止
	//}
	//if ((fabs(len2) - 1.0) < 0.0001) {
	//	len2 = 1.0;//誤差で急に大きさが変わるのを防止
	//}
	//if (fabs(len2) < 0.0001) {
	//	len2 = 0.0001;//0scale禁止
	//}
	//if ((fabs(len3) - 1.0) < 0.0001) {
	//	len3 = 1.0;//誤差で急に大きさが変わるのを防止
	//}
	//if (fabs(len3) < 0.0001) {
	//	len3 = 0.0001;//0scale禁止
	//}


	//2021/12/01
	//fabs : 2023/04/11
	if (fabs(len1 - 1.0) < 0.00001) {
		len1 = 1.0;//誤差で急に大きさが変わるのを防止
	}
	if (fabs(len1) < 0.00001) {
		if (len1 >= 0.0f) {
			len1 = 0.00001;//0scale禁止
		}
		else {
			len1 = -0.00001;//0scale禁止
		}
	}
	if (fabs(len2 - 1.0) < 0.00001) {
		len2 = 1.0;//誤差で急に大きさが変わるのを防止
	}
	if (fabs(len2) < 0.00001) {
		if (len2 >= 0.0f) {
			len2 = 0.00001;//0scale禁止
		}
		else {
			len2 = -0.00001;//0scale禁止
		}
	}
	if (fabs(len3 - 1.0) < 0.00001) {
		len3 = 1.0;//誤差で急に大きさが変わるのを防止
	}
	if (fabs(len3) < 0.00001) {
		if (len3 >= 0.0f) {
			len3 = 0.00001;//0scale禁止
		}
		else {
			len3 = -0.00001;//0scale禁止
		}
	}


	svecptr->x = (float)len1;
	svecptr->y = (float)len2;
	svecptr->z = (float)len3;

	if (len1 != 0.0) {
		rmatptr->data[MATI_11] = (float)((double)tmpmat1.data[MATI_11] / len1);
		rmatptr->data[MATI_12] = (float)((double)tmpmat1.data[MATI_12] / len1);
		rmatptr->data[MATI_13] = (float)((double)tmpmat1.data[MATI_13] / len1);

		//tvecptr->x /= len1;
	}
	else {
		rmatptr->data[MATI_11] = 1.0f;
		rmatptr->data[MATI_12] = 0.0f;
		rmatptr->data[MATI_13] = 0.0f;

		//tvecptr->x = 0.0f;
	}

	if (len2 != 0.0f) {
		rmatptr->data[MATI_21] = (float)((double)tmpmat1.data[MATI_21] / len2);
		rmatptr->data[MATI_22] = (float)((double)tmpmat1.data[MATI_22] / len2);
		rmatptr->data[MATI_23] = (float)((double)tmpmat1.data[MATI_23] / len2);

		//tvecptr->y /= len2;
	}
	else {
		rmatptr->data[MATI_21] = 0.0f;
		rmatptr->data[MATI_22] = 1.0f;
		rmatptr->data[MATI_23] = 0.0f;

		//tvecptr->y = 0.0f;
	}

	if (len3 != 0.0f) {
		rmatptr->data[MATI_31] = (float)((double)tmpmat1.data[MATI_31] / len3);
		rmatptr->data[MATI_32] = (float)((double)tmpmat1.data[MATI_32] / len3);
		rmatptr->data[MATI_33] = (float)((double)tmpmat1.data[MATI_33] / len3);

		//tvecptr->z /= len3;
	}
	else {
		rmatptr->data[MATI_31] = 0.0f;
		rmatptr->data[MATI_32] = 0.0f;
		rmatptr->data[MATI_33] = 1.0f;

		//tvecptr->z = 0.0f;
	}


}

void ChaCalcFunc::ccfGetSRTMatrix2(ChaMatrix srcmat, ChaMatrix* smatptr, ChaMatrix* rmatptr, ChaMatrix* tmatptr)
{
	if (!smatptr || !rmatptr || !tmatptr) {
		return;
	}

	ChaMatrix smat, rmat, tmat;
	ChaMatrixIdentity(&smat);
	ChaMatrixIdentity(&rmat);
	ChaMatrixIdentity(&tmat);

	ChaVector3 svec;
	svec.SetParams(0.0, 0.0, 0.0);
	ChaVector3 tvec;
	tvec.SetParams(0.0, 0.0, 0.0);

	GetSRTMatrix(srcmat, &svec, &rmat, &tvec);

	ChaMatrixScaling(&smat, svec.x, svec.y, svec.z);
	ChaMatrixTranslation(&tmat, tvec.x, tvec.y, tvec.z);

	*smatptr = smat;
	*rmatptr = rmat;
	*tmatptr = tmat;
}

void ChaCalcFunc::ccfGetSRTandTraAnim(ChaMatrix srcmat, ChaMatrix srcnodemat, ChaMatrix* smatptr, ChaMatrix* rmatptr, ChaMatrix* tmatptr, ChaMatrix* tanimmatptr)
{
	//###################
	//For Local Posture
	//###################

	if (!smatptr || !rmatptr || !tmatptr || !tanimmatptr) {
		_ASSERT(0);
		return;
	}

	ChaVector3 jointfpos;
	jointfpos.SetParams(0.0f, 0.0f, 0.0f);
	jointfpos = ChaMatrixTraVec(srcnodemat);

	ChaMatrix befrotmat, aftrotmat;
	befrotmat.SetIdentity();
	aftrotmat.SetIdentity();
	befrotmat.SetTranslation(-jointfpos);
	aftrotmat.SetTranslation(jointfpos);

	GetSRTMatrix2(srcmat, smatptr, rmatptr, tmatptr);

	ChaMatrix matwithouttraanim;
	matwithouttraanim = befrotmat * *smatptr * *rmatptr * aftrotmat;

	ChaVector3 srpos, localpos;
	ChaVector3TransformCoord(&srpos, &jointfpos, &matwithouttraanim);
	ChaVector3TransformCoord(&localpos, &jointfpos, &srcmat);
	ChaVector3 traanim;
	traanim = localpos - srpos;

	tanimmatptr->SetIdentity();
	ChaMatrixTranslation(tanimmatptr, traanim.x, traanim.y, traanim.z);

	return;
}

ChaMatrix ChaCalcFunc::ccfChaMatrixFromSRT(bool sflag, bool tflag, ChaMatrix srcnodemat, ChaMatrix* srcsmat, ChaMatrix* srcrmat, ChaMatrix* srctmat)
{
	//###################
	//For Local Posture
	//###################

	ChaMatrix retmat;
	retmat.SetIdentity();

	if (!srcrmat) {
		_ASSERT(0);
		return retmat;
	}
	if (sflag && !srcsmat) {
		_ASSERT(0);
		return retmat;
	}
	if (tflag && !srctmat) {
		_ASSERT(0);
		return retmat;
	}

	ChaVector3 jointfpos;
	jointfpos.SetParams(0.0f, 0.0f, 0.0f);
	jointfpos = ChaMatrixTraVec(srcnodemat);

	ChaMatrix befrotmat, aftrotmat;
	befrotmat.SetIdentity();
	aftrotmat.SetIdentity();
	befrotmat.SetTranslation(-jointfpos);
	aftrotmat.SetTranslation(jointfpos);

	//calc local srt matrix
	if (sflag == true) {
		if (tflag == true) {
			retmat = befrotmat * *srcsmat * *srcrmat * aftrotmat;
			//retmat = *srcsmat * *srcrmat * *srctmat;
			retmat.data[MATI_41] = srctmat->data[MATI_41];
			retmat.data[MATI_42] = srctmat->data[MATI_42];
			retmat.data[MATI_43] = srctmat->data[MATI_43];
		}
		else {
			retmat = befrotmat * *srcsmat * *srcrmat * aftrotmat;
		}
	}
	else {
		if (tflag == true) {
			retmat = befrotmat * *srcrmat * aftrotmat;
			//retmat = *srcrmat * *srctmat;
			retmat.data[MATI_41] = srctmat->data[MATI_41];
			retmat.data[MATI_42] = srctmat->data[MATI_42];
			retmat.data[MATI_43] = srctmat->data[MATI_43];
		}
		else {
			retmat = befrotmat * *srcrmat * aftrotmat;
		}
	}

	return retmat;

}

ChaMatrix ChaCalcFunc::ccfChaMatrixKeepScale(ChaMatrix srcmat, ChaVector3 srcsvec)
{
	ChaVector3 vecx, vecy, vecz;
	double lenx, leny, lenz;
	ChaVector3 vecx0, vecy0, vecz0;
	ChaVector3 keepvecx, keepvecy, keepvecz;

	vecx = srcmat.GetRow(0);
	vecy = srcmat.GetRow(1);
	vecz = srcmat.GetRow(2);

	lenx = ChaVector3LengthDbl(&vecx);
	leny = ChaVector3LengthDbl(&vecy);
	lenz = ChaVector3LengthDbl(&vecz);
	if (lenx >= 0.000001f) {
		vecx0 = vecx / lenx;
	}
	else {
		vecx0.SetParams(1.0f, 0.0f, 0.0f);//!!!
	}
	if (leny >= 0.000001f) {
		vecy0 = vecy / leny;
	}
	else {
		vecy0.SetParams(0.0f, 1.0f, 0.0f);//!!!
	}
	if (lenz >= 0.000001f) {
		vecz0 = vecz / lenz;
	}
	else {
		vecz0.SetParams(0.0f, 0.0f, 1.0f);//!!!
	}



	if (((srcsvec.x - 1.0f) > -0.00003f) && ((srcsvec.x - 1.0f) < 0.00003f)) {
		srcsvec.x = 1.0f;//誤差で急に大きさが変わるのを防止
	}
	if (srcsvec.x < 0.00003f) {
		srcsvec.x = 0.000030f;//0scale禁止
	}
	if (((srcsvec.y - 1.0f) > -0.00003f) && ((srcsvec.y - 1.0f) < 0.00003f)) {
		srcsvec.y = 1.0f;//誤差で急に大きさが変わるのを防止
	}
	if (srcsvec.y < 0.00003f) {
		srcsvec.y = 0.000030f;//0scale禁止
	}
	if (((srcsvec.z - 1.0f) > -0.00003f) && ((srcsvec.z - 1.0f) < 0.00003f)) {
		srcsvec.z = 1.0f;//誤差で急に大きさが変わるのを防止
	}
	if (srcsvec.z < 0.00003f) {
		srcsvec.z = 0.000030f;//0scale禁止
	}
	keepvecx = vecx0 * srcsvec.x;
	keepvecy = vecy0 * srcsvec.y;
	keepvecz = vecz0 * srcsvec.z;

	ChaMatrix retmat;
	retmat = srcmat;
	retmat.SetRow(0, keepvecx);
	retmat.SetRow(1, keepvecy);
	retmat.SetRow(2, keepvecz);

	return retmat;
}

ChaMatrix ChaCalcFunc::ccfChaMatrixFromSRTraAnim(bool sflag, bool tanimflag, ChaMatrix srcnodemat, ChaMatrix* srcsmat, ChaMatrix* srcrmat, ChaMatrix* srctanimmat)
{
	//###################
	//For Local Posture
	//###################

	ChaMatrix retmat;
	retmat.SetIdentity();

	if (!srcrmat) {
		_ASSERT(0);
		return retmat;
	}
	if (sflag && !srcsmat) {
		_ASSERT(0);
		return retmat;
	}
	if (tanimflag && !srctanimmat) {
		_ASSERT(0);
		return retmat;
	}

	ChaVector3 jointfpos;
	jointfpos.SetParams(0.0f, 0.0f, 0.0f);
	jointfpos = ChaMatrixTraVec(srcnodemat);

	ChaMatrix befrotmat, aftrotmat;
	befrotmat.SetIdentity();
	aftrotmat.SetIdentity();
	befrotmat.SetTranslation(-jointfpos);
	aftrotmat.SetTranslation(jointfpos);

	//calc local srt matrix
	if (sflag == true) {
		ChaMatrix tmpmat;
		tmpmat.SetIdentity();
		if (tanimflag == true) {
			tmpmat = befrotmat * *srcsmat * *srcrmat * aftrotmat * *srctanimmat;
		}
		else {
			tmpmat = befrotmat * *srcsmat * *srcrmat * aftrotmat;
		}

		//2023/02/12 スケール誤差によるグラフギザギザ対策
		ChaVector3 keepscale;
		keepscale.x = srcsmat->data[MATI_11];
		keepscale.y = srcsmat->data[MATI_22];
		keepscale.z = srcsmat->data[MATI_33];
		retmat = ChaMatrixKeepScale(tmpmat, keepscale);

	}
	else {
		ChaMatrix tmpmat;
		tmpmat.SetIdentity();
		if (tanimflag == true) {
			tmpmat = befrotmat * *srcrmat * aftrotmat * *srctanimmat;
		}
		else {
			tmpmat = befrotmat * *srcrmat * aftrotmat;
		}

		//2023/02/12 スケール誤差によるグラフギザギザ対策
		ChaVector3 keepscale;
		keepscale.SetParams(1.0f, 1.0f, 1.0f);
		retmat = ChaMatrixKeepScale(tmpmat, keepscale);
	}

	return retmat;

}

ChaMatrix ChaCalcFunc::ccfGetS0RTMatrix(ChaMatrix srcmat)
{
	//拡大縮小を初期化したRT行列を返す
	ChaMatrix retm;
	ChaVector3 svec, tvec;
	ChaMatrix rmat, tmat;
	GetSRTMatrix(srcmat, &svec, &rmat, &tvec);

	retm = srcmat;

	if (svec.x != 0.0f) {
		retm.data[MATI_11] = (float)((double)retm.data[MATI_11] / (double)svec.x);
		retm.data[MATI_12] = (float)((double)retm.data[MATI_12] / (double)svec.x);
		retm.data[MATI_13] = (float)((double)retm.data[MATI_13] / (double)svec.x);
	}
	else {
		retm.data[MATI_11] = 1.0f;
		retm.data[MATI_12] = 0.0f;
		retm.data[MATI_13] = 0.0f;
	}

	if (svec.y != 0.0f) {
		retm.data[MATI_21] = (float)((double)retm.data[MATI_21] / (double)svec.y);
		retm.data[MATI_22] = (float)((double)retm.data[MATI_22] / (double)svec.y);
		retm.data[MATI_23] = (float)((double)retm.data[MATI_23] / (double)svec.y);
	}
	else {
		retm.data[MATI_21] = 0.0f;
		retm.data[MATI_22] = 1.0f;
		retm.data[MATI_23] = 0.0f;
	}

	if (svec.z != 0.0f) {
		retm.data[MATI_31] = (float)((double)retm.data[MATI_31] / (double)svec.z);
		retm.data[MATI_32] = (float)((double)retm.data[MATI_32] / (double)svec.z);
		retm.data[MATI_33] = (float)((double)retm.data[MATI_33] / (double)svec.z);
	}
	else {
		retm.data[MATI_31] = 0.0f;
		retm.data[MATI_32] = 0.0f;
		retm.data[MATI_33] = 1.0f;
	}

	return retm;
}




ChaMatrix ChaCalcFunc::ccfTransZeroMat(ChaMatrix srcmat)
{
	ChaMatrix retmat;
	retmat = srcmat;
	retmat.data[MATI_41] = 0.0f;
	retmat.data[MATI_42] = 0.0f;
	retmat.data[MATI_43] = 0.0f;

	return retmat;
}

ChaMatrix ChaCalcFunc::ccfChaMatrixFromFbxAMatrix(FbxAMatrix srcmat)
{
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);

	retmat.data[MATI_11] = (float)srcmat.Get(0, 0);
	retmat.data[MATI_12] = (float)srcmat.Get(0, 1);
	retmat.data[MATI_13] = (float)srcmat.Get(0, 2);
	retmat.data[MATI_14] = (float)srcmat.Get(0, 3);

	retmat.data[MATI_21] = (float)srcmat.Get(1, 0);
	retmat.data[MATI_22] = (float)srcmat.Get(1, 1);
	retmat.data[MATI_23] = (float)srcmat.Get(1, 2);
	retmat.data[MATI_24] = (float)srcmat.Get(1, 3);

	retmat.data[MATI_31] = (float)srcmat.Get(2, 0);
	retmat.data[MATI_32] = (float)srcmat.Get(2, 1);
	retmat.data[MATI_33] = (float)srcmat.Get(2, 2);
	retmat.data[MATI_34] = (float)srcmat.Get(2, 3);

	retmat.data[MATI_41] = (float)srcmat.Get(3, 0);
	retmat.data[MATI_42] = (float)srcmat.Get(3, 1);
	retmat.data[MATI_43] = (float)srcmat.Get(3, 2);
	retmat.data[MATI_44] = (float)srcmat.Get(3, 3);

	return retmat;

}

ChaMatrix ChaCalcFunc::ccfChaMatrixFromBtMat3x3(btMatrix3x3* srcmat3x3)
{
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);
	if (!srcmat3x3) {
		return retmat;
	}

	btVector3 tmpcol[3];
	int colno;
	for (colno = 0; colno < 3; colno++) {
		tmpcol[colno] = srcmat3x3->getColumn(colno);
		//tmprow[rowno] = srcmat3x3->getRow(rowno);
	}
	//##############################
	//ChaMatrixのrowはbtMatrixのcol
	//##############################
	retmat.data[MATI_11] = tmpcol[0].x();
	retmat.data[MATI_12] = tmpcol[0].y();
	retmat.data[MATI_13] = tmpcol[0].z();
	retmat.data[MATI_21] = tmpcol[1].x();
	retmat.data[MATI_22] = tmpcol[1].y();
	retmat.data[MATI_23] = tmpcol[1].z();
	retmat.data[MATI_31] = tmpcol[2].x();
	retmat.data[MATI_32] = tmpcol[2].y();
	retmat.data[MATI_33] = tmpcol[2].z();

	return retmat;
}

ChaMatrix ChaCalcFunc::ccfChaMatrixFromBtTransform(btMatrix3x3* srcmat3x3, btVector3* srcpivot)
{
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);

	btVector3 tmpcol[3];
	int colno;
	for (colno = 0; colno < 3; colno++) {
		tmpcol[colno] = srcmat3x3->getColumn(colno);
		//tmprow[rowno] = srcmat3x3->getRow(rowno);
	}
	//##############################
	//ChaMatrixのrowはbtMatrixのcol
	//##############################
	retmat.data[MATI_11] = tmpcol[0].x();
	retmat.data[MATI_12] = tmpcol[0].y();
	retmat.data[MATI_13] = tmpcol[0].z();
	retmat.data[MATI_21] = tmpcol[1].x();
	retmat.data[MATI_22] = tmpcol[1].y();
	retmat.data[MATI_23] = tmpcol[1].z();
	retmat.data[MATI_31] = tmpcol[2].x();
	retmat.data[MATI_32] = tmpcol[2].y();
	retmat.data[MATI_33] = tmpcol[2].z();


	retmat.data[MATI_41] = srcpivot->x();
	retmat.data[MATI_42] = srcpivot->y();
	retmat.data[MATI_43] = srcpivot->z();

	return retmat;
}

ChaMatrix ChaCalcFunc::ccfChaMatrixInv(ChaMatrix srcmat)
{
	ChaMatrix retmat;
	retmat.SetIdentity();
	ChaMatrixInverse(&retmat, NULL, &srcmat);
	return retmat;
}



#ifndef MOTIONPOINTH
#define MOTIONPOINTH

#include <coef.h>
#include <ChaVecCalc.h>
//#include <quaternion.h>

#define MPPOOLBLKLEN	65536

//#define MPPOOLBLKLEN	2048
////#define MPPOOLBLKLEN	8192
////#define MPPOOLBLKLEN	32768

class CBone;


class CMotionPoint
{
public:
	CMotionPoint();
	~CMotionPoint();

	int InitParams();

	//int GetEul( ChaVector3* dsteul );
	//int SetEul( CQuaternion* axisq, ChaVector3 srceul );
	//int SetQ( CQuaternion* axisq, CQuaternion newq );

	//int UpdateMatrix( ChaMatrix* wmat, ChaMatrix* parmat, CQuaternion* parq, CBone* srcbone );

	int AddToPrev( CMotionPoint* addmp );
	int AddToNext( CMotionPoint* addmp );
	int LeaveFromChain( int srcmotid = -1, CBone* boneptr = 0 );

	int CopyMP( CMotionPoint* srcmp );

	int CalcQandTra( ChaMatrix srcmat, CBone* boneptr, float hrate = 1.0f );
	//int SetFirstFrameBaseMat(ChaMatrix srcfirstmat);



	CMotionPoint operator= (CMotionPoint mp);

public:
	//accesser

	double GetFrame(){ return m_frame; };
	void SetFrame( double srcframe ){ m_frame = srcframe; };


	ChaVector3 GetTra(){ return m_tra; };
	void SetTra( ChaVector3 srctra ){ m_tra = srctra; };

	//ChaVector3 GetFirstFrameTra(){ return m_firstframetra; };

	CQuaternion GetQ(){ return m_q; };
	CQuaternion GetInvQ(){
		CQuaternion invq;
		m_q.inv(&invq);
		return invq;
	};
	void SetQ( CQuaternion srcq ){ m_q = srcq; };

	ChaMatrix GetWorldMat();
	//void SetWorldMat(ChaMatrix srcmat) { m_calclimitedwm = 0; SetBefWorldMat(m_worldmat); m_worldmat = srcmat; };
	void SetWorldMat(ChaMatrix srcmat) { m_calclimitedwm = 0; m_worldmat = srcmat; };
	//void SetNewWorldMat(CBone* ownerbone, ChaMatrix srcmat);//after limit eul


	//ChaMatrix GetInvWorldMat(){ ChaMatrix invmat; ChaMatrixInverse(&invmat, NULL, &m_worldmat); return invmat; };


	ChaMatrix GetLocalMat() { return m_localmat; };//CameraAnimのeNullとeCameraノードのローカル姿勢を保存、書き出しに使う
	void SetLocalMat(ChaMatrix srcmat) { m_localmat = srcmat; };//CameraAnimのeNullとeCameraノードのローカル姿勢を保存、書き出しに使う

//btmat, btflagはCBoneに移動
	//ChaMatrix GetBefBtMat();
	//void SetBefBtMat(ChaMatrix srcmat);
	//ChaMatrix GetBtMat();
	//void SetBtMat(ChaMatrix srcmat);
	//int GetBtFlag();
	//void SetBtFlag(int srcflag);


	//2022/12/29
	//関数の呼び出し状況によってbefworldmatの内容が変わるのは不味いので
	//明示的に引数として渡すことにした(他のIK用のReq関数についても)
	//GetBefWorldMat()は使用しないことにした
	//
	//ChaMatrix GetBefWorldMat(){ return m_befworldmat; };
	//void SetBefWorldMat( ChaMatrix srcmat ){ m_befworldmat = srcmat; };

	ChaMatrix GetBefEditMat(){ return m_befeditmat; };
	void SetBefEditMat( ChaMatrix srcmat ){ m_befeditmat = srcmat; };

	ChaMatrix GetAbsMat(){ return m_absmat; };
	void SetAbsMat( ChaMatrix srcmat ){ m_absmat = srcmat; };

	CMotionPoint* GetPrev(){ return m_prev; };
	void SetPrev( CMotionPoint* srcprev ){ m_prev = srcprev; };

	CMotionPoint* GetNext(){ return m_next; };
	void SetNext( CMotionPoint* srcnext ){ m_next = srcnext; };

	ChaVector3 GetLocalEul()
	{
		return m_localeul;
	};
	void SetLocalEul(ChaVector3 neweul);

	int GetUndoValidFlag()
	{
		return m_undovalidflag;
	};
	void SetUndoValidFlag(int srcflag)
	{
		m_undovalidflag = srcflag;
	};
	int GetLocalMatFlag()
	{
		return m_localmatflag;//CopyAndPaste時にworldmatにlocalmatをセットしていることを示すフラグ
	};
	void SetLocalMatFlag(int srcflag)
	{
		m_localmatflag = srcflag;//CopyAndPaste時にworldmatにlocalmatをセットしていることを示すフラグ
	};

	int GetUseFlag()
	{
		return m_useflag;
	};
	void SetUseFlag(int srcflag)
	{
		m_useflag = srcflag;
	};
	int GetIndexOfPool()
	{
		return m_indexofpool;
	};
	void SetIndexOfPool(int srcindex)
	{
		m_indexofpool = srcindex;
	};
	int IsAllocHead()
	{
		return m_allocheadflag;
	};
	void SetIsAllocHead(int srcflag)
	{
		m_allocheadflag = srcflag;
	};

	void SetCalcLimitedWM(int srcval) {
		m_calclimitedwm = srcval;
	};
	int GetCalcLimitedWM()
	{
		return m_calclimitedwm;
	};

	void SetLimitedWM(ChaMatrix srcmat)
	{
		m_limitedwm = srcmat;
	};
	ChaMatrix GetLimitedWM() {
		return m_limitedwm;
	}

	void SetLimitedLocalEul(ChaVector3 srceul)
	{
		m_limitedlocaleul = srceul;
	};
	ChaVector3 GetLimitedLocalEul()
	{
		return m_limitedlocaleul;
	};

	//void SetSRT(ChaMatrix srcmat) 
	//{
	//	m_SRT = srcmat;
	//}
	//ChaMatrix GetSRT()
	//{
	//	return m_SRT;
	//}

	void SetSaveSRTandTraAnim(ChaMatrix srcsmat, ChaMatrix srcrmat, ChaMatrix srctmat, ChaMatrix srctanimmat) {
		m_savesmat = srcsmat;
		m_savermat = srcrmat;
		m_savetmat = srctmat;
		m_savetanimmat = srctanimmat;
	}
	int GetSaveSRTandTraAnim(ChaMatrix* dstsmat, ChaMatrix* dstrmat, ChaMatrix* dsttmat, ChaMatrix* dsttanimmat) {
		//if (!dstsmat || !dstrmat || !dsttmat || !dsttanimmat) {
		//	_ASSERT(0);
		//	return 1;
		//}
		if (dstsmat) {
			*dstsmat = m_savesmat;
		}
		if (dstrmat) {
			*dstrmat = m_savermat;
		}
		if (dsttmat) {
			*dsttmat = m_savetmat;
		}
		if (dsttanimmat) {
			*dsttanimmat = m_savetanimmat;
		}
		
		return 0;
	}
	ChaMatrix GetSaveWM()
	{
		return m_savewm;
	};
	void SaveWM()
	{
		m_savewm = m_worldmat;
	};
	void RestoreWM()
	{
		m_worldmat = m_savewm;
	};

	void SetAnimMat(ChaMatrix srcmat)
	{
		m_animmat = srcmat;
	};
	ChaMatrix GetAnimMat()
	{
		return m_animmat;
	};

	void SetLocalScale(ChaVector3 srcscale)
	{
		m_scale = srcscale;
	}
	ChaVector3 GetLocalScale()
	{
		return m_scale;
	}

	//CBone* GetNewWMOwner()
	//{
	//	return m_newwmowner;
	//}

	//void SetSetBefWorldMatFlag()
	//{
	//	m_setbefworldmatflag = true;
	//}
	//bool GetSetBefWorldMatFlag()
	//{
	//	return m_setbefworldmatflag;
	//}

	static CMotionPoint* GetNewMP();
	static void InvalidateMotionPoint(CMotionPoint* srcmp);
	static void InitMotionPoints();
	static void DestroyMotionPoints();

private:
	int DestroyObjs();
	//void UpdateChildNewWorldMatReq(CBone* srcbone, int broflag);


private:

	//######################################################################
	//メンバーを足す場合には　コピーペーストで使用するCPELEM2構造体で互換をとる必要有り
	//
	//######################################################################

	int m_useflag;//0: not use, 1: in use
	int m_indexofpool;//index of pool vector
	int m_allocheadflag;//1: head pointer at allocated

	int m_undovalidflag;
	double m_frame;
	int m_localmatflag;//CopyAndPaste時にworldmatにlocalmatをセットしていることを示すフラグ
	//ChaVector3 m_eul;
	ChaVector3 m_tra;
	//ChaVector3 m_firstframetra;

	CQuaternion m_q;
	ChaVector3 m_localeul;

	ChaVector3 m_scale;//2024/01/31

	ChaMatrix m_worldmat;//ワールド変換と親の影響を受けたマトリックス
	ChaMatrix m_localmat;//local matrix : CameraAnimのeNullとeCameraノードのローカル姿勢を保存、書き出しに使う
	
	//2023/02/02
	//CBone::m_curmp,CBone::GetCurMp()用　モデルのworldmatが掛かっていないアニメ姿勢
	//CBone::UpdateMatrix()でセットし　CBone::GetCurrent...やCBone::CalcCurrent...で使用
	ChaMatrix m_animmat;


	//ChaMatrix m_befworldmat;
	ChaMatrix m_befeditmat;

	ChaMatrix m_absmat;

	//ChaMatrix m_SRT;


	//bool m_setbefworldmatflag;


	//btmatはCBoneに移動
	//ChaMatrix m_btmat;
	//ChaMatrix m_befbtmat;
	//int m_setbtflag;

	//CBone* m_newwmowner;
	//ChaMatrix m_newworldmat;

	int m_calclimitedwm;
	ChaMatrix m_limitedwm;
	ChaVector3 m_limitedlocaleul;

	ChaMatrix m_savesmat;
	ChaMatrix m_savermat;
	ChaMatrix m_savetmat;
	ChaMatrix m_savetanimmat;
	ChaMatrix m_savewm;


	CMotionPoint* m_prev;
	CMotionPoint* m_next;


	//######################################################################
	//メンバーを足す場合には　コピーペーストで使用するCPELEM2構造体で互換をとる必要有り
	//
	//######################################################################


};


#endif
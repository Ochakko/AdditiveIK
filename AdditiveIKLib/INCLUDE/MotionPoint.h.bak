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

	ChaVector3 GetFirstFrameTra(){ return m_firstframetra; };

	CQuaternion GetQ(){ return m_q; };
	CQuaternion GetInvQ(){
		CQuaternion invq;
		m_q.inv(&invq);
		return invq;
	};
	void SetQ( CQuaternion srcq ){ m_q = srcq; };

	ChaMatrix GetWorldMat();
	void SetWorldMat(ChaMatrix srcmat) { m_calclimitedwm = 0; SetBefWorldMat(m_worldmat); m_worldmat = srcmat; };
	//void SetNewWorldMat(CBone* ownerbone, ChaMatrix srcmat);//after limit eul


	ChaMatrix GetInvWorldMat(){ ChaMatrix invmat; ChaMatrixInverse(&invmat, NULL, &m_worldmat); return invmat; };


	ChaMatrix GetLocalMat() { return m_localmat; };
	void SetLocalMat(ChaMatrix srcmat) { m_localmat = srcmat; };

//btmat, btflagはCBoneに移動
	//ChaMatrix GetBefBtMat();
	//void SetBefBtMat(ChaMatrix srcmat);
	//ChaMatrix GetBtMat();
	//void SetBtMat(ChaMatrix srcmat);
	//int GetBtFlag();
	//void SetBtFlag(int srcflag);


	ChaMatrix GetBefWorldMat(){ return m_befworldmat; };
	void SetBefWorldMat( ChaMatrix srcmat ){ m_befworldmat = srcmat; };

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
		return m_localmatflag;
	};
	void SetLocalMatFlag(int srcflag)
	{
		m_localmatflag = srcflag;
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

	void SetSRT(ChaMatrix srcmat) 
	{
		m_SRT = srcmat;
	}
	ChaMatrix GetSRT()
	{
		return m_SRT;
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

	int m_useflag;//0: not use, 1: in use
	int m_indexofpool;//index of pool vector
	int m_allocheadflag;//1: head pointer at allocated

	int m_undovalidflag;
	double m_frame;
	int m_localmatflag;
	//ChaVector3 m_eul;
	ChaVector3 m_tra;
	ChaVector3 m_firstframetra;

	CQuaternion m_q;
	ChaVector3 m_localeul;

	ChaMatrix m_worldmat;//ワールド変換と親の影響を受けたマトリックス
	ChaMatrix m_localmat;//local matrix


	ChaMatrix m_befworldmat;
	ChaMatrix m_befeditmat;

	ChaMatrix m_absmat;

	ChaMatrix m_SRT;


	bool m_setbefworldmatflag;


	//btmatはCBoneに移動
	//ChaMatrix m_btmat;
	//ChaMatrix m_befbtmat;
	//int m_setbtflag;

	//CBone* m_newwmowner;
	//ChaMatrix m_newworldmat;

	int m_calclimitedwm;
	ChaMatrix m_limitedwm;


	CMotionPoint* m_prev;
	CMotionPoint* m_next;

};


#endif
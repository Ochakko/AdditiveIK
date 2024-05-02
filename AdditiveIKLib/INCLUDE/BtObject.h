#ifndef BTOBJECTH
#define BTOBJECTH

//#include <d3dx9.h>
#include <wchar.h>
#include <Coef.h>
#include <string>
#include <map>
#include <vector>

#include <ConstraintElem.h>

#include <ChaVecCalc.h>

#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"

class CModel;
class CBone;
class CRigidElem;

class CBtObject
{
public:

/**
 * @fn
 * CBtObject
 * @breaf CBtObjectのコンストラクタ
 * @param (CBtObject* parbt) IN 親のCBtObjectへのポインタ。
 * @param (btDynamicsWorld* btWorld) IN bulletのbtDynamicsWorldへのポインタ。
 * @return なし。
 */
	CBtObject( CBtObject* parbt, btDynamicsWorld* btWorld );

/**
 * @fn
 * ~CBtObject
 * @breaf CBtObjectのデストラクタ。
 * @return なし。
 */
	~CBtObject();


/**
 * @fn
 * CreateObject
 * @breaf bulletの剛体オブジェクトを作成し設定する。
 * @param (CBtObject* parbt) IN 親のCBtObjectへのポインタ。
 * @param (CBone* parentbone) IN 剛体の親側のボーンのそのまた親のボーン。
 * @param (CBone* curbone) IN 剛体の親側のボーン。
 * @param (CBone* childbone) IN 剛体の子供側のボーン。
 * @return 成功したら０。
 */
	int CreateObject(bool limitdegflag, int srcmotid, double srcframe, 
		CBtObject* parbt, CBone* parentbone, CBone* curbone, CBone* childbone);

/**
 * @fn
 * CreateBtConstraint
 * @breaf この剛体と接続する剛体との間のコンストレイントを作成する。
 * @return 成功したら０。
 */
	int CreateBtConstraint(bool limitdegflag);


/**
 * @fn
 * SetBtMotion
 * @breaf 剛体シミュレーションの行列からシミュレーション適用後の行列を計算する。
 * @return 成功したら０。
 */
	int SetBtMotion(bool limitdegflag, ChaMatrix curtraanim);

	int SetCapsuleBtMotion(CRigidElem* srcre);



/**
 * @fn
 * Motion2Bt
 * @breaf 剛体シミュレーションのための設定をする。既存モーションからの変化分をスタート時の行列に掛ける。
 * @return 成功したら０。
 * @detail Motion2Btを呼んでからSetBtMotionを呼ぶ。
 */
	int Motion2Bt(CModel* srcmodel, int srcmotid, double srcframe);

	int SetPosture2Bt(bool secondcall, bool btmovable, int limitrate,
		ChaMatrix srcmat, ChaVector3 srcrigidcenter, int constraintupdateflag = 1);

	void RecalcConstraintFrameAB();

/**
 * @fn
 * AddChild
 * @breaf 剛体の階層構造を設定する。
 * @param (CBtObject* addbt) IN 子供にするCBtObjectへのポインタ。
 * @return 成功したら０。
 * @detail 子供の配列に加えるだけ。broは無い。
 */
	int AddChild(CBtObject* addbt);



	int SetEquilibriumPoint(bool limitdegflag, int lflag, int aflag);
	int EnableSpring(bool angleflag, bool linearflag);
	int SetDofRotAxis(int srcaxiskind);


	//int CreatePhysicsPosConstraint();
	//int DestroyPhysicsPosConstraint();

private:

/**
 * @fn
 * InitParams
 * @breaf メンバを初期化する。
 * @return ０を返す。
 */
	int InitParams();

/**
 * @fn
 * DestroyObjs
 * @breaf アロケートしたメモリを開放する。
 * @return ０を返す。
 */
	int DestroyObjs();

/**
 * @fn
 * localCreateRigidBody
 * @breaf bulletのRigidBodyを作成する。CreateObjectから呼ばれる。
 * @param (CRigidElem* curre) IN 剛体設定パラメータクラスCRigidElemへのポインタ。
 * @param (const btTransform& startTransform) IN 剛体を初期位置に設定するための変換行列。
 * @param (btCollisionShape* shape) IN 剛体のあたり判定用オブジェクト。
 * @return 作成したbtRigidBodyへのポインタ。
 */
	btRigidBody* localCreateRigidBody( CRigidElem* curre, const btTransform& startTransform, btCollisionShape* shape, btVector3 localInertia = btVector3(0.0, 0.0, 0.0));



/**
 * @fn
 * CalcConstraintTransform
 * @breaf コンストレイントを初期位置に置くための変換行列を計算する。
 * @param (int chilflag) IN 剛体の親側の場合は０、子供側の場合は１．
 * @param (CRigidElem* curre) IN 剛体パラメータ。
 * @param (CBtObject* curbto) IN 自分自身、または子供側の剛体オブジェクト。
 * @param (btTransform& dstmat) OUT 変換行列。
 * @return 戻り値の説明
 * @detail 親側のコンストレイントと子供側のコンストレイントと別々に両方呼び出す。
 */
	int CalcConstraintTransform( int chilflag, CRigidElem* curre, CBtObject* curbto, btTransform& dstmat, int setstartflag = 1 );

	//int CalcConstraintTransformA(btTransform& dsttraA, btQuaternion& rotA);
	//int CalcConstraintTransformB(CBtObject* childbto, btQuaternion rotA, btTransform& dsttraB);


	//void DestroyGZObj();
	//int CreatePhysicsPosConstraintCurrent();
	//int CreatePhysicsPosConstraintChild(CBtObject* childbto);


public: //accesser
	btDynamicsWorld* GetbtWorld(){ return m_btWorld; };
	void SetBtWorld( btDynamicsWorld* srcworld ){ m_btWorld = srcworld; };

	int GetTopFlag(){ return m_topflag; };
	void SetTopFlag( int srcflag ){ m_topflag = srcflag; };

	float GetBoneLeng(){ return m_boneleng; };
	void SetBoneLeng( float srcleng ){ m_boneleng = srcleng; };

	ChaMatrix GetTransMat(){ return m_transmat; };
	void SetTransMat( ChaMatrix srcmat ){ m_transmat = srcmat; };

	ChaMatrix GetPar2Cen(){ return m_par2cen; };
	void SetPar2Cen( ChaMatrix srcvec ){ m_par2cen = srcvec; };

	ChaMatrix GetCen2ParY(){ return m_cen2parY; };
	void SetCen2ParY( ChaMatrix srcvec ){ m_cen2parY = srcvec; };

	CBone* GetBone(){ return m_bone; };
	void SetBone( CBone* srcbone ){ m_bone = srcbone; };

	CBone* Getparentbone(){ return m_parentbone; };
	void Setparentbone( CBone* srcbone ){ m_parentbone = srcbone; };

	CBone* GetEndBone(){ return m_endbone; };
	void SetEndBone( CBone* srcbone ){ m_endbone = srcbone; };

	//btCollisionShape* GetColShape(){ return m_colshape; };
	//void SetColShape( btCollisionShape* srcshape ){ m_colshape = srcshape; };
	btCompoundShape* GetColShape() { return m_colshape; };
	void SetColShape(btCompoundShape* srcshape) { m_colshape = srcshape; };


	btRigidBody* GetRigidBody(){ return m_rigidbody; };
	void SetRigidBody( btRigidBody* srcbody ){ m_rigidbody = srcbody; };


	CBtObject* GetParBt(){ return m_parbt; };
	void SetParBt( CBtObject* srcbt ){ m_parbt = srcbt; };

	int GetChildBtSize(){ return (int)m_chilbt.size(); };
	void PushBackChildBt( CBtObject* srcchil ){ m_chilbt.push_back( srcchil ); };
	CBtObject* GetChildBt( int srcindex ){ return m_chilbt[ srcindex ]; };
	void CopyChildBt( std::vector<CBtObject*>& dstbt ){ dstbt = m_chilbt; };


	float GetConstZRad(){ return m_constzrad; };
	void SetConstZRad( float srcval ){ m_constzrad = srcval; };

	int GetConnectFlag(){ return m_connectflag; };
	void SetConnectFlag( int srcflag ){ m_connectflag = srcflag; };

	void GetCurPivot( btVector3& dstpivot ){ dstpivot = m_curpivot; };
	void SetCurPivot( btVector3& srcpivot ){ m_curpivot = srcpivot; };

	void GetChilPivot( btVector3& dstpivot ){ dstpivot = m_chilpivot; };
	void SetChilPivot( btVector3& srcpivot ){ m_chilpivot = srcpivot; };

	void GetFrameA( btTransform& dstframe ){ dstframe = m_FrameA; };
	void SetFrameA( btTransform& srcval ){ m_FrameA = srcval; };

	void GetFrameB( btTransform& dstframe ){ dstframe = m_FrameB; };
	void SetFrameB( btTransform& srcval ){ m_FrameB = srcval; };


	btMatrix3x3 GetFirstTransformMat(){
		return 	m_firstTransformMat;//bto->GetRigidBody()のCreateBtObject時のWorldTransform->getBasis
	};
	ChaMatrix GetFirstTransformMatX(){
		return m_firstTransformMatX;
	};
	ChaMatrix GetInvFirstTransformMatX() {
		return ChaMatrixInv(m_firstTransformMatX);
	};

	void SetFirstTransformMatX(ChaMatrix srcmat) {
		m_firstTransformMatX = srcmat;
	};
	btTransform GetFirstTransform(){
		return m_firstTransform;
	};

	//ChaMatrix GetXWorld(){ return m_xworld; };
	//ChaMatrix GetInvXWorld() { ChaMatrix invxworld; ChaMatrixInverse(&invxworld, NULL, &m_xworld); return invxworld; };
	//void SetXWorld( ChaMatrix srcworld ){ m_xworld = srcworld; };

	int GetConstraintSize(){
		return (int)m_constraint.size();
	};
	void PushBackConstraint( CONSTRAINTELEM srcconstraint ){ m_constraint.push_back( srcconstraint ); };
	btGeneric6DofSpringConstraint* GetConstraint(int srcindex){
		if ((srcindex >= 0) && (srcindex < (int)m_constraint.size())){
			return m_constraint[srcindex].constraint;
		}
		else{
			//CONSTRAINTELEM initelem;
			//ZeroMemory(&initelem, sizeof(CONSTRAINTELEM));
			//return initelem;
			return 0;
		}
	};
	CONSTRAINTELEM GetConstraintElem(int srcindex){
		if ((srcindex >= 0) && (srcindex < (int)m_constraint.size())){
			return m_constraint[srcindex];
		}
		else{
			CONSTRAINTELEM initelem;
			ZeroMemory(&initelem, sizeof(CONSTRAINTELEM));
			return initelem;
		}
	};

	btGeneric6DofSpringConstraint* FindConstraint(CBone* srcchildbone, CBone* srcchildendbone){
		//コンストレイント行列は子供のBtObjectのm_boneとm_endboneのベクトルに依存している。
		//つまりコンストレイントを取得する場合はまず子供Btoを持っているクラス(this)にアクセスして、ChildBtoのm_boneとm_endboneに相当するボーンを指定して取得する。

		int num = GetConstraintSize();
		int conno;
		for (conno = 0; conno < num; conno++){
			CONSTRAINTELEM chkelem = GetConstraintElem(conno);
			if (chkelem.centerbone == srcchildbone){
				CBtObject* chkbto = chkelem.childbto;
				if (chkbto->m_endbone == srcchildendbone){
					return chkelem.constraint;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				}
			}
		}
		return 0;
	};


	ChaVector3 GetBtPos()
	{
		return m_btpos;
	}

private:
	btDynamicsWorld* m_btWorld;

	int m_topflag;
	float m_boneleng;

	ChaMatrix m_transmat;//Y軸平行な剛体をボーンに合わせるための変換行列。
	ChaMatrix m_par2cen;//parent to centerのベクトル。
	ChaMatrix m_cen2parY;//center to parentのベクトル。剛体の初期状態がY軸に平行と仮定。
	//ChaMatrix m_xworld;//bulletの剛体に設定された変換行列。

	CBone* m_bone;//剛体の親側のボーン
	CBone* m_parentbone;//m_boneの親のボーン
	CBone* m_endbone;//剛体の子供側のボーン


	//btCollisionShape* m_colshape;//bulletのあたり判定形状データ。
	btCompoundShape* m_colshape;
	btRigidBody* m_rigidbody;//ブレットの剛体データ。	
	std::vector<CONSTRAINTELEM> m_constraint;//thisと子供のBtObjectをつなぐコンストレイントのvector。
	
	btCollisionShape* m_gz_colshape;//bulletのあたり判定形状データ。
	btRigidBody* m_gz_rigidbody;//ブレットの剛体データ。	
	std::vector<CONSTRAINTELEM> m_gz_vecconstraint;//thisと子供のBtObjectと質量ゼロ剛体をつなぐコンストレイントのvector。



	CBtObject* m_parbt;//親のCBtObject
	std::vector<CBtObject*> m_chilbt;//子供のCBtObject

	float m_constzrad;//Constraintの軸の傾き(Z軸)
	int m_connectflag;//bunki剛体と同位置の剛体をつないだかどうかのフラグ。

	btVector3 m_curpivot;//剛体の親側の支点
	btVector3 m_chilpivot;//剛体の子供側の支点

	btTransform m_FrameA;//剛体設定時のA側変換行列。
	btTransform m_FrameB;//剛体設定時のB側変換行列。

	btMatrix3x3 m_firstTransformMat;//bto->GetRigidBody()のCreateBtObject時のWorldTransform->getBasis
	ChaMatrix m_firstTransformMatX;
	btTransform m_firstTransform;


	ChaVector3 m_btpos;//Motion2Btで計算した剛体の位置

	CQuaternion m_btq;//CreateObjectで設定した剛体の初期の向き
};


#endif
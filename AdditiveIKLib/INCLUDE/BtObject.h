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
 * @breaf CBtObject�̃R���X�g���N�^
 * @param (CBtObject* parbt) IN �e��CBtObject�ւ̃|�C���^�B
 * @param (btDynamicsWorld* btWorld) IN bullet��btDynamicsWorld�ւ̃|�C���^�B
 * @return �Ȃ��B
 */
	CBtObject( CBtObject* parbt, btDynamicsWorld* btWorld );

/**
 * @fn
 * ~CBtObject
 * @breaf CBtObject�̃f�X�g���N�^�B
 * @return �Ȃ��B
 */
	~CBtObject();


/**
 * @fn
 * CreateObject
 * @breaf bullet�̍��̃I�u�W�F�N�g���쐬���ݒ肷��B
 * @param (CBtObject* parbt) IN �e��CBtObject�ւ̃|�C���^�B
 * @param (CBone* parentbone) IN ���̂̐e���̃{�[���̂��̂܂��e�̃{�[���B
 * @param (CBone* curbone) IN ���̂̐e���̃{�[���B
 * @param (CBone* childbone) IN ���̂̎q�����̃{�[���B
 * @return ����������O�B
 */
	int CreateObject(bool limitdegflag, int srcmotid, double srcframe, 
		CBtObject* parbt, CBone* parentbone, CBone* curbone, CBone* childbone);

/**
 * @fn
 * CreateBtConstraint
 * @breaf ���̍��̂Ɛڑ����鍄�̂Ƃ̊Ԃ̃R���X�g���C���g���쐬����B
 * @return ����������O�B
 */
	int CreateBtConstraint(bool limitdegflag);


/**
 * @fn
 * SetBtMotion
 * @breaf ���̃V�~�����[�V�����̍s�񂩂�V�~�����[�V�����K�p��̍s����v�Z����B
 * @return ����������O�B
 */
	int SetBtMotion(bool limitdegflag, ChaMatrix curtraanim);

	int SetCapsuleBtMotion(CRigidElem* srcre);



/**
 * @fn
 * Motion2Bt
 * @breaf ���̃V�~�����[�V�����̂��߂̐ݒ������B�������[�V��������̕ω������X�^�[�g���̍s��Ɋ|����B
 * @return ����������O�B
 * @detail Motion2Bt���Ă�ł���SetBtMotion���ĂԁB
 */
	int Motion2Bt(CModel* srcmodel, int srcmotid, double srcframe);

	int SetPosture2Bt(ChaMatrix srcmat, ChaVector3 srcrigidcenter, int constraintupdateflag = 1);

	void RecalcConstraintFrameAB();

/**
 * @fn
 * AddChild
 * @breaf ���̂̊K�w�\����ݒ肷��B
 * @param (CBtObject* addbt) IN �q���ɂ���CBtObject�ւ̃|�C���^�B
 * @return ����������O�B
 * @detail �q���̔z��ɉ����邾���Bbro�͖����B
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
 * @breaf �����o������������B
 * @return �O��Ԃ��B
 */
	int InitParams();

/**
 * @fn
 * DestroyObjs
 * @breaf �A���P�[�g�������������J������B
 * @return �O��Ԃ��B
 */
	int DestroyObjs();

/**
 * @fn
 * localCreateRigidBody
 * @breaf bullet��RigidBody���쐬����BCreateObject����Ă΂��B
 * @param (CRigidElem* curre) IN ���̐ݒ�p�����[�^�N���XCRigidElem�ւ̃|�C���^�B
 * @param (const btTransform& startTransform) IN ���̂������ʒu�ɐݒ肷�邽�߂̕ϊ��s��B
 * @param (btCollisionShape* shape) IN ���̂̂����蔻��p�I�u�W�F�N�g�B
 * @return �쐬����btRigidBody�ւ̃|�C���^�B
 */
	btRigidBody* localCreateRigidBody( CRigidElem* curre, const btTransform& startTransform, btCollisionShape* shape, btVector3 localInertia = btVector3(0.0, 0.0, 0.0));



/**
 * @fn
 * CalcConstraintTransform
 * @breaf �R���X�g���C���g�������ʒu�ɒu�����߂̕ϊ��s����v�Z����B
 * @param (int chilflag) IN ���̂̐e���̏ꍇ�͂O�A�q�����̏ꍇ�͂P�D
 * @param (CRigidElem* curre) IN ���̃p�����[�^�B
 * @param (CBtObject* curbto) IN �������g�A�܂��͎q�����̍��̃I�u�W�F�N�g�B
 * @param (btTransform& dstmat) OUT �ϊ��s��B
 * @return �߂�l�̐���
 * @detail �e���̃R���X�g���C���g�Ǝq�����̃R���X�g���C���g�ƕʁX�ɗ����Ăяo���B
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
		return 	m_firstTransformMat;//bto->GetRigidBody()��CreateBtObject����WorldTransform->getBasis
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
		//�R���X�g���C���g�s��͎q����BtObject��m_bone��m_endbone�̃x�N�g���Ɉˑ����Ă���B
		//�܂�R���X�g���C���g���擾����ꍇ�͂܂��q��Bto�������Ă���N���X(this)�ɃA�N�Z�X���āAChildBto��m_bone��m_endbone�ɑ�������{�[�����w�肵�Ď擾����B

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

	ChaMatrix m_transmat;//Y�����s�ȍ��̂��{�[���ɍ��킹�邽�߂̕ϊ��s��B
	ChaMatrix m_par2cen;//parent to center�̃x�N�g���B
	ChaMatrix m_cen2parY;//center to parent�̃x�N�g���B���̂̏�����Ԃ�Y���ɕ��s�Ɖ���B
	//ChaMatrix m_xworld;//bullet�̍��̂ɐݒ肳�ꂽ�ϊ��s��B

	CBone* m_bone;//���̂̐e���̃{�[��
	CBone* m_parentbone;//m_bone�̐e�̃{�[��
	CBone* m_endbone;//���̂̎q�����̃{�[��


	//btCollisionShape* m_colshape;//bullet�̂����蔻��`��f�[�^�B
	btCompoundShape* m_colshape;
	btRigidBody* m_rigidbody;//�u���b�g�̍��̃f�[�^�B	
	std::vector<CONSTRAINTELEM> m_constraint;//this�Ǝq����BtObject���Ȃ��R���X�g���C���g��vector�B
	
	btCollisionShape* m_gz_colshape;//bullet�̂����蔻��`��f�[�^�B
	btRigidBody* m_gz_rigidbody;//�u���b�g�̍��̃f�[�^�B	
	std::vector<CONSTRAINTELEM> m_gz_vecconstraint;//this�Ǝq����BtObject�Ǝ��ʃ[�����̂��Ȃ��R���X�g���C���g��vector�B



	CBtObject* m_parbt;//�e��CBtObject
	std::vector<CBtObject*> m_chilbt;//�q����CBtObject

	float m_constzrad;//Constraint�̎��̌X��(Z��)
	int m_connectflag;//bunki���̂Ɠ��ʒu�̍��̂��Ȃ������ǂ����̃t���O�B

	btVector3 m_curpivot;//���̂̐e���̎x�_
	btVector3 m_chilpivot;//���̂̎q�����̎x�_

	btTransform m_FrameA;//���̐ݒ莞��A���ϊ��s��B
	btTransform m_FrameB;//���̐ݒ莞��B���ϊ��s��B

	btMatrix3x3 m_firstTransformMat;//bto->GetRigidBody()��CreateBtObject����WorldTransform->getBasis
	ChaMatrix m_firstTransformMatX;
	btTransform m_firstTransform;


	ChaVector3 m_btpos;//Motion2Bt�Ōv�Z�������̂̈ʒu

	CQuaternion m_btq;//CreateObject�Őݒ肵�����̂̏����̌���
};


#endif
#ifndef RIGIDELEMH
#define RIGIDELEMH

//#include <d3dx9.h>
#include <ChaVecCalc.h>

#include <wchar.h>
#include <Coef.h>
#include <string>
#include <map>
#include <vector>

#include <Bone.h>

#define REPOOLBLKLEN	256

class CRigidElem
{
public:
	CRigidElem();
	~CRigidElem();

	int GetColiID();


private:
	int InitParams();
	int DestroyObjs();

public: //accesser
	int GetColtype(){ return m_coltype; };
	void SetColtype( int srctype ){ m_coltype = srctype; };

	int GetSkipflag(){ return m_skipflag; };
	void SetSkipflag( int srcflag ){ m_skipflag = srcflag; };

	float GetMass(){ return m_mass; };
	void SetMass( float srcmass ){ m_mass = srcmass; };

	float GetSphr(){ return m_sphr; };
	void SetSphr( float srcsphr ){ m_sphr = srcsphr; };

	float GetSphrate(){ return m_sphrate; };
	void SetSphrate( float srcrate ){ m_sphrate = srcrate; };

	float GetBoxz(){ return m_boxz; };
	void SetBoxz( float srcboxz ){ m_boxz = srcboxz; };

	float GetBoxzrate(){ return m_boxzrate; };
	void SetBoxzrate( float srcrate ){ m_boxzrate = srcrate; };

	float GetCylileng(){ return m_cylileng; };
	void SetCylileng( float srcleng ){ m_cylileng = srcleng; };

	int GetLKindex(){ return m_l_kindex; };
	void SetLKindex( int srcindex ){ m_l_kindex = srcindex; };

	int GetAKindex(){ return m_a_kindex; };
	void SetAKindex( int srcindex ){ m_a_kindex = srcindex; };

	float GetCusLk(){ return m_cus_lk; };
	void SetCusLk( float srck ){ m_cus_lk = srck; };

	float GetCusAk(){ return m_cus_ak; };
	void SetCusAk( float srck ){ m_cus_ak = srck; };

	float GetLDamping(){ return m_l_damping; };
	void SetLDamping( float srcd ){ m_l_damping = srcd; };

	float GetADamping(){ return m_a_damping; };
	void SetADamping( float srcd ){ m_a_damping = srcd; };

	float GetDampanimL(){ return m_dampanim_l; };
	void SetDampanimL( float srcd ){ m_dampanim_l = srcd; };

	float GetDampanimA(){ return m_dampanim_a; };
	void SetDampanimA( float srcd ){ m_dampanim_a = srcd; };

	float GetBtg(){ return m_btg; };
	void SetBtg( float srcg ){ m_btg = srcg; };

	int GetGroupid(){ return m_groupid; };
	void SetGroupid( int srcid ){ m_groupid = srcid; };

	int GetColiidsSize(){ return (int)m_coliids.size(); };
	int GetColiids( int srcindex ){ return m_coliids[ srcindex ]; };
	void PushBackColiids( int srcid ){ m_coliids.push_back( srcid ); };
	void CopyColiids(CRigidElem* srcre) { m_coliids = srcre->m_coliids; SetMyselfflag(srcre->GetMyselfflag()); };
	void CopyColiids( std::vector<int>& srcids ){ m_coliids = srcids; };
	void CopyColiids2Dstvec( std::vector<int>& dstids ){ dstids = m_coliids; };


	int GetMyselfflag(){ return m_myselfflag; };
	void SetMyselfflag( int srcflag ){ m_myselfflag = srcflag; };

	float GetRestitution(){ return m_restitution; };
	void SetRestitution( float srcval ){ m_restitution = srcval; };

	float GetFriction(){ return m_friction; };
	void SetFriction( float srcval ){ m_friction = srcval; };

	ChaMatrix GetCapsulematForColiShape(bool limitdegflag, int calczeroframe, int multworld = 1);

	CBone* GetBone(){ return m_bone; };
	void SetBone( CBone* srcbone ){ m_bone = srcbone; };

	CBone* GetEndbone(){ return m_endbone; };
	void SetEndbone( CBone* srcbone ){ m_endbone = srcbone; };

	float GetBoneLeng();

	int GetForbidRotFlag(){ return m_forbidrotflag; };
	void SetForbidRotFlag(int srcflag){
		if ((srcflag == 0) || (srcflag == 1)){
			m_forbidrotflag = srcflag;
		}
		else{
			m_forbidrotflag = 0;
		}
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

	static CRigidElem* GetNewRigidElem();
	static void InvalidateRigidElem(CRigidElem* srcmp);
	static void InitRigidElems();
	static void DestroyRigidElems();

private:
	int m_useflag;//0: not use, 1: in use
	int m_indexofpool;//index of pool vector
	int m_allocheadflag;//1: head pointer at allocated


	int m_coltype;
	int m_skipflag;

	float m_boneleng;
	float m_mass;//����

	float m_sphr;//�����a
	float m_sphrate;//�����a�䗦�A�܂�ȉ~�̗�
	float m_boxz;//���̕ӂ̒���
	float m_boxzrate;//�����̂̕ӂ̔䗦
	float m_cylileng;//�V�����_�[�̒���

	int m_l_kindex;//�ړ��΂˒萔(�e�Ƃ̐ڑ�)�̃v���Z�b�g�p�C���f�b�N�X
	int m_a_kindex;//��]�΂˒萔�̃v���Z�b�g�p�C���f�b�N�X

	float m_cus_lk;//�J�X�^�����̈ړ��΂˒萔
	float m_cus_ak;//�J�X�^�����̉�]�΂˒萔

	float m_l_damping;//�ړ�������
	float m_a_damping;//��]������
	float m_dampanim_l;
	float m_dampanim_a;

	float m_btg;//���O�h�[�����̏d��

	int m_groupid;//�R���W�����p�@�����̃O���[�vID
	std::vector<int> m_coliids;//�R���W�����p�@�Փ˔��肷��O���[�vID�̔z��
	int m_myselfflag;//�R���W�����p�@�����ƏՓ˔��肷�邩�ǂ����̃t���O

	float m_restitution;
	float m_friction;

	int m_forbidrotflag;

	CBone* m_bone;
	CBone* m_endbone;
};

#endif

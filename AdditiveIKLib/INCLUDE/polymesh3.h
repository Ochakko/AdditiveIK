#ifndef POLYMESH3H
#define POLYMESH3H

#include <coef.h>
//#include <D3DX9.h>
#include <ChaVecCalc.h>

#include <crtdbg.h>
//#include <infelem.h>

#include <map>
#include <string>
//using namespace std;

class CMQOObject;
class CMQOMaterial;
class CMQOFace;
class CInfBone;
//class CInfScope;
class CBone;

class CPolyMesh3
{
public:
	CPolyMesh3();
	~CPolyMesh3();

	int CreatePM3(bool fbxfileflag, int pointnum, int facenum, float facet, ChaVector3* pointptr, CMQOFace* faceptr, 
		std::map<int,CMQOMaterial*>& srcmat, ChaMatrix multmat);
	
	//int SetIndexBuf();	
	//int InvIndexBuf();

	int ChkAlphaNum(std::map<int,CMQOMaterial*>& srcmat );
	int CalcBound();

	//int CalcInfNoSkin( CBone* applybone );

	int MultScale( ChaVector3 srcscale, ChaVector3 srctra );

	void DestroySystemDispObj();

	int IncludeTransparent(CMQOObject* srcobj, float alphamult, bool* pfound_noalpha, bool* pfound_alpha);

private:
	void InitParams();
	void DestroyObjs();

	int MultVert( ChaMatrix multmat );
	int CreateN3PFromMQOFace( N3P* n3pptr, int* numptr );

	int CalcOrgNormal();
	int CalcNormal( ChaVector3* newn, ChaVector3* curp, ChaVector3* aftp1, ChaVector3* aftp2 );
	int Vec3Cross( ChaVector3* pOut, ChaVector3* pV1, ChaVector3* pV2 );
	int Vec3Normalize( ChaVector3* retvec, ChaVector3* srcvec );

	int SetSMFace();
	int AddSmFace( N3P* n3p1, N3P* n3p2 );
	//int CalcSMNormal();

	int SetOptV( PM3DISPV* optv, int* pleng, int* matnum, std::map<int,CMQOMaterial*>& srcmat );


/***
typedef struct tag_verface
{
	int			faceno;
	int			orgfaceno;
	int			materialno;
	ChaVector3	facenormal;
}PERFACE;

typedef struct tag_pervert
{
	int				indexno;//3�p�̏���
	int				vno;
	int				uvnum;
	ChaVector2		uv[2];
	int				vcolflag;
	DWORD			vcol;
	ChaVector3 smnormal;

	int				createflag;
		//���_���쐬���Ȃ��ꍇ�O
		//UV, VCOL, Materialno�̈Ⴂ�ɂ��쐬����ꍇ�͂P�𑫂�
		//normal�ɂ��쐬����ꍇ�͂Q�𑫂�
}PERVERT;

typedef struct tag_n3sm
{
	int smfacenum;
	void** ppsmface;//N3P�ւ̃|�C���^�̔z��
}N3SM;

typedef struct tag_n3p
{
	PERFACE*	perface;
	PERVERT*	pervert;
	N3SM*		n3sm;
}N3P;//n*3


n3p��vno, materialno, uv, vcol�Ń\�[�g����n3psort�����B
n3psort�𓪂���`�F�b�N���A�܂������������̂��������createflag�ɂP�𑫂��B

n3psort��vno�����������̖̂@�����`�F�b�N���X���[�W���O������̂�����������ppsmface�ɒǉ�����B
�S���I�������
n3psort��vno�����������̂�*ppsmface���`�F�b�N���A�������n�߂ďo�������Ƃ��܂��͏o�����Ȃ������Ƃ�����createflag�ɂQ��������Ă��Ȃ��Ƃ�
createflag�ɂQ�𑫂��B

n3sm����smnormal���v�Z����B

n3psort��materialno, faceno, indexno�Ń\�[�g����n3pafter�����B

n3pafter��createflag�����ĂO�ȊO�̂��̂���PM3OPTV�����B

typedef struct tag_pm3optv
{
	int orgvno;
	int orgfaceno;
	ChaVector3 pos;
	int materialno;
	ChaVector3 normal;
	int				uvnum;
	ChaVector2		uv[2];
	int				vcolflag;
	DWORD		vcol;
}PM3OPTV;


***/

public:
	//accesser
	int GetOrgPointNum(){
		return m_orgpointnum;
	};

	int GetOrgFaceNum(){
		return m_orgfacenum;
	};

	int GetFaceNum(){
		return m_facenum;
	};

	float GetFacet(){
		return m_facet;
	};

	//CInfBone* GetInfBone(){
	//	return m_infbone;
	//};

	int GetOptLeng(){
		return m_optleng;
	};

	int GetOptMatNum(){
		return m_optmatnum;
	};

	PM3DISPV* GetDispV(){
		return m_dispv;
	};

	int* GetDispIndex(){
		return m_dispindex;
	};

	int GetCreateOptFlag(){
		return m_createoptflag;
	};

	MATERIALBLOCK* GetMatBlock(){
		return m_matblock;
	};

	MODELBOUND GetBound(){
		return m_bound;
	};

	bool GetFbxFileFlag()
	{
		return m_fbxfileflag;
	}

private:
	bool m_fbxfileflag;

	int m_orgpointnum;
	int m_orgfacenum;
	int m_facenum;//�O�p������̖ʐ�

	float m_facet;
	//CInfBone* m_infbone;//orgpointnum��

	int m_optleng;
	int m_optmatnum;
	PM3DISPV* m_dispv;
	int*	m_dispindex;
	int m_createoptflag;

	MATERIALBLOCK* m_matblock;
	MODELBOUND	m_bound;


//�ȉ��A�N���X�O����A�N�Z�X���Ȃ��̂ŃA�N�Z�b�T�[�����B
	CMQOFace* m_mqoface;//�O��������
	ChaVector3* m_pointbuf;//�O��������
	N3P* m_n3p;
	CHKALPHA chkalpha;

};



#endif
#ifndef POLYMESH3H
#define POLYMESH3H

#include <coef.h>
//#include <D3DX9.h>
#include <ChaVecCalc.h>

#include <crtdbg.h>
//#include <infelem.h>

#include <unordered_map>
#include <string>
//using namespace std;

class CMQOObject;
class CMQOMaterial;
class CMQOFace;
class CInfBone;
//class CInfScope;
class CBone;
class CModel;

class CPolyMesh3
{
public:
	CPolyMesh3();
	~CPolyMesh3();

	int CreatePM3(bool fbxfileflag, int pointnum, int facenum, float facet, ChaVector3* pointptr, CMQOFace* faceptr, 
		CModel* pmodel, ChaMatrix multmat, int srcuvnum);
	
	//int SetIndexBuf();	
	//int InvIndexBuf();

	int ChkAlphaNum(std::unordered_map<int,CMQOMaterial*>& srcmat );
	int CalcBound();

	//int CalcInfNoSkin( CBone* applybone );

	int MultScale( ChaVector3 srcscale, ChaVector3 srctra );

	void DestroySystemDispObj(bool emptyshape);

	int IncludeTransparent(CMQOObject* srcobj, float alphamult, bool* pfound_noalpha, bool* pfound_alpha);
	int UpdateMorphBuffer(ChaVector3* mpoint);

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

	int SetOptV(BINORMALDISPV* optv, int* pleng, int* matnum, CModel* pmodel, int srcuvnum);
	int BuildTangentAndBinormal(int srcuvnum);

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
	int				indexno;//3角の順番
	int				vno;
	int				uvnum;
	ChaVector2		uv[2];
	int				vcolflag;
	DWORD			vcol;
	ChaVector3 smnormal;

	int				createflag;
		//頂点を作成しない場合０
		//UV, VCOL, Materialnoの違いにより作成する場合は１を足す
		//normalにより作成する場合は２を足す
}PERVERT;

typedef struct tag_n3sm
{
	int smfacenum;
	void** ppsmface;//N3Pへのポインタの配列
}N3SM;

typedef struct tag_n3p
{
	PERFACE*	perface;
	PERVERT*	pervert;
	N3SM*		n3sm;
}N3P;//n*3


n3pをvno, materialno, uv, vcolでソートしたn3psortを作る。
n3psortを頭からチェックし、まったく同じものが無ければcreateflagに１を足す。

n3psortでvnoが等しいものの法線をチェックしスムージングするものが見つかったらppsmfaceに追加する。
全部終わったら
n3psortのvnoが等しいものの*ppsmfaceをチェックし、自分が始めて出現したときまたは出現しなかったときかつcreateflagに２が足されていないとき
createflagに２を足す。

n3smからsmnormalを計算する。

n3psortをmaterialno, faceno, indexnoでソートしたn3pafterを作る。

n3pafterのcreateflagを見て０以外のものからPM3OPTVを作る。

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

	BINORMALDISPV* GetDispV(){
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

	const std::vector<MODELBOUND> GetARBound(){
		return m_ar_bound;
	};

	bool GetFbxFileFlag()
	{
		return m_fbxfileflag;
	}

private:
	bool m_fbxfileflag;

	int m_orgpointnum;
	int m_orgfacenum;
	int m_facenum;//三角分割後の面数

	float m_facet;
	//CInfBone* m_infbone;//orgpointnum長

	int m_optleng;
	int m_optmatnum;
	BINORMALDISPV* m_dispv;
	int*	m_dispindex;
	int m_createoptflag;

	MATERIALBLOCK* m_matblock;
	MODELBOUND	m_bound;
	std::vector<MODELBOUND> m_ar_bound;

//以下、クラス外からアクセスしないのでアクセッサー無し。
	CMQOFace* m_mqoface;//外部メモリ
	ChaVector3* m_pointbuf;//外部メモリ
	N3P* m_n3p;
	CHKALPHA chkalpha;

};



#endif
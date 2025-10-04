#ifndef POLYMESH4H
#define POLYMESH4H

//class ID3D12Device;


#include <coef.h>
//#include <D3DX9.h>
#include <ChaVecCalc.h>

#include <crtdbg.h>

#include <unordered_map>
//using namespace std;

class CMQOObject;
class CMQOMaterial;
class CMQOFace;
class CInfBone;
//class CInfScope;
class CBone;


typedef struct tag_materialoffsetelem
{
	CMQOMaterial* pmaterial;
	int offset;
	int trinum;

	tag_materialoffsetelem()
	{
		Init();
	};
	void Init()
	{
		pmaterial = nullptr;
		offset = 0;
		trinum = 0;
	};
}MATERIALOFFSETELEM;


class CPolyMesh4
{
public:
	CPolyMesh4();
	~CPolyMesh4();

	int CreatePM4(int normalmappingmode, int pointnum, int facenum, 
		int normalleng, int uvleng, ChaVector3* pointptr, 
		ChaVector3* nptr, ChaVector2* uvptr, ChaVector2* uv1ptr, 
		CMQOFace* faceptr, CModel* pmodel, int srcuvnum);
	
	int ChkAlphaNum( std::unordered_map<int,CMQOMaterial*>& srcmat );
	int CalcBound();

	int SetPm3Inf(CMQOObject* srcobj);
	int UpdateMorphBuffer( ChaVector3* mpoint );

	int DumpInfBone( CMQOObject* srcobj, std::unordered_map<int,CBone*>& srcbonelist );
	//int SetPm3InfNoSkin( ID3D12Device* pdev, CMQOObject* srcobj, int clusterno, std::unordered_map<int,CBone*>& srcbonelist );

	void DestroySystemDispObj(bool emptyshape);

private:
	void InitParams();
	void DestroyObjs();

	int SetTriFace( CMQOFace* faceptr, int* numptr );
	int SetOptV(BINORMALDISPV* optv, int* pleng, int* matnum, CModel* pmodel, int srcuvnum);
	int CreateVecMaterialOffset();
	int SetLastValidVno();
	int BuildTangentAndBinormal(int srcuvnum);

public:
	//accesser
	int GetOrgPointNum(){
		return m_orgpointnum;
		//return (m_lastvalidvno + 1);
	};

	int GetOrgFaceNum(){
		return m_orgfacenum;
	};

	ChaVector3* GetOrgPointBuf()
	{
		return m_pointbuf;
	};

	ChaVector3* GetOrgNormal()
	{
		return m_normal;
	};

	ChaVector2* GetOrgUV()
	{
		return m_uvbuf;
	};
	ChaVector2* GetOrgUV1()
	{
		return m_uv1buf;
	};
	int GetOrgUVLeng(){
		return m_uvleng;
	};

	CMQOFace* GetTriFace(){
		return m_triface;
	};


	int GetFaceNum(){
		return m_facenum;
	};

	float GetFacet(){
		return m_facet;
	};

	CInfBone* GetInfBone(){
		return m_infbone;
	};

	BINORMALDISPV* GetPm3Disp(){
		return m_dispv;
	};

	PM3INF* GetPm3Inf(){
		return m_pm3inf;
	};

	int* GetDispIndex(){
		return m_dispindex;
	};

	int* GetOrgIndex(){
		return m_orgindex;
	};

	int GetCreateOptFlag(){
		return m_createoptflag;
	};

	MODELBOUND GetBound(){
		return m_bound;
	};

	int GetOptLeng(){
		return m_optleng;
	};

	ChaVector3 GetNormalByControlPointNo(int vno);
	//ChaVector2 GetUVByControlPointNo(int vno);

	int GetDispMaterialNum()
	{
		size_t tmpsize = m_materialoffset.size();
		if (tmpsize < INT_MAX) {
			return (int)tmpsize;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	}
	int GetDispMaterial(int srcindex, CMQOMaterial** dstmaterial, int* dstoffset, int* dsttrinum) 
	{
		if ((!dstmaterial) || (!dstoffset)) {
			_ASSERT(0);
			return 1;
		}

		*dstmaterial = NULL;
		*dstoffset = 0;
		*dsttrinum = 0;

		bool findflag = false;
		int materialnum = (int)m_vec_materialoffset.size();

		if ((srcindex >= 0) && (srcindex < materialnum)) {
			MATERIALOFFSETELEM offsetelem = m_vec_materialoffset[srcindex];
			*dstmaterial = offsetelem.pmaterial;
			*dstoffset = offsetelem.offset;
			*dsttrinum = offsetelem.trinum;
			return 0;
		}
		else {
			return 1;
		}
	}

	int GetMaterialNoFromFaceNo(int srcfaceno)
	{
		CMQOMaterial* prevmat = 0;
		int retmaterialcnt = 0;


		int materialnum = GetDispMaterialNum();
		int materialcnt;
		for (materialcnt = 0; materialcnt < materialnum; materialcnt++) {

			CMQOMaterial* chkmat = 0;
			int chkoffset = 0;
			int chktrinum = 0;
			int result = GetDispMaterial(materialcnt, &chkmat, &chkoffset, &chktrinum);
			if (result == 0) {
				int chkstartface = chkoffset / 3;
				int chkendface = chkstartface + chktrinum;

				if (chkstartface <= srcfaceno) {
					prevmat = chkmat;
					retmaterialcnt = materialcnt;
				}
				else if (chkstartface > srcfaceno) {
					break;
				}
			}
		}

		if (prevmat != NULL) {
			return retmaterialcnt;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	}



private:
	int m_normalmappingmode;//0:polygonvertex, 1:controlpoint

	int m_orgpointnum;
	int m_orgfacenum;
	int m_facenum;//三角分割後の面数

	float m_facet;
	int m_normalleng;
	int m_uvleng;

	CInfBone* m_infbone;//orgpointnum長

	int m_optleng;
	BINORMALDISPV* m_dispv;
	PM3INF* m_pm3inf;
	int*	m_dispindex;
	int*	m_orgindex;
	int*	m_fbxindex;
	int m_createoptflag;

	std::map<int, CMQOMaterial*> m_materialoffset;//データエントリー順番が重要なのでunordered_mapには出来ない
	std::vector<MATERIALOFFSETELEM> m_vec_materialoffset;//2025/10/04 m_materialoffsetを高速検索するためのvector

	MODELBOUND	m_bound;

	int*	m_dirtyflag;
	int		m_lastvalidvno;

	CMQOFace* m_mqoface;//外部メモリ
	ChaVector3* m_pointbuf;//外部メモリ
	ChaVector3* m_normal;//外部メモリ
	ChaVector2* m_uvbuf;//外部メモリ
	ChaVector2* m_uv1buf;//外部メモリ
	CMQOFace* m_triface;

	CHKALPHA chkalpha;
};



#endif
#ifndef MQOOBJECTH
#define MQOOBJECTH

//class ID3D12Device;

#include <stdio.h>
#include <stdarg.h>

#include <coef.h>
//#include <basedat.h>

#define DBGH
#include <dbg.h>

//#include <d3dx9.h>
#include <ChaVecCalc.h>

#include <string>
#include <map>


class CMQOFace;
class CMQOMaterial;
class CPolyMesh3;
class CPolyMesh4;
class CExtLine;
class CDispObj;
class CBone;
class CInfBone;
class CRigidElem;

typedef struct tag_latermaterial
{
	CMQOMaterial* pmaterial;
	int offset;
	int trinum;

	void Init()
	{
		pmaterial = 0;
		offset = 0;
		trinum = 0;
	};
	tag_latermaterial()
	{
		Init();
	};
}LATERMATERIAL;


class CMQOObject
{
public:
	CMQOObject();
	~CMQOObject();

	int SetParams( char* srcchar, int srcleng );
	
	int SetVertex( int* vertnum, char* srcchar, int srcleng );
	int SetPointBuf( int vertno, char* srcchar, int srcleng );
	int SetPointBuf( unsigned char* srcptr, int srcnum );

	int SetFace( int* facenum, char* srcchar, int srcleng );
	int SetFaceBuf( int faceno, char* srcchar, int srcleng, int materialoffset );

	int CreateColor();
	int SetColor( char* srcchar, int srcleng );

	int Dump();

	int MakePolymesh3(bool fbxfileflag, ID3D12Device* pdev, std::map<int, CMQOMaterial*>& srcmaterial);
	int MakePolymesh4( ID3D12Device* pdev );
	int MakeExtLine();
	int MakeDispObj( ID3D12Device* pdev, int hasbone );
//	int MakeExtLine( map<int,CMQOMaterial*>& srcmat );

	int HasPolygon();
	int HasLine();

	int MultMat( ChaMatrix multmat );
	//int Shift( ChaVector3 shiftvec );
	int Multiple( float multiple );
	int MultVertex();

	int InitFaceDirtyFlag();

	//int SetMikoBoneIndex3();
	//int SetMikoBoneIndex2();
	//int CheckSameMikoBone();
	//int GetTopLevelMikoBone( CMQOFace** pptopface, int* topnumptr, int maxnum );
	//int SetTreeMikoBone( CMQOFace* srctopface );
	//int CheckLoopedMikoBoneReq( CMQOFace* faceptr, int* isloopedptr, int* jointnumptr );
	//int SetMikoBoneName( std::map<int, CMQOMaterial*> &srcmaterial );
	//int SetMikoFloatBoneName();

	int GetMaterialNoInUse( int* noptr, int arrayleng, int* getnumptr );
	int GetFaceInMaterial( int matno, CMQOFace** ppface, int arrayleng, int* getnumptr );
	int CollisionLocal_Ray(ChaVector3 startlocal, ChaVector3 dirlocal, 
		bool excludeinvface);

	//int MakeXBoneno2wno( int arrayleng, int* boneno2wno, int* infnumptr );
	//int GetSkinMeshHeader( int leng, int* maxpervert, int* maxperface );
	//int MakeXBoneInfluence( std::map<int, CBone*>& bonelist, int arrayleng, int bonenum, int* boneno2wno, BONEINFLUENCE* biptr );

	int AddInfBone( int srcboneno, int srcvno, float srcweight, int isadditive );
	int NormalizeInfBone();
	int UpdateMorphBuffer();

	int ScaleBtCapsule( CRigidElem* reptr, float boneleng, int srctype, float* lengptr );
	int ScaleBtCone( CRigidElem* reptr, float boneleng, float* cyliptr, float* sphptr );
	int ScaleBtBox( CRigidElem* reptr, float boneleng, float* cyliptr, float* sphptr, float* boxz );
	int ScaleBtSphere( CRigidElem* reptr, float boneleng, float* cyliptr, float* sphptr );

	int MultScale( ChaVector3 srcscale, ChaVector3 srctra );

	int DestroyShapeObj();
	int InitShapeWeight();
	int SetShapeWeight( char* nameptr, float srcweight );
	int AddShapeName( char* nameptr );

	int ExistShape( char* nameptr );
	int SetShapeVert( char* nameptr, int vno, ChaVector3 srcv );

	void DestroySystemDispObj();

	int ChkInView(ChaMatrix matWorld, ChaMatrix matVP);

	CBone* GetHipsBone();




	//2023/09/24
	int IncludeTransparent(float multalpha, bool* pfound_noalpha, bool* pfound_alpha);

	//2023/10/05
	int MakeLaterMaterial(std::vector<std::string> latername);
	bool ExistInLaterMaterial(CMQOMaterial* srcmat);

private:
	void InitParams();

	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );
	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );

	int MakeLatheBuf();
	int MakeMirrorBuf();

	int FindConnectFace( int issetface );
	int MakeMirrorPointAndFace( int axis, int doconnect );

	int CheckMirrorDis( ChaVector3* pbuf, CMQOFace* fbuf, int lno, int pnum );

	int CheckFaceSameChildIndex( CMQOFace* srcface, int chkno, CMQOFace** ppfindface );
	//int FindFaceSameParentIndex( CMQOFace* srcface, int chkno, CMQOFace** ppfindface, int* findnum, int maxnum );
	//int IsSameMikoBone( CMQOFace* face1, CMQOFace* face2 );
	int IsSameFaceIndex( CMQOFace* face1, CMQOFace* face2 );
	//int IsSameFacePos( CMQOFace* face1, CMQOFace* face2, VEC3F* pointptr );

	int CheckMaterialSameName( int srcmatno, std::map<int, CMQOMaterial*> &srcmaterial, int* nameflag );
	//int SetXInfluenceArray( CInfBone* ibptr, int vnum, int boneserino, DWORD* vertices, float* weights, int infnum, DWORD* setnumptr );

public:
	//accesser
	bool GetVisible();
	void SetInView(bool srcflag);

	int GetObjFrom(){
		return m_objfrom;
	};
	void SetObjFrom( int srcval ){
		m_objfrom = srcval;
	};

	int GetObjectNo(){
		return m_objectno;
	};
	void SetObjectNo( int srcval ){
		m_objectno = srcval;
	};

	const char* GetName(){
		return m_name;
	};
	void SetName( char* srcname ){
		strcpy_s( m_name, 256, srcname );
	};

	void SetDispName( std::string srcname ){
		m_dispname = srcname;
	};


	const char* GetEngName(){
		return m_engname;
	};

	int GetVertex(){
		return m_vertex;
	};
	void SetVertex( int srcval ){
		m_vertex = srcval;
	};

	int GetFace(){
		return m_face;
	};
	void SetFace( int srcval ){
		m_face = srcval;
	};

	ChaVector3* GetPointBuf(){
		return m_pointbuf;
	};
	void SetPointBuf( ChaVector3* srcbuf ){
		m_pointbuf = srcbuf;
	};

	CMQOFace* GetFaceBuf(){
		return m_facebuf;
	};
	void SetFaceBuf( CMQOFace* srcbuf ){
		m_facebuf = srcbuf;
	};

	CPolyMesh3* GetPm3(){
		return m_pm3;
	};
	void SetPm3( CPolyMesh3* srcval ){
		m_pm3 = srcval;
	};

	CPolyMesh4* GetPm4(){
		return m_pm4;
	};
	void SetPm4( CPolyMesh4* srcval ){
		m_pm4 = srcval;
	};

	CExtLine* GetExtLine(){
		return m_extline;
	};

	CDispObj* GetDispObj(){
		return m_dispobj;
	};
	void SetDispObj( CDispObj* srcval ){
		m_dispobj = srcval;
	};

	CDispObj* GetDispLine(){
		return m_displine;
	};
	void SetDispLine( CDispObj* srcval ){
		m_displine = srcval;
	};

	int GetDispFlag(){
		return m_dispflag;
	};
	void SetDispFlag( int srcval ){
		m_dispflag = srcval;
	};

	int GetNormalLeng(){
		return m_normalleng;
	};
	void SetNormalLeng( int srcval ){
		m_normalleng = srcval;
	};

	ChaVector3* GetNormal(){
		return m_normal;
	};
	void SetNormal( ChaVector3* srcval ){
		m_normal = srcval;
	};

	int GetUVLeng(){
		return m_uvleng;
	};
	void SetUVLeng( int srcval ){
		m_uvleng = srcval;
	};

	int GetNormalMappingMode() {
		return m_normalmappingmode;//0:eByPolygonVertex, 1:eByControlPoint
	};
	void SetNormalMappingMode(int srcval) {
		m_normalmappingmode = srcval;//0:eByPolygonVertex, 1:eByControlPoint
	};

	ChaVector2* GetUVBuf(){
		return m_uvbuf;
	};
	void SetUVBuf( ChaVector2* srcval ){
		m_uvbuf = srcval;
	};

	int GetMaterialSize(){
		return (int)m_material.size();
	};
	CMQOMaterial* GetMaterial( int srcindex ){
		return m_material[ srcindex ];
	};
	std::map<int,CMQOMaterial*>::iterator GetMaterialBegin(){
		return m_material.begin();
	};
	std::map<int,CMQOMaterial*>::iterator GetMaterialEnd(){
		return m_material.end();
	};
	void SetMaterial( int srcindex, CMQOMaterial* srcval ){
		m_material[ srcindex ] = srcval;
	};

	int GetClusterSize(){
		return (int)m_cluster.size();
	};
	CBone* GetCluster( int srcindex ){
		return m_cluster[ srcindex ];
	};
	std::vector<CBone*>::iterator GetClusterBegin(){
		return m_cluster.begin();
	};
	std::vector<CBone*>::iterator GetClusterEnd(){
		return m_cluster.end();
	};
	void PushBackCluster( CBone* srcval ){
		m_cluster.push_back( srcval );
	};

	BOOL EmptyFindShape(){
		return m_findshape.empty();
	};

	int FindShape( std::string srcname ){
		std::map<std::string,int>::iterator itrfind;
		itrfind = m_findshape.find( srcname );
		if( itrfind != m_findshape.end() ){
			return itrfind->second;
		}else{
			return -1;
		}
	};

	void GetShapeVert2( std::map<std::string,ChaVector3*>& dstmap ){
		dstmap = m_shapevert;
	};

	ChaMatrix GetMeshMat()
	{
		return m_meshmat;
	}
	void SetMeshMat(ChaMatrix srcmat)
	{
		m_meshmat = srcmat;
	}

	int GetDbgCount()
	{
		return m_dbgcount;
	}
	void SetDbgCount(int srccount)
	{
		m_dbgcount = srccount;
	}

	FbxNode* GetFbxNode()
	{
		return m_pnode;
	}
	void SetFbxNode(FbxNode* srcnode)
	{
		m_pnode = srcnode;
	}

	int GetLaterMaterialNum()
	{
		return (int)m_latermaterial.size();
	}
	LATERMATERIAL GetLaterMaterial(int srcindex)
	{
		LATERMATERIAL inimat;
		inimat.Init();

		if (m_latermaterial.empty()) {
			return inimat;
		}

		int latermatsize = GetLaterMaterialNum();
		if ((srcindex >= 0) && (srcindex < latermatsize)) {
			return m_latermaterial[srcindex];
		}
		else {
			return inimat;
		}
	}
	

private:
	int m_objfrom;

	int m_objectno;
	char m_name[256];
	std::string m_dispname;
	char m_engname[256];

	int m_vertex;
	int m_face;

	ChaVector3* m_pointbuf;
	CMQOFace* m_facebuf;

	CPolyMesh3* m_pm3;
	CPolyMesh4* m_pm4;

	CExtLine* m_extline;

	CDispObj* m_dispobj;
	CDispObj* m_displine;

	int m_dispflag;

	int m_normalleng;
	ChaVector3* m_normal;
	int m_uvleng;
	ChaVector2* m_uvbuf;
	std::map<int, CMQOMaterial*> m_material;
	std::vector<CBone*> m_cluster;//中身のCBone*は外部メモリ

	std::map<std::string,int> m_findshape;
	std::map<std::string,ChaVector3*> m_shapevert;

	FbxNode* m_pnode;

	ChaFrustumInfo m_frustum;

//以下、クラス外から参照しないのでアクセッサー無し
	int m_patch;
	int m_segment;

	int m_visible;
	int m_locking;

	int m_shading;
	float m_facet;
	ChaVector4 m_color;
	int m_color_type;
	int m_mirror;
	int m_mirror_axis;
	int m_issetmirror_dis;
	float m_mirror_dis;

	int m_lathe;
	int m_lathe_axis;
	int m_lathe_seg;

	int m_hascolor;
	ChaVector4* m_colorbuf;

	int m_vertex2;
	int m_face2;

	ChaVector3* m_pointbuf2;
	CMQOFace* m_facebuf2;
	ChaVector4* m_colorbuf2;

	int m_connectnum;
	CMQOFace* m_connectface;

	ChaMatrix m_multmat;
	//std::map<std::string, CMQOMaterial*> m_namematerial;
	int m_shapenum;
	std::map<std::string,float> m_shapeweight;
	ChaVector3* m_mpoint;


	//fbx GetFBXMesh()
	int m_normalmappingmode;//0:eByPolygonVertex, 1:eByControlPoint

	ChaMatrix m_meshmat;
	int m_dbgcount;

	std::vector<LATERMATERIAL> m_latermaterial;

};

#endif
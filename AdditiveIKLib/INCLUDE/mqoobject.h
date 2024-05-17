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
class CModel;

enum {
	DISPOBJ_NORMAL,
	DISPOBJ_SHADOW,
	DISPOBJ_MAX
};


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

	int MakePolymesh3(bool fbxfileflag, ID3D12Device* pdev, CModel* pmodel);
	int MakePolymesh4(ID3D12Device* pdev, CModel* pmodel);
	int MakeExtLine(CModel* srcmodel);
	int MakeDispObj(ID3D12Device* pdev, int hasbone, bool grassflag);
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
	CMQOMaterial* GetMaterialByFaceIndex(int srcfaceindex);
	int GetFaceInMaterial( int matno, CMQOFace** ppface, int arrayleng, int* getnumptr );

	//マニピュレータピック用
	int CollisionLocal_Ray(ChaVector3 startlocal, ChaVector3 dirlocal, 
		bool excludeinvface);

	//ComputeShader版　polymesh3, polymesh4両方OK
	int CollisionGlobal_Ray_Pm(ChaVector3 startglobal, ChaVector3 dirglobal,
		ChaVector3 startlocal, ChaVector3 dirlocal,
		bool excludeinvface, int* hitfaceindex, ChaVector3* dsthitpos);
	int GetResultOfPickRay(int* hitfaceindex, ChaVector3* dsthitpos);


	//CPU計算　polymesh3用
	int CollisionLocal_Ray_Pm3(ChaVector3 startlocal, ChaVector3 dirlocal,
		bool excludeinvface, int* hitfaceindex, ChaVector3* dsthitpos);
	int CollisionLocal_Ray_BB(ChaVector3 startlocal, ChaVector3 dirlocal);


	//int MakeXBoneno2wno( int arrayleng, int* boneno2wno, int* infnumptr );
	//int GetSkinMeshHeader( int leng, int* maxpervert, int* maxperface );
	//int MakeXBoneInfluence( std::map<int, CBone*>& bonelist, int arrayleng, int bonenum, int* boneno2wno, BONEINFLUENCE* biptr );

	int AddInfBone( int srcboneno, int srcvno, float srcweight, int isadditive );
	int NormalizeInfBone();
	int UpdateMorphWeight(int srcmotid, int framecnt);
	int UpdateMorphBuffer();

	int ScaleBtCapsule(bool setinstancescale, CRigidElem* reptr, float boneleng, int srctype, float* lengptr );
	int ScaleBtCone(bool setinstancescale, CRigidElem* reptr, float boneleng, float* cyliptr, float* sphptr );
	int ScaleBtBox(bool setinstancescale, CRigidElem* reptr, float boneleng, float* cyliptr, float* sphptr, float* boxz );
	int ScaleBtSphere(bool setinstancescale, CRigidElem* reptr, float boneleng, float* cyliptr, float* sphptr );

	int MultScale( ChaVector3 srcscale, ChaVector3 srctra );

	int DestroyShapeObj();
	int DestroyShapeAnim();
	int DestroyShapeAnim(char* srcname, int srcmotid);
	int AddShapeName(char* nameptr);
	int SetShapeVert(char* nameptr, int vno, ChaVector3 srcv);
	int AddShapeAnim(char* nameptr, int srcmotid, int animleng);
	int SetShapeAnim(char* nameptr, int srcmotid, int framecnt, float lWeight);
	int InitShapeWeight();//補間計算結果
	int SetShapeWeight(int channelindex, float srcweight);//補間計算結果

	void DestroySystemDispObj();

	int ChkInView(ChaMatrix matWorld, ChaMatrix matVP, int refposindex);

	CBone* GetHipsBone();




	//2023/09/24
	int IncludeTransparent(float multalpha, bool* pfound_noalpha, bool* pfound_alpha);

	//2023/10/05
	int MakeLaterMaterial(std::vector<std::string> latername);
	bool ExistInLaterMaterial(CMQOMaterial* srcmat);

	MODELBOUND GetBound();

	float GetShapeAnimWeight(int srcmotid, int framecnt, int channelindex);

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
	bool IsND() {
		//名前が_NDで始まるオブジェクトは、影響度設定用のダミーメッシュ　表示しないために判定関数を用意
		char cmpstr[5];
		ZeroMemory(cmpstr, sizeof(char) * 5);
		strncpy_s(cmpstr, 5, GetName(), 3);
		if (strncmp(cmpstr, "_ND", 3) == 0) {
			return true;
		}
		else {
			return false;
		}
	}

	//accesser
	bool GetVisible(int refposindex);//chkinview
	bool GetInShadow(int refposindex);//chkinview
	void SetInView(bool srcflag, int refposindex);//chkinview
	void SetInShadow(bool srcflag, int refposindex);//chkinview

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

	CPolyMesh3* GetPm3();
	void SetPm3( CPolyMesh3* srcval );
	CPolyMesh4* GetPm4();
	void SetPm4( CPolyMesh4* srcval );
	CExtLine* GetExtLine();

	CDispObj* GetDispObj(){
		return m_dispobj;
	};
	void SetDispObj(CDispObj* srcval){
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
	ChaVector2* GetUVBuf1() {
		return m_uvbuf1;
	};
	void SetUVBuf1(ChaVector2* srcval) {
		m_uvbuf1 = srcval;
	};

	int GetOnLoadMaterialSize(){
		return (int)m_onloadmaterial.size();
	};
	void AddOnLoadMaterial(CMQOMaterial* srcmat) {
		m_onloadmaterial.push_back(srcmat);
	};
	CMQOMaterial* GetOnLoadMaterialByMaterialNo( int srcno );
	CMQOMaterial* GetOnLoadMaterialByIndex(int srcindex);
	CMQOMaterial* GetOnLoadMaterialByName(const char* srcname);

	int GetClusterSize(){
		return (int)m_cluster.size();
	};
	CBone* GetCluster( int srcindex ){
		int clustersize = GetClusterSize();
		if ((srcindex >= 0) && (srcindex < clustersize)) {
			return m_cluster[srcindex];
		}
		else {
			return nullptr;
		}
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

	bool EmptyShape(){
		return m_shapenamevec.empty();
	};
	int GetShapeNameNum() {
		int namenum = (int)m_shapenamevec.size();
		return namenum;
	};
	bool ExistShape(char* nameptr){
		if (!nameptr) {
			return false;
		}
		bool existname = false;
		std::string cmpname = nameptr;

		std::vector<std::string>::iterator itrfindname;
		for (itrfindname = m_shapenamevec.begin(); itrfindname != m_shapenamevec.end(); itrfindname++) {
			if (*itrfindname == cmpname) {
				existname = true;
				break;
			}
		}
		return existname;
	};

	//void GetShapeVert2( std::map<std::string,ChaVector3*>& dstmap ){
	//	dstmap = m_shapevert;
	//};

	ChaVector3* GetShapeVert(std::string srcname) {
		std::map<std::string, ChaVector3*>::iterator itrfind;
		itrfind = m_shapevert.find(srcname);
		if (itrfind != m_shapevert.end()) {
			return itrfind->second;
		}
		else {
			return nullptr;
		}
	};
	
	std::string GetShapeName(int srcindex, int* perror) {
		if (!perror) {
			_ASSERT(0);
			return "error";
		}

		int namenum = (int)m_shapenamevec.size();
		if ((srcindex >= 0) && (srcindex < namenum)) {
			*perror = 0;
			return m_shapenamevec[srcindex];
		}
		else {
			_ASSERT(0);
			*perror = 1;
			return "error";
		}
	};

	ChaMatrix GetMeshMat()
	{
		return m_meshmat;
	};
	void SetMeshMat(ChaMatrix srcmat)
	{
		m_meshmat = srcmat;
	};

	int GetDbgCount()
	{
		return m_dbgcount;
	};
	void SetDbgCount(int srccount)
	{
		m_dbgcount = srccount;
	};

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
	
	void SetLODNum(int srcnum)
	{
		if (srcnum > m_lodnum) {
			m_lodnum = srcnum;
		}
	}
	int GetLODNum()
	{
		return m_lodnum;
	}

	void SetUVNum(int srcnum)
	{
		m_getuvnum = srcnum;
	}
	int GetUVNum()
	{
		return m_getuvnum;
	}

	void SetDistFromCamera(float srcval, int refposindex)
	{
		if ((refposindex < 0) || (refposindex >= REFPOSMAXNUM)) {
			_ASSERT(0);
			return;
		}
		m_frustum[refposindex].SetDistFromCamera(srcval);
	}
	double GetDistFromCamera(int refposindex)
	{
		if ((refposindex < 0) || (refposindex >= REFPOSMAXNUM)) {
			_ASSERT(0);
			return FLT_MAX;
		}
		return m_frustum[refposindex].GetDistFromCamera();
	}

	void SetCancelShadow(bool srcflag)
	{
		m_cancelshadow = srcflag;
	}
	bool GetCancelShadow()
	{
		return m_cancelshadow;
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
	ChaVector2* m_uvbuf1;

	//マテリアルを読み込み順で保存　CMQOMaterial*は外部ポインタ　破棄しない
	std::vector<CMQOMaterial*> m_onloadmaterial;

	std::vector<CBone*> m_cluster;//中身のCBone*は外部メモリ

	std::vector<std::string> m_shapenamevec;
	std::map<std::string,ChaVector3*> m_shapevert;
	std::map<std::string, std::map<int,float*>> m_shapeanim2;//複数アニメ対応
	std::map<int,int> m_shapeanimleng2;//複数アニメ対応

	FbxNode* m_pnode;

	ChaFrustumInfo m_frustum[REFPOSMAXNUM];
	bool m_cancelshadow;


//以下、クラス外から参照しないのでアクセッサー無し
	int m_patch;
	int m_segment;

	int m_visible;//mqofileのvisible. chkinviewのvisibleはm_frustum経由で取得
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
	//std::map<std::string,float> m_shapeweight;
	std::vector<float> m_shapeweightvec;//[channelindex]
	ChaVector3* m_mpoint;


	//fbx GetFBXMesh()
	int m_normalmappingmode;//0:eByPolygonVertex, 1:eByControlPoint

	ChaMatrix m_meshmat;
	int m_dbgcount;

	std::vector<LATERMATERIAL> m_latermaterial;

	int m_lodnum;
	int m_getuvnum;

};

#endif
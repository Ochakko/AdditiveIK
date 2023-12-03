#ifndef BNTFILEH
#define BNTFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <map>

#include <coef.h>

class CModel;
class CBone;
class CMQOObject;
class CMQOMaterial;
class BPWorld;

typedef struct tag_bntmaterial
{
	int materialno;
	char name[256];

	ChaVector4 dif4f;
	ChaVector3 amb3f;
	ChaVector3 emi3f;
	ChaVector3 spc3f;

	char tex[256];
}BNTMATERIAL;

typedef struct tag_bntbone
{
	int bonecnt;
	int validflag;
	int boneno;
	int topboneflag;
	char bonename[256];

	char parentname[256];
	char childname[256];
	char brothername[256];
}BNTBONE;

typedef struct tag_bntcluster
{
	char bonename[256];
	int boneno;
}BNTCLUSTER;

typedef struct tag_bntobjinfo
{
	char objname[256];
	int vertexnum;
	int facenum;
	BNTMATERIAL bntmaterial;
}BNTOBJINFO;

typedef struct tag_bntvertex
{
/*
	ChaVector4		pos;
	ChaVector3		normal;
	ChaVector2		uv;
*/
	BINORMALDISPV dispv;
/*
	float weight[4];
	int boneindex[4];
*/
	PM3INF inf;
}BNTVERTEX;

typedef struct tag_bntmotheader
{
	char motname[256];
	int frameleng;
	int bonenum;
}BNTMOTHEADER;

typedef struct tag_bntmotpoint
{
	int frameno;
	ChaMatrix matrix;
}BNTMOTPOINT;

class CBntFile : public CXMLIO
{
public:
	CBntFile();
	virtual ~CBntFile();

	int WriteBntFile(bool limitdegflag, WCHAR* wfilename, MODELELEM wme );

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteHeader();
	
	int WriteNumber( int srcnumber );

	int WriteBone( CModel* srcmodel );
	void WriteBntBoneReq( CBone* srcbone, int* pbonenum );

	int WriteObject( CModel* srcmodel );
	int WriteBntObjInfo( CMQOObject* srcobj );
	int WriteBntCluster( CBone* srcbone );
	int WriteBntVertex( BINORMALDISPV* srcpm3disp, PM3INF* srcpm3inf );
	int WriteBntIndex( int* srcindex, int indexnum );

	int WriteOneMotion(bool limitdegflag, CModel* srcmodel);
	void WriteMotionPointsReq(bool limitdegflag, CBone* srcbone, int srcmotid, int frameleng, int* pcnt);
	
private:
	MODELELEM m_me;

};

#endif

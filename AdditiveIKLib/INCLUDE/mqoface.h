#ifndef MQOFACEH
#define MQOFACEH

#include <stdio.h>
#include <stdarg.h>

#include <coef.h>
//#include <basedat.h>

#include <ChaVecCalc.h>

#define DBGH
#include <dbg.h>


class CMQOFace
{
public:
	CMQOFace();
	~CMQOFace();

	int SetParams( char* srcchar, int srcleng );

	int Dump();

	int CheckSameLine( CMQOFace* chkface, int* findflag );
	int SetInvFace( CMQOFace* srcface, int offset );

	//int SetMikoBoneName( char* srcname, int lrflag );
	int CheckLRFlag( ChaVector3* pointptr, int* lrflagptr );

private:
	void InitParams();

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetI64( __int64* dsti64, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );

public:
	//accesser
	int GetFaceNo(){
		return m_faceno;
	};
	void SetFaceNo( int srcno ){
		m_faceno = srcno;
	};

	int GetPointNum(){
		return m_pointnum;
	};
	void SetPointNum( int srcnum ){
		m_pointnum = srcnum;
	};

	int GetIndex( int srci ){
		if( (srci >= 0) && (srci < 4) ){
			return m_index[ srci ];
		}else{
			return 0;
		}
	};
	void SetIndex( int srci, int srcval ){
		if( (srci >= 0) && (srci < 4) ){
			m_index[ srci ] = srcval;
		}
	};

	int GetMaterialNo(){
		return m_materialno;
	};
	void SetMaterialNo( int srcval ){
		m_materialno = srcval;
	};

	int GetHasUV(){
		return m_hasuv;
	};
	void SetHasUV( int srcval ){
		m_hasuv = srcval;
	};

	ChaVector2 GetUV( int srci ){
		if( (srci >= 0) && (srci < 4) ){
			return m_uv[ srci ];
		}else{
			return ChaVector2( 0.0f, 0.0f );
		}
	};
	void SetUV( int srci, ChaVector2 srcval ){
		if( (srci >= 0) && (srci < 4) ){
			m_uv[ srci ] = srcval;
		}
	};
	ChaVector2 GetUV1(int srci) {
		if ((srci >= 0) && (srci < 4)) {
			return m_uv1[srci];
		}
		else {
			return ChaVector2(0.0f, 0.0f);
		}
	};
	void SetUV1(int srci, ChaVector2 srcval) {
		if ((srci >= 0) && (srci < 4)) {
			m_uv1[srci] = srcval;
		}
	};

	__int64 GetCol( int srci ){
		if( (srci >= 0) && (srci < 4) ){
			return m_col[ srci ];
		}else{
			return 0;
		}
	};
	void SetCol( int srci, __int64 srcval ){
		if( (srci >= 0) && (srci < 4) ){
			m_col[ srci ] = srcval;
		}
	};

	int GetVcolSetFlag(){
		return m_vcolsetflag;
	};
	void SetVcolSetFlag( int srcval ){
		m_vcolsetflag = srcval;
	};

	int GetBoneType(){
		return m_bonetype;
	};
	void SetBoneType( int srcval ){
		m_bonetype = srcval;
	};

	const char* GetBoneName(){
		return m_bonename;
	};
	void SetBoneName( char* srcname ){
		strcpy_s( m_bonename, 256, srcname );
	};

	int GetShapeNo(){
		return m_shapeno;
	};
	void SetShapeNo( int srcval ){
		m_shapeno = srcval;
	};

	CMQOFace* GetParent(){
		return m_parent;
	};
	void SetParent( CMQOFace* srcval ){
		m_parent = srcval;
	};

	CMQOFace* GetChild(){
		return m_child;
	};
	void SetChild( CMQOFace* srcval ){
		m_child = srcval;
	};

	CMQOFace* GetBrother(){
		return m_brother;
	};
	void SetBrother( CMQOFace* srcval ){
		m_brother = srcval;
	};

	int GetDirtyFlag(){
		return m_dirtyflag;
	};
	void SetDirtyFlag( int srcval ){
		m_dirtyflag = srcval;
	};

	int GetChildIndex(){
		return m_childindex;
	};

private:
	int m_faceno;
	int m_pointnum;
	int m_index[4];
	int m_materialno;
	
	int m_hasuv;
	ChaVector2 m_uv[4];
	ChaVector2 m_uv1[4];

	__int64 m_col[4];
	int m_vcolsetflag;


// for mikoto bone
	int m_bonetype; //MIKOBONE_NONE, MIKOBONE_NORMAL, MIKOBONE_FLOAT, MIKOBONE_ILLEAGAL
	char m_bonename[256];

	int m_shapeno;
	int m_dirtyflag;
	int m_childindex;

	CMQOFace* m_parent;
	CMQOFace* m_child;
	CMQOFace* m_brother;

//以下、クラス外からアクセスしないのでアクセッサー無し。
	int m_parentindex;
	int m_hindex;
	int m_serialno;

};

#endif

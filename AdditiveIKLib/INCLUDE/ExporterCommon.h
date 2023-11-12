//=============================================================================
// @file
//	RDB2プラグイン共通ヘッダー
//=============================================================================

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>
#include <coef_r.h>

#include <crtdbg.h>

class CModel;

#define NO_VCOLOR		(1)

#ifndef MAX
#define MAX(a,b)		((a>b)?a:b)
#endif
#ifndef MIN
#define MIN(a,b)		((a<b)?a:b)
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete [] (p); (p)=NULL; } }
#endif


void _MulMatrix( double *r, double *m1, double *m2 );
void _InvMatrix( double *r, double *m );
double _MatrixToRotX( double *mat );
double _MatrixToRotY( double *mat );
double _MatrixToRotZ( double *mat );
double _MatrixToRotX( RDBMatrix *mat );
double _MatrixToRotY( RDBMatrix *mat );
double _MatrixToRotZ( RDBMatrix *mat );
void _QuatToMat( double *mat, RDBQuaternion *quat );
void _QuatToMat( RDBMatrix *mat, RDBQuaternion *quat );
double _QuatToRotX( RDBQuaternion *quat );
double _QuatToRotY( RDBQuaternion *quat );
double _QuatToRotZ( RDBQuaternion *quat );

//-------------------------------------------------------------------
// DEBUG
//-------------------------------------------------------------------

//#ifdef _DEBUG
#if 1
inline void _DPrintf( char* pszFormat, ...) {
    va_list	argp;
    char pszBuf[512];
    va_start( argp, pszFormat );
    vsprintf_s( pszBuf, pszFormat, argp );
    va_end( argp );
	WCHAR wbuf[512];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, pszBuf, 512, wbuf, 512 );
    OutputDebugString( wbuf );
}
#else
#define	_DPrintf
#endif

//-------------------------------------------------------------------
// 例外クラス Exception
//-------------------------------------------------------------------

class RDBPlgException {
public:
	RDBPlgException() {};
	virtual ~RDBPlgException() {};
};

// RDB2プラグイン関数 RESULTエラー
class RDBPlgExceptionResult : public RDBPlgException {
public:
	RDBPlgExceptionResult() {};
	virtual ~RDBPlgExceptionResult() {};
};

// RDB2プラグイン関数 メモリ不足
class RDBPlgExceptionNoMemory : public RDBPlgException {
public:
	RDBPlgExceptionNoMemory() {};
	virtual ~RDBPlgExceptionNoMemory() {};
};

// RDB2プラグイン関数 不正なキャスト
class RDBPlgExceptionIllegalCast : public RDBPlgException {
public:
	RDBPlgExceptionIllegalCast() {};
	virtual ~RDBPlgExceptionIllegalCast() {};
};

//===================================================================
// RDB2 データアクセス
//===================================================================

//---------------------------------------------------------
// テクスチャ情報
//---------------------------------------------------------

class RDBPlgTexture {
public:
	RDBPlgTexture();
	virtual ~RDBPlgTexture();	

public:
	// 名前
	inline void SetName( const char *name ) { m_Name = name; }
	inline const char *GetName() { return m_Name.c_str(); }

	// ファイル名
	inline void SetFileName( const char *name ) { m_FileName = name; }
	inline const char *GetFileName() { return m_FileName.c_str(); }

public:
	std::string		m_Name;
	std::string		m_FileName;
};

class RDBPlgTextures {
public:
	RDBPlgTextures();
	virtual ~RDBPlgTextures();	
	
public:
	int SetModel( CModel* srcmodel );

	bool	Create();
	void	Release();

	// テクスチャ情報取得
	inline int GetTextureCount() { return m_Textures.size(); }
	inline RDBPlgTexture *GetTexture( int texno ) { return m_Textures.at(texno); }

	// テクスチャ検索
	RDBPlgTexture* SearchTextureName( std::string &name );

protected:
	void	InitData();

	RDBPlgTexture* AddTexture( std::string &name, std::string &filename );
	
protected:
	CModel*		m_model;
	std::vector<RDBPlgTexture*>		m_Textures;
};

//---------------------------------------------------------
// オブジェクト情報
//---------------------------------------------------------

class RDBPlgDispObjects {
public:
	RDBPlgDispObjects();
	virtual ~RDBPlgDispObjects();

public:
	int SetModel( CModel* srcmodel ){ m_model = srcmodel; return 0; };
	void Release();

protected:
	void InitData();

protected:
	CModel* m_model;

};

//---------------------------------------------------------
// メッシュ情報
//---------------------------------------------------------

class RDBPlgTriangle {
public:
	RDBPlgTriangle() { m_VertexNo[0] = m_VertexNo[1] = m_VertexNo[2] = 0; }
	virtual ~RDBPlgTriangle() {}

	void SetVertexNo( int idx, int vno ) { m_VertexNo[idx] = vno; }
	int GetVertexNo( int idx ) { return m_VertexNo[idx]; }

protected:
	int	m_VertexNo[3];
};

class RDBPlgVertex {
public:
	RDBPlgVertex() : m_Pos(0,0,0), m_U(0), m_V(0) {};
	virtual ~RDBPlgVertex() {};

	inline void SetRDBVertexNo( int vno ) { m_RDBVertexNo = vno; }
	inline void SetPos( RDBPoint &pos ) { m_Pos = pos; }
	inline void SetNormal( RDBPoint &nrm ) { m_Normal = nrm; }
	inline void SetTexCoord( float u, float v ) { m_U = u; m_V = v; }

	inline int   GetRDBVertexNo() { return m_RDBVertexNo; }
	inline const RDBPoint *GetPos() { return &m_Pos; }
	inline const RDBPoint *GetNormal() { return &m_Normal; }
	inline float GetTexCoordU() { return m_U; }
	inline float GetTexCoordV() { return m_V; }

public:
	int			m_RDBVertexNo;
	RDBPoint	m_Pos;
	RDBPoint	m_Normal;
	float		m_U;
	float		m_V;
};


class RDBPlgMesh {
public:
	RDBPlgMesh();
	virtual ~RDBPlgMesh();

public:
	void Release();

	inline void SetDispObjNo( int dispobjno ) { m_DispObjNo = dispobjno; }
	inline int  GetDispObjNo() { return m_DispObjNo; }

	// 名前
	inline void SetName( const char *name ) { m_Name = name; }
	inline const char* GetName() { return m_Name.c_str(); }

	// マテリアル
	inline void SetMaterial( int mtrno ) { m_MaterialNo = mtrno; }
	inline int GetMaterial() { return m_MaterialNo; }

	RDBPlgVertex* AddVertex();
	inline int GetVertexCount() { return m_Vertices.size(); }
	inline RDBPlgVertex *GetVertex( int vertno ) { return m_Vertices.at(vertno); }

	RDBPlgTriangle* AddTriangle();
	inline int GetTriangleCount() { return m_Triangles.size(); }
	inline RDBPlgTriangle *GetTriangle( int trino ) { return m_Triangles.at(trino); }

protected:
	int				m_DispObjNo;
	std::string		m_Name;
	int				m_MaterialNo;
	std::vector<RDBPlgVertex*>		m_Vertices;
	std::vector<RDBPlgTriangle*>	m_Triangles;

};

class RDBPlgMeshes {
public:
	RDBPlgMeshes();
	virtual ~RDBPlgMeshes();

public:
	int SetModel( CModel* srcmodel ){ m_model = srcmodel; Create(); return 0; };

	bool Create();
	void Release();

	RDBPlgMesh *AddMesh( std::string &name );
	inline int GetMeshCount() { return m_Meshes.size(); }
	inline RDBPlgMesh *GetMesh( int meshno ) { return m_Meshes.at(meshno); }

protected:
	void InitData();

protected:
	CModel*	m_model;
	std::vector<RDBPlgMesh*>		m_Meshes;

};

//---------------------------------------------------------
// 関節情報
//---------------------------------------------------------

class RDBPlgJoint {

public:
	RDBPlgJoint();
	virtual ~RDBPlgJoint();

public:
	inline RDBPlgJoint* GetParent() { return m_pParent; }
	inline RDBPlgJoint* GetChild() { return m_pChild; }
	inline RDBPlgJoint* GetNext() { return m_pNext; }
	inline RDBPlgJoint* GetPrev() { return m_pPrev; }

	void AddChild( RDBPlgJoint* );
	void RemoveChild( RDBPlgJoint* );

	// 名前
	inline void SetName( const char *name ) { m_Name = name; }
	inline const char* GetName() { return m_Name.c_str(); }

	// パーツ番号
	inline void SetPartNo( int partno ) { m_PartNo = partno; }
	inline int GetPartNo() { return m_PartNo; }

	// マトリクス
	inline void SetUnitMatrix() {
		for( int i=0; i<16; i++ ) { m_Matrix[i] = ((i/4)==(i%4))?1.0:0.0; }
	}
	inline void SetMatrix( RDBMatrix *matrix ) {
		m_Matrix[ 0 * 4 + 0 ] =  matrix->_11;
		m_Matrix[ 1 * 4 + 0 ] =  matrix->_12;
		m_Matrix[ 2 * 4 + 0 ] =  matrix->_13;
		m_Matrix[ 3 * 4 + 0 ] =  matrix->_14;
		m_Matrix[ 0 * 4 + 1 ] =  matrix->_21;
		m_Matrix[ 1 * 4 + 1 ] =  matrix->_22;
		m_Matrix[ 2 * 4 + 1 ] =  matrix->_23;
		m_Matrix[ 3 * 4 + 1 ] =  matrix->_24;
		m_Matrix[ 0 * 4 + 2 ] =  matrix->_31;
		m_Matrix[ 1 * 4 + 2 ] =  matrix->_32;
		m_Matrix[ 2 * 4 + 2 ] =  matrix->_33;
		m_Matrix[ 3 * 4 + 2 ] =  matrix->_34;
		m_Matrix[ 0 * 4 + 3 ] = -matrix->_41;
		m_Matrix[ 1 * 4 + 3 ] = -matrix->_42;
		m_Matrix[ 2 * 4 + 3 ] = -matrix->_43;
		m_Matrix[ 3 * 4 + 3 ] =  matrix->_44;
	}

	inline void GetMatrix( double matrix[] ) { memcpy( matrix, m_Matrix, sizeof(double)*16 ); }
	inline void GetInvMatrix( double matrix[] ) { _InvMatrix( matrix, m_Matrix ); }
	inline void GetLocalMatrix( double matrix[] ) {
		double parent_matrix[16];
		RDBPlgJoint *parent = GetParent();
		if( parent!=NULL ) {
			parent->GetInvMatrix( parent_matrix );
			_MulMatrix( matrix, parent_matrix, m_Matrix );
		} else {
			GetMatrix( matrix );
		}
	}
	inline void GetInvLocalMatrix( double matrix[] ) {
		GetLocalMatrix( matrix );
		_InvMatrix( matrix, matrix );
	}

	// ユーザデータ
	inline void SetUserData( void *ptr ) { m_pUserData = ptr; }
	inline void* GetUserData() { return m_pUserData; }

protected:
	inline void SetParent( RDBPlgJoint *jnt ) { m_pParent = jnt; }
	inline void SetChild ( RDBPlgJoint *jnt ) { m_pChild  = jnt; }
	inline void SetNext  ( RDBPlgJoint *jnt ) { m_pNext   = jnt; }
	inline void SetPrev  ( RDBPlgJoint *jnt ) { m_pPrev   = jnt; }

protected:
	RDBPlgJoint*	m_pParent;
	RDBPlgJoint*	m_pChild;
	RDBPlgJoint*	m_pNext;
	RDBPlgJoint*	m_pPrev;

	std::string		m_Name;
	int				m_PartNo;
	double			m_Matrix[4*4];
	void*			m_pUserData;
};

class RDBPlgJoints {
public:
	RDBPlgJoints();
	virtual ~RDBPlgJoints();

public:
	int SetModel( CModel* srcmodel );

	bool	Create();
	void	Release();

	inline int			GetJointCount() { return m_JointCount; }
	inline const char*	GetJointName( int jointno ) { _ASSERT( m_pJointNameStr ); if( (jointno >= 0) && (jointno < m_JointCount) ){ return &m_pJointNameStr[ 256 * jointno ]; }else{ _ASSERT( 0 ); return &m_pJointNameStr[ 0 ];} }

	inline RDBPlgJoint*	GetJointRoot() { return m_pJointRoot; }
	inline RDBPlgJoint*	GetJoint( int jointno ) { return &m_pJoints[jointno]; }

	inline int GetJointIndexFromPartNo( int partno ) {
		for( int i=0; i<m_JointCount; i++ ) {
			if( m_pJoints[i].GetPartNo()==partno ) { return i; }
		}
		return -1;
	}

protected:
	void	InitData();

protected:
	CModel*			m_model;

	int				m_JointCount;
	char*			m_pJointNameStr;

	RDBPlgJoint*	m_pJointRoot;
	RDBPlgJoint*	m_pJoints;
};

//---------------------------------------------------------
// RDBモデル情報
//---------------------------------------------------------

class RDBPlgInfo {
public:
	RDBPlgInfo( CModel* srcmodel );
	virtual ~RDBPlgInfo();

public:
	int SetModel();
	int GetDispObjCount();				// 表示オブジェ数取得

	int GetMeshCount();					// メッシュカウント
	RDBPlgMesh* GetMesh( int meshno );	// メッシュ取得

	int GetJointCount();				// ジョイント数取得
	int GetJointIndexFromPartNo( int partno ); // パーツ番号からジョイント番号に変換
	const char* GetJointName( int jointno );	// ジョイント名取得
	RDBPlgJoint* GetJointRoot();			// ルートジョイントを取得
	RDBPlgJoint* GetJoint( int jointno );

	int GetTextureCount();
	RDBPlgTexture* GetTexture( int texno );
	RDBPlgTexture* SearchTextureName( std::string &name );

protected:
	RDBPlgDispObjects	m_Objs;
	RDBPlgJoints		m_Joints;
	RDBPlgTextures		m_Textures;
	RDBPlgMeshes		m_Meshes;
public:
	CModel*	m_model;
};

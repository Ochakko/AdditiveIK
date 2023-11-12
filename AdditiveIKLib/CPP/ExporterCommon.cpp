#include "stdafx.h"
//=============================================================================
// @file
//	RDB2プラグイン共通ヘッダー
//=============================================================================

#include <new>
#include <windows.h>
#include <list>

#include "coef_r.h"
#include "ExporterCommon.h"

#include <Model.h>
#include <TexBank.h>
#include <Bone.h>
#include <mqoobject.h>
#include <polymesh3.h>
#include <MqoMaterial.h>

//#include <VecMath.h>
#include <ChaCalc.h>


#include <crtdbg.h>

//===================================================================
//
// math func
//
//===================================================================

// 掛け算
void _MulMatrix( double *r, double *m1, double *m2 ) {
	double _r[16];
	for( int i=0; i<4; i++ ) {
		for( int j=0; j<4; j++ ) {
			_r[i*4+j] = m1[i*4+0]*m2[0*4+j] + m1[i*4+1]*m2[1*4+j] + m1[i*4+2]*m2[2*4+j] + m1[i*4+3]*m2[3*4+j];
		}
	}
	memcpy( r, _r, sizeof(double)*16 );
}

// 逆行列
void _InvMatrix( double *r, double *m ) {
	double _r[16];
	// 回転分転置
	_r[ 0 * 4 + 0 ] = m[ 0 * 4 + 0 ];
	_r[ 1 * 4 + 0 ] = m[ 0 * 4 + 1 ];
	_r[ 2 * 4 + 0 ] = m[ 0 * 4 + 2 ];
	_r[ 3 * 4 + 0 ] = 0.0;
	_r[ 0 * 4 + 1 ] = m[ 1 * 4 + 0 ];
	_r[ 1 * 4 + 1 ] = m[ 1 * 4 + 1 ];
	_r[ 2 * 4 + 1 ] = m[ 1 * 4 + 2 ];
	_r[ 3 * 4 + 1 ] = 0.0;
	_r[ 0 * 4 + 2 ] = m[ 2 * 4 + 0 ];
	_r[ 1 * 4 + 2 ] = m[ 2 * 4 + 1 ];
	_r[ 2 * 4 + 2 ] = m[ 2 * 4 + 2 ];
	_r[ 3 * 4 + 2 ] = 0.0;
	// 移動量分
	_r[ 0 * 4 + 3 ] = -(m[ 0 * 4 + 3 ]*m[ 0 * 4 + 0 ]+
						m[ 0 * 4 + 3 ]*m[ 1 * 4 + 0 ]+
						m[ 0 * 4 + 3 ]*m[ 2 * 4 + 0 ]+
						m[ 0 * 4 + 3 ]*m[ 3 * 4 + 0 ]);
	_r[ 1 * 4 + 3 ] = -(m[ 1 * 4 + 3 ]*m[ 0 * 4 + 1 ]+
						m[ 1 * 4 + 3 ]*m[ 1 * 4 + 1 ]+
						m[ 1 * 4 + 3 ]*m[ 2 * 4 + 1 ]+
						m[ 1 * 4 + 3 ]*m[ 3 * 4 + 1 ]);
	_r[ 2 * 4 + 3 ] = -(m[ 2 * 4 + 3 ]*m[ 0 * 4 + 2 ]+
						m[ 2 * 4 + 3 ]*m[ 1 * 4 + 2 ]+
						m[ 2 * 4 + 3 ]*m[ 2 * 4 + 2 ]+
						m[ 2 * 4 + 3 ]*m[ 3 * 4 + 2 ]);
	_r[ 3 * 4 + 3 ] = 1.0;
	memcpy( r, _r, sizeof(double)*16 );
}

// かけ順はX,Y,Zとする。戻り値は度。左手座標系
double _MatrixToRotX( double *mat ) {
	if( fabs( mat[0*4+2] )==1 ) {
		return atan2( mat[2*4+1], mat[1*4+1] ) * 180 / M_PI;
	}
	return atan2( -mat[1*4+2], mat[2*4+2] ) * 180 / M_PI;
}

double _MatrixToRotY( double *mat ) {
	double m02 = mat[0*4+2];
	if( m02== 1 ) { return  90; }
	if( m02==-1 ) { return -90; }
	return asin( m02 ) * 180 / M_PI;
}

double _MatrixToRotZ( double *mat ) {
	if( fabs( mat[0*4+2] )==1 ) {
		return 0;
	}
	return atan2( -mat[0*4+1], mat[0*4+0] ) * 180 / M_PI;
}

double _MatrixToRotX( RDBMatrix *mat ) {
	double _mat[16];
	_mat[0*4+0] = mat->_11;
	_mat[0*4+1] = mat->_12;
	_mat[0*4+2] = mat->_13;
	_mat[0*4+3] = mat->_14;
	_mat[1*4+0] = mat->_21;
	_mat[1*4+1] = mat->_22;
	_mat[1*4+2] = mat->_23;
	_mat[1*4+3] = mat->_24;
	_mat[2*4+0] = mat->_31;
	_mat[2*4+1] = mat->_32;
	_mat[2*4+2] = mat->_33;
	_mat[2*4+3] = mat->_34;
	_mat[3*4+0] = mat->_41;
	_mat[3*4+1] = mat->_42;
	_mat[3*4+2] = mat->_43;
	_mat[3*4+3] = mat->_44;
	return _MatrixToRotX( _mat );
}
double _MatrixToRotY( RDBMatrix *mat ) {
	double _mat[16];
	_mat[0*4+0] = mat->_11;
	_mat[0*4+1] = mat->_12;
	_mat[0*4+2] = mat->_13;
	_mat[0*4+3] = mat->_14;
	_mat[1*4+0] = mat->_21;
	_mat[1*4+1] = mat->_22;
	_mat[1*4+2] = mat->_23;
	_mat[1*4+3] = mat->_24;
	_mat[2*4+0] = mat->_31;
	_mat[2*4+1] = mat->_32;
	_mat[2*4+2] = mat->_33;
	_mat[2*4+3] = mat->_34;
	_mat[3*4+0] = mat->_41;
	_mat[3*4+1] = mat->_42;
	_mat[3*4+2] = mat->_43;
	_mat[3*4+3] = mat->_44;
	return _MatrixToRotY( _mat );
}
double _MatrixToRotZ( RDBMatrix *mat ) {
	double _mat[16];
	_mat[0*4+0] = mat->_11;
	_mat[0*4+1] = mat->_12;
	_mat[0*4+2] = mat->_13;
	_mat[0*4+3] = mat->_14;
	_mat[1*4+0] = mat->_21;
	_mat[1*4+1] = mat->_22;
	_mat[1*4+2] = mat->_23;
	_mat[1*4+3] = mat->_24;
	_mat[2*4+0] = mat->_31;
	_mat[2*4+1] = mat->_32;
	_mat[2*4+2] = mat->_33;
	_mat[2*4+3] = mat->_34;
	_mat[3*4+0] = mat->_41;
	_mat[3*4+1] = mat->_42;
	_mat[3*4+2] = mat->_43;
	_mat[3*4+3] = mat->_44;
	return _MatrixToRotZ( _mat );
}

void _QuatToMat( double *mat, RDBQuaternion *quat ) {

	float qw, qx, qy, qz;
	float x2, y2, z2;
	float xy, yz, zx;
	float wx, wy, wz;

	qw = quat->w; qx = quat->x; qy = quat->y; qz = quat->z;

	x2 = 2.0f * qx * qx;
	y2 = 2.0f * qy * qy;
	z2 = 2.0f * qz * qz;

	xy = 2.0f * qx * qy;
	yz = 2.0f * qy * qz;
	zx = 2.0f * qz * qx;

	wx = 2.0f * qw * qx;
	wy = 2.0f * qw * qy;
	wz = 2.0f * qw * qz;

	mat[0*4+0] = 1.0f - y2 - z2;
	mat[1*4+0] = xy - wz;
	mat[2*4+0] = zx + wy;
	mat[3*4+0] = 0.0f;

	mat[0*4+1] = xy + wz;
	mat[1*4+1] = 1.0f - z2 - x2;
	mat[2*4+1] = yz - wx;
	mat[3*4+1] = 0.0f;

	mat[0*4+2] = zx - wy;
	mat[1*4+2] = yz + wx;
	mat[2*4+2] = 1.0f - x2 - y2;
	mat[3*4+2] = 0.0f;

	mat[0*4+3] = mat[1*4+3] = mat[2*4+3] = 0.0f;
	mat[3*4+3] = 1.0f;

}

void _QuatToMat( RDBMatrix *mat, RDBQuaternion *quat ) {

	double _mat[16];
	_QuatToMat( _mat, quat );
	mat->_11 = (float)_mat[0*4+0];
	mat->_12 = (float)_mat[0*4+1];
	mat->_13 = (float)_mat[0*4+2];
	mat->_14 = (float)_mat[0*4+3];
	mat->_21 = (float)_mat[1*4+0];
	mat->_22 = (float)_mat[1*4+1];
	mat->_23 = (float)_mat[1*4+2];
	mat->_24 = (float)_mat[1*4+3];
	mat->_31 = (float)_mat[2*4+0];
	mat->_32 = (float)_mat[2*4+1];
	mat->_33 = (float)_mat[2*4+2];
	mat->_34 = (float)_mat[2*4+3];
	mat->_41 = (float)_mat[3*4+0];
	mat->_42 = (float)_mat[3*4+1];
	mat->_43 = (float)_mat[3*4+2];
	mat->_44 = (float)_mat[3*4+3];

}

double _QuatToRotX( RDBQuaternion *quat ) {
	double mat[16];
	_QuatToMat( mat, quat );
	return _MatrixToRotX( mat );
}

double _QuatToRotY( RDBQuaternion *quat ) {
	double mat[16];
	_QuatToMat( mat, quat );
	return _MatrixToRotY( mat );
}

double _QuatToRotZ( RDBQuaternion *quat ) {
	double mat[16];
	_QuatToMat( mat, quat );
	return _MatrixToRotZ( mat );
}


//===================================================================
//
// class RDBPlgDispObjects
//
//===================================================================

//---------------------------------------------------------
//
// コンストラクタ／デストラクタ
//
//---------------------------------------------------------

RDBPlgDispObjects::RDBPlgDispObjects() {
	InitData();
}

RDBPlgDispObjects::~RDBPlgDispObjects() {
	Release();
}

//---------------------------------------------------------
//
// 初期化
//
//---------------------------------------------------------

void RDBPlgDispObjects::InitData() {
	m_model = 0;
}

//---------------------------------------------------------
//
// 解放
//
//---------------------------------------------------------

void RDBPlgDispObjects::Release() {
}


//===================================================================
//
// class RDBPlgMesh
//
//===================================================================

RDBPlgMesh::RDBPlgMesh() {
}
RDBPlgMesh::~RDBPlgMesh() {
	Release();
}

//---------------------------------------------------------
//
// 解放
//
//---------------------------------------------------------

void RDBPlgMesh::Release() {

	std::vector<RDBPlgVertex*>::iterator vert_ite;
	for( vert_ite=m_Vertices.begin(); vert_ite!=m_Vertices.end(); vert_ite++ ) {
		delete (*vert_ite);
	}
	m_Vertices.clear();

	std::vector<RDBPlgTriangle*>::iterator tri_ite;
	for( tri_ite=m_Triangles.begin(); tri_ite!=m_Triangles.end(); tri_ite++ ) {
		delete (*tri_ite);
	}
	m_Triangles.clear();

}

//---------------------------------------------------------
//
// 頂点追加
//
//---------------------------------------------------------

RDBPlgVertex* RDBPlgMesh::AddVertex() {

	RDBPlgVertex *vert = new RDBPlgVertex();

	m_Vertices.push_back( vert );

	return vert;

}

//---------------------------------------------------------
//
// トライアングル追加
//
//---------------------------------------------------------

RDBPlgTriangle* RDBPlgMesh::AddTriangle() {

	RDBPlgTriangle *tri = new RDBPlgTriangle();

	m_Triangles.push_back( tri );

	return tri;

}

//===================================================================
//
// class RDBPlgMeshes
//
//===================================================================

//---------------------------------------------------------
//
// コンストラクタ／デストラクタ
//
//---------------------------------------------------------

RDBPlgMeshes::RDBPlgMeshes() {
	InitData();
}

RDBPlgMeshes::~RDBPlgMeshes() {
	Release();
}

//---------------------------------------------------------
//
// 初期化
//
//---------------------------------------------------------

void RDBPlgMeshes::InitData() {
	m_model = 0;
}

//---------------------------------------------------------
//
// メッシュ追加
//
//---------------------------------------------------------

RDBPlgMesh *RDBPlgMeshes::AddMesh( std::string &name ) {

	RDBPlgMesh *mesh = new RDBPlgMesh();
	mesh->SetName( name.c_str() );

	m_Meshes.push_back( mesh );

	return mesh;

}

//---------------------------------------------------------
//
// 構築 / 解放
//
//---------------------------------------------------------

bool RDBPlgMeshes::Create() {


	RDBPoint* normalptr = NULL;
	int*   indices     = NULL;

	Release();

	try {
		int meshcnt = 0;
		map<int, CMQOObject*>::iterator itrobj;
		for( itrobj = m_model->m_object.begin(); itrobj != m_model->m_object.end(); itrobj++ ){
			/* 全表示オブジェクトを取得 */
			CMQOObject* curobj = itrobj->second;
			_ASSERT( curobj );
			CPolyMesh3* pm3 = curobj->m_pm3;
			if( !pm3 ){
				continue;
			}

			char name[256];
			std::string meshID;

			int mtrnum     = 0;
			int facenum    = 0;
			int vertnum    = 0;
			int indexnum   = 0;
			int revflag;
			int faceno;
			int mtrno;
			int vertno;
			std::list<int> mtrlist;
			std::list<int>::iterator mtrite;

			/* マテリアル総数 */
			mtrnum = pm3->m_optmatnum;

			/* 頂点数 */
			vertnum = pm3->m_optleng;

			/* 面数 */
			facenum = pm3->m_facenum;

			/* インデックス数 */
			indexnum = facenum * 3;
			revflag = 0;

			/* インデックス情報取得 */
			indices = pm3->m_dispindex;

			/* 法線情報取得 */
			if( vertnum > 0 ) {

				normalptr = (RDBPoint*)new RDBPoint[vertnum];
	
				int vcnt;
				for( vcnt = 0; vcnt < vertnum; vcnt++ ){
					PM3DISPV* curv = pm3->m_dispv + vcnt;
					RDBPoint* dstn = normalptr + vcnt;
					dstn->x = curv->normal.x;
					dstn->y = curv->normal.y;
					dstn->z = curv->normal.z;
				}
			}

			/* マテリアル毎 */
			int mtrcnt;
			for( mtrcnt = 0; mtrcnt < mtrnum; mtrcnt++ ) {
				MATERIALBLOCK* curmblock = pm3->m_matblock + mtrcnt;

				std::list<int> vertlist;
				std::list<int>::iterator vertite;

				mtrno = curmblock->materialno;

				/* メッシュを追加 */
				//sprintf_s( name, 256, "MESH-%d", GetMeshCount() );
				sprintf_s( name, 256, "MESH-%d", meshcnt );
				meshcnt++;
				meshID = name;
				RDBPlgMesh *mesh = AddMesh( meshID );

				mesh->SetDispObjNo( curobj->m_objectno );
				mesh->SetMaterial( mtrno );

				int startface = curmblock->startface;
				int endface = curmblock->endface;

				int facen;
				for( facen = startface; facen <= endface; facen++ ){
					int vcnt;
					for( vcnt = 0; vcnt < 3; vcnt++ ){
						vertno = indices[ facen * 3 + vcnt ];
						RDBPlgVertex *vert = mesh->AddVertex();
						vert->SetRDBVertexNo( vertno );
						RDBPoint vpos;
						ChaVector3 pos0 = (pm3->m_dispv + vertno)->pos;
						vpos.x = pos0.x;
						vpos.y = pos0.y;
						vpos.z = pos0.z;
						vert->SetPos( vpos );

						float u, v;
						u = (pm3->m_dispv + vertno)->uv.x;
						v = (pm3->m_dispv + vertno)->uv.y;
						vert->SetTexCoord( u, v );

						/* 法線 */
						vert->SetNormal( normalptr[vertno] );

					}
				}

				/* 面の追加 */
				int vertcnt = 0;
				for( faceno=startface; faceno<=endface; faceno++ ) {
					RDBPlgTriangle *tri = mesh->AddTriangle();
					tri->SetVertexNo( 0, vertcnt );
					tri->SetVertexNo( 1, vertcnt + 1 );
					tri->SetVertexNo( 2, vertcnt + 2 );
					vertcnt += 3;
				}
			}

			SAFE_DELETE_ARRAY( normalptr );

		}

	}
	catch( std::bad_alloc ) { // メモリ確保失敗
		goto L_ERROR_BAD_ALLOC;
	}


	return true;

L_ERROR_BAD_ALLOC:
	throw RDBPlgExceptionNoMemory();
	return false;

}

void RDBPlgMeshes::Release() {
	std::vector<RDBPlgMesh*>::iterator ite;
	for( ite=m_Meshes.begin(); ite!=m_Meshes.end(); ite++ ) {
		delete (*ite);
	}
	m_Meshes.clear();
}

//===================================================================
//
// class RDBPlgTexture
//
//===================================================================

//---------------------------------------------------------
//
// コンストラクタ／デストラクタ
//
//---------------------------------------------------------

RDBPlgTexture::RDBPlgTexture() {
}

RDBPlgTexture::~RDBPlgTexture() {
}

//===================================================================
//
// class RDBPlgTextures
//
//===================================================================

//---------------------------------------------------------
//
// コンストラクタ／デストラクタ
//
//---------------------------------------------------------

RDBPlgTextures::RDBPlgTextures() : m_Textures() {
	InitData();
}

RDBPlgTextures::~RDBPlgTextures() {
	Release();
}

//---------------------------------------------------------
//
// 初期化
//
//---------------------------------------------------------

void RDBPlgTextures::InitData() {
	m_model = 0;
}

int RDBPlgTextures::SetModel( CModel* srcmodel )
{ 
	m_model = srcmodel; 
	Create(); 
	return 0; 
}

//---------------------------------------------------------
//
// 構築 / 解放
//
//---------------------------------------------------------

bool RDBPlgTextures::Create() {



//	char*	dispobjname = NULL;
//	int*	dispobjinfo = NULL;
	int		dispobjnum = 0;

	Release();

	map<int, CMQOMaterial*>::iterator itrmat;
	for( itrmat = m_model->m_material.begin(); itrmat != m_model->m_material.end(); itrmat++ ){
		CMQOMaterial* curmat = itrmat->second;
		_ASSERT( curmat );

		if( curmat->tex[0] ){
			char texname[256];
			string strtex;
			string strtexpath;

			strtexpath = curmat->tex;
			char* lasten;
			lasten = strrchr( curmat->tex, '\\' );
			if( lasten ){
				strcpy_s( texname, 256, lasten + 1 );
			}else{
				strcpy_s( texname, 256, curmat->tex );
			}
			strtex = texname;

			std::string::size_type ich    = 0;
			/* . -> _ に置換 */
			while( (ich=strtex.find( ".", ich ))!=std::string::npos ) { strtex.replace( ich, 1, "_" ); ich++; }


			RDBPlgTexture *tex = NULL;
			tex = SearchTextureName( strtex );
			if( tex==NULL ) {
				AddTexture( strtex, strtexpath );
			}

		}
	}

	return true;

}

void RDBPlgTextures::Release() {
	std::vector<RDBPlgTexture*>::iterator ite;
	for( ite=m_Textures.begin(); ite!=m_Textures.end(); ite++ ) {
		delete (*ite);
	}
	m_Textures.clear();
}

//---------------------------------------------------------
//
// テクスチャ検索(名前)
//
//---------------------------------------------------------

RDBPlgTexture* RDBPlgTextures::SearchTextureName( std::string &name ) {

	int ntex = GetTextureCount();
	int i;

	for( i=0; i<ntex; i++ ) {

		RDBPlgTexture *tex = GetTexture( i );
		if( tex->GetName()==name ) {
			return tex;
		}

	}

	return NULL;

}

//---------------------------------------------------------
//
// テクスチャ追加
//
//---------------------------------------------------------

RDBPlgTexture* RDBPlgTextures::AddTexture( std::string &name, std::string &filename )
{

	RDBPlgTexture *tex = new RDBPlgTexture();
	tex->SetName( name.c_str() );
	tex->SetFileName( filename.c_str() );

	m_Textures.push_back( tex );

	return tex;

}


//===================================================================
//
// class RDBPlgJoint
//
//===================================================================

//---------------------------------------------------------
//
// コンストラクタ／デストラクタ
//
//---------------------------------------------------------

RDBPlgJoint::RDBPlgJoint() {
	m_PartNo    = -1;
	m_pParent   = NULL;
	m_pChild    = NULL;
	m_pNext     = NULL;
	m_pPrev     = NULL;
	m_pUserData = NULL;
	SetUnitMatrix();
}
RDBPlgJoint::~RDBPlgJoint() {
}

//---------------------------------------------------------
//
// 子から削除
//
//---------------------------------------------------------

void RDBPlgJoint::RemoveChild( RDBPlgJoint *remove ) {

	RDBPlgJoint *remove_parent = remove->GetParent();
	RDBPlgJoint *remove_prev   = remove->GetPrev();
	RDBPlgJoint *remove_next   = remove->GetNext();
	RDBPlgJoint *remove_child  = remove->GetChild();

	if( remove_parent!=this ) { return; } // 親が違う

	/* 兄弟がいる？ */
	if( remove_prev!=NULL ) {
		remove_prev->SetNext( remove_next );
	}

	/* 親がいる？ */
	if( remove_parent!=NULL ) {
		if( remove_next!=NULL ) {
			remove_parent->SetChild( remove_next );
		} else {
			remove_parent->SetChild( remove_child );
		}
	}

	/* 接続を解除 */
	remove->SetParent( NULL );
	remove->SetChild( NULL );
	remove->SetNext( NULL );
}
	
//---------------------------------------------------------
//
// 子に追加
//
//---------------------------------------------------------

void RDBPlgJoint::AddChild( RDBPlgJoint *add ) {

	RDBPlgJoint *add_parent = add->GetParent();
	RDBPlgJoint *child = GetChild();

	/* すでに別の親に接続されていたら一旦解除 */
	if( add_parent!=NULL ) {
		add_parent->RemoveChild( add );
	}

	/* 親を登録 */
	add->SetParent( this );

	/* 子が存在しなければ子として登録 */
	if( child==NULL ) {
		this->SetChild( add );
	}

	/* 子が存在すれば、その子の兄弟として登録 */
	else {
		RDBPlgJoint *_child = child;
		RDBPlgJoint *next;

		while( (next = _child->GetNext())!=NULL ) { _child = next; }

		if( _child!=NULL ) {
			_child->SetNext( add );
			add->SetPrev( _child );
		}
	}

}


//===================================================================
//
// class RDBPlgJoints
//
//===================================================================

//---------------------------------------------------------
//
// コンストラクタ／デストラクタ
//
//---------------------------------------------------------

RDBPlgJoints::RDBPlgJoints() {
	InitData();
}

RDBPlgJoints::~RDBPlgJoints() {
	Release();
}

//-------------------------------------------------------------------
//
// 初期化
//
//-------------------------------------------------------------------

void RDBPlgJoints::InitData() {
	m_model          = 0;
	m_JointCount    = 0;
	m_pJointNameStr = NULL;
//	m_pJointInfo    = NULL;
	m_pJoints       = NULL;
	m_pJointRoot    = NULL;
}

int RDBPlgJoints::SetModel( CModel* srcmodel ){ 
	m_model = srcmodel; 
	Create(); 
	return 0; 
};


//-------------------------------------------------------------------
//
// 解放
//
//-------------------------------------------------------------------

void RDBPlgJoints::Release() {
	if( m_pJointNameStr != NULL ) { delete [] m_pJointNameStr; m_pJointNameStr = 0; }
	if( m_pJoints != NULL ) { delete [] m_pJoints; m_pJoints = 0; }
}

//-------------------------------------------------------------------
//
// 構築
//
//-------------------------------------------------------------------

bool RDBPlgJoints::Create() {

	int jointnum = 0;

	Release();

	try {

		/* ジョイント数取得 */
		if( m_model->m_topbone ){
			jointnum = m_model->m_bonelist.size();
		}else{
			jointnum = 0;
		}

		if( jointnum > 0 ) {

			float wmult = 1.0f;
			m_model->CalcXTransformMatrix( wmult );

#if 1 // skeleton_root を使わない形式

			/* 領域確保 */
			m_pJointNameStr = new char[ 256 * jointnum ];
//			m_pJointInfo    = new int[ JI_MAX * jointnum ];
			m_pJoints       = new RDBPlgJoint[ jointnum ];
			ZeroMemory( m_pJointNameStr, sizeof( char ) * 256 * jointnum );

			int jcnt = 0;
			map<int, CBone*>::iterator itrbone;
			for( itrbone = m_model->m_bonelist.begin(); itrbone != m_model->m_bonelist.end(); itrbone++ ){
				//char* namehead = m_pJointNameStr + 256 * jcnt;
				CBone* curbone = itrbone->second;
				_ASSERT( curbone );
				
				//strcpy( namehead, curbone->m_bonename );

	
				RDBMatrix offsetmat;
				int parentno, parentindex;
				int jointno = curbone->m_boneno;

				/* パーツ番号 */
				m_pJoints[jcnt].SetPartNo( jointno );

				/* ジョイント名変更 */
				//_DPrintf( "JOINT NO:[%d] NAME:[%s]\n", jointno, &m_pJointNameStr[ i * 256 ] );
				// 都合の悪い名前がつけられている可能性があるので、名称変更
				char setname[256];
				ZeroMemory( setname, sizeof( char ) * 256 );
				sprintf_s( setname, 256, "JOINT-%d", jcnt );
				strcpy( m_pJointNameStr + jcnt * 256, setname );
				//sprintf_s( &m_pJointNameStr[ jcnt * 256 ], 256, "JOINT-%d", jcnt );
				m_pJoints[jcnt].SetName( setname );

				/* 親子関係構築 */
				if( curbone->m_parent ){
					parentno = curbone->m_parent->m_boneno;
				}else{
					parentno = -1;
				}
				parentindex = -1;
				if( parentno >= 0 ) {
					map<int, CBone*>::iterator itrb1;
					int j = 0;
					for( itrb1 = m_model->m_bonelist.begin(); itrb1 != m_model->m_bonelist.end(); itrb1++ ){
						CBone* tmpbone1 = itrb1->second;
						if( tmpbone1->m_boneno == parentno ){
							parentindex = j;
							break;
						}
						j++;
					}
				}
				_DPrintf( "jcnt:[%d] parentno:[%d] parentindex:[%d]\n", jcnt, parentno, parentindex );

				//_DPrintf( "joint:[%d] parent:[%d]\n", i, parentindex );
				if( parentindex < 0 ) {
					/* ルートジョイント設定 */
					m_pJointRoot = &m_pJoints[jcnt];
				} else {
					m_pJoints[ parentindex ].AddChild( &m_pJoints[jcnt] );
				}

				/* マトリクス */
				ChaMatrix offmat = curbone->m_xoffsetmat;
				offsetmat._11 = offmat._11;
				offsetmat._12 = offmat._12;
				offsetmat._13 = offmat._13 * -1.0f;
				offsetmat._14 = offmat._14;

				offsetmat._21 = offmat._21;
				offsetmat._22 = offmat._22;
				offsetmat._23 = offmat._23 * -1.0f;
				offsetmat._24 = offmat._24;

				offsetmat._31 = offmat._31 * -1.0f;
				offsetmat._32 = offmat._32 * -1.0f;
				offsetmat._33 = offmat._33;
				offsetmat._34 = offmat._34 * -1.0f;

				offsetmat._41 = offmat._41;
				offsetmat._42 = offmat._42;
				offsetmat._43 = offmat._43 * -1.0f;
				offsetmat._44 = offmat._44;

				m_pJoints[jcnt].SetMatrix( &offsetmat );

				//_DPrintf( " %6.2f %6.2f %6.2f %6.2f\n", offsetmat._11, offsetmat._21, offsetmat._31, offsetmat._41 );
				//_DPrintf( " %6.2f %6.2f %6.2f %6.2f\n", offsetmat._12, offsetmat._22, offsetmat._32, offsetmat._42 );
				//_DPrintf( " %6.2f %6.2f %6.2f %6.2f\n", offsetmat._13, offsetmat._23, offsetmat._33, offsetmat._43 );
				//_DPrintf( " %6.2f %6.2f %6.2f %6.2f\n", offsetmat._14, offsetmat._24, offsetmat._34, offsetmat._44 );

				jcnt++;

			}

			_DPrintf( "!!! jointnum:%d\n", jointnum );
			_ASSERT( jointnum == jcnt );
			m_JointCount = jcnt;

			int cnt2;
			for( cnt2 = 0; cnt2 < m_JointCount; cnt2++ ){
				_DPrintf( "jointname %d;%s\n", cnt2, m_pJointNameStr + 256 * cnt2 );
			}


#else
#endif
		}

	}
	catch( std::bad_alloc ) { // メモリ確保失敗
		throw RDBPlgExceptionNoMemory();
	}

	return true;

}
	
//===================================================================
//
// class RDBPlgInfo
//
//===================================================================

//-------------------------------------------------------------------
//
// コンストラクタ／デストラクタ
//
//-------------------------------------------------------------------

RDBPlgInfo::RDBPlgInfo( CModel* srcmodel ) {
	m_model = srcmodel;
	_ASSERT( m_model );
}

RDBPlgInfo::~RDBPlgInfo() {
	//m_Objs.Release();
	//m_Joints.Release();
}

int RDBPlgInfo::SetModel()
{
	m_Textures.SetModel( m_model );
	m_Objs.SetModel( m_model );
	m_Joints.SetModel( m_model );
	m_Meshes.SetModel( m_model );

	return 0;
}

//-------------------------------------------------------------------
//
// 表示オブジェ数取得
//
//-------------------------------------------------------------------

int RDBPlgInfo::GetDispObjCount() throw( RDBPlgException ) {
	int dispobjnum = m_model->m_object.size();
	if( dispobjnum==0 ) {
		throw RDBPlgExceptionResult();
	}
	return dispobjnum;
}


//-------------------------------------------------------------------
//
// メッシュカウント取得
//
//-------------------------------------------------------------------

int RDBPlgInfo::GetMeshCount() {
	return m_Meshes.GetMeshCount();
}

//-------------------------------------------------------------------
//
// メッシュ取得
//
//-------------------------------------------------------------------

RDBPlgMesh* RDBPlgInfo::GetMesh( int meshno ) {
	return m_Meshes.GetMesh( meshno );
}

//-------------------------------------------------------------------
//
// ジョイント数取得
//
//-------------------------------------------------------------------

int RDBPlgInfo::GetJointCount() throw( RDBPlgException ) {
	return m_Joints.GetJointCount();
}

//-------------------------------------------------------------------
//
// パーツ番号からジョイント番号に変換
//
//-------------------------------------------------------------------

int RDBPlgInfo::GetJointIndexFromPartNo( int partno ) {
	return m_Joints.GetJointIndexFromPartNo( partno );
}

//-------------------------------------------------------------------
//
// ジョイント名取得
//
//-------------------------------------------------------------------

const char* RDBPlgInfo::GetJointName( int jointno ) throw( RDBPlgException ) {
	return m_Joints.GetJointName( jointno );
}

//-------------------------------------------------------------------
//
// ジョイント取得
//
//-------------------------------------------------------------------

RDBPlgJoint* RDBPlgInfo::GetJoint( int jointno ) throw( RDBPlgException ) {
	return m_Joints.GetJoint( jointno );
}

//-------------------------------------------------------------------
//
// ルートジョイント取得
//
//-------------------------------------------------------------------

RDBPlgJoint* RDBPlgInfo::GetJointRoot() throw( RDBPlgException ) {
	return m_Joints.GetJointRoot();
}


//-------------------------------------------------------------------
//
// テクスチャ数取得
//
//-------------------------------------------------------------------

int RDBPlgInfo::GetTextureCount() {
	return m_Textures.GetTextureCount();
}

//-------------------------------------------------------------------
//
// テクスチャ取得
//
//-------------------------------------------------------------------

RDBPlgTexture* RDBPlgInfo::GetTexture( int texno ) {
	return m_Textures.GetTexture( texno );
}

//-------------------------------------------------------------------
//
// テクスチャ検索
//
//-------------------------------------------------------------------

RDBPlgTexture* RDBPlgInfo::SearchTextureName( std::string &name ) {
	return m_Textures.SearchTextureName( name );
}


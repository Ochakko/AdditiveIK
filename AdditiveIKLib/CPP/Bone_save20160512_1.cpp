#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#include <Model.h>
#include <mqomaterial.h>
#include <mqoobject.h>

#define DBGH
#include <dbg.h>

#include <Bone.h>
#include <MQOFace.h>

#include <InfScope.h>
#include <MotionPoint.h>

#include <VecMath.h>

#include <RigidElem.h>
#include <EngName.h>
#include <BoneProp.h>

using namespace std;
using namespace OrgWinGUI;


extern int g_oldaxisflag;
map<CModel*,int> g_bonecntmap;


CBone::CBone( CModel* parmodel ) : m_curmp(), m_axisq()
{
	InitParams();

	m_parmodel = parmodel;

	map<CModel*,int>::iterator itrcnt;
	itrcnt = g_bonecntmap.find( m_parmodel );
	if( itrcnt == g_bonecntmap.end() ){
		g_bonecntmap[ m_parmodel ] = 0;
	}

	int curno = g_bonecntmap[ m_parmodel ]; 
	m_boneno = curno;
	g_bonecntmap[ m_parmodel ] = m_boneno + 1;
}

CBone::~CBone()
{
	DestroyObjs();
}

int CBone::InitParams()
{
	m_upkind = UPVEC_NONE;
	m_motmark.clear();

	m_parmodel = 0;
	m_validflag = 1;
	m_rigidelem.clear();
	m_btobject.clear();
	m_btkinflag = 1;
	m_btforce = 0;

	m_globalpos.SetIdentity();
	ChaMatrixIdentity( &m_invfirstmat );
	ChaMatrixIdentity( &m_firstmat );
	ChaMatrixIdentity( &m_startmat2 );
	ChaMatrixIdentity( &m_axismat_par );
	ChaMatrixIdentity(&m_initmat);
	ChaMatrixIdentity(&m_invinitmat);
	ChaMatrixIdentity(&m_tmpmat);
	ChaMatrixIdentity(&m_firstaxismatX);

	m_boneno = 0;
	m_topboneflag = 0;
	ZeroMemory( m_bonename, sizeof( char ) * 256 );
	ZeroMemory( m_wbonename, sizeof( WCHAR ) * 256 );
	ZeroMemory( m_engbonename, sizeof( char ) * 256 );

	m_childworld = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_childscreen = ChaVector3( 0.0f, 0.0f, 0.0f );

	m_parent = 0;
	m_child = 0;
	m_brother = 0;

	m_selectflag = 0;

	m_getanimflag = 0;

	m_type = FBXBONE_NONE;

	ChaMatrixIdentity( &m_nodemat );

	m_jointwpos = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_jointfpos = ChaVector3( 0.0f, 0.0f, 0.0f );
	m_oldjointfpos = ChaVector3(0.0f, 0.0f, 0.0f);

	ChaMatrixIdentity( &m_laxismat );
	ChaMatrixIdentity( &m_gaxismatXpar );

	m_remap.clear();
	m_impmap.clear();

	m_tmpq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);

	m_firstframebonepos = ChaVector3(0.0f, 0.0f, 0.0f);

	m_posefoundflag = false;

	return 0;
}
int CBone::DestroyObjs()
{
	m_motmark.clear();

	map<int, CMotionPoint*>::iterator itrmp;
	for( itrmp = m_motionkey.begin(); itrmp != m_motionkey.end(); itrmp++ ){
		CMotionPoint* topkey = itrmp->second;
		if( topkey ){
			CMotionPoint* curmp = topkey;
			CMotionPoint* nextmp = 0;
			while( curmp ){
				nextmp = curmp->GetNext();

				delete curmp;

				curmp = nextmp;
			}
		}
	}

	m_motionkey.clear();

	
	map<string, std::map<CBone*, CRigidElem*>>::iterator itrmap;
	for( itrmap = m_remap.begin(); itrmap != m_remap.end(); itrmap++ ){
		map<CBone*, CRigidElem*> &curmap = itrmap->second;
		map<CBone*, CRigidElem*>::iterator itrre;
		for( itrre = curmap.begin(); itrre != curmap.end(); itrre++ ){
			CRigidElem* curre = itrre->second;
			delete curre;
		}
		curmap.clear();
	}
	m_remap.clear();
	m_impmap.clear();

	m_rigidelem.clear();

	return 0;
}


int CBone::AddChild( CBone* childptr )
{
	childptr->m_parent = this;
	if( !m_child ){
		m_child = childptr;
	}else{
		CBone* broptr = m_child;
		while( broptr->m_brother ){
			broptr = broptr->m_brother;
		}
		broptr->m_brother = childptr;
		broptr->m_brother->m_parent = this;//!!!!!!!!
	}

	return 0;
}


int CBone::UpdateMatrix( int srcmotid, double srcframe, ChaMatrix* wmat, ChaMatrix* vpmat )
{
	int existflag = 0;
	if( srcframe >= 0.0 ){
		CallF( CalcFBXMotion( srcmotid, srcframe, &m_curmp, &existflag ), return 1 );
		ChaMatrix tmpmat = m_curmp.GetWorldMat() * *wmat;
		m_curmp.SetWorldMat( tmpmat ); 

		ChaVector3 jpos = GetJointFPos();
		ChaVector3TransformCoord( &m_childworld, &jpos, &m_curmp.GetWorldMat() );
		ChaVector3TransformCoord( &m_childscreen, &m_childworld, vpmat );
	}else{
		m_curmp.InitParams();
		m_curmp.SetWorldMat( *wmat );
	}

	return 0;
}


CMotionPoint* CBone::AddMotionPoint(int srcmotid, double srcframe, int* existptr)
{
	CMotionPoint* newmp = 0;
	CMotionPoint* pbef = 0;
	CMotionPoint* pnext = 0;
	CallF(GetBefNextMP(srcmotid, srcframe, &pbef, &pnext, existptr), return 0);

	if (*existptr){
		pbef->SetFrame(srcframe);
		newmp = pbef;
	}
	else{
		newmp = new CMotionPoint();
		if (!newmp){
			_ASSERT(0);
			return 0;
		}
		newmp->SetFrame(srcframe);

		if (pbef){
			CallF(pbef->AddToNext(newmp), return 0);
		}
		else{
			m_motionkey[srcmotid] = newmp;
			if (pnext){
				newmp->SetNext(pnext);
			}
		}
	}

	return newmp;
}


int CBone::CalcFBXMotion( int srcmotid, double srcframe, CMotionPoint* dstmpptr, int* existptr )
{
	CMotionPoint* befptr = 0;
	CMotionPoint* nextptr = 0;
	CallF( GetBefNextMP( srcmotid, srcframe, &befptr, &nextptr, existptr ), return 1 );
	CallF( CalcFBXFrame( srcframe, befptr, nextptr, *existptr, dstmpptr ), return 1 );

	return 0;
}


int CBone::GetBefNextMP( int srcmotid, double srcframe, CMotionPoint** ppbef, CMotionPoint** ppnext, int* existptr )
{
	CMotionPoint* pbef = 0;
	CMotionPoint* pcur = m_motionkey[srcmotid];

	*existptr = 0;

	while( pcur ){

		if( (pcur->GetFrame() >= srcframe - 0.0001) && (pcur->GetFrame() <= srcframe + 0.0001) ){
			*existptr = 1;
			pbef = pcur;
			break;
		}else if( pcur->GetFrame() > srcframe ){
			*existptr = 0;
			break;
		}else{
			pbef = pcur;
			pcur = pcur->GetNext();
		}
	}
	*ppbef = pbef;

	if( *existptr ){
		*ppnext = pbef->GetNext();
	}else{
		*ppnext = pcur;
	}

	return 0;
}

int CBone::CalcFBXFrame( double srcframe, CMotionPoint* befptr, CMotionPoint* nextptr, int existflag, CMotionPoint* dstmpptr )
{

	if( existflag == 1 ){
		*dstmpptr = *befptr;
		return 0;
	}else if( !befptr ){
		dstmpptr->InitParams();
		dstmpptr->SetFrame( srcframe );
		return 0;
	}else if( !nextptr ){
		*dstmpptr = *befptr;
		dstmpptr->SetFrame( srcframe );
		return 0;
	}else{
		double diffframe = nextptr->GetFrame() - befptr->GetFrame();
		_ASSERT( diffframe != 0.0 );
		double t = ( srcframe - befptr->GetFrame() ) / diffframe;

		ChaMatrix tmpmat = befptr->GetWorldMat() + (float)t * ( nextptr->GetWorldMat() - befptr->GetWorldMat() );
		dstmpptr->SetWorldMat( tmpmat );
		dstmpptr->SetFrame( srcframe );

		dstmpptr->SetPrev( befptr );
		dstmpptr->SetNext( nextptr );

		return 0;
	}
}

int CBone::DeleteMotion( int srcmotid )
{
	map<int, CMotionPoint*>::iterator itrmp;
	itrmp = m_motionkey.find( srcmotid );
	if( itrmp != m_motionkey.end() ){
		CMotionPoint* topkey = itrmp->second;
		if( topkey ){
			CMotionPoint* curmp = topkey;
			CMotionPoint* nextmp = 0;
			while( curmp ){
				nextmp = curmp->GetNext();

				delete curmp;

				curmp = nextmp;
			}
		}
	}

	m_motionkey.erase( itrmp );

	return 0;
}



int CBone::DeleteMPOutOfRange( int motid, double srcleng )
{
	CMotionPoint* curmp = m_motionkey[ motid ];

	while( curmp ){
		CMotionPoint* nextmp = curmp->GetNext();

		if( curmp->GetFrame() > srcleng ){
			curmp->LeaveFromChain( motid, this );
			delete curmp;
		}
		curmp = nextmp;
	}

	return 0;
}


int CBone::SetName( char* srcname )
{
	strcpy_s( m_bonename, 256, srcname );
	TermJointRepeats(m_bonename);

	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_bonename, 256, m_wbonename, 256 );

	return 0;
}

int CBone::CalcAxisMatZ( ChaVector3* curpos, ChaVector3* chilpos )
{
	if( *curpos == *chilpos ){
		ChaMatrixIdentity( &m_laxismat );
		m_axisq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
		return 0;
	}

	ChaVector3 startpos, endpos, upvec;

	ChaVector3 vecx0, vecy0, vecz0;
	ChaVector3 vecx1, vecy1, vecz1;

	startpos = *curpos;
	endpos = *chilpos;

	vecx0.x = 1.0;
	vecx0.y = 0.0;
	vecx0.z = 0.0;

	vecy0.x = 0.0;
	vecy0.y = 1.0;
	vecy0.z = 0.0;

	vecz0.x = 0.0;
	vecz0.y = 0.0;
	vecz0.z = 1.0;

	ChaVector3 bonevec;
	bonevec = endpos - startpos;
	ChaVector3Normalize( &bonevec, &bonevec );

	if( (bonevec.x != 0.0f) || (bonevec.y != 0.0f) ){
		upvec.x = 0.0f;
		upvec.y = 0.0f;
		upvec.z = 1.0f;
	}else{
		upvec.x = 1.0f;
		upvec.y = 0.0f;
		upvec.z = 0.0f;
	}

	vecz1 = bonevec;
		
	ChaVector3Cross( &vecx1, &upvec, &vecz1 );
	ChaVector3Normalize( &vecx1, &vecx1 );

	ChaVector3Cross( &vecy1, &vecz1, &vecx1 );
	ChaVector3Normalize( &vecy1, &vecy1 );

	D3DXQUATERNION tmpxq;

	ChaMatrixIdentity( &m_laxismat );
	m_laxismat._11 = vecx1.x;
	m_laxismat._12 = vecx1.y;
	m_laxismat._13 = vecx1.z;

	m_laxismat._21 = vecy1.x;
	m_laxismat._22 = vecy1.y;
	m_laxismat._23 = vecy1.z;

	m_laxismat._31 = vecz1.x;
	m_laxismat._32 = vecz1.y;
	m_laxismat._33 = vecz1.z;


	D3DXQuaternionRotationMatrix( &tmpxq, &m_laxismat );

	m_axisq.x = tmpxq.x;
	m_axisq.y = tmpxq.y;
	m_axisq.z = tmpxq.z;
	m_axisq.w = tmpxq.w;

	return 0;
}

int CBone::CalcAxisMatX()
{

	ChaVector3 curpos;
	ChaVector3 chilpos;

	if( m_child ){
		ChaVector3 jpos = GetJointFPos();
		ChaVector3 chiljpos = m_child->GetJointFPos();
		ChaVector3TransformCoord( &curpos, &jpos, &m_startmat2 );
		ChaVector3TransformCoord( &chilpos, &chiljpos, &(m_child->m_startmat2) );

		CalcAxisMatX_aft( curpos, chilpos, &m_gaxismatXpar );
	}


	return 0;
}

int CBone::CalcAxisMatX_aft( ChaVector3 curpos, ChaVector3 chilpos, ChaMatrix* dstmat )
{
	if( curpos == chilpos ){
		ChaMatrixIdentity( dstmat );
		return 0;
	}

	ChaVector3 startpos, endpos, upvec;

	ChaVector3 vecx0, vecy0, vecz0;
	ChaVector3 vecx1, vecy1, vecz1;

	startpos = curpos;
	endpos = chilpos;

	vecx0.x = 1.0;
	vecx0.y = 0.0;
	vecx0.z = 0.0;

	vecy0.x = 0.0;
	vecy0.y = 1.0;
	vecy0.z = 0.0;

	vecz0.x = 0.0;
	vecz0.y = 0.0;
	vecz0.z = 1.0;

	ChaVector3 bonevec;
	bonevec = endpos - startpos;
	ChaVector3Normalize( &bonevec, &bonevec );

	if( (bonevec.x != 0.0f) || (bonevec.y != 0.0f) ){
		upvec.x = 0.0f;
		upvec.y = 0.0f;
		upvec.z = 1.0f;
		m_upkind = UPVEC_Z;//vecy1-->Y, vecz1-->Z
	}else{
		upvec.x = 0.0f;
		upvec.y = 1.0f;
		upvec.z = 0.0f;
		m_upkind = UPVEC_Y;//vecy1-->Z, vecz1-->Y
	}

	vecx1 = bonevec;
		
	ChaVector3Cross( &vecy1, &upvec, &vecx1 );
	ChaVector3Normalize( &vecy1, &vecy1 );

	ChaVector3Cross( &vecz1, &vecx1, &vecy1 );
	ChaVector3Normalize( &vecy1, &vecy1 );

	D3DXQUATERNION tmpxq;

	ChaMatrixIdentity( dstmat );
	dstmat->_11 = vecx1.x;
	dstmat->_12 = vecx1.y;
	dstmat->_13 = vecx1.z;

	dstmat->_21 = vecy1.x;
	dstmat->_22 = vecy1.y;
	dstmat->_23 = vecy1.z;

	dstmat->_31 = vecz1.x;
	dstmat->_32 = vecz1.y;
	dstmat->_33 = vecz1.z;


	return 0;
}

int CBone::CalcAxisMatY( CBone* chilbone, ChaMatrix* dstmat )
{

	ChaVector3 curpos;
	ChaVector3 chilpos;

	ChaVector3 jpos = GetJointFPos();
	ChaVector3 chiljpos = chilbone->GetJointFPos();
	ChaVector3TransformCoord( &curpos, &jpos, &(m_curmp.GetWorldMat()) );
	ChaVector3TransformCoord( &chilpos, &chiljpos, &(chilbone->m_curmp.GetWorldMat()) );

	ChaVector3 diff = curpos - chilpos;
	float leng;
	leng = ChaVector3Length( &diff );

	if( leng <= 0.00001f ){
		ChaMatrixIdentity( dstmat );
		return 0;
	}

	ChaVector3 startpos, endpos, upvec;

	ChaVector3 vecx0, vecy0, vecz0;
	ChaVector3 vecx1, vecy1, vecz1;

	startpos = curpos;
	endpos = chilpos;

	vecx0.x = 1.0;
	vecx0.y = 0.0;
	vecx0.z = 0.0;

	vecy0.x = 0.0;
	vecy0.y = 1.0;//!!!!!!!!!!!!!!!!!!
	vecy0.z = 0.0;

	vecz0.x = 0.0;
	vecz0.y = 0.0;
	vecz0.z = 1.0;

	ChaVector3 bonevec;
	bonevec = endpos - startpos;
	ChaVector3Normalize( &bonevec, &bonevec );

	if( (bonevec.x != 0.0f) || (bonevec.y != 0.0f) ){
		upvec.x = 0.0f;
		upvec.y = 0.0f;
		upvec.z = 1.0f;
	}else{
		upvec.x = 1.0f;
		upvec.y = 0.0f;
		upvec.z = 0.0f;
	}

	vecy1 = bonevec;
		
	ChaVector3Cross( &vecx1, &vecy1, &upvec );

	int illeagalflag = 0;
	float crleng = ChaVector3Length( &vecx1 );
	if( crleng < 0.000001f ){
		illeagalflag = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	ChaVector3Normalize( &vecx1, &vecx1 );

	ChaVector3Cross( &vecz1, &vecx1, &vecy1 );
	ChaVector3Normalize( &vecy1, &vecy1 );

	D3DXQUATERNION tmpxq;

	ChaMatrixIdentity( dstmat );
	if( illeagalflag == 0 ){
		dstmat->_11 = vecx1.x;
		dstmat->_12 = vecx1.y;
		dstmat->_13 = vecx1.z;

		dstmat->_21 = vecy1.x;
		dstmat->_22 = vecy1.y;
		dstmat->_23 = vecy1.z;

		dstmat->_31 = vecz1.x;
		dstmat->_32 = vecz1.y;
		dstmat->_33 = vecz1.z;
	}

	return 0;
}

int CBone::CalcRigidElemParams( CModel* colptr[COL_MAX], CBone* chilbone, int setstartflag )
{

	CRigidElem* curre = m_rigidelem[ chilbone ];
	if( !curre ){
		_ASSERT( 0 );
		return 0;
	}

	_ASSERT( colptr );
	_ASSERT( chilbone );

	CModel* curcoldisp = colptr[curre->GetColtype()];
	_ASSERT( curcoldisp );


	ChaMatrix bmmat;
	ChaMatrixIdentity( &bmmat );

	ChaVector3 aftbonepos;
	ChaVector3 jpos = GetJointFPos();
	ChaVector3TransformCoord( &aftbonepos, &jpos, &(m_curmp.GetWorldMat()) );

	ChaVector3 aftchilpos;
	ChaVector3 chiljpos = chilbone->GetJointFPos();
	ChaVector3TransformCoord( &aftchilpos, &chiljpos, &(chilbone->m_curmp.GetWorldMat()) );

	CalcAxisMatZ( &aftbonepos, &aftchilpos );
	CalcAxisMatY( chilbone, &bmmat );			
	ChaVector3 diffvec = aftchilpos - aftbonepos;
	float diffleng = ChaVector3Length( &diffvec );

	float cylileng = curre->GetCylileng();
	float sphr = curre->GetSphr();
	float boxz = curre->GetBoxz();

	if( curre->GetColtype() == COL_CAPSULE_INDEX ){
		map<int,CMQOObject*>::iterator itrobj;
		for( itrobj = curcoldisp->GetMqoObjectBegin(); itrobj != curcoldisp->GetMqoObjectEnd(); itrobj++ ){
			CMQOObject* curobj = itrobj->second;
			_ASSERT( curobj );
			if( strcmp( curobj->GetName(), "cylinder" ) == 0 ){
				CallF( curobj->ScaleBtCapsule( curre, diffleng, 0, &cylileng ), return 1 );
			}else if( strcmp( curobj->GetName(), "sph_ue" ) == 0 ){
				CallF( curobj->ScaleBtCapsule( curre, diffleng, 1, &sphr ), return 1 );
			}else{
				CallF( curobj->ScaleBtCapsule( curre, diffleng, 2, 0 ), return 1 );
			}
		}
	}else if( curre->GetColtype() == COL_CONE_INDEX ){
		map<int,CMQOObject*>::iterator itrobj;
		for( itrobj = curcoldisp->GetMqoObjectBegin(); itrobj != curcoldisp->GetMqoObjectEnd(); itrobj++ ){
			CMQOObject* curobj = itrobj->second;
			_ASSERT( curobj );
			CallF( curobj->ScaleBtCone( curre, diffleng, &cylileng, &sphr ), return 1 );
		}
	}else if( curre->GetColtype() == COL_SPHERE_INDEX ){
		map<int,CMQOObject*>::iterator itrobj;
		for( itrobj = curcoldisp->GetMqoObjectBegin(); itrobj != curcoldisp->GetMqoObjectEnd(); itrobj++ ){
			CMQOObject* curobj = itrobj->second;
			_ASSERT( curobj );
			CallF( curobj->ScaleBtSphere( curre, diffleng, &cylileng, &sphr ), return 1 );
		}
	}else if( curre->GetColtype() == COL_BOX_INDEX ){
		map<int,CMQOObject*>::iterator itrobj;
		for( itrobj = curcoldisp->GetMqoObjectBegin(); itrobj != curcoldisp->GetMqoObjectEnd(); itrobj++ ){
			CMQOObject* curobj = itrobj->second;
			_ASSERT( curobj );
			CallF( curobj->ScaleBtBox( curre, diffleng, &cylileng, &sphr, &boxz ), return 1 );

DbgOut( L"bonecpp : calcrigidelemparams : BOX : cylileng %f, sphr %f, boxz %f\r\n", cylileng, sphr, boxz );

		}
	}else{
		_ASSERT( 0 );
		return 1;
	}
			
	bmmat._41 = ( aftbonepos.x + aftchilpos.x ) * 0.5f;
	bmmat._42 = ( aftbonepos.y + aftchilpos.y ) * 0.5f;
	bmmat._43 = ( aftbonepos.z + aftchilpos.z ) * 0.5f;

	curre->SetCapsulemat( bmmat );
	curre->SetCylileng( cylileng );
	curre->SetSphr( sphr );
	curre->SetBoxz( boxz );

	if( setstartflag == 1 ){
		curre->SetFirstcapsulemat( curre->GetCapsulemat() );
	}

	return 0;
}

void CBone::SetStartMat2Req()
{
	SetStartMat2(m_curmp.GetWorldMat());

	if (m_child){
		m_child->SetStartMat2Req();
	}
	if (m_brother){
		m_brother->SetStartMat2Req();
	}
}

int CBone::CalcAxisMat( int firstflag, float delta )
{
	if( firstflag == 1 ){
		SetStartMat2Req();
		
		CalcAxisMatX();
	}

	if( delta != 0.0f ){
		CQuaternion multq;
		ChaVector3 gparpos, gchilpos, gbonevec;
		if( m_child ){
			ChaVector3 jpos = GetJointFPos();
			ChaVector3 chiljpos = m_child->GetJointFPos();
			ChaVector3TransformCoord( &gparpos, &jpos, &m_startmat2 );
			ChaVector3TransformCoord( &gchilpos, &chiljpos, &(m_child->m_startmat2) );
			gbonevec = gchilpos - gparpos;
			ChaVector3Normalize( &gbonevec, &gbonevec );
		}else{
			_ASSERT( 0 );
			return 0;
		}

		multq.SetAxisAndRot( gbonevec, delta );
		ChaMatrix multmat = multq.MakeRotMatX();
		m_gaxismatXpar = m_gaxismatXpar * multmat;
	}

	ChaMatrix invpar;
	ChaMatrixInverse( &invpar, NULL, &m_gaxismatXpar );

	CalcLocalAxisMat( m_startmat2, m_gaxismatXpar, m_gaxismatYpar );

	return 0;
}

int CBone::CalcLocalAxisMat( ChaMatrix motmat, ChaMatrix axismatpar, ChaMatrix gaxisy )
{
	ChaMatrix startpar0 = axismatpar;
	startpar0._41 = 0.0f;
	startpar0._42 = 0.0f;
	startpar0._43 = 0.0f;

	ChaMatrix starty = gaxisy;
	starty._41 = 0.0f;
	starty._42 = 0.0f;
	starty._43 = 0.0f;

	ChaMatrix motmat0 = motmat;
	motmat0._41 = 0.0f;
	motmat0._42 = 0.0f;
	motmat0._43 = 0.0f;

	ChaMatrix invmotmat;
	ChaMatrixInverse( &invmotmat, NULL, &motmat0 );

	m_axismat_par = startpar0 * invmotmat;

	return 0;
}

int CBone::CreateRigidElem( CBone* chil, int reflag, std::string rename, int impflag, std::string impname )
{
	_ASSERT( chil );

	if( reflag ){
		map<string, map<CBone*, CRigidElem*>>::iterator findremap;
		findremap = m_remap.find( rename );
		if( findremap != m_remap.end() ){
			map<CBone*, CRigidElem*>& curmap = findremap->second;
	
	DbgOut( L"CreateRigidElem : map exist : curbone %s, chilbone %s\r\n", m_wbonename, chil->m_wbonename );

			CRigidElem* chkre = curmap[ chil ];
			if( chkre ){
	DbgOut( L"CreateRigidElem : chkre return !!!\r\n" );
				return 0;
			}

			curmap[ chil ] = new CRigidElem();
			if( !curmap[ chil ] ){
				_ASSERT( 0 );
				return 1;
			}

			curmap[ chil ]->SetBone( this );
			curmap[ chil ]->SetEndbone( chil );

			SetGroupNoByName( curmap[ chil ], chil );

		}else{
			map<CBone*, CRigidElem*> curmap;

	DbgOut( L"CreateRigidElem : map [not] exist : curbone %s, chilbone %s\r\n", m_wbonename, chil->m_wbonename );

			curmap[ chil ] = new CRigidElem();
			if( !curmap[ chil ] ){
				_ASSERT( 0 );
				return 1;
			}

			curmap[ chil ]->SetBone( this );
			curmap[ chil ]->SetEndbone( chil );

			m_remap[ rename ] = curmap;

			SetGroupNoByName( curmap[ chil ], chil );

			//_ASSERT( 0 );
		}
	}

//////////////
	if( impflag ){
		map<string, map<CBone*, ChaVector3>>::iterator findimpmap;
		findimpmap = m_impmap.find( impname );
		if( findimpmap != m_impmap.end() ){
			map<CBone*,ChaVector3>::iterator itrimp;
			itrimp = findimpmap->second.find( chil );
			if( itrimp != findimpmap->second.end() ){
				return 0;
			}

			findimpmap->second[chil] = ChaVector3(0.0f, 0.0f, 0.0f);

		}else{
			map<CBone*, ChaVector3> curmap;

			curmap[ chil ] = ChaVector3( 0.0f, 0.0f, 0.0f );
			m_impmap[ impname ] = curmap;
		}
	}
	return 0;
}

int CBone::SetGroupNoByName( CRigidElem* curre, CBone* chilbone )
{
	char* groupmark = strstr( chilbone->m_bonename, "_G_" );
	if( groupmark ){
		char* numstart = groupmark + 3;
		char* numend = strstr( numstart, "_" );
		if( numend ){
			int numleng = numend - numstart;
			if( (numleng > 0) && (numleng <= 2) ){
				char strnum[5];
				ZeroMemory( strnum, sizeof( char ) * 5 );
				strncpy_s( strnum, 5, numstart, numleng );
				int gno = (int)atoi( strnum );
				if( (gno >= 1) && (gno <= COLIGROUPNUM) ){
					curre->SetGroupid( gno ); 
				}
			}
		}
	}
	int cmpbt = strncmp( chilbone->m_bonename, "BT_", 3 );
	if( cmpbt == 0 ){
		m_btforce = 1;
	}

	return 0;
}

int CBone::SetCurrentRigidElem( std::string curname )
{
	m_rigidelem.clear();

	if( !m_child ){
		return 0;
	}

	map<string, map<CBone*, CRigidElem*>>::iterator itrmap;
	itrmap = m_remap.find( curname );
	if( itrmap == m_remap.end() ){
		_ASSERT( 0 );
		return 1;
	}

	m_rigidelem = itrmap->second;

	return 0;
}


CMotionPoint* CBone::AddBoneTraReq( CMotionPoint* parmp, int srcmotid, double srcframe, ChaVector3 srctra )
{
	int existflag = 0;
	CMotionPoint* curmp = AddMotionPoint( srcmotid, srcframe, &existflag );
	if( !curmp || !existflag ){
		_ASSERT( 0 );
		return 0;
	}

	curmp->SetBefWorldMat( curmp->GetWorldMat() );
	if( parmp ){
		ChaMatrix invbefpar;
		ChaMatrixInverse( &invbefpar, NULL, &parmp->GetBefWorldMat() );
		ChaMatrix tmpmat = curmp->GetWorldMat() * invbefpar * parmp->GetWorldMat();
		curmp->SetWorldMat( tmpmat );
	}else{
		ChaMatrix tramat;
		ChaMatrixTranslation( &tramat, srctra.x, srctra.y, srctra.z );
		ChaMatrix tmpmat = curmp->GetWorldMat() * tramat;
		curmp->SetWorldMat( tmpmat );
	}

	curmp->SetAbsMat( curmp->GetWorldMat() );

	if( m_child ){
		m_child->AddBoneTraReq( curmp, srcmotid, srcframe, srctra );
	}
	if( m_brother && parmp ){
		m_brother->AddBoneTraReq( parmp, srcmotid, srcframe, srctra );
	}

	return curmp;
}


CMotionPoint* CBone::PasteRotReq( int srcmotid, double srcframe, double dstframe )
{
	//src : srcmp srcparmp
	//dst : curmp parmp

	int existflag0 = 0;
	CMotionPoint* srcmp = AddMotionPoint( srcmotid, srcframe, &existflag0 );
	if( !existflag0 || !srcmp ){
		_ASSERT( 0 );
		return 0;
	}

	int existflag = 0;
	CMotionPoint* curmp = AddMotionPoint( srcmotid, dstframe, &existflag );
	if( !existflag || !curmp ){
		_ASSERT( 0 );
		return 0;
	}
	
	curmp->SetBefWorldMat( curmp->GetWorldMat() );
	curmp->SetWorldMat( srcmp->GetWorldMat() );
	curmp->SetAbsMat( srcmp->GetAbsMat() );

	if( m_child ){
		m_child->PasteRotReq( srcmotid, srcframe, dstframe );
	}
	if( m_brother ){
		m_brother->PasteRotReq( srcmotid, srcframe, dstframe );
	}
	return curmp;

}


CMotionPoint* CBone::RotBoneQReq(CMotionPoint* parmp, int srcmotid, double srcframe, CQuaternion rotq, CBone* bvhbone, ChaVector3 traanim, int setmatflag, ChaMatrix* psetmat)
{
	int existflag = 0;
	CMotionPoint* curmp = AddMotionPoint( srcmotid, srcframe, &existflag );
	if( !existflag || !curmp ){
		_ASSERT( 0 );
		return 0;
	}
	
	curmp->SetBefWorldMat( curmp->GetWorldMat() );
	if( parmp ){
		ChaMatrix invbefpar;
		ChaMatrixInverse( &invbefpar, NULL, &parmp->GetBefWorldMat() );
		ChaMatrix tmpmat = curmp->GetWorldMat() * invbefpar * parmp->GetWorldMat(); 
		curmp->SetWorldMat( tmpmat );
	}else{
		//初回呼び出し
		ChaMatrix tramat;
		ChaMatrixIdentity(&tramat);
		ChaMatrixTranslation(&tramat, traanim.x, traanim.y, traanim.z);

		if (setmatflag == 0){
			ChaVector3 rotcenter;// = m_childworld;
			ChaVector3 jpos = GetJointFPos();
			ChaVector3TransformCoord(&rotcenter, &jpos, &(curmp->GetWorldMat()));

			ChaMatrix befrot, aftrot;
			ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
			ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
			ChaMatrix rotmat = befrot * rotq.MakeRotMatX() * aftrot;
			ChaMatrix tmpmat = curmp->GetWorldMat() * rotmat * tramat;
			curmp->SetWorldMat(tmpmat);
			if (bvhbone){
				bvhbone->SetTmpMat(tmpmat);
			}
		}
		else{
			ChaMatrix tmpmat = *psetmat;
			curmp->SetWorldMat(tmpmat);
			if (bvhbone){
				bvhbone->SetTmpMat(tmpmat);
			}
		}
	}

	curmp->SetAbsMat( curmp->GetWorldMat() );

	if( m_child ){
		m_child->RotBoneQReq( curmp, srcmotid, srcframe, rotq );
	}
	if( m_brother && parmp ){
		m_brother->RotBoneQReq( parmp, srcmotid, srcframe, rotq );
	}
	return curmp;
}

CMotionPoint* CBone::RotBoneQOne(CMotionPoint* parmp, int srcmotid, double srcframe, ChaMatrix srcmat)
{
	int existflag = 0;
	CMotionPoint* curmp = AddMotionPoint(srcmotid, srcframe, &existflag);
	if (!existflag || !curmp){
		_ASSERT(0);
		return 0;
	}

	curmp->SetBefWorldMat(curmp->GetWorldMat());
	if (parmp){
		//parentの行列をセット !!!!!!!!!
		curmp->SetWorldMat(parmp->GetWorldMat());
	} else{
		curmp->SetWorldMat(srcmat);
	}

	curmp->SetAbsMat(curmp->GetWorldMat());

	return curmp;
}

/*
CMotionPoint* CBone::RotBoneQOne(CMotionPoint* parmp, int srcmotid, double srcframe, CQuaternion rotq)
{

	//一番親から１つずつ呼び出すように使用する。

	int existflag = 0;
	CMotionPoint* curmp = AddMotionPoint(srcmotid, srcframe, &existflag);
	if (!existflag || !curmp){
		_ASSERT(0);
		return 0;
	}

	curmp->SetBefWorldMat(curmp->GetWorldMat());

	//ChaMatrix prerotmat = curmp->GetWorldMat();
	ChaMatrix prerotmat = GetInitMat();

	ChaVector3 rotcenter;// = m_childworld;
	ChaVector3TransformCoord(&rotcenter, &m_jointfpos, &prerotmat);
	//ChaVector3 rotcenter = m_jointfpos;

	ChaMatrix befrot, aftrot;
	ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
	ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
	ChaMatrix rotmat = befrot * rotq.MakeRotMatX() * aftrot;


	//ChaMatrix tmpmat = curmp->GetWorldMat() * rotmat;
	ChaMatrix tmpmat = GetInitMat() * rotmat;
	//if (parmp){
	//	tmpmat = tmpmat * parmp->GetWorldMat();
	//}
	curmp->SetWorldMat(tmpmat);

	curmp->SetAbsMat(curmp->GetWorldMat());

	return curmp;
}
*/

CMotionPoint* CBone::SetAbsMatReq( int broflag, int srcmotid, double srcframe, double firstframe )
{
	int existflag = 0;
	CMotionPoint* curmp = AddMotionPoint( srcmotid, srcframe, &existflag );
	if( !existflag || !curmp ){
		_ASSERT( 0 );
		return 0;
	}

	int existflag2 = 0;
	CMotionPoint* firstmp = AddMotionPoint( srcmotid, firstframe, &existflag2 );
	if( !existflag2 || !firstmp ){
		_ASSERT( 0 );
		return 0;
	}

	curmp->SetWorldMat( firstmp->GetAbsMat() );

	if( m_child ){
		m_child->SetAbsMatReq( 1, srcmotid, srcframe, firstframe );
	}
	if( m_brother && broflag ){
		m_brother->SetAbsMatReq( 1, srcmotid, srcframe, firstframe );
	}
	return curmp;
}

int CBone::DestroyMotionKey( int srcmotid )
{
	CMotionPoint* curmp = m_motionkey[ srcmotid ];
	while( curmp ){
		CMotionPoint* nextmp = curmp->GetNext();
		delete curmp;
		curmp = nextmp;
	}

	m_motionkey[ srcmotid ] = NULL;

	return 0;
}


int CBone::AddBoneMarkIfNot( int motid, OrgWinGUI::OWP_Timeline* owpTimeline, int curlineno, double curframe, int flag )
{
	map<double, int> curmark;
	map<int, map<double, int>>::iterator itrcur;
	itrcur = m_motmark.find( motid );
	if( itrcur == m_motmark.end() ){
		curmark.clear();
	}else{
		curmark = itrcur->second;
	}
	map<double, int>::iterator itrmark;
	itrmark = curmark.find( curframe );
	if( itrmark == curmark.end() ){
		curmark[ curframe ] = flag;
		m_motmark[ motid ] = curmark;
	}

	return 0;
}

int CBone::DelBoneMarkRange( int motid, OrgWinGUI::OWP_Timeline* owpTimeline, int curlineno, double startframe, double endframe )
{
	map<int, map<double, int>>::iterator itrcur;
	itrcur = m_motmark.find( motid );
	if( itrcur == m_motmark.end() ){
		return 0;
	}

	map<double, int> curmark;
	curmark = itrcur->second;

	double frame;
	for( frame = (startframe + 1.0); frame <= (endframe - 1.0); frame += 1.0 ){
		map<double, int>::iterator itrfind;
		itrfind = curmark.find( frame );
		if( itrfind != curmark.end() ){
			curmark.erase( itrfind );
		}
	}

	m_motmark[ motid ] = curmark;

	return 0;
}

int CBone::AddBoneMotMark( int motid, OWP_Timeline* owpTimeline, int curlineno, double startframe, double endframe, int flag )
{
	if( startframe != endframe ){
		AddBoneMarkIfNot( motid, owpTimeline, curlineno, startframe, flag );
		AddBoneMarkIfNot( motid, owpTimeline, curlineno, endframe, flag );
		DelBoneMarkRange( motid, owpTimeline, curlineno, startframe, endframe );
	}else{
		AddBoneMarkIfNot( motid, owpTimeline, curlineno, startframe, flag );
	}

	return 0;
}

int CBone::CalcLocalInfo( int motid, double frameno, CMotionPoint* pdstmp )
{

	CMotionPoint* pcurmp = 0;
	CMotionPoint* pparmp = 0;
	pcurmp = GetMotionPoint(motid, frameno);
	if( m_parent ){
		if( pcurmp ){
			pparmp = m_parent->GetMotionPoint(motid, frameno);
			if( pparmp ){
				ChaMatrix invpar = pparmp->GetInvWorldMat();
				ChaMatrix localmat = pcurmp->GetWorldMat() * invpar;
				pcurmp->CalcQandTra(localmat, this);

				int inirotcur, inirotpar;
				inirotcur = IsInitRot(pcurmp->GetWorldMat());
				inirotpar = IsInitRot(pparmp->GetWorldMat());
				if (inirotcur && inirotpar){
					CQuaternion iniq;
					iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
					pcurmp->SetQ(iniq);
				}

				*pdstmp = *pcurmp;
			}else{
				CMotionPoint inimp;
				*pdstmp = inimp;
				_ASSERT( 0 );
				return 0;
			}
		}else{
			CMotionPoint inimp;
			*pdstmp = inimp;

			_ASSERT( 0 );
			return 0;
		}
	}else{
		if( pcurmp ){
			ChaMatrix localmat = pcurmp->GetWorldMat();
			pcurmp->CalcQandTra( localmat, this );

			int inirotcur;
			inirotcur = IsInitRot(pcurmp->GetWorldMat());
			if (inirotcur ){
				CQuaternion iniq;
				iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
				pcurmp->SetQ(iniq);
			}

			*pdstmp = *pcurmp;

		}else{
			CMotionPoint inimp;
			*pdstmp = inimp;

			_ASSERT( 0 );
			return 0;
		}
	}

	return 0;
}

int CBone::CalcInitLocalInfo(int motid, double frameno, CMotionPoint* pdstmp)
{

	CMotionPoint* pcurmp = 0;
	CMotionPoint* pparmp = 0;
	pcurmp = GetMotionPoint(motid, frameno);
	if (m_parent){
		if (pcurmp){
			pparmp = GetMotionPoint(motid, frameno);
			if (pparmp){
				ChaMatrix invpar = pparmp->GetInvWorldMat();
				ChaMatrix invinitmat = GetInvInitMat();
				ChaMatrix localmat = invinitmat * pcurmp->GetWorldMat() * invpar;//world == init * local * parだからlocalを計算するには、invinit * world * invpar。
				pcurmp->CalcQandTra(localmat, this);
			}
			else{
				_ASSERT(0);
				return 0;
			}
		}
		else{
			_ASSERT(0);
			return 0;
		}
	}
	else{
		if (pcurmp){
			ChaMatrix invinitmat = GetInvInitMat();
			ChaMatrix localmat = invinitmat * pcurmp->GetWorldMat();
			pcurmp->CalcQandTra(localmat, this);
		}
		else{
			_ASSERT(0);
			return 0;
		}
	}

	*pdstmp = *pcurmp;

	return 0;
}


int CBone::GetBoneNum()
{
	int retnum = 0;

	if( !m_child ){
		return 0;
	}else{
		retnum++;
	}

	CBone* cbro = m_child->m_brother;
	while( cbro ){
		retnum++;
		cbro = cbro->m_brother;
	}

	return retnum;
}

int CBone::CalcFirstFrameBonePos(ChaMatrix srcmat)
{
	ChaVector3 jpos = GetJointFPos();
	ChaVector3TransformCoord(&m_firstframebonepos, &jpos, &srcmat);

	//if ((m_firstframebonepos.x == 0.0f) && (m_firstframebonepos.y == 0.0f) && (m_firstframebonepos.z == 0.0f)){
	//	_ASSERT(0);
	//}
	return 0;
}

void CBone::CalcFirstAxisMatX()
{
	ChaVector3 curpos;
	ChaVector3 chilpos;

	if (m_child){
		ChaVector3 jpos = GetJointFPos();
		ChaVector3 chiljpos = m_child->GetJointFPos();
		ChaVector3TransformCoord(&curpos, &jpos, &m_firstmat);
		ChaVector3TransformCoord(&chilpos, &chiljpos, &(m_child->m_firstmat));

		CalcAxisMatX_aft(curpos, chilpos, &m_firstaxismatX);
	}
}

int CBone::CalcBoneDepth()
{
	int retdepth = 0;
	CBone* curbone = this;
	while (curbone->GetParent()){
		retdepth++;
		curbone = curbone->GetParent();
	}

	return retdepth;
}

ChaVector3 CBone::GetJointFPos()
{
	if (g_oldaxisflag == 0){
		return m_jointfpos;
	}
	else{
		return m_oldjointfpos;
	}
}
void CBone::SetJointFPos(ChaVector3 srcpos)
{ 
	m_jointfpos = srcpos; 
}
void CBone::SetOldJointFPos(ChaVector3 srcpos){
	m_oldjointfpos = srcpos;
}
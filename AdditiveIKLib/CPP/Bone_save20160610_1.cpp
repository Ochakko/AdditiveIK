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


map<CModel*,int> g_bonecntmap;
extern int g_boneaxis;
extern bool g_limitdegflag;
extern bool g_wmatDirectSetFlag;
extern bool g_underRetargetFlag;

CBone::CBone( CModel* parmodel ) : m_curmp(), m_axisq()
{
	InitParams();

	m_parmodel = parmodel;
	_ASSERT(m_parmodel);

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
	ChaMatrixIdentity(&m_tmpsymmat);

	m_initcustomrigflag = 0;
	//InitCustomRig();//<-- after set name

	InitAngleLimit();
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
	ChaMatrixIdentity(&m_firstaxismatZ);

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

int CBone::InitCustomRig()
{
	int rigno;
	for (rigno = 0; rigno < MAXRIGNUM; rigno++){
		::InitCustomRig(&(m_customrig[rigno]), this, rigno);
	}
	return 0;
}

void CBone::InitAngleLimit()
{
	::InitAngleLimit(&m_anglelimit);
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
	if( !m_child ){
		m_child = childptr;
		m_child->m_parent = this;
	}
	else{
		CBone* broptr = m_child;
		if (broptr){
			while (broptr->m_brother){
				broptr = broptr->m_brother;
			}
			broptr->m_brother = childptr;
			broptr->m_brother->m_parent = this;//!!!!!!!!
		}
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

	if (m_initcustomrigflag == 0){
		InitCustomRig();
		m_initcustomrigflag = 1;
	}

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

	m_axisq.RotationMatrix(m_laxismat);

	return 0;
}

int CBone::CalcAxisMatX()
{
	ChaVector3 curpos;
	ChaVector3 chilpos;

	if (m_parent){
		ChaVector3TransformCoord(&curpos, &(m_parent->GetJointFPos()), &(m_parent->m_startmat2));
		ChaVector3TransformCoord(&chilpos, &(GetJointFPos()), &m_startmat2);

		CalcAxisMatX_aft(curpos, chilpos, &m_gaxismatXpar);
	}

	return 0;
}


int CBone::CalcAxisMatX_aft(ChaVector3 curpos, ChaVector3 chilpos, ChaMatrix* dstmat)
{
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);
	if (curpos == chilpos){
		*dstmat = retmat;
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
	ChaVector3Normalize(&bonevec, &bonevec);

	if ((fabs(bonevec.x) >= 0.000001f) || (fabs(bonevec.y) >= 0.000001f)){
		upvec.x = 0.0f;
		upvec.y = 0.0f;
		upvec.z = 1.0f;
		m_upkind = UPVEC_Z;//vecx1-->X(bone), vecy1-->Y, vecz1-->Z
	}
	else{
		upvec.x = 0.0f;
		upvec.y = 1.0f;
		upvec.z = 0.0f;
		m_upkind = UPVEC_Y;//vecx1-->X(bone), vecy1-->Z, vecz1-->Y
	}

	vecx1 = bonevec;

	ChaVector3Cross(&vecy1, &upvec, &vecx1);
	ChaVector3Normalize(&vecy1, &vecy1);

	ChaVector3Cross(&vecz1, &vecx1, &vecy1);
	ChaVector3Normalize(&vecz1, &vecz1);


	retmat._11 = vecx1.x;
	retmat._12 = vecx1.y;
	retmat._13 = vecx1.z;

	retmat._21 = vecy1.x;
	retmat._22 = vecy1.y;
	retmat._23 = vecy1.z;

	retmat._31 = vecz1.x;
	retmat._32 = vecz1.y;
	retmat._33 = vecz1.z;

	*dstmat = retmat;
	return 0;
}

int CBone::CalcAxisMatZ_aft(ChaVector3 curpos, ChaVector3 chilpos, ChaMatrix* dstmat)
{
	ChaMatrix retmat;
	ChaMatrixIdentity(&retmat);
	if (curpos == chilpos){
		*dstmat = retmat;
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
	ChaVector3Normalize(&bonevec, &bonevec);

	if ((fabs(bonevec.x) >= 0.000001f) || (fabs(bonevec.z) >= 0.000001f)){
		upvec.x = 0.0f;
		upvec.y = 1.0f;
		upvec.z = 0.0f;
		m_upkind = UPVEC_Y;//vecx1-->X, vecy1-->Y, vecz1-->Z(bone)
	}
	else{
		upvec.x = 1.0f;
		upvec.y = 0.0f;
		upvec.z = 0.0f;
		m_upkind = UPVEC_X;//vecx1-->Y, vecy1-->X, vecz1-->Z(bone)
	}

	vecz1 = bonevec;

	//ChaVector3Cross(&vecx1, &vecz1, &upvec);
	ChaVector3Cross(&vecx1, &upvec, &vecz1);
	ChaVector3Normalize(&vecx1, &vecx1);

	ChaVector3Cross(&vecy1, &vecz1, &vecx1);
	ChaVector3Normalize(&vecy1, &vecy1);


	retmat._11 = vecx1.x;
	retmat._12 = vecx1.y;
	retmat._13 = vecx1.z;

	retmat._21 = vecy1.x;
	retmat._22 = vecy1.y;
	retmat._23 = vecy1.z;

	retmat._31 = vecz1.x;
	retmat._32 = vecz1.y;
	retmat._33 = vecz1.z;

	*dstmat = retmat;

	return 0;
}



int CBone::CalcAxisMatY( CBone* chilbone, ChaMatrix* dstmat )
{

	ChaVector3 curpos;
	ChaVector3 chilpos;

	ChaVector3TransformCoord(&curpos, &(GetJointFPos()), &(m_curmp.GetWorldMat()));
	ChaVector3TransformCoord(&chilpos, &(chilbone->GetJointFPos()), &(chilbone->m_curmp.GetWorldMat()));

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
	ChaVector3TransformCoord(&aftbonepos, &(GetJointFPos()), &(m_curmp.GetWorldMat()));

	ChaVector3 aftchilpos;
	ChaVector3TransformCoord(&aftchilpos, &(chilbone->GetJointFPos()), &(chilbone->m_curmp.GetWorldMat()));

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
		if (m_parent){
			ChaVector3TransformCoord(&gparpos, &(m_parent->GetJointFPos()), &(m_parent->m_startmat2));
			ChaVector3TransformCoord(&gchilpos, &(GetJointFPos()), &m_startmat2);
			gbonevec = gchilpos - gparpos;
			ChaVector3Normalize(&gbonevec, &gbonevec);
		}
		else{
			_ASSERT(0);
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
			int numleng = (int)(numend - numstart);
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


	//curmp->SetBefWorldMat( curmp->GetWorldMat() );
	if( parmp ){
		ChaMatrix invbefpar;
		ChaMatrixInverse( &invbefpar, NULL, &parmp->GetBefWorldMat() );
		ChaMatrix tmpmat = curmp->GetWorldMat() * invbefpar * parmp->GetWorldMat();
		g_wmatDirectSetFlag = true;
		SetWorldMat( 0, srcmotid, srcframe, tmpmat );
		g_wmatDirectSetFlag = false;
	}
	else{
		ChaMatrix tramat;
		ChaMatrixTranslation( &tramat, srctra.x, srctra.y, srctra.z );
		ChaMatrix tmpmat = curmp->GetWorldMat() * tramat;
		g_wmatDirectSetFlag = true;
		SetWorldMat(0, srcmotid, srcframe, tmpmat);
		g_wmatDirectSetFlag = false;
	}

	curmp->SetAbsMat( curmp->GetWorldMat() );

	if (m_child){
		m_child->AddBoneTraReq(curmp, srcmotid, srcframe, srctra);
	}
	if (m_brother && parmp){
		m_brother->AddBoneTraReq(parmp, srcmotid, srcframe, srctra);
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
	
	//curmp->SetBefWorldMat( curmp->GetWorldMat() );
	curmp->SetWorldMat( srcmp->GetWorldMat() );
	curmp->SetAbsMat( srcmp->GetAbsMat() );


	//オイラー角初期化
	ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
	int paraxsiflag = 1;
	int isfirstbone = 0;
	cureul = CalcLocalEulZXY(-1, srcmotid, srcframe, BEFEUL_ZERO, isfirstbone);
	SetLocalEul(srcmotid, srcframe, cureul);


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
	

	if (parmp){
		//再帰から呼び出し
		ChaMatrix befparmat = parmp->GetBefWorldMat();
		ChaMatrix newparmat = parmp->GetWorldMat();
		//if ((g_underRetargetFlag == true) || (IsSameMat(befparmat, newparmat) == 0)){
			ChaMatrix invbefpar;
			ChaMatrixInverse(&invbefpar, NULL, &befparmat);
			ChaMatrix tmpmat = curmp->GetWorldMat() * invbefpar * newparmat;
			g_wmatDirectSetFlag = true;
			SetWorldMat(0, srcmotid, srcframe, tmpmat);
			g_wmatDirectSetFlag = false;
		//}
		//else{
			//parmatに変化がないときは変更しない。
		//	curmp->SetBefWorldMat( curmp->GetWorldMat() );
		//}
		if (bvhbone){
			if (m_child){
				bvhbone->SetTmpMat(curmp->GetWorldMat());
			}
			else{
				bvhbone->SetTmpMat(newparmat);
			}
		}
	}
	else{
		//初回呼び出し
		ChaMatrix tramat;
		ChaMatrixIdentity(&tramat);
		ChaMatrixTranslation(&tramat, traanim.x, traanim.y, traanim.z);

		if (m_child){
			if (setmatflag == 0){
				ChaVector3 rotcenter;// = m_childworld;
				ChaVector3TransformCoord(&rotcenter, &(GetJointFPos()), &(curmp->GetWorldMat()));

				ChaMatrix befrot, aftrot;
				ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
				ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
				ChaMatrix rotmat = befrot * rotq.MakeRotMatX() * aftrot;
				ChaMatrix tmpmat = curmp->GetWorldMat() * rotmat * tramat;
				SetWorldMat(0, srcmotid, srcframe, tmpmat);
				if (bvhbone){
					bvhbone->SetTmpMat(tmpmat);
				}
			}
			else{
				ChaMatrix tmpmat = *psetmat;
				g_wmatDirectSetFlag = true;
				SetWorldMat(0, srcmotid, srcframe, tmpmat);
				g_wmatDirectSetFlag = false;
				if (bvhbone){
					bvhbone->SetTmpMat(tmpmat);
				}
			}
		}
		else{
			ChaVector3 rotcenter;// = m_childworld;
			ChaVector3TransformCoord(&rotcenter, &(GetJointFPos()), &(curmp->GetWorldMat()));

			ChaMatrix befrot, aftrot;
			ChaMatrixTranslation(&befrot, -rotcenter.x, -rotcenter.y, -rotcenter.z);
			ChaMatrixTranslation(&aftrot, rotcenter.x, rotcenter.y, rotcenter.z);
			ChaMatrix rotmat = befrot * rotq.MakeRotMatX() * aftrot;
			ChaMatrix tmpmat = curmp->GetWorldMat() * rotmat * tramat;
			g_wmatDirectSetFlag = true;//!!!!!!!!
			SetWorldMat(0, srcmotid, srcframe, tmpmat);
			g_wmatDirectSetFlag = false;//!!!!!!!
			if (bvhbone){
				bvhbone->SetTmpMat(tmpmat);
			}
		}
	}


	curmp->SetAbsMat(curmp->GetWorldMat());

	if (m_child && curmp){
		m_child->RotBoneQReq(curmp, srcmotid, srcframe, rotq);
	}
	if (m_brother && parmp){
		m_brother->RotBoneQReq(parmp, srcmotid, srcframe, rotq);
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

	if (parmp){
		//parentの行列をセット !!!!!!!!!
		g_wmatDirectSetFlag = true;
		SetWorldMat(0, srcmotid, srcframe, parmp->GetWorldMat());
		g_wmatDirectSetFlag = false;
	} else{
		g_wmatDirectSetFlag = true;
		SetWorldMat(0, srcmotid, srcframe, srcmat);
		g_wmatDirectSetFlag = false;
	}

	curmp->SetAbsMat(curmp->GetWorldMat());

	return curmp;
}


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

	g_wmatDirectSetFlag = true;
	SetWorldMat(0, srcmotid, srcframe, firstmp->GetAbsMat());
	g_wmatDirectSetFlag = false;

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
				CMotionPoint setmp;
				ChaMatrix invpar = pparmp->GetInvWorldMat();
				ChaMatrix localmat = pcurmp->GetWorldMat() * invpar;
				//pcurmp->CalcQandTra(localmat, this);
				setmp.CalcQandTra(localmat, this);

				int inirotcur, inirotpar;
				inirotcur = IsInitRot(pcurmp->GetWorldMat());
				inirotpar = IsInitRot(pparmp->GetWorldMat());
				if (inirotcur && inirotpar){
					CQuaternion iniq;
					iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
					//pcurmp->SetQ(iniq);
					setmp.SetQ(iniq);
				}

				*pdstmp = setmp;
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
			CMotionPoint setmp;
			ChaMatrix localmat = pcurmp->GetWorldMat();
			setmp.CalcQandTra( localmat, this );

			int inirotcur;
			inirotcur = IsInitRot(pcurmp->GetWorldMat());
			if (inirotcur ){
				CQuaternion iniq;
				iniq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
				setmp.SetQ(iniq);
			}

			*pdstmp = setmp;

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

	if (m_parent){
		CalcAxisMatX_aft(m_parent->GetJointFPos(), GetJointFPos(), &m_firstaxismatX);
	}

}
void CBone::CalcFirstAxisMatZ()
{
	ChaVector3 curpos;
	ChaVector3 chilpos;

	if (m_parent){
		CalcAxisMatZ_aft(m_parent->GetJointFPos(), GetJointFPos(), &m_firstaxismatZ);
	}

}

int CBone::CalcBoneDepth()
{
	int retdepth = 0;
	CBone* curbone = this;
	if (curbone){
		while (curbone->GetParent()){
			retdepth++;
			curbone = curbone->GetParent();
		}
	}
	return retdepth;
}

ChaVector3 CBone::GetJointFPos()
{
	
	if (m_parmodel->GetOldAxisFlagAtLoading() == 0){
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


ChaVector3 CBone::CalcLocalEulZXY(int axiskind, int srcmotid, double srcframe, enum tag_befeulkind befeulkind, int isfirstbone)
{
	//axiskind : BONEAXIS_*  or  -1(CBone::m_anglelimit.boneaxiskind)

	ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);
	ChaVector3 befeul = ChaVector3(0.0f, 0.0f, 0.0f);

	const WCHAR* bonename = GetWBoneName();
	if (wcscmp(bonename, L"RootNode") == 0){
		return cureul;//!!!!!!!!!!!!!!!!!!!!!!!!
	}

	if (befeulkind == BEFEUL_BEFFRAME){
		//1つ前のフレームのEULはすでに計算されていると仮定する。
		double befframe;
		befframe = srcframe - 1.0;
		if (befframe >= -0.0001){
			CMotionPoint* befmp;
			befmp = GetMotionPoint(srcmotid, befframe);
			if (befmp){
				befeul = befmp->GetLocalEul();
			}
		}
	}

	CMotionPoint tmpmp;
	CalcLocalInfo(srcmotid, srcframe, &tmpmp);//local!!!
	
	ChaMatrix axismat;
	CQuaternion axisq;
	int multworld = 0;//local!!!
	axismat = CalcManipulatorMatrix(1, 0, multworld, srcmotid, srcframe);
	axisq.RotationMatrix(axismat);

	if (axiskind == -1){
		if (m_anglelimit.boneaxiskind != BONEAXIS_GLOBAL){
			tmpmp.GetQ().CalcFBXEul(&axisq, befeul, &cureul, isfirstbone);
		}
		else{
			tmpmp.GetQ().CalcFBXEul(0, befeul, &cureul, isfirstbone);
		}
	}
	else if (axiskind != BONEAXIS_GLOBAL){
		tmpmp.GetQ().CalcFBXEul(&axisq, befeul, &cureul, isfirstbone);
	}
	else{
		tmpmp.GetQ().CalcFBXEul(0, befeul, &cureul, isfirstbone);
	}

	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, srcframe);
	if (curmp){
		ChaVector3 oldeul = curmp->GetLocalEul();
		if (IsSameEul(oldeul, cureul) == 0){
			return cureul;
		}
		else{
			return oldeul;
		}
	}
	else{
		return cureul;
	}
}

ChaMatrix CBone::CalcManipulatorMatrix(int anglelimitaxisflag, int settraflag, int multworld, int srcmotid, double srcframe)
{
	ChaMatrix selm;
	ChaMatrixIdentity(&selm);

	CMotionPoint* pcurmp = 0;
	CMotionPoint* pparmp = 0;
	pcurmp = GetMotionPoint(srcmotid, srcframe);
	if (!pcurmp){
		//_ASSERT(0);
		return selm;
	}
	if (m_parent){
		pparmp = m_parent->GetMotionPoint(srcmotid, srcframe);
		if (!pparmp){
			_ASSERT(0);
			return selm;
		}
	}

	if (anglelimitaxisflag == 0){
		if (g_boneaxis == 2){
			//global axis
			ChaMatrixIdentity(&selm);
		}
		else if (g_boneaxis == 0){
			//current bone axis
			if (m_child){
				if (m_parmodel->GetOldAxisFlagAtLoading() == 1){
					//FBXの初期のボーンの向きがIdentityの場合
					if (m_parent){
						if (GetBoneLeng() > 0.00001f){
							if (multworld == 1){
								selm = GetFirstAxisMatZ() * pcurmp->GetWorldMat();
							}
							else{
								selm = GetFirstAxisMatZ();
							}
						}
						else{
							if (multworld == 1){
								selm = pcurmp->GetWorldMat();
							}
							else{
								ChaMatrixIdentity(&selm);
							}
						}
					}
					else{
						if (multworld == 1){
							selm = pcurmp->GetWorldMat();
						}
						else{
							ChaMatrixIdentity(&selm);
						}
					}
				}
				else{
					//FBXにボーンの初期の軸の向きが記録されている場合
					if (m_parent){
						if (multworld == 1){
							selm = GetNodeMat() * pcurmp->GetWorldMat();
						}
						else{
							selm = GetNodeMat();
						}
					}
					else{
						if (multworld == 1){
							selm = GetNodeMat() * pcurmp->GetWorldMat();
						}
						else{
							selm = GetNodeMat();
						}
					}
				}
			}
			else{
				//endjoint
				if (m_parent){
					if (multworld == 1){
						selm = pcurmp->GetWorldMat();
					}
					else{
						ChaMatrixIdentity(&selm);
					}
				}
				else{
					if (multworld == 1){
						selm = pcurmp->GetWorldMat();
					}
					else{
						ChaMatrixIdentity(&selm);
					}
				}
			}
		}
		else if (g_boneaxis == 1){
			//parent bone axis
			if (m_child){
				if (m_parmodel->GetOldAxisFlagAtLoading() == 1){
					//FBXの初期のボーンの向きがIdentityの場合
					if (m_parent){
						if (GetBoneLeng() > 0.00001f){
							if (multworld == 1){
								selm = GetFirstAxisMatZ() * pparmp->GetWorldMat();
							}
							else{
								selm = GetFirstAxisMatZ();
							}
						}
						else{
							if (multworld == 1){
								selm = pcurmp->GetWorldMat();
							}
							else{
								ChaMatrixIdentity(&selm);
							}
						}
					}
					else{
						if (multworld == 1){
							selm = pcurmp->GetWorldMat();
						}
						else{
							ChaMatrixIdentity(&selm);
						}
					}
				}
				else{
					//FBXにボーンの初期の軸の向きが記録されている場合
					if (m_parent){
						if (multworld == 1){
							selm = GetNodeMat() * pparmp->GetWorldMat();
						}
						else{
							selm = GetNodeMat();
						}
					}
					else{
						if (multworld == 1){
							selm = GetNodeMat() * pcurmp->GetWorldMat();
						}
						else{
							selm = GetNodeMat();
						}
					}
				}
			}
			else{
				//endjoint
				if (m_parent){
					if (multworld == 1){
						selm = pparmp->GetWorldMat();
					}
					else{
						ChaMatrixIdentity(&selm);
					}
				}
				else{
					if (multworld == 1){
						selm = pcurmp->GetWorldMat();
					}
					else{
						ChaMatrixIdentity(&selm);
					}
				}
			}
		}
		else{
			_ASSERT(0);
			ChaMatrixIdentity(&selm);
		}
	}
	else{
		if (m_anglelimit.boneaxiskind == 2){
			//global axis
			ChaMatrixIdentity(&selm);
		}
		else if (m_anglelimit.boneaxiskind == 0){
			//current bone axis
			if (m_child){
				if (m_parmodel->GetOldAxisFlagAtLoading() == 1){
					//FBXの初期のボーンの向きがIdentityの場合
					if (m_parent){
						if (GetBoneLeng() > 0.00001f){
							if (multworld == 1){
								selm = GetFirstAxisMatZ() * pcurmp->GetWorldMat();
							}
							else{
								selm = GetFirstAxisMatZ();
							}
						}
						else{
							if (multworld == 1){
								selm = pcurmp->GetWorldMat();
							}
							else{
								ChaMatrixIdentity(&selm);
							}
						}
					}
					else{
						if (multworld == 1){
							selm = pcurmp->GetWorldMat();
						}
						else{
							ChaMatrixIdentity(&selm);
						}
					}
				}
				else{
					//FBXにボーンの初期の軸の向きが記録されている場合
					if (m_parent){
						if (multworld == 1){
							selm = GetNodeMat() * pcurmp->GetWorldMat();
						}
						else{
							selm = GetNodeMat();
						}
					}
					else{
						if (multworld == 1){
							selm = GetNodeMat() * pcurmp->GetWorldMat();
						}
						else{
							selm = GetNodeMat();
						}
					}
				}
			}
			else{
				//endjoint
				if (m_parent){
					if (multworld == 1){
						selm = pcurmp->GetWorldMat();
					}
					else{
						ChaMatrixIdentity(&selm);
					}
				}
				else{
					if (multworld == 1){
						selm = pcurmp->GetWorldMat();
					}
					else{
						ChaMatrixIdentity(&selm);
					}
				}
			}
		}
		else if (m_anglelimit.boneaxiskind == 1){
			//parent bone axis
			if (m_child){
				if (m_parmodel->GetOldAxisFlagAtLoading() == 1){
					//FBXの初期のボーンの向きがIdentityの場合
					if (m_parent){
						if (GetBoneLeng() > 0.00001f){
							if (multworld == 1){
								selm = GetFirstAxisMatZ() * pparmp->GetWorldMat();
							}
							else{
								selm = GetFirstAxisMatZ();
							}
						}
						else{
							if (multworld == 1){
								selm = pcurmp->GetWorldMat();
							}
							else{
								ChaMatrixIdentity(&selm);
							}
						}
					}
					else{
						if (multworld == 1){
							selm = pcurmp->GetWorldMat();
						}
						else{
							ChaMatrixIdentity(&selm);
						}
					}
				}
				else{
					//FBXにボーンの初期の軸の向きが記録されている場合
					if (m_parent){
						if (multworld == 1){
							selm = GetNodeMat() * pparmp->GetWorldMat();
						}
						else{
							selm = GetNodeMat();
						}
					}
					else{
						if (multworld == 1){
							selm = GetNodeMat() * pcurmp->GetWorldMat();
						}
						else{
							selm = GetNodeMat();
						}
					}
				}
			}
			else{
				//endjoint
				if (m_parent){
					if (multworld == 1){
						selm = pparmp->GetWorldMat();
					}
					else{
						ChaMatrixIdentity(&selm);
					}
				}
				else{
					if (multworld == 1){
						selm = pcurmp->GetWorldMat();
					}
					else{
						ChaMatrixIdentity(&selm);
					}
				}
			}
		}
		else{
			_ASSERT(0);
			ChaMatrixIdentity(&selm);
		}
	}

	if (settraflag == 0){
		selm._41 = 0.0f;
		selm._42 = 0.0f;
		selm._43 = 0.0f;
	}
	else{
		ChaVector3 aftjpos;
		ChaVector3TransformCoord(&aftjpos, &(GetJointFPos()), &(pcurmp->GetWorldMat()));

		selm._41 = aftjpos.x;
		selm._42 = aftjpos.y;
		selm._43 = aftjpos.z;
	}


	return selm;
}


int CBone::SetWorldMatFromEul(int setchildflag, ChaVector3 srceul, int srcmotid, double srcframe)
{
	//anglelimitをした後のオイラー角が渡される。anglelimitはCBone::SetWorldMatで処理する。
	if (!m_child){
		return 0;
	}

	ChaMatrix axismat;
	CQuaternion axisq;
	int multworld = 0;//local!!!
	axismat = CalcManipulatorMatrix(1, 0, multworld, srcmotid, srcframe);
	axisq.RotationMatrix(axismat);
	CQuaternion invaxisq;
	axisq.inv(&invaxisq);

	CQuaternion newrot;
	if (m_anglelimit.boneaxiskind != BONEAXIS_GLOBAL){
		newrot.SetRotation(&axisq, srceul);
	}
	else{
		newrot.SetRotation(0, srceul);
	}

	ChaMatrix newlocalmat, newrotmat, befrotmat, aftrotmat;
	newrotmat = newrot.MakeRotMatX();
	ChaMatrixIdentity(&befrotmat);
	ChaMatrixTranslation(&befrotmat, -GetJointFPos().x, -GetJointFPos().y, -GetJointFPos().z);
	ChaMatrixIdentity(&aftrotmat);
	ChaMatrixTranslation(&aftrotmat, GetJointFPos().x, GetJointFPos().y, GetJointFPos().z);
	newlocalmat = befrotmat * newrotmat * aftrotmat;

	CMotionPoint oldmp;
	CalcLocalInfo(srcmotid, srcframe, &oldmp);//local!!!
	ChaVector3 oldtravec;
	oldtravec = oldmp.GetTra();
	ChaMatrix oldtramat;
	ChaMatrixIdentity(&oldtramat);
	ChaMatrixTranslation(&oldtramat, oldtravec.x, oldtravec.y, oldtravec.z);

	ChaMatrix newmat;
	if (m_parent){
		CMotionPoint* parmp;
		parmp = m_parent->GetMotionPoint(srcmotid, srcframe);
		if (parmp){
			ChaMatrix parmat;
			parmat = parmp->GetWorldMat();
			newmat = newlocalmat * parmat;
		}
		else{
			_ASSERT(0);
			newmat = newlocalmat;
		}
	}
	else{
		newmat = newlocalmat;
	}

	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, srcframe);
	if (curmp){
		//curmp->SetBefWorldMat(curmp->GetWorldMat());
		curmp->SetWorldMat(newmat);
		curmp->SetLocalEul(srceul);

		if (setchildflag == 1){
			if (m_child){
				CQuaternion dummyq;
				m_child->RotBoneQReq(curmp, srcmotid, srcframe, dummyq);
			}
		}
	}
	else{
		_ASSERT(0);
	}

	return 0;
}

int CBone::SetLocalEul(int srcmotid, double srcframe, ChaVector3 srceul)
{
	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, srcframe);
	if (!curmp){
		_ASSERT(0);
		return 1;
	}

	curmp->SetLocalEul(srceul);

	return 0;

}

void CBone::SetWorldMat(int setchildflag, int srcmotid, double srcframe, ChaMatrix srcmat)
{
	//if pose is change, return 1 else return 0
	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, srcframe);
	if (!curmp){
		return;
	}

	if ((g_wmatDirectSetFlag == false) && (g_underRetargetFlag == false)){
		ChaMatrix saveworldmat;
		saveworldmat = curmp->GetWorldMat();

		ChaVector3 oldeul = ChaVector3(0.0f, 0.0f, 0.0f);
		int paraxsiflag = 1;
		int isfirstbone = 0;
		oldeul = CalcLocalEulZXY(-1, srcmotid, srcframe, BEFEUL_ZERO, isfirstbone);


		curmp->SetWorldMat(srcmat);//tmp time
		ChaVector3 neweul = ChaVector3(0.0f, 0.0f, 0.0f);
		neweul = CalcLocalEulZXY(-1, srcmotid, srcframe, BEFEUL_ZERO, isfirstbone);

		curmp->SetWorldMat(saveworldmat);

		int ismovable = ChkMovableEul(neweul);
		if (ismovable == 1){
			if (IsSameEul(oldeul, neweul) == 0){
				SetWorldMatFromEul(setchildflag, neweul, srcmotid, srcframe);//setchildflag有り!!!!
			}
			else{
				curmp->SetBefWorldMat(curmp->GetWorldMat());
			}
		}
		else{
			curmp->SetBefWorldMat(curmp->GetWorldMat());
		}
	}
	else{
		//curmp->SetBefWorldMat(curmp->GetWorldMat());
		curmp->SetWorldMat(srcmat);

		ChaVector3 neweul = CalcLocalEulZXY(-1, srcmotid, srcframe, BEFEUL_ZERO, 0);
		curmp->SetLocalEul(neweul);
	}
	/*
	if (setchildflag){
		if (GetChild()){
			CQuaternion dummyq;
			GetChild()->RotBoneQReq(curmp, srcmotid, srcframe, dummyq);
		}
	}
	*/
}

int CBone::ChkMovableEul(ChaVector3 srceul)
{
	if (g_limitdegflag == false){
		return 1;//movable
	}

	int dontmove = 0;
	int axiskind;

	float chkval[3];
	chkval[0] = srceul.x;
	chkval[1] = srceul.y;
	chkval[2] = srceul.z;

	for (axiskind = AXIS_X; axiskind <= AXIS_Z; axiskind++){
		if (m_anglelimit.via180flag[axiskind] == 0){
			if ((m_anglelimit.lower[axiskind] > (int)chkval[axiskind]) || (m_anglelimit.upper[axiskind] < (int)chkval[axiskind])){
				dontmove++;
			}
		}
		else{
			if ((m_anglelimit.lower[axiskind] <= (int)chkval[axiskind]) && (m_anglelimit.upper[axiskind] >= (int)chkval[axiskind])){
				dontmove++;
			}
		}
	}

	if (dontmove != 0){
		return 0;
	}
	else{
		return 1;//movable
	}
}


float CBone::LimitAngle(enum tag_axiskind srckind, float srcval)
{
	SetAngleLimitOff(&m_anglelimit);
	if (m_anglelimit.limitoff[srckind] == 1){
		return srcval;
	}
	else{
		int newval;
		newval = min((int)srcval, m_anglelimit.upper[srckind]);
		newval = max(newval, m_anglelimit.lower[srckind]);
		return (float)newval;
	}
}


ChaVector3 CBone::LimitEul(ChaVector3 srceul)
{
	ChaVector3 reteul = ChaVector3(0.0f, 0.0f, 0.0f);

	reteul.x = LimitAngle(AXIS_X, srceul.x);
	reteul.y = LimitAngle(AXIS_Y, srceul.y);
	reteul.z = LimitAngle(AXIS_Z, srceul.z);

	return reteul;
}

ANGLELIMIT CBone::GetAngleLimit()
{
	::SetAngleLimitOff(&m_anglelimit);
	return m_anglelimit;
};
void CBone::SetAngleLimit(ANGLELIMIT srclimit)
{
	m_anglelimit = srclimit;

	int axiskind;
	for (axiskind = AXIS_X; axiskind < AXIS_MAX; axiskind++){
		if (m_anglelimit.lower[axiskind] < -180){
			m_anglelimit.lower[axiskind] = -180;
		}
		else if (m_anglelimit.lower[axiskind] > 180){
			m_anglelimit.lower[axiskind] = 180;
		}

		if (m_anglelimit.upper[axiskind] < -180){
			m_anglelimit.upper[axiskind] = -180;
		}
		else if (m_anglelimit.upper[axiskind] > 180){
			m_anglelimit.upper[axiskind] = 180;
		}

		if (m_anglelimit.lower[axiskind] > m_anglelimit.upper[axiskind]){
			_ASSERT(0);
			//swap
			int tmpval = m_anglelimit.lower[axiskind];
			m_anglelimit.lower[axiskind] = m_anglelimit.upper[axiskind];
			m_anglelimit.upper[axiskind] = tmpval;
		}
	}
	SetAngleLimitOff(&m_anglelimit);
};


int CBone::GetFreeCustomRigNo()
{
	int rigno;
	int findrigno = -1;
	for (rigno = 0; rigno < MAXRIGNUM; rigno++){
		if (m_customrig[rigno].useflag == 0){//0 : free mark
			findrigno = rigno;
			break;
		}
	}
	
	if (findrigno == -1){
		//freeがなかった場合、rentalから
		for (rigno = 0; rigno < MAXRIGNUM; rigno++){
			if (m_customrig[rigno].useflag == 1){//1 : rental mark
				findrigno = rigno;
				break;
			}
		}
	}

	return findrigno;
}
CUSTOMRIG CBone::GetFreeCustomRig()
{
	int freerigno = GetFreeCustomRigNo();
	if ((freerigno >= 0) && (freerigno < MAXRIGNUM)){
		m_customrig[freerigno].useflag = 1;//1 : rental mark
		//_ASSERT(0);
		return m_customrig[freerigno];
	}
	else{
		_ASSERT(0);
		CUSTOMRIG dummycr;
		::InitCustomRig(&dummycr, 0, 0);
		return dummycr;
	}
}

CUSTOMRIG CBone::GetCustomRig(int rigno)
{
	if ((rigno >= 0) && (rigno < MAXRIGNUM)){
		return m_customrig[rigno];
	}
	else{
		_ASSERT(0);
		CUSTOMRIG dummycr;
		::InitCustomRig(&dummycr, 0, 0);
		return dummycr;
	}
}
void CBone::SetCustomRig(CUSTOMRIG srccr)
{
	int isvalid = IsValidCustomRig(m_parmodel, srccr, this);
	if (isvalid == 1){
		m_customrig[srccr.rigno] = srccr;
		m_customrig[srccr.rigno].useflag = 2;//2 : valid mark
	}
}

ChaMatrix CBone::CalcSymXMat(int srcmotid, double srcframe)
{
	return CalcSymXMat2(srcmotid, srcframe);
}

ChaMatrix CBone::CalcSymXMat2(int srcmotid, double srcframe)
{
	ChaMatrix symrotmat;
	ChaVector3 bonepos;
	ChaMatrix befrotmat, aftrotmat;
	bonepos = GetJointFPos();
	ChaMatrixIdentity(&befrotmat);
	ChaMatrixIdentity(&aftrotmat);
	befrotmat._41 = -bonepos.x;
	befrotmat._42 = -bonepos.y;
	befrotmat._43 = -bonepos.z;
	aftrotmat._41 = bonepos.x;
	aftrotmat._42 = bonepos.y;
	aftrotmat._43 = bonepos.z;


	if (!GetParent()){
		//root boneはそのまま。位置も向きもそのまま。
		ChaMatrix directsetmat = GetWorldMat(srcmotid, srcframe);
		return directsetmat;
	}
	else{
		int symboneno = 0;
		int existflag = 0;
		m_parmodel->GetSymBoneNo(GetBoneNo(), &symboneno, &existflag);
		if (symboneno >= 0){
			CBone* symbone = m_parmodel->GetBoneByID(symboneno);
			_ASSERT(symbone);
			if (symbone){

				//if (symbone == this){
				//	WCHAR dbgmes[1024];
				//	swprintf_s(dbgmes, 1024, L"CalcSymRotQ : frame %lf : samebone : this[%s]--sym[%s]", srcframe, GetWBoneName(), symbone->GetWBoneName());
				//	::MessageBox(NULL, dbgmes, L"check", MB_OK);
				//}

				CMotionPoint symlocalmp;
				symbone->CalcLocalInfo(srcmotid, srcframe, &symlocalmp);
				symrotmat = symlocalmp.GetQ().CalcSymX2();

				symrotmat._41 = 0.0f;
				symrotmat._42 = 0.0f;
				symrotmat._43 = 0.0f;

				ChaMatrix directsetmat = befrotmat * symrotmat * aftrotmat;

				return directsetmat;
			}
			else{
				CMotionPoint curlocalmp;
				CalcLocalInfo(srcmotid, srcframe, &curlocalmp);
				symrotmat = curlocalmp.GetQ().MakeRotMatX();
				ChaMatrix directsetmat = befrotmat * symrotmat * aftrotmat;
				_ASSERT(0);

				return directsetmat;
			}
		}
		else{
			CMotionPoint curlocalmp;
			CalcLocalInfo(srcmotid, srcframe, &curlocalmp);
			symrotmat = curlocalmp.GetQ().MakeRotMatX();
			ChaMatrix directsetmat = befrotmat * symrotmat * aftrotmat;
			_ASSERT(0);

			return directsetmat;

		}
	}
}


ChaMatrix CBone::GetWorldMat(int srcmotid, double srcframe)
{
	ChaMatrix curmat;
	ChaMatrixIdentity(&curmat);
	CMotionPoint* curmp;
	curmp = GetMotionPoint(srcmotid, srcframe);
	if (curmp){
		curmat = curmp->GetWorldMat();
	}
	return curmat;
}

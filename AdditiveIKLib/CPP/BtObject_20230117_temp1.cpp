#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>


#include <GlobalVar.h>

#include <BtObject.h>

#include <Model.h>
#include <Bone.h>
//#include <quaternion.h>
#include <RigidElem.h>


//#include <BoneProp.h>

#define DBGH
#include <dbg.h>

using namespace std;

extern double g_erp;

/*
extern float g_miscale;
extern float g_l_kval[3];
extern float g_a_kval[3];
extern int g_previewFlag;			// プレビューフラグ
*/
CBtObject::CBtObject( CBtObject* parbt, btDynamicsWorld* btWorld )
{
	InitParams();
	//if( parbt ){
	//	parbt->AddChild( this );
	//}
	m_btWorld = btWorld;
}

CBtObject::~CBtObject()
{
	DestroyObjs();
}

int CBtObject::InitParams()
{
	ChaMatrixIdentity(&m_firstTransformMatX);
	m_firstTransform.setIdentity();

	m_btpos = ChaVector3(0.0f, 0.0f, 0.0f);

	m_connectflag = 0;
	m_constzrad = 0.0f;
	ChaMatrixIdentity( &m_transmat );
	ChaMatrixIdentity( &m_xworld );
	m_btWorld = 0;

	m_topflag = 0;
	m_parentbone = 0;
	m_endbone = 0;
	m_bone = 0;
	m_colshape = 0;
	m_rigidbody = 0;
	m_constraint.clear();

	m_gz_colshape = 0;
	m_gz_rigidbody = 0;
	m_gz_vecconstraint.clear();



	m_parbt = 0;
	m_chilbt.clear();

	m_curpivot.setValue( 0.0f, 0.0f, 0.0f );
	m_chilpivot.setValue( 0.0f, 0.0f, 0.0f );

	m_FrameA.setIdentity();
	m_FrameB.setIdentity();
	m_firstTransformMat.setIdentity();//bto->GetRigidBody()のCreateBtObject時のWorldTransform->getBasis

	m_btq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);

	return 0;
}
int CBtObject::DestroyObjs()
{
	int chilno;
	for( chilno = 0; chilno < (int)m_constraint.size(); chilno++ ){
		//btTypedConstraint* constptr = m_constraint[ chilno ];
		btGeneric6DofSpringConstraint* constptr = m_constraint[chilno].constraint;
		if( constptr ){
			m_btWorld->removeConstraint( constptr );
			delete constptr;
		}
	}
	m_constraint.clear();

	if( m_rigidbody ){
		if( m_rigidbody->getMotionState() ){
			delete m_rigidbody->getMotionState();
		}
		m_btWorld->removeRigidBody( m_rigidbody );
		delete m_rigidbody;
		m_rigidbody = 0;
	}

	if( m_colshape ){
		delete m_colshape;
		m_colshape = 0;
	}

	DestroyPhysicsPosConstraint();


	m_chilbt.clear();

	return 0;
}

void CBtObject::DestroyGZObj()
{
	for (int i = 0; i < m_gz_vecconstraint.size(); i++) {
		CONSTRAINTELEM curce = m_gz_vecconstraint[i];
		btGeneric6DofSpringConstraint* constptr = curce.constraint;
		if (constptr) {
			m_btWorld->removeConstraint(constptr);
			delete constptr;
		}
	}
	m_gz_vecconstraint.clear();

	if (m_gz_rigidbody){
		if (m_gz_rigidbody->getMotionState()){
			delete m_gz_rigidbody->getMotionState();
		}
		m_btWorld->removeRigidBody(m_gz_rigidbody);
		delete m_gz_rigidbody;
		m_gz_rigidbody = 0;
	}

	if (m_gz_colshape){
		delete m_gz_colshape;
		m_gz_colshape = 0;
	}
}

//localInteria = btVector3(0.0, 0.0, 0.0)
btRigidBody* CBtObject::localCreateRigidBody( CRigidElem* curre, const btTransform& startTransform, btCollisionShape* shape, btVector3 localInertia)
{
	_ASSERT( shape );
	static int s_cnt = 0;

	btRigidBody* body = 0;

	if (curre){
		//bool isDynamic = (curre->GetMass() != 0.f);
		bool isDynamic = true;
		//btVector3 localInertia(0, 0, 0);
		//btVector3 localInertia(0, -m_boneleng * 0.5f, 0);
		if (isDynamic)
			shape->calculateLocalInertia(curre->GetMass(), localInertia);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(curre->GetMass(), myMotionState, shape, localInertia);
		body = new btRigidBody(rbInfo);


		//body->setRestitution(curre->GetRestitution());
		//body->setFriction(curre->GetFriction());
		body->setRestitution(0.0);
		body->setFriction(1.0);
		if (g_previewFlag != 5){
			body->setDamping(0.3, 0.6);
		}else{
			//body->setDamping(0.3, 1.0);
			//body->setDamping(1.0, 1.0);
			body->setDamping(0.7, 0.7);
		}


		int myid;// = curre->GetGroupid();
		int coliid;
		if (g_previewFlag == 5){
			s_cnt++;
			myid = s_cnt;//!!!!!!!!!!!!!!!!!!!!!
			coliid = 65536;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		else{
			myid = curre->GetGroupid();
			coliid = curre->GetColiID();
		}
		m_btWorld->addRigidBody(body, myid, coliid);
	}
	else{
		bool isDynamic = false;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		btScalar setmass = 0.0;

		//btVector3 localInertia(0, 0, 0);
		//if (isDynamic)
			shape->calculateLocalInertia(setmass, localInertia);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(setmass, myMotionState, shape, localInertia);
		body = new btRigidBody(rbInfo);

		body->setRestitution(0.0);
		body->setFriction(1.0);
		//body->setDamping(0.3, 0.6);

		//int myid = 999;
		//int coliid = 0;
		s_cnt++;
		int myid = s_cnt;//!!!!!!!!!!!!!!!!!!!!!
		int coliid = 65536;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		m_btWorld->addRigidBody(body, myid, coliid);

	}
	return body;
}
int CBtObject::AddChild( CBtObject* addbt )
{
	m_chilbt.push_back( addbt );
	return 0;
}

int CBtObject::CreateObject( CBtObject* parbt, CBone* parentbone, CBone* curbone, CBone* childbone )
{
	m_bone = curbone;
	m_parentbone = parentbone;
	m_parbt = parbt;
	m_endbone = childbone;

	if( !m_bone ){
		return 0;
	}
	if( !m_endbone ){
		return 0;
	}

	m_bone->SetFirstCalcRigid(true);
	m_endbone->SetFirstCalcRigid(true);


	CRigidElem* curre = m_bone->GetRigidElem( childbone );
	if( !curre ){
		_ASSERT( 0 );
		return 1;
	}

	//if( curre && (curre->GetSkipflag() == 1) ){
	//	return 0;
	//}

	ChaVector3 centerA, parentposA, childposA, aftparentposA, aftchildposA;
	ChaMatrix tmpzerofm = m_bone->GetCurrentZeroFrameMat(0);
	parentposA = m_bone->GetJointFPos();
	ChaVector3TransformCoord(&aftparentposA, &parentposA, &tmpzerofm);
	childposA = m_endbone->GetJointFPos();
	//ChaVector3TransformCoord(&aftchildposA, &childposA, &m_endbone->GetCurrentZeroFrameMat(0));
	ChaVector3TransformCoord(&aftchildposA, &childposA, &tmpzerofm);
	ChaVector3 diffA = childposA - parentposA;
	m_boneleng = (float)ChaVector3LengthDbl(&diffA);

	float h, r, z;
	//max : boneleng 0 対策
	r = max(0.0001f, curre->GetSphr());// * 0.95f;
	h = max(0.0001f, curre->GetCylileng());// *0.70f;//!!!!!!!!!!!!!
	z = max(0.0001f, curre->GetBoxz());

	//double lengrate = 1.0;
	//double lengrate = 0.95;

	double lengrate;
	double rrate;

	if (g_previewFlag == 5){
		//lengrate = 0.90;
		//rrate = 0.30;
		//rrate = 0.1;

		lengrate = 0.90;
		rrate = 0.050;
	}
	else{
		lengrate = 0.90;
		rrate = 0.90;
	}


	if( curre->GetColtype() == COL_CAPSULE_INDEX ){
		m_colshape = new btCapsuleShape(btScalar(r * rrate), btScalar(h * lengrate));//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		_ASSERT( m_colshape );
	}else if( curre->GetColtype() == COL_CONE_INDEX ){
		m_colshape = new btConeShape(btScalar(r * rrate), btScalar(h * lengrate));
		_ASSERT( m_colshape );
	}else if( curre->GetColtype() == COL_SPHERE_INDEX ){
		m_colshape = new btSphereShape(btScalar(r * rrate));
		_ASSERT( m_colshape );
	}else if( curre->GetColtype() == COL_BOX_INDEX ){
		m_colshape = new btBoxShape(btVector3(btScalar(r * rrate), btScalar(h * lengrate), btScalar(z * rrate)));
		_ASSERT( m_colshape );
	}else{
		_ASSERT( 0 );
		return 1;
	}


//	if( m_boneleng < 0.00001f ){
//		_ASSERT( 0 );
//	}


	ChaMatrix startrot = curre->GetCapsulemat(1);
	//ChaMatrix startrot = m_bone->CalcManipulatorPostureMatrix(0, 0, 1);

	//m_transmat = startrot;


	CQuaternion startrotq;
	startrotq.RotationMatrix(startrot);

	btScalar qx = startrotq.x;
	btScalar qy = startrotq.y;
	btScalar qz = startrotq.z;
	btScalar qw = startrotq.w;
	btQuaternion btq( qx, qy, qz, qw );

	centerA = ( aftparentposA + aftchildposA ) * 0.5f;
	//centerA = aftparentposA;
	btVector3 btv( btScalar( centerA.x ), btScalar( centerA.y ), btScalar( centerA.z ) );

	btTransform transform;
	transform.setIdentity();
	transform.setRotation( btq );
	transform.setOrigin( btv );


	m_btq.SetParams(btq.getW(), btq.getX(), btq.getY(), btq.getZ());//2023/01/17



	//-0.374995, 0.249996, 0.000000
	ChaMatrixIdentity( &m_cen2parY );
	m_cen2parY.data[MATI_41] = 0.0f;
	//m_cen2parY._42 = -m_boneleng * 0.5f;
	m_cen2parY.data[MATI_42] = 0.0f;
	m_cen2parY.data[MATI_43] = 0.0f;
//	m_cen2parY._41 = 0.0f - -0.374995f;
//	m_cen2parY._42 = -m_boneleng * 0.5f - 0.249996f;
//	m_cen2parY._43 = 0.0f;


	ChaVector3 partocen = centerA - aftparentposA;
	ChaMatrixIdentity( &m_par2cen );
	m_par2cen.data[MATI_41] = partocen.x;
	m_par2cen.data[MATI_42] = partocen.y;
	m_par2cen.data[MATI_43] = partocen.z;

	m_transmat = startrot;
	m_transmat.data[MATI_41] = centerA.x;
	m_transmat.data[MATI_42] = centerA.y;
	m_transmat.data[MATI_43] = centerA.z;

//m_transmat = curre->m_capsulemat;


	m_rigidbody = localCreateRigidBody( curre, transform, m_colshape );
	_ASSERT( m_rigidbody );
	if (!m_rigidbody){
		_ASSERT(0);
		return 1;
	}

	btTransform worldtra;
	m_rigidbody->getMotionState()->getWorldTransform( worldtra );
	btMatrix3x3 worldmat = worldtra.getBasis();
	btVector3 worldpos = worldtra.getOrigin();
	
	m_firstTransform = worldtra;
	m_firstTransformMat = worldmat;//bto->GetRigidBody()のCreateBtObject時のWorldTransform->getBasis
	SetFirstTransformMatX(ChaMatrixFromBtTransform(&worldtra.getBasis(), &worldtra.getOrigin()));

	btVector3 tmpcol[3];
	int colno;
	for( colno = 0; colno < 3; colno++ ){
		tmpcol[colno] = worldmat.getColumn( colno );
//		tmpcol[colno] = worldmat.getRow( colno );
	}

	ChaMatrixIdentity( &m_xworld );

	m_xworld.data[MATI_11] = tmpcol[0].x();
	m_xworld.data[MATI_12] = tmpcol[0].y();
	m_xworld.data[MATI_13] = tmpcol[0].z();

	m_xworld.data[MATI_21] = tmpcol[1].x();
	m_xworld.data[MATI_22] = tmpcol[1].y();
	m_xworld.data[MATI_23] = tmpcol[1].z();

	m_xworld.data[MATI_31] = tmpcol[2].x();
	m_xworld.data[MATI_32] = tmpcol[2].y();
	m_xworld.data[MATI_33] = tmpcol[2].z();

	m_xworld.data[MATI_41] = worldpos.x();
	m_xworld.data[MATI_42] = worldpos.y();
	m_xworld.data[MATI_43] = worldpos.z();


	return 0;
}




/*
int CBtObject::CalcConstraintTransform( int chilflag, CRigidElem* curre, CBtObject* curbto, btTransform& dsttra )
{
	dsttra.setIdentity();

	if (!m_rigidbody){
		return 1;
	}

	ChaVector3 parentposA, childposA, aftparentposA, aftchildposA;
	parentposA = curbto->m_bone->GetJointFPos();
	ChaVector3TransformCoord( &aftparentposA, &parentposA, &curbto->m_bone->GetStartMat2() );
	childposA = curbto->m_endbone->GetJointFPos();
	ChaVector3TransformCoord( &aftchildposA, &childposA, &curbto->m_endbone->GetStartMat2() );

	ChaVector2 dirxy, ndirxy;
	dirxy.x = aftchildposA.x - aftparentposA.x;
	dirxy.y = aftchildposA.y - aftparentposA.y;
	float lengxy = D3DXVec2Length( &dirxy );
	D3DXVec2Normalize( &ndirxy, &dirxy );


	ChaVector2 basex( 1.0f, 0.0f );
	float dotx;
	dotx = D3DXVec2Dot( &basex, &ndirxy );
	if( dotx > 1.0f ){
		dotx = 1.0f;
	}
	if( dotx < -1.0f ){
		dotx = -1.0f;
	}

	float calcacos = (float)acos( dotx );
	if( fabs( calcacos ) <= 0.1f * (float)DEG2PAI ){
		m_constzrad = -90.0f * (float)DEG2PAI;
	}else{
		if( dirxy.y >= 0.0f ){
			m_constzrad = -calcacos;
		}else{
			m_constzrad = calcacos;
		}
	}

	if( lengxy < 0.2f ){
		//_ASSERT( 0 );
		m_constzrad = -90.0f * (float)DEG2PAI;
	}


	dsttra.getBasis().setEulerZYX(0.0f, 0.0f, m_constzrad);
	//dsttra.getBasis().setEulerZYX(m_constzrad, 0.0f, 0.0f);
	//dsttra.getBasis().setEulerZYX(0.0, 0.0f, 0.0f);

	btTransform rigidtra = curbto->m_rigidbody->getWorldTransform();
	btTransform invtra = rigidtra.inverse();
	//btVector3 localpivot;
	if( chilflag == 0 ){
		m_curpivot = invtra( btVector3( aftchildposA.x, aftchildposA.y, aftchildposA.z ) );
		//m_curpivot = btVector3( 0.0f, 0.5f * curbto->m_boneleng, 0.0f );
	}else{
		m_curpivot = invtra( btVector3( aftparentposA.x, aftparentposA.y, aftparentposA.z ) );
		//m_curpivot = btVector3( 0.0f, -0.5f * curbto->m_boneleng, 0.0f );
	}
	dsttra.setOrigin( m_curpivot );

	return 0;
}
*/

//int setstartflag = 1;
int CBtObject::CalcConstraintTransform(int chilflag, CRigidElem* curre, CBtObject* curbto, btTransform& dsttra, int setstartflag)
{
	dsttra.setIdentity();

	if (!m_rigidbody){
		return 1;
	}

	ChaMatrix transmatx;
	ChaMatrixIdentity(&transmatx);
	//int setstartflag = 1;

	curbto->m_bone->CalcAxisMatX_RigidBody(0, curbto->m_endbone, &transmatx, setstartflag);

	CQuaternion rotq;
	rotq.RotationMatrix(transmatx);
	CQuaternion invrotq;
	rotq.inv(&invrotq);

	//ChaVector3 befeul = ChaVector3(0.0f, 0.0f, 0.0f);
	//ChaVector3 eul = ChaVector3(0.0f, 0.0f, 0.0f);
	//invrotq.Q2EulXYZ(0, befeul, &eul);
	////rotq.Q2EulZYX(0, 0, befeul, &eul);
	//dsttra.getBasis().setEulerZYX(eul.x * PAI / 180.0, eul.y * PAI / 180.0, eul.z * PAI / 180.0);
	////dsttra.getBasis().setRotation(btQuaternion(rotq.x, rotq.y, rotq.z, rotq.w));
	

	dsttra.setRotation(btQuaternion(invrotq.x, invrotq.y, invrotq.z, invrotq.w));


	btTransform rigidtra = curbto->m_rigidbody->getWorldTransform();
	btTransform invtra = rigidtra.inverse();

	ChaVector3 parentposA, childposA, aftparentposA, aftchildposA;
	parentposA = curbto->m_bone->GetJointFPos();
	childposA = curbto->m_endbone->GetJointFPos();
	if (setstartflag == 1) {
		ChaMatrix tmpzerofm = curbto->m_bone->GetCurrentZeroFrameMat(0);
		ChaVector3TransformCoord(&aftparentposA, &parentposA, &tmpzerofm);
		//ChaVector3TransformCoord(&aftchildposA, &childposA, &curbto->m_endbone->GetCurrentZeroFrameMat(0));
		ChaVector3TransformCoord(&aftchildposA, &childposA, &tmpzerofm);
	}
	else {
		if (g_previewFlag != 5) {
			ChaMatrix tmpwm = curbto->m_bone->GetCurMp().GetWorldMat();
			ChaVector3TransformCoord(&aftparentposA, &parentposA, &tmpwm);
			//ChaVector3TransformCoord(&aftchildposA, &childposA, &curbto->m_endbone->GetCurMp().GetWorldMat());
			ChaVector3TransformCoord(&aftchildposA, &childposA, &tmpwm);
		}
		else {
			ChaMatrix tmpbtmat = curbto->m_bone->GetBtMat();
			ChaVector3TransformCoord(&aftparentposA, &parentposA, &tmpbtmat);
			//ChaVector3TransformCoord(&aftchildposA, &childposA, &curbto->m_endbone->GetBtMat());
			ChaVector3TransformCoord(&aftchildposA, &childposA, &tmpbtmat);
		}
	}
	if (chilflag == 0){
		m_curpivot = invtra(btVector3(aftchildposA.x, aftchildposA.y, aftchildposA.z));
		//m_curpivot = btVector3( 0.0f, 0.5f * curbto->m_boneleng, 0.0f );
	}
	else{
		m_curpivot = invtra(btVector3(aftparentposA.x, aftparentposA.y, aftparentposA.z));
		//m_curpivot = btVector3( 0.0f, -0.5f * curbto->m_boneleng, 0.0f );
	}


	dsttra.setOrigin(m_curpivot);

	return 0;
}


int CBtObject::CreateBtConstraint()
{
	if( m_topflag == 1 ){
		return 0;
	}
	_ASSERT( m_btWorld );
	_ASSERT( m_bone );

	if( !m_endbone ){
		return 1;
	}
	if (!m_rigidbody){
		return 1;
	}

	int chilno;
	for (chilno = 0; chilno < (int)m_chilbt.size(); chilno++){
		CBtObject* chilbto = m_chilbt[ chilno ];
		if( !chilbto ){
			continue;
		}

		m_FrameA.setIdentity();
		m_FrameB.setIdentity();

		CRigidElem* tmpre;
		tmpre = m_bone->GetRigidElem( m_endbone );
		_ASSERT( tmpre );
		CalcConstraintTransform( 0, tmpre, this, m_FrameA );
		tmpre = chilbto->m_bone->GetRigidElem( chilbto->m_endbone );
		_ASSERT( tmpre );
		CalcConstraintTransform( 1, tmpre, chilbto, m_FrameB );
		//CalcConstraintTransform(1, tmpre, chilbto, m_FrameB);

		if( m_rigidbody && chilbto->m_rigidbody ){

DbgOut( L"CreateBtConstraint (bef) : curbto %s---%s, chilbto %s---%s\r\n", 
	   m_bone->GetWBoneName(), m_endbone->GetWBoneName(),
	   chilbto->m_bone->GetWBoneName(), chilbto->m_endbone->GetWBoneName() );

			float angPAI2, angPAI;
			angPAI2 = 90.0f * (float)DEG2PAI;
			angPAI = 180.0f * (float)DEG2PAI;

			float lmax, lmin;
			lmin = -10000.0f;
			lmax = 10000.0f;

			btGeneric6DofSpringConstraint* dofC = 0;
			//dofC = new btConeTwistConstraint( *m_rigidbody, *(chilbto->m_rigidbody), m_FrameA, m_FrameB, true );
			dofC = new btGeneric6DofSpringConstraint(*m_rigidbody, *(chilbto->m_rigidbody), m_FrameA, m_FrameB, true);
			_ASSERT( dofC );
			if (dofC){
				btTransform worldtra;
				m_rigidbody->getMotionState()->getWorldTransform(worldtra);
				btMatrix3x3 worldmat = worldtra.getBasis();
				btVector3 worldpos = worldtra.getOrigin();

				//##dofC->setLinearLowerLimit(btVector3(worldpos.x() + lmin, worldpos.y() + lmin, worldpos.z() + lmin));
				//##dofC->setLinearUpperLimit(btVector3(worldpos.x() + lmax, worldpos.y() + lmax, worldpos.z() + lmax));

				//dofC->setBreakingImpulseThreshold(FLT_MAX);
				//dofC->setBreakingImpulseThreshold( 1e9 );
				//dofC->setBreakingImpulseThreshold(0.0);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//dofC->setBreakingImpulseThreshold(1e7);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

				int l_kindex = chilbto->m_bone->GetRigidElem(chilbto->m_endbone)->GetLKindex();
				int a_kindex = chilbto->m_bone->GetRigidElem(chilbto->m_endbone)->GetAKindex();
				float l_damping = chilbto->m_bone->GetRigidElem(chilbto->m_endbone)->GetLDamping();
				float a_damping = chilbto->m_bone->GetRigidElem(chilbto->m_endbone)->GetADamping();
				float l_cusk = chilbto->m_bone->GetRigidElem(chilbto->m_endbone)->GetCusLk();
				float a_cusk = chilbto->m_bone->GetRigidElem(chilbto->m_endbone)->GetCusAk();
				int forbidrotflag = chilbto->m_bone->GetRigidElem(chilbto->m_endbone)->GetForbidRotFlag();

				ANGLELIMIT anglelimit;
				anglelimit = chilbto->m_bone->GetAngleLimit(0);


				int dofid;
				/*
				for (dofid = 0; dofid < 3; dofid++){
					dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
				}
				for (dofid = 3; dofid < 6; dofid++){
					dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
				}
				*/
				for (dofid = 0; dofid < 3; dofid++) {
					if (l_kindex <= 2) {
						dofC->setStiffness(dofid, g_l_kval[l_kindex]);
					}
					else {
						dofC->setStiffness(dofid, l_cusk);
					}
					//dofC->setStiffness(dofid, 1.0e12);
					if (g_previewFlag != 5){
						dofC->setDamping(dofid, l_damping);
					}
					else {
						dofC->setDamping(dofid, 0.7);
					}
					//dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
					dofC->enableSpring(dofid, true);

					//dofC->setEquilibriumPoint(dofid);
				}
				for (dofid = 3; dofid < 6; dofid++){
					if (a_kindex <= 2){
						dofC->setStiffness(dofid, g_a_kval[a_kindex]);
					}
					else{
						dofC->setStiffness(dofid, a_cusk);
					}
					if (g_previewFlag != 5) {
						dofC->setDamping(dofid, a_damping);
					}
					else {
						dofC->setDamping(dofid, 0.7);
					}
					//dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!
					dofC->enableSpring(dofid, true);

					//dofC->setEquilibriumPoint(dofid);
				}				

				//dofC->setDamping(a_damping);

				//dofC->setDamping(1.0);


				CONSTRAINTELEM addelem;
				addelem.constraint = dofC;
				addelem.centerbone = m_endbone;
				addelem.childbto = chilbto;
				m_constraint.push_back(addelem);

				DbgOut(L"CreateBtConstraint (aft) : curbto %s---%s, chilbto %s---%s\r\n",
					m_bone->GetWBoneName(), m_endbone->GetWBoneName(),
					chilbto->m_bone->GetWBoneName(), chilbto->m_endbone->GetWBoneName());


				//m_btWorld->addConstraint(dofC, false);
				m_btWorld->addConstraint(dofC, true);
				//m_btWorld->addConstraint((btTypedConstraint*)dofC, false);//!!!!!!!!!!!! disable collision between linked bodies
				//m_btWorld->addConstraint((btTypedConstraint*)dofC, true);//!!!!!!!!!!!! disable collision between linked bodies
				//m_dofC = dofC;

				//dofC->setEquilibriumPoint();//!!!!!!!!!!!!tmp disable


				//以下、setAxisコメントアウト
				//2つの軸はZとY（XはそれらのCross）
				//以下の設定をすると不自然にだらーんと伸びてバネで奇妙に動く。謎。
				//CRigidElem* tmpre2 = chilbto->m_bone->GetRigidElem(chilbto->m_endbone);
				//ChaMatrix bmat = tmpre2->GetBindcapsulemat();
				//btVector3 axisZ, axisY;
				//axisZ = btVector3(bmat._31, bmat._32, bmat._33);
				//axisY = btVector3(bmat._21, bmat._22, bmat._23);
				////axisZ = btVector3(bmat._13, bmat._23, bmat._33);
				////axisY = btVector3(bmat._12, bmat._22, bmat._32);
				//dofC->setAxis(axisZ, axisY);

					int dofcindex;
					for (dofcindex = 0; dofcindex < 6; dofcindex++) {
						//0-2:linear, 3-5:angular
						//dofC->setParam(BT_CONSTRAINT_STOP_CFM, 0, dofcindex);//CFM 0 壊れにくい
						if (g_previewFlag != 5) {
							dofC->setParam(BT_CONSTRAINT_STOP_CFM, btScalar(0), dofcindex);//CFM 0 壊れにくい
							dofC->setParam(BT_CONSTRAINT_STOP_ERP, btScalar(g_erp), dofcindex);//ERP(0-1) 値大 --> エラー補正大
						}
						else {
							dofC->setParam(BT_CONSTRAINT_STOP_CFM, btScalar(0.5), dofcindex);//CFM 0 壊れにくい
							dofC->setParam(BT_CONSTRAINT_STOP_ERP, btScalar(0.0), dofcindex);//ERP(0-1) 値大 --> エラー補正大

							//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.0080, dofcindex);//ERP(0-1) 値大 --> エラー補正大
							//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.010, dofcindex);//ERP(0-1) 値大 --> エラー補正大
							//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.10, dofcindex);//ERP(0-1) 値大 --> エラー補正大
							//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.040, dofcindex);//ERP(0-1) 値大 --> エラー補正大
							//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.070, dofcindex);//ERP(0-1) 値大 --> エラー補正大

							//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.00020, dofcindex);//ERP(0-1) 値大 --> エラー補正大
						}
					}

				dofC->setEquilibriumPoint();


				if (forbidrotflag == 0) {
					dofC->setAngularLowerLimit(btVector3(angPAI, angPAI2, angPAI));
					dofC->setAngularUpperLimit(btVector3(-angPAI, -angPAI2, -angPAI));
				}
				else {
					//dofC->setAngularLowerLimit(btVector3(0.0, 0.0, 0.0));
					//dofC->setAngularUpperLimit(btVector3(0.0, 0.0, 0.0));
					dofC->calculateTransforms();
					btScalar currentx = dofC->getAngle(0);
					btScalar currenty = dofC->getAngle(1);
					btScalar currentz = dofC->getAngle(2);
					dofC->setAngularLowerLimit(btVector3(btScalar(currentx - 0.5 * (float)DEG2PAI), btScalar(currenty - 0.5 * (float)DEG2PAI), btScalar(currentz - 0.5 * (float)DEG2PAI)));
					dofC->setAngularUpperLimit(btVector3(btScalar(currentx + 0.5 * (float)DEG2PAI), btScalar(currenty + 0.5 * (float)DEG2PAI), btScalar(currentz + 0.5 * (float)DEG2PAI)));

				}

			}
		}
	}

	return 0;
}

int CBtObject::EnableSpring(bool angleflag, bool linearflag)
{
	
	size_t constraintnum = m_constraint.size();
	size_t constno;
	for (constno = 0; constno < constraintnum; constno++){
		btGeneric6DofSpringConstraint* curconst = m_constraint[constno].constraint;
		if (curconst){
			int dofid;
			for (dofid = 0; dofid < 3; dofid++){
				curconst->enableSpring(dofid, linearflag);
			}
			for (dofid = 3; dofid < 6; dofid++){
				curconst->enableSpring(dofid, angleflag);
			}
		}
	}
	
	return 0;
}

int CBtObject::SetDofRotAxis(int srcaxiskind)
{


	/*
	float angPAI4, angPAI2, angPAI, ang5;
	angPAI4 = 45.0f * (float)DEG2PAI;
	angPAI2 = 90.0f * (float)DEG2PAI;
	angPAI = 180.0f * (float)DEG2PAI;
	ang5 = 5.0f * (float)DEG2PAI;

	int constraintnum = m_constraint.size();
	int constno;
	for (constno = 0; constno < constraintnum; constno++){
		btConeTwistConstraint* dofC = m_constraint[constno].constraint;
		if (dofC){
			if (srcaxiskind == PICK_CENTER){
				dofC->setLimit(angPAI, angPAI, angPAI2);
			}
			else if (srcaxiskind == PICK_X){
				dofC->setLimit(angPAI, 0.0, 0.0);
			}
			else if (srcaxiskind == PICK_Y){
				dofC->setLimit(0.0, 0.0, angPAI2);
			}
			else if (srcaxiskind == PICK_Z){
				dofC->setLimit(0.0, angPAI, 0.0);

			}
		}
	}

	//btConeTwistConstraint* gzdofC = m_gz_constraint.constraint;
	//if (gzdofC){
		//if (srcaxiskind == PICK_CENTER){
		//gzdofC->setLimit(angPAI, angPAI, angPAI);
		//}
		//else if (srcaxiskind == PICK_X){
		//	gzdofC->setLimit(angPAI, 0.0, 0.0);
		//}
		//else if (srcaxiskind == PICK_Y){
		//	gzdofC->setLimit(0.0, 0.0, angPAI2);
		//}
		//else if (srcaxiskind == PICK_Z){
		//	gzdofC->setLimit(0.0, angPAI, 0.0);
		//}
	//}
	*/

	return 0;
}


int CBtObject::SetEquilibriumPoint(int lflag, int aflag)
{

	size_t constraintnum = m_constraint.size();
	size_t constno;
	//if (g_previewFlag != 5) {
		for (constno = 0; constno < constraintnum; constno++) {
			btGeneric6DofSpringConstraint* dofC = m_constraint[constno].constraint;
			dofC->setEquilibriumPoint();
		}
	//}

	/*
	float angPAI = 180.0f * (float)DEG2PAI;
	float angPAI2 = 90.0f * (float)DEG2PAI;
	float angPAI4 = 45.0f * (float)DEG2PAI;
	float ang5 = 5.0f * (float)DEG2PAI;

	int constraintnum = m_constraint.size();
	int constno;
	for (constno = 0; constno < constraintnum; constno++){
		btConeTwistConstraint* dofC = m_constraint[constno].constraint;
		if (dofC){
			dofC->setLimit(angPAI, angPAI, angPAI);
		}
	}
	*/


	float angPAI4, angPAI2, angPAI, ang5;
	angPAI4 = 45.0f * (float)DEG2PAI;
	angPAI2 = 90.0f * (float)DEG2PAI;
	angPAI = 180.0f * (float)DEG2PAI;
	ang5 = 5.0f * (float)DEG2PAI;

	////int constraintnum = m_constraint.size();
	////int constno;
	//for (constno = 0; constno < constraintnum; constno++){
	//	btGeneric6DofSpringConstraint* dofC = m_constraint[constno].constraint;
	//	if (dofC){
	//		//dofC->setLimit(angPAI, angPAI, angPAI, 0.8, 0.0, 0.5);
	//		//dofC->setLimit(angPAI, angPAI, angPAI2);
	//		//dofC->setLimit(angPAI2, angPAI2, angPAI2);
	//		//dofC->setLimit(angPAI2, angPAI2, angPAI4, 0.8, 0.0, 0.5);
	//		//dofC->setLimit(angPAI2, angPAI2, angPAI4, 0.8, 0.3, 0.8);
	//		//dofC->setLimit(angPAI2, angPAI2, angPAI4);
	//		if (g_previewFlag != 5){
	//			dofC->setLimit(angPAI4, angPAI4, angPAI4);
	//		}
	//		else{
	//			dofC->setLimit(angPAI, angPAI, angPAI);
	//		}
	//	}
	//}

	for (constno = 0; constno < constraintnum; constno++){
		btGeneric6DofSpringConstraint* dofC = m_constraint[constno].constraint;
		CBtObject* childbto = m_constraint[constno].childbto;
		if (childbto && childbto->m_bone){

			ANGLELIMIT anglelimit;
			ZeroMemory(&anglelimit, sizeof(ANGLELIMIT));
			anglelimit = childbto->m_bone->GetAngleLimit(0);
			//ANGLELIMIT anglelimit = childbto->m_bone->GetAngleLimit();

			int forbidrotflag = childbto->m_bone->GetRigidElem(childbto->m_endbone)->GetForbidRotFlag();

			//if (forbidrotflag == 0){
			//	dofC->setAngularLowerLimit(btVector3(angPAI, angPAI2, angPAI));
			//	dofC->setAngularUpperLimit(btVector3(-angPAI, -angPAI2, -angPAI));
			//}
			//else{
			//	dofC->setAngularLowerLimit(btVector3(0.0, 0.0, 0.0));
			//	dofC->setAngularUpperLimit(btVector3(0.0, 0.0, 0.0));
			//}

			if ((forbidrotflag == 0) || (g_limitdegflag != 0)){
				//XYZ
				//dofC->setAngularLowerLimit(btVector3(anglelimit.lower[0] * PAI / 180.0f, anglelimit.lower[1] * PAI / 180.0f, anglelimit.lower[2] * PAI / 180.0f));
				//dofC->setAngularUpperLimit(btVector3(anglelimit.upper[0] * PAI / 180.0f, anglelimit.upper[1] * PAI / 180.0f, anglelimit.upper[2] * PAI / 180.0f));

				ChaMatrix eulaxismat;
				CQuaternion eulaxisq;
				int multworld = 0;//local!!!
				CRigidElem* curre = childbto->m_bone->GetRigidElem(childbto->m_endbone);
				if (curre) {
					eulaxismat = curre->GetBindcapsulemat();
				}
				else {
					_ASSERT(0);
					ChaMatrixIdentity(&eulaxismat);
				}
				eulaxisq.RotationMatrix(eulaxismat);


				ChaVector3 lowereul, uppereul;
				lowereul = ChaVector3(btScalar(anglelimit.lower[0]), btScalar(anglelimit.lower[1]), btScalar(anglelimit.lower[2]));
				uppereul = ChaVector3(btScalar(anglelimit.upper[0]), btScalar(anglelimit.upper[1]), btScalar(anglelimit.upper[2]));

				CQuaternion lowereulq;
				lowereulq.SetRotationXYZ(&eulaxisq, lowereul);
				CQuaternion uppereulq;
				uppereulq.SetRotationXYZ(&eulaxisq, uppereul);

				btTransform lowereultra;
				btTransform uppereultra;
				lowereultra.setIdentity();
				uppereultra.setIdentity();
				btQuaternion lowerbteulq(lowereulq.x, lowereulq.y, lowereulq.z, lowereulq.w);
				btQuaternion upperbteulq(uppereulq.x, uppereulq.y, uppereulq.z, uppereulq.w);
				lowereultra.setRotation(lowerbteulq);
				uppereultra.setRotation(upperbteulq);
				btScalar lowereulz, lowereuly, lowereulx;
				btScalar uppereulz, uppereuly, uppereulx;
				lowereultra.getBasis().getEulerZYX(lowereulz, lowereuly, lowereulx, 1);//関数名とは裏腹に回転順序としてはXYZ
				uppereultra.getBasis().getEulerZYX(uppereulz, uppereuly, uppereulx, 1);//関数名とは裏腹に回転順序としてはXYZ


				btScalar startx, endx, starty, endy, startz, endz;
				if (lowereulx <= uppereulx) {
					startx = lowereulx;
					endx = uppereulx;
				}
				else {
					startx = uppereulx;
					endx = lowereulx;
				}
				if (lowereuly <= uppereuly) {
					starty = lowereuly;
					endy = uppereuly;
				}
				else {
					starty = uppereuly;
					endy = lowereuly;
				}
				if (lowereulz <= uppereulz) {
					startz = lowereulz;
					endz = uppereulz;
				}
				else {
					startz = uppereulz;
					endz = lowereulz;
				}

				dofC->setAngularLowerLimit(btVector3(startx, starty, startz));
				dofC->setAngularUpperLimit(btVector3(endx, endy, endz));
			}
			else{
				dofC->calculateTransforms();
				btScalar currentx = dofC->getAngle(0);
				btScalar currenty = dofC->getAngle(1);
				btScalar currentz = dofC->getAngle(2);
				dofC->setAngularLowerLimit(btVector3(btScalar(currentx - 1.0 * (float)DEG2PAI), btScalar(currenty - 1.0 * (float)DEG2PAI), btScalar(currentz - 1.0 * (float)DEG2PAI)));
				dofC->setAngularUpperLimit(btVector3(btScalar(currentx + 1.0 * (float)DEG2PAI), btScalar(currenty + 1.0 * (float)DEG2PAI), btScalar(currentz + 1.0 * (float)DEG2PAI)));

				//dofC->setAngularLowerLimit(btVector3(0.0, 0.0, 0.0));
				//dofC->setAngularUpperLimit(btVector3(0.0, 0.0, 0.0));
			}
		}
	}







	return 0;
}

int CBtObject::Motion2Bt(CModel* srcmodel)
{
	if( m_topflag == 1 ){
		return 0;
	}
	if( !m_bone ){
		return 0;
	}
	if( !m_rigidbody ){
		return 0;
	}

	if( !m_rigidbody->getMotionState() ){
		_ASSERT( 0 );
		return 0;
	}
	if (!GetBone() || !GetEndBone()){
		return 0;
	}


	CRigidElem* curre = m_bone->GetRigidElem( m_endbone );
	if( curre ){
		ChaMatrix newrotmat;
		ChaVector3 newrigidpos;
		GetBone()->CalcNewBtMat(srcmodel, curre, GetEndBone(), &newrotmat, &newrigidpos);

		SetPosture2Bt(newrotmat, newrigidpos);

	}else{
		_ASSERT( 0 );
	}

	return 0;
}

//int constraintupdateflag = 1
int CBtObject::SetPosture2Bt(ChaMatrix srcmat, ChaVector3 srcrigidcenter, int constraintupdateflag)
{
	CQuaternion tmpq;
	tmpq.RotationMatrix(srcmat);
	btQuaternion btrotq(tmpq.x, tmpq.y, tmpq.z, tmpq.w);

	btTransform worldtra;
	worldtra.setIdentity();
	worldtra.setRotation(btrotq);
	worldtra.setOrigin(btVector3(srcrigidcenter.x, srcrigidcenter.y, srcrigidcenter.z));

	m_rigidbody->getMotionState()->setWorldTransform(worldtra);

	m_btpos = ChaVector3(srcrigidcenter.x, srcrigidcenter.y, srcrigidcenter.z);

	//constraintのFrameA, FrameBの更新
	if (constraintupdateflag == 1) {
		RecalcConstraintFrameAB();
	}
	return 0;

}

void CBtObject::RecalcConstraintFrameAB()
{
	if (g_previewFlag == 5) {
		for (int i = 0; i < GetConstraintSize(); i++) {
			CONSTRAINTELEM curce = GetConstraintElem(i);
			btGeneric6DofSpringConstraint* dofC = curce.constraint;
			CBtObject* childbto = curce.childbto;
			if (dofC && childbto) {
				btQuaternion rotA;
				btTransform FrameA;
				btTransform FrameB;
				FrameA.setIdentity();
				FrameB.setIdentity();

				CalcConstraintTransformA(FrameA, rotA);
				CalcConstraintTransformB(childbto, rotA, FrameB);

				if (m_rigidbody && childbto->m_rigidbody) {
					dofC->setFrames(FrameA, FrameB);
					dofC->setEquilibriumPoint();
					dofC->calculateTransforms();
				}

				//dofC->setEquilibriumPoint();
			}
		}
	}
}

int CBtObject::SetBtMotion()
{
	if( m_topflag == 1 ){
		return 0;
	}
	if( !m_rigidbody ){
		return 0;
	}
	if( !m_rigidbody->getMotionState() ){
		_ASSERT( 0 );
		return 0;
	}

	btTransform worldtra;
	m_rigidbody->getMotionState()->getWorldTransform( worldtra );
	ChaMatrix newxworld;
	newxworld = ChaMatrixFromBtTransform(&(worldtra.getBasis()), &(worldtra.getOrigin()));


	btTransform parentworldtra;
	ChaMatrix parentnewxworld;
	if (GetParBt() && GetParBt()->GetRigidBody()) {
		GetParBt()->GetRigidBody()->getMotionState()->getWorldTransform(parentworldtra);
		parentnewxworld = ChaMatrixFromBtTransform(&(parentworldtra.getBasis()), &(parentworldtra.getOrigin()));
	}
	else {
		parentworldtra.setIdentity();
		parentnewxworld.SetIdentity();
	}

	ChaMatrix newxlocal;
	newxlocal = newxworld * ChaMatrixInv(parentnewxworld);

	//ChaVector3 newcenterpos;
	//newcenterpos = ChaVector3(newxworld.data[MATI_41], newxworld.data[MATI_42], newxworld.data[MATI_43]);
	ChaVector3 newxvec;
	newxvec = ChaVector3(newxworld.data[MATI_11], newxworld.data[MATI_12], newxworld.data[MATI_13]);
	//ChaVector3 newyvec;
	//newyvec = ChaVector3(newxworld.data[MATI_21], newxworld.data[MATI_22], newxworld.data[MATI_23]);
	//ChaVector3 newzvec;
	//newzvec = ChaVector3(newxworld.data[MATI_31], newxworld.data[MATI_32], newxworld.data[MATI_33]);
	float boneleng = GetBoneLeng();
	//ChaVector3 newpivot;
	//newpivot = newcenterpos - newxvec * (boneleng * 0.5f);
	////newpivot = newcenterpos - newyvec * (boneleng * 0.5f);
	////newpivot = newcenterpos - newzvec * (boneleng * 0.5f);

	ChaVector3 aftpivot;
	//aftpivot = -newxvec * (boneleng * 0.5f);
	aftpivot = newxvec * (boneleng * 0.5f);
	ChaMatrix aftpivotmat;
	aftpivotmat.SetIdentity();
	aftpivotmat.SetTranslation(aftpivot);

	ChaVector3 orgpos, orgchildpos, aftpos, aftchildpos;
	ChaMatrix zerowm;
	orgpos = m_bone->GetJointFPos();
	orgchildpos = m_endbone->GetJointFPos();
	zerowm = m_bone->GetCurrentZeroFrameMat(1);
	ChaVector3TransformCoord(&aftpos, &orgpos, &zerowm);
	ChaVector3TransformCoord(&aftchildpos, &orgchildpos, &zerowm);
	ChaMatrix befpivotmat;
	befpivotmat.SetIdentity();
	//befpivotmat.SetTranslation(-aftpos);
	//befpivotmat.SetTranslation(-orgpos);
	//befpivotmat.SetTranslation(-orgchildpos);
	befpivotmat.SetTranslation(-aftchildpos);

	//ChaVector3 centerpos;
	//centerpos = (aftpos + aftchildpos) * 0.5f;
	//ChaVector3 center2parent;
	//center2parent = aftpos - centerpos;
	//ChaMatrix center2parentmat;
	//center2parentmat.SetIdentity();
	//center2parentmat.SetTranslation(ChaVector3(center2parent));

	ChaMatrix setwm;
	ChaMatrix befrot, aftrot, rotmat;
	ChaMatrix parentchamatrix;
	CQuaternion offsetq;
	CQuaternion rotq;
	CQuaternion localq;
	befrot.SetIdentity();
	aftrot.SetIdentity();
	rotmat.SetIdentity();
	parentchamatrix.SetIdentity();
	offsetq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	//befrot.SetTranslation(-newpivot);
	//aftrot.SetTranslation(newpivot);
	//befrot.SetTranslation(-m_bone->GetJointFPos());
	//aftrot.SetTranslation(m_bone->GetJointFPos());
	//befrot.SetTranslation(-m_endbone->GetJointFPos());
	//aftrot.SetTranslation(m_endbone->GetJointFPos());
	//rotmat = ChaMatrixRot(newxworld);
	//rotmat = ChaMatrixRot(newxlocal);

	//offsetq.SetRotationXYZ(0, ChaVector3(0.0f, 90.0f, 0.0f));
	offsetq = m_btq;
	rotq.RotationMatrix(newxlocal);

	//localq = rotq;
	//localq = rotq * offsetq;
	//localq = offsetq * rotq;
	//localq = rotq * offsetq.inverse();
	//localq = offsetq.inverse() * rotq;
	//localq = offsetq.inverse() * rotq * offsetq;
	//localq = offsetq * rotq * offsetq.inverse();

	//if (m_bone && m_bone->GetParent()) {
	//	parentchamatrix = m_bone->GetParent()->GetBtMat();
	//}
	//else {
	//	parentchamatrix.SetIdentity();
	//}
	////parentchamatrix = m_bone->GetBtMat();


	//setwm = befrot * rotmat * aftrot * parentchamatrix;
	//setwm = befrot * ChaMatrixInv(rotmat) * aftrot * parentchamatrix;
	//setwm = befrot * localq.MakeRotMatX() * aftrot * parentchamatrix;


	//if ((m_endbone->GetBtFlag() == 0) && ((m_endbone->GetTmpKinematic() == false) || (m_endbone->GetMass0() == TRUE))) {
	//	////m_bone->SetBtMat(m_bone->GetStartMat2() * diffxworld);
	//	m_endbone->SetBtMat(setwm);
	//	m_endbone->SetBtFlag(1);
	//}


	
	//setwm = center2parentmat * newxworld;
	//setwm = ChaMatrixInv(center2parentmat) * newxworld;


	//setwm = newxworld * aftpivotmat;
	setwm = befpivotmat * newxworld * aftpivotmat;


	if ((m_bone->GetBtFlag() == 0) && ((m_bone->GetTmpKinematic() == false) || (m_bone->GetMass0() == TRUE))) {
		////m_bone->SetBtMat(m_bone->GetStartMat2() * diffxworld);
		m_bone->SetBtMat(setwm);
		m_bone->SetBtFlag(1);
	}

	//ChaMatrix invxworld;
	//ChaMatrixInverse( &invxworld, NULL, &m_xworld );
	////invxworld = GetInvFirstTransformMatX();

	//ChaMatrix diffxworld;
	//diffxworld = invxworld * newxworld;

	//if ((m_bone->GetBtFlag() == 0) && ((m_bone->GetTmpKinematic() == false) || (m_bone->GetMass0() == TRUE))) {
	//	////m_bone->SetBtMat(m_bone->GetStartMat2() * diffxworld);
	//	m_bone->SetBtMat(m_bone->GetCurrentZeroFrameMat(0) * diffxworld);
	//	m_bone->SetBtFlag(1);
	//}

	//boneleng 0 対策はCreateObjectの剛体のサイズを決めるところで最小値を設定することにした。

	return 0;
}

int CBtObject::SetCapsuleBtMotion(CRigidElem* srcre)
{
	if (m_topflag == 1){
		return 0;
	}
	if (!m_rigidbody){
		return 0;
	}
	if (!m_rigidbody->getMotionState()){
		_ASSERT(0);
		return 0;
	}

	btTransform worldtra;
	m_rigidbody->getMotionState()->getWorldTransform(worldtra);
	btMatrix3x3 worldmat = worldtra.getBasis();
	btVector3 worldpos = worldtra.getOrigin();
	btVector3 tmpcol[3];//行列のカラム表現。
	int colno;
	for (colno = 0; colno < 3; colno++){
		tmpcol[colno] = worldmat.getColumn(colno);
		//		tmpcol[colno] = worldmat.getRow( colno );
	}

	ChaMatrix newxworld;
	ChaMatrixIdentity(&newxworld);
	newxworld.data[MATI_11] = tmpcol[0].x();
	newxworld.data[MATI_12] = tmpcol[0].y();
	newxworld.data[MATI_13] = tmpcol[0].z();

	newxworld.data[MATI_21] = tmpcol[1].x();
	newxworld.data[MATI_22] = tmpcol[1].y();
	newxworld.data[MATI_23] = tmpcol[1].z();

	newxworld.data[MATI_31] = tmpcol[2].x();
	newxworld.data[MATI_32] = tmpcol[2].y();
	newxworld.data[MATI_33] = tmpcol[2].z();

	newxworld.data[MATI_41] = worldpos.x();
	newxworld.data[MATI_42] = worldpos.y();
	newxworld.data[MATI_43] = worldpos.z();

	ChaMatrix invxworld;
	ChaMatrixInverse(&invxworld, NULL, &m_xworld);
	//invxworld = m_bone->GetCurrentZeroFrameInvMat(0);

	ChaMatrix diffxworld;
	diffxworld = invxworld * newxworld;

	//CMotionPoint curmp;
	//curmp = m_bone->GetCurMp();
	//curmp.SetBtMat(m_bone->GetStartMat2() * diffxworld);
	//curmp.SetBtFlag(1);
	//m_bone->SetCurMp(curmp);

	//ChaMatrix newcapsulemat;
	//newcapsulemat = srcre->GetBindcapsulemat() * diffxworld;
	//newcapsulemat = m_bone->CalcManipulatorPostureMatrix(0, 1, 1);
	//srcre->SetCapsulemat(newcapsulemat);


	return 0;
}



int CBtObject::CreatePhysicsPosConstraint()
{

	DestroyGZObj();

	if (m_topflag == 1) {
		return 0;
	}
	if (!m_bone) {
		return 1;
	}
	if (!m_endbone) {
		return 1;
	}

	//////////////
	////////////// mass zero rigidbody
	float h, r, z;
	//max : boneleng 0 対策
	if (GetBoneLeng() >= 0.001) {
		r = (float)(GetBoneLeng() * 0.1);
		h = r;
		z = r;
	}
	else {
		r = 0.0001f;
		h = 0.0001f;
		z = 0.0001f;
	}

	//r = 1.0f;
	//h = 1.0f;
	//z = 1.0f;
	//r = 0.00001f;
	//h = 0.00001f;
	//z = 0.00001f;
	//r = GetBoneLeng() * 0.1;
	//h = r;
	//z = r;

	m_gz_colshape = new btSphereShape(btScalar(r));

	ChaMatrix parentmat;
	ChaMatrix childmat;
	if (g_previewFlag == 5) {
		parentmat = m_bone->GetBtMat();
		childmat = m_endbone->GetBtMat();
	}
	else {
		parentmat = m_bone->GetCurMp().GetWorldMat();
		childmat = m_endbone->GetCurMp().GetWorldMat();
	}

	ChaVector3 parentposA, childposA, aftparentposA, aftchildposA;
	parentposA = m_bone->GetJointFPos();
	ChaVector3TransformCoord(&aftparentposA, &parentposA, &parentmat);
	childposA = m_endbone->GetJointFPos();
	ChaVector3TransformCoord(&aftchildposA, &childposA, &childmat);
	//ChaVector3 centerA = (aftparentposA + aftchildposA) * 0.5f;
	ChaVector3 centerA = aftchildposA;
	//ChaVector3 centerA = aftparentposA;
	btVector3 btv(btScalar(centerA.x), btScalar(centerA.y), btScalar(centerA.z));
	btTransform rigidtransform;
	rigidtransform.setIdentity();
	rigidtransform.setOrigin(btv);

	//btVector3 localInteria = -btv;
	//m_gz_rigidbody = localCreateRigidBody(0, rigidtransform, m_gz_colshape, localInteria);
	m_gz_rigidbody = localCreateRigidBody(0, rigidtransform, m_gz_colshape);
	_ASSERT(m_gz_rigidbody);
	if (!m_gz_rigidbody) {
		_ASSERT(0);
		return 1;
	}
	m_gz_rigidbody->setDamping(0.7, 0.7);

	CreatePhysicsPosConstraintCurrent();

	for (int i = 0; i < GetChildBtSize(); i++) {
		CBtObject* childbto = GetChildBt(i);
		if (childbto) {
			CreatePhysicsPosConstraintChild(childbto);
		}
	}


	return 0;
}

int CBtObject::CalcConstraintTransformA(btTransform& dsttraA, btQuaternion& rotA)
{
	//FrameAは剛体Aの座標系におけるコンストレイントの姿勢

	if (m_topflag == 1) {
		return 0;
	}
	if (!m_endbone) {
		return 1;
	}

	ChaMatrix parentmat;
	ChaMatrix childmat;
	if (g_previewFlag == 5) {
		parentmat = m_bone->GetBtMat();
		childmat = m_endbone->GetBtMat();
	}
	else {
		parentmat = m_bone->GetCurMp().GetWorldMat();
		childmat = m_endbone->GetCurMp().GetWorldMat();
	}

	ChaVector3 parentposA, childposA, aftparentposA, aftchildposA;
	parentposA = m_bone->GetJointFPos();
	ChaVector3TransformCoord(&aftparentposA, &parentposA, &parentmat);
	childposA = m_endbone->GetJointFPos();
	ChaVector3TransformCoord(&aftchildposA, &childposA, &childmat);
	ChaVector3 centerA = (aftparentposA + aftchildposA) * 0.5f;

	//////////////////
	////////////////// constraint
	_ASSERT(m_btWorld);
	_ASSERT(m_bone);

	dsttraA.setIdentity();

	btTransform rigidtraA = m_rigidbody->getWorldTransform();
	btTransform invtraA = rigidtraA.inverse();
	btVector3 originA = m_rigidbody->getWorldTransform().getOrigin();

	btQuaternion btrotqA;
	btrotqA = rigidtraA.getRotation();
	btQuaternion invbtrotqA;
	invbtrotqA = btrotqA.inverse();
	dsttraA.setRotation(invbtrotqA);

	ChaMatrix transmatx;
	ChaMatrixIdentity(&transmatx);
	m_bone->CalcAxisMatX_RigidBody(0, m_endbone, &transmatx, 0);
	CQuaternion rotq;
	rotq.RotationMatrix(transmatx);
	CQuaternion invrotq;
	rotq.inv(&invrotq);
	btQuaternion btrotq;
	btrotq = btQuaternion(rotq.x, rotq.y, rotq.z, rotq.w);
	btQuaternion invbtrotq;
	invbtrotq = btQuaternion(invrotq.x, invrotq.y, invrotq.z, invrotq.w);

	//btQuaternion setbtqA;
	//setbtqA = invbtrotqA * btrotq * btrotqA;
	////setbtqA = invbtrotqA * invbtrotq * btrotqA;
	////setbtqA = btrotqA * invbtrotq * invbtrotqA;
	//dsttraA.setRotation(setbtqA);
	//rotA = setbtqA;

	btVector3 pivotA;
	pivotA = invtraA(btVector3(aftchildposA.x, aftchildposA.y, aftchildposA.z));
	dsttraA.setOrigin(pivotA);

	return 0;

}
int CBtObject::CalcConstraintTransformB(CBtObject* childbto, btQuaternion rotA, btTransform& dsttraB)
{
	//FrameBは剛体Bの座標系におけるコンストレイントの姿勢

	if (m_topflag == 1) {
		return 0;
	}
	if (!childbto) {
		return 1;
	}
	if (!childbto->m_bone) {
		return 1;
	}
	if (!childbto->m_endbone) {
		return 1;
	}


	ChaMatrix parentmat;
	ChaMatrix childmat;
	if (g_previewFlag == 5) {
		parentmat = childbto->m_bone->GetBtMat();
		childmat = childbto->m_endbone->GetBtMat();
	}
	else {
		parentmat = childbto->m_bone->GetCurMp().GetWorldMat();
		childmat = childbto->m_endbone->GetCurMp().GetWorldMat();
	}

	ChaVector3 parentposB, childposB, aftparentposB, aftchildposB;
	parentposB = childbto->m_bone->GetJointFPos();
	ChaVector3TransformCoord(&aftparentposB, &parentposB, &parentmat);
	childposB = childbto->m_endbone->GetJointFPos();
	ChaVector3TransformCoord(&aftchildposB, &childposB, &childmat);
	ChaVector3 centerB = (aftparentposB + aftchildposB) * 0.5f;

	//////////////////
	////////////////// constraint
	_ASSERT(m_btWorld);
	_ASSERT(m_bone);

	dsttraB.setIdentity();

	btTransform rigidtraB = childbto->m_rigidbody->getWorldTransform();
	btTransform invtraB = rigidtraB.inverse();

	btQuaternion btrotqB;
	btrotqB = rigidtraB.getRotation();
	btQuaternion invbtrotqB;
	invbtrotqB = btrotqB.inverse();

	dsttraB.setRotation(invbtrotqB);


	//ChaMatrix transmatx;
	//ChaMatrixIdentity(&transmatx);
	//m_bone->CalcAxisMatX(0, m_endbone, &transmatx, 0);
	//CQuaternion rotq;
	//rotq.RotationMatrix(transmatx);
	//CQuaternion invrotq;
	//rotq.inv(&invrotq);
	//btQuaternion btrotq;
	//btrotq = btQuaternion(rotq.x, rotq.y, rotq.z, rotq.w);
	//btQuaternion invbtrotq;
	//invbtrotq = btQuaternion(invrotq.x, invrotq.y, invrotq.z, invrotq.w);

	//btQuaternion setbtqB;
	////setbtqB = invbtrotqB * btrotq * btrotqB;
	////setbtqB = invbtrotqB * invbtrotq * btrotqB;
	////setbtqB = btrotqB * invbtrotq * invbtrotqB;
	//setbtqB = btrotqB * rotA * invbtrotqB;
	//dsttraB.setRotation(setbtqB);


	btVector3 pivotB;
	pivotB = invtraB(btVector3(aftparentposB.x, aftparentposB.y, aftparentposB.z));;
	dsttraB.setOrigin(pivotB);

	return 0;
}



int CBtObject::CreatePhysicsPosConstraintCurrent()
{
	if (m_topflag == 1) {
		return 0;
	}
	if (!m_endbone) {
		return 1;
	}
	if (!m_gz_rigidbody) {
		return 1;
	}


	ChaMatrix parentmat;
	ChaMatrix childmat;
	if (g_previewFlag == 5) {
		parentmat = m_bone->GetBtMat();
		childmat = m_endbone->GetBtMat();
	}
	else {
		parentmat = m_bone->GetCurMp().GetWorldMat();
		childmat = m_endbone->GetCurMp().GetWorldMat();
	}

	ChaVector3 parentposA, childposA, aftparentposA, aftchildposA;
	parentposA = m_bone->GetJointFPos();
	ChaVector3TransformCoord(&aftparentposA, &parentposA, &parentmat);
	childposA = m_endbone->GetJointFPos();
	ChaVector3TransformCoord(&aftchildposA, &childposA, &childmat);
	//ChaVector3 centerA = (aftparentposA + aftchildposA) * 0.5f;
	ChaVector3 centerA = aftchildposA;

	//////////////////
	////////////////// constraint
	_ASSERT(m_btWorld);
	_ASSERT(m_bone);

	btTransform FrameA;//剛体設定時のA側変換行列。
	btTransform FrameB;//剛体設定時のB側変換行列。
	FrameA.setIdentity();
	FrameB.setIdentity();

	//ChaMatrix transmatx;
	//ChaMatrixIdentity(&transmatx);
	////m_bone->CalcAxisMatX(0, m_endbone, &transmatx, 0);
	//m_bone->CalcAxisMatX(0, m_endbone, &transmatx, 0);
	//CQuaternion rotq;
	//rotq.RotationMatrix(transmatx);
	//CQuaternion invrotq;
	//rotq.inv(&invrotq);
	//btQuaternion btq = btQuaternion(invrotq.x, invrotq.y, invrotq.z, invrotq.w);
	////btQuaternion btq = btQuaternion(rotq.x, rotq.y, rotq.z, rotq.w);
	//FrameA.setRotation(btq);


	btTransform rigidtraA = m_gz_rigidbody->getWorldTransform();
	btTransform invtraA = rigidtraA.inverse();
	btTransform rigidtraB = m_rigidbody->getWorldTransform();
	btTransform invtraB = rigidtraB.inverse();
	ChaVector3 rigidcenter = (aftparentposA + aftchildposA) * 0.5f;
	btVector3 originA = m_rigidbody->getWorldTransform().getOrigin();

	//CQuaternion cqA, invcqA;
	//cqA.RotationMatrix(parentmat);
	//btQuaternion btqA = btQuaternion(cqA.x, cqA.y, cqA.z, cqA.w);
	////FrameA.setRotation(btqA);
	//cqA.inv(&invcqA);
	//btQuaternion invbtqA = btQuaternion(invcqA.x, invcqA.y, invcqA.z, invcqA.w);
	//FrameA.setRotation(invbtqA);


	//btQuaternion firstbtqA = GetFirstTransform().getRotation();
	//btQuaternion invfirstbtqA = firstbtqA.inverse();
	//FrameA.setRotation(firstbtqA);
	////FrameA.setRotation(invfirstbtqA);


	//////初期姿勢になる
	//btQuaternion btqA, invbtqA;
	//btQuaternion btqB, invbtqB, btqBFromA, invbtqBFromA;
	//btqA = rigidtraA.getRotation();
	//invbtqA = invtraA.getRotation();
	//FrameA.setRotation(btqA);
	//btqB = rigidtraB.getRotation();
	//invbtqB = invtraB.getRotation();
	//FrameB.setRotation(btqB);
	//btVector3 pivotA, pivotB;
	//pivotA = invtraA(btVector3(centerA.x, centerA.y, centerA.z));
	//FrameA.setOrigin(pivotA);
	//pivotB = invtraB(btVector3(centerA.x, centerA.y, centerA.z));
	//FrameB.setOrigin(pivotB);


	//FrameA, FrameBはコンストレイントからみたときの剛体Aと剛体Bの姿勢？？？？
	//それとも剛体からみたコンストレイントの姿勢？？？？
	//どっち？？？？
	//位置が正しい時の情報からすると、剛体からみたコンストレイントの姿勢！！！？？？

	btQuaternion btqA, invbtqA, btqAFromB;
	btQuaternion btqB, invbtqB, btqBFromA, invbtqBFromA;
	btqA = rigidtraA.getRotation();
	invbtqA = invtraA.getRotation();
	//FrameA.setRotation(invbtqA);
	//FrameA.setRotation(btqA);
	btqB = rigidtraB.getRotation();
	invbtqB = invtraB.getRotation();
	//btqBFromA = btqB * invbtqA;
	//btqBFromA = invbtqA * btqB;
	//btqBFromA = invbtqA * btqB * btqA;
	//btqBFromA = btqA * btqB * invbtqA;
	//btqAFromB = invbtqB * btqA;
	//btqAFromB = btqA * invbtqB;
	//btqAFromB = btqB * btqA * invbtqB;
	//btqAFromB = invbtqB * btqA * btqB;
	//FrameB.setRotation(btqB);
	//FrameB.setRotation(btqBFromA);
	//FrameB.setRotation(invbtqB);
	//FrameB.setRotation(btqAFromB);


	//位置が正しい
	//剛体Aはコンストレイントの場所と同じだからFrameAのOriginはゼロ。
	btVector3 pivotA, pivotB;
	pivotB = btVector3(aftchildposA.x, aftchildposA.y, aftchildposA.z) - btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z);
	FrameB.setOrigin(pivotB);

	btGeneric6DofSpringConstraint* dofC = 0;
	//dofC = new btGeneric6DofSpringConstraint(*m_rigidbody, *m_gz_rigidbody, FrameA, FrameB, true);
	dofC = new btGeneric6DofSpringConstraint(*m_gz_rigidbody, *m_rigidbody, FrameA, FrameB, true);
	//dofC = new btGeneric6DofSpringConstraint(*m_gz_rigidbody, *m_rigidbody, FrameA, FrameB, false);
	//dofC = new btConeTwistConstraint(*m_rigidbody, *m_gz_rigidbody, FrameA, FrameB);
	_ASSERT(dofC);
	if (dofC) {
		int dofid;
		for (dofid = 0; dofid < 3; dofid++) {
			//dofC->setEquilibriumPoint(dofid);
			dofC->enableSpring(dofid, true);
			//dofC->enableSpring(dofid, false);

			dofC->setStiffness(dofid, 800.0);
		}
		for (dofid = 3; dofid < 6; dofid++) {
			//dofC->setEquilibriumPoint(dofid);
			//dofC->enableSpring(dofid, true);
			dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!

			dofC->setStiffness(dofid, 1.0);
		}

		int dofcindex;
		for (dofcindex = 0; dofcindex < 6; dofcindex++) {
			//0-2:linear, 3-5:angular
			//dofC->setParam(BT_CONSTRAINT_STOP_CFM, 0, dofcindex);//CFM 0 壊れにくい
			//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.8, dofcindex);//ERP(0-1) 値大 --> エラー補正大
			//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.10, dofcindex);//ERP(0-1) 値大 --> エラー補正大
			//dofC->setParam(BT_CONSTRAINT_STOP_CFM, 0, dofcindex);//CFM 0 壊れにくい
			//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.010, dofcindex);//ERP(0-1) 値大 --> エラー補正大
			//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.10, dofcindex);//ERP(0-1) 値大 --> エラー補正大

			
			if (dofcindex < 3) {
				//位置
				dofC->setParam(BT_CONSTRAINT_STOP_CFM, 0, dofcindex);//CFM 0 壊れにくい
				//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.0040, dofcindex);//ERP(0-1) 値大 --> エラー補正大
				//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.0080, dofcindex);//ERP(0-1) 値大 --> エラー補正大
				dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.010, dofcindex);//ERP(0-1) 値大 --> エラー補正大
			}
			else {
				//回転
				//dofC->setParam(BT_CONSTRAINT_STOP_CFM, 1.0, dofcindex);//CFM 0 壊れにくい
				dofC->setParam(BT_CONSTRAINT_STOP_CFM, 0.5, dofcindex);//CFM 0 壊れにくい
				dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.0, dofcindex);//ERP(0-1) 値大 --> エラー補正大
				//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.0080, dofcindex);//ERP(0-1) 値大 --> エラー補正大
			}

			dofC->setDamping(dofcindex, 0.7);
		}

		dofC->setEquilibriumPoint();

		CONSTRAINTELEM newce;
		newce.constraint = dofC;//!!!!!!!!!!!!!!!!!!!!!!
		newce.centerbone = m_endbone;
		newce.childbto = NULL;
		m_gz_vecconstraint.push_back(newce);

		//m_btWorld->addConstraint(dofC, true);
		//m_btWorld->addConstraint((btTypedConstraint*)dofC, false);//!!!!!!!!!!!! disable collision between linked bodies
		m_btWorld->addConstraint((btTypedConstraint*)dofC, true);//!!!!!!!!!!!! disable collision between linked bodies
																 //m_dofC = dofC;
	}

	return 0;
}

int CBtObject::CreatePhysicsPosConstraintChild(CBtObject* childbto)
{
	if (m_topflag == 1) {
		return 0;
	}
	if (!childbto) {
		return 1;
	}
	if (!childbto->m_bone) {
		return 1;
	}
	if (!childbto->m_endbone) {
		return 1;
	}
	if (!m_gz_rigidbody) {
		return 1;
	}


	ChaMatrix parentmat;
	ChaMatrix childmat;
	if (g_previewFlag == 5) {
		parentmat = childbto->m_bone->GetBtMat();
		childmat = childbto->m_endbone->GetBtMat();
	}
	else {
		parentmat = childbto->m_bone->GetCurMp().GetWorldMat();
		childmat = childbto->m_endbone->GetCurMp().GetWorldMat();
	}

	ChaVector3 parentposA, childposA, aftparentposA, aftchildposA;
	parentposA = childbto->m_bone->GetJointFPos();
	ChaVector3TransformCoord(&aftparentposA, &parentposA, &parentmat);
	childposA = childbto->m_endbone->GetJointFPos();
	ChaVector3TransformCoord(&aftchildposA, &childposA, &childmat);
	ChaVector3 rigidcenter = (aftparentposA + aftchildposA) * 0.5f;
	//ChaVector3 centerA = aftchildposA;
	ChaVector3 centerA = aftparentposA;

	//////////////////
	////////////////// constraint
	_ASSERT(m_btWorld);
	_ASSERT(m_bone);

	btTransform FrameA;//剛体設定時のA側変換行列。
	btTransform FrameB;//剛体設定時のB側変換行列。
	FrameA.setIdentity();
	FrameB.setIdentity();



	btTransform rigidtraA = m_gz_rigidbody->getWorldTransform();
	btTransform invtraA = rigidtraA.inverse();
	btTransform rigidtraB = childbto->m_rigidbody->getWorldTransform();
	btTransform invtraB = rigidtraB.inverse();
	//ChaVector3 rigidcenter = (aftparentposA + aftchildposA) * 0.5f;
	//btVector3 originA = childbto->m_rigidbody->getWorldTransform().getOrigin();

	//btQuaternion btqA, invbtqA;
	//btQuaternion btqB, invbtqB, btqBFromA, invbtqBFromA;
	//btqA = rigidtraA.getRotation();
	//invbtqA = invtraA.getRotation();
	////FrameA.setRotation(invbtqA);
	////FrameA.setRotation(btqA);
	//btqB = rigidtraB.getRotation();
	//invbtqB = invtraB.getRotation();
	////FrameB.setRotation(invbtqB);
	////FrameB.setRotation(btqB);
	////invbtqBFromA = btqA * invbtqB * invbtqA;
	////invbtqBFromA = invbtqA * invbtqB * btqA;
	////invbtqBFromA = invbtqA * invbtqB;
	////FrameB.setRotation(invbtqBFromA);
	////btqBFromA = invbtqA * btqB * btqA;
	////btqBFromA = btqA * btqB * invbtqA;
	////btqBFromA = invbtqA * btqB;
	////btqBFromA = btqB * invbtqA;
	////FrameB.setRotation(btqBFromA);


	//FrameA, FrameBはコンストレイントからみたときの剛体Aと剛体Bの姿勢？？？？
	//それとも剛体からみたコンストレイントの姿勢？？？？
	//どっち？？？？
	//位置が正しい時の情報からすると、剛体からみたコンストレイントの姿勢！！！？？？



	btQuaternion btqA, invbtqA, btqAFromB;
	btQuaternion btqB, invbtqB, btqBFromA, invbtqBFromA;
	btqA = rigidtraA.getRotation();
	invbtqA = invtraA.getRotation();
	//FrameA.setRotation(invbtqA);
	//FrameA.setRotation(btqA);
	btqB = rigidtraB.getRotation();
	invbtqB = invtraB.getRotation();
	//btqBFromA = btqB * invbtqA;
	//btqBFromA = invbtqA * btqB;
	//btqBFromA = invbtqA * btqB * btqA;
	//btqBFromA = btqA * btqB * invbtqA;
	//btqAFromB = invbtqB * btqA;
	//btqAFromB = btqA * invbtqB;
	//btqAFromB = btqB * btqA * invbtqB;
	//btqAFromB = invbtqB * btqA * btqB;
	//FrameB.setRotation(btqB);
	//FrameB.setRotation(btqBFromA);
	//FrameB.setRotation(invbtqB);
	//FrameB.setRotation(btqAFromB);

	btVector3 pivotA, pivotB;
	////pivotA = btVector3(centerA.x, centerA.y, centerA.z) - originA;
	//pivotA = btVector3(centerA.x, centerA.y, centerA.z);
	////pivotA = invtraA(btVector3(centerA.x, centerA.y, centerA.z));
	//pivotA = btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z);
	//FrameA.setOrigin(pivotA);
	////pivotB = invtraA(btVector3(centerA.x, centerA.y, centerA.z));
	//pivotB = btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z) - btVector3(aftparentposA.x, aftparentposA.y, aftparentposA.z);
	//pivotB = btVector3(aftparentposA.x, aftparentposA.y, aftparentposA.z) - btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z);
	//FrameB.setOrigin(pivotB);
	//FrameB.setOrigin(pivotA);


	//位置が正しい
	//剛体Aはコンストレイントの場所と同じだからFrameAのOriginはゼロ。
	pivotB = btVector3(aftparentposA.x, aftparentposA.y, aftparentposA.z) - btVector3(rigidcenter.x, rigidcenter.y, rigidcenter.z);
	FrameB.setOrigin(pivotB);


	btGeneric6DofSpringConstraint* dofC = 0;
	//dofC = new btGeneric6DofSpringConstraint(*(childbto->m_rigidbody), *m_gz_rigidbody, FrameA, FrameB, true);
	dofC = new btGeneric6DofSpringConstraint(*m_gz_rigidbody, *(childbto->m_rigidbody), FrameA, FrameB, true);
	//dofC = new btGeneric6DofSpringConstraint(*m_gz_rigidbody, *(childbto->m_rigidbody), FrameA, FrameB, false);
	//dofC = new btConeTwistConstraint(*m_rigidbody, *m_gz_rigidbody, FrameA, FrameB);
	_ASSERT(dofC);
	if (dofC) {
		int dofid;
		for (dofid = 0; dofid < 3; dofid++) {
			//dofC->setEquilibriumPoint(dofid);
			dofC->enableSpring(dofid, true);
			//dofC->enableSpring(dofid, false);

			dofC->setStiffness(dofid, 800.0);

		}
		for (dofid = 3; dofid < 6; dofid++) {
			//dofC->setEquilibriumPoint(dofid);
			//dofC->enableSpring(dofid, true);
			dofC->enableSpring(dofid, false);//!!!!!!!!!!!!!!!!!!

			dofC->setStiffness(dofid, 1.0);

		}

		int dofcindex;
		for (dofcindex = 0; dofcindex < 6; dofcindex++) {
			//0-2:linear, 3-5:angular
			//dofC->setParam(BT_CONSTRAINT_STOP_CFM, 0, dofcindex);//CFM 0 壊れにくい
			//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.8, dofcindex);//ERP(0-1) 値大 --> エラー補正大
			//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.10, dofcindex);//ERP(0-1) 値大 --> エラー補正大
			//dofC->setParam(BT_CONSTRAINT_STOP_CFM, 0, dofcindex);//CFM 0 壊れにくい
			//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.010, dofcindex);//ERP(0-1) 値大 --> エラー補正大

			if (dofcindex < 3) {
				//位置
				dofC->setParam(BT_CONSTRAINT_STOP_CFM, 0, dofcindex);//CFM 0 壊れにくい
				//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.0040, dofcindex);//ERP(0-1) 値大 --> エラー補正大
				//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.0080, dofcindex);//ERP(0-1) 値大 --> エラー補正大
				dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.010, dofcindex);//ERP(0-1) 値大 --> エラー補正大
			}
			else {
				//回転
				//dofC->setParam(BT_CONSTRAINT_STOP_CFM, 1.0, dofcindex);//CFM 0 壊れにくい
				dofC->setParam(BT_CONSTRAINT_STOP_CFM, 0.5, dofcindex);//CFM 0 壊れにくい
				dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.0, dofcindex);//ERP(0-1) 値大 --> エラー補正大
				//dofC->setParam(BT_CONSTRAINT_STOP_ERP, 0.0080, dofcindex);//ERP(0-1) 値大 --> エラー補正大
			}


			dofC->setDamping(dofcindex, 0.7);
		}

		dofC->setEquilibriumPoint();

		CONSTRAINTELEM newce;
		newce.constraint = dofC;//!!!!!!!!!!!!!!!!!!!!!!
		newce.centerbone = childbto->m_endbone;
		newce.childbto = childbto;
		m_gz_vecconstraint.push_back(newce);

		//m_btWorld->addConstraint(dofC, true);
		//m_btWorld->addConstraint((btTypedConstraint*)dofC, false);//!!!!!!!!!!!! disable collision between linked bodies
		m_btWorld->addConstraint((btTypedConstraint*)dofC, true);//!!!!!!!!!!!! disable collision between linked bodies
																 //m_dofC = dofC;
	}

	return 0;
}

int CBtObject::DestroyPhysicsPosConstraint()
{
	DestroyGZObj();
	return 0;
}

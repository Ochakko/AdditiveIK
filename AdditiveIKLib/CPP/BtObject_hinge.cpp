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

#include <BtObject.h>

#include <Bone.h>

#include <RigidElem.h>

#define DBGH
#include <dbg.h>

using namespace std;

CBtObject::CBtObject( CBtObject* parbt, btDynamicsWorld* btWorld )
{
	InitParams();
	if( parbt ){
		parbt->AddChild( this );
	}
	m_btWorld = btWorld;
}

CBtObject::~CBtObject()
{
	DestroyObjs();
}

int CBtObject::InitParams()
{
	ChaMatrixIdentity( &m_transmat );
	m_btWorld = 0;

	m_topflag = 0;
	m_parbone = 0;
	m_bone = 0;
	m_colshape = 0;
	m_rigidbody = 0;
	m_constraint.clear();

	m_parbt = 0;
	m_chilbt.clear();

	m_mass = 0.0f;

	return 0;
}
int CBtObject::DestroyObjs()
{
	int chilno;
	for( chilno = 0; chilno < (int)m_constraint.size(); chilno++ ){
		btTypedConstraint* constptr = m_constraint[ chilno ];
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

	m_chilbt.clear();

	return 0;
}

btRigidBody* CBtObject::localCreateRigidBody( btScalar mass, const btTransform& startTransform, btCollisionShape* shape )
{
	_ASSERT( shape );

	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		shape->calculateLocalInertia( mass, localInertia );

	btDefaultMotionState* myMotionState = new btDefaultMotionState( startTransform );
		
	btRigidBody::btRigidBodyConstructionInfo rbInfo( mass, myMotionState, shape, localInertia );
	btRigidBody* body = new btRigidBody( rbInfo );

body->setRestitution( 0.0 );
body->setFriction( 1.0 );

	m_btWorld->addRigidBody(body);


	return body;
}
int CBtObject::AddChild( CBtObject* addbt )
{
	m_chilbt.push_back( addbt );
	return 0;
}

int CBtObject::CreateObject( CBtObject* parbt, CBone* parbone, CBone* curbone )
{
	m_bone = curbone;
	m_parbone = parbone;
	m_parbt = parbt;

	if( !m_bone ){
		return 0;
	}

	if( !m_bone->m_child ){
		return 0;
	}

	float r, h;
	r = curbone->m_rigidelem->m_sphr;
	h = curbone->m_rigidelem->m_cylileng;
	if( (r < 0.001f) || (h < 0.001f) ){
		r = 0.001f;
		h = 0.001f;
	}

	m_colshape = new btCapsuleShape( btScalar( r ), btScalar( h ) );
	_ASSERT( m_colshape );

	float capvol = 2.0f * (float)PAI * r * h + 4.0f * (float)PAI * r * r * r / 3.0f;
	m_mass = capvol * curbone->m_rigidelem->m_mitsudo * 1.0e-5f;
	//m_mass = capvol * curbone->m_rigidelem->m_mitsudo * 250.0f;
//DbgOut( L"check!!!m_mass [%s] %f\r\n", curbone->m_wbonename, m_mass );

	//m_mass = 0.0f;

	if( m_mass <= 0.001f ){
		m_mass = 5.0f;
	}
//	if( m_mass > 10.0f ){
//		m_mass = 10.0f;
//	}

	//if( m_mass != 0.0f ){
		ChaMatrix startrot = curbone->m_rigidelem->m_axismat_parY * curbone->m_rigidelem->m_startmat;
		m_transmat = startrot;

		startrot._41 = 0.0f;
		startrot._42 = 0.0f;
		startrot._43 = 0.0f;
		D3DXQUATERNION xq;
		D3DXQuaternionRotationMatrix( &xq, &startrot );

		btScalar qx = xq.x;
		btScalar qy = xq.y;
		btScalar qz = xq.z;
		btScalar qw = xq.w;
		btQuaternion btq( qx, qy, qz, qw ); 

//void  setEulerZYX (btScalar eulerX, btScalar eulerY, btScalar eulerZ) 
//  Set the matrix from euler angles YPR around ZYX axes. 
 

		ChaVector3 centerA, parposA, chilposA, aftparposA, aftchilposA;
		parposA = m_bone->m_jointfpos;
		ChaVector3TransformCoord( &aftparposA, &parposA, &m_bone->m_rigidelem->m_startmat );
		chilposA = m_bone->m_child->m_jointfpos;
		ChaVector3TransformCoord( &aftchilposA, &chilposA, &m_bone->m_child->m_rigidelem->m_startmat );
		centerA = ( aftparposA + aftchilposA ) * 0.5f;
		btVector3 btv( btScalar( centerA.x ), btScalar( centerA.y ), btScalar( centerA.z ) );

		btTransform transform;
		transform.setIdentity();
		transform.setRotation( btq );
		transform.setOrigin( btv );

		ChaVector3 diffA = chilposA - parposA;
		m_boneleng = ChaVector3Length( &diffA );

		ChaMatrixIdentity( &m_cen2parY );
		m_cen2parY._41 = 0.0f;
		m_cen2parY._42 = -m_boneleng * 0.5f;
		m_cen2parY._43 = 0.0f;


		ChaVector3 partocen = centerA - aftparposA;
		ChaMatrixIdentity( &m_par2cen );
		m_par2cen._41 = partocen.x;
		m_par2cen._42 = partocen.y;
		m_par2cen._43 = partocen.z;

		m_transmat = startrot;
		m_transmat._41 = centerA.x;
		m_transmat._42 = centerA.y;
		m_transmat._43 = centerA.z;


		m_rigidbody = localCreateRigidBody( m_mass, transform, m_colshape );
		_ASSERT( m_rigidbody );

	//}

	return 0;
}

int CBtObject::CreateBtConstraint()
{
	if( m_topflag == 1 ){
		return 0;
	}
	_ASSERT( m_btWorld );
	_ASSERT( m_bone );

	if( !m_bone->m_child ){
		return 0;
	}
	if( !m_bone->m_child->m_child ){
		return 0;
	}

	int chilno;
	for( chilno = 0; chilno < (int)m_chilbt.size(); chilno++ ){
		CBtObject* chilbto = m_chilbt[ chilno ];
		if( !chilbto ){
			continue;
		}

		ChaVector3 centerposA, parposA, chilposA, aftparposA, aftchilposA;
		parposA = m_bone->m_jointfpos;
		ChaVector3TransformCoord( &aftparposA, &parposA, &m_bone->m_rigidelem->m_startmat );
		chilposA = chilbto->m_bone->m_jointfpos;
		ChaVector3TransformCoord( &aftchilposA, &chilposA, &chilbto->m_bone->m_rigidelem->m_startmat );
		centerposA = ( aftparposA + aftchilposA ) * 0.5f;
		ChaVector3 pivotA;
		pivotA = aftchilposA - centerposA;
		btVector3 btpivotA;
		btpivotA.setValue( btScalar(pivotA.x), btScalar(pivotA.y), btScalar(pivotA.z) );

		btTransform localA, localB;
		ChaMatrix axisA, axisB;
		axisA = m_bone->m_rigidelem->m_axismat_par * m_bone->m_rigidelem->m_startmat;
		axisA._41 = 0.0f;
		axisA._42 = 0.0f;
		axisA._43 = 0.0f;

		//int magono;
		//for( magono = 0; magono < (int)chilbto->m_chilbt.size(); magono++ ){
//		if( !chilbto->m_chilbt.empty() ){
			//CBtObject* magobto = chilbto->m_chilbt[ 0 ];
			//_ASSERT( magobto );

			ChaVector3 centerposB, parposB, chilposB, aftparposB, aftchilposB;
			parposB = chilbto->m_bone->m_jointfpos;
			ChaVector3TransformCoord( &aftparposB, &parposB, &chilbto->m_bone->m_rigidelem->m_startmat );
			chilposB = chilbto->m_bone->m_child->m_jointfpos;
			ChaVector3TransformCoord( &aftchilposB, &chilposB, &chilbto->m_bone->m_child->m_rigidelem->m_startmat );
			centerposB = ( aftparposB + aftchilposB ) * 0.5f;
			ChaVector3 pivotB;
			pivotB = aftparposB - centerposB;
			btVector3 btpivotB;
			btpivotB.setValue( btScalar(pivotB.x), btScalar(pivotB.y), btScalar(pivotB.z) );

			//axisB = m_bone->m_rigidelem->m_axismat_chil[ magobto->m_bone ] * chilbto->m_bone->m_rigidelem->m_startmat;
			axisB = chilbto->m_bone->m_rigidelem->m_axismat_par * chilbto->m_bone->m_rigidelem->m_startmat;
			axisB._41 = 0.0f;
			axisB._42 = 0.0f;
			axisB._43 = 0.0f;

			/***
			ChaVector3 basez( 0.0f, 0.0f, 1.0f );
			ChaVector3 basex( 1.0f, 0.0f, 0.0f );
			ChaVector3 axis1, axis2;
			ChaVector3TransformCoord( &axis1, &basez, &axisA );
			ChaVector3Normalize( &axis1, &axis1 );
			ChaVector3TransformCoord( &axis2, &basex, &axisA );
			ChaVector3Normalize( &axis2, &axis2 );
			btVector3 btaxis1, btaxis2;
			btaxis1.setValue( btScalar( axis1.x ), btScalar( axis1.y ), btScalar( axis1.z ) );
			btaxis2.setValue( btScalar( axis2.x ), btScalar( axis2.y ), btScalar( axis2.z ) );
			***/

			ChaVector3 basez( 0.0f, 0.0f, 1.0f );
			btVector3 btaxisA( btScalar( 0.0f ), btScalar( 0.0f ), btScalar( 1.0f ) );
			
			ChaVector3 axiszB;
			ChaVector3TransformCoord( &axiszB, &basez, &axisB );
			ChaVector3Normalize( &axiszB, &axiszB );
			btVector3 btaxisB( btScalar( axiszB.x ), btScalar( axiszB.y ), btScalar( axiszB.z ) );

			/***
			ChaMatrix invA;
			ChaMatrixInverse( &invA, NULL, &axisA );
			ChaVector3 axiszB;
			ChaVector3TransformCoord( &axiszB, &basez, &invA );
			ChaVector3 finaxiszB;
			ChaVector3TransformCoord( &finaxiszB, &axiszB, &axisB );
			ChaVector3Normalize( &finaxiszB, &finaxiszB );
			btVector3 btaxisB( btScalar( finaxiszB.x ), btScalar( finaxiszB.y ), btScalar( finaxiszB.z ) );
			***/

//			if( m_bone->m_rigidelem->m_bcone == false ){
				btHingeConstraint* hingeC;

				//ChaVector3 axiszA, axiszB;
				if( m_rigidbody && chilbto->m_rigidbody ){

					_ASSERT( m_rigidbody );
					_ASSERT( chilbto->m_rigidbody );

/***					
					localA.getBasis().setValue( axisA._11, axisA._12, axisA._13, axisA._21, axisA._22, axisA._23, axisA._31, axisA._32, axisA._33 ); 
					localA.setOrigin(btpivotA);

					localB.getBasis().setValue( axisA._11, axisA._12, axisA._13, axisA._21, axisA._22, axisA._23, axisA._31, axisA._32, axisA._33 ); 
					localB.setOrigin(btpivotB);
***/

					hingeC =  new btHingeConstraint( *m_rigidbody, *(chilbto->m_rigidbody),  btpivotA, btpivotB, btaxisA, btaxisB );
//					hingeC = new btHinge2Constraint( *m_rigidbody, *( chilbto->m_rigidbody ), anchor, btaxis1, btaxis2 );
					hingeC->setBreakingImpulseThreshold( btScalar( 1.0e38 ) );

					//hingeC =  new btHingeConstraint( *m_rigidbody, *( chilbto->m_rigidbody ), localA, localB );
					//hingeC->setLimit(btScalar((float)PAI / 2.0f), btScalar((float)PAI / 2.0f));
					//hingeC->setLowerLimit( btScalar( (float)-PAI / 4.0f ) );
					//hingeC->setUpperLimit( btScalar( (float)PAI / 4.0f ) );
					m_constraint.push_back( (btTypedConstraint*)hingeC );
					m_btWorld->addConstraint(hingeC);
					//m_btWorld->addConstraint(hingeC, true);
				}
/***
			}else{
				_ASSERT( 0 );
				btConeTwistConstraint* coneC;

				localA.setIdentity(); localB.setIdentity();
		
				localA.getBasis().setValue( axisA._11, axisA._12, axisA._13, axisA._21, axisA._22, axisA._23, axisA._31, axisA._32, axisA._33 ); 
				localA.setOrigin(btpivotA);

				localB.getBasis().setValue( axisB._11, axisB._12, axisB._13, axisB._21, axisB._22, axisB._23, axisB._31, axisB._32, axisB._33 ); 
				localB.setOrigin(btpivotB);

				if( m_rigidbody && chilbto->m_rigidbody ){
					coneC = new btConeTwistConstraint( *m_rigidbody, *( chilbto->m_rigidbody ), localA, localB);
					m_constraint.push_back( (btTypedConstraint*)coneC );
					coneC->setLimit( (float)PAI / 4.0f, (float)PAI / 4.0f, (float)PAI / 2.0f );
					m_btWorld->addConstraint(coneC, true);
				}
			}
***/
//		}

	}

	return 0;
}

int CBtObject::SetBtMotion( ChaMatrix* wmat, ChaMatrix* vpmat )
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

	btMatrix3x3 worldmat = worldtra.getBasis();
	btVector3 worldpos = worldtra.getOrigin();

	btVector3 tmpcol[3];
	int colno;
	for( colno = 0; colno < 3; colno++ ){
		tmpcol[colno] = worldmat.getColumn( colno );
//		tmpcol[colno] = worldmat.getRow( colno );
	}

	ChaMatrix xworld;
	ChaMatrixIdentity( &xworld );

	xworld._11 = tmpcol[0].x();
	xworld._12 = tmpcol[0].y();
	xworld._13 = tmpcol[0].z();

	xworld._21 = tmpcol[1].x();
	xworld._22 = tmpcol[1].y();
	xworld._23 = tmpcol[1].z();

	xworld._31 = tmpcol[2].x();
	xworld._32 = tmpcol[2].y();
	xworld._33 = tmpcol[2].z();

	xworld._41 = worldpos.x();
	xworld._42 = worldpos.y();
	xworld._43 = worldpos.z();

	ChaMatrix invtransmat;
	ChaMatrixInverse( &invtransmat, NULL, &m_transmat );

	//m_bone->m_curmp.m_btmat = m_bone->m_rigidelem->m_startmat * invtransmat * xworld;
	//m_bone->m_curmp.m_btmat = invtransmat * m_cen2par * xworld;
	//m_bone->m_curmp.m_btmat = m_par2cen * invtransmat * m_cen2parY * xworld;
	//m_bone->m_curmp.m_btmat = m_bone->m_rigidelem->m_startmat * m_par2cen * invtransmat * m_cen2parY * xworld;
	//m_bone->m_curmp.m_btmat = m_par2cen * invtransmat * xworld;
	//m_bone->m_curmp.m_btmat = invtransmat * xworld;

	if( m_boneleng > 0.001f ){
		m_bone->m_curmp.m_btmat = m_bone->m_rigidelem->m_startmat * m_par2cen * invtransmat * m_cen2parY * xworld;
	}else{
		m_bone->m_curmp.m_btmat = m_bone->m_parent->m_curmp.m_btmat;
	}

	m_bone->m_curmp.m_setbtflag = 1;

	return 0;
}

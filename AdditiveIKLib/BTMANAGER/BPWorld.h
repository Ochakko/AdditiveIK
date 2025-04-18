#ifndef BP_WORLD_H
#define BP_WORLD_H


#include "../Examples/CommonInterfaces/CommonExampleInterface.h"
#include "../Examples/CommonInterfaces/CommonGUIHelperInterface.h"
#include "../BTSRC/BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "../BTSRC/BulletCollision/CollisionShapes/btCollisionShape.h"
#include "../BTSRC/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h"


#include "../BTSRC/LinearMath/btTransform.h"
#include "../BTSRC/LinearMath/btHashMap.h"

//#include "../Examples/MultiThreadedDemo/CommonRigidBodyMTBase.h"

#include "../Examples/CommonInterfaces/CommonRigidBodyBase.h"
#include "../Examples/MultiThreadedDemo/CommonRigidBodyMTBase.h"


//struct BasicExample : public CommonRigidBodyBase

/*
#include "GlutDemoApplication.h"
#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "GLDebugDrawer.h"
*/

//#include <d3dx9.h>

#include <vector>

#include <ChaVecCalc.h>
/**
 *  動力学シミュレーションのワールド
 *
 *  動力学シミュレーションを行う環境を一通り準備する．
 *  また，ウィンドウなどGUIの管理も行う．
 *
 */


class CommonExampleInterface*    BPWorldCreateFunc(struct CommonExampleOptions& options);





class BPWorld : public CommonRigidBodyMTBase
{
  typedef void (*MonitoringFunc)(btScalar currentTime); // ユーザが定義するモニタリング関数へのポインタの型
  typedef void (*KeyboardFunc)(int key, int x, int y);

public:
  int RemakeG();
  btRigidBody* m_rigidbodyG;
  btCollisionShape* m_collisionG;

  enum WorldType{
    RIGID_BODY_ONLY,
    SOFT_BODY_INCLUDED
  };
  // コンストラクタ
  //BPWorld(void);
  BPWorld(struct GUIHelperInterface* helper, ChaMatrix wmat, char* winTitle, int winWidth, int winHeight, MonitoringFunc monFnPtr = NULL, KeyboardFunc keyFnPtr = NULL);
  BPWorld(struct GUIHelperInterface* helper, char* winTitle, int winWidth, int winHeight, WorldType wldType, MonitoringFunc monFnPtr = NULL, KeyboardFunc keyFnPtr = NULL);

  // コンストラクタの共通する処理を行う関数
  void init(char* winTitle, int winWidth, int winHeight, MonitoringFunc monFnPtr, KeyboardFunc keyFnPtr);

  void setNumThread(int srcnumthread);

  // デストラクタ
  virtual ~BPWorld(void);

  //// Begin: このクラス固有のメンバ関数
  btRigidBody* createRigidBody(btScalar mass, const btTransform& initTransform, btCollisionShape* shape);
  //void addConstraint(btTypedConstraint *constraint,
  //                   bool disableCollisionsBetweenLinkedBodies = false);
  void addConstraint(btGeneric6DofSpringConstraint *constraint,
	  bool disableCollisionsBetweenLinkedBodies);

  void removeConstraint(btTypedConstraint *constraint);
  int start();
  void setTimeStep(btScalar timeStep) { m_timeStep = timeStep; }
  void enableFixedTimeStep(bool onOff) { m_fixedTimeStepEnabled = onOff; }
  btScalar getTimeStep() { return m_timeStep; }
  btScalar getCurrentTime() { return m_currentTime; }
  void enableConstraintDisplay(bool onOff);
  void setKeyboardCallback(KeyboardFunc keyboard);
  //btSoftBodyWorldInfo& getSoftBodyWorldInfo() { return m_softBodyWorldInfo; }
  //void addSoftBody(btSoftBody* sb);
  void setCamera(double ptx, double pty, double ptz,
                 double dist, double azi, double ele,
                 double upx, double upy, double upz);
  void setGlobalERP(btScalar erp);

	btRigidBody* LCreateRigidBody( btScalar mass, const btTransform& startTransform, btCollisionShape* shape );

  //// End

  //// Begin: オーバライドするメンバ関数
  virtual void initPhysics();
  virtual void clientMoveAndDisplay();
  virtual void clientResetScene();
  virtual void displayCallback();
  virtual void keyboardCallback(unsigned char key, int x, int y);
  virtual void mouseMotionFunc(int x, int y);
  virtual void updateCamera();
  //// End

  //btDiscreteDynamicsWorldMt* getDynamicsWorld()
  //{
	 // return m_dynamicsWorld;
  //};


	int m_groupid;
	std::vector<int> m_coliids;
	//int m_myselfflag;

	btScalar m_gplaneh;
	btScalar m_gdisph;
	ChaVector2 m_gplanesize;
	int m_gplanedisp;

	double m_restitution;
	double m_friction;

private:
  //explicit BPWorld(); // 引数なしコンストラクタの呼び出しを禁止する

  ChaMatrix m_wmat;
  //// Begin: Bullet Physics を使うのに必要なメンバ変数
  btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
  btBroadphaseInterface* m_broadphase;
  btCollisionDispatcher* m_dispatcher;
  //btConstraintSolver* m_solver;
  btDefaultCollisionConfiguration* m_collisionConfiguration;
  //// End

  //// Begin: このクラスで使うメンバ変数
  MonitoringFunc m_doMonitoring; // ユーザが定義するモニタリング関数へのポインタ
  KeyboardFunc m_checkKey; // ユーザが定義するキーボード入力処理関数へのポインタ
  btScalar m_timeStep; // 刻み時間（シミュレーションを固定ステップで進める場合）
  bool m_fixedTimeStepEnabled; // 固定ステップでのシミュレーションが有効か否かを決めるフラグ

  btScalar m_currentTime; // 現在時刻
  char *m_winTitle; // ウィンドウのタイトル
  int m_winWidth;  // ウィンドウの幅 [pixels]
  int m_winHeight; // ウィンドウの高さ [pixels]


  //GLDebugDrawer m_debugDrawer;
  bool m_softBodyRequested;
  //btSoftBodyWorldInfo m_softBodyWorldInfo;
  //// End

  //void initWorldForRigidOnly();
  //void initWorldForSoftBody();

  int GetColiID();

  // 追加の描画機能（平面の描画など）を担当するクラス
  //class NewShapeDrawer : public GL_ShapeDrawer
  //{
  //  virtual ~NewShapeDrawer(){}
  //  void drawOpenGL(btScalar* transMatrix,
  //                  const btCollisionShape* shape,
  //                  const btVector3& color, int debugMode,
  //                  const btVector3& worldBoundsMin,
  //                  const btVector3& worldBoundsMax);
  //};


	virtual void physicsDebugDraw(int);
	virtual void renderScene(void);
	virtual void drawScreenText(void);
	//virtual void createEmptyDynamicsWorld(void);
	//virtual void createDefaultParameters(void);

	//virtual double getContactBreakingThreshold(double);
	//virtual double etAngularMotionDisc(void);
	//virtual void getBoundingSphere(class btVector3 &, double &);
	//virtual void project(class btTransform const &, class btVector3 const &, double &, double &, class btVector3 &, class btVector3 &);
	//virtual void calculateLocalInertia(double, class btVector3 &);

};

#endif // BP_WORLD_H

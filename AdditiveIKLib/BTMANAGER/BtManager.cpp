#include "stdafx.h"

#include "BPWorld.h" // BPWorldを使うため
#include <cstdio>

////////////////////////////////////////////////////////////////////////////////
/**
 *  シミュレーションのステップごとに呼ばれる関数（コールバック）
 *  物体の状態をモニタリングするために使う．
 */
void doMonitoring(btScalar currentTime){
//  btVector3 pos = g_sphereBody->getCenterOfMassPosition(); // 質量中心位置を取得
//  printf("y = %f\n", pos.y());
}

////////////////////////////////////////////////////////////////////////////////
/**
 *  物体を作成して，ワールドに配置する 
 */
void setUpObjects(BPWorld& bpWorld){
}

////////////////////////////////////////////////////////////////////////////////
/**
 *  プログラムの開始点
 */
int SetUpBtManager(){
  // 動力学シミュレーションのワールドの準備
  BPWorld bpWorld("Falling Sphere", // ウィンドウのタイトル
                  640, 480,         // ウィンドウの幅と高さ [pixels]
                  doMonitoring);    // モニタリング用関数へのポインタ
  
  setUpObjects(bpWorld); // 物体を作成して配置する
  bpWorld.enableFixedTimeStep(true);
  bpWorld.setTimeStep(0.01); // seconds
  bpWorld.setGlobalERP(0.2); // ERP
  bpWorld.start(); // ウィンドウを表示して，シミュレーションを開始する
  
  return 0;
}

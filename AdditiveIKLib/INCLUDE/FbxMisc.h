#ifndef FBXMISCH
#define FBXMISCH

#include <map>
#include <string>

#include <Coef.h>
#include <ChaVecCalc.h>

#include <fbxsdk.h>

#ifdef FBXMISCCPP
bool IsNullAndChildIsCameraNode(FbxNode* pNode);
int IntRotationOrder(EFbxRotationOrder srcorder);

#else
extern bool IsNullAndChildIsCameraNode(FbxNode* pNode);
extern int IntRotationOrder(EFbxRotationOrder srcorder);
#endif

#endif




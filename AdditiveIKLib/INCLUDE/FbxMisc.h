#ifndef FBXMISCH
#define FBXMISCH

#include <unordered_map>
#include <string>

#include <Coef.h>
#include <ChaVecCalc.h>

#include <fbxsdk.h>

#ifdef FBXMISCCPP
bool IsNullAndChildIsCameraNode(FbxNode* pNode);
int IntRotationOrder(EFbxRotationOrder srcorder);
int TrimBlendShapeName(const char* srcname, char* dstname, int dstlen);//srcnameの最後の.以降の文字列をdstnameに格納

#else
extern bool IsNullAndChildIsCameraNode(FbxNode* pNode);
extern int IntRotationOrder(EFbxRotationOrder srcorder);
extern int TrimBlendShapeName(const char* srcname, char* dstname, int dstlen);//srcnameの最後の.以降の文字列をdstnameに格納
#endif

#endif




#ifndef FBXFILEH
#define FBXFILEH

#include <fbxsdk.h>
#include <stdio.h>
#include <ChaVecCalc.h>

class CBVHFile;

class CModel;
class CBone;
class CNodeOnLoad;

#ifdef FBXFILECPP

/**
 * @fn
 * InitializeSdkObjects
 * @breaf FBXSDKのオブジェクトの作成と初期化。
 * @return 成功したら０。
 */
	int InitializeSdkObjects();

/**
 * @fn
 * DestroySdkObjects
 * @breaf FBXSDKのオブジェクトの開放。
 * @return 成功したら０。
 */
	int DestroySdkObjects();
	
/**
 * @fn
 * WriteFBXFile
 * @breaf FBXファイルを書き出す。
 * @param (FbxManager* psdk) IN FBXSDKのマネージャオブジェクト。
 * @param (CModel* pmodel) IN 書き出すモデルのデータ。
 * @param (char* pfilename) IN 書き出すFBXファイル名。
 * @return 成功したら０。
 * @detail ボーン影響度やアニメーションも書き出す。
 */	
	int WriteFBXFile(bool limitdegflag, FbxManager* psdk, CModel* pmodel, char* pfilename, char* fbxdate);
	void DestroySavedScene();
	
/**
 * @fn
 * BVH2FBXFile
 * @breaf BVHのアニメーションをFBXファイルに書き出す。
 * @param (FbxManager* psdk) IN FBXSDKのマネージャオブジェクト。
 * @param (CBVHFile* pbvhfile) IN 書き出すBVHのデータ。
 * @param (char* pfilename) IN 書き出すFBXファイル名。
 * @return 成功したら０。
 */	
	int BVH2FBXFile(FbxManager* psdk, CBVHFile* pbvhfile, char* pfilename, char* fbxdate);


	FbxAMatrix FbxGetGlobalPosition(bool usecache, CModel* srcmodel, FbxScene* pScene, FbxNode* pNode, const FbxTime& pTime, int srcframe, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition = 0);
	FbxAMatrix FbxGetPoseMatrix(FbxPose* pPose, int pNodeIndex);
	FbxAMatrix FbxGetGeometry(FbxNode* pNode);
	void FbxSetDefaultBonePosReq(FbxScene* pScene, CModel* pmodel, CNodeOnLoad* nodeonload, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* ParentGlobalPosition);
	FbxDouble3 FbxGetMaterialProperty(const FbxSurfaceMaterial* pMaterial, const char* pPropertyName, const char* pFactorPropertyName, char** ppTextureName);
	FbxDouble FbxGetMaterialShininessProperty(const FbxSurfaceMaterial* pMaterial);
	int IsValidFbxCluster(FbxCluster* cluster);

	//int CalcLocalNodeMatForMesh(FbxNode* pNode, ChaMatrix* dstnodemat);


#else
	extern int InitializeSdkObjects();
	extern int DestroySdkObjects();
	extern int WriteFBXFile(bool limitdegflag, FbxManager* psdk, CModel* pmodel, char* pfilename, char* fbxdate);
	extern void DestroySavedScene();
	extern int BVH2FBXFile(FbxManager* psdk, CBVHFile* pbvhfile, char* pfilename, char* fbxdate);

	extern FbxAMatrix FbxGetGlobalPosition(bool usecache, CModel* srcmodel, FbxScene* pScene, FbxNode* pNode, const FbxTime& pTime, int srcframe, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition = 0);
	extern FbxAMatrix FbxGetPoseMatrix(FbxPose* pPose, int pNodeIndex);
	extern FbxAMatrix FbxGetGeometry(FbxNode* pNode);
	extern void FbxSetDefaultBonePosReq(FbxScene* pScene, CModel* pmodel, CNodeOnLoad* nodeonload, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* ParentGlobalPosition);
	extern FbxDouble3 FbxGetMaterialProperty(const FbxSurfaceMaterial* pMaterial, const char* pPropertyName, const char* pFactorPropertyName, char** ppTextureName);
	extern FbxDouble FbxGetMaterialShininessProperty(const FbxSurfaceMaterial* pMaterial);
	extern int IsValidFbxCluster(FbxCluster* cluster);

	//extern int CalcLocalNodeMatForMesh(FbxNode* pNode, ChaMatrix* dstnodemat);

#endif

#endif


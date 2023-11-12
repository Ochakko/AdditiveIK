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
 * @breaf FBXSDK�̃I�u�W�F�N�g�̍쐬�Ə������B
 * @return ����������O�B
 */
	int InitializeSdkObjects();

/**
 * @fn
 * DestroySdkObjects
 * @breaf FBXSDK�̃I�u�W�F�N�g�̊J���B
 * @return ����������O�B
 */
	int DestroySdkObjects();
	
/**
 * @fn
 * WriteFBXFile
 * @breaf FBX�t�@�C���������o���B
 * @param (FbxManager* psdk) IN FBXSDK�̃}�l�[�W���I�u�W�F�N�g�B
 * @param (CModel* pmodel) IN �����o�����f���̃f�[�^�B
 * @param (char* pfilename) IN �����o��FBX�t�@�C�����B
 * @return ����������O�B
 * @detail �{�[���e���x��A�j���[�V�����������o���B
 */	
	int WriteFBXFile(bool limitdegflag, FbxManager* psdk, CModel* pmodel, char* pfilename, char* fbxdate);

/**
 * @fn
 * BVH2FBXFile
 * @breaf BVH�̃A�j���[�V������FBX�t�@�C���ɏ����o���B
 * @param (FbxManager* psdk) IN FBXSDK�̃}�l�[�W���I�u�W�F�N�g�B
 * @param (CBVHFile* pbvhfile) IN �����o��BVH�̃f�[�^�B
 * @param (char* pfilename) IN �����o��FBX�t�@�C�����B
 * @return ����������O�B
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


#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#define FBXFILECPP
#include <FBXFile.h>
//#include <fbxfilesdk/kfbxtransformation.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>
#include <coef.h>

#include <FBXBone.h>
#include <Model.h>
#include <MQOObject.h>
#include <Bone.h>
#include <PolyMesh4.h>
#include <MQOMaterial.h>
#include <MQOFace.h>
#include <InfBone.h>
#include <MotionPoint.h>
//#include <MorphKey.h>

#include <bvhelem.h>
#include <bvhfile.h>

#define KARCH_ENV_WIN


static FbxNode::EPivotSet s_convPivot;

static float FlClamp( float srcval, float minval, float maxval );

static int s_doublevertices = 0;
static CBVHElem* s_behead = 0;
static int s_invindex[3] = {0, 2, 1};
static int s_firstanimout = 0;

#include <map>
using namespace std;

typedef struct tag_blsindex
{
	int serialno;
	int blendshapeno;
	int channelno;
}BLSINDEX;
typedef struct tag_blsinfo
{
	BLSINDEX blsindex;
	FbxNode* basenode;
	CMQOObject* base;
	string targetname;
}BLSINFO;
static map<int, BLSINFO> s_blsinfo;

typedef struct tag_animinfo
{
	int motid;
	int orgindex;
	int maxframe;
	char* engmotname;
	FbxAnimLayer* animlayer;
}ANIMINFO;

static map<CBone*, map<int, int>> s_linkdirty;
static CModel* s_model = 0;

static ANIMINFO* s_ai = 0;
static int s_ainum = 0;

static CFBXBone* s_fbxbone = 0;
static CFBXBone* s_firsttopbone = 0;
static int s_fbxbonenum = 0;


static int sortfunc_leng( void *context, const void *elem1, const void *elem2);

int sortfunc_leng( void *context, const void *elem1, const void *elem2)
{
	ANIMINFO* info1 = (ANIMINFO*)elem1;
	ANIMINFO* info2 = (ANIMINFO*)elem2;

	int diffleng = info1->maxframe - info2->maxframe;
	return diffleng;
}

static FbxManager* s_pSdkManager = 0;
static int s_bvhflag = 0;

//static map<CBone*, FbxNode*> s_bone2skel;
static int s_firstoutmot;

static CFBXBone* CreateFBXBone( FbxScene* pScene, CModel* pmodel );
static void CreateFBXBoneReq( FbxScene* pScene, CBone* pbone, CFBXBone* parfbxbone );

static CFBXBone* CreateFBXBoneOfBVH( FbxScene* pScene );
static void CreateFBXBoneOfBVHReq( FbxScene* pScene, CBVHElem* pbe, CFBXBone* parfbxbone );

static int DestroyFBXBoneReq( CFBXBone* fbxbone );



static void CreateAndFillIOSettings(FbxManager* pSdkManager);
static bool SaveScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename, int pFileFormat=-1, bool pEmbedMedia=false);


static bool CreateScene(FbxManager* pSdkManager, FbxScene* pScene, CModel* pmodel );
static bool CreateBVHScene(FbxManager* pSdkManager, FbxScene* pScene );
static FbxNode* CreateFbxMesh( FbxManager* pSdkManager, FbxScene* pScene, CModel* pmodel, CMQOObject* curobj );
static FbxNode* CreateSkeleton(FbxScene* pScene, CModel* pmodel);
static void CreateSkeletonReq( FbxScene* pScene, CBone* pbone, CBone* pparbone, FbxNode* pparnode );
//static void LinkMeshToSkeletonReq( CFBXBone* fbxbone, FbxSkin* lSkin, FbxScene* pScene, FbxNode* lMesh, CMQOObject* curobj, CModel* pmodel );
static void LinkMeshToSkeletonReq(CFBXBone* fbxbone, FbxSkin* lSkin, FbxScene* pScene, FbxNode* pMesh, CMQOObject* curobj, CPolyMesh4* pm4, int* psetflag, CBone** ppsetbone);
static void LinkToTopBone(FbxSkin* lSkin, FbxScene* pScene, CMQOObject* curobj, CPolyMesh4* pm4, int* psetflag);



static int WriteBindPose(FbxScene* pScene, int bvhflag = 0);
static void WriteBindPoseReq( CFBXBone* fbxbone, FbxPose* lPose );



static void AnimateSkeleton(FbxScene* pScene, CModel* pmodel);
static void AnimateBoneReq( CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer, int curmotid, int motmax );
static int AnimateMorph(FbxScene* pScene, CModel* pmodel);

static void AnimateSkeletonOfBVH( FbxScene* pScene );
static void AnimateBoneOfBVHReq( CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer );


static FbxTexture*  CreateTexture( FbxManager* pSdkManager, CMQOMaterial* mqomat );
static int ExistBoneInInf( int boneno, CMQOObject* srcobj, int* dstclusterno );

static int MapShapesOnMesh( FbxScene* pScene, FbxNode* pNode, CModel* pmodel, CMQOObject* curobj, BLSINDEX* blsindex );
static int MapTargetShape( FbxBlendShapeChannel* lBlendShapeChannel, FbxScene* pScene, CMQOObject* curobj, ChaVector3* targetv, int targetcnt );

static void CreateDummyInfDataReq(CFBXBone* fbxbone, FbxManager*& pSdkManager, FbxScene*& pScene, FbxNode* lMesh, FbxSkin* lSkin, int bonecnt);
static FbxNode* CreateDummyFbxMesh(FbxManager* pSdkManager, FbxScene* pScene);
static void LinkDummyMeshToSkeleton(CFBXBone* fbxbone, FbxSkin* lSkin, FbxScene* pScene, FbxNode* pMesh, int bonecnt);

static FbxAMatrix CalcBindMatrix(CFBXBone* fbxbone);

static int WriteFBXAnimTra(CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer, int curmotid, int maxframe, int axiskind);
static int WriteFBXAnimRot(CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer, int curmotid, int maxframe, int axiskind);
static int WriteFBXAnimTraOfBVH(CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer, int axiskind, int zeroflag);
static int WriteFBXAnimRotOfBVH(CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer, int axiskind, int zeroflag);


#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pSdkManager->GetIOSettings()))
#endif


extern float g_tmpbvhfilter;



float FlClamp( float srcval, float minval, float maxval )
{
	float retval = srcval;

	if( srcval < minval ){
		return minval;
	}else if( srcval > maxval ){
		return maxval;
	}else{
		return retval;
	}
}

int InitializeSdkObjects()
{
    // The first thing to do is to create the FBX SDK manager which is the 
    // object allocator for almost all the classes in the SDK.
    s_pSdkManager = FbxManager::Create();

    if (!s_pSdkManager)
    {
		_ASSERT( 0 );
		return 1;
	}

	// create an IOSettings object
	FbxIOSettings * ios = FbxIOSettings::Create(s_pSdkManager, IOSROOT );
	s_pSdkManager->SetIOSettings(ios);

	// Load plugins from the executable directory
	FbxString lPath = FbxGetApplicationDirectory();
#if defined(KARCH_ENV_WIN)
	FbxString lExtension = "dll";
#elif defined(KARCH_ENV_MACOSX)
	FbxString lExtension = "dylib";
#elif defined(KARCH_ENV_LINUX)
	FbxString lExtension = "so";
#endif
	s_pSdkManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

	return 0;
}

int DestroySdkObjects()
{
    // Delete the FBX SDK manager. All the objects that have been allocated 
    // using the FBX SDK manager and that haven't been explicitly destroyed 
    // are automatically destroyed at the same time.
    if (s_pSdkManager) s_pSdkManager->Destroy();
    s_pSdkManager = 0;


	return 0;
}

bool SaveScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
{
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    FbxExporter* lExporter = FbxExporter::Create(pSdkManager, "");

    if( pFileFormat < 0 || pFileFormat >= pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount() )
    {
        // Write in fall back format in less no ASCII format found
        pFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeWriterFormat();

		/*
        //Try to export in ASCII if possible
        int lFormatIndex, lFormatCount = pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount();

        for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
        {
            if (pSdkManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
            {
                FbxString lDesc =pSdkManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                char *lASCII = "ascii";
                if (lDesc.Find(lASCII)>=0)
                {
                    pFileFormat = lFormatIndex;
                    break;
                }
            }
        }
		*/
    }

    // Set the export states. By default, the export states are always set to 
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
    // shows how to change these states.

    IOS_REF.SetBoolProp(EXP_FBX_MATERIAL,        true);
    IOS_REF.SetBoolProp(EXP_FBX_TEXTURE,         true);
    IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED,        pEmbedMedia);
    IOS_REF.SetBoolProp(EXP_FBX_SHAPE,           true);
    IOS_REF.SetBoolProp(EXP_FBX_GOBO,            true);
    IOS_REF.SetBoolProp(EXP_FBX_ANIMATION,       true);
    IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Initialize the exporter by providing a filename.
    if(lExporter->Initialize(pFilename, pFileFormat, pSdkManager->GetIOSettings()) == false)
    {
        printf("Call to FbxExporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", lExporter->GetLastErrorString());
        return false;
    }

    FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
    printf("FBX version number for this version of the FBX SDK is %d.%d.%d\n\n", lMajor, lMinor, lRevision);

    // Export the scene.
    lStatus = lExporter->Export(pScene); 

    // Destroy the exporter.
    lExporter->Destroy();
    return lStatus;
}

int BVH2FBXFile(FbxManager* psdk, CBVHFile* pbvhfile, char* pfilename)
{
	s_bvhflag = 1;
	s_behead = pbvhfile->GetBVHElem();
	s_model = 0;

	s_pSdkManager = psdk;

	if( s_fbxbone ){
		DestroyFBXBoneReq( s_fbxbone );
		s_fbxbone = 0;
	}


	s_firstoutmot = -1;

    bool lResult;
    // Create the entity that will hold the scene.
	FbxScene* lScene;
    lScene = FbxScene::Create(s_pSdkManager,"");
    // Create the scene.
    lResult = CreateBVHScene( s_pSdkManager, lScene );// create, set
    if(lResult == false)
    {
		_ASSERT( 0 );
		return 1;
    }


    // Save the scene.
    // The example can take an output file name as an argument.
    lResult = SaveScene( s_pSdkManager, lScene, pfilename );// write
    if(lResult == false)
    {
		_ASSERT( 0 );
		return 1;
	}

	if( s_fbxbone ){
		DestroyFBXBoneReq( s_fbxbone );
		s_fbxbone = 0;
	}



	s_bvhflag = 0;

	return 0;
}


int WriteFBXFile( FbxManager* psdk, CModel* pmodel, char* pfilename )
{

	s_bvhflag = 0;
	s_pSdkManager = psdk;
	s_model = pmodel;
	if( s_fbxbone ){
		DestroyFBXBoneReq( s_fbxbone );
		s_fbxbone = 0;
	}


	s_firstoutmot = -1;

	CallF( pmodel->MakeEnglishName(), return 1 );

    bool lResult;

    // Create the entity that will hold the scene.
	FbxScene* lScene;
    lScene = FbxScene::Create(s_pSdkManager,"");

    // Create the scene.
    lResult = CreateScene( s_pSdkManager, lScene, pmodel );

    if(lResult == false)
    {
		_ASSERT( 0 );
		return 1;
    }

    // Save the scene.

    // The example can take an output file name as an argument.
    lResult = SaveScene( s_pSdkManager, lScene, pfilename );

    if(lResult == false)
    {
		_ASSERT( 0 );
		return 1;
	}

	if( s_fbxbone ){
		DestroyFBXBoneReq( s_fbxbone );
		s_fbxbone = 0;
	}


	return 0;
}

bool CreateBVHScene( FbxManager *pSdkManager, FbxScene* pScene )
{
    // create scene info
    FbxDocumentInfo* sceneInfo = FbxDocumentInfo::Create(pSdkManager,"SceneInfo");
	sceneInfo->mTitle = "scene made by MameBake3D";
	sceneInfo->mSubject = "skinmesh and animation";
	sceneInfo->mAuthor = "OchakkoLab";
	sceneInfo->mRevision = "rev. 2.2";
	sceneInfo->mKeywords = "BVH animation";
	sceneInfo->mComment = "no particular comments required.";

    // we need to add the sceneInfo before calling AddThumbNailToScene because
    // that function is asking the scene for the sceneInfo.
    pScene->SetSceneInfo(sceneInfo);

//    AddThumbnailToScene(pScene);

    FbxNode* lRootNode = pScene->GetRootNode();

	s_fbxbone = CreateFBXBoneOfBVH( pScene );
	lRootNode->AddChild( s_fbxbone->GetSkelNode() );
	if( !s_fbxbone ){
		_ASSERT( 0 );
		return 0;
	}

/***
	BLSINDEX blsindex;
	ZeroMemory( &blsindex, sizeof( BLSINDEX ) );
	s_blsinfo.clear();
	map<int,CMQOObject*>::iterator itrobj;
	for( itrobj = pmodel->m_object.begin(); itrobj != pmodel->m_object.end(); itrobj++ ){
		CMQOObject* curobj = itrobj->second;
		FbxNode* lMesh = CreateFbxMesh( pSdkManager, pScene, pmodel, curobj );
		if( !(curobj->m_findshape.empty()) ){
			MapShapesOnMesh( pScene, lMesh, pmodel, curobj, &blsindex );
		}
		lRootNode->AddChild(lMesh);

		FbxGeometry* lMeshAttribute = (FbxGeometry*)lMesh->GetNodeAttribute();
		FbxSkin* lSkin = FbxSkin::Create(pScene, "");
		LinkMeshToSkeletonReq( s_fbxbone, lSkin, pScene, lMesh, curobj, pmodel);
		lMeshAttribute->AddDeformer(lSkin);
	}
***/

	FbxNode* lMesh = CreateDummyFbxMesh(pSdkManager, pScene);
	lRootNode->AddChild(lMesh);
	FbxGeometry* lMeshAttribute = (FbxGeometry*)lMesh->GetNodeAttribute();
	FbxSkin* lSkin = FbxSkin::Create(pScene, "");
	CreateDummyInfDataReq(s_fbxbone, pSdkManager, pScene, lMesh, lSkin, 0);
	lMeshAttribute->AddDeformer(lSkin);


//    StoreRestPose(pScene, lSkeletonRoot);


    AnimateSkeletonOfBVH( pScene );


	s_firstoutmot = 1;

	WriteBindPose(pScene, 1);

	if( s_ai ){
		free( s_ai );
		s_ai = 0;
	}
	s_ainum = 0;

    return true;
}



bool CreateScene( FbxManager *pSdkManager, FbxScene* pScene, CModel* pmodel )
{
    // create scene info
    FbxDocumentInfo* sceneInfo = FbxDocumentInfo::Create(pSdkManager,"SceneInfo");
    sceneInfo->mTitle = "scene made by MameBake3D";
    sceneInfo->mSubject = "skinmesh and animation";
    sceneInfo->mAuthor = "OchakkoLab";
    sceneInfo->mRevision = "rev. 2.2";
    sceneInfo->mKeywords = "skinmesh animation";
    sceneInfo->mComment = "no particular comments required.";

    // we need to add the sceneInfo before calling AddThumbNailToScene because
    // that function is asking the scene for the sceneInfo.
    pScene->SetSceneInfo(sceneInfo);

//    AddThumbnailToScene(pScene);

    FbxNode* lRootNode = pScene->GetRootNode();

	s_fbxbone = CreateFBXBone(pScene, pmodel);
	lRootNode->AddChild( s_fbxbone->GetSkelNode() );
	if( !s_fbxbone ){
		_ASSERT( 0 );
		return 0;
	}

	BLSINDEX blsindex;
	ZeroMemory( &blsindex, sizeof( BLSINDEX ) );
	s_blsinfo.clear();
	map<int,CMQOObject*>::iterator itrobj;
	for( itrobj = pmodel->GetMqoObjectBegin(); itrobj != pmodel->GetMqoObjectEnd(); itrobj++ ){
		CMQOObject* curobj = itrobj->second;

		const char* dummynameptr = strstr(curobj->GetName(), "_ND_dtri");
		if (dummynameptr){
			continue;
		}

		FbxNode* lMesh = CreateFbxMesh( pSdkManager, pScene, pmodel, curobj );
		if (!lMesh){
			continue;//RootNode
		}
		//if( !(curobj->EmptyFindShape()) ){
		//	MapShapesOnMesh( pScene, lMesh, pmodel, curobj, &blsindex );
		//}
		lRootNode->AddChild(lMesh);

		FbxGeometry* lMeshAttribute = (FbxGeometry*)lMesh->GetNodeAttribute();
		FbxSkin* lSkin = FbxSkin::Create(pScene, "");


		int* psetflag = (int*)malloc( curobj->GetPm4()->GetOrgPointNum() * sizeof(int));
		_ASSERT(psetflag);
		ZeroMemory(psetflag, curobj->GetPm4()->GetOrgPointNum() * sizeof(int));
		CBone** ppsetbone = (CBone**)malloc(s_model->GetBoneListSize() * sizeof(CBone*));
		if (!ppsetbone){
			_ASSERT(0);
			return 1;
		}
		_ASSERT(ppsetbone);
		ZeroMemory(ppsetbone, s_model->GetBoneListSize() * sizeof(CBone*));

		s_linkdirty.clear();
		LinkMeshToSkeletonReq(s_fbxbone, lSkin, pScene, lMesh, curobj, curobj->GetPm4(), psetflag, ppsetbone);
		LinkToTopBone(lSkin, pScene, curobj, curobj->GetPm4(), psetflag);
		
		free(psetflag);
		psetflag = 0;
		free(ppsetbone);
		ppsetbone = 0;

		lMeshAttribute->AddDeformer(lSkin);
	}


	FbxNode* lMesh = CreateDummyFbxMesh(pSdkManager, pScene);
	lRootNode->AddChild(lMesh);
	FbxGeometry* lMeshAttribute = (FbxGeometry*)lMesh->GetNodeAttribute();
	FbxSkin* lSkin = FbxSkin::Create(pScene, "");
	CreateDummyInfDataReq(s_fbxbone, pSdkManager, pScene, lMesh, lSkin, 0);
	lMeshAttribute->AddDeformer(lSkin);


//    StoreRestPose(pScene, lSkeletonRoot);


    AnimateSkeleton(pScene, pmodel);
	AnimateMorph(pScene, pmodel);

	WriteBindPose(pScene);

	if( s_ai ){
		free( s_ai );
		s_ai = 0;
	}
	s_ainum = 0;

    return true;
}




//void MapBoxShape(FbxScene* pScene, FbxBlendShapeChannel* lBlendShapeChannel)
//int MapTargetShape( FbxBlendShapeChannel* lBlendShapeChannel, FbxScene* pScene, CMQOObject* curobj, CMQOObject* curtarget, MATERIALBLOCK* pmb, int mbno )
int MapTargetShape( FbxBlendShapeChannel* lBlendShapeChannel, FbxScene* pScene, CMQOObject* curobj, ChaVector3* targetv, int targetcnt )
{
	char shapename[256]={0};
	sprintf_s( shapename, 256, "SHAPE_%s_%d", curobj->GetEngName(), targetcnt );
    FbxShape* lShape = FbxShape::Create(pScene,shapename);

	CPolyMesh4* basepm4 = curobj->GetPm4();
	_ASSERT( basepm4 );

	int shapevertnum = basepm4->GetFaceNum() * 3;
	lShape->InitControlPoints( shapevertnum );
    FbxVector4* lVector4 = lShape->GetControlPoints();

	int shapevno;
	for( shapevno = 0; shapevno < shapevertnum; shapevno++ ){
		int orgvno = *( basepm4->GetDispIndex() + shapevno );

		ChaVector3 shapev = *( targetv + orgvno );
		lVector4[ shapevno ].Set( shapev.x, shapev.y, shapev.z, 1.0 );
	}
	lBlendShapeChannel->AddTargetShape(lShape);
	return 0;
}


int MapShapesOnMesh( FbxScene* pScene, FbxNode* pNode, CModel* pmodel, CMQOObject* curobj, BLSINDEX* blsindex )
{
	char blsname[256] = {0};
	int mbno = 0;//mqoでつかう
	sprintf_s( blsname, 256, "BLS_%s_%d", curobj->GetEngName(), mbno );
	FbxBlendShape* lBlendShape = FbxBlendShape::Create(pScene, blsname);

	(blsindex->channelno) = 0;

	int targetcnt = 0;
	map<string,ChaVector3*> tmpmap;
	curobj->GetShapeVert2( tmpmap );
	map<string,ChaVector3*>::iterator itrshapev;
	for( itrshapev = tmpmap.begin(); itrshapev != tmpmap.end(); itrshapev++ ){
		string targetname = itrshapev->first;
		ChaVector3* curv = itrshapev->second;
		if( curv ){
			string curshapename = itrshapev->first;

			char blscname[256] = {0};
			strcpy_s( blscname, 256, curshapename.c_str() );
			FbxBlendShapeChannel* lBlendShapeChannel = FbxBlendShapeChannel::Create(pScene,blscname);
			MapTargetShape( lBlendShapeChannel, pScene, curobj, curv, targetcnt );
			lBlendShape->AddBlendShapeChannel(lBlendShapeChannel);


			BLSINFO blsinfo;
			ZeroMemory( &blsinfo, sizeof( BLSINFO ) );

			blsinfo.blsindex = *blsindex;
			blsinfo.base = curobj;
			blsinfo.targetname = targetname;
			blsinfo.basenode = pNode;

			s_blsinfo[ blsindex->serialno ] = blsinfo;

			(blsindex->channelno)++;
			(blsindex->serialno)++;

			targetcnt++;
		}
	}


	FbxGeometry* lGeometry = pNode->GetGeometry();
	lGeometry->AddDeformer(lBlendShape);
	return 0;
};


FbxNode* CreateFbxMesh(FbxManager* pSdkManager, FbxScene* pScene, CModel* pmodel, CMQOObject* curobj)
{
	CPolyMesh4* pm4 = curobj->GetPm4();
	_ASSERT( pm4 );
	CMQOMaterial* mqomat = curobj->GetMaterialBegin()->second;
	_ASSERT( mqomat );
	if (!pm4->GetOrgPointBuf()){
		return 0;// RootNode
	}

	char meshname[256] = {0};

	sprintf_s( meshname, 256, "%s", curobj->GetEngName() );
	int facenum = pm4->GetFaceNum();

	FbxMesh* lMesh = FbxMesh::Create( pScene, meshname );
	s_doublevertices = 0;
	lMesh->InitControlPoints(pm4->GetOrgPointNum());
	/*
	if (pm4->GetOrgPointNum() < (facenum * 3)){
		s_doublevertices = 1;
		lMesh->InitControlPoints(pm4->GetOrgPointNum() * 2);
	}
	else{
		s_doublevertices = 0;
		lMesh->InitControlPoints(pm4->GetOrgPointNum());
	}
	*/

	FbxVector4* lcp = lMesh->GetControlPoints();

	FbxGeometryElementNormal* lElementNormal= lMesh->CreateElementNormal();
	lElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
	lElementNormal->SetReferenceMode(FbxGeometryElement::eDirect);

	FbxGeometryElementUV* lUVDiffuseElement = lMesh->CreateElementUV( "DiffuseUV");
	_ASSERT( lUVDiffuseElement != NULL);
	//lUVDiffuseElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
	//lUVDiffuseElement->SetReferenceMode(FbxGeometryElement::eDirect);
	lUVDiffuseElement->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	lUVDiffuseElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	int vsetno = 0;
	for (vsetno = 0; vsetno < pm4->GetOrgPointNum(); vsetno++){
		ChaVector3* curv = pm4->GetOrgPointBuf() + vsetno;
		*(lcp + vsetno) = FbxVector4(curv->x, curv->y, curv->z, 1.0f);

		ChaVector3 curn = pm4->GetNormalByControlPointNo(vsetno);
		FbxVector4 fbxn = FbxVector4(curn.x, curn.y, curn.z, 0.0f);
		lElementNormal->GetDirectArray().Add(fbxn);
	}
	/*
	if (s_doublevertices == 1){
		for (vsetno = 0; vsetno < pm4->GetOrgPointNum(); vsetno++){
			ChaVector3* curv = pm4->GetOrgPointBuf() + vsetno;
			*(lcp + vsetno + pm4->GetOrgPointNum()) = FbxVector4(curv->x, curv->y, curv->z, 1.0f);

			ChaVector3 curn = pm4->GetNormalByControlPointNo(vsetno);
			FbxVector4 fbxn = FbxVector4(curn.x, curn.y, curn.z, 0.0f);
			lElementNormal->GetDirectArray().Add(fbxn);
		}
	}
	*/

	vsetno = 0;
	int faceno;
	for (faceno = 0; faceno < facenum; faceno++){
		PM3DISPV* curdispv = pm4->GetPm3Disp() + faceno * 3;
		int vcnt;
		for (vcnt = 0; vcnt < 3; vcnt++){
			//PM3DISPV* curv = curdispv + vcnt;
			PM3DISPV* curv = curdispv + s_invindex[vcnt];
			FbxVector2 fbxuv = FbxVector2(curv->uv.x, 1.0f - curv->uv.y);
			//FbxVector2 fbxuv = FbxVector2(curv->uv.x, curv->uv.y);
			lUVDiffuseElement->GetDirectArray().Add(fbxuv);

			vsetno++;
		}
	}
	lUVDiffuseElement->GetIndexArray().SetCount(facenum * 3);


	vsetno = 0;
	for( faceno = 0; faceno < facenum; faceno++ ){
		CMQOFace* curface = pm4->GetTriFace() + faceno;
		int vno[3];
		vno[0] = curface->GetIndex(0);
		vno[2] = curface->GetIndex(2);
		vno[1] = curface->GetIndex(1);

		_ASSERT((vno[0] >= 0) && (vno[0] < pm4->GetOrgPointNum()));
		_ASSERT((vno[1] >= 0) && (vno[1] < pm4->GetOrgPointNum()));
		_ASSERT((vno[2] >= 0) && (vno[2] < pm4->GetOrgPointNum()));

		lMesh->BeginPolygon(-1, -1, -1, false);

		int vcnt;
		for( vcnt = 0; vcnt < 3; vcnt++ ){
			lMesh->AddPolygon(vno[vcnt]);
			lUVDiffuseElement->GetIndexArray().SetAt(vsetno, vsetno);
			vsetno++;
		}
		lMesh->EndPolygon ();
	}


	// create a FbxNode
//		FbxNode* lNode = FbxNode::Create(pSdkManager,pName);
	FbxNode* lNode = FbxNode::Create( pScene, meshname );
	// set the node attribute
	lNode->SetNodeAttribute(lMesh);
	// set the shading mode to view texture
	//lNode->SetShadingMode(FbxNode::eTextureShading);
	// rotate the plane
	lNode->LclRotation.Set(FbxVector4(0, 0, 0));

	// Set material mapping.
	FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
	lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
	lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
	if( !lMesh->GetElementMaterial(0) ){
		_ASSERT( 0 );
		return NULL;
	}

	// add material to the node. 
	// the material can't in different document with the geometry node or in sub-document
	// we create a simple material here which belong to main document
	static int s_matcnt = 0;
	s_matcnt++;

	char matname[256];
	sprintf_s( matname, 256, "%s_%d", mqomat->GetName(), s_matcnt );
	//FbxString lMaterialName = mqomat->name;
	FbxString lMaterialName = matname;
	FbxString lShadingName  = "Phong";
	FbxSurfacePhong* lMaterial = FbxSurfacePhong::Create( pScene, lMaterialName.Buffer() );

	lMaterial->Diffuse.Set(FbxDouble3(mqomat->GetDif4F().x, mqomat->GetDif4F().y, mqomat->GetDif4F().z));
    lMaterial->Emissive.Set(FbxDouble3(mqomat->GetEmi3F().x, mqomat->GetEmi3F().y, mqomat->GetEmi3F().z));
    lMaterial->Ambient.Set(FbxDouble3(mqomat->GetAmb3F().x, mqomat->GetAmb3F().y, mqomat->GetAmb3F().z));
    lMaterial->AmbientFactor.Set(1.0);
	FbxTexture* curtex = CreateTexture(pSdkManager, mqomat);
	if( curtex ){
		lMaterial->Diffuse.ConnectSrcObject( curtex );
		lNode->SetShadingMode(FbxNode::eTextureShading);
	}
	else{
		lNode->SetShadingMode(FbxNode::eHardShading);
	}

    lMaterial->DiffuseFactor.Set(1.0);
    lMaterial->TransparencyFactor.Set(mqomat->GetDif4F().w);
    lMaterial->ShadingModel.Set(lShadingName);
    lMaterial->Shininess.Set(0.5);
    lMaterial->Specular.Set(FbxDouble3(mqomat->GetSpc3F().x, mqomat->GetSpc3F().y, mqomat->GetSpc3F().z));
    lMaterial->SpecularFactor.Set(0.3);

	lNode->AddMaterial(lMaterial);
	// We are in eByPolygon, so there's only need for index (a plane has 1 polygon).
	lMaterialElement->GetIndexArray().SetCount( lMesh->GetPolygonCount() );
	// Set the Index to the material
	//for(int i=0; i<lMesh->GetPolygonCount(); ++i){
	for (int i = 0; i<lMesh->GetPolygonCount(); i++){
		lMaterialElement->GetIndexArray().SetAt(i,0);
	}

	return lNode;
}

FbxTexture*  CreateTexture(FbxManager* pSdkManager, CMQOMaterial* mqomat)
{
	if( !*(mqomat->GetTex()) ){
		return NULL;
	}

    FbxFileTexture* lTexture = FbxFileTexture::Create(pSdkManager,"");
    FbxString lTexPath = mqomat->GetTex();

    // Set texture properties.
    lTexture->SetFileName(lTexPath.Buffer());
    //lTexture->SetName("Diffuse Texture");
	lTexture->SetName(mqomat->GetTex());
    lTexture->SetTextureUse(FbxTexture::eStandard);
    lTexture->SetMappingType(FbxTexture::eUV);
    lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
    lTexture->SetSwapUV(false);
    lTexture->SetAlphaSource (FbxTexture::eNone);
    lTexture->SetTranslation(0.0, 0.0);
    lTexture->SetScale(1.0, 1.0);
    lTexture->SetRotation(0.0, 0.0);

    return lTexture;
}

void LinkToTopBone(FbxSkin* lSkin, FbxScene* pScene, CMQOObject* curobj, CPolyMesh4* pm4, int* psetflag)
{

	FbxAMatrix lXMatrix;
	FbxNode* lSkel;
	FbxCluster *lCluster = 0;

	if (s_firsttopbone){
		//lSkel = s_fbxbone->skelnode;
		lSkel = s_firsttopbone->GetSkelNode();
		if (!lSkel){
			_ASSERT(0);
			return;
		}
		CBone* curbone = s_firsttopbone->GetBone();
		_ASSERT(curbone);
		if (curbone){

			map<CBone*, map<int, int>>::iterator itrlinkdirty;
			map<int, int> mapdirty;
			s_linkdirty[curbone] = mapdirty;
			itrlinkdirty = s_linkdirty.find(curbone);

			FbxCluster *lCluster = 0;
			int curclusterno = -1;
			int infdirty = ExistBoneInInf(curbone->GetBoneNo(), curobj, &curclusterno);

			if (infdirty){
				lCluster = FbxCluster::Create(pScene, "");
				lCluster->SetLink(lSkel);
				lCluster->SetLinkMode(FbxCluster::eTotalOne);

				//*(ppsetbone + curbone->GetBoneNo()) = curbone;//!!!!!!!!!

				int vsetno = 0;
				int fno;
				for (fno = 0; fno < pm4->GetFaceNum(); fno++){
					CMQOFace* curface = pm4->GetTriFace() + fno;
					int vno[3];
					vno[0] = curface->GetIndex(0);
					vno[2] = curface->GetIndex(2);
					vno[1] = curface->GetIndex(1);

					_ASSERT((vno[0] >= 0) && (vno[0] < pm4->GetOrgPointNum()));
					_ASSERT((vno[1] >= 0) && (vno[1] < pm4->GetOrgPointNum()));
					_ASSERT((vno[2] >= 0) && (vno[2] < pm4->GetOrgPointNum()));

					int vcnt;
					for (vcnt = 0; vcnt < 3; vcnt++){
						CInfBone* curib = pm4->GetInfBone() + vno[vcnt];
						int ieno = curib->ExistBone(curobj, curclusterno);
						if (ieno >= 0){
							map<int, int>::iterator itrmapdirty;
							itrmapdirty = itrlinkdirty->second.find(vno[vcnt]);
							if (itrmapdirty == itrlinkdirty->second.end()){
								INFDATA* infd = curib->GetInfData(curobj);
								if (infd){
									lCluster->AddControlPointIndex(vno[vcnt], (double)(infd->m_infelem[ieno].dispinf));
								}
								*(psetflag + vno[vcnt]) = 1;
								itrlinkdirty->second[vno[vcnt]] = 1;
							}
							else if ((int)(itrmapdirty->second) != 1){
								INFDATA* infd = curib->GetInfData(curobj);
								if (infd){
									lCluster->AddControlPointIndex(vno[vcnt], (double)(infd->m_infelem[ieno].dispinf));
								}
								*(psetflag + vno[vcnt]) = 1;
								itrlinkdirty->second[vno[vcnt]] = 1;
							}
						}
						vsetno++;
					}
				}
			}


			int findnotdirty = 0;
			int chkvno;
			for (chkvno = 0; chkvno < pm4->GetOrgPointNum(); chkvno++){
				if (*(psetflag + chkvno) == 0){
					findnotdirty = 1;
					break;
				}
			}

			if (findnotdirty == 1){
				if (!lCluster){
					lCluster = FbxCluster::Create(pScene, "");
					lCluster->SetLink(lSkel);
					lCluster->SetLinkMode(FbxCluster::eTotalOne);
				}

				int setv;
				for (setv = 0; setv < pm4->GetOrgPointNum(); setv++){
					if (*(psetflag + setv) == 0){
						lCluster->AddControlPointIndex(setv, 1.0);
						*(psetflag + setv) = 1;
					}
				}
			}

			if (s_doublevertices == 1){//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if (!lCluster){
					lCluster = FbxCluster::Create(pScene, "");
					lCluster->SetLink(lSkel);
					lCluster->SetLinkMode(FbxCluster::eTotalOne);
				}

				int setv;
				for (setv = 0; setv < pm4->GetOrgPointNum(); setv++){
					lCluster->AddControlPointIndex(setv + pm4->GetOrgPointNum(), 1.0);
				}
			}

			if (lCluster){
				ChaVector3 pos = ChaVector3(0.0f, 0.0f, 0.0f);
				pos.x = s_firsttopbone->GetBone()->GetJointFPos().x;
				pos.y = s_firsttopbone->GetBone()->GetJointFPos().y;
				pos.z = s_firsttopbone->GetBone()->GetJointFPos().z;

				//KFbxScene* lScene = pMesh->GetScene();
				//lXMatrix = pMesh->EvaluateGlobalTransform();
				//lCluster->SetTransformMatrix(lXMatrix);
				lXMatrix.SetIdentity();
				lXMatrix[3][0] = -pos.x;
				lXMatrix[3][1] = -pos.y;
				lXMatrix[3][2] = pos.z;
				lCluster->SetTransformMatrix(lXMatrix);

				//lXMatrix = lSkel->EvaluateGlobalTransform();
				//lCluster->SetTransformLinkMatrix(lXMatrix);
				lXMatrix.SetIdentity();
				lCluster->SetTransformLinkMatrix(lXMatrix);

				lSkin->AddCluster(lCluster);
			}

			/*
			if (lCluster){
			FbxAMatrix lXMatrix;
			lXMatrix.SetIdentity();
			lCluster->SetTransformMatrix(lXMatrix);

			FbxAMatrix lXMatrix2;
			lXMatrix2 = lSkel->EvaluateGlobalTransform();
			lCluster->SetTransformLinkMatrix(lXMatrix2);

			lSkin->AddCluster(lCluster);
			}
			*/
		}
	}
}



void LinkMeshToSkeletonReq(CFBXBone* fbxbone, FbxSkin* lSkin, FbxScene* pScene, FbxNode* pMesh, CMQOObject* curobj, CPolyMesh4* pm4, int* psetflag, CBone** ppsetbone)
{
	_ASSERT( pm4 );
	FbxGeometry* lMeshAttribute;
    FbxNode* lSkel;

	lMeshAttribute = (FbxGeometry*)pMesh->GetNodeAttribute();

	if ((fbxbone != s_fbxbone) && (fbxbone->GetBone() != s_firsttopbone->GetBone()) && ((fbxbone->GetType() == FB_NORMAL) || (fbxbone->GetType() == FB_BUNKI_CHIL))){
		CBone* curbone = fbxbone->GetBone();
		_ASSERT(curbone);
		if( curbone ){
			lSkel = fbxbone->GetSkelNode();
			if( !lSkel ){
				_ASSERT( 0 );
				return;
			}

			int doneflag = 0;
			_ASSERT(curbone->GetBoneNo() >= 0);
			_ASSERT(curbone->GetBoneNo() < s_model->GetBoneListSize());
			if (*(ppsetbone + curbone->GetBoneNo()) != 0){
				doneflag = 1;
			}


			if (doneflag == 0){
				map<int, int> mapdirty;
				s_linkdirty[curbone] = mapdirty;
				map<CBone*, map<int, int>>::iterator itrlinkdirty;
				itrlinkdirty = s_linkdirty.find(curbone);

				int curclusterno = -1;
				int infdirty = ExistBoneInInf(curbone->GetBoneNo(), curobj, &curclusterno);

				if (infdirty){
					FbxCluster *lCluster = FbxCluster::Create(pScene, "");
					lCluster->SetLink(lSkel);
					lCluster->SetLinkMode(FbxCluster::eTotalOne);

					_ASSERT(curbone->GetBoneNo() >= 0);
					_ASSERT(curbone->GetBoneNo() < s_model->GetBoneListSize());
					*(ppsetbone + curbone->GetBoneNo()) = curbone;//!!!!!!!!!

					int vsetno = 0;
					int fno;
					for (fno = 0; fno < pm4->GetFaceNum(); fno++){
						CMQOFace* curface = pm4->GetTriFace() + fno;
						int vno[3];
						vno[0] = curface->GetIndex(0);
						vno[2] = curface->GetIndex(2);
						vno[1] = curface->GetIndex(1);

						_ASSERT((vno[0] >= 0) && (vno[0] < pm4->GetOrgPointNum()));
						_ASSERT((vno[1] >= 0) && (vno[1] < pm4->GetOrgPointNum()));
						_ASSERT((vno[2] >= 0) && (vno[2] < pm4->GetOrgPointNum()));

						int vcnt;
						for (vcnt = 0; vcnt < 3; vcnt++){
							CInfBone* curib = pm4->GetInfBone() + vno[vcnt];
							int ieno = curib->ExistBone(curobj, curclusterno);
							if (ieno >= 0){
								map<int, int>::iterator itrmapdirty;
								itrmapdirty = itrlinkdirty->second.find(vno[vcnt]);
								if (itrmapdirty == itrlinkdirty->second.end()){
									INFDATA* infd = curib->GetInfData(curobj);
									if (infd){
										lCluster->AddControlPointIndex(vno[vcnt], (double)(infd->m_infelem[ieno].dispinf));
									}
									*(psetflag + vno[vcnt]) = 1;
									itrlinkdirty->second[vno[vcnt]] = 1;
								}
								else if ((int)(itrmapdirty->second) != 1){
									INFDATA* infd = curib->GetInfData(curobj);
									if (infd){
										lCluster->AddControlPointIndex(vno[vcnt], (double)(infd->m_infelem[ieno].dispinf));
									}
									*(psetflag + vno[vcnt]) = 1;
									itrlinkdirty->second[vno[vcnt]] = 1;
								}
							}
							vsetno++;
						}
					}


					FbxAMatrix lXMatrix;
					lXMatrix = pMesh->EvaluateGlobalTransform();
					lCluster->SetTransformMatrix(lXMatrix);


					FbxAMatrix lXMatrix2;
					lXMatrix2 = lSkel->EvaluateGlobalTransform();
					lCluster->SetTransformLinkMatrix(lXMatrix2);


					lSkin->AddCluster(lCluster);
				}
			}
		}
	}

	if( fbxbone->GetChild() ){
		LinkMeshToSkeletonReq(fbxbone->GetChild(), lSkin, pScene, pMesh, curobj, pm4, psetflag, ppsetbone);
	}
	if( fbxbone->GetBrother() ){
		LinkMeshToSkeletonReq(fbxbone->GetBrother(), lSkin, pScene, pMesh, curobj, pm4, psetflag, ppsetbone);
	}

}

/*
void SkinToSkeletonReq(CFBXBone* fbxbone, FbxSkin* lSkin, FbxScene* pScene)
{
	//CPolyMesh4* pm4 = curobj->GetPm4();
	//_ASSERT(pm4);
	FbxGeometry* lMeshAttribute = 0;
	FbxAMatrix lXMatrix;
	FbxNode* lSkel;

	//lMeshAttribute = (FbxGeometry*)pMesh->GetNodeAttribute();

	if (fbxbone->GetType() == FB_NORMAL){
		CBone* curbone = fbxbone->GetBone();

		if (curbone){
			lSkel = fbxbone->GetSkelNode();
			if (!lSkel){
				_ASSERT(0);
				return;
			}

			int curclusterno = -1;

			FbxCluster *lCluster = FbxCluster::Create(pScene, "");
			lCluster->SetLink(lSkel);
			lCluster->SetLinkMode(FbxCluster::eTotalOne);

			lCluster->AddControlPointIndex(0, 1.0);
			
			lXMatrix = lSkel->EvaluateGlobalTransform();//!!!!!!!!!!!!
			lCluster->SetTransformMatrix(lXMatrix);


			lSkel    ->AddCluster(lCluster);
		}
	}

	if (fbxbone->GetChild()){
		LinkMeshToSkeletonReq(fbxbone->GetChild(), lSkin, pScene, pMesh, curobj, pmodel);
	}
	if (fbxbone->GetBrother()){
		LinkMeshToSkeletonReq(fbxbone->GetBrother(), lSkin, pScene, pMesh, curobj, pmodel);
	}

}
*/

void AnimateSkeletonOfBVH( FbxScene* pScene )
{
    FbxString lAnimStackName;
    FbxTime lTime;
    int lKeyIndex = 0;

	
	lAnimStackName = "bvh_animation_nyan";
	FbxAnimStack* lAnimStack = FbxAnimStack::Create(pScene, lAnimStackName);
	FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(pScene, "Base Layer");
    lAnimStack->AddMember(lAnimLayer);

	s_firstanimout = 1;
	AnimateBoneOfBVHReq( s_fbxbone, lAnimLayer );

	pScene->GetRootNode()->ConvertPivotAnimationRecursive(lAnimStackName, s_convPivot, 30.0, true);

}
void AnimateBoneOfBVHReq( CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer )
{
    FbxNode* lSkel = 0;

	if (fbxbone){
		CBVHElem* curbe = fbxbone->GetBvhElem();
		_ASSERT(curbe);
		if (curbe){

			lSkel = fbxbone->GetSkelNode();
			if (!lSkel){
				_ASSERT(0);
				return;
			}

			EFbxRotationOrder lRotationOrderSrc = eEulerZXY;
			EFbxRotationOrder lRotationOrderDst = eEulerXYZ;
			lSkel->GetRotationOrder(FbxNode::eSourcePivot, lRotationOrderSrc);
			lSkel->GetRotationOrder(FbxNode::eDestinationPivot, lRotationOrderDst);
			//lSkel->SetRotationOrder(FbxNode::eSourcePivot, eEulerZXY);
			//lSkel->SetRotationOrder(FbxNode::eDestinationPivot, eEulerXYZ);
			lSkel->SetRotationOrder(FbxNode::eSourcePivot, eEulerXYZ);
			lSkel->SetRotationOrder(FbxNode::eDestinationPivot, eEulerZXY);
			s_convPivot = FbxNode::eSourcePivot;

			int zeroflag;

			
			if ((fbxbone->GetType() == FB_BUNKI_CHIL) || (fbxbone->GetType() == FB_ROOT)){
				zeroflag = 1;
				WriteFBXAnimTraOfBVH(fbxbone, lAnimLayer, AXIS_X, zeroflag);
				WriteFBXAnimTraOfBVH(fbxbone, lAnimLayer, AXIS_Y, zeroflag);
				WriteFBXAnimTraOfBVH(fbxbone, lAnimLayer, AXIS_Z, zeroflag);

				WriteFBXAnimRotOfBVH(fbxbone, lAnimLayer, AXIS_Z, zeroflag);
				WriteFBXAnimRotOfBVH(fbxbone, lAnimLayer, AXIS_X, zeroflag);
				WriteFBXAnimRotOfBVH(fbxbone, lAnimLayer, AXIS_Y, zeroflag);
			}
			else{
				zeroflag = 0;
				WriteFBXAnimTraOfBVH(fbxbone, lAnimLayer, AXIS_X, zeroflag);
				WriteFBXAnimTraOfBVH(fbxbone, lAnimLayer, AXIS_Y, zeroflag);
				WriteFBXAnimTraOfBVH(fbxbone, lAnimLayer, AXIS_Z, zeroflag);

				WriteFBXAnimRotOfBVH(fbxbone, lAnimLayer, AXIS_Z, zeroflag);
				WriteFBXAnimRotOfBVH(fbxbone, lAnimLayer, AXIS_X, zeroflag);
				WriteFBXAnimRotOfBVH(fbxbone, lAnimLayer, AXIS_Y, zeroflag);
			}
			
		}

		if (fbxbone->GetChild()){
			AnimateBoneOfBVHReq(fbxbone->GetChild(), lAnimLayer);
		}
		if (fbxbone->GetBrother()){
			AnimateBoneOfBVHReq(fbxbone->GetBrother(), lAnimLayer);
		}
	}
}


void AnimateSkeleton(FbxScene* pScene, CModel* pmodel)
{

    FbxString lAnimStackName;
    FbxTime lTime;
    int lKeyIndex = 0;

	int motionnum = pmodel->GetMotInfoSize();
	if( motionnum <= 0 ){
		return;
	}

	s_ai = (ANIMINFO*)malloc( sizeof( ANIMINFO ) * motionnum );
	if( !s_ai ){
		_ASSERT( 0 );
		return;
	}
	ZeroMemory( s_ai, sizeof( ANIMINFO ) * motionnum );
	s_ainum = motionnum;

	int aino = 0;
	map<int, MOTINFO*>::iterator itrmi;
	for( itrmi = pmodel->GetMotInfoBegin(); itrmi != pmodel->GetMotInfoEnd(); itrmi++ ){
		MOTINFO* curmi = itrmi->second;
		(s_ai + aino)->motid = curmi->motid;
		(s_ai + aino)->orgindex = aino;
		(s_ai + aino)->maxframe = (int)( curmi->frameleng - 1.0 );
		(s_ai + aino)->engmotname = curmi->engmotname;
		aino++;
	}

_ASSERT( motionnum == aino );

	qsort_s( s_ai, motionnum, sizeof( ANIMINFO ), sortfunc_leng, NULL );//モーション長が短い順に出力しないと正しく読み込めない。FBXの仕様？

	s_firstoutmot = s_ai->motid;

	for( aino = 0; aino < motionnum; aino++ ){
		ANIMINFO* curai = s_ai + aino;
		int curmotid = curai->motid;
		int maxframe = curai->maxframe;

		lAnimStackName = curai->engmotname;
	    FbxAnimStack* lAnimStack = FbxAnimStack::Create(pScene, lAnimStackName);
		FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(pScene, "Base Layer");
		curai->animlayer = lAnimLayer;
        lAnimStack->AddMember(lAnimLayer);

		pmodel->SetCurrentMotion( curmotid );

		s_firstanimout = 1;
		AnimateBoneReq( s_fbxbone, lAnimLayer, curmotid, maxframe );

		pScene->GetRootNode()->ConvertPivotAnimationRecursive( lAnimStackName, s_convPivot, 30.0, true );
		//pScene->GetRootNode()->ConvertPivotAnimationRecursive( lAnimStackName, FbxNode::eDestinationPivot, 30.0, true );
	}

}

void AnimateBoneReq( CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer, int curmotid, int maxframe )
{
	static int s_dbgcnt = 0;

    int lKeyIndex = 0;
    FbxNode* lSkel = 0;

	if(fbxbone){
		CBone* curbone = fbxbone->GetBone();
		if( curbone ){

			lSkel = fbxbone->GetSkelNode();
			if( !lSkel ){
				_ASSERT( 0 );
				return;
			}
			
			EFbxRotationOrder lRotationOrderSrc = eEulerZXY;
			EFbxRotationOrder lRotationOrderDst = eEulerXYZ;
			lSkel->GetRotationOrder( FbxNode::eSourcePivot, lRotationOrderSrc );
			lSkel->GetRotationOrder( FbxNode::eDestinationPivot, lRotationOrderDst );

			
			lSkel->SetRotationOrder(FbxNode::eSourcePivot, eEulerXYZ );
			lSkel->SetRotationOrder(FbxNode::eDestinationPivot, eEulerZXY );
			s_convPivot =  FbxNode::eSourcePivot;
			s_dbgcnt++;



			WriteFBXAnimTra(fbxbone, lAnimLayer, curmotid, maxframe, AXIS_X);
			WriteFBXAnimTra(fbxbone, lAnimLayer, curmotid, maxframe, AXIS_Y);
			WriteFBXAnimTra(fbxbone, lAnimLayer, curmotid, maxframe, AXIS_Z);

			WriteFBXAnimRot(fbxbone, lAnimLayer, curmotid, maxframe, AXIS_Z);
			WriteFBXAnimRot(fbxbone, lAnimLayer, curmotid, maxframe, AXIS_X);
			WriteFBXAnimRot(fbxbone, lAnimLayer, curmotid, maxframe, AXIS_Y);

		}
	}

	if( fbxbone->GetChild() ){
		AnimateBoneReq( fbxbone->GetChild(), lAnimLayer, curmotid, maxframe );
	}
	if( fbxbone->GetBrother() ){
		AnimateBoneReq( fbxbone->GetBrother(), lAnimLayer, curmotid, maxframe );
	}
}



/***
int WriteBindPose(FbxScene* pScene, CModel* pmodel)
{
	FbxTime lTime0;
	lTime0.SetSecondDouble( 0.0 );

	FbxPose* lPose = FbxPose::Create(pScene,"BindPose0");
	lPose->SetIsBindPose(true);

	if( s_firstoutmot >= 0 ){
		KFbxAnimStack * lCurrentAnimationStack = pScene->FindMember(FBX_TYPE(KFbxAnimStack), s_ai->engmotname);
		if (lCurrentAnimationStack == NULL)
		{
			_ASSERT( 0 );
			return 1;
		}
		FbxAnimLayer * mCurrentAnimLayer;
		mCurrentAnimLayer = lCurrentAnimationStack->GetMember(FBX_TYPE(FbxAnimLayer), 0);
		pScene->GetEvaluator()->SetContext(lCurrentAnimationStack);
	}else{
		_ASSERT( 0 );
	}

	map<int, CBone*>::iterator itrbone;
	for( itrbone = pmodel->m_bonelist.begin(); itrbone != pmodel->m_bonelist.end(); itrbone++ ){
		CBone* curbone = itrbone->second;
		FbxNode* curskel = s_bone2skel[ curbone ];
		if( curskel ){
			if( curskel ){
				FbxAMatrix lBindMatrix = curskel->EvaluateGlobalTransform( lTime0 );
				lPose->Add(curskel, lBindMatrix);
			}
		}
	}

	pScene->AddPose(lPose);

	return 0;
}
***/



int WriteBindPose(FbxScene* pScene, int bvhflag)
{

	FbxPose* lPose = FbxPose::Create(pScene,"BindPose0");
	lPose->SetIsBindPose(true);

	if( s_firstoutmot >= 0 ){
		FbxAnimStack * lCurrentAnimationStack;
		if (bvhflag == 0){
			lCurrentAnimationStack = pScene->FindMember(FBX_TYPE(FbxAnimStack), s_ai->engmotname);
		}
		else{
			lCurrentAnimationStack = pScene->FindMember(FBX_TYPE(FbxAnimStack), "bvh_animation_nyan");
		}
		if (lCurrentAnimationStack == NULL)
		{
			_ASSERT( 0 );
			return 1;
		}
		FbxAnimLayer * mCurrentAnimLayer;
		mCurrentAnimLayer = lCurrentAnimationStack->GetMember(FBX_TYPE(FbxAnimLayer), 0);
		pScene->GetEvaluator()->SetContext(lCurrentAnimationStack);
	}else{
		_ASSERT( 0 );
	}

	WriteBindPoseReq( s_fbxbone, lPose );

	pScene->AddPose(lPose);

	return 0;
}

void WriteBindPoseReq( CFBXBone* fbxbone, FbxPose* lPose )
{
	FbxTime lTime0;
	lTime0.SetSecondDouble( 0.0 );

	FbxAMatrix lBindMatrix;
	lBindMatrix.SetIdentity();
	ChaMatrix tramat;
	ChaMatrixIdentity(&tramat);
	FbxNode* curskel = fbxbone->GetSkelNode();
	if (fbxbone->GetType() != FB_ROOT){
		if (curskel){
			FbxAMatrix lBindMatrix;
			lBindMatrix = CalcBindMatrix(fbxbone);
			lPose->Add(curskel, lBindMatrix);
		}
	}


	if( fbxbone->GetChild() ){
		WriteBindPoseReq( fbxbone->GetChild(), lPose );
	}
	if( fbxbone->GetBrother() ){
		WriteBindPoseReq( fbxbone->GetBrother(), lPose );
	}
}

FbxAMatrix CalcBindMatrix(CFBXBone* fbxbone)
{
	int notexistflag = 0;

	ChaVector3 curpos, parpos;
	if (s_bvhflag == 1){
		CBVHElem* curbone = fbxbone->GetBvhElem();
		if (curbone){
			curpos = curbone->GetPosition();

			CFBXBone* parfbxbone;
			parfbxbone = fbxbone->GetParent();
			if (parfbxbone){
				CBVHElem* parbone = parfbxbone->GetBvhElem();
				if (parbone){
					parpos = parbone->GetPosition();
				}
				else{
					parpos = ChaVector3(0.0f, 0.0f, 0.0f);
				}
			}
			else{
				parpos = ChaVector3(0.0f, 0.0f, 0.0f);
			}
		}
		else{
			notexistflag = 1;
		}
	}
	else{
		CBone* curbone = fbxbone->GetBone();
		if (curbone){
			curpos = curbone->GetJointFPos();

			CFBXBone* parfbxbone;
			parfbxbone = fbxbone->GetParent();
			if (parfbxbone){
				CBone* parbone = parfbxbone->GetBone();
				if (parbone){
					parpos = parbone->GetJointFPos();
				}
				else{
					parpos = ChaVector3(0.0f, 0.0f, 0.0f);
				}
			}
			else{
				parpos = ChaVector3(0.0f, 0.0f, 0.0f);
			}
		}
		else{
			notexistflag = 1;
		}
	}

	CBone calcbone(s_model);

	ChaMatrix tramat;
	ChaMatrixIdentity(&tramat);

	if (notexistflag == 1){
		FbxAMatrix lInitMatrix;
		lInitMatrix.SetIdentity();
		return lInitMatrix;//!!!!!!!!!!!!!!! curbone == NULL
	}

	if ((s_bvhflag == 0) && fbxbone->GetBone() && (s_model->GetOldAxisFlagAtLoading() == 0)){
		tramat = fbxbone->GetBone()->GetNodeMat();
	}
	else{
		ChaVector3 diffvec = curpos - parpos;
		float leng = ChaVector3Length(&diffvec);
		if (leng >= 0.00001f){
			//tramat = CalcAxisMatX_aft(parpos, curpos);
			calcbone.CalcAxisMatZ_aft(parpos, curpos, &tramat);
			tramat._41 = curpos.x;
			tramat._42 = curpos.y;
			tramat._43 = curpos.z;
		}
		else{
			ChaMatrixIdentity(&tramat);
			tramat._41 = curpos.x;
			tramat._42 = curpos.y;
			tramat._43 = curpos.z;
		}
	}
	FbxAMatrix lBindMatrix;
	lBindMatrix[0][0] = tramat._11;
	lBindMatrix[0][1] = tramat._12;
	lBindMatrix[0][2] = tramat._13;
	lBindMatrix[0][3] = tramat._14;
	lBindMatrix[1][0] = tramat._21;
	lBindMatrix[1][1] = tramat._22;
	lBindMatrix[1][2] = tramat._23;
	lBindMatrix[1][3] = tramat._24;
	lBindMatrix[2][0] = tramat._31;
	lBindMatrix[2][1] = tramat._32;
	lBindMatrix[2][2] = tramat._33;
	lBindMatrix[2][3] = tramat._34;
	lBindMatrix[3][0] = tramat._41;
	lBindMatrix[3][1] = tramat._42;
	lBindMatrix[3][2] = tramat._43;
	lBindMatrix[3][3] = tramat._44;

	return lBindMatrix;

}


/*
FbxAMatrix CalcBindMatrix(CFBXBone* fbxbone)
{
	FbxAMatrix lBindMatrix;
	FbxTime lTime0;
	lTime0.SetSecondDouble(0.0);

	FbxNode* curskel = fbxbone->GetSkelNode();
	if (curskel){
		lBindMatrix = curskel->EvaluateGlobalTransform(lTime0);
	}
	else{
		lBindMatrix.SetIdentity();
	}
	return lBindMatrix;
}


*/

/***
void StoreBindPose(FbxScene* pScene, FbxNode* pMesh, FbxNode* pSkeletonRoot)
{
    // In the bind pose, we must store all the link's global matrix at the time of the bind.
    // Plus, we must store all the parent(s) global matrix of a link, even if they are not
    // themselves deforming any model.

    // In this example, since there is only one model deformed, we don't need walk through 
    // the scene
    //

    // Now list the all the link involve in the Mesh deformation
    KArrayTemplate<FbxNode*> lClusteredFbxNodes;
    int                       i, j;

    if (pMesh && pMesh->GetNodeAttribute())
    {
        int lSkinCount=0;
        int lClusterCount=0;
        switch (pMesh->GetNodeAttribute()->GetAttributeType())
        {
        case FbxNodeAttribute::eMESH:
        case FbxNodeAttribute::eNURB:

            lSkinCount = ((FbxGeometry*)pMesh->GetNodeAttribute())->GetDeformerCount(KFbxDeformer::eSKIN);
            //Go through all the skins and count them
            //then go through each skin and get their cluster count
            for(i=0; i<lSkinCount; ++i)
            {
                FbxSkin *lSkin=(FbxSkin*)((FbxGeometry*)pMesh->GetNodeAttribute())->GetDeformer(i, KFbxDeformer::eSKIN);
                lClusterCount+=lSkin->GetClusterCount();
            }
            break;
        }
        //if we found some clusters we must add the node
        if (lClusterCount)
        {
            //Again, go through all the skins get each cluster link and add them
            for (i=0; i<lSkinCount; ++i)
            {
                FbxSkin *lSkin=(FbxSkin*)((FbxGeometry*)pMesh->GetNodeAttribute())->GetDeformer(i, KFbxDeformer::eSKIN);
                lClusterCount=lSkin->GetClusterCount();
                for (j=0; j<lClusterCount; ++j)
                {
                    FbxNode* lClusterNode = lSkin->GetCluster(j)->GetLink();
                    AddNodeRecursively(lClusteredFbxNodes, lClusterNode);
                }

            }

            // Add the Mesh to the pose
            lClusteredFbxNodes.Add(pMesh);
        }
    }

    // Now create a bind pose with the link list
    if (lClusteredFbxNodes.GetCount())
    {
        // A pose must be named. Arbitrarily use the name of the Mesh node.
        FbxPose* lPose = FbxPose::Create(pScene,pMesh->GetName());

        // default pose type is rest pose, so we need to set the type as bind pose
        lPose->SetIsBindPose(true);

        for (i=0; i<lClusteredFbxNodes.GetCount(); i++)
        {
            FbxNode*  lFbxNode   = lClusteredFbxNodes.GetAt(i);
            FbxAMatrix lBindMatrix = lFbxNode->EvaluateGlobalTransform();

            lPose->Add(lFbxNode, lBindMatrix);
        }

        // Add the pose to the scene
        pScene->AddPose(lPose);
    }
}
***/

int ExistBoneInInf( int boneno, CMQOObject* srcobj, int* dstclusterno )
{
	*dstclusterno = -1;

	CPolyMesh4* pm4 = srcobj->GetPm4();
	_ASSERT( pm4 );

	int dirtyflag = 0;

	int clusterno = -1;
	CBone* chkbone;
	int clno;
	for( clno = 0; clno < srcobj->GetClusterSize(); clno++ ){
		chkbone = srcobj->GetCluster( clno );
		if( chkbone ){
			if( chkbone->GetBoneNo() == boneno ){
				clusterno = clno;
				break;
			}
		}
	}

	if( clusterno < 0 ){
		return 0;
	}

	*dstclusterno = clusterno;

	int faceno;
	for (faceno = 0; faceno < srcobj->GetPm4()->GetFaceNum(); faceno++){
		CMQOFace* curface = pm4->GetTriFace() + faceno;
		int vno[3];
		vno[0] = curface->GetIndex(0);
		vno[2] = curface->GetIndex(2);
		vno[1] = curface->GetIndex(1);

		_ASSERT((vno[0] >= 0) && (vno[0] < pm4->GetOrgPointNum()));
		_ASSERT((vno[1] >= 0) && (vno[1] < pm4->GetOrgPointNum()));
		_ASSERT((vno[2] >= 0) && (vno[2] < pm4->GetOrgPointNum()));

		int vcnt;
		for (vcnt = 0; vcnt < 3; vcnt++){
			CInfBone* curib = pm4->GetInfBone() + vno[vcnt];
			int ieno = curib->ExistBone(srcobj, clusterno);
			if (ieno >= 0){
				dirtyflag = 1;
				break;
			}
		}
	}
	/*
	int vno;
	for (vno = 0; vno < pm4->GetOrgPointNum(); vno++){
		CInfBone* curib = pm4->GetInfBone() + vno;
		int ieno = curib->ExistBone(srcobj, clusterno);
		if (ieno >= 0){
			dirtyflag = 1;
			break;
		}
	}
	*/
	return dirtyflag;
}


int AnimateMorph(FbxScene* pScene, CModel* pmodel)
{
    FbxTime lTime;
    int lKeyIndex = 0;

	if( s_blsinfo.empty() ){
		return 0;
	}

	if( s_ainum <= 0 ){
		return 0;
	}

	double timescale = 30.0;
//	double timescale = 300.0;

	int aino;
	for( aino = 0; aino < s_ainum; aino++ ){

		ANIMINFO* curai = s_ai + aino;
		int curmotid = curai->motid;
		int maxframe = curai->maxframe;

		FbxAnimStack * lCurrentAnimationStack = pmodel->GetScene()->FindMember<FbxAnimStack>(pmodel->GetAnimStackName( curai->orgindex )->Buffer());
		if (lCurrentAnimationStack == NULL){
			_ASSERT( 0 );
			return 1;
		}
		pmodel->GetScene()->GetEvaluator()->SetContext(lCurrentAnimationStack);


		FbxAnimLayer* ldstAnimLayer = curai->animlayer;
		map<int, BLSINFO>::iterator itrblsinfo;
		for( itrblsinfo = s_blsinfo.begin(); itrblsinfo != s_blsinfo.end(); itrblsinfo++ ){
			BLSINFO curinfo = itrblsinfo->second;
			FbxGeometry* lAttribute = (FbxGeometry*)(curinfo.basenode)->GetNodeAttribute();
			FbxAnimCurve* lCurve = lAttribute->GetShapeChannel(0, curinfo.blsindex.channelno, ldstAnimLayer, true);
			if (lCurve)
			{
				lCurve->KeyModifyBegin();

				int frameno;
				for( frameno = 0; frameno <= maxframe; frameno++ ){
					double dframe = (double)frameno;
					float curweight = pmodel->GetTargetWeight( curmotid, dframe, timescale, curinfo.base, curinfo.targetname );

					lTime.SetSecondDouble( dframe / timescale);
					lKeyIndex = lCurve->KeyAdd(lTime);
					lCurve->KeySetValue(lKeyIndex, curweight);
					lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

//if( aino == 0 ){
//	WCHAR wtargetname[256]={0L};
//	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (char*)curinfo.targetname.c_str(), 256, wtargetname, 256 );
//	DbgOut( L"weight check !!! : target %s, frame %f, weight %f\r\n",
//		wtargetname, dframe, curweight );
//}
				}
				lCurve->KeyModifyEnd();
			}
		}
	}
	return 0;
}



CFBXBone* CreateFBXBoneOfBVH( FbxScene* pScene )
{
	s_fbxbonenum = 0;
	if( !s_behead ){
		_ASSERT( 0 );
		return 0;
	}



	FbxNode* lSkeletonNode;
	FbxString lNodeName( "RootNode" );
	FbxSkeleton* lSkeletonNodeAttribute = FbxSkeleton::Create(pScene, lNodeName);
	lSkeletonNodeAttribute->SetSkeletonType(FbxSkeleton::eRoot);
	lSkeletonNode = FbxNode::Create(pScene,lNodeName.Buffer());
	lSkeletonNodeAttribute->Size.Set(1.0);
	lSkeletonNode->SetNodeAttribute(lSkeletonNodeAttribute);
	lSkeletonNode->LclTranslation.Set(FbxVector4(0.0, 0.0, 0.0));
	CFBXBone* fbxbone = new CFBXBone();
	if( !fbxbone ){
		_ASSERT( 0 );
		return 0;
	}
	fbxbone->SetType( FB_ROOT );
	fbxbone->SetBvhElem( 0 );
	fbxbone->SetSkelNode( lSkeletonNode );
	s_fbxbonenum++;

	FbxNode* lSkeletonNode2;
	FbxString lNodeName2(s_behead->GetName());
	FbxSkeleton* lSkeletonNodeAttribute2 = FbxSkeleton::Create(pScene, lNodeName2);
	lSkeletonNodeAttribute2->SetSkeletonType(FbxSkeleton::eLimbNode);
	lSkeletonNode2 = FbxNode::Create(pScene, lNodeName2.Buffer());
	lSkeletonNodeAttribute2->Size.Set(1.0);
	lSkeletonNode2->SetNodeAttribute(lSkeletonNodeAttribute2);
	lSkeletonNode2->LclTranslation.Set(FbxVector4(s_behead->GetPosition().x, s_behead->GetPosition().y, s_behead->GetPosition().z));
	CFBXBone* fbxbone2 = new CFBXBone();
	if (!fbxbone2){
		_ASSERT(0);
		return 0;
	}
	fbxbone2->SetType(FB_NORMAL);
	fbxbone2->SetBvhElem(s_behead);
	fbxbone2->SetSkelNode(lSkeletonNode2);
	fbxbone->GetSkelNode()->AddChild(lSkeletonNode2);
	fbxbone->AddChild(fbxbone2);
	s_fbxbonenum++;


	s_fbxbone = fbxbone;//!!!!!!!!!!!!!!!!!!!!!!!!!

	if( s_behead->GetChild() ){
		CreateFBXBoneOfBVHReq( pScene, s_behead->GetChild(), fbxbone2 );
	}

	return fbxbone;

}
void CreateFBXBoneOfBVHReq( FbxScene* pScene, CBVHElem* pbe, CFBXBone* parfbxbone )
{
	static int s_siteno = 0;


	CFBXBone* fbxbone = new CFBXBone();
	if( !fbxbone ){
		_ASSERT( 0 );
		return;
	}


	CBVHElem* parpbe;// = pbe->GetParent();
	if (parfbxbone){
		parpbe = parfbxbone->GetBvhElem();
	}
	else{
		parpbe = 0;
	}

	ChaVector3 curpos, parpos, gparpos;
	curpos = pbe->GetPosition();
	if (parpbe){
		parpos = parpbe->GetPosition();
	}
	else{
		parpos = ChaVector3(0.0f, 0.0f, 0.0f);
	}


	if (parpbe && (parpbe->GetBoneNum() >= 2)){//bvhelemにbonenum追加
		CFBXBone* fbxbone2 = new CFBXBone();
		if (!fbxbone2){
			_ASSERT(0);
			return;
		}

				
		char newname2[256] = { 0 };
		sprintf_s(newname2, 256, "%s_bunki%d", parpbe->GetName(), pbe->GetBroNo());

		FbxString lLimbNodeName2(newname2);
		FbxSkeleton* lSkeletonLimbNodeAttribute2 = FbxSkeleton::Create(pScene, lLimbNodeName2);
		lSkeletonLimbNodeAttribute2->SetSkeletonType(FbxSkeleton::eLimbNode);
		lSkeletonLimbNodeAttribute2->Size.Set(1.0);
		FbxNode* lSkeletonLimbNode2 = FbxNode::Create(pScene, lLimbNodeName2.Buffer());
		lSkeletonLimbNode2->SetNodeAttribute(lSkeletonLimbNodeAttribute2);
		//lSkeletonLimbNode2->LclTranslation.Set(FbxVector4(curpos2.x - parpos2.x, curpos2.y - parpos2.y, curpos2.z - parpos2.z));
		lSkeletonLimbNode2->LclTranslation.Set(FbxVector4(0.0, 0.0, 0.0));
		parfbxbone->GetSkelNode()->AddChild(lSkeletonLimbNode2);

		fbxbone2->SetBvhElem(parpbe);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		fbxbone2->SetSkelNode(lSkeletonLimbNode2);
		parfbxbone->AddChild(fbxbone2);

		fbxbone2->SetType(FB_BUNKI_CHIL);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (parpbe){
			parfbxbone->SetType(FB_BUNKI_PAR);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		s_fbxbonenum++;

		char newname[256] = { 0 };
		sprintf_s(newname, 256, "%s", pbe->GetName());


		FbxString lLimbNodeName1(newname);
		FbxSkeleton* lSkeletonLimbNodeAttribute1 = FbxSkeleton::Create(pScene, lLimbNodeName1);
		lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eLimbNode);
		lSkeletonLimbNodeAttribute1->Size.Set(1.0);
		FbxNode* lSkeletonLimbNode1 = FbxNode::Create(pScene, lLimbNodeName1.Buffer());
		lSkeletonLimbNode1->SetNodeAttribute(lSkeletonLimbNodeAttribute1);
		//if (pbe->GetChild()){
		if (pbe->GetParent()){//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			lSkeletonLimbNode1->LclTranslation.Set(FbxVector4(curpos.x - parpos.x, curpos.y - parpos.y, curpos.z - parpos.z));
		}
		else{
			//lSkeletonLimbNode1->LclTranslation.Set(FbxVector4(parpos.x - gparpos.x, parpos.y - gparpos.y, parpos.z - gparpos.z));
			lSkeletonLimbNode1->LclTranslation.Set(FbxVector4(curpos.x, curpos.y, curpos.z));//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		fbxbone2->GetSkelNode()->AddChild(lSkeletonLimbNode1);

		fbxbone->SetBvhElem(pbe);
		fbxbone->SetSkelNode(lSkeletonLimbNode1);
		fbxbone2->AddChild(fbxbone);

		if (pbe->GetChild()){
			fbxbone->SetType(FB_NORMAL);
		}
		else{
			//endjoint
			fbxbone->SetType(FB_ENDJOINT);
		}
		s_fbxbonenum++;

		if (pbe->GetBrother()){
			CreateFBXBoneOfBVHReq(pScene, pbe->GetBrother(), parfbxbone);
		}

		if (pbe->GetChild()){
			CreateFBXBoneOfBVHReq(pScene, pbe->GetChild(), fbxbone);
		}

		/*
		if (pbone->child){
			CreateFBXBoneReq(pScene, pbone->child, fbxbone2);
		}

		CShdElem* validbro = s_lpsh->GetValidBrother(pbone->brother);
		if (validbro){
			CreateFBXBoneReq(pScene, validbro, parfbxbone);
		}
		*/
	}
	else{
		char newname[256] = { 0 };
		sprintf_s(newname, 256, "%s", pbe->GetName());
		int cmp0 = strcmp(newname, "Site");
		int cmp1 = strcmp(newname, "site");
		if ((cmp0 == 0) || (cmp1 == 0)){
			char addstr[256];
			sprintf_s(addstr, 256, "_%d", s_siteno);
			strcat_s(newname, 256, addstr);
			s_siteno++;
		}

		FbxString lLimbNodeName1(newname);
		FbxSkeleton* lSkeletonLimbNodeAttribute1 = FbxSkeleton::Create(pScene, lLimbNodeName1);
		lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eLimbNode);
		lSkeletonLimbNodeAttribute1->Size.Set(1.0);
		FbxNode* lSkeletonLimbNode1 = FbxNode::Create(pScene, lLimbNodeName1.Buffer());
		lSkeletonLimbNode1->SetNodeAttribute(lSkeletonLimbNodeAttribute1);
		//if (pbe->GetChild()){
		if (pbe->GetParent()){//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			lSkeletonLimbNode1->LclTranslation.Set(FbxVector4(curpos.x - parpos.x, curpos.y - parpos.y, curpos.z - parpos.z));
		}
		else{
			//lSkeletonLimbNode1->LclTranslation.Set(FbxVector4(parpos.x - gparpos.x, parpos.y - gparpos.y, parpos.z - gparpos.z));
			lSkeletonLimbNode1->LclTranslation.Set(FbxVector4(curpos.x, curpos.y, curpos.z));//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			_ASSERT(0);
		}
		parfbxbone->GetSkelNode()->AddChild(lSkeletonLimbNode1);

		fbxbone->SetBvhElem(pbe);
		fbxbone->SetSkelNode(lSkeletonLimbNode1);
		parfbxbone->AddChild(fbxbone);

		if (pbe->GetChild()){
			fbxbone->SetType(FB_NORMAL);
		}
		else{
			//endjoint出力
			fbxbone->SetType(FB_ENDJOINT);
		}
		s_fbxbonenum++;

		if (pbe->GetBrother()){
			CreateFBXBoneOfBVHReq(pScene, pbe->GetBrother(), parfbxbone);
		}

		if (pbe->GetChild()){
			CreateFBXBoneOfBVHReq(pScene, pbe->GetChild(), fbxbone);
		}
	}
}


CFBXBone* CreateFBXBone( FbxScene* pScene, CModel* pmodel )
{
	s_fbxbonenum = 0;

	CBone* topj = pmodel->GetTopBone();
	if( !topj ){
		_ASSERT( 0 );
		return 0;
	}

	//_ASSERT(0);

	FbxNode* lSkeletonNode2;
	FbxString lNodeName2("RootNode");
	FbxSkeleton* lSkeletonNodeAttribute2 = FbxSkeleton::Create(pScene, lNodeName2);
	lSkeletonNodeAttribute2->SetSkeletonType(FbxSkeleton::eRoot);
	lSkeletonNodeAttribute2->Size.Set(1.0);
	lSkeletonNode2 = FbxNode::Create(pScene, lNodeName2.Buffer());
	lSkeletonNode2->SetNodeAttribute(lSkeletonNodeAttribute2);
	lSkeletonNode2->LclTranslation.Set(FbxVector4(0.0f, 0.0f, 0.0f));

	CFBXBone* fbxbone2 = new CFBXBone();
	if (!fbxbone2){
		_ASSERT(0);
		return 0;
	}
	fbxbone2->SetType(FB_ROOT);
	fbxbone2->SetBone(0);
	fbxbone2->SetSkelNode(lSkeletonNode2);
	s_fbxbone = fbxbone2;//!!!!!!!!!!!!!!!!!!!!!!!!!



	FbxNode* lSkeletonNode;
	FbxString lNodeName( topj->GetEngBoneName() );
	FbxSkeleton* lSkeletonNodeAttribute = FbxSkeleton::Create(pScene, lNodeName);
	lSkeletonNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);

	lSkeletonNodeAttribute->Size.Set(1.0);

	lSkeletonNode = FbxNode::Create(pScene,lNodeName.Buffer());
	lSkeletonNode->SetNodeAttribute(lSkeletonNodeAttribute);
	lSkeletonNode->LclTranslation.Set(FbxVector4(topj->GetJointFPos().x, topj->GetJointFPos().y, topj->GetJointFPos().z));

	CFBXBone* fbxbone = new CFBXBone();
	if( !fbxbone ){
		_ASSERT( 0 );
		return 0;
	}
	fbxbone->SetType( FB_NORMAL );
	fbxbone->SetBone( topj );
	fbxbone->SetSkelNode( lSkeletonNode );

	fbxbone2->GetSkelNode()->AddChild(lSkeletonNode);
	fbxbone2->AddChild(fbxbone);
	s_fbxbonenum++;

	s_firsttopbone = fbxbone;//rootの最初の子供

	if( topj->GetChild() ){
		CreateFBXBoneReq( pScene, topj->GetChild(), fbxbone );
	}

	return fbxbone2;//!!!!!!!!!!

}

void CreateFBXBoneReq( FbxScene* pScene, CBone* pbone, CFBXBone* parfbxbone )
{
	ChaVector3 curpos, parpos, gparpos;
	curpos = pbone->GetJointFPos();
	CBone* parbone = pbone->GetParent();
	if( parbone ){
		parpos = parbone->GetJointFPos();
	}else{
		parpos = ChaVector3( 0.0f, 0.0f, 0.0f );
	}

	CFBXBone* fbxbone = new CFBXBone();
	if( !fbxbone ){
		_ASSERT( 0 );
		return;
	}

	char newname[256] = {0};
	sprintf_s( newname, 256, "%s", pbone->GetEngBoneName() );

	FbxString lLimbNodeName1( newname );
	FbxSkeleton* lSkeletonLimbNodeAttribute1 = FbxSkeleton::Create(pScene,lLimbNodeName1);
	lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eLimbNode);
	lSkeletonLimbNodeAttribute1->Size.Set(1.0);
	FbxNode* lSkeletonLimbNode1 = FbxNode::Create(pScene,lLimbNodeName1.Buffer());
	lSkeletonLimbNode1->SetNodeAttribute(lSkeletonLimbNodeAttribute1);
	lSkeletonLimbNode1->LclTranslation.Set(FbxVector4(curpos.x - parpos.x, curpos.y - parpos.y, curpos.z - parpos.z));
	parfbxbone->GetSkelNode()->AddChild(lSkeletonLimbNode1);

	fbxbone->SetBone( pbone );
	fbxbone->SetSkelNode( lSkeletonLimbNode1 );
	parfbxbone->AddChild( fbxbone );

	if( pbone->GetChild() ){
		fbxbone->SetType( FB_NORMAL );
	}else{
		//endjoint出力
		fbxbone->SetType( FB_ENDJOINT );
	}
	s_fbxbonenum++;

	if( pbone->GetChild() ){
		CreateFBXBoneReq( pScene, pbone->GetChild(), fbxbone );
	}
	if( pbone->GetBrother() ){
		CreateFBXBoneReq( pScene, pbone->GetBrother(), parfbxbone );
	}
}
int DestroyFBXBoneReq( CFBXBone* fbxbone )
{
	CFBXBone* chilbone = fbxbone->GetChild();
	CFBXBone* brobone = fbxbone->GetBrother();

	delete fbxbone;

	if( chilbone ){
		DestroyFBXBoneReq( chilbone );
	}
	if( brobone ){
		DestroyFBXBoneReq( brobone );
	}

	return 0;
}


void CreateDummyInfDataReq(CFBXBone* fbxbone, FbxManager*& pSdkManager, FbxScene*& pScene, FbxNode* lMesh, FbxSkin* lSkin, int bonecnt)
{
	//FbxNode* lMesh = CreateDummyFbxMesh(pSdkManager, pScene);
	//srcRootNode->AddChild(lMesh);
	//FbxGeometry* lMeshAttribute = (FbxGeometry*)lMesh->GetNodeAttribute();
	//FbxSkin* lSkin = FbxSkin::Create(pScene, "");

	LinkDummyMeshToSkeleton(fbxbone, lSkin, pScene, lMesh, bonecnt);
	//lMeshAttribute->AddDeformer(lSkin);
	bonecnt++;

	if (fbxbone->GetChild()){
		CreateDummyInfDataReq(fbxbone->GetChild(), pSdkManager, pScene, lMesh, lSkin, bonecnt);
	}

	if (fbxbone->GetBrother()){
		CreateDummyInfDataReq(fbxbone->GetBrother() , pSdkManager, pScene, lMesh, lSkin, bonecnt);
	}

}

FbxNode* CreateDummyFbxMesh(FbxManager* pSdkManager, FbxScene* pScene)
{
	static int s_namecnt = 0;

	char meshname[256] = { 0 };
	sprintf_s(meshname, 256, "_ND_dtri%d", s_namecnt);
	s_namecnt++;
	int facenum = s_fbxbonenum;

	FbxMesh* lMesh = FbxMesh::Create(pScene, meshname);
	lMesh->InitControlPoints(facenum * 3);
	FbxVector4* lcp = lMesh->GetControlPoints();

	FbxGeometryElementNormal* lElementNormal = lMesh->CreateElementNormal();
	lElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
	lElementNormal->SetReferenceMode(FbxGeometryElement::eDirect);

	FbxGeometryElementUV* lUVDiffuseElement = lMesh->CreateElementUV("DiffuseUV");
	_ASSERT(lUVDiffuseElement != NULL);
	lUVDiffuseElement->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	lUVDiffuseElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	int vsetno = 0;
	int vcnt;
	for (vcnt = 0; vcnt < facenum * 3; vcnt++){
		*(lcp + vsetno) = FbxVector4(0.0f, 0.0f, 0.0f, 1.0f);
		FbxVector4 fbxn = FbxVector4(0.0f, 0.0f, -1.0f, 0.0f);
		lElementNormal->GetDirectArray().Add(fbxn);
		FbxVector2 fbxuv = FbxVector2(0.0f, 0.0f);
		lUVDiffuseElement->GetDirectArray().Add(fbxuv);
		vsetno++;
	}

	lUVDiffuseElement->GetIndexArray().SetCount(facenum * 3);

	vsetno = 0;
	int faceno;
	for (faceno = 0; faceno < facenum; faceno++){
		lMesh->BeginPolygon(-1, -1, -1, false);
		int i;
		for (i = 0; i < 3; i++)
		{
			// Control point index
			lMesh->AddPolygon(vsetno);
			// update the index array of the UVs that map the texture to the face
			//lUVDiffuseElement->GetIndexArray().SetAt(vsetno, vsetno);
			//lUVDiffuseElement->GetIndexArray().SetAt(vsetno, faceno);
			//lUVDiffuseElement->GetIndexArray().SetAt(vsetno, 0);
			lUVDiffuseElement->GetIndexArray().SetAt(vsetno, vsetno);
			vsetno++;
		}
		lMesh->EndPolygon();
	}


	// create a KFbxNode
	//		KFbxNode* lNode = KFbxNode::Create(pSdkManager,pName);
	FbxNode* lNode = FbxNode::Create(pScene, meshname);
	// set the node attribute
	lNode->SetNodeAttribute(lMesh);
	// set the shading mode to view texture
	lNode->SetShadingMode(FbxNode::eTextureShading);
	// rotate the plane
	lNode->LclRotation.Set(FbxVector4(0, 0, 0));

	// Set material mapping.
	FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
	lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
	lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
	if (!lMesh->GetElementMaterial(0)){
		_ASSERT(0);
		return NULL;
	}

	// add material to the node. 
	// the material can't in different document with the geometry node or in sub-document
	// we create a simple material here which belong to main document
	static int s_matcnt = 0;
	s_matcnt++;


	char matname[256];
	sprintf_s(matname, 256, "material_%d", s_matcnt);
	//KString lMaterialName = mqomat->name;
	FbxString lMaterialName = matname;
	FbxString lShadingName = "Phong";
	FbxSurfacePhong* lMaterial = FbxSurfacePhong::Create(pScene, lMaterialName.Buffer());

	//	lMaterial->Diffuse.Set(fbxDouble3(mqomat->dif4f.r, mqomat->dif4f.g, mqomat->dif4f.b));
	lMaterial->Diffuse.Set(FbxDouble3(1.0, 1.0, 1.0));
	lMaterial->Emissive.Set(FbxDouble3(0.0, 0.0, 0.0));
	lMaterial->Ambient.Set(FbxDouble3(0.0, 0.0, 0.0));
	lMaterial->AmbientFactor.Set(1.0);
	//KFbxTexture* curtex = CreateTexture(pSdkManager, xmat->pTextureFilename);
	//if (curtex){
	//	lMaterial->Diffuse.ConnectSrcObject(curtex);
	//}
	lMaterial->DiffuseFactor.Set(1.0);
	lMaterial->TransparencyFactor.Set(1.0);
	lMaterial->ShadingModel.Set(lShadingName);
	lMaterial->Shininess.Set(0.5);
	lMaterial->Specular.Set(FbxDouble3(0.0, 0.0, 0.0));
	lMaterial->SpecularFactor.Set(0.3);

	lNode->AddMaterial(lMaterial);
	// We are in eBY_POLYGON, so there's only need for index (a plane has 1 polygon).
	lMaterialElement->GetIndexArray().SetCount(lMesh->GetPolygonCount());
	// Set the Index to the material
	//for (int i = 0; i<lMesh->GetPolygonCount(); ++i){
	for (int i = 0; i<lMesh->GetPolygonCount(); i++){
		lMaterialElement->GetIndexArray().SetAt(i, 0);
	}

	return lNode;
}

void LinkDummyMeshToSkeleton(CFBXBone* fbxbone, FbxSkin* lSkin, FbxScene* pScene, FbxNode* pMesh, int bonecnt)
{
	FbxNode* lSkel;

	//if ((fbxbone->GetType() == FB_NORMAL) || (fbxbone->GetType() == FB_BUNKI_PAR) || (fbxbone->GetType() == FB_BUNKI_CHIL)){
		lSkel = fbxbone->GetSkelNode();
		if (!lSkel){
			_ASSERT(0);
			return;
		}

		FbxCluster *lCluster = FbxCluster::Create(pScene, "");
		lCluster->SetLink(lSkel);
		lCluster->SetLinkMode(FbxCluster::eTotalOne);

		int vsetno = bonecnt * 3;
		int vcnt;
		for (vcnt = 0; vcnt < 3; vcnt++){
			lCluster->AddControlPointIndex(vsetno, 1.0);
			vsetno++;
		}

		/*
		FbxAMatrix lXMatrix;
		lXMatrix.SetIdentity();
		lCluster->SetTransformMatrix(lXMatrix);

		FbxAMatrix lXMatrix2;
		lXMatrix2 = lSkel->EvaluateGlobalTransform();
		lCluster->SetTransformLinkMatrix(lXMatrix2);


		lSkin->AddCluster(lCluster);
		*/

		if (fbxbone->GetBone()){
			CBone* curbone = fbxbone->GetBone();
			if (curbone){
				ChaVector3 pos;
				pos = curbone->GetJointFPos();

				FbxAMatrix lXMatrix;
				lXMatrix.SetIdentity();
				lXMatrix[3][0] = -pos.x;
				lXMatrix[3][1] = -pos.y;
				lXMatrix[3][2] = -pos.z;
				lCluster->SetTransformMatrix(lXMatrix);

				lXMatrix.SetIdentity();
				lCluster->SetTransformLinkMatrix(lXMatrix);
			}
			else{
				FbxAMatrix lXMatrix;
				lXMatrix.SetIdentity();
				lCluster->SetTransformMatrix(lXMatrix);
				lCluster->SetTransformLinkMatrix(lXMatrix);
			}
			lSkin->AddCluster(lCluster);
		}
		else if (fbxbone->GetBvhElem()){
			CBVHElem* curbone = fbxbone->GetBvhElem();
			if (curbone){
				ChaVector3 pos;
				pos = curbone->GetPosition();

				FbxAMatrix lXMatrix;
				lXMatrix.SetIdentity();
				lXMatrix[3][0] = -pos.x;
				lXMatrix[3][1] = -pos.y;
				lXMatrix[3][2] = -pos.z;
				lCluster->SetTransformMatrix(lXMatrix);

				lXMatrix.SetIdentity();
				lCluster->SetTransformLinkMatrix(lXMatrix);
			}
			else{
				FbxAMatrix lXMatrix;
				lXMatrix.SetIdentity();
				lCluster->SetTransformMatrix(lXMatrix);
				lCluster->SetTransformLinkMatrix(lXMatrix);
			}
			lSkin->AddCluster(lCluster);
		}

		//}

	/*
	if (fbxbone->m_child){
		LinkDummyMeshToSkeleton(fbxbone->m_child, lSkin, pScene);
	}
	if (fbxbone->m_brother){
		LinkDummyMeshToSkeleton(fbxbone->m_brother, lSkin, pScene);
	}
	*/

}


static int WriteFBXAnimTra(CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer, int curmotid, int maxframe, int axiskind)
{
	FbxTime lTime;
	int lKeyIndex = 0;
	double timescale = 30.0;


	CBone* curbone = fbxbone->GetBone();
	if (curbone){
		FbxNode* lSkel;
		lSkel = fbxbone->GetSkelNode();
		if (!lSkel){
			_ASSERT(0);
			return 1;
		}

		FbxAnimCurve* lCurve;
		int frameno;

		const char* strChannel;
		switch (axiskind){
		case AXIS_X:
			strChannel = FBXSDK_CURVENODE_COMPONENT_X;
			break;
		case AXIS_Y:
			strChannel = FBXSDK_CURVENODE_COMPONENT_Y;
			break;
		case AXIS_Z:
			strChannel = FBXSDK_CURVENODE_COMPONENT_Z;
			break;
		default:
			_ASSERT(0);
			strChannel = FBXSDK_CURVENODE_COMPONENT_X;
			break;
		}

		ChaVector3 fbxtra;
		lCurve = lSkel->LclTranslation.GetCurve(lAnimLayer, strChannel, true);
		lCurve->KeyModifyBegin();
		for (frameno = 0; frameno <= maxframe; frameno++){
			fbxtra = curbone->CalcFBXTra(curmotid, frameno);
			lTime.SetSecondDouble((double)frameno / timescale);
			lKeyIndex = lCurve->KeyAdd(lTime);
			switch (axiskind){
			case AXIS_X:
				lCurve->KeySetValue(lKeyIndex, fbxtra.x);
				break;
			case AXIS_Y:
				lCurve->KeySetValue(lKeyIndex, fbxtra.y);
				break;
			case AXIS_Z:
				lCurve->KeySetValue(lKeyIndex, fbxtra.z);
				break;
			default:
				_ASSERT(0);
				lCurve->KeySetValue(lKeyIndex, fbxtra.x);
				break;
			}
			lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);
		}
		lCurve->KeyModifyEnd();

	}

	return 0;
}
static int WriteFBXAnimRot(CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer, int curmotid, int maxframe, int axiskind)
{
	FbxTime lTime;
	int lKeyIndex = 0;
	double timescale = 30.0;


	CBone* curbone = fbxbone->GetBone();
	if (curbone){
		FbxNode* lSkel;
		lSkel = fbxbone->GetSkelNode();
		if (!lSkel){
			_ASSERT(0);
			return 1;
		}

		FbxAnimCurve* lCurve;
		int frameno;

		const char* strChannel;
		switch (axiskind){
		case AXIS_X:
			strChannel = FBXSDK_CURVENODE_COMPONENT_X;
			break;
		case AXIS_Y:
			strChannel = FBXSDK_CURVENODE_COMPONENT_Y;
			break;
		case AXIS_Z:
			strChannel = FBXSDK_CURVENODE_COMPONENT_Z;
			break;
		default:
			_ASSERT(0);
			strChannel = FBXSDK_CURVENODE_COMPONENT_X;
			break;
		}


		ChaVector3 befeul = ChaVector3(0.0f, 0.0f, 0.0f);
		ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);

		lCurve = lSkel->LclRotation.GetCurve(lAnimLayer, strChannel, true);
		lCurve->KeyModifyBegin();
		for (frameno = 0; frameno <= maxframe; frameno++){
			cureul = curbone->CalcFBXEul(curmotid, frameno, &befeul);
			lTime.SetSecondDouble((double)frameno / timescale);
			lKeyIndex = lCurve->KeyAdd(lTime);

			switch (axiskind){
			case AXIS_X:
				lCurve->KeySetValue(lKeyIndex, cureul.x);
				break;
			case AXIS_Y:
				lCurve->KeySetValue(lKeyIndex, cureul.y);
				break;
			case AXIS_Z:
				lCurve->KeySetValue(lKeyIndex, cureul.z);
				break;
			default:
				_ASSERT(0);
				lCurve->KeySetValue(lKeyIndex, cureul.x);
				break;
			}
			lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);
			befeul = cureul;
		}
		lCurve->KeyModifyEnd();
	}
	return 0;
}

int WriteFBXAnimTraOfBVH(CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer, int axiskind, int zeroflag)
{
	FbxTime lTime;
	int lKeyIndex = 0;
	FbxNode* lSkel = 0;
	double timescale = 30.0;

	CBVHElem* curbe = fbxbone->GetBvhElem();
	_ASSERT(curbe);
	if (curbe){

		lSkel = fbxbone->GetSkelNode();
		if (!lSkel){
			_ASSERT(0);
			return 1;
		}
		ChaVector3 difftra, orgtra, settra;
		CBVHElem* parbe;// = curbe->GetParent();
		if (fbxbone->GetParent()){
			parbe = fbxbone->GetParent()->GetBvhElem();
		}
		else{
			parbe = 0;
		}
		if (parbe){
			if (parbe == s_fbxbone->GetBvhElem()){
				orgtra = curbe->GetPosition();
			}
			else{
				orgtra = curbe->GetPosition() - parbe->GetPosition();
			}
		}
		else{
			orgtra = curbe->GetPosition();
		}

		const char* strChannel;
		switch (axiskind){
		case AXIS_X:
			strChannel = FBXSDK_CURVENODE_COMPONENT_X;
			break;
		case AXIS_Y:
			strChannel = FBXSDK_CURVENODE_COMPONENT_Y;
			break;
		case AXIS_Z:
			strChannel = FBXSDK_CURVENODE_COMPONENT_Z;
			break;
		default:
			_ASSERT(0);
			strChannel = FBXSDK_CURVENODE_COMPONENT_X;
			break;
		}

		FbxAnimCurve* lCurve;
		int frameno;

		lCurve = lSkel->LclTranslation.GetCurve(lAnimLayer, strChannel, true);
		lCurve->KeyModifyBegin();
		for (frameno = 0; frameno <= curbe->GetFrameNum(); frameno++){
			curbe->GetTrans(frameno, &difftra);
			lTime.SetSecondDouble((double)frameno / timescale);
			lKeyIndex = lCurve->KeyAdd(lTime);
			if (zeroflag == 1){
				lCurve->KeySetValue(lKeyIndex, 0.0f);
			}
			else{
				switch (axiskind){
				case AXIS_X:
					lCurve->KeySetValue(lKeyIndex, difftra.x + orgtra.x);
					break;
				case AXIS_Y:
					lCurve->KeySetValue(lKeyIndex, difftra.y + orgtra.y);
					break;
				case AXIS_Z:
					lCurve->KeySetValue(lKeyIndex, difftra.z + orgtra.z);
					break;
				default:
					_ASSERT(0);
					lCurve->KeySetValue(lKeyIndex, difftra.x + orgtra.x);
					break;
				}
			}
			lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);
		}
		lCurve->KeyModifyEnd();
	}

	return 0;
}

int WriteFBXAnimRotOfBVH(CFBXBone* fbxbone, FbxAnimLayer* lAnimLayer, int axiskind, int zeroflag)
{
	FbxTime lTime;
	int lKeyIndex = 0;
	FbxNode* lSkel = 0;
	double timescale = 30.0;

	CBVHElem* curbe = fbxbone->GetBvhElem();
	_ASSERT(curbe);
	if (curbe){

		lSkel = fbxbone->GetSkelNode();
		if (!lSkel){
			_ASSERT(0);
			return 1;
		}

		FbxAnimCurve* lCurve;
		int frameno;
		float curval;

		const char* strChannel;
		switch (axiskind){
		case AXIS_X:
			strChannel = FBXSDK_CURVENODE_COMPONENT_X;
			break;
		case AXIS_Y:
			strChannel = FBXSDK_CURVENODE_COMPONENT_Y;
			break;
		case AXIS_Z:
			strChannel = FBXSDK_CURVENODE_COMPONENT_Z;
			break;
		default:
			_ASSERT(0);
			strChannel = FBXSDK_CURVENODE_COMPONENT_X;
			break;
		}

		lCurve = lSkel->LclRotation.GetCurve(lAnimLayer, strChannel, true);
		lCurve->KeyModifyBegin();
		for (frameno = 0; frameno <= curbe->GetFrameNum(); frameno++){
			lTime.SetSecondDouble((double)frameno / timescale);
			lKeyIndex = lCurve->KeyAdd(lTime);
			if (zeroflag == 1){
				curval = 0.0f;
			}
			else{
				switch (axiskind){
				case AXIS_X:
					curval = curbe->GetZxyRot(frameno, ROTAXIS_X);
					break;
				case AXIS_Y:
					curval = curbe->GetZxyRot(frameno, ROTAXIS_Y);
					break;
				case AXIS_Z:
					curval = curbe->GetZxyRot(frameno, ROTAXIS_Z);
					break;
				default:
					_ASSERT(0);
					curval = curbe->GetZxyRot(frameno, ROTAXIS_X);
					break;
				}
			}
			lCurve->KeySetValue(lKeyIndex, curval);
			lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);
		}
		lCurve->KeyModifyEnd();
	}

	return 0;
}



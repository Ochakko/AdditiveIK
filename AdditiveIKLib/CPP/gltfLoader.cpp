#include "stdafx.h"
//#include <stdafx.h>

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


#define GLTFLOADERIMPL
#include <gltfLoader.h>

#include <Model.h>
#include <polymesh3.h>
#include <polymesh4.h>
#include <ExtLine.h>

#include <GetMaterial.h>

#include <mqofile.h>
#include <mqomaterial.h>
#include <mqoobject.h>

#include <Bone.h>
#include <mqoface.h>

#include <InfoWindow.h>

#include <FbxFile.h>

#include <EGPFile.h>

#define DBGH
#include <dbg.h>

//#include <DXUT.h>
//#include <DXUTcamera.h>
//#include <DXUTgui.h>
//#include <DXUTsettingsdlg.h>
//#include <SDKmisc.h>

#include <DispObj.h>
#include <MySprite.h>

#include <MotionPoint.h>
//#include <quaternion.h>
//#include <ChaVecCalc.h>//TheadingCalcEul.hでChaCalcFunc.hからChaVecCalc.hはインクルート

#include <Collision.h>
#include <EngName.h>

#include <RigidElem.h>
#include <ChaScene.h>
#include <CSChkInView.h>
#include <FootRigDlg.h>

#include <string>

#include <fbxsdk.h>
#include <fbxsdk/scene/shading/fbxlayeredtexture.h>
#include <fbxsdk/scene/animation/fbxanimevaluator.h>

#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"


#include <BopFile.h>
#include <BtObject.h>
#include <FbxMisc.h>

#include <collision.h>
#include <EditRange.h>
//#include <BoneProp.h>

#include <ThreadingLoadFbx.h>
#include <ThreadingUpdateMatrix.h>
#include <ThreadingPostIK.h>
#include <ThreadingFKTra.h>
#include <ThreadingCopyW2LW.h>
#include <ThreadingCalcEul.h>
#include <ThreadingRetarget.h>
#include <ThreadingInitMp.h>

#include <NodeOnLoad.h>

#include <OrgWindow.h>

#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"
#include "../../MiniEngine/TResourceBank.h"

//#include <DXUT.h>
#include <io.h>


//for __nop()
#include <intrin.h>


using namespace std;


CGltfLoader::CGltfLoader() : m_gltfmodel(), m_gltfloader()
{
	InitParams();
}
CGltfLoader::~CGltfLoader()
{
	DestroyObjs();
}

void CGltfLoader::InitParams()
{
	m_loadedflag = false;
	ZeroMemory(m_vrmpath, sizeof(WCHAR) * MAX_PATH);
	ZeroMemory(m_vrmname, sizeof(WCHAR) * MAX_PATH);
}
void CGltfLoader::DestroyObjs()
{
	m_loadedflag = false;
}

//int CGltfLoader::LoadTest()
//{
//    tinygltf::Model model;
//    tinygltf::TinyGLTF loader;
//    std::string err, warn;
//    //bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, "./Resource/Mesh/test_model/testmodel.gltf");
//    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, 
//        "C:/Users/info/Desktop/VRoid_VRM/Spring2_vrm1/Spring2_embed_vrm1.fbm/Spring2_vrm1.vrm");
//    if (!ret) {
//        bool ret2 = loader.LoadBinaryFromFile(&model, &err, &warn,
//            "C:/Users/info/Desktop/VRoid_VRM/Spring2_vrm1/Spring2_embed_vrm1.fbm/Spring2_vrm1.vrm");
//        if (!ret2) {
//            _ASSERT(0);
//            return 1;
//        }
//    }
//
//    return 0;
//}


int CGltfLoader::LoadEmbeddedVrm(const WCHAR* srcwfilename, const WCHAR* srcvrmname)
{
	m_loadedflag = false;

	if (!srcwfilename || !srcvrmname) {
		_ASSERT(0);
		return 1;
	}

	wcscpy_s(m_vrmpath, MAX_PATH, srcwfilename);
	wcscpy_s(m_vrmname, MAX_PATH, srcvrmname);


	WCHAR tmpwname[MAX_PATH] = { 0L };
	char filename[MAX_PATH] = { 0 };
	wcscpy_s(tmpwname, MAX_PATH, srcwfilename);
	WideCharToMultiByte(CP_ACP, 0, tmpwname, -1, filename, MAX_PATH, NULL, NULL);


	std::string err, warn;
	bool ret = m_gltfloader.LoadBinaryFromFile(&m_gltfmodel, &err, &warn, filename);
	if (!ret) {
	    _ASSERT(0);
	    return 1;
	}


	m_loadedflag = true;
	return 0;
}

int CGltfLoader::GetTexNameByMaterialName(int textype, const char* srcmaterialname, char* dsttexname, int dstleng)
{
	if ((textype < 0) || (textype > 2)) {
		_ASSERT(0);
		return 1;
	}
	if (!srcmaterialname || !dsttexname) {
		_ASSERT(0);
		return 1;
	}
	if (dstleng > 512) {
		_ASSERT(0);
		return 1;
	}
	if (!m_loadedflag) {
		_ASSERT(0);
		return 1;
	}


	char findmaterialname[MAX_PATH] = { 0 };
	strcpy_s(findmaterialname, MAX_PATH, srcmaterialname);
	char* strinstance = strstr(findmaterialname, "__Instance_");
	if (strinstance) {
		*strinstance = 0;
		strcat_s(findmaterialname, MAX_PATH, " (Instance)");
	}

	bool findflag = false;
	int materialnum = (int)m_gltfmodel.materials.size();
	int materialindex;
	for (materialindex = 0; materialindex < materialnum; materialindex++) {
		tinygltf::Material curmaterial = m_gltfmodel.materials[materialindex];
		int cmp0 = strcmp(curmaterial.name.c_str(), findmaterialname);
		if (cmp0 == 0) {
			int texindex = -1;
			switch (textype) {
			case 0:
				texindex = curmaterial.pbrMetallicRoughness.baseColorTexture.index;
				break;
			case 1:
				texindex = curmaterial.normalTexture.index;
				break;
			case 2:
				texindex = curmaterial.emissiveTexture.index;
				break;
			default:
				texindex = -1;
				break;
			}

			if ((texindex >= 0) && (texindex < (int)m_gltfmodel.textures.size())) {
				tinygltf::Texture curtexture = m_gltfmodel.textures[texindex];
				strcpy_s(dsttexname, dstleng, curtexture.name.c_str());
				findflag = true;//!!!!!!!!!!!!!!
				break;
			}
		}
	}

	if (findflag) {
		return 0;
	}
	else {
		_ASSERT(0);
		return 1;
	}


}

int CGltfLoader::GetImageByTexName(const char* srctexname, tinygltf::Image* dstimage)
{
	if (!srctexname) {
		_ASSERT(0);
		return 1;
	}

	int imagenum = (int)m_gltfmodel.images.size();
	int imageindex;
	for (imageindex = 0; imageindex < imagenum; imageindex++) {
		if (strcmp(srctexname, m_gltfmodel.images[imageindex].name.c_str()) == 0) {
			*dstimage = m_gltfmodel.images[imageindex];
			return 0;
		}
	}

	_ASSERT(0);//みつからなかった場合
	return 1;
}


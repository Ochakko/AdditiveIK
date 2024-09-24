#ifndef GLTFLOADERH
#define GLTFLOADERH

#include <wchar.h>
#include <string>
#include <map>

#ifdef GLTFLOADERIMPL
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#include <tiny_gltf.h>


class CGltfLoader
{
public:
	CGltfLoader();
	~CGltfLoader();

	//int LoadTest();
	int LoadEmbeddedVrm(const WCHAR* srcwfilename, const WCHAR* srcvrmname);
	int GetTexNameByMaterialName(int textype, const char* srcmaterialname, char* dsttexname, int dstleng);
	int GetImageByTexName(const char* srctexname, tinygltf::Image* dstimage);

public:
	bool GetLoadedFlag() {
		return m_loadedflag;
	};
	const WCHAR* GetVrmPath() {
		return m_vrmpath;
	};
	const WCHAR* GetVrmName() {
		return m_vrmname;
	};

private:
	void InitParams();
	void DestroyObjs();

private:
	bool m_loadedflag;
	tinygltf::Model m_gltfmodel;
	tinygltf::TinyGLTF m_gltfloader;
	WCHAR m_vrmpath[MAX_PATH];
	WCHAR m_vrmname[MAX_PATH];
};



#endif
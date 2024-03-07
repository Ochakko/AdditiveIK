#ifndef TRESOURCEBANKH
#define TRESOURCEBANKH

#include <windows.h>

#include <memory>
#include <vector>
#include <map>

//#include "../MiniEngine/MiniEngine.h"
//#include "d3dx12.h"

//#include "Texture.h"
//#include "Shader.h"
//#include "mqomaterial.h"


//template<class TResource>
//class TResourceBank {
//public:
//	TResourceBank() {
//		m_resourceMap.clear();
//	};
//	~TResourceBank() {
//		DestroyObjs();
//	};
//
//	void DestroyObjs() {
//		for (auto& it : m_resourceMap) {
//			TResource* delres = it.second;
//			if (delres) {
//				delete delres;
//			}
//		}
//		m_resourceMap.clear();
//	};
//
//	TResource* Get(const char* filePath)
//	{
//		auto it = m_resourceMap.find(filePath);
//		if (it != m_resourceMap.end()) {
//			return it->second;
//		}
//		return nullptr;
//	}
//	void Regist(const char* filePath, TResource* resource)
//	{
//		auto it = m_resourceMap.find(filePath);
//		if (it == m_resourceMap.end()) {
//			m_resourceMap.insert(
//				std::pair< std::string, TResource*>(filePath, resource)
//			);
//		}
//	}
//private:
//	std::map<std::string, TResource*> m_resourceMap;
//};


template<class TResource> class TResourceBank {
public:
	TResourceBank() {
		InitParams();
	};
	~TResourceBank() {
		DestroyObjs();
	};

	void InitParams() {
		m_resourceVec.clear();
		m_resourceMap.clear();
	};
	void DestroyObjs() {
		for (auto& it : m_resourceMap) {
			if (it.second && it.second.get()) {
				it.second.reset();
			}
		}
		//m_resourceVec.clear();
		//m_resourceMap.clear();
	};

	int GetSize() {
		return (int)m_resourceMap.size();
	};

	TResource* Get(const char* filePath)
	{
		auto it = m_resourceMap.find(filePath);
		if (it != m_resourceMap.end()) {
			return it->second.get();
		}
		return nullptr;
	}
	TResource* Get(int srcindex)
	{
		int resnum = GetSize();
		if ((srcindex >= 0) && (srcindex < resnum)) {
			return m_resourceVec[srcindex];
		}
		else {
			//_ASSERT(0);
			return 0;
		}

		//int resnum = GetSize();
		//if ((srcindex >= 0) && (srcindex < resnum)) {
		//	auto it = m_resourceMap.begin();
		//	if (it != m_resourceMap.end()) {
		//		int stepcount = 0;
		//		for (stepcount = 0; stepcount < srcindex; stepcount++) {
		//			it++;
		//		}
		//		if (it != m_resourceMap.end()) {
		//			return it->second.get();
		//		}
		//		else {
		//			return nullptr;
		//		}
		//	}
		//	else {
		//		return nullptr;
		//	}
		//}
		//else {
		//	_ASSERT(0);
		//	return nullptr;
		//}
	}

	void Regist(const char* filePath, TResource* resource)
	{
		auto it = m_resourceMap.find(filePath);
		if (it == m_resourceMap.end()) {
			//m_resourceMap.insert(
			//	std::pair<std::string, TResourcePtr>(filePath, resource);
			//);
			m_resourceMap[filePath] = TResourcePtr(resource);
			m_resourceVec.push_back(resource);//clear時に２回解放されないようにvectorは普通のポインタを格納
		}
	}
private:
	using TResourcePtr = std::unique_ptr<TResource>;
	std::map<std::string, TResourcePtr> m_resourceMap;//unique_ptr:空にすると削除される
	std::vector<TResource*> m_resourceVec;//普通のポインタ  indexで取得可能に
};

#endif

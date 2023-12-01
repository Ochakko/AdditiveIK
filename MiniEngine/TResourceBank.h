#pragma once

#include <memory>

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

template<class TResource>
class TResourceBank {
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
		m_resourceVec.clear();
		m_resourceMap.clear();
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
		if ((srcindex >= 0) && (resnum)) {
			return m_resourceVec[srcindex];
		}
		else {
			_ASSERT(0);
			return 0;
		}
	}

	void Regist(const char* filePath, TResource* resource)
	{
		auto it = m_resourceMap.find(filePath);
		if (it == m_resourceMap.end()) {
			m_resourceMap.insert(
				std::pair< std::string, TResourcePtr>(filePath, resource )
			);
			m_resourceVec.push_back(resource);
		}
	}
private:
	using TResourcePtr = std::unique_ptr<TResource> ;
	std::map<std::string, TResourcePtr> m_resourceMap;//unique_ptr:空にすると削除される
	std::vector<TResource*> m_resourceVec;//普通のポインタ  indexで取得可能に
};


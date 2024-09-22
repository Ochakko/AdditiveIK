#ifndef TEXELEMH
#define TEXELEMH

//class ID3D12Device;
//class ID3D11ShaderResourceView;
//class ID3D12Resource;
//class D3DXCOLOR;

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <wchar.h>

#include "../../MiniEngine/Texture.h"

class CGltfLoader;

class CTexElem
{
public:
	CTexElem();
	~CTexElem();

	int CreateTexData(ID3D12Device* pdev, CGltfLoader* srcloader);
	int InvalidateTexData();

private:
	int InitParams();
	int DestroyObjs();

	int SetNullTexture();

public:
	//ID3D11ShaderResourceView* GetPTex(){
	//	return m_ResView;
	//};
	//ID3D12Resource* GetPTex() {
	//	return m_ptex;
	//}
	Texture* GetPTex() {
		if (m_texture) {
			return m_texture;
		}
		else {
			//::MessageBox(NULL, L"AlbedoMap NULL error, exit app!!!", L"ERROR", MB_OK);
			//PostQuitMessage(404);
			return nullptr;
		}
	}

	int GetID(){
		return m_id;
	};
	void SetID( int srcval ){
		m_id = srcval;
	};

	const WCHAR* GetName(){
		return m_name;
	};
	void SetName( const WCHAR* srcval ){
		wcscpy_s( m_name, MAX_PATH, srcval );
	};

	const WCHAR* GetPath(){
		return m_path;
	};
	void SetPath( const WCHAR* srcval ){
		wcscpy_s( m_path, MAX_PATH, srcval );
	};

	void SetTransparent( int srcval ){
		m_transparent = srcval;
	};
	int GetTransparent(){
		return m_transparent;
	};

	int GetPool(){
		return m_pool;
	};
	void SetPool( int srcval ){
		m_pool = srcval;
	};

	bool IsValid() {
		return m_validflag;
	}
	void SetValidFlag(bool srcflag) {
		m_validflag = srcflag;
	}

	//void SetTransCol( D3DXCOLOR srcval ){
	//	m_transcol = srcval;
	//};

private:
	int m_id;
	WCHAR m_name[ MAX_PATH ];
	WCHAR m_path[ MAX_PATH ];
	int m_transparent;
	int m_pool;
	int m_orgheight, m_orgwidth;
	int m_height, m_width;

	//ID3D12Resource* m_ptex;
	//ID3D11ShaderResourceView* m_ResView;

	//ID3D12Resource* m_ptex;	//テクスチャ。
	//D3D12_RESOURCE_DESC m_textureDesc;	//テクスチャ情報
	Texture* m_texture;

	bool m_validflag;

};

#endif
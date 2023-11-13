#ifndef TEXELEMH
#define TEXELEMH

//class ID3D12Device;
//class ID3D11ShaderResourceView;
//class ID3D12Resource;
//class D3DXCOLOR;

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <wchar.h>

class CTexElem
{
public:
	CTexElem();
	~CTexElem();

	int CreateTexData( ID3D12Device* pdev );
	int InvalidateTexData();

private:
	int InitParams();
	int DestroyObjs();

public:
	//ID3D11ShaderResourceView* GetPTex(){
	//	return m_ResView;
	//};

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
	bool m_validflag;
};

#endif
#ifndef TEXBANKH
#define TEXBANKH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <wchar.h>
#include <unordered_map>
//using namespace std;


//class ID3D12Device;
//class D3DXCOLOR;
class CTexElem;
class CGltfLoader;

class CTexBank
{
public:
	CTexBank( ID3D12Device* pdev );
	~CTexBank();

	//int AddTex( WCHAR* srcpath, WCHAR* srcname, int srctransparent, int srcpool, D3DXCOLOR* srccol, int* dstid );
	int AddTex(CGltfLoader* srcloader,
		const WCHAR* srcpath, const WCHAR* srcname, int srctransparent, int srcpool, int* dstid);

	//int Invalidate( int invalmode );
	//int Restore(RenderContext* pRenderContext);
	//int DestroyTex( int srcid );

public:
	//accesser
	CTexElem* GetTexElem( int srcindex ){
		auto it = m_texmap.find(srcindex);
		if (it != m_texmap.end()) {
			//return it->second.get();
			return it->second;
		}
		else {
			return nullptr;
		}
	};

private:
	int InitParams();
	int DestroyObjs();

	CTexElem* ExistTex( const WCHAR* srcpath, const WCHAR* srcname, int srctransparent );

private:
	ID3D12Device* m_pdev;

	std::unordered_map<int, CTexElem*> m_texmap;
	//using TexElemPtr = std::unique_ptr<CTexElem>;
	//std::unordered_map<int, TexElemPtr> m_texmap;

};
#endif


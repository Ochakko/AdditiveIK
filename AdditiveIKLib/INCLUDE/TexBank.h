#ifndef TEXBANKH
#define TEXBANKH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <wchar.h>
#include <map>
//using namespace std;


//class ID3D12Device;
//class D3DXCOLOR;
class CTexElem;

class CTexBank
{
public:
	CTexBank( ID3D12Device* pdev );
	~CTexBank();

	//int AddTex( WCHAR* srcpath, WCHAR* srcname, int srctransparent, int srcpool, D3DXCOLOR* srccol, int* dstid );
	int AddTex(const WCHAR* srcpath, const WCHAR* srcname, int srctransparent, int srcpool, int* dstid);

	int Invalidate( int invalmode );
	int Restore(RenderContext* pRenderContext);

	int DestroyTex( int srcid );

public:
	//accesser
	CTexElem* GetTexElem( int srcindex ){
		return m_texmap[ srcindex ];
	};
	void GetTexElem2( std::map<int,CTexElem*>& dstmap ){
		dstmap = m_texmap;
	};

private:
	int InitParams();
	int DestroyObjs();

	CTexElem* ExistTex( const WCHAR* srcpath, const WCHAR* srcname, int srctransparent );

private:
	ID3D12Device* m_pdev;
	std::map<int, CTexElem*> m_texmap;
};
#endif

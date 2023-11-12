#ifndef MYSPRITEH
#define MYSPRITEH

//class ID3D11Device;
//struct D3D11_BUFFER_DESC;
//class ID3D11InputLayout;
//class ID3D11Buffer;//表示用頂点バッファ。
//class ID3D11Resource;
//class D3DXCOLOR;

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <Coef.h>


class CMySprite
{
public:
	CMySprite( ID3D11Device* pdev );
	~CMySprite();

	//int Create( WCHAR* srcpath, WCHAR* srcname, int srctransparent, int srcpool, D3DXCOLOR* srccol );
	int Create(ID3D11DeviceContext* pd3dImmediateContext, const WCHAR* srcpath, const WCHAR* srcname, int srctransparent, int srcpool);
	int CreateDecl();
	int SetPos( ChaVector3 srcpos );
	int SetSize( ChaVector2 srcsize );
	int SetColor( ChaVector4 srccol );
	int OnRender(ID3D11DeviceContext* pd3dImmediateContext, ID3D11Resource* ptex = 0 );

private:
	int InitParams();
	int DestroyObjs();

private:
	ID3D11Device* m_pdev;
	int m_texid;
	SPRITEV m_v[6];

	D3D11_BUFFER_DESC* m_BufferDesc;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_VB;//表示用頂点バッファ。

	ChaVector3 m_pos;
	ChaVector2 m_size;
	ChaVector4 m_col;
	ChaVector3 m_spriteoffset;
	ChaVector2 m_spritescale;
};

#endif
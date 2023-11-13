#ifndef FPSSPRITEH
#define FPSSPRITEH

#include <windows.h>

#include <ChaVecCalc.h>

class CMySprite;

class CFpsSprite
{
public:
	CFpsSprite();
	~CFpsSprite();

	int CreateSprites(ID3D12Device* pdev, WCHAR* mpath);
	void DestroySprites();
	int SetParams(int srcmainwidth, int srcmainheight);
	int Render(RenderContext* pd3dImmediateContext, int srcfps);

private:
	void InitParams();
	void DestroyObjs();

private:
	bool m_createdflag;
	CMySprite* m_sprite[11];
	int m_index[6];
	POINT m_point[6];//dispcenter
	ChaVector3 m_pos[6];
	ChaVector2 m_size_label;
	ChaVector2 m_size_num;
};



#endif
#ifndef FPSSPRITEH
#define FPSSPRITEH

#include <windows.h>

#include <ChaVecCalc.h>
#include "../MiniEngine/InstancedSprite.h"
#include "../MiniEngine/Texture.h"
#include "../MiniEngine/RenderContext.h"


class CFpsSprite
{
public:
	CFpsSprite();
	~CFpsSprite();

	int CreateSprites(WCHAR* mpath);
	void DestroySprites();
	int DrawScreen(RenderContext* rc, int srcfps);

private:
	void InitParams();
	void DestroyObjs();
	int SetParams();

private:
	bool m_createdflag;
	InstancedSprite m_sprite[11];
	Texture* m_texture[11];
	int m_index[6];
	POINT m_point[6];//dispcenter
	ChaVector3 m_pos[6];
	ChaVector2 m_size_label;
	ChaVector2 m_size_num;
};


#endif
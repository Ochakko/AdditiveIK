#ifndef UNDOSPRITEH
#define UNDOSPRITEH

#include <windows.h>

#include <ChaVecCalc.h>
#include "../MiniEngine/InstancedSprite.h"
#include "../MiniEngine/Texture.h"
#include "../MiniEngine/RenderContext.h"

class CUndoSprite
{
public:
	CUndoSprite();
	~CUndoSprite();

	int CreateSprites(WCHAR* mpath);
	void DestroySprites();
	int DrawScreen(RenderContext* rc, int undoR, int undoW);

private:
	void InitParams();
	void DestroyObjs();
	int SetParams();

private:
	bool m_createdflag;
	InstancedSprite m_sprite[12];
	Texture* m_texture[12];
	int m_indexR[4];
	int m_indexW[4];
	POINT m_pointR[4];//dispcenter
	POINT m_pointW[4];//dispcenter
	ChaVector3 m_posR[4];
	ChaVector3 m_posW[4];
	ChaVector2 m_size;
};



#endif
#ifndef THREADINGRENDERMODELSH
#define THREADINGRENDERMODELSH


#include <ThreadingBase.h>
//#include <ChaVecCalc.h>
#include <ChaCalcFunc.h>

#include <vector>

//スレッド１つの場合もあるので512までにした
//#define MAXBONEUPDATE	256
//#define MAXBONEUPDATE	512
//#define MAXBONEUPDATE	1024

class CModel;
class CBone;

class CThreadingRenderModels : public CThreadingBase
{
public:
	CThreadingRenderModels();
	virtual ~CThreadingRenderModels();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	void RenderModels(ChaScene* pchascene, myRenderer::RenderingEngine* Engine, RenderContext* rc);
	//bool IsFinished();

private:
	//static unsigned __stdcall ThreadFuncCaller(LPVOID lpThreadParam);
	virtual int ThreadFunc();


private:
	//CRITICAL_SECTION m_CritSection_UpdateMatrix;
	//HANDLE m_hEvent; //手動リセットイベント
	//HANDLE m_hExitEvent; //手動リセットイベント

	//HANDLE m_hthread;
	//LONG m_exit_state;
	//LONG m_start_state;

	ChaScene* m_chascene;
	myRenderer::RenderingEngine* m_Engine;
	RenderContext* m_rc;

	//int m_lightflag;
	//ChaVector4 m_diffusemult;
	//int m_btflag;


	//int updateslot;
};


#endif
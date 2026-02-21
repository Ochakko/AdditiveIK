#ifndef THREADINGUPDATEBLENDSHAPEH
#define THREADINGUPDATEBLENDSHAPEH

#include <ThreadingBase.h>
//#include <fbxsdk.h>


class CModel;
//class CBone;

class CThreadingUpdateBlendShape : public CThreadingBase
{
public:
	CThreadingUpdateBlendShape();
	virtual ~CThreadingUpdateBlendShape();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	//int ClearBoneList();
	//int SetBoneList(int srcindex, FbxNode* srcnode, CBone* srcbone);
	void UpdateBlendShape(CModel* srcmodel, int srcmotid, double srcframe);
	//bool IsFinished();

private:
	//static unsigned __stdcall ThreadFuncCaller(LPVOID lpThreadParam);
	virtual int ThreadFunc();

private:
	//CRITICAL_SECTION m_CritSection;
	//HANDLE m_hEvent; //手動リセットイベント
	//HANDLE m_hExitEvent; //手動リセットイベント

	//HANDLE m_hthread;
	//LONG m_exit_state;
	//LONG m_start_state;

	CModel* m_model;
	int m_motid;
	double m_frame;
};

#endif

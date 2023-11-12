#ifndef THREADINGBASEH
#define THREADINGBASEH

#include <Coef.h>
#include <Windows.h>

class CThreadingBase
{
public:
	CThreadingBase();
	~CThreadingBase();

	int CreateThread(DWORD affinitymask);
	virtual int InitParams();
	virtual void DestroyObjs();

	//int SetBoneList(int srcindex, FbxNode* srcnodeindex, CBone* srcbone);
	//void LoadFbxAnim(int srcanimno, int srcmotid, double srcanimleng);
	bool IsFinished();


public:
	//int GetBoneNum()
	//{
	//	return m_bonenum;
	//}

	//void SetModel(CModel* srcmodel)
	//{
	//	m_model = srcmodel;
	//}
	//CModel* GetModel()
	//{
	//	return m_model;
	//}

	//void SetScene(FbxScene* pscene)
	//{
	//	m_pscene = pscene;
	//}
	//FbxScene* GetScene()
	//{
	//	return m_pscene;
	//}
private:
	static unsigned __stdcall ThreadFuncCaller(LPVOID lpThreadParam);
	virtual int ThreadFunc();

protected:
	CRITICAL_SECTION m_CritSection;
	HANDLE m_hEvent; //手動リセットイベント
	HANDLE m_hExitEvent; //手動リセットイベント

	HANDLE m_hthread;
	LONG m_exit_state;
	LONG m_start_state;

	//FbxScene* m_pscene;
	//CModel* m_model;
	//int m_bonenum;
	//CBone* m_bonelist[MAXLOADFBXANIMBONE];
	//FbxNode* m_nodelist[MAXLOADFBXANIMBONE];

	//int m_animno;
	//int m_motid;
	//double m_animleng;
};

#endif
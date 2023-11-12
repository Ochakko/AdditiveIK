#ifndef THREADINGLOADFBXH
#define THREADINGLOADFBXH

#include <ThreadingBase.h>
#include <fbxsdk.h>


class CModel;
class CBone;


#ifndef MAXLOADFBXANIMBONE
#define MAXLOADFBXANIMBONE	1024
#endif

class CThreadingLoadFbx : public CThreadingBase
{
public:
	CThreadingLoadFbx();
	virtual ~CThreadingLoadFbx();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	int ClearBoneList();
	int SetBoneList(int srcindex, FbxNode* srcnode, CBone* srcbone);
	void LoadFbxAnim(int srcanimno, int srcmotid, double srcanimleng);
	//bool IsFinished();

public:
	int GetBoneNum()
	{
		return m_bonenum;
	}

	void SetModel(CModel* srcmodel)
	{
		m_model = srcmodel;
	}
	CModel* GetModel()
	{
		return m_model;
	}

	void SetScene(FbxScene* pscene)
	{
		m_pscene = pscene;
	}
	FbxScene* GetScene()
	{
		return m_pscene;
	}
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

	FbxScene* m_pscene;
	CModel* m_model;
	int m_bonenum;
	CBone* m_bonelist[MAXLOADFBXANIMBONE];
	FbxNode* m_nodelist[MAXLOADFBXANIMBONE];

	int m_animno;
	int m_motid;
	double m_animleng;
};

#endif

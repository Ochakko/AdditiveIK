#ifndef THREADINGUPDATEMATRIXH
#define THREADINGUPDATEMATRIXH


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

class CThreadingUpdateMatrix : public CThreadingBase
{
public:
	CThreadingUpdateMatrix();
	virtual ~CThreadingUpdateMatrix();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	int ClearBoneList();
	int SetModel(CModel* srcmodel);
	int AddBoneList(CBone* srcbone);
	void UpdateMatrix(bool limitdegflag, int srcmotid, double srcframe, ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat, int srcupdateslot = 0);
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
	CModel* m_model;

	//int m_bonenum;
	//CBone* m_bonelist[MAXBONEUPDATE];
	std::vector<CBone*> m_bonevec;

	int motid;
	double frame;
	ChaMatrix wmat;
	ChaMatrix vmat;
	ChaMatrix pmat;
	bool m_limitdegflag;
	int updateslot;
};


#endif
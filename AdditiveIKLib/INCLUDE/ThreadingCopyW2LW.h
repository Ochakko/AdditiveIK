#ifndef THREADINGCOPYW2LWH
#define THREADINGCOPYW2LWH


#include <ThreadingBase.h>
//#include <ChaVecCalc.h>
#include <ChaCalcFunc.h>

#include <vector>

////スレッド１つの場合もあるので512までにした
////#define MAXBONEUPDATE	256
////#define MAXBONEUPDATE	512
//#define MAXBONEUPDATE	1024


class CModel;
class CBone;

class CThreadingCopyW2LW : public CThreadingBase
{
public:
	CThreadingCopyW2LW();
	virtual ~CThreadingCopyW2LW();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	int ClearFrameList();
	int SetModel(CModel* srcmodel);
	int AddFramenoList(double srcframeno);
	void CopyWorldToLimitedWorld(CBone* srcbone, int srcmotid, double srcstartframe, double srcendframe);

private:
	//static unsigned __stdcall ThreadFuncCaller(LPVOID lpThreadParam);
	virtual int ThreadFunc();


private:
	////CRITICAL_SECTION m_CritSection_UpdateMatrix;
	////HANDLE m_hEvent; //手動リセットイベント
	////HANDLE m_hExitEvent; //手動リセットイベント

	////HANDLE m_hthread;
	////LONG m_exit_state;
	////LONG m_start_state;

	CModel* m_model;
	CBone* pbone;
	int motid;
	double startframe;
	double endframe;

	std::vector<double> m_framenovec;
};


#endif
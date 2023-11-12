#ifndef THREADINGINITMPH
#define THREADINGINITMPH


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

class CThreadingInitMp : public CThreadingBase
{
public:
	CThreadingInitMp();
	virtual ~CThreadingInitMp();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	int ClearFrameList();
	int SetModel(CModel* srcmodel);
	int AddFramenoList(double srcframeno);
	void InitMPReq(bool srclimitdegflag, CBone* srcbone, int srcmotid);

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

	int limitdegflag;
	CBone* pbone;
	int motid;

	std::vector<double> m_framenovec;
};


#endif
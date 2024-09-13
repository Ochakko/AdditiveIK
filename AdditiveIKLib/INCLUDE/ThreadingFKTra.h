#ifndef THREADINGFKTRAH
#define THREADINGFKTRAH


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

class CThreadingFKTra : public CThreadingBase
{
public:
	CThreadingFKTra();
	virtual ~CThreadingFKTra();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	int ClearFrameList();
	int SetModel(CModel* srcmodel);
	int AddFramenoList(double srcframeno);
	void FKBoneTraOneFrame(CModel* srcmodel, bool srclimitdegflag, int srcwallscrapingikflag, 
		CEditRange* srcerptr,
		int srcboneno, int srcmotid, ChaVector3 srcaddtra);

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
	int wallscrapingikflag;
	CEditRange* erptr;
	int boneno;
	int motid;
	ChaVector3 addtra;

	std::vector<double> m_framenovec;
};


#endif
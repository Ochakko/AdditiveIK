#ifndef THREADINGPOSTIKH
#define THREADINGPOSTIKH


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

class CThreadingPostIK : public CThreadingBase
{
public:
	CThreadingPostIK();
	virtual ~CThreadingPostIK();

	//int CreateThread();
	virtual int InitParams();
	virtual void DestroyObjs();

	int ClearFrameList();
	int SetModel(CModel* srcmodel);
	int AddFramenoList(double srcframeno);
	void IKRotateOneFrame(CModel* srcmodel, int srclimitdegflag, CEditRange* srcerptr,
		int srckeyno, CBone* srcrotbone, CBone* srcparentbone,
		int srcmotid, double srcstartframe, double srcapplyframe,
		CQuaternion srcrotq0, bool srckeynum1flag, bool srcpostflag, bool srcfromiktarget);

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
	CEditRange* erptr;
	int keyno;
	CBone* rotbone;
	CBone* parentbone;
	int motid;
	double startframe;
	double applyframe;
	CQuaternion rotq0;
	bool keynum1flag;
	bool postflag;
	bool fromiktarget;

	std::vector<double> m_framenovec;
};


#endif
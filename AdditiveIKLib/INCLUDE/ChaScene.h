#ifndef CHASCENEH
#define CHASCENEH


#include <wchar.h>
#include <string>
#include <map>

#include <Coef.h>
#include <OrgWindow.h>

#include <ChaVecCalc.h>
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"

#include <fbxsdk.h>


#include <UndoMotion.h>
#include <CameraFbx.h>

#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"
#include "../../AdditiveIKLib/Grimoire/ModelRender.h"
#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"

class CModel;
class BPWorld;
class CThreadingMotion2Bt;
class CThreadingSetBtMotion;

class ChaScene
{
public:
	ChaScene();
	~ChaScene();

	void ClearModelIndex()
	{
		m_modelindex.clear();
	};
	int AddModelElem(MODELELEM srcmodelelem)
	{
		if (srcmodelelem.modelptr) {
			m_modelindex.push_back(srcmodelelem);

			CreateMotion2BtThreads();
			CreateSetBtMotionThreads();

			return 0;
		}
		else {
			_ASSERT(0);
			return 1;
		}
	};
	//int AddToModelVec(CModel* srcmodel)
	//{
	//	if (srcmodel) {
	//		MODELELEM addelem;
	//		addelem.Init();
	//		addelem.modelptr = srcmodel;
	//		m_modelindex.push_back(addelem);
	//		return 0;
	//	}
	//	else {
	//		_ASSERT(0);
	//		return 1;
	//	}
	//};

	int UpdateMatrixModels(bool limitdegflag, ChaMatrix* vpmat, double srcframe);
	int UpdateMatrixOneModel(CModel* srcmodel, bool limitdegflag, ChaMatrix* wmat, ChaMatrix* vpmat, double srcframe);
	int WaitUpdateThreads();
	int SetBoneMatrixForShader(int btflag, bool calcslotflag);
	int RenderModels(myRenderer::RenderingEngine* renderringEngine, int lightflag, ChaVector4 diffusemult, int btflag = 0);
	void WaitForUpdateMatrixModels();
	int RenderOneModel(CModel* srcmodel, bool forcewithalpha, myRenderer::RenderingEngine* renderringEngine, 
		int lightflag, ChaVector4 diffusemult, int btflag, bool zcmpalways);


	CModel* GetTheLastCameraModel();
	int CalcTotalModelBound();
	const WCHAR* GetModelFileName(int srcmodelindex);

	int DelModel(int srcmodelindex);
	int DelAllModel();

	int StopBt();
	int StartBt();

	int SetMotionSpeed(int srcmodelindex, double srcspeed);
	int SetMotionFrame(int srcmodelindex, double srcframe);
	int SetENullTime(int srcmodelindex, double srcframe);

	int UpdateBtFunc(bool limitdegflag, double nextframe, ChaMatrix* pmVP, int loopstartflag,
		CModel* smodel, bool recstopflag, BPWorld* bpWorld, double srcreccnt,
		int (*srcStopBtRec)());

private:
	void InitParams();
	void DestroyObjs();

	int AddModelBound(MODELBOUND* mb, MODELBOUND* addmb);

	void SetKinematicToHand(CModel* srcmodel, bool srcflag);
	void SetKinematicToHandReq(CModel* srcmodel, CBone* srcbone, bool srcflag);

	int Motion2Bt(bool limitdegflag, double nextframe, ChaMatrix* pmVP, int loopstartflag);
	int SetBtMotion(bool limitdegflag, double nextframe, ChaMatrix* pmVP, CModel* smodel, double srcreccnt);


	int CreateMotion2BtThreads();
	int DestroyMotion2BtThreads();
	void WaitMotion2BtFinished();

	int CreateSetBtMotionThreads();
	int DestroySetBtMotionThreads();
	void WaitSetBtMotionFinished();


public:
	int GetUpdateSlot()
	{
		return m_updateslot;
	}
	bool ModelEmpty()
	{
		if (m_modelindex.empty()) {
			return true;
		}
		else {
			return false;
		}
	};
	int GetModelNum()
	{
		if (m_modelindex.empty()) {
			return 0;
		}
		else {
			return (int)m_modelindex.size();
		}
	};
	CModel* GetModel(int srcmodelindex)
	{
		if (m_modelindex.empty()) {
			return 0;
		}
		else if ((srcmodelindex >= 0) && (srcmodelindex < m_modelindex.size())) {
			return m_modelindex[srcmodelindex].modelptr;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};
	MODELELEM GetModelElem(int srcmodelindex)
	{
		MODELELEM retelem;
		retelem.Init();

		if (m_modelindex.empty()) {
			retelem.Init();
			return retelem;
		}
		else if ((srcmodelindex >= 0) && (srcmodelindex < m_modelindex.size())) {
			return m_modelindex[srcmodelindex];
		}
		else {
			_ASSERT(0);
			retelem.Init();
			return retelem;
		}
	};
	int GetModelIndex(std::vector<MODELELEM>& dstmodelindex)
	{
		dstmodelindex = m_modelindex;
		return 0;
	}
	MODELBOUND GetTotalModelBound()
	{
		return m_totalmb;
	};
	int SetTimelineArray(int srcmodelindex, std::vector<TLELEM> srctlarray)
	{
		if (m_modelindex.empty()) {
			_ASSERT(0);
			return 1;
		}
		else if ((srcmodelindex >= 0) && (srcmodelindex < m_modelindex.size())) {
			m_modelindex[srcmodelindex].tlarray = srctlarray;
			return 0;
		}
		else {
			_ASSERT(0);
			return 1;
		}
	};
	int GetTimelineArray(int srcmodelindex, std::vector<TLELEM>& dsttlarray)
	{
		if (m_modelindex.empty()) {
			_ASSERT(0);
			return 1;
		}
		else if ((srcmodelindex >= 0) && (srcmodelindex < m_modelindex.size())) {
			dsttlarray = m_modelindex[srcmodelindex].tlarray;
			return 0;
		}
		else {
			_ASSERT(0);
			return 1;
		}

	};
	int SetLineno2Boneno(int srcmodelindex, std::map<int, int> srclineno2boneno, std::map<int, int> srcboneno2lineno)
	{
		if (m_modelindex.empty()) {
			_ASSERT(0);
			return 1;
		}
		else if ((srcmodelindex >= 0) && (srcmodelindex < m_modelindex.size())) {
			m_modelindex[srcmodelindex].lineno2boneno = srclineno2boneno;
			m_modelindex[srcmodelindex].boneno2lineno = srcboneno2lineno;
			return 0;
		}
		else {
			_ASSERT(0);
			return 1;
		}
	};
	int GetLineno2Boneno(int srcmodelindex, std::map<int, int>& dstlineno2boneno, std::map<int, int>& dstboneno2lineno)
	{
		if (m_modelindex.empty()) {
			_ASSERT(0);
			return 1;
		}
		else if ((srcmodelindex >= 0) && (srcmodelindex < m_modelindex.size())) {
			dstlineno2boneno = m_modelindex[srcmodelindex].lineno2boneno;
			dstboneno2lineno = m_modelindex[srcmodelindex].boneno2lineno;
			return 0;
		}
		else {
			_ASSERT(0);
			return 1;
		}
	};
	int GetMotMenuIndex(int srcmodelindex)
	{
		if (m_modelindex.empty()) {
			_ASSERT(0);
			return 0;
		}
		else if ((srcmodelindex >= 0) && (srcmodelindex < m_modelindex.size())) {
			return m_modelindex[srcmodelindex].motmenuindex;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};

	//void SetRenderingEngine(myRenderer::RenderingEngine* prenderingEngine)
	//{
	//	m_renderingEngine = prenderingEngine;
	//}

private:
	std::vector<MODELELEM> m_modelindex;
	MODELBOUND	m_totalmb;
	int m_curmodelmenuindex;


	int m_totalupdatethreadsnum;
	int m_updateslot;

	CThreadingMotion2Bt* m_Motion2BtThreads;//モデル数分配列
	CThreadingSetBtMotion* m_SetBtMotionThreads;//モデル数分配列
	int m_created_Motion2BtThreadsNum;
	int m_created_SetBtMotionThreadsNum;

	//myRenderer::RenderingEngine* m_renderingEngine;


};


#endif

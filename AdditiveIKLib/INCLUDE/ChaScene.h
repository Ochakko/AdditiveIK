#ifndef CHASCENEH
#define CHASCENEH


#include <wchar.h>
#include <string>
#include <unordered_map>

#include <Coef.h>
#include <OrgWindow.h>

#include <ChaVecCalc.h>
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"

#include <fbxsdk.h>


#include <UndoMotion.h>
#include <CameraFbx.h>
#include <GrassElem.h>

#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"
//#include "../../AdditiveIKLib/Grimoire/ModelRender.h"
//#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"

#include <vector>


class CModel;
class BPWorld;
class CThreadingMotion2Bt;
class CThreadingSetBtMotion;
class CFootRigDlg;

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


	void SetUpdateSlot();
	void ResetCSFirstDispatchFlag();
	int UpdateMatrixModels(bool limitdegflag, ChaMatrix* vmat, ChaMatrix* pmat, double srcframe, int loopstartflag);
	int UpdateMatrixOneModel(CModel* srcmodel, bool limitdegflag, 
		ChaMatrix* wmat, ChaMatrix* vmat, ChaMatrix* pmat, double srcframe, int refposindex);
	int WaitUpdateThreads();
	int SetBoneMatrixForShader(int btflag, bool calcslotflag);
	int RenderModels(myRenderer::RenderingEngine* Engine, int lightflag, ChaVector4 diffusemult, int btflag = 0);
	void WaitForUpdateMatrixModels();
	int RenderOneModel(CModel* srcmodel, bool forcewithalpha, myRenderer::RenderingEngine* renderingEngine, 
		int lightflag, ChaVector4 diffusemult, int btflag, 
		bool zcmpalways, bool zenable,
		int refposindex = 0);
	int RenderInstancingModel(CModel* srcmodel, bool forcewithalpha,
		int lightflag, ChaVector4 diffusemult, int btflag, 
		bool zcmpalways, bool zenable, int renderkind);


	int ResetRefPos();
	int AddToRefPos(CModel* srcmodel, bool forcewithalpha, myRenderer::RenderingEngine* renderingEngine,
		int lightflag, ChaVector4 diffusemult, int btflag,
		bool zcmpalways, bool zenable,
		int refposindex);
	int RenderRefPos(myRenderer::RenderingEngine* renderingEngine, bool opaqueflag);


	CModel* GetTheLastCameraModel();
	int CalcTotalModelBound();
	const WCHAR* GetModelFileName(int srcmodelindex);

	int DelModel(int srcmodelindex, std::vector<CGrassElem*>& grasselemvec);
	int DelAllModel();

	int StopBt();
	int StartBt();

	int SetMotionSpeed(int srcmodelindex, double srcspeed);
	int SetMotionFrame(int srcmodelindex, double srcframe);
	int SetENullTime(int srcmodelindex, double srcframe);

	int UpdateBtFunc(bool limitdegflag, double nextframe, 
		ChaMatrix* pmView, ChaMatrix* pmProj, int loopstartflag,
		CModel* smodel, bool recstopflag, BPWorld* bpWorld, double srcreccnt,
		int (*srcStopBtRec)());

	bool PickPolyMesh(int pickkind, UIPICKINFO* tmppickinfo,
		CModel** pickmodel, CMQOObject** pickmqoobj, CMQOMaterial** pickmaterial, ChaVector3* pickhitpos);
	bool GetResultOfPickRay(int pickkind,
		CModel** pickmodel, CMQOObject** pickmqoobj, CMQOMaterial** pickmaterial, ChaVector3* pickhitpos);


	int CopyCSDeform();

	int InitializeBoneAxisKind();
	int ChangeAngleSpringScale();

	int SetRefPosFlag(CModel* srcmodel);

	bool IsCameraModel(CModel* srcmodel);
	bool IsCameraMotion(CModel* srcmodel, int srcmotid);


	int FindModelIndex(CModel* srcmodel);

private:
	void InitParams();
	void DestroyObjs();

	int AddModelBound(MODELBOUND* mb, MODELBOUND* addmb);

	void SetKinematicToHand(CModel* srcmodel, bool srcflag);
	void SetKinematicToHandReq(CModel* srcmodel, CBone* srcbone, bool srcflag);

	int Motion2Bt(bool secondcall, bool limitdegflag, bool updatematrixflag, double nextframe, 
		ChaMatrix* pmView, ChaMatrix* pmProj, int loopstartflag);
	int SetBtMotion(bool limitdegflag, double nextframe, 
		ChaMatrix* pmView, ChaMatrix* pmProj, CModel* smodel, double srcreccnt);


	int CreateMotion2BtThreads();
	int DestroyMotion2BtThreads();
	void WaitMotion2BtFinished();

	int CreateSetBtMotionThreads();
	int DestroySetBtMotionThreads();
	void WaitSetBtMotionFinished();


public:
	void SetFootRigDlg(CFootRigDlg* srcfootrigdlg)
	{
		m_footrigdlg = srcfootrigdlg;
	};
	int GetUpdateSlot()
	{
		return m_updateslot;
	};
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
	CModel* GetModel(WCHAR* wfilename)
	{
		if (wfilename) {
			int modelnum = GetModelNum();
			int modelindex;
			for (modelindex = 0; modelindex < modelnum; modelindex++) {
				CModel* chkmodel = GetModel(modelindex);
				if (chkmodel && (wcscmp(wfilename, chkmodel->GetFileName()) == 0)) {
					return chkmodel;
				}
			}
			return nullptr;
		}
		else {
			return nullptr;
		}
	};
	CModel* GetModel(char* srcfolder, char* srcfilename)
	{
		if ((srcfilename == nullptr) || (srcfolder == nullptr)) {
			_ASSERT(0);
			return nullptr;
		}

		WCHAR wfolder[MAX_PATH] = { 0L };
		WCHAR wfilename[MAX_PATH] = { 0L };
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, srcfolder, -1, wfolder, MAX_PATH);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, srcfilename, -1, wfilename, MAX_PATH);


		int modelnum = GetModelNum();
		int modelindex;
		for (modelindex = 0; modelindex < modelnum; modelindex++) {
			CModel* chkmodel = GetModel(modelindex);
			if (chkmodel && (wcscmp(wfilename, chkmodel->GetFileName()) == 0) && (wcscmp(wfolder, chkmodel->GetModelFolder()) == 0)) {
				return chkmodel;
			}
		}

		_ASSERT(0);
		return nullptr;
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
	int SetLineno2Boneno(int srcmodelindex, std::unordered_map<int, int> srclineno2boneno, std::unordered_map<int, int> srcboneno2lineno)
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
	int GetLineno2Boneno(int srcmodelindex, std::unordered_map<int, int>& dstlineno2boneno, std::unordered_map<int, int>& dstboneno2lineno)
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
	int MotID2SelIndex(int srcmodelindex, int srcmotid);

	//void SetRenderingEngine(myRenderer::RenderingEngine* prenderingEngine)
	//{
	//	m_renderingEngine = prenderingEngine;
	//}

	void ClearRenderObjs()
	{
		//m_shadowmapModels.clear();
		//m_renderToGBufferModels.clear();
		m_forwardRenderModels.clear();
		m_instancingRenderModels.clear();
		//m_zprepassModels.clear();
		m_forwardRenderSprites.clear();
		m_forwardRenderFont.clear();

		m_forwardModelsReserveSize = 0;

		ResetRefPos();
	};


	void Add3DModelToForwardRenderPass(std::vector<myRenderer::RENDEROBJ>& rendervec)
	{
		//m_forwardRenderModels.push_back(renderobj);

		size_t srcsize = m_forwardRenderModels.size();
		size_t addsize = rendervec.size();
		size_t newsize = srcsize + addsize;

		if ((newsize > 0) && (newsize > m_forwardModelsReserveSize)) {
			m_forwardRenderModels.reserve(newsize);
			m_forwardModelsReserveSize = newsize;
		}

		////std::copy(rendervec.begin(), rendervec.end(), std::back_inserter(m_forwardRenderModels));

		//size_t addno;
		//for (addno = 0; addno < addsize; addno++) {
		//    m_forwardRenderModels[srcsize + addno] = rendervec[addno];
		//}

		size_t addno;
		for (addno = 0; addno < addsize; addno++) {
			m_forwardRenderModels.push_back(rendervec[addno]);
		}

	};
	void Add3DModelToInstancingRenderPass(myRenderer::RENDEROBJ renderobj)
	{
	    m_instancingRenderModels.push_back(renderobj);
	};
	void AddSpriteToForwardRenderPass(myRenderer::RENDERSPRITE rendersprite)
	{
		m_forwardRenderSprites.push_back(rendersprite);
	};
	void AddFontToForwardRenderPass(myRenderer::RENDERFONT renderfont)
	{
		m_forwardRenderFont.push_back(renderfont);
	};




	int GetForwardRenderObjNum()
	{
		return (int)m_forwardRenderModels.size();
	};
	myRenderer::RENDEROBJ GetForwardRenderObj(int srcindex)
	{
		myRenderer::RENDEROBJ retobj;
		retobj.Init();
		if ((srcindex >= 0) && (srcindex < GetForwardRenderObjNum())) {
			retobj = m_forwardRenderModels[srcindex];
		}
		return retobj;
	};

	int GetInstancingRenderObjNum()
	{
		return (int)m_instancingRenderModels.size();
	};
	myRenderer::RENDEROBJ GetInstancingRenderObj(int srcindex)
	{
		myRenderer::RENDEROBJ retobj;
		retobj.Init();
		if ((srcindex >= 0) && (srcindex < GetInstancingRenderObjNum())) {
			retobj = m_instancingRenderModels[srcindex];
		}
		return retobj;
	};

	int GetSpriteRenderObjNum()
	{
		return (int)m_forwardRenderSprites.size();
	};
	myRenderer::RENDERSPRITE GetSpriteRenderObj(int srcindex)
	{
		myRenderer::RENDERSPRITE retobj;
		retobj.Init();
		if ((srcindex >= 0) && (srcindex < GetSpriteRenderObjNum())) {
			retobj = m_forwardRenderSprites[srcindex];
		}
		return retobj;
	};

	int GetFontRenderObjNum()
	{
		return (int)m_forwardRenderFont.size();
	};
	myRenderer::RENDERFONT GetFontRenderObj(int srcindex)
	{
		myRenderer::RENDERFONT retobj;
		retobj.Init();
		if ((srcindex >= 0) && (srcindex < GetFontRenderObjNum())) {
			retobj = m_forwardRenderFont[srcindex];
		}
		return retobj;
	};

	void SetCurrentModel(CModel* srcmodel)
	{
		m_currentmodel = srcmodel;
	};
	CModel* GetCurrentModel() {
		return m_currentmodel;
	};

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

	//std::vector<RENDEROBJ> m_renderToGBufferModels;                  // Gバッファへの描画パスで描画するモデルのリスト
	std::vector<myRenderer::RENDEROBJ> m_forwardRenderModels;                    // フォワードレンダリングの描画パスで描画されるモデルのリスト
	size_t m_forwardModelsReserveSize;
	std::vector<myRenderer::RENDEROBJ> m_instancingRenderModels;                    // インスタンシングレンダリングの描画パスで描画されるモデルのリスト
	std::vector<myRenderer::RENDERSPRITE> m_forwardRenderSprites;
	std::vector<myRenderer::RENDERFONT> m_forwardRenderFont;

	std::vector<myRenderer::RENDEROBJ> m_refpos_opaque;
	std::vector<myRenderer::RENDEROBJ> m_refpos_transparent;

	//myRenderer::RenderingEngine* m_renderingEngine;

	CModel* m_currentmodel;


	CFootRigDlg* m_footrigdlg;//外部データ
};


#endif

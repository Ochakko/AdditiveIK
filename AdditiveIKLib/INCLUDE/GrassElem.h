#ifndef GRASSELEMH
#define GRASSELEMH


#include <wchar.h>
#include <string>
#include <map>

#include <Coef.h>
#include <OrgWindow.h>

#include <ChaVecCalc.h>

#include <fbxsdk.h>

#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"
//#include "../../AdditiveIKLib/Grimoire/ModelRender.h"
//#include "../../AdditiveIKLib/Grimoire/RenderingEngine.h"

#include <vector>


class CModel;
class ChaScene;

class CGrassElem
{
public:
	CGrassElem(CModel* srcmodel);
	~CGrassElem();

	int SetInstancingParams(ChaMatrix matVP);
	int RenderInstancingModel(ChaScene* srcchascene);

	int AddGrassPosition(ChaVector3 srcpos, ChaVector3 srcdir);
	int RemoveGrassPosition(ChaVector3 srcpos, float removedistance);

private:
	void InitParams();
	void DestroyObjs();

public:
	CModel* GetGrass() {
		return m_grass;
	};
	void ClearGrassMat() {
		m_grassmat.clear();
	};
	int GetGrassNum() {
		int grassnum = (int)m_grassmat.size();
		return grassnum;
	}
	int AddGrassMat(ChaMatrix srcmat) {
		int grassnum = GetGrassNum();
		if (grassnum < GRASSINDEXMAX) {
			m_grassmat.push_back(srcmat);
			return 0;
		}
		else {
			_ASSERT(0);
			return 1;
		}
	};
	ChaMatrix GetGrassMat(int srcindex) {
		int grassnum = GetGrassNum();
		if ((srcindex >= 0) && (srcindex < grassnum) && (srcindex < GRASSINDEXMAX)) {
			return m_grassmat[srcindex];
		}
		else {
			_ASSERT(0);
			ChaMatrix dummyret;
			dummyret.SetIdentity();
			return dummyret;
		}
	};

private:
	CModel* m_grass;
	std::vector<ChaMatrix> m_grassmat;
};


#endif

#ifndef RIGFILEH
#define RIGFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <Coef.h>
#include <XMLIO.h>

#include <map>
#include <string>

class CModel;
class CBone;

class CRigFile : public CXMLIO
{
public:
	CRigFile();
	virtual ~CRigFile();

	int WriteRigFile(WCHAR* strpath, CModel* srcmodel);
	int LoadRigFile(WCHAR* strpath, CModel* srcmodel);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	void WriteRigReq(CBone* srcbone);
	int WriteRig(CBone* srcbone);

	int ReadBone(XMLIOBUF* xmliobuf);
	int ReadRig(XMLIOBUF* xmliobuf, int elemno);

public:
	CUSTOMRIG m_customrig;

private:
	CModel* m_model;

};

#endif
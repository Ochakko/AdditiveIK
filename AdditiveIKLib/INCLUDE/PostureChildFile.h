#ifndef POSTURECHILDFILEH
#define POSTURECHILDFILEH

#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <unordered_map>


class CPostureChildFile : public CXMLIO
{
public:
	CPostureChildFile();
	virtual ~CPostureChildFile();

	int WritePostureChildFile(const WCHAR* srcfilepath, CModel* srcmodel, ChaScene* srcchascene);
	int LoadPostureChildFile(const WCHAR* srcfilepath, CModel* srcmodel, ChaScene* srcchascene);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	int WritePostureChildElem(CBone* srcbone, ChaScene* srcchascene);

	//int CheckFileVersion( XMLIOBUF* xmliobuf );
	//int ReadFileInfo( XMLIOBUF* xmliobuf, int* charanumptr );
	int ReadPostureChildElem(CModel* srcmodel, ChaScene* srcchascene, XMLIOBUF* xmlbuf);
	int ReadPostureCamera(CModel* srcmodel, XMLIOBUF* xmlbuf);
};

#endif
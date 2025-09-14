#ifndef DISPGROUPFILEH
#define DISPGROUPFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <Coef.h>
#include <XMLIO.h>

#include <vector>
#include <unordered_map>
#include <string>

class CModel;
class CMQOObject;

class CDispGroupFile : public CXMLIO
{
public:
	CDispGroupFile();
	virtual ~CDispGroupFile();

	int WriteDispGroupFile(WCHAR* strpath, CModel* srcmodel);
	int LoadDispGroupFile(WCHAR* strpath, CModel* srcmodel);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteDig(int groupindex);
	int ReadDig(XMLIOBUF* xmliobuf);

private:
	CModel* m_model;

};

#endif
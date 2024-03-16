#ifndef MNLFILEH
#define MNLFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <map>

class CModel;
class BPWorld;

class CMNLFile : public CXMLIO
{
public:
	CMNLFile();
	virtual ~CMNLFile();

	int WriteMNLFile(WCHAR* strpath, CModel* srcmodel);
	int LoadMNLFile(WCHAR* strpath, CModel* srcmodel);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	int WriteMNEntry(int mcnt, MOTINFO curmi);

	//int CheckFileVersion( XMLIOBUF* xmliobuf );
	int ReadFileInfo( XMLIOBUF* xmliobuf, int* charanumptr );
	int ReadMNEntry( int mnum, int mcnt, XMLIOBUF* xmliobuf );


private:
	CModel* m_model;

};

#endif
#ifndef DOFPARAMSFILEH
#define DOFPARAMSFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <map>

class CDofParamsFile : public CXMLIO
{
public:
	CDofParamsFile();
	virtual ~CDofParamsFile();

	int WriteDofParamsFile(WCHAR* filepath);
	int LoadDofParamsFile(WCHAR* filepath);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();

	////int CheckFileVersion( XMLIOBUF* xmliobuf );
	int ReadProjectInfo( XMLIOBUF* xmliobuf, int* charanumptr );

};

#endif
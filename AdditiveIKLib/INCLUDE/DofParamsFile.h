#ifndef DOFPARAMSFILEH
#define DOFPARAMSFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <unordered_map>

class CDofParamsFile : public CXMLIO
{
public:
	CDofParamsFile();
	virtual ~CDofParamsFile();

	int WriteDofParamsFile(WCHAR* filepath, int srcindex);
	int LoadDofParamsFile(WCHAR* filepath, int srcindex);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();

	////int CheckFileVersion( XMLIOBUF* xmliobuf );
	int ReadProjectInfo( XMLIOBUF* xmliobuf, int* charanumptr );

};

#endif
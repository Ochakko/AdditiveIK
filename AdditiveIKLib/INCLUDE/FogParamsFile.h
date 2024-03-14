#ifndef FOGPARAMSFILEH
#define FOGPARAMSFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <map>

class CFogParamsFile : public CXMLIO
{
public:
	CFogParamsFile();
	virtual ~CFogParamsFile();

	int WriteFogParamsFile(WCHAR* filepath);
	int LoadFogParamsFile(WCHAR* filepath);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();

	////int CheckFileVersion( XMLIOBUF* xmliobuf );
	int ReadProjectInfo( XMLIOBUF* xmliobuf, int* charanumptr );

};

#endif
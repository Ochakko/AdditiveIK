#ifndef SHADOWPARAMSFILEH
#define SHADOWPARAMSFILEH

#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <unordered_map>


class CShadowParamsFile : public CXMLIO
{
public:
	CShadowParamsFile();
	virtual ~CShadowParamsFile();

	int WriteShadowParamsFile(const WCHAR* srcfilepath);
	int LoadShadowParamsFile(const WCHAR* srcfilepath);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	//int WriteLight(int slotindex, int lightindex);

	//int CheckFileVersion( XMLIOBUF* xmliobuf );
	//int ReadFileInfo( XMLIOBUF* xmliobuf, int* charanumptr );
	//int ReadLight(int slotindex, int lightcnt, XMLIOBUF* xmlbuf);

};

#endif
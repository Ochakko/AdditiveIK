#ifndef LIGHTSFOREDITFILEH
#define LIGHTSFOREDITFILEH

#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <unordered_map>


class CLightsForEditFile : public CXMLIO
{
public:
	CLightsForEditFile();
	virtual ~CLightsForEditFile();

	int WriteLightsForEditFile(const WCHAR* srcfilepath, int slotindex);
	int LoadLightsForEditFile(const WCHAR* srcfilepath, int slotindex);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	int WriteLight(int slotindex, int lightindex);

	//int CheckFileVersion( XMLIOBUF* xmliobuf );
	//int ReadFileInfo( XMLIOBUF* xmliobuf, int* charanumptr );
	int ReadLight(int slotindex, int lightcnt, XMLIOBUF* xmlbuf);

};

#endif
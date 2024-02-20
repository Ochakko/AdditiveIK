#ifndef THRESHOLDFILEH
#define THRESHOLDFILEH

#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <map>


class CThresholdFile : public CXMLIO
{
public:
	CThresholdFile();
	virtual ~CThresholdFile();

	int WriteThresholdFile(const WCHAR* srcfilepath);
	int LoadThresholdFile(const WCHAR* srcfilepath);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	int WriteThreshold();

	//int CheckFileVersion( XMLIOBUF* xmliobuf );
	//int ReadFileInfo( XMLIOBUF* xmliobuf, int* charanumptr );
	int ReadThreshold(XMLIOBUF* xmlbuf);

};

#endif
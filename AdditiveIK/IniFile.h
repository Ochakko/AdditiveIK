#ifndef INIFILEH
#define INIFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <map>

class CModel;
class BPWorld;

class CIniFile : public CXMLIO
{
public:
	CIniFile();
	virtual ~CIniFile();

	int WriteIniFile(WCHAR* srcpath);
	int LoadIniFile(WCHAR* strpath);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	int WriteIniInfo();

	//int CheckFileVersion( XMLIOBUF* xmliobuf );
	int ReadFileInfo( XMLIOBUF* xmliobuf );
	int ReadIniInfo(XMLIOBUF* xmliobuf );

};

#endif
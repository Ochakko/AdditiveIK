#ifndef SKYPARAMSFILEH
#define SKYPARAMSFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <unordered_map>

#include <MQOMaterial.h>

class CSkyParamsFile : public CXMLIO
{
public:
	CSkyParamsFile();
	virtual ~CSkyParamsFile();

	int WriteSkyParamsFile(WCHAR* filepath, CShaderTypeParams srcparams, int srcindex);
	int LoadSkyParamsFile(WCHAR* filepath, CShaderTypeParams* dstparams);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	//int WriteChara(bool limitdegflag, MODELELEM* srcme, WCHAR* projname, std::unordered_map<CModel*, CFrameCopyDlg*> srcselbonedlgmap);

	////int CheckFileVersion( XMLIOBUF* xmliobuf );
	int ReadProjectInfo( XMLIOBUF* xmliobuf, int* charanumptr );
	//int ReadChara(bool limitdegflag, int charanum, int characnt, XMLIOBUF* xmliobuf);
	////int ReadMotion( XMLIOBUF* xmliobuf, WCHAR* modelfolder, CModel* modelptr );
	//int ReadWall(XMLIOBUF* xmliobuf);

};

#endif
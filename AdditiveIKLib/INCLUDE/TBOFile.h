#ifndef TBOFILEH
#define TBOFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <map>

class CModel;
class BPWorld;

class CTboFile : public CXMLIO
{
public:
	CTboFile();
	virtual ~CTboFile();

	int WriteTboFile( WCHAR* strpath, );
	int LoadTboFile( WCHAR* strpath, CModel* (*srcfbxfunc)( int skipdefref, int inittimelineflag ), int (*srcReffunc)(), int (*srcImpFunc)(), int (*srcGcoFunc)(), 
		int (*srcReMenu)( int selindex1, int callbymenu1 ), int (*srcRgdMenu)( int selindex2, int callbymenu2 ), int (*srcMorphMenu)( int selindex3 ), int (*srcImpMenu)( int selindex4 ) );

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	int WriteTbora( MODELELEM* srcme, WCHAR* projname );

	//int CheckFileVersion( XMLIOBUF* xmliobuf );
	int ReadProjectInfo( XMLIOBUF* xmliobuf, int* charanumptr );
	int ReadTbora( int charanum, int characnt, XMLIOBUF* xmliobuf );
	//int ReadMotion( XMLIOBUF* xmliobuf, WCHAR* modelfolder, CModel* modelptr );
	int ReadWall(XMLIOBUF* xmliobuf);


private:


};

#endif
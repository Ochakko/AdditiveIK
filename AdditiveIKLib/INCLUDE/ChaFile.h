#ifndef CHAFILEH
#define CHAFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <map>

class CModel;
class BPWorld;
class CFrameCopyDlg;
class CGrassElem;
class CFootRigDlg;

class CChaFile : public CXMLIO
{
public:
	CChaFile();
	virtual ~CChaFile();

	int WriteChaFile(bool limitdegflag, BPWorld* srcbpw, WCHAR* projdir, WCHAR* projname, 
		std::vector<MODELELEM>& srcmodelindex, float srcmotspeed, 
		std::map<CModel*, CFrameCopyDlg*> srcselbonedlgmap,
		std::vector<CGrassElem*> srcgrasselemvec);
	int LoadChaFile(bool limitdegflag, WCHAR* strpath,
		CFootRigDlg* srcfootrigdlg,
		CModel* (*srcfbxfunc)( bool callfromcha, bool dorefreshtl, int skipdefref, int inittimelineflag, std::vector<std::string> ikstopname, bool srcgrassflag ),
		int (*srcReffunc)(), int (*srcImpFunc)(), int (*srcGcoFunc)(),
		int (*srcReMenu)( int selindex1, int callbymenu1 ), 
		int (*srcRgdMenu)( int selindex2, int callbymenu2 ), 
		int (*srcMorphMenu)( int selindex3 ), int (*srcImpMenu)( int selindex4 ),
		std::vector<CGrassElem*>& dstgrasselemvec);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	int WriteChara(bool limitdegflag, MODELELEM* srcme, WCHAR* projname, 
		std::map<CModel*, CFrameCopyDlg*> srcselbonedlgmap,
		CGrassElem* srcgrasselem);

	//int CheckFileVersion( XMLIOBUF* xmliobuf );
	int ReadProjectInfo( XMLIOBUF* xmliobuf, int* charanumptr );
	int ReadChara(bool limitdegflag, int charanum, int characnt, std::vector<CGrassElem*>& dstgrasselemvec, XMLIOBUF* xmliobuf);
	//int ReadMotion( XMLIOBUF* xmliobuf, WCHAR* modelfolder, CModel* modelptr );
	int ReadWall(XMLIOBUF* xmliobuf);


private:
	std::vector<MODELELEM> m_modelindex;
	WCHAR m_newdirname[MAX_PATH];

	std::map<int, MOTINFO*> m_motinfo;
	WCHAR m_wloaddir[MAX_PATH];
	char m_mloaddir[MAX_PATH];

	CFootRigDlg* m_footrigdlg;//外部データ

	CModel* (*m_FbxFunc)(bool callfromcha, bool dorefreshtl, int skipdefref, int inittimelineflag, std::vector<std::string> ikstopname, bool srcgrassflag);
	int (*m_RefFunc)();
	int (*m_ImpFunc)();
	int (*m_GcoFunc)();
	int (*m_ReMenu)( int selindex, int callbymenu );
	int (*m_RgdMenu)( int selindex, int callbymenu );
	int (*m_MorphMenu)( int selindex );
	int (*m_ImpMenu)( int selindex );

	float m_motspeed;
};

#endif
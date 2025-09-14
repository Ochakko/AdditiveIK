#ifndef RETARGETFILEH
#define RETARGETFILEH

//#include <d3dx9.h>
#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <unordered_map>

class CModel;
class BPWorld;

class CRetargetFile : public CXMLIO
{
public:
	CRetargetFile();
	virtual ~CRetargetFile();

	int WriteRetargetFile(WCHAR* srcpath, CModel* srcmodel, CModel* srcbvh, std::unordered_map<CBone*, CBone*>& convbonemap);
	int LoadRetargetFile(WCHAR* strpath, CModel* srcmodel, CModel* srcbvh, std::unordered_map<CBone*, CBone*>& convbonemap);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	int WriteRetargetInfo(const char* modeljointname, const char* bvhjointname);

	//int CheckFileVersion( XMLIOBUF* xmliobuf );
	int ReadFileInfo( XMLIOBUF* xmliobuf );
	int ReadRetargetInfo( int jointcnt, XMLIOBUF* xmliobuf );

private:

	std::unordered_map<CBone*, CBone*> m_convbonemap;
	CModel* m_model;
	CModel* m_bvh;

};

#endif
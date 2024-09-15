#ifndef FOOTRIGFILEH
#define FOOTRIGFILEH

#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <map>


class CFootRigFile : public CXMLIO
{
public:
	CFootRigFile();
	virtual ~CFootRigFile();

	int WriteFootRigFile(const WCHAR* srcfilepath, FOOTRIGELEM srcfootrigelem);
	int LoadFootRigFile(const WCHAR* srcfilepath, CModel* srcmodel, ChaScene* srcchascene, FOOTRIGELEM* dstfootrigelem);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	int WriteFootRigElem(FOOTRIGELEM srcfootrigelem);

	//int CheckFileVersion( XMLIOBUF* xmliobuf );
	//int ReadFileInfo( XMLIOBUF* xmliobuf, int* charanumptr );
	int ReadFootRigElem(CModel* srcmodel, ChaScene* srcchascene, FOOTRIGELEM* dstfootrigelem, XMLIOBUF* xmlbuf);

};

#endif
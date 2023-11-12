#ifndef CHOOSECOLORFILEH
#define CHOOSECOLORFILEH

#include <ChaVecCalc.h>
#include <coef.h>
#include <XMLIO.h>

#include <vector>
#include <string>
#include <map>


class CChooseColorFile : public CXMLIO
{
public:
	CChooseColorFile();
	virtual ~CChooseColorFile();

	int WriteChooseColorFile(const WCHAR* srcfilepath, const COLORREF srccolref[16]);
	int LoadChooseColorFile(const WCHAR* srcfilepath, COLORREF* dstcolref);

private:
	virtual int InitParams();
	virtual int DestroyObjs();

	int WriteFileInfo();
	int WriteChooseColor(int colorindex, COLORREF srccolor);

	//int CheckFileVersion( XMLIOBUF* xmliobuf );
	//int ReadFileInfo( XMLIOBUF* xmliobuf, int* charanumptr );
	int ReadChooseColor(int colorcnt, XMLIOBUF* xmlbuf, COLORREF* dstcolref);

};

#endif
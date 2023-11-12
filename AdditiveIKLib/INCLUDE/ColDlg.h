#ifndef COLDLGH_
#define COLDLGH_

#include <coef.h>


class CColDlg
{
public:
	CColDlg();
	~CColDlg();

	int Choose( HWND srchwnd, COLORREF* dstcol );

	int SetCustomColor( COLORREF* srccol );
	int GetCustomColor(int colornum, COLORREF* dstcolref);

//private:
	int InitParams();

//public:
private:
	CHOOSECOLOR m_cc;
	COLORREF m_custom[16];



};

#endif
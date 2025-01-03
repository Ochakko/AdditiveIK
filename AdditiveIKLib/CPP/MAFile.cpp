#include "stdafx.h"

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <MAFile.h>
#include <MCHandler.h>
#include <Model.h>
#include <EventKey.h>

#define DBGH
#include <dbg.h>


#include <forbidid.h>
#include <MotChangeDlg.h>

#include <GlobalVar.h>

static char s_maheader[256] = "Motion Accelerator File ver1.0 type01\r\n";
static char s_maheader2[256] = "Motion Accelerator File ver2.0 type01\r\n";
static char s_maheader3[256] = "Motion Accelerator File ver3.0 type01\r\n";
static char s_maheader4[256] = "Motion Accelerator File ver4.0 type01\r\n";
static char s_maheader5[256] = "Motion Accelerator File ver5.0 type01\r\n";
static char s_maheader6[256] = "Motion Accelerator File ver6.0 type01\r\n";


CMAFile::CMAFile()
{
	InitParams();
}
CMAFile::~CMAFile()
{
	DestroyObjs();
}


int CMAFile::InitParams()
{
	m_state = MA_FINDCHUNK;
	ZeroMemory( &mabuf, sizeof( MABUF ) );
	ZeroMemory( m_linechar, sizeof( char ) * MALINELENG );

	m_model = nullptr;

	m_apphwnd = nullptr;

	m_loadtrunknum = 0;
	m_moaversion = 0;

	m_quamult = 1.0f;
	m_fuleng = g_defaultFillUpMOA;

	return 0;
}
int CMAFile::DestroyObjs()
{
	if( mabuf.hfile != INVALID_HANDLE_VALUE ){
		if( mabuf.buf == 0 ){
			//書き込み時だけ
			FlushFileBuffers( mabuf.hfile );
			SetEndOfFile( mabuf.hfile );
		}
		CloseHandle( mabuf.hfile );
		mabuf.hfile = INVALID_HANDLE_VALUE;
	}

	if( mabuf.buf ){
		free( mabuf.buf );
		mabuf.buf = 0;
	}

	mabuf.bufleng = 0;
	mabuf.pos = 0;
	mabuf.isend = 0;

	return 0;
}

int CMAFile::WriteMAHeader()
{
	int ret;
	strcpy_s( m_linechar, MALINELENG, s_maheader6 );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CMAFile::WriteFULeng()
{
	int ret;
	CMCHandler* mch = GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 1;
	}


	sprintf_s(m_linechar, MALINELENG, "#FULENG {\r\n\t%d\r\n}\r\n", mch->GetFillUpLeng());
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CMAFile::WriteEventKey()
{
	int ret;

	CEventKey* ekptr = GetEventKey();
	if (ekptr) {
		strcpy_s( m_linechar, MALINELENG, "#EVENTKEY {" );
		ret = WriteLinechar( 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		int kindex;
		for (kindex = 0; kindex < ekptr->GetKeyNum(); kindex++) {
			sprintf_s(m_linechar, MALINELENG, "%d, %d, %d, %d",
				ekptr->GetEventNo(kindex), ekptr->GetKey(kindex), 
				ekptr->GetComboNo(kindex), ekptr->GetSingleEvent(kindex));
			ret = WriteLinechar(1);
			if (ret) {
				_ASSERT(0);
				return 1;
			}
		}

		strcpy_s( m_linechar, MALINELENG, "}\r\n" );
		ret = WriteLinechar( 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}



int CMAFile::WriteLinechar( int addreturn )
{
	unsigned long wleng, writeleng;

	if( addreturn )
		strcat_s( m_linechar, MALINELENG, "\r\n" );

	wleng = (int)strlen( m_linechar );
	if( wleng >= MALINELENG ){
		DbgOut(  L"mafile : WriteLinechar : wleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	WriteFile( mabuf.hfile, m_linechar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMAFile::SaveMAFile( WCHAR* srcfilename, CModel* srcmodel, HWND srchwnd, int overwriteflag )

{

	int ret;
	if (!srcfilename || !srcmodel) {
		_ASSERT(0);
		return 1;
	}
	m_model = srcmodel;

	CMCHandler* mch = GetMotChangeHandler();
	if (!mch) {
		//未設定モデル(モーション無し)の場合には０リターン　エラーにはしない
		return 0;
	}

	ret = CheckSameMotionName();
	if( ret ){
		DbgOut(  L"mafile : SaveMAFile : CheckSameMotionName : same name found error !!!\n" );
		_ASSERT( 0 );
		return -1;//!!!!!!!!!!!
	}

	ret = CheckIdlingMotion(mch);
	if( ret ){
		DbgOut(  L"mafile : SaveMAFile : CheckIdlingMotion : idling setting error !!!\n" );
		_ASSERT( 0 );
		return -2;//!!!!!!!!!!!!
	}

	ret = CheckCommonid( srchwnd );
	if( ret ){
		DbgOut(  L"mafile : SaveMAFile : CheckCommonid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( overwriteflag == 0 ){
		DWORD fattr;
		fattr = GetFileAttributes( srcfilename );
		if( (fattr & FILE_ATTRIBUTE_DIRECTORY) == 0 ){//ファイルが見つかった場合
			WCHAR messtr[1024];
			swprintf_s( messtr, 1024, L"%sは、既に存在します。\n上書きしますか？", srcfilename );

			int dlgret;
			dlgret = (int)MessageBox( srchwnd, messtr, L"上書き確認", MB_OKCANCEL );
			if( dlgret != IDOK ){
				return 0;//!!!!!!!!!!!!!!!!!!
			}
		}
	}

	mabuf.hfile = CreateFile(srcfilename, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if( mabuf.hfile == INVALID_HANDLE_VALUE ){
		DbgOut(  L"mafile : SaveMAFile : CreateFile error !!! %s\n", srcfilename );
		_ASSERT( 0 );
		return 1;
	}	
	
	ret = WriteMAHeader();
	if( ret ){
		DbgOut(  L"mafile : SaveMAFile : WriteMAHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteFULeng();
	if( ret ){
		DbgOut(  L"mafile : SaveMAFile : WriteFULeng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteEventKey();
	if( ret ){
		DbgOut(  L"mafile : SaveMAFile : WriteEventKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = WriteTrunk();
	if( ret ){
		DbgOut(  L"mafile : SaveMAFile : WriteTrunk error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteBranch();
	if( ret ){
		DbgOut(  L"mafile : SaveMAFile : WriteBranch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMAFile::CheckSameMotionName()
{
	if (!m_model) {
		_ASSERT(0);
		return 0;
	}
	CMCHandler* mch = GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 0;
	}

	if( m_model->GetMotInfoSize() <= 1 ){
		return 0;//!!!!!!!!!!!!!!!!!!
	}

	
	int findsame = 0;
	int mcno;
	for( mcno = 0; mcno < mch->GetMCNum(); mcno++ ){
		MCELEM* curmce;
		curmce = mch->GetMCElem(mcno);
		if (curmce) {
			MOTINFO currentmi = m_model->GetMotInfo(curmce->id);
			if (currentmi.motid > 0) {
				char* curname = currentmi.motname;

				int chkno;
				for (chkno = 0; chkno < mch->GetMCNum(); chkno++) {
					if (chkno != mcno) {
						MCELEM* chkmce;
						chkmce = mch->GetMCElem(chkno);
						if (chkmce) {
							MOTINFO chkmi = m_model->GetMotInfo(chkmce->id);
							if (chkmi.motid > 0) {
								char* chkname = chkmi.motname;
								int cmp0 = strcmp(curname, chkname);
								if (cmp0 == 0) {
									findsame = 1;//!!!!!!!!!!!!!!
									break;
								}
							}
							else {
								_ASSERT(0);
								return 1;
							}
						}
						else {
							_ASSERT(0);
							return 1;
						}
					}
				}
				if (findsame) {
					break;
				}
			}
			else {
				_ASSERT(0);
				return 1;
			}
		}
		else {
			_ASSERT(0);
			return 1;
		}
	}


	return findsame;

}

int CMAFile::CheckIdlingMotion( CMCHandler* mch )
{
	int idlingnum = 0;

	int mcno;
	for( mcno = 0; mcno < mch->GetMCNum(); mcno++ ){
		MCELEM* curmce;
		curmce = mch->GetMCElem(mcno);
		if (curmce) {
			if (curmce->idling == 1) {
				DbgOut(L"mafile : CheckIdlingMotion : find idling %d %d\r\n", mcno, curmce->idling);
				idlingnum++;
			}
		}
		else {
			_ASSERT(0);
			return 1;
		}
	}

	if( idlingnum == 1 ){
		return 0;
	}else{
		DbgOut(  L"mafile : CheckIdlingMotin : idlingnum error !!! %d\r\n", idlingnum );
		return 1;
	}
}


int CMAFile::CheckCommonid( HWND srchwnd )
{
	if (!m_model) {
		_ASSERT(0);
		return 0;
	}
	CMCHandler* mch = GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 0;
	}
	int motnum = m_model->GetMotInfoSize();
	if (motnum <= 0) {
		_ASSERT(0);
		return 0;
	}
	
	int mcno;
	for( mcno = 0; mcno < mch->GetMCNum(); mcno++ ){
		MCELEM* parmce;
		parmce = mch->GetMCElem(mcno);
		if (parmce && parmce->childmc && (parmce->childnum > 0)) {
			int parentmotid = parmce->id;
		
			int childno;
			for (childno = 0; childno < parmce->childnum; childno++) {
				MCELEM* childmce;
				childmce = parmce->childmc + childno;
				int childmotid = childmce->id;

				if ((parentmotid > 0) && (childmotid > 0) && (parentmotid != childmotid)) {
					if ((HitTestForbidID(parmce, childmce->eventno1) == 0) && (childmce->eventno1 == parmce->commonid)) {
						MOTINFO parentmi = m_model->GetMotInfo(parmce->id);
						MOTINFO childmi = m_model->GetMotInfo(childmce->id);
						char tmpline[2048];
						sprintf_s(tmpline, 2048, "分岐モーションの中に、共通分岐IDと同じIDが見つかりました。\n親項目 %s\n子項目 %s\nイベントID %d",
							parentmi.motname, childmi.motname, parmce->commonid);
						::MessageBoxA(srchwnd, tmpline, "警告", MB_OK);
						_ASSERT(0);
					}
				}
			}

		}
		else {
			//分岐が１つもないケース　エラーではない
		}
	}


	return 0;
}

int CMAFile::WriteTrunk()
{

//#TRUNK {
//"MotionName1", "filename1.qua", "Idling0", ev0idle, commonid, forbidcommonid, notfu 
//"MotionName2", "filename2.qua", "Normal0", ev0idle, commonid, forbidcommonid, notfu
//}
	int ret;

	if (!m_model) {
		_ASSERT(0);
		return 0;
	}
	CMCHandler* mch = GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 0;
	}
	int motnum = m_model->GetMotInfoSize();
	if (motnum <= 0) {
		_ASSERT(0);
		return 0;
	}

	strcpy_s( m_linechar, MALINELENG, "#TRUNK {" );
	ret = WriteLinechar( 1 );
	_ASSERT( !ret );


	int mcno;
	for( mcno = 0; mcno < mch->GetMCNum(); mcno++ ){
		MCELEM* curmce;
		curmce = mch->GetMCElem(mcno);
		if (curmce) {
			MOTINFO currentmi = m_model->GetMotInfo(curmce->id);
			if (currentmi.motid > 0) {
				char* mname;
				mname = currentmi.motname;

				sprintf_s(m_linechar, MALINELENG, "\t\"%s\", \"%s.qua\", ", mname, mname);

				if (curmce->idling == 1) {
					strcat_s(m_linechar, MALINELENG, "\"Idling0\", ");
				}
				else {
					strcat_s(m_linechar, MALINELENG, "\"Normal0\", ");
				}

				char tmpline[1024];
				sprintf_s(tmpline, 1024, "%d, %d, %d", curmce->ev0idle, curmce->commonid, curmce->forbidnum);
				strcat_s(m_linechar, MALINELENG, tmpline);

				int fno;
				for (fno = 0; fno < curmce->forbidnum; fno++) {
					sprintf_s(tmpline, 1024, ", %d", *(curmce->forbidid + fno));
					strcat_s(m_linechar, MALINELENG, tmpline);
				}

				sprintf_s(tmpline, 1024, ", %d", curmce->notfu);
				strcat_s(m_linechar, MALINELENG, tmpline);


				ret = WriteLinechar(1);
				_ASSERT(!ret);
			}
			else {
				_ASSERT(0);
				return 1;
			}

		}
		else {
			_ASSERT(0);
			return 1;
		}
	}

	strcpy_s( m_linechar, MALINELENG, "}\r\n" );
	ret = WriteLinechar( 1 );
	_ASSERT( !ret );


	return 0;
}

int CMAFile::WriteBranch()
{

//#BRANCH "MotionName1(parentname)"{
//    "MotionName2(childname)", eventno, frameno1, frameno2, notfu, nottoidle
//    "MotionName3(childname)", eventno, frameno1, frameno2, notfu, nottoidle
//    "MotionName4(childname)", eventno, frameno1, frameno2, notfu, nottoidle
//}

	int ret;

	if (!m_model) {
		_ASSERT(0);
		return 0;
	}
	CMCHandler* mch = GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 0;
	}
	int motnum = m_model->GetMotInfoSize();
	if (motnum <= 0) {
		_ASSERT(0);
		return 0;
	}


	int mcno;
	for (mcno = 0; mcno < mch->GetMCNum(); mcno++) {
		MCELEM* parmce;
		parmce = mch->GetMCElem(mcno);
		if (parmce) {
			MOTINFO currentmi = m_model->GetMotInfo(parmce->id);
			if (currentmi.motid > 0) {
				char* parname;
				parname = currentmi.motname;
				if (parmce->childmc && (parmce->childnum >= 1)) {

					sprintf_s(m_linechar, MALINELENG, "#BRANCH \"%s\" {", parname);
					ret = WriteLinechar(1);
					_ASSERT(!ret);


					int chilno;
					for (chilno = 0; chilno < parmce->childnum; chilno++) {
						MCELEM* childmce;
						childmce = parmce->childmc + chilno;
						MOTINFO childmi = m_model->GetMotInfo(childmce->id);
						if (childmi.motid > 0) {
							char* childname;
							childname = childmi.motname;

							sprintf_s(m_linechar, "\t\"%s\", %d, %d, %d, %d, %d", 
								childname, 
								childmce->eventno1, childmce->frameno1, childmce->frameno2, 
								childmce->notfu, childmce->nottoidle);
							ret = WriteLinechar(1);
							_ASSERT(!ret);

						}
						else {
							_ASSERT(0);
							return 1;
						}
					}

					strcpy_s(m_linechar, MALINELENG, "}\r\n");
					ret = WriteLinechar(1);
					_ASSERT(!ret);

				}
			}
			else {
				_ASSERT(0);
				return 1;
			}
		}
		else {
			_ASSERT(0);
			return 1;
		}
	}

	return 0;
}

int CMAFile::SetBuffer( WCHAR* srcfilename )
{
	DestroyObjs();

	mabuf.hfile = CreateFile(srcfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if( mabuf.hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	

	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( mabuf.hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut(  L"mafile : SetBuffer :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut(  L"mafile : SetBuffer :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );//bufleng + 1
	if( !newbuf ){
		DbgOut(  L"mafile : SetBuffer :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	BOOL result1 = ReadFile( mabuf.hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( (result1 == FALSE) || (rleng != readleng) ){
		DbgOut(  L"mafile : SetBuffer :  ReadFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	mabuf.buf = newbuf;
	*(mabuf.buf + bufleng) = 0;//!!!!!!!!
	mabuf.bufleng = bufleng;
	mabuf.pos = 0;	
	

//DbgOut(  L"check !!! mafile : SetBuffer : filename %s, bufleng %d\r\n", srcfilename, bufleng );

	/////////////

	//char* lasten;
	//lasten = strrchr( srcfilename, '\\' );
	//if( lasten ){
	//	int cpleng;
	//	cpleng = (int)( lasten - srcfilename );
	//	if( cpleng >= MAX_PATH ){
	//		DbgOut(  L"mafile : SetBuffer : mediadir path too long error !!!\n" );
	//		_ASSERT( 0 );
	//		return 1;
	//	}
	//	strncpy_s( m_mediadir, MAX_PATH, srcfilename, cpleng );
	//	m_mediadir[cpleng] = 0;
	//
	//}else{
	//	m_mediadir[0] = 0;
	//}


	return 0;
}

int CMAFile::GetLine( int* getlen )
{
	//int ret;

	int notfound = 1;
	int stepno = 0;
	while( (mabuf.pos + stepno < mabuf.bufleng) && (notfound = ( *(mabuf.buf + mabuf.pos + stepno) != '\n' )) ){
		stepno++;

		if( (mabuf.pos + stepno) >= mabuf.bufleng ){
			//mabuf.isend = 1;//!!!!!!!!!!
			//_ASSERT( 0 );
			break;
		}
	}

	if( notfound == 0 ){
		stepno++; //\nの分
	}

	if( MALINELENG > stepno ){
		if( (stepno > 0) && ((mabuf.pos + stepno) <= mabuf.bufleng) )
			strncpy_s( m_linechar, MALINELENG, (const char*)(mabuf.buf + mabuf.pos), stepno );
		*getlen = stepno;
		mabuf.pos += stepno;

		m_linechar[stepno] = 0;

	}else{
		_ASSERT( 0 );
		DbgOut(  L"MAFile : GetLine : dstlen short error !!!" );
		return 1;
	}

//	if( mabuf.pos + stepno >= mabuf.bufleng ){
	if( mabuf.pos >= mabuf.bufleng ){

//DbgOut(  L"check !!! mafile : GetLine : pos + stepno >= bufleng, %d + %d >= %d, %s\r\n", mabuf.pos, stepno, mabuf.bufleng, m_linechar );
		mabuf.isend = 1;
		m_state = MA_FINISH;
		return 0;
	}else{

//DbgOut(  L"check !!! mafile : GetLine : return notfound\r\n" );
//			return notfound;


		if( ((*getlen == 2) && (strcmp( m_linechar, "\r\n" ) == 0)) || ((*getlen == 1) && (*m_linechar == '\n')) ){
//DbgOut(  L"check !!! mafile : GetLine : only return : call GetLine\r\n" );
			int retgl;			
			retgl = GetLine( getlen );	
			return retgl;
		}else{

//DbgOut(  L"check !!! mafile : GetLine : return notfound\r\n" );

			return notfound;
		}

	}

/***
	//if( (notfound == 1) && (mabuf.isend == 1) ){
	if( mabuf.isend == 1 ){
		m_state = MA_FINISH;
		return 0;
	}else{

		if( ((*getlen == 2) && (strcmp( m_linechar, "\r\n" ) == 0)) || ((*getlen == 1) && (*m_linechar == '\n')) ){
			int retgl;			
			retgl = GetLine( getlen );	
			return retgl;
		}else{
			return notfound;
		}
	}
***/

}


int CMAFile::CheckFileVersion()
{
	int ret;
	int leng1;

	ret = GetLine( &leng1 );
	DbgOut(  L"MAFile : CheckFileVersion : filekind : %s\n", m_linechar );
	if( ret ){
		//if( m_apphwnd )
		//	::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 0, 0 );
		::MessageBoxA( m_apphwnd, "moaファイルではありません。\n読み込めません。", "読み込みエラー", MB_OK );
		//if( m_apphwnd )
		//	::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 1, 0 );

		return 0;
	}

	int cmp1, cmp2, cmp3, cmp4, cmp5, cmp6;
	cmp1 = strcmp( s_maheader, m_linechar );
	cmp2 = strcmp( s_maheader2, m_linechar );
	cmp3 = strcmp( s_maheader3, m_linechar );
	cmp4 = strcmp( s_maheader4, m_linechar );
	cmp5 = strcmp( s_maheader5, m_linechar );
	cmp6 = strcmp( s_maheader6, m_linechar );
	if( cmp1 && cmp2 && cmp3 && cmp4 && cmp5 && cmp6 ){
		//if( m_apphwnd )
		//	::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 0, 0 );
		::MessageBoxA( m_apphwnd, "moaファイルではありません。\n読み込めません。", "読み込みエラー", MB_OK );
		//if( m_apphwnd )
		//	::SendMessage( m_apphwnd, WM_USER_ENABLE_MENU, 1, 0 );
		return 0;
	}

	if( cmp1 == 0 ){
		return 10;
	}else if( cmp2 == 0 ){
		return 20;
	}else if( cmp3 == 0 ){
		return 30;
	}else if( cmp4 == 0 ){
		return 40;
	}else if( cmp5 == 0 ){
		return 50;
	}else if( cmp6 == 0 ){
		return 60;
	}else{
		_ASSERT( 0 );
		return 0;
	}

}

int CMAFile::FindChunk()
{
	int isfind = 0;
	int ret = 0;
	int getleng;
	while( (isfind == 0) && (mabuf.isend == 0) ){
		ret = GetLine( &getleng );

//DbgOut(  L"check !!! mafile : FindChunk %d, %s\r\n", ret, m_linechar );

		if( ret ){
			//_ASSERT( 0 );
			m_state = MA_FINISH;
			if( mabuf.isend == 1 ){
				ret = 0;//!!!!
			}
			break;
		}
		if( (getleng >= 3) && (strstr( m_linechar, "{\r\n" ) != NULL) ){
			isfind = 1;
			ret = GetChunkType( m_linechar, getleng );

//DbgOut(  L"check !!! mafile : FindChunk : aft GetChunkType %d, m_state %d\r\n", ret, m_state );

			if( ret ){
				_ASSERT( 0 );
				m_state = MA_FINISH;
				break;
			}
		}else if( (getleng >= 5) && (strstr( m_linechar, "Eof\r\n" ) != NULL) ){

//DbgOut(  L"check !!! mafile : FindChunk :find Eof\r\n" );

			m_state = MA_FINISH;
			break;
		}

		if( mabuf.isend == 1 ){

//DbgOut(  L"check !!! mafile : FindChunk : mabuf.isend == 1\r\n" );

			m_state = MA_FINISH;
			break;
		}

	}

	return ret;
}

int CMAFile::GetChunkType( char* chunkname, int nameleng )
{
	char* namehead = chunkname;

	while( (*namehead == ' ') || (*namehead == '\t') ){
		namehead++;
	}

	char chunkpat[4][20] =
	{
		"#TRUNK",
		"#BRANCH",
		"#FULENG",
		"#EVENTKEY"
	};

	int isfind = 0;
	int patno;
	for( patno = 0; patno < 4; patno++ ){
		if( isfind == 1 )
			break;

		int patleng;
		patleng = (int)strlen( chunkpat[patno] );
		int cmp = 1;
		if( nameleng >= patleng ){
			cmp = strncmp( chunkpat[patno], namehead, patleng );
			if( cmp == 0 ){
				isfind = 1;
				switch( patno ){
				case 0:
					m_state = MA_TRUNK;
					break;
				case 1:
					m_state = MA_BRANCH;
					break;
				case 2:
					m_state = MA_FULENG;
					break;
				case 3:
					m_state = MA_EVENTKEY;
					break;
				default:
					break;
				}
			}
		}

	}

	if( isfind == 0 ){
		_ASSERT( 0 );
		m_state = MA_UNKCHUNK;
	}

	return 0;
}

int CMAFile::SkipChunk()
{
	int depth = 1;
	int findend = 0;
	int ret;
	//char linechar[512];

	while( (findend == 0) || (depth != 0) ){
		
		findend = 0;

		int getleng;
		ret = GetLine( &getleng );
		if( ret ){
			_ASSERT( 0 );
			return ret;
		}

		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
			depth--;
		}else if( (getleng >= 3) && (strstr( m_linechar, "{\r\n" ) != NULL) ){
			depth++;
		}
	}

	return 0;
}

int CMAFile::LoadMAFile( WCHAR* srcfilename, CModel* srcmodel )
{
	if (!srcfilename || !srcmodel) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	m_model = srcmodel;
	m_loadtrunknum = 0;

	//m_apphwnd = srcpapp->m_hWnd;


	BOOL bexist;
	bexist = PathFileExists(srcfilename);//ファイルが存在する場合にTRUE
	if (!bexist) {
		//ファイルが存在しない場合は０リターン　エラーにはしない
		return 0;
	}

	ret = LoadMAFile_aft( srcfilename );
	if( ret ){
		DbgOut(  L"mafile : LoadMAFile : LoadMAFile_aft error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CMAFile::LoadMAFile_aft( WCHAR* srcfilename )
{
	int ret;

	if (!m_model) {
		_ASSERT(0);
		return 1;
	}

	ret = m_model->CreateMotChangeHandlerIfNot();
	if (ret) {
		DbgOut(L"mafile : LoadMAFile : CreateMotChangeHandlerIfNot error !!!\n");
		_ASSERT(0);
		return 1;
	}
	ret = m_model->ResetMotChangeHandler();
	if (ret) {
		DbgOut(L"mafile : LoadMAFile : ResetMotChangeHandler error !!!\n");
		_ASSERT(0);
		return 1;
	}


	ret = SetBuffer( srcfilename );
	if( ret ){
		DbgOut(  L"mafile : LoadMAFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	///////

	m_moaversion = CheckFileVersion();
	if( m_moaversion == 0 ){
		DbgOut(  L"mafile : Loadmafile : file version error!!!\n" );
		return 1;
	}

	m_state = MA_FINDCHUNK;


	//char chunkname[256];

	while( (m_state != MA_FINISH) && (mabuf.isend == 0) ){

		switch( m_state ){
		case MA_FINDCHUNK:
			ret = FindChunk();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;
		case MA_UNKCHUNK:
			ret = SkipChunk();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			m_state = MA_FINDCHUNK;
			break;
		case MA_FULENG:
			ret = ReadFULeng();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;
		case MA_EVENTKEY:
			ret = ReadEventKey();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;

		case MA_TRUNK:
			ret = ReadTrunk();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;
		case MA_BRANCH:
			ret = ReadBranch();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			break;

		case MA_FINISH:
		case MA_NONE:
		default:
			m_state = MA_FINISH;
			break;
		}
	}


	return 0;
}
int CMAFile::ReadFULeng()
{
	if (!m_model) {
		_ASSERT(0);
		return 0;
	}
	CMCHandler* mch = GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 0;
	}

	int ret;
	int findend = 0;
	int getleng;
	int pos, stepnum;
	while( findend == 0 ){
		ret = GetLine( &getleng );
		if( ret )
			return ret;
		
		pos = 0;
		stepnum = 0;

		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
		}else{
			m_fuleng = g_defaultFillUpMOA;
			ret = GetInt( &m_fuleng, m_linechar, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut(  L"mafile : ReadFULeng : GetInt fuleng error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( m_fuleng > 0 ){
				mch->SetFillUpLeng(m_fuleng);
			}else{
				mch->SetFillUpLeng(10);
			}
		}
	}
	m_state = MA_FINDCHUNK;

	return 0;
}

int CMAFile::ReadEventKey()
{
	if (!m_model) {
		_ASSERT(0);
		return 0;
	}
	CMCHandler* mch = GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 0;
	}

	CEventKey* eventkey = GetEventKey();
	if (!eventkey) {
		_ASSERT(0);
		return 0;
	}

	int ret;
	int findend = 0;
	int getleng;
	int pos, stepnum;

	ret = eventkey->DelEKeyByIndex( -1 );
	if( ret ){
		DbgOut(  L"mafile : ReadEventKey : ek DelEKeyByIndex all error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	while( findend == 0 ){
		ret = GetLine( &getleng );
		if( ret )
			return ret;
		
		pos = 0;
		stepnum = 0;

		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
		}else{
			EKEY ek;
			ZeroMemory( &ek, sizeof( EKEY ) );

			ret = GetInt( &ek.eventno, m_linechar, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut(  L"mafile : ReadEventKey : GetInt eventno error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

			ret = GetInt( &ek.key, m_linechar, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut(  L"mafile : ReadEventKey : GetInt key error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

			ret = GetInt( &ek.combono, m_linechar, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut(  L"mafile : ReadEventKey : GetInt combono error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

			if( m_moaversion >= 60 ){
				int tmpsingle = 1;
				ret = GetInt( &tmpsingle, m_linechar, pos, MALINELENG, &stepnum );
				if( ret ){
					DbgOut(  L"mafile : ReadEventKey : GetInt singleevent error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				pos += stepnum;

				if( tmpsingle == 0 ){
					ek.singleevent = 0;
				}else{	
					ek.singleevent = 1;
				}
			}else{
				ek.singleevent = 1;
			}

			ret = eventkey->AddEKey( ek );
			if( ret ){
				DbgOut(  L"mafile : ReadEventKey : ek AddEKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	m_state = MA_FINDCHUNK;

	return 0;
}


int CMAFile::ReadTrunk()
{
	int ret;

	if (!m_model) {
		_ASSERT(0);
		return 0;
	}
	CMCHandler* mch = GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 0;
	}


	int findend = 0;
	int getleng;
	int vertno = 0;
	while( findend == 0 ){
		ret = GetLine( &getleng );
		if( ret )
			return ret;
		
		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
		}else{
			MATRUNK matrunk;
			ZeroMemory( &matrunk, sizeof( MATRUNK ) );

			ret = SetMATrunk( m_linechar, &matrunk );
			if( ret ){
				DbgOut(  L"mafile : ReadTrunk : SetMATrunk error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			//char fullname[MAX_PATH];
			//int chkleng;
			//chkleng = (int)strlen( m_mediadir ) + (int)strlen( matrunk.filename ) + 1;
			//if( chkleng >= MAX_PATH ){
			//	DbgOut(  L"mafile : ReadTrunk : fullname path too long error %s!!!\n", matrunk.filename );
			//	_ASSERT( 0 );
			//	return 1;
			//}
			//sprintf_s( fullname, MAX_PATH, "%s\\%s", m_mediadir, matrunk.filename );

			//DbgOut(  L"check !!! mafile : ReadTrunk : fullname %s\r\n", fullname );

			MOTINFO addmi = m_model->GetMotInfoByNameMOA(matrunk.motname);
			if (addmi.motid > 0) {
				mch->AddParentMC(addmi.motid,
					matrunk.idling, matrunk.ev0idle, matrunk.commonid, matrunk.forbidnum, matrunk.forbidid, matrunk.notfu);

				m_loadtrunknum++;//!!!!!!!!!!!!!

				if (matrunk.forbidid) {
					free(matrunk.forbidid);
					matrunk.forbidid = 0;
				}
				matrunk.forbidnum = 0;


				m_model->SetMotInfoLoopFlagByID(addmi.motid, 0);
			}
			else {
				_ASSERT(0);
				return 1;
			}
		}
	}
	m_state = MA_FINDCHUNK;

	return 0;
}

int CMAFile::ReadBranch()
{
	int ret;

	if (!m_model) {
		_ASSERT(0);
		return 0;
	}
	CMCHandler* mch = GetMotChangeHandler();
	if (!mch) {
		_ASSERT(0);
		return 0;
	}


	int pos = 0;
	int stepnum;

	char parentname[MAX_PATH];
	char* startpoint;

	stepnum = 0;
	startpoint = strchr( m_linechar + pos, '\"' );
	if( !startpoint ){
		_ASSERT( 0 );
		return 1;
	}
	pos = (int)(startpoint - m_linechar);
	ret = GetName( parentname, MAX_PATH, m_linechar, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut(  L"mafile : ReadBranch : GetName parentname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
//DbgOut(  L"check !!! mafile : ReadBranch : parentname %s\r\n", parentname );


	MOTINFO parentmi = m_model->GetMotInfoByNameMOA(parentname);
	if (parentmi.motid <= 0) {
		DbgOut(L"mafile : ReadBranch : mh GetMotionIDByName %s error !!!\n", parentname);
		_ASSERT(0);
		return 1;
	}
	int parentcookie = parentmi.motid;
	int parentsetno = mch->ExistMotIdInTrunk(parentmi.motid);
	if (parentsetno < 0) {
		DbgOut(L"mafile : ReadBranch : mh ExistMotIdInTrunk %s Not Exist !!!\n", parentname);
		_ASSERT(0);
		return 1;
	}

	//DbgOut(  L"check !!! : mafile : ReadBranch : parentname %s\r\n", parentname );

	int findend = 0;
	int getleng;
	int vertno = 0;
	while( findend == 0 ){
		ret = GetLine( &getleng );

//DbgOut(  L"check !!! mafile : ReadBranch : aft GetLine %d, %s\r\n", ret, m_linechar );

		if( ret )
			return ret;
			
		if( (getleng >= 3) && (strstr( m_linechar, "}\r\n" ) != NULL) ){
			findend = 1;
		}else{
			MABRANCH mabranch;
			ret = SetMABranch( m_linechar, &mabranch );

//DbgOut(  L"check !!! mafile : ReadBranch : aft SetBranch %d, %s\r\n", ret, m_linechar );

			if( ret ){
				DbgOut(  L"mafile : ReadBranch : SetMABranch error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			MOTINFO childmi = m_model->GetMotInfoByNameMOA(mabranch.motname);
			if (childmi.motid <= 0) {
				DbgOut(L"mafile : ReadBranch : mh GetMotionIDByName child %s error !!!\n", mabranch.motname);
				_ASSERT(0);
				return 1;
			}
			int childcookie = childmi.motid;

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			int srcalways = 0;//!!!!!!!!!!!!!!!!!!!!
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			ret = mch->AddChild(parentsetno, childcookie,
				srcalways, mabranch.frameno1, mabranch.frameno2,
				mabranch.eventno, mabranch.notfu, mabranch.nottoidle);
			if( ret ){
				DbgOut(  L"mafile : ReadBranch : mcdlg AddChildMC error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			m_model->SetMotInfoLoopFlagByID(childcookie, 0);

	//DbgOut(  L"check !!! : mafile : ReadBranch : %s, %d, %d, %d\r\n", mabranch.motname, mabranch.eventno, mabranch.frameno1, mabranch.frameno2 );

		}
	}

	m_state = MA_FINDCHUNK;

	return 0;
}





int CMAFile::SetMATrunk( char* srcline, MATRUNK* srctrunk )
{
	int ret;
	int pos = 0;
	int stepnum;

	stepnum = 0;
	char* startpoint;
	startpoint = strchr( srcline, '\"' );
	if( !startpoint ){
		_ASSERT( 0 );
		return 1;
	}
	pos = (int)(startpoint - srcline);
	ret = GetName( srctrunk->motname, MAX_PATH, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut(  L"mafile : SetMATrunk : GetName motname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;



	stepnum = 0;
	startpoint = strchr( srcline + pos, ',' );
	if( !startpoint ){
		_ASSERT( 0 );
		return 1;
	}
	pos = (int)(startpoint - srcline + 1);
	ret = GetName( srctrunk->filename, MAX_PATH, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut(  L"mafile : SetMATrunk : GetName filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;


	char tempchar[MAX_PATH];
	stepnum = 0;
	startpoint = strchr( srcline + pos, ',' );
	if( !startpoint ){
		_ASSERT( 0 );
		return 1;
	}
	pos = (int)(startpoint - srcline + 1);
	ret = GetName( tempchar, MAX_PATH, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut(  L"mafile : SetMATrunk : GetName tempchar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;

	int cmp0;
	cmp0 = strcmp( tempchar, "Idling0" );
	if( cmp0 == 0 ){
		srctrunk->idling = 1;
	}else{
		srctrunk->idling = 0;
	}

	if( m_moaversion >= 20 ){

		stepnum = 0;
		ret = GetInt( &srctrunk->ev0idle, srcline, pos, MALINELENG, &stepnum );
		if( ret ){
			DbgOut(  L"mafile : SetMATrunk : GetInt ev0idle error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		pos += stepnum;
		

		stepnum = 0;
		ret = GetInt( &srctrunk->commonid, srcline, pos, MALINELENG, &stepnum );
		if( ret ){
			DbgOut(  L"mafile : SetMATrunk : GetInt commonid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		pos += stepnum;

		if( m_moaversion < 30 ){
			int forbidtotal = 0;
			stepnum = 0;
			ret = GetInt( &forbidtotal, srcline, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut(  L"mafile : SetMATrunk : GetInt forbidcommonid error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

			srctrunk->forbidnum = 0;
			_ASSERT( !srctrunk->forbidid );
			srctrunk->forbidid = 0;

			int bno;
			int curval = 1;
			int setno = 0;
			for( bno = 0; bno < 30; bno++ ){
				int andval;
				andval = forbidtotal & curval;
				if( andval ){
					int* newforbidid = nullptr;
					newforbidid = (int*)realloc(srctrunk->forbidid, sizeof(int) * (setno + 1));
					if(!newforbidid){
						DbgOut(  L"mafile : SetMATrunk : forbidid alloc error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					else {
						srctrunk->forbidid = newforbidid;
						*(srctrunk->forbidid + setno) = andval;
						setno++;
					}
				}
				curval = curval << 1;
			}

			srctrunk->forbidnum = setno;
		}else{

			stepnum = 0;
			ret = GetInt( &srctrunk->forbidnum, srcline, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut(  L"mafile : SetMATrunk : GetInt forbidnum error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

			if( srctrunk->forbidid ){
				free( srctrunk->forbidid );
				srctrunk->forbidid = 0;
			}
			
			if( srctrunk->forbidnum > 0 ){
				srctrunk->forbidid = (int*)malloc( sizeof( int ) * srctrunk->forbidnum );
				if( !srctrunk->forbidid ){
					DbgOut(  L"mafile : SetMATrunk : forbidid alloc error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ZeroMemory( srctrunk->forbidid, sizeof( int ) * srctrunk->forbidnum );

				int fno;
				for( fno = 0; fno < srctrunk->forbidnum; fno++ ){

					stepnum = 0;
					ret = GetInt( srctrunk->forbidid + fno, srcline, pos, MALINELENG, &stepnum );
					if( ret ){
						DbgOut(  L"mafile : SetMATrunk : GetInt forbidid error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					pos += stepnum;
				}
			}
		}

		if( m_moaversion >= 40 ){
			stepnum = 0;
			ret = GetInt( &srctrunk->notfu, srcline, pos, MALINELENG, &stepnum );
			if( ret ){
				DbgOut(  L"mafile : SetMATrunk : GetInt notfu error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;

//			_ASSERT( 0 );

		}else{
			srctrunk->notfu = 0;
		}

	}else{

		srctrunk->ev0idle = 0;
		srctrunk->commonid = 0;
		srctrunk->forbidnum = 0;
		srctrunk->forbidid = 0;
		srctrunk->notfu = 0;
	}

	return 0;
}

int CMAFile::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) &&  
		( ( isdigit( *(srcchar + startpos) ) == 0 ) && (*(srcchar + startpos) != '-') ) 
	
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( (isdigit( *(srcchar + endpos) ) != 0) || ( *(srcchar + endpos) == '-' ) )
	){
		endpos++;
	}

	char tempchar[256];
	if( (endpos - startpos < 256) && (endpos - startpos > 0) ){
		strncpy_s( tempchar, 256, srcchar + startpos, endpos - startpos );
		tempchar[endpos - startpos] = 0;

		*dstint = atoi( tempchar );

		*stepnum = endpos - pos;
	}else{
		_ASSERT( 0 );
		*stepnum = endpos - pos;
		return 1;
	}


	return 0;
}
int CMAFile::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) &&  
		( ( isdigit( *(srcchar + startpos) ) == 0 ) && (*(srcchar + startpos) != '-') && (*(srcchar + startpos) != '.') ) 
	
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( (isdigit( *(srcchar + endpos) ) != 0) || ( *(srcchar + endpos) == '-' ) || (*(srcchar + endpos) == '.') )
	){
		endpos++;
	}

	char tempchar[256];
	if( (endpos - startpos < 256) && (endpos - startpos > 0) ){
		strncpy_s( tempchar, 256, srcchar + startpos, endpos - startpos );
		tempchar[endpos - startpos] = 0;

		*dstfloat = (float)atof( tempchar );

		*stepnum = endpos - pos;
	}else{
		_ASSERT( 0 );
		*stepnum = endpos - pos;
		return 1;
	}


	return 0;
}

int CMAFile::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) && 
		( ( *(srcchar + startpos) == ' ' ) || ( *(srcchar + startpos) == '\t' ) || ( *(srcchar + startpos) == '\"' ) ) 
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( ( *(srcchar + endpos) != ' ' ) && ( *(srcchar + endpos) != '\t' ) && (*(srcchar + endpos) != '\r') && (*(srcchar + endpos) != '\n') && (*(srcchar + endpos) != '\"') )
	){
		endpos++;
	}

	if( (endpos - startpos < dstleng) && (endpos - startpos > 0) ){
		strncpy_s( dstchar, dstleng, srcchar + startpos, endpos - startpos );
		*(dstchar + endpos - startpos) = 0;

		*stepnum = endpos - pos;

	}else{
		_ASSERT( 0 );
		*stepnum = endpos - pos;
		return 1;
	}

	return 0;
}


int CMAFile::SetMABranch( char* srcline, MABRANCH* srcbranch )
{

	int ret;
	int pos = 0;
	int stepnum;

	char* startpoint;

	stepnum = 0;
	startpoint = strchr( srcline + pos, '\"' );
	if( !startpoint ){
		_ASSERT( 0 );
		return 1;
	}
	pos = (int)(startpoint - srcline);
	ret = GetName( srcbranch->motname, MAX_PATH, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut(  L"mafile : SetMABranch : GetName motname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;


	stepnum = 0;
	ret = GetInt( &srcbranch->eventno, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut(  L"mafile : SetMABranch : GetInt eventno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;


	stepnum = 0;
	ret = GetInt( &srcbranch->frameno1, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut(  L"mafile : SetMABranch : GetInt frameno1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;


	stepnum = 0;
	ret = GetInt( &srcbranch->frameno2, srcline, pos, MALINELENG, &stepnum );
	if( ret ){
		DbgOut(  L"mafile : SetMABranch : GetInt frameno2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;


	if( m_moaversion >= 20 ){
		stepnum = 0;
		ret = GetInt( &srcbranch->notfu, srcline, pos, MALINELENG, &stepnum );
		if( ret ){
			DbgOut(  L"mafile : SetMABranch : GetInt notfu error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		pos += stepnum;
	}else{
		srcbranch->notfu = 0;
	}

	if( m_moaversion >= 60 ){
		stepnum = 0;
		ret = GetInt( &srcbranch->nottoidle, srcline, pos, MALINELENG, &stepnum );
		if( ret ){
			DbgOut(  L"mafile : SetMABranch : GetInt nottoidle error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		pos += stepnum;

	}else{
		srcbranch->nottoidle = 0;
	}

	return 0;
}


CMCHandler* CMAFile::GetMotChangeHandler()
{
	if (m_model) {
		return m_model->GetMotChangeHandler();
	}
	else {
		return nullptr;
	}
}

CEventKey* CMAFile::GetEventKey()
{
	if (m_model) {
		return m_model->GetEventKey();
	}
	else {
		return nullptr;
	}
}

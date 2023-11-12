#include "stdafx.h"
//#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <windows.h>

#include <BVHFile.h>
#include <crtdbg.h>

//#include <mothandler.h>
//#include <motionctrl.h>
//#include <motioninfo.h>
//#include <motionpoint2.h>
//#include <quaternion.h>

//#include <treehandler2.h>
//#include <treeelem2.h>
//#include <shdhandler.h>
//#include <shdelem.h>

#include <bvhelem.h>

#include <mqofile.h>
//#include <bonefile.h>

////#include <BoneProp.h>

//#include <d3dx9.h>
#include <ChaVecCalc.h>

#define DBGH
#include <dbg.h>

//#ifndef INEASY3D
//#include "c:\pgfile9\RokDeBone2DX\MotParamDlg.h"
//#endif

enum {
	STATE_ROOT,
	STATE_OFFSET,
	STATE_CHANELS,
	STATE_JOINT,
	STATE_ENDSITE,
	STATE_BRACE1,
	STATE_BRACE2,
	STATE_MOTION,
	STATE_FRAMES,
	STATE_FRAMETIME,
	STATE_MOTPARAMS,
	STATE_NONE,
	STATE_FINISH,
	STATE_MAX
};

char bvhpat[STATE_MAX + 1][20] = {
	"ROOT",
	"OFFSET",
	"CHANNELS",
	"JOINT",
	"End",
	"{",
	"}",
	"MOTION",
	"Frames:",
	"Frame Time:",
	"",
	"",
	""
};


CBVHFile::CBVHFile()
{
	InitParams();
}
CBVHFile::~CBVHFile()
{
	DestroyObjs();
}

int CBVHFile::InitParams()
{
	m_state = STATE_NONE;

	m_hfile = INVALID_HANDLE_VALUE;
	m_buf = 0;
	m_pos = 0;
	m_bufleng = 0;


	m_hwfile = INVALID_HANDLE_VALUE;
	m_writenum = 0;

	m_motcookie = -1;

	m_behead = 0;
	m_curbe = 0;
	m_parbe = 0;

	ZeroMemory( m_linechar, sizeof(char) * BVHLINELENG );
	
	m_frames = 0;
	m_frametime = 0.00833333f;

	m_benum = 0;
	m_bearray = 0;

	m_appwnd = 0;

	m_jointnum = 0;
	m_bonenum = 0;

	m_motframenum = 0;

	return 0;
}
int CBVHFile::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}

	if( m_buf ){
		free( m_buf );
		m_buf = 0;
	}

	m_pos = 0;
	m_bufleng = 0;


	if( m_hwfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hwfile );
		SetEndOfFile( m_hwfile );
		CloseHandle( m_hwfile );
		m_hwfile = INVALID_HANDLE_VALUE;
	}


////////

	if( m_bearray ){

		int beno;
		CBVHElem* delbe;
		for( beno = 0; beno < m_benum; beno++ ){
			delbe = *( m_bearray + beno );
			delete delbe;
		}
		
		free( m_bearray );
		m_bearray = 0;
		m_benum = 0;
	}

	m_behead = 0;

	return 0;
}

int CBVHFile::LoadBVHFile( HWND srcapphwnd, WCHAR* srcname, float srcmult )
{
	int ret = 0;

	DestroyObjs();
	InitParams();

	m_appwnd = srcapphwnd;

	ret = SetBuffer( srcname );
	if( ret ){
		DbgOut( L"bvhfile : LoadBVHFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto ldbvhexit;
	}

	ret = CheckFileHeader();
	if( ret ){
		DbgOut( L"bvhfile : LoadBVHFile : CheckFileHeader error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto ldbvhexit;
	}

	m_state = STATE_NONE;
	while( m_pos < m_bufleng ){

		m_state = GetStateAndLine();

		switch( m_state ){
		case STATE_ROOT:
		case STATE_JOINT:
		case STATE_ENDSITE:
//DbgOut( L"bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = CreateNewElem();
			if( ret ){
				DbgOut( L"bvhfile : LoadBVHFile : CreateNewElem error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
			break;
		case STATE_OFFSET:
//DbgOut( L"bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = LoadOffset();
			if( ret ){
				DbgOut( L"bvhfile : LoadBVHFile : LoadOffset error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
			break;
		case STATE_CHANELS:
//DbgOut( L"bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = LoadChanels();
			if( ret ){
				DbgOut( L"bvhfile : LoadBVHFile : LoadChanels error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
			break;
		case STATE_BRACE1:
//DbgOut( L"bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = BeginBrace();
			if( ret ){
				DbgOut( L"bvhfile : LoadBVHFile : BeginBrace error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
			break;
		case STATE_BRACE2:
//DbgOut( L"bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = EndBrace();
			if( ret ){
				DbgOut( L"bvhfile : LoadBVHFile : EndBrace error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
			break;
		case STATE_MOTION:

//DbgOut( L"bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			break;
		case STATE_FRAMES:
//DbgOut( L"bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = LoadFrames();
			if( ret ){
				DbgOut( L"bvhfile : LoadBVHFile : LoadFrames error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
//_ASSERT( 0 );
			break;
		case STATE_FRAMETIME:
//DbgOut( L"bvhfile : LoadBVHFile : state %s\r\n", bvhpat[m_state] );
			ret = LoadFrameTime();
			if( ret ){
				DbgOut( L"bvhfile : LoadBVHFile : LoadFrameTime error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
//_ASSERT( 0 );
			ret = LoadMotionParams();
			if( ret ){
				DbgOut( L"bvhfile : LoadBVHFile : LoadMotionParams error !!!\n" );
				_ASSERT( 0 );
				goto ldbvhexit;
			}
//_ASSERT( 0 );
			break;
		case STATE_MOTPARAMS:
			break;
		case STATE_NONE:
			m_pos += 1;
			break;
		case STATE_FINISH:
			m_pos = m_bufleng;
			break;
		default:
			_ASSERT( 0 );
			break;
		}

		if( (m_state < 0) || (m_state == STATE_FINISH) || (m_state == STATE_NONE) || (m_pos >= (int)m_bufleng) ){
			break;
		}
	}


	if( m_behead ){

		MultBVHElemReq(m_behead, srcmult);


		SetBVHElemPositionReq( m_behead );
		
		int beno;
		for( beno = 0; beno < m_benum; beno++ ){

			ret = (*(m_bearray + beno))->ConvertRotate2Q();
			_ASSERT(!ret);
			
			CalcBVHTreeQReq(m_behead);

			//ret = (*( m_bearray + beno ))->ConvZxyRot();
			ret = (*(m_bearray + beno))->ConvXYZRot();
			_ASSERT(!ret);

			CalcTransMatReq(m_behead);

			/////// for debug
			//char* findname = 0;
			//(*( m_bearray + beno ))->CheckNotAlNumName( &findname );
			//if( findname ){
			//	DbgOut( "bvhfile : LoadBVHFile : be CheckNotAlNumName : find : %s\r\n", findname );
			//}
		}

	}

	goto ldbvhexit;
ldbvhexit:
	
	//DestroyObjs();

	return ret;
}

int CBVHFile::SetBuffer( WCHAR* filename )
{
	DestroyObjs();

	m_hfile = CreateFileW( filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	

	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng <= 0 ){
		DbgOut( L"bvhfile : SetBuffer :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( L"bvhfile : SetBuffer :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );//bufleng + 1
	if( !newbuf ){
		DbgOut( L"bvhfile : SetBuffer :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	BOOL bresult = ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( !bresult || (rleng != readleng) ){
		DbgOut( L"bvhfile : SetBuffer :  ReadFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	m_buf = newbuf;
	*(m_buf + bufleng) = 0;//!!!!!!!!
	m_bufleng = bufleng;
	m_pos = 0;	

	return 0;
}
int CBVHFile::CheckFileHeader()
{
	char headerstr[20] = "HIERARCHY\r\n";
	char headerstr2[20] = "HIERARCHY\n";
	char headerstr3[20] = "HIERARCHY\r";

	size_t headerleng;

	char* findptr;
	findptr = strstr( m_buf, headerstr );
	if( findptr )
		headerleng = strlen( headerstr );

	if( !findptr ){
		findptr = strstr( m_buf, headerstr2 );
		if( findptr )
			headerleng = strlen( headerstr2 );
	}
	if( !findptr ){
		findptr = strstr( m_buf, headerstr3 );
		if( findptr )
			headerleng = strlen( headerstr3 );
	}

	if( findptr ){
		size_t leng;
		leng = ( findptr - m_buf + headerleng );
		if ((leng > 0) && (leng < 20)) {
			m_pos += (DWORD)leng;//!!!!!!!
			return 0;
		}
		else {
			DbgOut(L"bvhfile : CheckFileHeader : this file is not BVH format error !!!\n");
			_ASSERT(0);
			return 1;
		}

	}else{
		DbgOut( L"bvhfile : CheckFileHeader : this file is not BVH format error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
}

int CBVHFile::GetStateAndLine()
{
	int retstate = STATE_NONE;

	char chkchar;
	int startpos = m_pos;
	while( *(m_buf + startpos) ){
		chkchar = *(m_buf + startpos);
		if( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}') || (chkchar == ':')
		){
			break;
		}else{
			if( startpos < (int)m_bufleng ){
				startpos++;
			}else{
				break;
			}
		}
	}

	int endpos = startpos;
	while( *(m_buf + endpos) ){
		chkchar = *(m_buf + endpos);
		if( (endpos < (int)m_bufleng) &&
			( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}') || (chkchar == ':') )
		){
			endpos++;
		}else{
			break;
		}
	}

	int patno;
	int matchflag;
	int findpat = -1;
	int patleng;
	for( patno = STATE_ROOT; patno <= STATE_FRAMETIME; patno++ ){
		patleng = (int)strlen( bvhpat[patno] );
		if ((patleng > 0) && (patleng < 20)) {
			matchflag = strncmp(m_buf + startpos, bvhpat[patno], patleng);
			if (matchflag == 0) {
				findpat = patno;
				break;
			}
		}
	}
	if( findpat >= 0 ){
		retstate = patno;
		m_pos = (DWORD)(startpos + patleng);//!!!!!!!!!!!!!!!!!!
	}else{
		//_ASSERT( 0 );
		m_pos = (DWORD)endpos;
	}

	if( (retstate == STATE_MOTION) || (retstate == STATE_BRACE1) || (retstate == STATE_BRACE2) ){
		return retstate;//!!!!!!!!!!!!!!!!!!!
	}

////////////////
	int startpos2 = m_pos;
	while( *(m_buf + startpos2) ){
		chkchar = *(m_buf + startpos2);
		if( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}') || (chkchar == ':')
		){
			break;
		}else{
			if( startpos2 < (int)m_bufleng ){
				startpos2++;
			}else{
				break;
			}
		}
	}

	int endpos2 = startpos2;
	while( *(m_buf + endpos2) ){
		chkchar = *(m_buf + endpos2);
		if( (chkchar == '\r') || (chkchar == '\n') ){
			break;
		}else{
			if( endpos2 < (int)m_bufleng ){
				endpos2++;
			}else{
				break;
			}
		}
	}

	int lineleng = endpos2 - startpos2;
	if( lineleng >= BVHLINELENG ){
		_ASSERT( 0 );
		ZeroMemory( m_linechar, sizeof( char ) * BVHLINELENG );
		retstate = STATE_NONE;
	}else{
		strncpy_s( m_linechar, BVHLINELENG, m_buf + startpos2, lineleng );
		m_linechar[ lineleng ] = 0;
	}

	m_pos = endpos2;

	return retstate;
}


int CBVHFile::GetState()
{
	int retstate = STATE_NONE;

	char chkchar;
	int startpos = m_pos;
	while( *(m_buf + startpos) ){
		chkchar = *(m_buf + startpos);
		if( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}')
		){
			break;
		}else{
			if( startpos < (int)m_bufleng ){
				startpos++;
			}else{
				break;
			}
		}
	}
//	m_pos = startpos;


	int patno;
	int matchflag;
	int findpat = -1;
	size_t patleng;
	for( patno = STATE_ROOT; patno <= STATE_FRAMETIME; patno++ ){
		patleng = strlen( bvhpat[patno] );
		if ((patleng > 0) && (patleng < 20)) {
			//		matchflag = strncmp( m_buf + m_pos, bvhpat[patno], patleng );
			matchflag = strncmp(m_buf + startpos, bvhpat[patno], patleng);
			if (matchflag == 0) {
				findpat = patno;
				break;
			}
		}
	}

	if( findpat >= 0 ){
		retstate = patno;
//		m_pos += patleng;//!!!!!!!!!!!!!!!!!!
	}

	return retstate;
}

int CBVHFile::CreateNewElem()
{

	int ret;
//	ret = GetLine();
//	if( ret ){
//		DbgOut( L"bvhfile : CreateNewElem : GetLine error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}


	CBVHElem* newbe;
	newbe = new CBVHElem();
	if( !newbe ){
		DbgOut( L"bvhfile : CreateNewElem : newbe alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_linechar[BVHLINELENG - 1] = 0;
	ret = newbe->SetName( m_linechar );
	if( ret ){
		DbgOut( L"bvhfile : CreatenewElem : newbe SetName error !!!\n" );
		_ASSERT( 0 );
		delete newbe;
		return 1;
	}

	if( m_state == STATE_ROOT ){
		ret = newbe->SetIsRoot( 1 );
	}else{
		ret = newbe->SetIsRoot( 0 );
	}
	_ASSERT( !ret );

///////////

	newbe->SetParent( m_parbe );
	if( m_parbe ){
		if( !m_parbe->GetChild() ){
			//子供にセット
			m_parbe->SetChild( newbe );
			m_parbe->SetBoneNum( 1 );
			newbe->SetBroNo( 0 );
		}else{
			m_parbe->SetBoneNum( m_parbe->GetBoneNum() + 1 );

			//最後のbrotherにセット
			int brono = 0;
			CBVHElem* lastbro;
			CBVHElem* brobe;
			brobe = m_parbe->GetChild();
			lastbro = brobe;
			while( brobe ){
				lastbro = brobe;
				brobe = brobe->GetBrother();
				brono++;
			}


			_ASSERT( lastbro );
			lastbro->SetBrother( newbe );
			lastbro->SetBroNo( brono );
		}
	}

////////////

	if( !m_behead ){
		m_behead = newbe;
	}
////////////

	m_benum++;
	CBVHElem** newbearray = 0;
	newbearray = (CBVHElem**)realloc(m_bearray, sizeof(CBVHElem*) * m_benum);
	if( !newbearray ){
		DbgOut( L"bvhfile : CreateNewElem : bearray alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	else {
		m_bearray = newbearray;
	}

	*( m_bearray + m_benum - 1 ) = newbe;

/////////////

	m_curbe = newbe;


//	if( newbe->serialno == 4 ){
//		_ASSERT( 0 );
//	}


	return 0;
}

int CBVHFile::GetLine()
{
	char chkchar;
	int startpos2 = m_pos;
	while( *(m_buf + startpos2) ){
		chkchar = *(m_buf + startpos2);
		if( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}') || (chkchar == ':')
		){
			break;
		}else{
			if( startpos2 < (int)m_bufleng ){
				startpos2++;
			}else{
				break;
			}
		}
	}

	int endpos2 = startpos2;
	while( *(m_buf + endpos2) ){
		chkchar = *(m_buf + endpos2);
		if( (chkchar == '\r') || (chkchar == '\n') ){
			break;
		}else{
			if( endpos2 < (int)m_bufleng ){
				endpos2++;
			}else{
				break;
			}
		}
	}

	int lineleng = endpos2 - startpos2;
	if( lineleng >= BVHLINELENG ){
		_ASSERT( 0 );
		ZeroMemory( m_linechar, sizeof( char ) * BVHLINELENG );
	}else{
		strncpy_s( m_linechar, BVHLINELENG, m_buf + startpos2, lineleng );
		m_linechar[ lineleng ] = 0;
	}

	m_pos = endpos2;

	return 0;
}


/***
int CBVHFile::GetLine()
{

	char chkchar;
	int startpos = m_pos;
	while( *(m_buf + startpos) ){
		chkchar = *(m_buf + startpos);
		if( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
			((chkchar >= '0') && (chkchar <= '9')) || 
			(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') || 
			(chkchar == '{') || (chkchar == '}') || (chkchar == ':')
		){
			break;
		}else{
			if( startpos < (int)m_bufleng ){
				startpos++;
			}else{
				break;
			}
		}
	}
	m_pos = startpos;



	char retmark[5] = "\r\n";
	char retmark2[5] = "\n";
	char retmark3[5] = "\r";
	int retkind = 0;
	char* retptr;
	int leng;

	retptr = strstr( m_buf + m_pos, retmark );
	if( retptr ){
		leng = (int)( retptr - (m_buf + m_pos) );
		retkind = 1;
	}else{
		retptr = strstr( m_buf + m_pos, retmark2 );
		if( retptr ){
			leng = (int)( retptr - (m_buf + m_pos) );
			retkind = 2;
		}else{
			retptr = strstr( m_buf + m_pos, retmark3 );
			if( retptr ){
				leng = (int)( retptr - (m_buf + m_pos) );
				retkind = 3;
			}else{
				leng = m_bufleng - m_pos;
				retkind = 0;
			}
		}
	}


	if( leng >= BVHLINELENG ){
		DbgOut( L"bvhfile : GetLine : line leng too short error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strncpy_s( m_linechar, BVHLINELENG, m_buf + m_pos, leng );
	m_linechar[leng] = 0;

	switch( retkind ){
	case 1:
		m_pos += (leng + 2);
		break;
	case 2:
	case 3:
		m_pos += (leng + 1);
		break;
	default:
		m_pos += leng;
		break;
	}

	return 0;
}
***/
int CBVHFile::LoadOffset()
{
	int ret;
//	ret = GetLine();
//	if( ret ){
//		DbgOut( L"bvhfile : LoadOffset : GetLine error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	if( !m_curbe ){
		DbgOut( L"bvhfile : LoadOffset : curbe NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_linechar[BVHLINELENG - 1] = 0;
	ret = m_curbe->SetOffset( m_linechar );
	if( ret ){
		DbgOut( L"bvhfile : LoadOffset : curbe SetOffset error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBVHFile::LoadChanels()
{
	int ret;
//	ret = GetLine();
//	if( ret ){
//		DbgOut( L"bvhfile : LoadChanels : GetLine error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	if( !m_curbe ){
		DbgOut( L"bvhfile : LoadChanels : curbe NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_linechar[BVHLINELENG - 1] = 0;
	ret = m_curbe->SetChanels( m_linechar );
	if( ret ){
		DbgOut( L"bvhfile : LoadChanels : curbe SetChanels error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBVHFile::BeginBrace()
{
	m_parbe = m_curbe;

	m_pos += 1;//!!!!!!!!!!!!!!!!!!!!

	return 0;
}
int CBVHFile::EndBrace()
{
	if( !m_curbe ){
		DbgOut( L"bvhfile : EndBrace : curbe NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_parbe = m_curbe->GetParent();
	m_curbe = m_parbe;//!!!!!!!!!!!!!!

	m_pos += 1;//!!!!!!!!!!!!!!!!!!!!

	return 0;
}

int CBVHFile::LoadFrames()
{
	int ret;
//	ret = GetLine();
//	if( ret ){
//		DbgOut( L"bvhfile : LoadFrames : GetLine error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	int setflag;
	m_linechar[BVHLINELENG - 1] = 0;
	char* valuehead = m_linechar;

	valuehead = GetDigit( valuehead, &m_frames, &setflag );
	if( !setflag ){
		DbgOut( L"bvhfile : LoadFrames : GetDigit frames error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
/////////
	int beno;
	CBVHElem* beptr;
	for( beno = 0; beno < m_benum; beno++ ){
		beptr = *( m_bearray + beno );
		_ASSERT( beptr );
		if (beptr){
			ret = beptr->CreateMotionObjs(m_frames);
			if (ret){
				DbgOut(L"bvhfile : LoadFrames : be CreateMotionObjs error !!!\n");
				_ASSERT(0);
				return 1;
			}
		}
	}


	return 0;
}
int CBVHFile::LoadFrameTime()
{

//	ret = GetLine();
//	if( ret ){
//		DbgOut( L"bvhfile : LoadFrameTime : GetLine error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	int setflag;
	m_linechar[BVHLINELENG - 1] = 0;
	char* valuehead = m_linechar;

	float dummytime;
	//valuehead = GetFloat( valuehead, &m_frametime, &setflag );
	valuehead = GetFloat( valuehead, &dummytime, &setflag );
	if( !setflag ){
		DbgOut( L"bvhfile : LoadFrameTime : GetFloat frametime error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBVHFile::LoadMotionParams()
{
	int ret;

	int frameno;
	for( frameno = 0; frameno < m_frames; frameno++ ){
		ret = GetLine();
		if( ret ){
			DbgOut( L"bvhfile : LoadMotionParams : GetLine error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int setflag;
		m_linechar[BVHLINELENG - 1] = 0;
		char* valuehead = m_linechar;


		int beno;
		CBVHElem* beptr;
		for( beno = 0; beno < m_benum; beno++ ){
			beptr = *( m_bearray + beno );
			_ASSERT( beptr );
			if (beptr){
				int chanelnum;
				chanelnum = beptr->GetChanelNum();

				int paramno;
				for (paramno = 0; paramno < chanelnum; paramno++){
					if (!valuehead){
						DbgOut(L"bvhfile : LoadMotionParams : no more params error !!!\n");
						_ASSERT(0);
						return 1;
					}

					valuehead = GetFloat(valuehead, &(m_tempparam[paramno]), &setflag);
					if (!setflag){
						DbgOut(L"bvhfile : LoadMotionParams : GetFloat error !!!\n");
						_ASSERT(0);
						return 1;
					}
				}


				ret = beptr->SetMotionParams(frameno, m_tempparam);
				if (ret){
					DbgOut(L"bvhfile : LoadMotionParams : be SetMotionParams error !!!\n");
					_ASSERT(0);
					return 1;
				}
			}
		}

	}

	return 0;
}



char* CBVHFile::GetFloat( char* srcstr, float* dstfloat, int* dstsetflag )
{
	char* valuehead = srcstr;

	int curpos;
	size_t strleng;

	curpos = 0;
	strleng = strlen( srcstr );
	if ((strleng <= 0) || (strleng > 2048)) {
		_ASSERT(0);
		*dstsetflag = 0;//
		return 0;
	}


	//先頭の非数字をスキップ
	while( (curpos < strleng) && ( (isdigit( *valuehead ) == 0) && (*valuehead != '-'))  ){
		if( isalpha( *valuehead ) != 0 ){
			//illeagal letter
			DbgOut( L"bvhfile : GetDigit : isalpha error !!!\n" );
			*dstsetflag = 0;
			return 0;//!!!!
		}
		valuehead++;
		curpos++;
	}

	char* valueend = valuehead;
	//valueの終わりをサーチ
	int valueleng = 0;
	while( (curpos < strleng) && ( isdigit( *valueend ) || (*valueend == '-') ) || (*valueend == '.') || (*valueend == 'e') ){
		valueend++;
		curpos++;
		valueleng++;
	}


	//char ---> float
	if( valueleng >= 255 ){
		_ASSERT( 0 );
		*dstsetflag = 0;//
		return 0;
	}
	char tmpchar[256];
	strncpy_s( tmpchar, 256, valuehead, valueleng );
	tmpchar[valueleng] = 0;

	float tempfloat;
	tempfloat = (float)atof( tmpchar );

	*dstfloat = tempfloat;
	*dstsetflag = 1;//

	return valueend;

}



char* CBVHFile::GetDigit( char* srcstr, int* dstint, int* dstsetflag )
{

	char* valuehead = srcstr;

	int curpos;
	size_t strleng;

	curpos = 0;
	strleng = strlen( srcstr );
	if ((strleng <= 0) || (strleng > 2048)) {
		_ASSERT(0);
		*dstsetflag = 0;//
		return 0;
	}

	//先頭の非数字をスキップ
	while( (curpos < strleng) && ( (isdigit( *valuehead ) == 0) && (*valuehead != '-'))  ){
		if( isalpha( *valuehead ) != 0 ){
			//illeagal letter
			DbgOut( L"bvhfile : GetDigit : isalpha error !!!\n" );
			*dstsetflag = 0;
			return 0;//!!!!
		}
		valuehead++;
		curpos++;
	}

	char* valueend = valuehead;
	//valueの終わりをサーチ
	int valueleng = 0;
	while( (curpos < strleng) && ( isdigit( *valueend ) || (*valueend == '-') ) ){
		valueend++;
		curpos++;
		valueleng++;
	}


	//char ---> int
	if( valueleng >= 255 ){
		_ASSERT( 0 );
		*dstsetflag = 0;//
		return 0;
	}
	char tmpchar[256];
	strncpy_s( tmpchar, 256, valuehead, valueleng );
	tmpchar[valueleng] = 0;

	int tempint;
	tempint = atoi( tmpchar );

	*dstint = tempint;
	*dstsetflag = 1;//

	return valueend;
}

void CBVHFile::DbgOutBVHElemReq( CBVHElem* outbe, int srcdepth )
{
	int ret;

	ret = outbe->DbgOutBVHElem( srcdepth, 1 );
	if( ret ){
		DbgOut( L"bvhfile : DbgOutBVHElemReq : be DbgOutBVHElem error !!!\n" );
		_ASSERT( 0 );
		return;
	}

	if( outbe->GetChild() ){
		DbgOutBVHElemReq( outbe->GetChild(), srcdepth + 1 );
	}

	if( outbe->GetBrother() ){
		DbgOutBVHElemReq( outbe->GetBrother(), srcdepth );
	}

}

void CBVHFile::SetBVHElemPositionReq( CBVHElem* srcbe )
{
	int ret;

	ret = srcbe->SetPosition();
	if( ret ){
		DbgOut( L"bvhfile : SetBVHElemPositionReq : be SetPosition error !!!\n" );
		_ASSERT( 0 );
		return;
	}

	if( srcbe->GetChild() ){
		SetBVHElemPositionReq( srcbe->GetChild() );
	}

	if( srcbe->GetBrother() ){
		SetBVHElemPositionReq( srcbe->GetBrother() );
	}

}

void CBVHFile::CountJointNumReq( CBVHElem* srcbe, int* jointnumptr, int* bonenumptr )
{
	(*jointnumptr)++;

///////////
	if( srcbe->GetChild() ){
		(*bonenumptr)++;//!!!!!!!!!!!
		CountJointNumReq( srcbe->GetChild(), jointnumptr, bonenumptr );
	}
	if( srcbe->GetBrother() ){
		(*bonenumptr)++;//!!!!!!!!!!!
		CountJointNumReq( srcbe->GetBrother(), jointnumptr, bonenumptr );
	}
}

void CBVHFile::MultBVHElemReq( CBVHElem* srcbe, float srcmult )
{
	int ret;

	ret = srcbe->Mult( srcmult );
	if( ret ){
		DbgOut( L"bvhfile : MultBVHElemReq : be Mult error !!!\n" );
		_ASSERT( 0 );
		return;
	}

////////
	if( srcbe->GetChild() ){
		MultBVHElemReq( srcbe->GetChild(), srcmult );
	}
	if( srcbe->GetBrother() ){
		MultBVHElemReq( srcbe->GetBrother(), srcmult );
	}
}


void CBVHFile::CalcBVHTreeQReq( CBVHElem* srcbe )
{
	int fno;

	if( srcbe->GetParent() ){
		for( fno = 0; fno < m_frames; fno++ ){
			*(srcbe->GetTreeQ() + fno) = *(srcbe->GetParent()->GetTreeQ() + fno) * *(srcbe->GetQPtr() + fno);
		}
	}else{
		for( fno = 0; fno < m_frames; fno++ ){
			*(srcbe->GetTreeQ() + fno) = *(srcbe->GetQPtr() + fno);
		}
	}
//////////
	if( srcbe->GetChild() ){
		CalcBVHTreeQReq( srcbe->GetChild() );
	}
	if( srcbe->GetBrother() ){
		CalcBVHTreeQReq( srcbe->GetBrother() );
	}

}

void CBVHFile::CalcTransMatReq(CBVHElem* srcbe)
{
	int fno;

	ChaVector3 roottra;
	ChaMatrix roottramat, befrot, aftrot, rot;
	ChaMatrixIdentity(&roottramat);
	ChaMatrixIdentity(&befrot);
	ChaMatrixIdentity(&aftrot);
	ChaMatrixIdentity(&rot);
	m_behead->GetTrans(0, &roottra);
	ChaMatrixTranslation(&roottramat, roottra.x, roottra.y, roottra.z);
	ChaMatrixTranslation(&befrot, -srcbe->GetPosition().x, -srcbe->GetPosition().y, -srcbe->GetPosition().z);
	ChaMatrixTranslation(&aftrot, srcbe->GetPosition().x, srcbe->GetPosition().y, srcbe->GetPosition().z);

	if (srcbe->GetParent()){
		for (fno = 0; fno < m_frames; fno++){
			rot = (srcbe->GetQPtr() + fno)->MakeRotMatX();
			*(srcbe->GetTransMat() + fno) = befrot * rot * aftrot * *(srcbe->GetParent()->GetTransMat() + fno);// * roottramat;
		}
	}
	else{
		for (fno = 0; fno < m_frames; fno++){
			rot = (srcbe->GetQPtr() + fno)->MakeRotMatX();
			*(srcbe->GetTransMat() + fno) = befrot * rot * aftrot;// *roottramat;
		}
	}
	//////////
	if (srcbe->GetChild()){
		CalcTransMatReq(srcbe->GetChild());
	}
	if (srcbe->GetBrother()){
		CalcTransMatReq(srcbe->GetBrother());
	}

}


int CBVHFile::SetBVHSameNameBoneSeri()
{
/***
	int ret;

	if( !lpth || !lpsh ){
		DbgOut( L"bvhfile : SetBVHSameNameBoneSeri : handler NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CBVHElem* curbe;
	int beno;
	for( beno = 0; beno < m_benum; beno++ ){
		curbe = *( m_bearray + beno );

		int serialno;
		int findserialno = -1;//!!!!!!!!
		ret = lpth->GetPartNoByName( curbe->name, &serialno );
		if( ret ){
			DbgOut( L"bvhfile : SetBVHSameNameBoneSeri : th GetPartNoByName error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( serialno >= 0 ){

			CShdElem* selem;
			selem = (*lpsh)( serialno );
			_ASSERT( selem );

			if( selem->IsJoint() && (selem->type != SHDMORPH) ){
				CPart* partptr;
				partptr = selem->part;
				if( !partptr ){
					DbgOut( L"bvhfile : SetBVHSameNameBoneSeri : partptr NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( partptr->bonenum > 0 ){
					findserialno = serialno;//!!!!!!!!!!
				}

			}
		}

		curbe->samenameboneseri = findserialno;

	}
***/
	return 0;
}



int CBVHFile::CalcMPQ( CBVHElem* beptr, int fno, CQuaternion* dstq )
{
	//int ret;

	CBVHElem* curbe = beptr->GetParent();//!!!!!!!
	CBVHElem* findparbe = 0;
	while( curbe ){
		if( curbe->GetSameNameBoneSeri() > 0 ){
			findparbe = curbe;
			break;
		}
		curbe = curbe->GetParent();
	}

	if( findparbe ){
		CQuaternion curq;
		CQuaternion parq;
		CQuaternion invparq;

		curq = *(beptr->GetTreeQ() + fno);
		parq = *(findparbe->GetTreeQ() + fno);
		parq.inv( &invparq );

		*dstq = invparq * curq;
		//*dstq = curq * invparq;
	
	}else{
		*dstq = *(beptr->GetTreeQ() + fno);
	}

	return 0;
}

int CBVHFile::Write2File( char* lpFormat, ... )
{
	if( !m_hwfile ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[2048];
			
	ZeroMemory( outchar, 2048 );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, 2048, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 ){
		_ASSERT( 0 );
		return 1;
	}

	outchar[2048 - 1] = 0;//保険
	wleng = (unsigned long)strlen( outchar );
	WriteFile( m_hwfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );	
		return 1;
	}

	return 0;	
}

int CBVHFile::WriteTab( int tabnum )
{
	if( !m_hwfile ){
		_ASSERT( 0 );
		return 0;
	}

	if( tabnum >= 2048 ){
		_ASSERT( 0 );
		return 1;
	}

	char outchar[2048];
	ZeroMemory( outchar, sizeof( char ) * 2048 );
	int tno;
	int wtabnum = min((2048 - 1), tabnum);
	for( tno = 0; tno < wtabnum; tno++ ){
		outchar[tno] = '\t';
	}

	unsigned long wleng, writeleng;
	outchar[2048 - 1] = 0;//保険
	wleng = (unsigned long)::strlen(outchar);
	WriteFile( m_hwfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );	
		return 1;
	}

	return 0;
}


/***
int CBVHFile::WriteBVHMotion( int* wno2seri, int motid, float srcmult )
{
	int ret;
	int frameleng = 0;
	ret = lpmh->GetMotionFrameLength( motid, &frameleng );
	if( ret ){
		DbgOut( L"bvhfile : WriteBVHMotion : mh GetMotionFrameLeng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ChaVector3* befeul;
	befeul = new ChaVector3[ m_writenum ];
	if( !befeul ){
		DbgOut( L"bvhfile : WriteBVHMotion : befuel alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( befeul, sizeof( ChaVector3 ) * m_writenum );



	Write2File( "MOTION\r\n" );
	Write2File( "Frames: %d\r\n", frameleng );
	float frametime = 1.0f / 60.0f;
	Write2File( "Frame Time: %f\r\n", frametime );

	int frameno;
	int wno;
	for( frameno = 0; frameno < frameleng; frameno++ ){
		for( wno = 0; wno < m_writenum; wno++ ){
			int seri;
			seri = *( wno2seri + wno );

			if( seri > 0 ){
				CMotionCtrl* mcptr;
				mcptr = (*lpmh)( seri );
				_ASSERT( mcptr );
				CMotionInfo* mi;
				mi = mcptr->motinfo;
				_ASSERT( mi );

				CShdElem* selem;
				selem = (*lpsh)( seri );
				_ASSERT( selem );

				ret = lpmh->SetBoneAxisQ( lpsh, selem->serialno, lpmh->GetZaType( motid ), motid, frameno );
				_ASSERT( !ret );
				CQuaternion axisq;
				ret = selem->GetBoneAxisQ( &axisq );
				_ASSERT( !ret );



				CMotionPoint2 mp;
				int hasmpflag;
				ret = mi->CalcMotionPointOnFrame( &axisq, selem, &mp, motid, frameno, &hasmpflag );
				if( ret ){
					DbgOut( L"bvhfile : WriteBVHMotion : mi CalcMotionPointOnFrame error !!!\n" );
					_ASSERT( 0 );
					delete [] befeul;
					return 1;
				}
								
				ChaVector3 neweul;
				ret = qToEulerYXZ( selem, &mp.m_q, &neweul );
				_ASSERT( !ret );
				ret = modifyEuler( &neweul, &(befeul[wno]) );
				_ASSERT( !ret );
				
				befeul[wno] = neweul;

				if( wno == 0 ){
					Write2File( "%f\t%f\t%f\t", mp.m_mvx, mp.m_mvy, mp.m_mvz );
					Write2File( "%f\t%f\t%f\t", neweul.z, neweul.x, neweul.y - 180.0f );
				}else{
					Write2File( "%f\t%f\t%f\t", neweul.z, neweul.x, neweul.y );
				}



			}else if( seri == 0 ){
				//Write2File( "%d-EmptyData\r\n", wno );
			}else{
				//Write2File( "%d-IniData\r\n", wno );
				Write2File( "0\t0\t0\t" );
			}

		}
		Write2File( "\r\n" );
	}


	delete [] befeul;


	return 0;
}
***/
#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#include <ChaScene.h>
#include <GrassFile.h>


#define DBGH
#include <dbg.h>
#include <vector>

#include <shlwapi.h>


extern ChaScene* g_chascene;
extern GRASSMOVER g_grassmover;


using namespace std;

CGrassFile::CGrassFile()
{
	InitParams();
}

CGrassFile::~CGrassFile()
{
	DestroyObjs();
}

int CGrassFile::InitParams()
{
	CXMLIO::InitParams();
	return 0;
}

int CGrassFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}


int CGrassFile::WriteGrassFile(WCHAR* strpath)
{
	m_mode = XMLIO_WRITE;

	m_hfile = CreateFile( strpath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"GrassFile : WriteGrassFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<Grass>\r\n" ), return 1 );  
	CallF( Write2File( "    <FileInfo>1001-01</FileInfo>\r\n" ), return 1 );

	CallF(WriteGrass(), return 1);

	CallF( Write2File( "</Grass>\r\n" ), return 1 );

	FlushFileBuffers(m_hfile);
	SetEndOfFile(m_hfile);


	return 0;
}

int CGrassFile::WriteGrass()
{
	if (g_chascene == nullptr) {
		_ASSERT(0);
		return 1;
	}
	if (g_grassmover.mover_model == nullptr) {
		_ASSERT(0);
		return 1;
	}

	WCHAR modeldir[MAX_PATH] = { 0L };
	WCHAR filename[MAX_PATH] = { 0L };
	wcscpy_s(modeldir, MAX_PATH, g_grassmover.mover_model->GetModelFolder());
	wcscpy_s(filename, MAX_PATH, g_grassmover.mover_model->GetFileName());
	char mbmodeldir[MAX_PATH] = { 0 };
	char mbfilename[MAX_PATH] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, modeldir, -1, mbmodeldir, MAX_PATH, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, filename, -1, mbfilename, MAX_PATH, NULL, NULL);

	CallF(Write2File("    <MODELDIR>%s</MODELDIR>\r\n", mbmodeldir), return 1);
	CallF(Write2File("    <FILENAME>%s</FILENAME>\r\n", mbfilename), return 1);

	CallF(Write2File("    <MOVER_R>%f</MOVER_R>\r\n", g_grassmover.mover_r), return 1);

	return 0;
}


int CGrassFile::LoadGrassFile(WCHAR* strpath)
{
	m_mode = XMLIO_LOAD;



	//èâä˙âª
	g_grassmover.Init();


	BOOL bexist;
	bexist = PathFileExists(strpath);
	if (!bexist)
	{
		_ASSERT(0);
		return 2;//not found
	}

	m_hfile = CreateFile( strpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	

	CallF( SetBuffer(), return 1 );
	CallF( ReadGrass(&m_xmliobuf), return 1 );

	return 0;
}

int CGrassFile::ReadGrass(XMLIOBUF* xmlbuf)
{
	int getdir = 0;
	char modeldir[MAX_PATH] = { 0 };
	Read_Str(xmlbuf, "<MODELDIR>", "</MODELDIR>", modeldir, MAX_PATH);

	int getname = 0;
	char modelname[MAX_PATH] = { 0 };
	Read_Str(xmlbuf, "<FILENAME>", "</FILENAME>", modelname, MAX_PATH);

	int getr = 0;
	float modelr = 0.0f;
	getr = Read_Float(xmlbuf, "<MOVER_R>", "</MOVER_R>", &modelr);


	if ((getdir == 0) && (getname == 0)) {
		WCHAR wmodeldir[MAX_PATH] = { 0L };
		WCHAR wmodelname[MAX_PATH] = { 0L };
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, modeldir, -1, wmodeldir, MAX_PATH);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, modelname, -1, wmodelname, MAX_PATH);

		vector<MODELELEM> modelindex;
		g_chascene->GetModelIndex(modelindex);

		int indexnum = (int)modelindex.size();
		int chkindex;
		for (chkindex = 0; chkindex < indexnum; chkindex++) {
			MODELELEM chkelem = modelindex[chkindex];
			if (chkelem.modelptr) {
				if ((wcscmp(wmodeldir, chkelem.modelptr->GetModelFolder()) == 0) &&
					(wcscmp(wmodelname, chkelem.modelptr->GetFileName()) == 0)) {

					g_grassmover.mover_model = chkelem.modelptr;
					break;
				}
			}
		}

		if ((g_grassmover.mover_model != nullptr) && (getr == 0)) {
			g_grassmover.mover_r = modelr;
		}
		else {
			g_grassmover.mover_r = 0.0f;
		}
	}
	else {
		g_grassmover.Init();
	}


	return 0;
}


/***
int CGrassFile::CheckFileVersion( XMLIOBUF* xmlbuf )
{
	char kind[256];
	char version[256];
	char type[256];
	ZeroMemory( kind, sizeof( char ) * 256 );
	ZeroMemory( version, sizeof( char ) * 256 );
	ZeroMemory( type, sizeof( char ) * 256 );

	CallF( Read_Str( xmlbuf, "<kind>", "</kind>", kind, 256 ), return 1 );
	CallF( Read_Str( xmlbuf, "<version>", "</version>", version, 256 ), return 1 );
	CallF( Read_Str( xmlbuf, "<type>", "</type>", type, 256 ), return 1 );

	int cmpkind, cmpversion1, cmpversion2, cmptype;
	cmpkind = strcmp( kind, "OpenRDBMotionFile" );
	cmpversion1 = strcmp( version, "1001" );
	cmpversion2 = strcmp( version, "1002" );
	cmptype = strcmp( type, "0" );

	if( (cmpkind == 0) && (cmptype == 0) ){
		if( cmpversion1 == 0 ){
			return 1;
		}else if( cmpversion2 == 0 ){
			return 2;
		}else{
			return 0;
		}

		return 0;
	}else{
		_ASSERT( 0 );
		return 0;
	}

	return 0;
}
***/

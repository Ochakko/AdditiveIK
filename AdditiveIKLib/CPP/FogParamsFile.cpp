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

#include <FogParamsFile.h>

#include <Model.h>
#include <MQOObject.h>
#include <MQOMaterial.h>


#define DBGH
#include <dbg.h>

#include <GlobalVar.h>


using namespace std;


CFogParamsFile::CFogParamsFile()
{
	InitParams();
}

CFogParamsFile::~CFogParamsFile()
{
	DestroyObjs();
}

int CFogParamsFile::InitParams()
{
	CXMLIO::InitParams();
	return 0;
}

int CFogParamsFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();
	InitParams();
	return 0;
}

int CFogParamsFile::WriteFogParamsFile(WCHAR* filename, int srcindex)
{
	if (!filename) {
		_ASSERT(0);
		return 1;
	}
	if ((srcindex < 0) || (srcindex >= FOGSLOTNUM)) {
		_ASSERT(0);
		return 1;
	}

	m_mode = XMLIO_WRITE;

	m_hfile = CreateFile( filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"FogParamsFile : WriteFogParamsFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<FogParamsFile>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );

	//CallF( Write2File( "  <MotSpeed>%f</MotSpeed>\r\n", srcmotspeed ), return 1 );
	//char mprojname[256];
	//ZeroMemory( mprojname, sizeof( char ) * 256 );
	//WideCharToMultiByte( CP_ACP, 0, projname, -1, mprojname, 256, NULL, NULL );



	CallF(Write2File("  <FogKind>%d</FogKind>\r\n", g_fogparams[srcindex].GetFogKind()), return 1);

	CallF(Write2File("  <DistColorR>%.4f</DistColorR>\r\n", g_fogparams[srcindex].GetDistColor().x), return 1);
	CallF(Write2File("  <DistColorG>%.4f</DistColorG>\r\n", g_fogparams[srcindex].GetDistColor().y), return 1);
	CallF(Write2File("  <DistColorB>%.4f</DistColorB>\r\n", g_fogparams[srcindex].GetDistColor().z), return 1);

	CallF(Write2File("  <DistNear>%.1f</DistNear>\r\n", g_fogparams[srcindex].GetDistNear()), return 1);
	CallF(Write2File("  <DistFar>%.1f</DistFar>\r\n", g_fogparams[srcindex].GetDistFar()), return 1);
	CallF(Write2File("  <DistRate>%.2f</DistRate>\r\n", g_fogparams[srcindex].GetDistRate()), return 1);



	CallF(Write2File("  <HeightColorR>%.4f</HeightColorR>\r\n", g_fogparams[srcindex].GetHeightColor().x), return 1);
	CallF(Write2File("  <HeightColorG>%.4f</HeightColorG>\r\n", g_fogparams[srcindex].GetHeightColor().y), return 1);
	CallF(Write2File("  <HeightColorB>%.4f</HeightColorB>\r\n", g_fogparams[srcindex].GetHeightColor().z), return 1);

	CallF(Write2File("  <HeightMaxH>%.1f</HeightMaxH>\r\n", g_fogparams[srcindex].GetHeightHigh()), return 1);
	CallF(Write2File("  <HeightRate>%.2f</HeightRate>\r\n", g_fogparams[srcindex].GetHeightRate()), return 1);




	CallF( Write2File( "</FogParamsFile>\r\n" ), return 1 );

	FlushFileBuffers(m_hfile);
	SetEndOfFile(m_hfile);

	return 0;
}

int CFogParamsFile::WriteFileInfo()
{
	//2024/03/07
	CallF(Write2File("  <FileInfo>\r\n    <kind>FogParamsFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);


	return 0;
}


int CFogParamsFile::LoadFogParamsFile(WCHAR* filename, int srcindex)
{
	if (!filename) {
		_ASSERT(0);
		return 1;
	}
	if ((srcindex < 0) || (srcindex >= FOGSLOTNUM)) {
		_ASSERT(0);
		return 1;
	}


	m_hfile = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		//_ASSERT( 0 );
		return 1;
	}	

	CallF( SetBuffer(), return 1 );

	int posstep = 0;
	//XMLIOBUF projinfobuf;
	//ZeroMemory( &projinfobuf, sizeof( XMLIOBUF ) );
	//CallF( SetXmlIOBuf( &m_xmliobuf, "<ProjectInfo>", "</ProjectInfo>", &projinfobuf ), return 1 );
	//int charanum = 0;
	//CallF( ReadProjectInfo( &projinfobuf, &charanum ), return 1 );

	//m_xmliobuf.pos = 0;
	//m_motspeed = 1.0f;
	//Read_Float( &m_xmliobuf, "<MotSpeed>", "</MotSpeed>", &m_motspeed );

	int fogkind = g_fogparams[srcindex].GetFogKind();
	Read_Int(&m_xmliobuf, "<FogKind>", "</FogKind>", &fogkind);
	g_fogparams[srcindex].SetFogKind(fogkind);

	float distR = g_fogparams[srcindex].GetDistColor().x;
	Read_Float(&m_xmliobuf, "<DistColorR>", "</DistColorR>", &distR);
	float distG = g_fogparams[srcindex].GetDistColor().y;
	Read_Float(&m_xmliobuf, "<DistColorG>", "</DistColorG>", &distG);
	float distB = g_fogparams[srcindex].GetDistColor().z;
	Read_Float(&m_xmliobuf, "<DistColorB>", "</DistColorB>", &distB);
	g_fogparams[srcindex].SetDistColor(ChaVector4(distR, distG, distB, 1.0f));

	float distnear = g_fogparams[srcindex].GetDistNear();
	Read_Float(&m_xmliobuf, "<DistNear>", "</DistNear>", &distnear);
	g_fogparams[srcindex].SetDistNear(distnear);
	float distfar = g_fogparams[srcindex].GetDistFar();
	Read_Float(&m_xmliobuf, "<DistFar>", "</DistFar>", &distfar);
	g_fogparams[srcindex].SetDistFar(distfar);
	float distrate = g_fogparams[srcindex].GetDistRate();
	Read_Float(&m_xmliobuf, "<DistRate>", "</DistRate>", &distrate);
	g_fogparams[srcindex].SetDistRate(distrate);


	float heightR = g_fogparams[srcindex].GetHeightColor().x;
	Read_Float(&m_xmliobuf, "<HeightColorR>", "</HeightColorR>", &heightR);
	float heightG = g_fogparams[srcindex].GetHeightColor().y;
	Read_Float(&m_xmliobuf, "<HeightColorG>", "</HeightColorG>", &heightG);
	float heightB = g_fogparams[srcindex].GetHeightColor().z;
	Read_Float(&m_xmliobuf, "<HeightColorB>", "</HeightColorB>", &heightB);
	g_fogparams[srcindex].SetHeightColor(ChaVector4(heightR, heightG, heightB, 1.0f));



	float heightmaxh = g_fogparams[srcindex].GetHeightHigh();
	Read_Float(&m_xmliobuf, "<HeightMaxH>", "</HeightMaxH>", &heightmaxh);
	g_fogparams[srcindex].SetHeightHigh(heightmaxh);

	float heightrate = g_fogparams[srcindex].GetHeightRate();
	Read_Float(&m_xmliobuf, "<HeightRate>", "</HeightRate>", &heightrate);
	g_fogparams[srcindex].SetHeightRate(heightrate);

	return 0;
}

/***
int CFogParamsFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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

	int cmpkind, cmpversion, cmptype;
	cmpkind = strcmp( kind, "OpenRDBProjectFile" );
	cmpversion = strcmp( version, "1001" );
	cmptype = strcmp( type, "0" );

	if( (cmpkind == 0) && (cmpversion == 0) && (cmptype == 0) ){
		return 0;
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/
int CFogParamsFile::ReadProjectInfo( XMLIOBUF* xmlbuf, int* charanumptr )
{
	//CallF( Read_Int( xmlbuf, "<CharaNum>", "</CharaNum>", charanumptr ), return 1 );

	return 0;
}

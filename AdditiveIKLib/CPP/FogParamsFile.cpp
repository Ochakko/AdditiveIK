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

int CFogParamsFile::WriteFogParamsFile(WCHAR* filename)
{
	if (!filename) {
		_ASSERT(0);
		return 1;
	}

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



	CallF(Write2File("  <FogKind>%d</FogKind>\r\n", g_fogparams.GetFogKind()), return 1);

	CallF(Write2File("  <DistColorR>%.4f</DistColorR>\r\n", g_fogparams.GetDistColor().x), return 1);
	CallF(Write2File("  <DistColorG>%.4f</DistColorG>\r\n", g_fogparams.GetDistColor().y), return 1);
	CallF(Write2File("  <DistColorB>%.4f</DistColorB>\r\n", g_fogparams.GetDistColor().z), return 1);

	CallF(Write2File("  <DistNear>%.1f</DistNear>\r\n", g_fogparams.GetDistNear()), return 1);
	CallF(Write2File("  <DistFar>%.1f</DistFar>\r\n", g_fogparams.GetDistFar()), return 1);
	CallF(Write2File("  <DistRate>%.2f</DistRate>\r\n", g_fogparams.GetDistRate()), return 1);



	CallF(Write2File("  <HeightColorR>%.4f</HeightColorR>\r\n", g_fogparams.GetHeightColor().x), return 1);
	CallF(Write2File("  <HeightColorG>%.4f</HeightColorG>\r\n", g_fogparams.GetHeightColor().y), return 1);
	CallF(Write2File("  <HeightColorB>%.4f</HeightColorB>\r\n", g_fogparams.GetHeightColor().z), return 1);

	CallF(Write2File("  <HeightMaxH>%.1f</HeightMaxH>\r\n", g_fogparams.GetHeightHigh()), return 1);
	CallF(Write2File("  <HeightRate>%.2f</HeightRate>\r\n", g_fogparams.GetHeightRate()), return 1);




	CallF( Write2File( "</FogParamsFile>\r\n" ), return 1 );

	return 0;
}

int CFogParamsFile::WriteFileInfo()
{
	//2024/03/07
	CallF(Write2File("  <FileInfo>\r\n    <kind>FogParamsFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);


	return 0;
}


int CFogParamsFile::LoadFogParamsFile(WCHAR* filename)
{
	if (!filename) {
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

	int fogkind = g_fogparams.GetFogKind();
	Read_Int(&m_xmliobuf, "<FogKind>", "</FogKind>", &fogkind);
	g_fogparams.SetFogKind(fogkind);

	float distR = g_fogparams.GetDistColor().x;
	Read_Float(&m_xmliobuf, "<DistColorR>", "</DistColorR>", &distR);
	float distG = g_fogparams.GetDistColor().y;
	Read_Float(&m_xmliobuf, "<DistColorG>", "</DistColorG>", &distG);
	float distB = g_fogparams.GetDistColor().z;
	Read_Float(&m_xmliobuf, "<DistColorB>", "</DistColorB>", &distB);
	g_fogparams.SetDistColor(ChaVector4(distR, distG, distB, 1.0f));

	float distnear = g_fogparams.GetDistNear();
	Read_Float(&m_xmliobuf, "<DistNear>", "</DistNear>", &distnear);
	float distfar = g_fogparams.GetDistFar();
	Read_Float(&m_xmliobuf, "<DistFar>", "</DistFar>", &distfar);
	float distrate = g_fogparams.GetDistRate();
	Read_Float(&m_xmliobuf, "<DistRate>", "</DistRate>", &distrate);




	float heightR = g_fogparams.GetHeightColor().x;
	Read_Float(&m_xmliobuf, "<HeightColorR>", "</HeightColorR>", &heightR);
	float heightG = g_fogparams.GetHeightColor().y;
	Read_Float(&m_xmliobuf, "<HeightColorG>", "</HeightColorG>", &heightG);
	float heightB = g_fogparams.GetHeightColor().z;
	Read_Float(&m_xmliobuf, "<HeightColorB>", "</HeightColorB>", &heightB);
	g_fogparams.SetHeightColor(ChaVector4(heightR, heightG, heightB, 1.0f));



	float heightmaxh = g_fogparams.GetHeightHigh();
	Read_Float(&m_xmliobuf, "<HeightMaxH>", "</HeightMaxH>", &heightmaxh);
	g_fogparams.SetHeightHigh(heightmaxh);

	float heightrate = g_fogparams.GetHeightRate();
	Read_Float(&m_xmliobuf, "<HeightRate>", "</HeightRate>", &heightrate);
	g_fogparams.SetHeightRate(heightrate);

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

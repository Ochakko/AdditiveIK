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

#include <ShaderTypeFile.h>

#include <Model.h>
#include <MQOObject.h>
#include <MQOMaterial.h>


#define DBGH
#include <dbg.h>

using namespace std;


CShaderTypeFile::CShaderTypeFile()
{
	InitParams();
}

CShaderTypeFile::~CShaderTypeFile()
{
	DestroyObjs();
}

int CShaderTypeFile::InitParams()
{
	CXMLIO::InitParams();
	return 0;
}

int CShaderTypeFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();
	InitParams();
	return 0;
}

int CShaderTypeFile::WriteShaderTypeFile(WCHAR* filename, CModel* srcmodel)
{
	if (!filename || !srcmodel) {
		_ASSERT(0);
		return 1;
	}

	m_hfile = CreateFile( filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"ShaderTypeFile : WriteShaderTypeFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<SHADERTYPEFILE>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );

	//CallF( Write2File( "  <MotSpeed>%f</MotSpeed>\r\n", srcmotspeed ), return 1 );
	//char mprojname[256];
	//ZeroMemory( mprojname, sizeof( char ) * 256 );
	//WideCharToMultiByte( CP_ACP, 0, projname, -1, mprojname, 256, NULL, NULL );

	int materialnum = srcmodel->GetMQOMaterialSize();
	int mqomatindex;
	for (mqomatindex = 0; mqomatindex < materialnum; mqomatindex++) {
		CMQOMaterial* mqomat = srcmodel->GetMQOMaterialByIndex(mqomatindex);
		if (mqomat) {
			char materialname[256] = { 0 };
			strcpy_s(materialname, 256, mqomat->GetName());
			int shadertype = mqomat->GetShaderType();
			float metalcoef = mqomat->GetMetalCoef();
			float smoothcoef = mqomat->GetSmoothCoef();
			float lightscale[8];
			int litindex;
			for (litindex = 0; litindex < 8; litindex++) {
				lightscale[litindex] = mqomat->GetLightScale(litindex);
			}

			CallF(Write2File("  <Material>\r\n"), return 1);

			CallF(Write2File("    <MaterialName>%s</MaterialName>\r\n", materialname), return 1);
			CallF(Write2File("    <ShaderType>%d</ShaderType>\r\n", shadertype), return 1);
			CallF(Write2File("    <MetalCoef>%f</MetalCoef>\r\n", metalcoef), return 1);
			CallF(Write2File("    <SmoothCoef>%f</SmoothCoef>\r\n", smoothcoef), return 1);

			CallF(Write2File("    <LightScale1>%f</LightScale1>\r\n", lightscale[0]), return 1);
			CallF(Write2File("    <LightScale2>%f</LightScale2>\r\n", lightscale[1]), return 1);
			CallF(Write2File("    <LightScale3>%f</LightScale3>\r\n", lightscale[2]), return 1);
			CallF(Write2File("    <LightScale4>%f</LightScale4>\r\n", lightscale[3]), return 1);
			CallF(Write2File("    <LightScale5>%f</LightScale5>\r\n", lightscale[4]), return 1);
			CallF(Write2File("    <LightScale6>%f</LightScale6>\r\n", lightscale[5]), return 1);
			CallF(Write2File("    <LightScale7>%f</LightScale7>\r\n", lightscale[6]), return 1);
			CallF(Write2File("    <LightScale8>%f</LightScale8>\r\n", lightscale[7]), return 1);

			CallF(Write2File("  </Material>\r\n"), return 1);

		}
	}


	CallF( Write2File( "</SHADERTYPEFILE>\r\n" ), return 1 );

	return 0;
}

int CShaderTypeFile::WriteFileInfo()
{
	CallF(Write2File("  <FileInfo>\r\n    <kind>ShaderTypeFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	return 0;
}


int CShaderTypeFile::LoadShaderTypeFile(WCHAR* filename, CModel* srcmodel)
{
	if (!filename || !srcmodel) {
		_ASSERT(0);
		return 1;
	}

	m_hfile = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
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

	m_xmliobuf.pos = 0;
	int errorval = 0;
	while(errorval == 0){
		XMLIOBUF materialbuf;
		ZeroMemory(&materialbuf, sizeof(XMLIOBUF));
		errorval = SetXmlIOBuf( &m_xmliobuf, "<Material>", "</Material>", &materialbuf );
		if (errorval == 0) {
			char materialname[256] = { 0 };
			CallF(Read_Str(&materialbuf, "<MaterialName>", "</MaterialName>", materialname, 256), return 1);
			CMQOMaterial* curmqomat = srcmodel->GetMQOMaterialByName(materialname);
			if (curmqomat) {
				int shadertype = -2;
				Read_Int(&materialbuf, "<ShaderType>", "</ShaderType>", &shadertype);
				curmqomat->SetShaderType(shadertype);

				float metalcoef = 0.25f;
				Read_Float(&materialbuf, "<MetalCoef>", "</MetalCoef>", &metalcoef);
				curmqomat->SetMetalCoef(metalcoef);

				float smoothcoef = 0.25f;
				Read_Float(&materialbuf, "<SmoothCoef>", "</SmoothCoef>", &smoothcoef);
				curmqomat->SetSmoothCoef(smoothcoef);

				float lightscale1 = 1.0f;
				Read_Float(&materialbuf, "<LightScale1>", "</LightScale1>", &lightscale1);
				curmqomat->SetLightScale(0, lightscale1);

				float lightscale2 = 1.0f;
				Read_Float(&materialbuf, "<LightScale2>", "</LightScale2>", &lightscale2);
				curmqomat->SetLightScale(1, lightscale2);

				float lightscale3 = 1.0f;
				Read_Float(&materialbuf, "<LightScale3>", "</LightScale3>", &lightscale3);
				curmqomat->SetLightScale(2, lightscale3);

				float lightscale4 = 1.0f;
				Read_Float(&materialbuf, "<LightScale4>", "</LightScale4>", &lightscale4);
				curmqomat->SetLightScale(3, lightscale4);

				float lightscale5 = 1.0f;
				Read_Float(&materialbuf, "<LightScale5>", "</LightScale5>", &lightscale5);
				curmqomat->SetLightScale(4, lightscale5);

				float lightscale6 = 1.0f;
				Read_Float(&materialbuf, "<LightScale6>", "</LightScale6>", &lightscale6);
				curmqomat->SetLightScale(5, lightscale6);

				float lightscale7 = 1.0f;
				Read_Float(&materialbuf, "<LightScale7>", "</LightScale7>", &lightscale7);
				curmqomat->SetLightScale(6, lightscale7);

				float lightscale8 = 1.0f;
				Read_Float(&materialbuf, "<LightScale8>", "</LightScale8>", &lightscale8);
				curmqomat->SetLightScale(7, lightscale8);
			}
		}		
	}

	return 0;
}

/***
int CShaderTypeFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
int CShaderTypeFile::ReadProjectInfo( XMLIOBUF* xmlbuf, int* charanumptr )
{
	//CallF( Read_Int( xmlbuf, "<CharaNum>", "</CharaNum>", charanumptr ), return 1 );

	return 0;
}

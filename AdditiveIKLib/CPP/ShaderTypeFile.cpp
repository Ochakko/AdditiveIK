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
			bool enableEmission = mqomat->GetEnableEmission();
			float emissiveScale = mqomat->GetEmissiveScale();
			HSVTOON hsvtoon = mqomat->GetHSVToon();
			float specularcoef = mqomat->GetSpecularCoef();
			bool normaly0flag = mqomat->GetNormalY0Flag();
			bool shadowcasterflag = mqomat->GetShadowCasterFlag();
			bool lightingflag = mqomat->GetLightingFlag();
			float alphatestclipval = (float)mqomat->GetAlphaTestClipVal();

			int litindex;
			for (litindex = 0; litindex < 8; litindex++) {
				lightscale[litindex] = mqomat->GetLightScale(litindex);
			}

			CallF(Write2File("  <Material>\r\n"), return 1);

			CallF(Write2File("    <MaterialName>%s</MaterialName>\r\n", materialname), return 1);
			CallF(Write2File("    <ShaderType>%d</ShaderType>\r\n", shadertype), return 1);
			CallF(Write2File("    <MetalCoef>%f</MetalCoef>\r\n", metalcoef), return 1);
			CallF(Write2File("    <SmoothCoef>%f</SmoothCoef>\r\n", smoothcoef), return 1);

			//2024/03/22
			CallF(Write2File("    <AlphaTestClipVal>%f</AlphaTestClipVal>\r\n", alphatestclipval), return 1);

			CallF(Write2File("    <LightScale1>%f</LightScale1>\r\n", lightscale[0]), return 1);
			CallF(Write2File("    <LightScale2>%f</LightScale2>\r\n", lightscale[1]), return 1);
			CallF(Write2File("    <LightScale3>%f</LightScale3>\r\n", lightscale[2]), return 1);
			CallF(Write2File("    <LightScale4>%f</LightScale4>\r\n", lightscale[3]), return 1);
			CallF(Write2File("    <LightScale5>%f</LightScale5>\r\n", lightscale[4]), return 1);
			CallF(Write2File("    <LightScale6>%f</LightScale6>\r\n", lightscale[5]), return 1);
			CallF(Write2File("    <LightScale7>%f</LightScale7>\r\n", lightscale[6]), return 1);
			CallF(Write2File("    <LightScale8>%f</LightScale8>\r\n", lightscale[7]), return 1);

			if (enableEmission) {
				CallF(Write2File("    <EnableEmission>1</EnableEmission>\r\n"), return 1);
			}
			else {
				CallF(Write2File("    <EnableEmission>0</EnableEmission>\r\n"), return 1);
			}

			//2024/01/30
			CallF(Write2File("    <EmissiveScale>%f</EmissiveScale>\r\n", emissiveScale), return 1);

			//2024/02/13
			CallF(Write2File("    <ToonHiColorH>%.2f</ToonHiColorH>\r\n", hsvtoon.hicolorh), return 1);
			CallF(Write2File("    <ToonLowColorH>%.2f</ToonLowColorH>\r\n", hsvtoon.lowcolorh), return 1);
			CallF(Write2File("    <ToonHiAddH>%.2f</ToonHiAddH>\r\n", hsvtoon.hiaddhsv.x), return 1);
			CallF(Write2File("    <ToonHiAddS>%.2f</ToonHiAddS>\r\n", hsvtoon.hiaddhsv.y), return 1);
			CallF(Write2File("    <ToonHiAddV>%.2f</ToonHiAddV>\r\n", hsvtoon.hiaddhsv.z), return 1);
			CallF(Write2File("    <ToonHiAddA>%.2f</ToonHiAddA>\r\n", hsvtoon.hiaddhsv.w), return 1);
			CallF(Write2File("    <ToonLowAddH>%.2f</ToonLowAddH>\r\n", hsvtoon.lowaddhsv.x), return 1);
			CallF(Write2File("    <ToonLowAddS>%.2f</ToonLowAddS>\r\n", hsvtoon.lowaddhsv.y), return 1);
			CallF(Write2File("    <ToonLowAddV>%.2f</ToonLowAddV>\r\n", hsvtoon.lowaddhsv.z), return 1);
			CallF(Write2File("    <ToonLowAddA>%.2f</ToonLowAddA>\r\n", hsvtoon.lowaddhsv.w), return 1);

			//2024/02/14
			CallF(Write2File("    <ToonLightIndex>%d</ToonLightIndex>\r\n", hsvtoon.lightindex), return 1);
			CallF(Write2File("    <ToonBaseH>%.2f</ToonBaseH>\r\n", hsvtoon.basehsv.x), return 1);
			CallF(Write2File("    <ToonBaseS>%.2f</ToonBaseS>\r\n", hsvtoon.basehsv.y), return 1);
			CallF(Write2File("    <ToonBaseV>%.2f</ToonBaseV>\r\n", hsvtoon.basehsv.z), return 1);
			CallF(Write2File("    <ToonBaseA>%.2f</ToonBaseA>\r\n", hsvtoon.basehsv.w), return 1);

			//2024/02/18
			int gradationflag = hsvtoon.gradationflag ? 1 : 0;
			CallF(Write2File("    <Gradation>%d</Gradation>\r\n", gradationflag), return 1);
			int powertoon = hsvtoon.powertoon ? 1 : 0;
			CallF(Write2File("    <PowerToon>%d</PowerToon>\r\n", powertoon), return 1);

			//2024/02/19
			CallF(Write2File("    <SpecularCoef>%.3f</SpecularCoef>\r\n", specularcoef), return 1);
			int y0flag = normaly0flag ? 1 : 0;
			CallF(Write2File("    <NormalY0>%d</NormalY0>\r\n", normaly0flag), return 1);

			//2024/03/03
			int casterflag = shadowcasterflag ? 1 : 0;
			CallF(Write2File("    <ShadowCaster>%d</ShadowCaster>\r\n", casterflag), return 1);

			//2024/03/07
			int lightflag = lightingflag ? 1 : 0;
			CallF(Write2File("    <Lighting>%d</Lighting>\r\n", lightflag), return 1);



			CallF(Write2File("  </Material>\r\n"), return 1);

		}
	}


	CallF( Write2File( "</SHADERTYPEFILE>\r\n" ), return 1 );

	return 0;
}

int CShaderTypeFile::WriteFileInfo()
{
	//CallF(Write2File("  <FileInfo>\r\n    <kind>ShaderTypeFile</kind>\r\n    <version>1002</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);

	//2024/01/30
	//CallF(Write2File("  <FileInfo>\r\n    <kind>ShaderTypeFile</kind>\r\n    <version>1003</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//2024/02/13
	//CallF(Write2File("  <FileInfo>\r\n    <kind>ShaderTypeFile</kind>\r\n    <version>1004</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//2024/02/18
	//CallF(Write2File("  <FileInfo>\r\n    <kind>ShaderTypeFile</kind>\r\n    <version>1005</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);
	//2024/02/19
	CallF(Write2File("  <FileInfo>\r\n    <kind>ShaderTypeFile</kind>\r\n    <version>1006</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);


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


				//2024/03/22
				float alphatestclipval = (float)(8.0 / 255.0);
				Read_Float(&materialbuf, "<AlphaTestClipVal>", "</AlphaTestClipVal>", &alphatestclipval);
				curmqomat->SetAlphaTestClipVal(alphatestclipval);


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

				int enableEmission = 0;
				Read_Int(&materialbuf, "<EnableEmission>", "</EnableEmission>", &enableEmission);
				if (enableEmission == 1) {
					curmqomat->SetEnableEmission(true);
				}
				else {
					curmqomat->SetEnableEmission(false);
				}
				
				//2024/01/30
				float emissiveScale = 1.0f;
				Read_Float(&materialbuf, "<EmissiveScale>", "</EmissiveScale>", &emissiveScale);
				curmqomat->SetEmissiveScale(emissiveScale);

				//2024/02/19
				float specularcoef = 0.1250f;
				Read_Float(&materialbuf, "<SpecularCoef>", "</SpecularCoef>", &specularcoef);
				curmqomat->SetSpecularCoef(specularcoef);
				int y0flag = 0;
				Read_Int(&materialbuf, "<NormalY0>", "</NormalY0>", &y0flag);
				bool normaly0flag;
				normaly0flag = (y0flag == 1) ? true : false;
				curmqomat->SetNormalY0Flag(normaly0flag);


				//2024/03/03
				int casterflag = 1;
				Read_Int(&materialbuf, "<ShadowCaster>", "</ShadowCaster>", &casterflag);
				bool shadowcasterflag;
				shadowcasterflag = (casterflag == 1) ? true : false;
				curmqomat->SetShadowCasterFlag(shadowcasterflag);


				//2024/02/13
				HSVTOON hsvtoon;
				hsvtoon.Init();
				float hicolorh = hsvtoon.hicolorh;
				Read_Float(&materialbuf, "<ToonHiColorH>", "</ToonHiColorH>\r\n", &hicolorh);
				hsvtoon.hicolorh = hicolorh;

				float lowcolorh = hsvtoon.lowcolorh;
				Read_Float(&materialbuf, "<ToonLowColorH>", "</ToonLowColorH>\r\n", &lowcolorh);
				hsvtoon.lowcolorh = lowcolorh;

				float hiaddh = hsvtoon.hiaddhsv.x;
				Read_Float(&materialbuf, "<ToonHiAddH>", "</ToonHiAddH>\r\n", &hiaddh);
				hsvtoon.hiaddhsv.x = hiaddh;

				float hiadds = hsvtoon.hiaddhsv.y;
				Read_Float(&materialbuf, "<ToonHiAddS>", "</ToonHiAddS>\r\n", &hiadds);
				hsvtoon.hiaddhsv.y = hiadds;

				float hiaddv = hsvtoon.hiaddhsv.z;
				Read_Float(&materialbuf, "<ToonHiAddV>", "</ToonHiAddV>\r\n", &hiaddv);
				hsvtoon.hiaddhsv.z = hiaddv;

				float hiadda = hsvtoon.hiaddhsv.w;
				Read_Float(&materialbuf, "<ToonHiAddA>", "</ToonHiAddA>\r\n", &hiadda);
				hsvtoon.hiaddhsv.w = hiadda;

				float lowaddh = hsvtoon.lowaddhsv.x;
				Read_Float(&materialbuf, "<ToonLowAddH>", "</ToonLowAddH>\r\n", &lowaddh);
				hsvtoon.lowaddhsv.x = lowaddh;

				float lowadds = hsvtoon.lowaddhsv.y;
				Read_Float(&materialbuf, "<ToonLowAddS>", "</ToonLowAddS>\r\n", &lowadds);
				hsvtoon.lowaddhsv.y = lowadds;

				float lowaddv = hsvtoon.lowaddhsv.z;
				Read_Float(&materialbuf, "<ToonLowAddV>", "</ToonLowAddV>\r\n", &lowaddv);
				hsvtoon.lowaddhsv.z = lowaddv;

				float lowadda = hsvtoon.lowaddhsv.w;
				Read_Float(&materialbuf, "<ToonLowAddA>", "</ToonLowAddA>\r\n", &lowadda);
				hsvtoon.lowaddhsv.w = lowadda;

				//2024/02/14
				int lightindex = hsvtoon.lightindex;
				Read_Int(&materialbuf, "<ToonLightIndex>", "</ToonLightIndex>\r\n", &lightindex);
				hsvtoon.lightindex = lightindex;

				ChaVector4 basehsv = curmqomat->GetDif4F().RGB2HSV();
				Read_Float(&materialbuf, "<ToonBaseH>", "</ToonBaseH>\r\n", &(basehsv.x));
				Read_Float(&materialbuf, "<ToonBaseS>", "</ToonBaseS>\r\n", &(basehsv.y));
				Read_Float(&materialbuf, "<ToonBaseV>", "</ToonBaseV>\r\n", &(basehsv.z));
				Read_Float(&materialbuf, "<ToonBaseA>", "</ToonBaseA>\r\n", &(basehsv.w));
				hsvtoon.basehsv = basehsv;

				//2024/02/18
				int gradationflag = hsvtoon.gradationflag ? 1 : 0;
				Read_Int(&materialbuf, "<Gradation>", "</Gradation>\r\n", &gradationflag);
				if (gradationflag == 1) {
					hsvtoon.gradationflag = true;
				}
				else {
					hsvtoon.gradationflag = false;
				}

				//2024/02/18
				int powertoon = hsvtoon.powertoon ? 1 : 0;
				Read_Int(&materialbuf, "<PowerToon>", "</PowerToon>\r\n", &powertoon);
				if (gradationflag == 1) {
					hsvtoon.powertoon = true;
				}
				else {
					hsvtoon.powertoon = false;
				}

				curmqomat->SetHSVToon(hsvtoon);



				//2024/03/07
				int lightflag = 1;
				Read_Int(&materialbuf, "<Lighting>", "</Lighting>\r\n", &lightflag);
				if (lightflag == 1) {
					curmqomat->SetLightingFlag(true);
				}
				else {
					curmqomat->SetLightingFlag(false);
				}

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

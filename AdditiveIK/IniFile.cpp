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

#include "IniFile.h"

#include <GlobalVar.h>

#define DBGH
#include <dbg.h>


CIniFile::CIniFile()
{
	InitParams();
}

CIniFile::~CIniFile()
{
	DestroyObjs();
}

int CIniFile::InitParams()
{
	CXMLIO::InitParams();

	return 0;
}

int CIniFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();
	InitParams();

	return 0;
}

int CIniFile::WriteIniFile(WCHAR* srcpath)
{
	if (!srcpath) {
		_ASSERT(0);
		return 1;
	}

	m_hfile = CreateFile( srcpath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"IniFile : WriteIniFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<INI>\r\n" ), return 1 );  
	CallF( WriteFileInfo(), return 1 );
	
	CallF(WriteIniInfo(), return 1);

	CallF(Write2File("\r\n</INI>\r\n"), return 1);


	return 0;
}

int CIniFile::WriteFileInfo()
{

	//CallF( Write2File( "  <FileInfo>\r\n    <kind>AdditiveIKIniFile</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n" ), return 1 );

	////2024/01/12
	//CallF(Write2File("  <FileInfo>\r\n    <kind>AdditiveIKIniFile</kind>\r\n    <version>1002</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);

	//2025/03/30
	CallF(Write2File("  <FileInfo>\r\n    <kind>AdditiveIKIniFile</kind>\r\n    <version>1003</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"), return 1);


	return 0;
}

int CIniFile::WriteIniInfo()
{
	CallF(Write2File("  <IniFileBody>\r\n"), return 1);

	CallF(Write2File("    <ClearColorIndex>%d</ClearColorIndex>\r\n", g_ClearColorIndex), return 1);
	CallF(Write2File("    <UsePhysIK>%d</UsePhysIK>\r\n", g_usephysik), return 1);
	CallF(Write2File("    <PrepCntOnPhysIK>%d</PrepCntOnPhysIK>\r\n", g_prepcntonphysik), return 1);

	CallF(Write2File("    <AmbientFactorAtLoading>%f</AmbientFactorAtLoading>\r\n", g_AmbientFactorAtLoading), return 1);
	CallF(Write2File("    <DiffuseFactorAtLoading>%f</DiffuseFactorAtLoading>\r\n", g_DiffuseFactorAtLoading), return 1);
	CallF(Write2File("    <SpecularFactorAtLoading>%f</SpecularFactorAtLoading>\r\n", g_SpecularFactorAtLoading), return 1);
	CallF(Write2File("    <EmissiveFactorAtLoading>%f</EmissiveFactorAtLoading>\r\n", g_EmissiveFactorAtLoading), return 1);

	CallF(Write2File("    <AmbientFactorAtSaving>%f</AmbientFactorAtSaving>\r\n", g_AmbientFactorAtSaving), return 1);
	CallF(Write2File("    <DiffuseFactorAtSaving>%f</DiffuseFactorAtSaving>\r\n", g_AmbientFactorAtSaving), return 1);
	CallF(Write2File("    <SpecularFactorAtSaving>%f</SpecularFactorAtSaving>\r\n", g_AmbientFactorAtSaving), return 1);
	CallF(Write2File("    <EmissiveFactorAtSaving>%f</EmissiveFactorAtSaving>\r\n", g_AmbientFactorAtSaving), return 1);

	CallF(Write2File("    <BoneMarkBrightness>%f</BoneMarkBrightness>\r\n", g_bonemark_bright), return 1);
	CallF(Write2File("    <RigidMarkAlpha>%f</RigidMarkAlpha>\r\n", g_rigidmark_alpha), return 1);
	CallF(Write2File("    <RigMarkAlpha>%f</RigMarkAlpha>\r\n", g_rigmark_alpha), return 1);

	CallF(Write2File("    <IKRate>%f</IKRate>\r\n", g_ikrate), return 1);
	CallF(Write2File("    <IKMaxDeg>%f</IKMaxDeg>\r\n", g_ikmaxdeg), return 1);
	CallF(Write2File("    <PosConstTimes>%d</PosConstTimes>\r\n", g_iktargettimes), return 1);


	CallF(Write2File("  </IniFileBody>\r\n"), return 1);

	return 0;
}

int CIniFile::LoadIniFile(WCHAR* srcpath)
{
	if (!srcpath) {
		_ASSERT(0);
		return 1;
	}

	m_hfile = CreateFile( srcpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		//_ASSERT( 0 );
		return 1;
	}	

	CallF(SetBuffer(), return 1);

	int posstep = 0;
	XMLIOBUF fileinfobuf;
	ZeroMemory( &fileinfobuf, sizeof( XMLIOBUF ) );
	CallF( SetXmlIOBuf( &m_xmliobuf, "<FileInfo>", "</FileInfo>", &fileinfobuf ), return 1 );
	CallF( ReadFileInfo( &fileinfobuf ), return 1 );

	m_xmliobuf.pos = 0;
	XMLIOBUF jointbuf;
	ZeroMemory( &jointbuf, sizeof( XMLIOBUF ) );
	int result;
	result = SetXmlIOBuf( &m_xmliobuf, "<IniFileBody>", "</IniFileBody>", &jointbuf );
	if (result == 0) {
		CallF(ReadIniInfo(&jointbuf), return 1);
	}

	return 0;
}

/***
int CIniFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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
int CIniFile::ReadFileInfo( XMLIOBUF* xmlbuf )
{

	return 0;
}
int CIniFile::ReadIniInfo(XMLIOBUF* xmlbuf )
{
	int clearcolorindex = 0;
	int usephysik = 0;
	int prepcntonphysik = 30;

	int result1 = 0;
	int result2 = 0;
	int result3 = 0;
	result1 = Read_Int( xmlbuf, "<ClearColorIndex>", "</ClearColorIndex>", &clearcolorindex );
	if (result1 == 0) {
		if ((clearcolorindex >= 0) && (clearcolorindex < BGCOL_MAX)) {
			g_ClearColorIndex = clearcolorindex;
		}
		else {
			g_ClearColorIndex = 0;
		}
	}
	else {
		g_ClearColorIndex = 0;
	}

	result2 = Read_Int(xmlbuf, "<UsePhysIK>", "</UsePhysIK>", &usephysik);
	if (result2 == 0) {
		if ((usephysik >= 0) && (usephysik <= 1)) {
			g_usephysik = usephysik;
		}
		else {
			g_usephysik = 0;
		}
	}
	else {
		g_usephysik = 0;
	}


	result3 = Read_Int( xmlbuf, "<PrepCntOnPhysIK>", "</PrepCntOnPhysIK>", &prepcntonphysik );
	if (result3 == 0) {
		if ((prepcntonphysik >= 0) && (prepcntonphysik <= 100)) {
			g_prepcntonphysik = prepcntonphysik;
		}
		else {
			g_prepcntonphysik = 30;
		}
	}
	else {
		g_prepcntonphysik = 30;
	}



	int result4, result5, result6, result7;
	float AmbientFactorAtLoading, DiffuseFactorAtLoading, SpecularFactorAtLoading, EmissiveFactorAtLoading;

	result4 = Read_Float(xmlbuf, "<AmbientFactorAtLoading>", "</AmbientFactorAtLoading>", &AmbientFactorAtLoading);
	if (result4 == 0) {
		if ((AmbientFactorAtLoading >= 0.0) && (AmbientFactorAtLoading <= 100.0f)) {
			g_AmbientFactorAtLoading = AmbientFactorAtLoading;
		}
		else {
			g_AmbientFactorAtLoading = 1.0f;
		}
	}
	else {
		g_AmbientFactorAtLoading = 1.0f;
	}
	result5 = Read_Float(xmlbuf, "<DiffuseFactorAtLoading>", "</DiffuseFactorAtLoading>", &DiffuseFactorAtLoading);
	if (result5 == 0) {
		if ((DiffuseFactorAtLoading >= 0.0) && (DiffuseFactorAtLoading <= 100.0f)) {
			g_DiffuseFactorAtLoading = DiffuseFactorAtLoading;
		}
		else {
			g_DiffuseFactorAtLoading = 1.0f;
		}
	}
	else {
		g_DiffuseFactorAtLoading = 1.0f;
	}
	result6 = Read_Float(xmlbuf, "<SpecularFactorAtLoading>", "</SpecularFactorAtLoading>", &SpecularFactorAtLoading);
	if (result6 == 0) {
		if ((SpecularFactorAtLoading >= 0.0) && (SpecularFactorAtLoading <= 100.0f)) {
			g_SpecularFactorAtLoading = SpecularFactorAtLoading;
		}
		else {
			g_SpecularFactorAtLoading = 1.0f;
		}
	}
	else {
		g_SpecularFactorAtLoading = 1.0f;
	}
	result7 = Read_Float(xmlbuf, "<EmissiveFactorAtLoading>", "</EmissiveFactorAtLoading>", &EmissiveFactorAtLoading);
	if (result7 == 0) {
		if ((EmissiveFactorAtLoading >= 0.0) && (EmissiveFactorAtLoading <= 100.0f)) {
			g_EmissiveFactorAtLoading = EmissiveFactorAtLoading;
		}
		else {
			g_EmissiveFactorAtLoading = 1.0f;
		}
	}
	else {
		g_EmissiveFactorAtLoading = 1.0f;
	}


	int result8, result9, result10, result11;
	float AmbientFactorAtSaving, DiffuseFactorAtSaving, SpecularFactorAtSaving, EmissiveFactorAtSaving;

	result8 = Read_Float(xmlbuf, "<AmbientFactorAtSaving>", "</AmbientFactorAtSaving>", &AmbientFactorAtSaving);
	if (result8 == 0) {
		if ((AmbientFactorAtSaving >= 0.0) && (AmbientFactorAtSaving <= 100.0f)) {
			g_AmbientFactorAtSaving = AmbientFactorAtSaving;
		}
		else {
			g_AmbientFactorAtSaving = 1.0f;
		}
	}
	else {
		g_AmbientFactorAtSaving = 1.0f;
	}
	result9 = Read_Float(xmlbuf, "<DiffuseFactorAtSaving>", "</DiffuseFactorAtSaving>", &DiffuseFactorAtSaving);
	if (result9 == 0) {
		if ((DiffuseFactorAtSaving >= 0.0) && (DiffuseFactorAtSaving <= 100.0f)) {
			g_DiffuseFactorAtSaving = DiffuseFactorAtSaving;
		}
		else {
			g_DiffuseFactorAtSaving = 1.0f;
		}
	}
	else {
		g_DiffuseFactorAtSaving = 1.0f;
	}
	result10 = Read_Float(xmlbuf, "<SpecularFactorAtSaving>", "</SpecularFactorAtSaving>", &SpecularFactorAtSaving);
	if (result10 == 0) {
		if ((SpecularFactorAtSaving >= 0.0) && (SpecularFactorAtSaving <= 100.0f)) {
			g_SpecularFactorAtSaving = SpecularFactorAtSaving;
		}
		else {
			g_SpecularFactorAtSaving = 1.0f;
		}
	}
	else {
		g_SpecularFactorAtSaving = 1.0f;
	}
	result11 = Read_Float(xmlbuf, "<EmissiveFactorAtSaving>", "</EmissiveFactorAtSaving>", &EmissiveFactorAtSaving);
	if (result11 == 0) {
		if ((EmissiveFactorAtSaving >= 0.0) && (EmissiveFactorAtSaving <= 100.0f)) {
			g_EmissiveFactorAtSaving = EmissiveFactorAtSaving;
		}
		else {
			g_EmissiveFactorAtSaving = 1.0f;
		}
	}
	else {
		g_EmissiveFactorAtSaving = 1.0f;
	}



	int result12, result13, result14;
	float bonemarkbrightness, rigidmarkalpha, rigmarkalpha;
	result12 = Read_Float(xmlbuf, "<BoneMarkBrightness>", "</BoneMarkBrightness>", &bonemarkbrightness);
	if (result12 == 0) {
		if ((bonemarkbrightness >= 0.0) && (bonemarkbrightness <= 1.0f)) {
			g_bonemark_bright = bonemarkbrightness;
		}
		else {
			g_bonemark_bright = 1.0f;
		}
	}
	else {
		g_bonemark_bright = 1.0f;
	}
	result13 = Read_Float(xmlbuf, "<RigidMarkAlpha>", "</RigidMarkAlpha>", &rigidmarkalpha);
	if (result13 == 0) {
		if ((rigidmarkalpha >= 0.0) && (rigidmarkalpha <= 1.0f)) {
			g_rigidmark_alpha = rigidmarkalpha;
		}
		else {
			g_rigidmark_alpha = 0.75f;
		}
	}
	else {
		g_rigidmark_alpha = 0.75f;
	}
	result14 = Read_Float(xmlbuf, "<RigMarkAlpha>", "</RigMarkAlpha>", &rigmarkalpha);
	if (result14 == 0) {
		if ((rigmarkalpha >= 0.0) && (rigmarkalpha <= 1.0f)) {
			g_rigmark_alpha = rigmarkalpha;
		}
		else {
			g_rigmark_alpha = 0.75f;
		}
	}
	else {
		g_rigmark_alpha = 0.75f;
	}


	int result15, result16, result17;
	float ikrate, ikmaxdeg;
	int iktargettimes;
	result15 = Read_Float(xmlbuf, "<IKRate>", "</IKRate>", &ikrate);
	if (result15 == 0) {
		if ((ikrate >= 0.0f) && (ikrate <= 1000.0f)) {
			g_ikrate = ikrate;
		}
		else {
			g_ikrate = 0.50f;
		}
	}
	else {
		g_ikrate = 0.50f;
	}
	result16 = Read_Float(xmlbuf, "<IKMaxDeg>", "</IKMaxDeg>", &ikmaxdeg);
	if (result16 == 0) {
		if ((ikmaxdeg >= 0.0f) && (ikmaxdeg <= 360.0f)) {
			g_ikmaxdeg = ikmaxdeg;
		}
		else {
			g_ikmaxdeg = 2.0f;
		}
	}
	else {
		g_ikmaxdeg = 2.0f;
	}
	result17 = Read_Int(xmlbuf, "<PosConstTimes>", "</PosConstTimes>", &iktargettimes);
	if (result17 == 0) {
		if ((iktargettimes >= 1) && (iktargettimes <= 5000)) {
			g_iktargettimes = iktargettimes;
		}
		else {
			g_iktargettimes = 100;
		}
	}
	else {
		g_iktargettimes = 100;
	}

/*
	CallF(Write2File("    <AmbientFactorAtLoading>%f</AmbientFactorAtLoading>\r\n", g_AmbientFactorAtLoading), return 1);
	CallF(Write2File("    <DiffuseFactorAtLoading>%f</DiffuseFactorAtLoading>\r\n", g_DiffuseFactorAtLoading), return 1);
	CallF(Write2File("    <SpecularFactorAtLoading>%f</SpecularFactorAtLoading>\r\n", g_SpecularFactorAtLoading), return 1);
	CallF(Write2File("    <EmissiveFactorAtLoading>%f</EmissiveFactorAtLoading>\r\n", g_EmissiveFactorAtLoading), return 1);

	CallF(Write2File("    <AmbientFactorAtLoading>%f</AmbientFactorAtSaving>\r\n", g_AmbientFactorAtSaving), return 1);
	CallF(Write2File("    <DiffuseFactorAtLoading>%f</DiffuseFactorAtSaving>\r\n", g_AmbientFactorAtSaving), return 1);
	CallF(Write2File("    <SpecularFactorAtLoading>%f</SpecularFactorAtSaving>\r\n", g_AmbientFactorAtSaving), return 1);
	CallF(Write2File("    <EmissiveFactorAtLoading>%f</EmissiveFactorAtSaving>\r\n", g_AmbientFactorAtSaving), return 1);

	CallF(Write2File("    <IKRate>%f</IKRate>\r\n", g_ikrate), return 1);
	CallF(Write2File("    <IKMaxDeg>%f</IKMaxDeg>\r\n", g_ikmaxdeg), return 1);
	CallF(Write2File("    <PosConstTimes>%d</PosConstTimes>\r\n", g_iktargettimes), return 1);
*/
	return 0;
}

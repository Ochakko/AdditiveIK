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

#include <RigFile.h>

#include <Model.h>
#include <Bone.h>
//#include <BoneProp.h>

#define DBGH
#include <dbg.h>
#include <map>
#include <vector>

using namespace std;



extern void InitCustomRig(CUSTOMRIG* dstcr, CBone* parentbone, int rigno);
extern int IsValidCustomRig(CModel* srcmodel, CUSTOMRIG srccr, CBone* parentbone);
//void SetCustomRigBone(CUSTOMRIG* dstcr, CBone* childbone);
extern int IsValidRigElem(CModel* srcmodel, RIGELEM srcrigelem);




CRigFile::CRigFile()
{
	InitParams();
}

CRigFile::~CRigFile()
{
	DestroyObjs();
}

int CRigFile::InitParams()
{
	CXMLIO::InitParams();

	m_model = 0;
	return 0;
}

int CRigFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}


int CRigFile::WriteRigFile( WCHAR* strpath, CModel* srcmodel )
{
	if (!srcmodel){
		_ASSERT(0);
		return 1;
	}

	m_model = srcmodel;
	m_mode = XMLIO_WRITE;
	if( !m_model->GetTopBone() ){
		return 0;
	}

	m_hfile = CreateFile( strpath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"RigFile : WriteRigFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<RIGFILE>\r\n" ), return 1 );  
	
	//CallF( Write2File( "    <FileInfo>1001-01</FileInfo>\r\n" ), return 1 );
	
	//2023/03/01
	//disporder[0,2]-->[0,15]
	//CallF(Write2File("    <FileInfo>1001-02</FileInfo>\r\n"), return 1);

	//2023/03/02 1.2.0.14 RC2
	//disporder[0,15]-->[0,RIGPOSINDEXMAX]
	//shapemult新規　[0, RIGMULTINDEXMAX]
	//CallF(Write2File("    <FileInfo>1001-03</FileInfo>\r\n"), return 1);

	//2023/03/02 1.2.0.14 RC3
	//shapekind新規　[RIGSHAPE_SPHERE(0), RIGSHAPE_RING(1)]
	//rigcolor新規　[RIGCOLOR_RED(0), RIGCOLOR_BLUE(2)]
	//CallF(Write2File("    <FileInfo>1001-04</FileInfo>\r\n"), return 1);
	//2024/01/09
	CallF(Write2File("    <FileInfo>1002-01</FileInfo>\r\n"), return 1);


	//WriteRigReq( m_model->GetTopBone(false) );

	std::map<int, CBone*>::iterator itrbone;
	for (itrbone = m_model->GetBoneListBegin(); itrbone != m_model->GetBoneListEnd(); itrbone++) {
		CBone* srcbone = itrbone->second;
		if (srcbone && srcbone->IsSkeleton()) {
			int rigno;
			for (rigno = 0; rigno < MAXRIGNUM; rigno++) {
				m_customrig = srcbone->GetCustomRig(rigno);
				if (m_customrig.useflag == 2) {
					int isvalid = IsValidCustomRig(m_model, m_customrig, srcbone);
					if (isvalid) {
						WriteRig(srcbone);
					}
				}
			}
		}
	}



	CallF( Write2File( "</RIGFILE>\r\n" ), return 1 );

	return 0;
}
void CRigFile::WriteRigReq( CBone* srcbone )
{
	if (!srcbone){
		return;
	}

	if (srcbone->IsSkeleton()) {
		int rigno;
		for (rigno = 0; rigno < MAXRIGNUM; rigno++) {
			m_customrig = srcbone->GetCustomRig(rigno);
			if (m_customrig.useflag == 2) {
				int isvalid = IsValidCustomRig(m_model, m_customrig, srcbone);
				if (isvalid) {
					WriteRig(srcbone);
				}
			}
		}
	}

	if( srcbone->GetChild(false) ){
		WriteRigReq(srcbone->GetChild(false));
	}
	if( srcbone->GetBrother(false) ){
		WriteRigReq(srcbone->GetBrother(false));
	}
}


int CRigFile::WriteRig(CBone* srcbone)
{
/***
  <Bone>
    <RigName>RigName</RigName>
	<OwnerBone>koshi</OwnerBone>
	<RigNumber>0</RigNumber>
	<ElemNum>3</ElemNum>
    <RigElem>
      <RigRigName>Lte</RigRigName>
      <RigRigNo>0</RigRigNo>
	  <BoneName>koshi</BoneName>
	  <AxisU>X</AxisU>
	  <RateU>1.0000</RateU>
	  <EnableU>1</EnableU>
	  <AxisV>X</AxisV>
	  <RateV>1.0000</RateV>
	  <EnableV>1</EnableV>
	</RigElem>
	<RigElem>
	  <RigRigName>Rte</RigRigName>
	  <RigRigNo>0</RigRigNo>
	  <BoneName>mune</BoneName>
	  <AxisU>X</AxisU>
	  <RateU>1.0000</RateU>
	  <EnableU>1</EnableU>
	  <AxisV>X</AxisV>
	  <RateV>1.0000</RateV>
	  <EnableV>1</EnableV>
	</RigElem>
  </Bone>
***/

	CallF(Write2File("  <Bone>\r\n" ), return 1);

	char mrigname[256];
	WideCharToMultiByte(CP_ACP, 0, m_customrig.rigname, -1, mrigname, 256, NULL, NULL);
	CallF(Write2File("    <RigName>%s</RigName>\r\n", mrigname ), return 1);

	CBone* ownerbone = m_model->GetBoneByID(m_customrig.rigboneno);
	if (!ownerbone || (ownerbone != srcbone)){
		_ASSERT(0);
		return 1;
	}
	CallF(Write2File("    <OwnerBone>%s</OwnerBone>\r\n", ownerbone->GetBoneName()), return 1);

	CallF(Write2File("    <RigNumber>%d</RigNumber>\r\n", m_customrig.rigno), return 1);

	CallF(Write2File("    <ElemNum>%d</ElemNum>\r\n", m_customrig.elemnum), return 1);

	CallF(Write2File("    <DispAxis>%d</DispAxis>\r\n", m_customrig.dispaxis), return 1);

	CallF(Write2File("    <DispOrder>%d</DispOrder>\r\n", m_customrig.disporder), return 1);

	CallF(Write2File("    <DispPosInverse>%d</DispPosInverse>\r\n", (int)(m_customrig.posinverse)), return 1);

	CallF(Write2File("    <ShapeMult>%d</ShapeMult>\r\n", m_customrig.shapemult), return 1);

	CallF(Write2File("    <ShapeKind>%d</ShapeKind>\r\n", m_customrig.shapekind), return 1);

	CallF(Write2File("    <RigColor>%d</RigColor>\r\n", m_customrig.rigcolor), return 1);

	int elemno;
	for (elemno = 0; elemno < m_customrig.elemnum; elemno++){
		RIGELEM currigelem = m_customrig.rigelem[elemno];

		CallF(Write2File("    <RigElem>\r\n"), return 1);


		char mrigrigname[256] = "!noname!";
		if (currigelem.rigrigboneno >= 0){
			CBone* rigrigbone = m_model->GetBoneByID(currigelem.rigrigboneno);
			if (rigrigbone){
				strcpy_s(mrigrigname, 256, rigrigbone->GetBoneName());
			}
		}
		CallF(Write2File("    <RigRigName>%s</RigRigName>\r\n", mrigrigname), return 1);		
		CallF(Write2File("    <RigRigNo>%d</RigRigNo>\r\n", currigelem.rigrigno), return 1);


		////for debug
		//int cmpfront = strcmp(mrigname, "Front");
		//if (cmpfront == 0) {
		//	_ASSERT(0);
		//}
		//int cmpback = strcmp(mrigname, "Back");
		//if (cmpback == 0) {
		//	_ASSERT(0);
		//}


		CBone* rigelembone = m_model->GetBoneByID(currigelem.boneno);
		if (!rigelembone){
			_ASSERT(0);
			return 1;
		}
		CallF(Write2File("      <BoneName>%s</BoneName>\r\n", rigelembone->GetBoneName()), return 1);

		char straxis[RIGAXIS_MAX][20] = {
			"CurrentX", "CurrentY", "CurrentZ",
			"ParentX", "ParentY", "ParentZ",
			"GlobalX", "GlobalY", "GlobalZ",
			"NodeX", "NodeY", "NodeZ"
		};
		int axiskind;

		axiskind = currigelem.transuv[0].axiskind;
		if ((axiskind < 0) || (axiskind >= RIGAXIS_MAX)){
			_ASSERT(0);
			return 1;
		}
		CallF(Write2File("      <AxisU>%s</AxisU>\r\n", straxis[axiskind]), return 1);
		CallF(Write2File("      <RateU>%f</RateU>\r\n", currigelem.transuv[0].applyrate), return 1);
		CallF(Write2File("      <EnableU>%d</EnableU>\r\n", currigelem.transuv[0].enable), return 1);

		axiskind = currigelem.transuv[1].axiskind;
		if ((axiskind < 0) || (axiskind >= RIGAXIS_MAX)) {
			_ASSERT(0);
			return 1;
		}
		CallF(Write2File("      <AxisV>%s</AxisV>\r\n", straxis[axiskind]), return 1);
		CallF(Write2File("      <RateV>%f</RateV>\r\n", currigelem.transuv[1].applyrate), return 1);
		CallF(Write2File("      <EnableV>%d</EnableV>\r\n", currigelem.transuv[1].enable), return 1);

		CallF(Write2File("    </RigElem>\r\n"), return 1);
	}

	CallF(Write2File("  </Bone>\r\n"), return 1);

	return 0;
}




int CRigFile::LoadRigFile( WCHAR* strpath, CModel* srcmodel )
{
	m_model = srcmodel;
	m_mode = XMLIO_LOAD;

	m_hfile = CreateFile( strpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		//_ASSERT( 0 );
		return 1;
	}	

	CallF( SetBuffer(), return 1 );

	CBone* topbone = srcmodel->GetTopBone();
	if( !topbone ){
		return 0;
	}


	int findflag = 1;
	while( findflag ){
		XMLIOBUF bonebuf;
		ZeroMemory( &bonebuf, sizeof( XMLIOBUF ) );
		int ret;
		ret = SetXmlIOBuf( &m_xmliobuf, "<Bone>", "</Bone>", &bonebuf );
		if( ret == 0 ){
			CallF( ReadBone( &bonebuf ), return 1 );
		}else{
			findflag = 0;
		}
	}

	return 0;
}

int CRigFile::ReadBone(XMLIOBUF* xmliobuf)
{
	/***
	<Bone>
  	  <RigName>RigName</RigName>
	  <OwnerBone>koshi</OwnerBone>
	  <RigNumber>0</RigNumber>
	  <ElemNum>3</ElemNum>
	  <DispAxis>0</DispAxis>
	  <DispOrder>0</DispOrder>
	  <RigElem>
        <RigRigName>Lte</RigRigName>
	    <RigRigNo>0</RigRigNo>
	    <BoneName>koshi</BoneName>
	    <AxisU>X</AxisU>
	    <RateU>1.0000</RateU>
	    <AxisV>X</AxisV>
	    <RateV>1.0000</RateV>
	  </RigElem>
	  <RigElem>
	    <RigRigName>Rte</RigRigName>
	    <RigRigNo>0</RigRigNo>
	    <BoneName>mune</BoneName>
	    <AxisU>X</AxisU>
	    <RateU>1.0000</RateU>
	    <AxisV>X</AxisV>
	    <RateV>1.0000</RateV>
	  </RigElem>
	</Bone>
	***/

	ZeroMemory(&m_customrig, sizeof(CUSTOMRIG));

	char rigname[256] = { 0 };
	CallF(Read_Str(xmliobuf, "<RigName>", "</RigName>", rigname, 256), return 1);
	WCHAR wrigname[256];
	ZeroMemory(wrigname, sizeof(WCHAR) * 256);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, rigname, 256, wrigname, 256);
	wcscpy_s(m_customrig.rigname, 256, wrigname);

	char bonename[256] = { 0 };
	CallF(Read_Str(xmliobuf, "<OwnerBone>", "</OwnerBone>", bonename, 256), return 1);
	CBone* ownerbone = FindBoneByName(m_model, bonename, 256);
	if (!ownerbone){
		_ASSERT(0);
		return 1;
	}
	m_customrig.rigboneno = ownerbone->GetBoneNo();

	int rigno;
	CallF(Read_Int(xmliobuf, "<RigNumber>", "</RigNumber>", &rigno), return 1);
	m_customrig.rigno = rigno;

	int elemnum;
	CallF(Read_Int(xmliobuf, "<ElemNum>", "</ElemNum>", &elemnum), return 1);
	m_customrig.elemnum = elemnum;

	int dispaxis;
	int resultdispaxis = Read_Int(xmliobuf, "<DispAxis>", "</DispAxis>", &dispaxis);
	if (resultdispaxis == 0) {
		m_customrig.dispaxis = dispaxis;
	}
	else {
		m_customrig.dispaxis = 0;
	}

	int disporder;
	int resultdisporder = Read_Int(xmliobuf, "<DispOrder>", "</DispOrder>", &disporder);
	if ((resultdisporder == 0) && 
		(disporder >= 0) && (disporder <= RIGPOSINDEXMAX)) {
		m_customrig.disporder = disporder;
	}
	else {
		m_customrig.disporder = 0;
	}

	int posinverse;
	int resultposinverse = Read_Int(xmliobuf, "<DispPosInverse>", "</DispPosInverse>", &posinverse);
	if (resultposinverse == 0) {
		m_customrig.posinverse = (bool)(posinverse != 0);
	}
	else {
		m_customrig.posinverse = false;
	}

	//FileInfo 1001-03で　新規
	int shapemult;
	int resultshapemult = Read_Int(xmliobuf, "<ShapeMult>", "</ShapeMult>", &shapemult);
	if ((resultshapemult == 0) && 
		(shapemult >= 0) && (shapemult <= RIGMULTINDEXMAX)) {
		m_customrig.shapemult = shapemult;
	}
	else {
		m_customrig.shapemult = 0;
	}

	int shapekind;
	int resultshapekind = Read_Int(xmliobuf, "<ShapeKind>", "</ShapeKind>", &shapekind);
	if (resultshapekind == 0) {
		m_customrig.shapekind = shapekind;
	}
	else {
		m_customrig.shapekind = 0;
	}

	int rigcolor;
	int resultrigcolor = Read_Int(xmliobuf, "<RigColor>", "</RigColor>", &rigcolor);
	if (resultrigcolor == 0) {
		m_customrig.rigcolor = rigcolor;
	}
	else {
		m_customrig.rigcolor = 0;
	}

	m_customrig.useflag = 2;

	int readnum = 0;
	int findflag = 1;
	while( findflag ){
		XMLIOBUF rebuf;
		ZeroMemory( &rebuf, sizeof( XMLIOBUF ) );
		int ret;
		ret = SetXmlIOBuf( xmliobuf, "<RigElem>", "</RigElem>", &rebuf );
		if( ret == 0 ){
			CallF(ReadRig(&rebuf, readnum), return 1);
			readnum++;
		}else{
			findflag = 0;
		}
	}

	if (readnum == m_customrig.elemnum){
		if (readnum < MAXRIGELEMNUM){
			int elemno;
			for (elemno = readnum; elemno < MAXRIGELEMNUM; elemno++){
				m_customrig.rigelem[elemno].boneno = -1;
			}
		}

		int isvalid = IsValidCustomRig(m_model, m_customrig, ownerbone);
		if (isvalid == 1){
			ownerbone->SetCustomRig(m_customrig);
		}
		else{
			_ASSERT(0);
			return 1;
		}
	}
	else{
		_ASSERT(0);
		return 1;
	}

	return 0;
}

int CRigFile::ReadRig(XMLIOBUF* xmlbuf, int elemno)
{
	/*
	<RigElem>
	  <RigRigName>Rte</RigRigName>
	  <RigRigNo>0</RigRigNo>
	  <BoneName>koshi</BoneName>
	  <AxisU>X</AxisU>
	  <RateU>1.0000</RateU>
	  <EnableU>1</EnableU>
	  <AxisV>X</AxisV>
	  <RateV>1.0000</RateV>
	  <EnableV>1</EnableV>
	</RigElem>
	*/

	if ((elemno < 0) || (elemno > MAXRIGELEMNUM)){
		_ASSERT(0);
		return 1;
	}
	RIGELEM* dstrigelem = &(m_customrig.rigelem[elemno]);

	int ret, ret2;

	char rigrigname[256] = { 0 };
	ret = Read_Str(xmlbuf, "<RigRigName>", "</RigRigName>", rigrigname, 256);
	if (ret == 0){
		int cmp0 = strcmp("!noname!", rigrigname);
		if (cmp0 == 0){
			dstrigelem->rigrigboneno = -1;
			dstrigelem->rigrigno = -1;
		}
		else {
			CBone* rigrigbone = m_model->FindBoneByName(rigrigname);//_Joint有無対応
			if (rigrigbone){
				dstrigelem->rigrigboneno = rigrigbone->GetBoneNo();

				int rigrigno = -1;
				ret2 = Read_Int(xmlbuf, "<RigRigNo>", "</RigRigNo>", &rigrigno);
				if (ret2 == 0){
					if ((rigrigno >= 0) && (rigrigno < MAXRIGNUM)){
						dstrigelem->rigrigno = rigrigno;
					}
					else{
						dstrigelem->rigrigno = -1;
					}
				}
				else{
					dstrigelem->rigrigno = -1;
				}
			}
			else{
				dstrigelem->rigrigboneno = -1;
				dstrigelem->rigrigno = -1;
			}
		}
	}
	else{
		dstrigelem->rigrigboneno = -1;
		dstrigelem->rigrigno = -1;
	}

	char bonename[256] = { 0 };
	ZeroMemory(bonename, sizeof(char) * 256);
	CallF(Read_Str(xmlbuf, "<BoneName>", "</BoneName>", bonename, 256), return 1);
	CBone* elembone = m_model->FindBoneByName(bonename);//_Joint有無対応
	if (!elembone) {
		_ASSERT(0);
		return 0;
	}
	dstrigelem->boneno = elembone->GetBoneNo();


	char str_startaxis[2][10] = { "<AxisU>", "<AxisV>" };
	char str_endaxis[2][10] = { "</AxisU>", "</AxisV>" };
	char str_startrate[2][10] = { "<RateU>", "<RateV>" };
	char str_endrate[2][10] = { "</RateU>", "</RateV>" };
	char str_startenable[2][15] = { "<EnableU>", "<EnableV>" };
	char str_endenable[2][15] = { "</EnableU>", "</EnableV>" };

	int uvno;
	for (uvno = 0; uvno < 2; uvno++){
		char straxis[256] = { 0 };
		CallF(Read_Str(xmlbuf, str_startaxis[uvno], str_endaxis[uvno], straxis, 256), return 1);
		int cmpx, cmpy, cmpz;
		int cmpxCurrent, cmpyCurrent, cmpzCurrent;
		int cmpxParent, cmpyParent, cmpzParent;
		int cmpxGlobal, cmpyGlobal, cmpzGlobal;
		int cmpxNode, cmpyNode, cmpzNode;
		cmpx = strncmp(straxis, "X", 1);//旧ファイル(fileinfo 1001-*)対応
		cmpy = strncmp(straxis, "Y", 1);//旧ファイル(fileinfo 1001-*)対応
		cmpz = strncmp(straxis, "Z", 1);//旧ファイル(fileinfo 1001-*)対応
		cmpxCurrent = strncmp(straxis, "CurrentX", 8);
		cmpyCurrent = strncmp(straxis, "CurrentY", 8);
		cmpzCurrent = strncmp(straxis, "CurrentZ", 8);
		cmpxParent = strncmp(straxis, "ParentX", 7);
		cmpyParent = strncmp(straxis, "ParentY", 7);
		cmpzParent = strncmp(straxis, "ParentZ", 7);
		cmpxGlobal = strncmp(straxis, "GlobalX", 7);
		cmpyGlobal = strncmp(straxis, "GlobalY", 7);
		cmpzGlobal = strncmp(straxis, "GlobalZ", 7);
		cmpxNode = strncmp(straxis, "NodeX", 5);
		cmpyNode = strncmp(straxis, "NodeY", 5);
		cmpzNode = strncmp(straxis, "NodeZ", 5);
		if (cmpx == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_CURRENT_X;
		}
		else if (cmpy == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_CURRENT_Y;
		}
		else if (cmpz == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_CURRENT_Z;
		}
		else if (cmpxCurrent == 0){
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_CURRENT_X;
		}
		else if (cmpyCurrent == 0){
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_CURRENT_Y;
		}
		else if (cmpzCurrent == 0){
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_CURRENT_Z;
		}
		else if (cmpxParent == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_PARENT_X;
		}
		else if (cmpyParent == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_PARENT_Y;
		}
		else if (cmpzParent == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_PARENT_Z;
		}
		else if (cmpxGlobal == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_GLOBAL_X;
		}
		else if (cmpyGlobal == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_GLOBAL_Y;
		}
		else if (cmpzGlobal == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_GLOBAL_Z;
		}
		else if (cmpxNode == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_NODE_X;
		}
		else if (cmpyNode == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_NODE_Y;
		}
		else if (cmpzNode == 0) {
			dstrigelem->transuv[uvno].axiskind = RIGAXIS_NODE_Z;
		}
		else {
			_ASSERT(0);
		}
		float rate;
		CallF(Read_Float(xmlbuf, str_startrate[uvno], str_endrate[uvno], &rate), return 1);
		dstrigelem->transuv[uvno].applyrate = rate;
		int ret3;
		int enable;
		ret3 = Read_Int(xmlbuf, str_startenable[uvno], str_endenable[uvno], &enable);
		if (ret3 != 0){
			enable = 1;
		}
		else{
			if ((enable != 0) && (enable != 1)){
				enable = 0;
			}
		}
		dstrigelem->transuv[uvno].enable = enable;
	}


	return 0;
}


/***
int CRigFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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

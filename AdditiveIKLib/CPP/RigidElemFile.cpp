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

#include <GlobalVar.h>

#include <RigidElemFile.h>

#include <Model.h>
#include <mqoobject.h>
#include <Bone.h>
#include <RigidElem.h>

#define DBGH
#include <dbg.h>
#include <map>
#include <vector>

#include <shlwapi.h>

/*
extern float g_initcuslk;
extern float g_initcusak;
extern float g_l_dmp;
extern float g_a_dmp;
*/
using namespace std;

CRigidElemFile::CRigidElemFile()
{
	InitParams();
}

CRigidElemFile::~CRigidElemFile()
{
	DestroyObjs();
}

int CRigidElemFile::InitParams()
{
	CXMLIO::InitParams();

	m_model = 0;
	m_btgscale = 9.8f;
	return 0;
}

int CRigidElemFile::DestroyObjs()
{
	CXMLIO::DestroyObjs();

	InitParams();

	return 0;
}


int CRigidElemFile::WriteRigidElemFile( WCHAR* strpath, CModel* srcmodel, int reindex )
{
	if (!srcmodel){
		_ASSERT(0);
		return 1;
	}

	m_model = srcmodel;
	m_mode = XMLIO_WRITE;
	m_btgscale = srcmodel->GetBtGScale(reindex);

	if( !m_model->GetTopBone() ){
		return 0;
	}

	m_hfile = CreateFile( strpath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( L"RigidElemFile : WriteRigidElemFile : file open error !!!\n" );
		//_ASSERT( 0 );
		return 1;
	}


	CallF( Write2File( "<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<RIGIDELEM>\r\n" ), return 1 );  
	//CallF( Write2File( "    <FileInfo>1001-01</FileInfo>\r\n" ), return 1 );
	CallF(Write2File("    <FileInfo>1002-01</FileInfo>\r\n"), return 1);//2024/05/05 GROUP-->GROUPIDS

	CallF( Write2File( "    <SCBTG>%f</SCBTG>\r\n", m_btgscale ), return 1 );

	//WriteREReq( m_model->GetTopBone(false) );
	std::map<int, CBone*>::iterator itrbone;
	for (itrbone = m_model->GetBoneListBegin(); itrbone != m_model->GetBoneListEnd(); itrbone++) {
		CBone* srcbone = itrbone->second;
		if (srcbone && srcbone->IsSkeleton()) {
			WriteRE(srcbone);
		}
	}



	CallF( Write2File( "</RIGIDELEM>\r\n" ), return 1 );

	FlushFileBuffers(m_hfile);
	SetEndOfFile(m_hfile);

	return 0;
}
void CRigidElemFile::WriteREReq( CBone* srcbone )
{
	if (srcbone->IsSkeleton()) {
		WriteRE(srcbone);
	}

	if( srcbone->GetChild(false) ){
		WriteREReq( srcbone->GetChild(false) );
	}
	if( srcbone->GetBrother(false) ){
		WriteREReq( srcbone->GetBrother(false) );
	}
}


int CRigidElemFile::WriteRE( CBone* srcbone )
{
/***
  <Bone>
    <Name>zentai_ido</Nama>
    <RigidElem>
      <ChildName>koshi</ChildName>
	  <SkipFlag>0</SkipFlag>
	  <BCone>0</BCone>
	  <ShpRate>0.2</ShpRate>
	  <Mitusdo>5.0</Mitusdo>
	  <FORBIDROT>0</FORBIDROT>
    </RigidElem>
    <RigidElem>
	  <ChildName>mune</ChildName>
	  <SkipFlag>0</SkipFlag>
	  <BCone>0</BCone>
	  <ShpRate>0.2</ShpRate>
	  <Mitsudo>5.0</Mitsudo>
	  <FORBIDROT>1</FORBIDROT>
	  </RigidElem>
  </Bone>
***/
	if( srcbone->GetRigidElemSize() <= 0 ){
		return 0;
	}

	CallF( Write2File( "  <Bone>\r\n" ), return 1);
	CallF( Write2File( "    <Name>%s</Name>\r\n", srcbone->GetBoneName() ), return 1);
	CallF( Write2File( "    <BTKIN>%d</BTKIN>\r\n", srcbone->GetBtForce() ), return 1);

	//map<CBone*,CRigidElem*>::iterator itrre;
	//for( itrre = srcbone->GetRigidElemMapBegin(); itrre != srcbone->GetRigidElemMapEnd(); itrre++ ){
	//	CRigidElem* curre = itrre->second;
	CBone* childbone = srcbone->GetChild(false);
	while (childbone){
		if (childbone->IsSkeleton()) {
			CRigidElem* curre = srcbone->GetRigidElem(childbone);

			if (curre) {
				CallF(Write2File("    <RigidElem>\r\n"), return 1);

				CallF(Write2File("      <ChildName>%s</ChildName>\r\n", childbone->GetBoneName()), return 1);
				CallF(Write2File("      <ColType>%d</ColType>\r\n", curre->GetColtype()), return 1);
				CallF(Write2File("      <SkipFlag>%d</SkipFlag>\r\n", curre->GetSkipflag()), return 1);
				CallF(Write2File("      <ShpRate>%f</ShpRate>\r\n", curre->GetSphrate()), return 1);
				CallF(Write2File("      <BoxzRate>%f</BoxzRate>\r\n", curre->GetBoxzrate()), return 1);
				CallF(Write2File("      <LK>%d</LK>\r\n", curre->GetLKindex()), return 1);
				CallF(Write2File("      <CUSLK>%f</CUSLK>\r\n", curre->GetCusLk()), return 1);
				CallF(Write2File("      <AK>%d</AK>\r\n", curre->GetAKindex()), return 1);
				CallF(Write2File("      <CUSAK>%f</CUSAK>\r\n", curre->GetCusAk()), return 1);
				CallF(Write2File("      <Mass>%f</Mass>\r\n", curre->GetMass()), return 1);
				CallF(Write2File("      <LDMP>%f</LDMP>\r\n", curre->GetLDamping()), return 1);
				CallF(Write2File("      <ADMP>%f</ADMP>\r\n", curre->GetADamping()), return 1);
				CallF(Write2File("      <BTG>%f</BTG>\r\n", curre->GetBtg()), return 1);
				CallF(Write2File("      <DMPANIML>%f</DMPANIML>\r\n", curre->GetDampanimL()), return 1);
				CallF(Write2File("      <DMPANIMA>%f</DMPANIMA>\r\n", curre->GetDampanimA()), return 1);


				//CallF(Write2File("      <GROUP>%d</GROUP>\r\n", curre->GetGroupid()), return 1);
				//CallF(Write2File("      <MYSELF>%d</MYSELF>\r\n", curre->GetMyselfflag()), return 1);

				vector<int> groupids;
				curre->GroupIdVec(groupids);
				int gidnum = (int)groupids.size();
				CallF(Write2File("      <GROUPIDNUM>%d</GROUPIDNUM>\r\n", gidnum), return 1);
				int gino;
				for (gino = 0; gino < gidnum; gino++) {
					CallF(Write2File("      <GROUPID>%d</GROUPID>\r\n", groupids[gino]), return 1);
				}


				int idnum = curre->GetColiidsSize();
				CallF(Write2File("      <COLIIDNUM>%d</COLIIDNUM>\r\n", idnum), return 1);
				int ino;
				for (ino = 0; ino < idnum; ino++) {
					CallF(Write2File("      <COLIID>%d</COLIID>\r\n", curre->GetColiids(ino)), return 1);
				}

				CallF(Write2File("      <RESTITUTION>%f</RESTITUTION>\r\n", curre->GetRestitution()), return 1);
				CallF(Write2File("      <FRICTION>%f</FRICTION>\r\n", curre->GetFriction()), return 1);

				CallF(Write2File("      <FORBIDROT>%d</FORBIDROT>\r\n", curre->GetForbidRotFlag()), return 1);

				CallF(Write2File("    </RigidElem>\r\n"), return 1);
			}
		}
		childbone = childbone->GetBrother(false);
	}

	CallF( Write2File( "  </Bone>\r\n" ), return 1);

	return 0;
}




int CRigidElemFile::LoadRigidElemFile( WCHAR* strpath, CModel* srcmodel )
{
	m_model = srcmodel;
	m_mode = XMLIO_LOAD;

	WCHAR wfilename[MAX_PATH] = {0L};
	WCHAR* lasten;
	lasten = wcsrchr( strpath, TEXT('\\') );
	if( !lasten ){
		_ASSERT( 0 );
		return 1;
	}
	wcscpy_s( wfilename, MAX_PATH, lasten + 1 );
	char mfilename[MAX_PATH];
	ZeroMemory( mfilename, sizeof( char ) * MAX_PATH );
	WideCharToMultiByte( CP_ACP, 0, wfilename, -1, mfilename, MAX_PATH, NULL, NULL );


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


	REINFO reinfo;
	reinfo.Init();
	reinfo.SetFileName(mfilename);


	m_rename = reinfo.filename;
	CBone* topbone = srcmodel->GetTopBone(false);
	if( topbone ){
		srcmodel->CreateRigidElem(m_rename.c_str(), 1, srcmodel->GetDefaultImpName(), 0);
		//CreateBtObjectReq(NULL, startbone, startbone->GetChild());
	}



	int posstep = 0;
	//float scbtg = 9.8f;
	int getscbtg = 1;
	getscbtg = Read_Float(&m_xmliobuf, "<SCBTG>", "</SCBTG>", &m_btgscale);
	if( getscbtg == 0 ){
		reinfo.btgscale = m_btgscale;
	}else{
		reinfo.btgscale = 9.8f;
	}

	int findflag = 1;
	while (findflag) {
		XMLIOBUF bonebuf;
		ZeroMemory(&bonebuf, sizeof(XMLIOBUF));
		int ret;
		ret = SetXmlIOBuf(&m_xmliobuf, "<Bone>", "</Bone>", &bonebuf);
		if (ret == 0) {
			CallF(ReadBone(&bonebuf), return 1);
		}
		else {
			findflag = 0;
		}
	}

	int newindex = srcmodel->CalcNewRigidElemInfoIndexByName(reinfo.filename);
	if (newindex >= 0) {
		srcmodel->SetRigidElemInfo(newindex, reinfo);
		srcmodel->SetCurrentRigidElem(newindex);

		srcmodel->SetBtGScale(m_btgscale, newindex);//CModel::m_rigideleminfoへのaddが済んでから。
		srcmodel->SetBtObjectVec();//2024/06/16
	}

	//_ASSERT(0);

	return 0;
}

int CRigidElemFile::ReadBone( XMLIOBUF* xmliobuf )
{

	char bonename[256];//ファイルの記述
	ZeroMemory( bonename, sizeof( char ) * 256 );
	CallF( Read_Str( xmliobuf, "<Name>", "</Name>", bonename, 256 ), return 1 );
	CBone* curbone = m_model->FindBoneByName(bonename);//_Joint有無対応
	if (!curbone){
		//_ASSERT(0);
		return 0;
	}


	int getkin = 0;
	int kin = 0;
	getkin = Read_Int( xmliobuf,  "<BTKIN>", "</BTKIN>", &kin );


	if( curbone ){
		if( getkin == 0 ){
			curbone->SetBtForce( kin );
		}else{
			_ASSERT( 0 );
			curbone->SetBtForce( 0 );
		}
	}
	else{
		_ASSERT(0);
	}

	int findflag = 1;
	while( findflag ){
		XMLIOBUF rebuf;
		ZeroMemory( &rebuf, sizeof( XMLIOBUF ) );
		int ret;
		ret = SetXmlIOBuf( xmliobuf, "<RigidElem>", "</RigidElem>", &rebuf );
		if( ret == 0 ){
			CallF( ReadRE( &rebuf, curbone ), return 1 );
		}else{
			findflag = 0;
		}
	}

	return 0;
}

int CRigidElemFile::ReadRE( XMLIOBUF* xmlbuf, CBone* curbone )
{

	char childname[256];
	ZeroMemory( childname, sizeof( char ) * 256 );
	CallF( Read_Str( xmlbuf, "<ChildName>", "</ChildName>", childname, 256 ), return 1 );
	CBone* childbone = m_model->FindBoneByName(childname);//_Joint有無対応
	if (!childbone){
		//_ASSERT(0);
		return 0;
	}


	int coltype = 0;
	CallF( Read_Int( xmlbuf, "<ColType>", "</ColType>", &coltype ), return 1 );

	int skipflag = 0;
	CallF( Read_Int( xmlbuf, "<SkipFlag>", "</SkipFlag>", &skipflag ), return 1 );

	float rate = 0.0f;
	CallF( Read_Float( xmlbuf, "<ShpRate>", "</ShpRate>", &rate ), return 1 );


	//1.2.0.16で追加　１０年前からあったパラメータ
	float boxzrate = 0.0f;
	int result1 = Read_Float(xmlbuf, "<BoxzRate>", "</BoxzRate>", &boxzrate);
	if (result1 != 0) {
		//無くてもエラーにしないで　デフォルト値をセット
		boxzrate = 0.60f;//default value
	}

	int lkindex = 0;
	int retki = Read_Int( xmlbuf, "<LK>", "</LK>", &lkindex );
	if( retki ){
		lkindex = 2;
	}
	float cuslk = 0.0f;
	int retcuslk = Read_Float( xmlbuf, "<CUSLK>", "</CUSLK>", &cuslk );
	if( retcuslk ){
		cuslk = g_initcuslk;
	}


	int akindex = 0;
	int retaki = Read_Int( xmlbuf, "<AK>", "</AK>", &akindex );
	if( retaki ){
		akindex = 2;
	}
	float cusak = 0.0f;
	int retcusak = Read_Float( xmlbuf, "<CUSAK>", "</CUSAK>", &cusak );
	if( retcusak ){
		cusak = g_initcusak;
	}

	float mass = 0.0f;
	int retm = Read_Float( xmlbuf, "<Mass>", "</Mass>", &mass );
	if( retm ){
		mass = 1.0f;
	}

	float ldmp = 0.0f;
	int retld = Read_Float( xmlbuf, "<LDMP>", "</LDMP>", &ldmp );
	if( retld ){
		ldmp = g_l_dmp;
	}
	float admp = 0.0f;
	int retad = Read_Float( xmlbuf, "<ADMP>", "</ADMP>", &admp );
	if( retad ){
		admp = g_a_dmp;
	}

	float btg = 0.0f;
	int getbtg = Read_Float( xmlbuf, "<BTG>", "</BTG>", &btg );
	if( getbtg ){
		btg = -0.09f;
	}

	float dmpanimL = 0.0f;
	int getdal = Read_Float( xmlbuf, "<DMPANIML>", "</DMPANIML>", &dmpanimL );
	if( getdal ){
		dmpanimL = 0.0f;
	}
	float dmpanimA = 0.0f;
	int getdaa = Read_Float( xmlbuf, "<DMPANIMA>", "</DMPANIMA>", &dmpanimA );
	if( getdaa ){
		dmpanimA = 0.0f;
	}


	int gid = 0;
	int retgid = Read_Int( xmlbuf, "<GROUP>", "</GROUP>", &gid );
	if( retgid ){
		//gid = 2;
		//vector<int> groupids;
		//curre->GroupIdVec(groupids);
		//int gidnum = groupids.size();
		//CallF(Write2File("      <GROUPIDNUM>%d</GROUPIDNUM>\r\n", gidnum), return 1);
		//int gino;
		//for (gino = 0; gino < gidnum; gino++) {
		//	CallF(Write2File("      <GROUPID>%d</GROUPID>\r\n", groupids[gino]), return 1);
		//}
		int idnum = 0;
		vector<int> tmpids;
		tmpids.clear();
		int retidnum = Read_Int(xmlbuf, "<GROUPIDNUM>", "</GROUPIDNUM>", &idnum);
		if (retidnum) {
			gid = 1;
			idnum = 1;
			tmpids.push_back(1);//!!!!!!!! defaultで地面と当たる
		}
		else {	
			int ino;
			for (ino = 0; ino < idnum; ino++) {
				XMLIOBUF idbuf;
				ZeroMemory(&idbuf, sizeof(XMLIOBUF));
				int ret;
				ret = SetXmlIOBuf(xmlbuf, "<GROUPID>", "</GROUPID>", &idbuf, 0);
				if (ret == 0) {
					int id = 0;
					int retid = Read_Int(&idbuf, "<GROUPID>", "</GROUPID>", &id);
					if (retid) {
						_ASSERT(0);
						return 1;
					}
					tmpids.push_back(id);

				}
				else {
					_ASSERT(0);
					break;
				}
			}
		}

		
		int ino;
		for (ino = 0; ino < idnum; ino++) {
			if ((tmpids[ino] - 1) >= 0) {
				int curid = 1 << (tmpids[ino] - 1);
				gid |= curid;
			}
		}


	}


	int myself = 0;
	int retmyself = Read_Int( xmlbuf, "<MYSELF>", "</MYSELF>", &myself );
	//if( retmyself ){
	//	myself = 1;
	//}


	int idnum = 0;
	int retidnum = Read_Int( xmlbuf, "<COLIIDNUM>", "</COLIIDNUM>", &idnum );
	if( retidnum ){
		idnum = 0;
	}

	vector<int> tmpids;
	tmpids.clear();

	int ino;
	for( ino = 0; ino < idnum; ino++ ){
		XMLIOBUF idbuf;
		ZeroMemory( &idbuf, sizeof( XMLIOBUF ) );
		int ret;
		ret = SetXmlIOBuf( xmlbuf, "<COLIID>", "</COLIID>", &idbuf, 0 );
		if( ret == 0 ){
			int id = 0;
			int retid = Read_Int( &idbuf, "<COLIID>", "</COLIID>", &id );
			if( retid ){
				_ASSERT( 0 );
				return 1;
			}
			tmpids.push_back( id );

		}else{
			_ASSERT( 0 );
			break;
		}
	}
	if( retidnum ){//!!!!!!!! defaultで地面と当たる
		idnum = 1;
		tmpids.push_back( 1 );
	}


	//2024/05/05
	//間違ったGUIでうまくいっていた場合(ビットシフト数とシフト後の値が同じ場合)を推測して　限定的だが互換をとる
	if ((retmyself == 0) && (myself == 1) && ((gid == 1) || (gid == 2))) {
		bool foundgid = false;
		int idindex;
		for (idindex = 0; idindex < (int)tmpids.size(); idindex++) {
			int currentid = tmpids[idindex];
			if (currentid == gid) {
				foundgid = true;
				break;
			}
		}

		if (foundgid == false) {
			tmpids.push_back(gid);
			idnum = (int)tmpids.size();
		}
	}



	float rest = 0.0f;
	int retrest = Read_Float( xmlbuf, "<RESTITUTION>", "</RESTITUTION>", &rest );
	if( retrest ){
		rest = 0.5f;
	}
	float fric = 0.0f;
	int retfric = Read_Float( xmlbuf, "<FRICTION>", "</FRICTION>", &fric );
	if( retfric ){
		fric = 0.5f;
	}

	int forbidrot = 0;
	int retforbidrot = Read_Int(xmlbuf, "<FORBIDROT>", "</FORBIDROT>", &forbidrot);
	if (retforbidrot || ((forbidrot != 0) && (forbidrot != 1))){
		forbidrot = 0;
	}



	if( curbone ){
		if( childbone ){			
			CRigidElem* curre;
			curre = curbone->GetRigidElemOfMap( m_rename, childbone );
			if( curre ){
				curre->SetSkipflag(skipflag);
				curre->SetSphrate(rate);
				curre->SetBoxzrate(boxzrate);
				curre->SetColtype(coltype);
				curre->SetLKindex(lkindex);
				curre->SetAKindex(akindex);
				curre->SetCusLk(cuslk);
				curre->SetCusAk(cusak);
				curre->SetMass(mass);
				curre->SetLDamping(ldmp);
				curre->SetADamping(admp);
				curre->SetBtg(btg);

				curre->SetGroupid(gid);
				curre->CopyColiids(tmpids);
				//curre->SetMyselfflag(myself);

				curre->SetRestitution(rest);
				curre->SetFriction(fric);

				curre->SetForbidRotFlag(forbidrot);

				curre->SetDampanimL(dmpanimL);
				curre->SetDampanimA(dmpanimA);
			}
			else{
				if (childbone->IsHipsBone() == false) {//hipsのときには正常
					_ASSERT(0);
				}
			}
		}
		else{
			_ASSERT(0);
		}
	}
	return 0;
}


/***
int CRigidElemFile::CheckFileVersion( XMLIOBUF* xmlbuf )
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

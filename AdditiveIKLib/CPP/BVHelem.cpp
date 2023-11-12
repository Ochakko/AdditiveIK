#include "stdafx.h"
//#include <stdafx.h> //ダミー


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <windows.h>

#include <bvhelem.h>
#include <crtdbg.h>

//#include <quaternion.h>
//#include <BoneProp.h>
//#include <D3DX9.h>
#include <ChaVecCalc.h>

#include <GlobalVar.h>

#define DBGH
#include <dbg.h>

static int s_allocno = 0;


static char chanelstr[ CHANEL_MAX ][ 20 ] =
{
	"Xposition",
	"Yposition",
	"Zposition",
	"Zrotation",
	"Xrotation",
	"Yrotation"
};


CBVHElem::CBVHElem()
{
	InitParams();
	serialno = s_allocno++;
}

CBVHElem::~CBVHElem()
{
	DestroyObjs();
}

int CBVHElem::InitParams()
{
	isroot = 0;

	describeno = 0;
	serialno = 0;

	ZeroMemory( name, sizeof( char ) * PATH_LENG );
	ZeroMemory( &offset, sizeof( ChaVector3 ) );
	chanelnum = 0;
	ZeroMemory( chanels, sizeof( int ) * CHANEL_MAX );

	framenum = 0;
	frametime = 0.0f;

	trans = 0;
	rotate = 0;
	xyzrot = 0;
	qptr = 0;
	treeq = 0;
	transpose = 0;
	transmat = 0;

	partransptr = 0;

	parent = 0;
	child = 0;
	brother = 0;

	ZeroMemory( &position, sizeof( ChaVector3 ) );

	mqono = 0;

	samenameboneseri = -1;

	rotordercnt = 0;
	ZeroMemory( rotorder, sizeof( int ) * ROTAXIS_MAX );

	bonenum = 0;
	brono = 0;

	return 0;
}
int CBVHElem::DestroyObjs()
{
	if( trans ){
		delete [] trans;
		trans = 0;
	}

	if( rotate ){
		delete [] rotate;
		rotate = 0;
	}
	if (xyzrot){
		delete[] xyzrot;
		xyzrot = 0;
	}

	if( qptr ){
		delete [] qptr;
		qptr = 0;
	}

	if( treeq ){
		delete [] treeq;
		treeq = 0;
	}

	if( transpose ){
		delete [] transpose;
		transpose = 0;
	}

	if (transmat){
		delete[] transmat;
		transmat = 0;
	}

	return 0;
}

char* CBVHElem::GetFloat( char* srcstr, float* dstfloat, int* dstsetflag )
{
	char* valuehead = srcstr;

	int curpos;
	size_t strleng;

	curpos = 0;
	strleng = strlen(srcstr);
	if ((strleng <= 0) || (strleng > 2048)) {
		_ASSERT(0);
		*dstsetflag = 0;//
		return 0;
	}

	//先頭の非数字をスキップ
	while( (curpos < strleng) && ( (isdigit( *valuehead ) == 0) && (*valuehead != '-'))  ){
		if( isalpha( *valuehead ) != 0 ){
			//illeagal letter
			DbgOut( L"bvhelem : GetDigit : isalpha error !!!\n" );
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



char* CBVHElem::GetDigit( char* srcstr, int* dstint, int* dstsetflag )
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
			DbgOut( L"bvhelem : GetDigit : isalpha error !!!\n" );
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

char* CBVHElem::GetChanelType( char* srcstr, int* dstint, int* dstsetflag )
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

	//先頭の非アルファベットをスキップ
	while( (curpos < strleng) && (isalpha( *valuehead ) == 0) ){

		valuehead++;
		curpos++;
	}

	char* valueend = valuehead;
	//valueの終わりをサーチ
	int valueleng = 0;
	while( (curpos < strleng) && ( isalpha( *valueend ) ) ){
		valueend++;
		curpos++;
		valueleng++;
	}

	if( valueleng >= 255 ){
		_ASSERT( 0 );
		*dstsetflag = 0;//
		return 0;
	}
	char tmpchar[256];
	strncpy_s( tmpchar, 256, valuehead, valueleng );
	tmpchar[valueleng] = 0;


	int chanelno;
	int cmpflag;
	int findtype = -1;
	for( chanelno = CHANEL_XPOS; chanelno <= CHANEL_YROT; chanelno++ ){
		cmpflag = strcmp( tmpchar, chanelstr[chanelno] );
		if( cmpflag == 0 ){
			findtype = chanelno;

			if( rotordercnt >= ROTAXIS_MAX ){
				//_ASSERT( 0 );
				//ボーン読み込みとモーション読み込みと２回通る。２回目はスキップ
			}else{
				switch( findtype ){
				case CHANEL_XROT:
					rotorder[rotordercnt] = ROTAXIS_X;
					rotordercnt++;
					break;
				case CHANEL_YROT:
					rotorder[rotordercnt] = ROTAXIS_Y;
					rotordercnt++;
					break;
				case CHANEL_ZROT:
					rotorder[rotordercnt] = ROTAXIS_Z;
					rotordercnt++;
					break;
				}
			}

			break;
		}
	}

	if( findtype < 0 ){
		DbgOut( L"bvhelem : GetChanelType : unknown chanel type error !!!\n" );
		_ASSERT( 0 );
		*dstsetflag = 0;
		return 0;
	}else{
		*dstint = findtype;
	}

	return valueend;
}


int CBVHElem::SetName( char* srcname )
{
	size_t totalleng = strlen( srcname );
	if ((totalleng <= 0) || (totalleng > 2048)) {
		DbgOut(L"bvhelem : SetName : too long name error !!!\n");
		_ASSERT(0);
		return 1;
	}

	int startpos = 0;
	int spaceflag;

	spaceflag = isspace( *srcname );
	while( spaceflag && (startpos < totalleng) ){
		startpos++;
		if( *(srcname + startpos) != 0 ){
			spaceflag = isspace( *(srcname + startpos) );
		}else{
			spaceflag = 0;
		}
	}

	int endpos = startpos;
	if( startpos < totalleng ){	
		while( *(srcname + endpos) ){
			char chkchar = *(srcname + endpos);
			if( (endpos < totalleng) &&
				( ((chkchar >= 'a') && (chkchar <= 'z')) || ((chkchar >= 'A') && (chkchar <= 'Z')) ||
				((chkchar >= '0') && (chkchar <= '9')) || 
				(chkchar == '_') || (chkchar == '-') || (chkchar == '+') || (chkchar == '.') )
			){
				endpos++;
			}else{
				break;
			}
		}
	}


	int leng;
	leng = endpos - startpos + 1;

	if( leng >= PATH_LENG ){
		DbgOut( L"bvhelem : SetName : name buffer leng too short error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strncpy_s( name, PATH_LENG, srcname + startpos, leng );
	*( name + endpos ) = 0;

	return 0;
}
int CBVHElem::SetIsRoot( int srcisroot )
{
	isroot = srcisroot;
	return 0;
}

int CBVHElem::SetOffset( char* srcchar )
{
	
	int setflag;
	char* valuehead = srcchar;

	valuehead = GetFloat( valuehead, &(offset.x), &setflag );
	if( !setflag || !valuehead ){
		DbgOut( L"bvhelem : SetOffset : GetFloat x error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	valuehead = GetFloat( valuehead, &(offset.y), &setflag );
	if( !setflag || !valuehead ){
		DbgOut( L"bvhelem : SetOffset : GetFloat y error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	valuehead = GetFloat( valuehead, &(offset.z), &setflag );
	if( !setflag ){
		DbgOut( L"bvhelem : SetOffset : GetFloat z error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBVHElem::SetChanels( char* srcchar )
{
	int setflag;
	char* valuehead = srcchar;

	valuehead = GetDigit( valuehead, &chanelnum, &setflag );
	if( !setflag || !valuehead ){
		DbgOut( L"bvhelem : SetChanels : GetDigit chanelnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (chanelnum != 3) && (chanelnum != 6) ){
		DbgOut( L"bvhelem : SetChanels : chanelnum %d is not typical value : not supported error !!!\n", chanelnum );
		_ASSERT( 0 );
		return 1;
	}

	int chanelno;
	for( chanelno = 0; chanelno < chanelnum; chanelno++ ){
		if( !valuehead ){
			DbgOut( L"bvhelem : SetChanels : no more params error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		valuehead = GetChanelType( valuehead, &(chanels[chanelno]), &setflag );
		if( !setflag ){
			DbgOut( L"bvhelem : SetChanels : GetChanelType %d error !!!\n", chanelno );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int CBVHElem::CreateMotionObjs( int srcframes )
{

	DestroyObjs();

	framenum = srcframes;

	trans = new ChaVector3[ framenum ];
	if( !trans ){
		DbgOut( L"bvhelem : CreateMotionObjs : trans alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( trans, sizeof( ChaVector3 ) * framenum );


	rotate = new ChaVector3[ framenum ];
	if( !rotate ){
		DbgOut( L"bvhelem : CreateMotionObjs : rotate alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( rotate, sizeof( ChaVector3 ) * framenum );

	xyzrot = new ChaVector3[framenum];
	if (!xyzrot){
		DbgOut(L"bvhelem : CreateMotionObjs : xyzrot alloc error !!!\n");
		_ASSERT(0);
		return 1;
	}
	ZeroMemory(xyzrot, sizeof(ChaVector3)* framenum);


	qptr = new CQuaternion[ framenum ];
	if( !qptr ){
		DbgOut( L"bvhelem : CreateMotionObj : qptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	treeq = new CQuaternion[ framenum ];
	if( !treeq ){
		DbgOut( L"bvhelem : CreateMotionObj : treeq alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	transpose = new CQuaternion[ framenum ];
	if( !transpose ){
		DbgOut( L"bvhelem : CreateMotionObj : transpose alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	transmat = new ChaMatrix[framenum];
	if (!transmat){
		DbgOut(L"bvhelem : CreateMotionObj : transmat alloc error !!!\n");
		_ASSERT(0);
		return 1;
	}
	return 0;
}

int CBVHElem::SetMotionParams( int srcframeno, float* srcfloat )
{
	int chanelno;
	int curtype;

	if( trans && rotate ){
		for( chanelno = 0; chanelno < chanelnum; chanelno++ ){
			curtype = chanels[chanelno];

			switch( curtype ){
			case CHANEL_XPOS:
				( trans + srcframeno )->x = *( srcfloat + chanelno );
				break;
			case CHANEL_YPOS:
				( trans + srcframeno )->y = *( srcfloat + chanelno );
				break;
			case CHANEL_ZPOS:
				( trans + srcframeno )->z = *( srcfloat + chanelno );
				break;
			case CHANEL_ZROT:
				( rotate + srcframeno )->z = *( srcfloat + chanelno );
				break;
			case CHANEL_XROT:
				( rotate + srcframeno )->x = *( srcfloat + chanelno );
				break;
			case CHANEL_YROT:
				( rotate + srcframeno )->y = *( srcfloat + chanelno );
				break;
			default:
				DbgOut( L"bvhelem : SetMotionParams : unknown chanel type warning skip !!!\n" );
				_ASSERT( 0 );
				break;
			}
		}
	}
	return 0;
}

int CBVHElem::DbgOutBVHElem( int srcdepth, int outmotionflag )
{
/***
	char tabchar[ MAXBONENUM + 1 ];

	int dno;
	for( dno = 0; dno < srcdepth; dno++ ){
		tabchar[dno] = '\t';
	}
	tabchar[srcdepth] = 0;

	DbgOut( L"%sname : %s, isroot %d, serialno %d\r\n", tabchar, name, isroot, serialno );
	DbgOut( L"%soffset : %f %f %f\r\n", tabchar, offset.x, offset.y, offset.z );
	DbgOut( L"%sposition : %f %f %f\r\n", tabchar, position.x, position.y, position.z );
	DbgOut( L"%schanelnum %d\r\n", tabchar, chanelnum );
	DbgOut( L"%sparent %x, child %x, brother %x\r\n", tabchar, parent, child, brother );

	int cno;
	for( cno = 0; cno < chanelnum; cno++ ){
		DbgOut( L"%schanel%d : %s\r\n", tabchar, cno, chanelstr[ chanels[ cno ] ] );
	}
	DbgOut( L"%sframenum %d\r\n", tabchar, framenum );
***/
/***
	if( outmotionflag && trans && rotate ){
		int fno;
		for( fno = 0; fno < framenum; fno++ ){
			DbgOut( L"%smotion%d : trans %f %f %f, rotate %f %f %f\r\n",
				tabchar, fno, 
				( trans + fno )->x, ( trans + fno )->y, ( trans + fno )->z, 
				( rotate + fno )->x, ( rotate + fno )->y, ( rotate + fno )->z 
			);
		}
	}
***/

	return 0;
}

int CBVHElem::SetPosition()
{

	ChaVector3 parentpos;
	if( parent ){
		parentpos = parent->position;

		partransptr = parent->trans;

	}else{
		parentpos.x = 0.0f;
		parentpos.y = 0.0f;
		parentpos.z = 0.0f;

		partransptr = 0;
	}

	position = offset + parentpos;


	return 0;
}

int CBVHElem::Mult( float srcmult )
{
	
	offset *= srcmult;
	position *= srcmult;

	if( trans ){
		int fno;
		for( fno = 0; fno < framenum; fno++ ){
			*( trans + fno ) *= srcmult;
		}
	}

	return 0;
}

/***
int CBVHElem::ConvertRotate2Q()
{
	int fno;

	ChaMatrix matx, maty, matz;
	ChaMatrix tmatx, tmaty, tmatz;
	ChaMatrix mat;
	D3DXQUATERNION q;

	for( fno = 0; fno < framenum; fno++ ){
		ChaMatrixRotationX( &matx, ( rotate + fno )->x * (float)DEG2PAI );
		ChaMatrixRotationY( &maty, ( rotate + fno )->y * (float)DEG2PAI );
		ChaMatrixRotationZ( &matz, ( rotate + fno )->z * (float)DEG2PAI );

		ChaMatrixTranspose( &tmatx, &matx );
		ChaMatrixTranspose( &tmaty, &maty );
		ChaMatrixTranspose( &tmatz, &matz );

		mat = tmatz * tmatx * tmaty;
		//mat = tmaty * tmatx * tmatz;

		D3DXQuaternionRotationMatrix( &q, &mat );
		( qptr + fno )->x = -q.x;
		( qptr + fno )->y = -q.y;
		( qptr + fno )->z = -q.z;
		( qptr + fno )->w = q.w;
///////////////
		(qptr + fno)->transpose( transpose + fno );

	}

	return 0;
}
***/


int CBVHElem::ConvertRotate2Q()
{
	int fno;
	CQuaternion iniq;

	//CQuaternion y180q;
	//y180q.SetRotation( 0.0f, 180.0f, 0.0f );

	if( qptr && trans && transpose ){
		CQuaternion q[ ROTAXIS_MAX ];
		CQuaternion qall;
		for( fno = 0; fno < framenum; fno++ ){
			q[ROTAXIS_X].SetParams(1.0f, 0.0f, 0.0f, 0.0f );
			q[ROTAXIS_Y].SetParams(1.0f, 0.0f, 0.0f, 0.0f);
			q[ROTAXIS_Z].SetParams(1.0f, 0.0f, 0.0f, 0.0f);

			q[ROTAXIS_X].SetRotationZXY(&iniq, (rotate + fno)->x, 0.0f, 0.0f);
			q[ROTAXIS_Y].SetRotationZXY(&iniq, 0.0f, (rotate + fno)->y, 0.0f);
			q[ROTAXIS_Z].SetRotationZXY(&iniq, 0.0f, 0.0f, (rotate + fno)->z);

			qall = q[ rotorder[0] ] * q[ rotorder[1] ] * q[ rotorder[2] ];
			//qall = qy * qx * qz;//Z, X, Yの時　： matrixに直すとmatz * matx * maty
			*( qptr + fno ) = qall;

	///////////////
			qall.transpose( transpose + fno );

	///////////////
			//y180q.Rotate( ( trans + fno ), *( trans + fno ) );
		}
	}
	return 0;
}

//int CBVHElem::ConvZxyRot()
int CBVHElem::ConvXYZRot()
{
	//int frameno;
	//ChaVector3 befeul;
	//ZeroMemory(&befeul, sizeof(ChaVector3));
	//CQuaternion befq;
	//befq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
	//for (frameno = 0; frameno < framenum; frameno++){
	//	befq.InOrder(qptr + frameno);

	//	ChaVector3 euler;
	//	CQuaternion calcq;
	//	CQuaternion iniq;
	//	//qToEulerAxis(iniq, (qptr + frameno), &euler);
	//	//modifyEuler(&euler, &befeul);
	//	(qptr + frameno)->Q2EulXYZ(&iniq, befeul, &euler);

	//	*(xyzrot + frameno) = euler;

	//	befeul = euler;
	//	//befq.SetRotationZXY(0, *(xyzrot + frameno));
	//	befq.SetRotationXYZ(0, *(xyzrot + frameno));
	//}


	int isfirstbone;
	if (GetParent()) {
		isfirstbone = 0;
	}
	else {
		isfirstbone = 1;
	}

	//#####################################################################################
	//bvhのendjointの名前はsite*. リターゲット時に末端に渡すのはその手前. ３代チェックする
	//#####################################################################################
	int isendbone;
	if (GetChild()) {
		if (GetChild()->GetChild()) {
			if (GetChild()->GetChild()->GetChild()) {
				isendbone = 0;
			}
			else {
				isendbone = 1;
			}
		}
		else {
			isendbone = 1;
		}
	}
	else {
		isendbone = 1;
	}


	//ChaVector3 befeul = ChaVector3(0.0f, 0.0f, 0.0f);
	BEFEUL befeul;
	befeul.Init();
	ChaVector3 cureul = ChaVector3(0.0f, 0.0f, 0.0f);

	int frameno;
	for (frameno = 0; frameno < framenum; frameno++) {
		CQuaternion* curq;
		curq = (qptr + frameno);

		int befframe;
		befframe = frameno - 1;
		if (frameno <= 1) {
			//roundingframe が0.0または1.0の場合 
			befeul.befframeeul = ChaVector3(0.0f, 0.0f, 0.0f);
			befeul.currentframeeul = ChaVector3(0.0f, 0.0f, 0.0f);
		}
		else {
			befeul.befframeeul = cureul;
			befeul.currentframeeul = cureul;
		}


		//2023/01/14
		//rootjointを２回転する場合など　180度補正は必要(１フレームにつき165度までの変化しか出来ない制限は必要)
		//しかし　bvh2fbxなど　１フレームにアニメが付いているデータでうまくいくようにするために　0フレームと１フレームは除外
		int notmodify180flag = 1;
		//if (g_underIKRot == false) {
			if (frameno <= 1) {
				//0フレームと１フレームは　180度ずれチェックをしない
				notmodify180flag = 1;
			}
			else {
				notmodify180flag = 0;
			}
		//}
		//else {
		//	//2023/01/26
		//	//IKRot中は　０フレームも１フレームも　180度チェックをする
		//	notmodify180flag = 0;
		//	//if (frameno <= 1) {
		//	//	befeul.befframeeul = ChaVector3(0.0f, 0.0f, 0.0f);
		//	//}
		//}

		curq->CalcFBXEulXYZ(0, befeul, &cureul, isfirstbone, isendbone, notmodify180flag);
		*(xyzrot + frameno) = cureul;
		//if ((frameno == 0) || (frameno == 1) || IsValidNewEul(cureul, befeul.befframeeul)) {
		//	befeul.befframeeul = cureul;
		//}
	}


	return 0;
}


int CBVHElem::CheckNotAlNumName( char** ppdstname )
{
	if(!ppdstname){
		_ASSERT(0);
		return 1;
	}

	size_t leng;
	leng = strlen( name );
	if ((leng <= 0) || (leng > 2048)) {
		_ASSERT(0);
		*ppdstname = 0;
		return 1;
	}

	int curc;
	size_t cno;
	int findflag = 0;
	int chk;
	for( cno = 0; cno < leng; cno++ ){
		curc = *( name + cno );

		chk = isalnum( curc );
		if( chk == 0 ){
			findflag = 1;
			break;
		}
	}

	if( findflag ){
		*ppdstname = name;
	}else{
		*ppdstname = 0;
	}

	return 0;
}


int CBVHElem::CalcDiffTra( int frameno, ChaVector3* pdifftra )
{

	ChaMatrix rotmat = (qptr + frameno)->MakeRotMatX();
	rotmat.AddTranslation(*(trans + frameno));

	ChaVector3 aftpos( 0.0f, 0.0f, 0.0f );
	ChaVector3TransformCoord( &aftpos, &position, &rotmat );

	*pdifftra = aftpos - position;

	return 0;
}

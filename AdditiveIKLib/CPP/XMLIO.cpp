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

#include <XMLIO.h>
#include <Model.h>
#include <Bone.h>
//#include <quaternion.h>

#define DBGH
#include <dbg.h>

#include <string>

//#include <PmCipherDll.h>

CXMLIO::CXMLIO()
{
	InitParams();
}

CXMLIO::~CXMLIO()
{
	DestroyObjs();
}

int CXMLIO::InitParams()
{
	m_mode = 0;
	ZeroMemory( &m_xmliobuf, sizeof( XMLIOBUF ) );
	m_hfile = INVALID_HANDLE_VALUE;

	return 0;
}

int CXMLIO::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == XMLIO_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( m_xmliobuf.buf ){
		free( m_xmliobuf.buf );
		m_xmliobuf.buf = 0;
	}

	m_mode = 0;
	ZeroMemory( &m_xmliobuf, sizeof( XMLIOBUF ) );
	m_hfile = INVALID_HANDLE_VALUE;

	return 0;
}

int CXMLIO::WriteVoid2File( void* pvoid, unsigned int srcleng )
{
	if( m_hfile == INVALID_HANDLE_VALUE ){
		return 0;
	}
	if (!pvoid) {
		_ASSERT(0);
		return 1;
	}


	DWORD writeleng = 0;
	WriteFile( m_hfile, pvoid, srcleng, &writeleng, NULL );
	if( srcleng != writeleng ){
		return 1;
	}

	return 0;
}


int CXMLIO::Write2File( const char* lpFormat, ... )
{
	if( m_hfile == INVALID_HANDLE_VALUE ){
		return 0;
	}
	if (!lpFormat) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[XMLIOLINELEN];
			
	ZeroMemory( outchar, XMLIOLINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, XMLIOLINELEN, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 )
		return 1;

	outchar[XMLIOLINELEN - 1] = 0;
	wleng = (unsigned long)strlen( outchar );
	WriteFile( m_hfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		return 1;
	}
	
	return 0;
}

/***
int CXMLIO::SetBuffer( CPmCipherDll* cipher, int dataindex )
{
	PNDPROP prop;
	ZeroMemory( &prop, sizeof( PNDPROP ) );


	CallF( cipher->GetProperty( dataindex, &prop ), return 1 );

	if( prop.sourcesize <= 0 ){
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( prop.sourcesize + 1 ) );
	if( !newbuf ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( prop.sourcesize + 1 ) );

	int getsize = 0;
	CallF( cipher->Decrypt( prop.path, (unsigned char*)newbuf, prop.sourcesize, &getsize ), return 1 );

	if( getsize != prop.sourcesize ){
		_ASSERT( 0 );
		return 1;
	}

	m_xmliobuf.buf = newbuf;
	m_xmliobuf.pos = 0;
	m_xmliobuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, "</XMLIO>" );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = prop.sourcesize;
	}
	m_xmliobuf.bufleng = validleng;


	return 0;
}
***/

int CXMLIO::SetBuffer()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng <= 0 ){
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	BOOL bresult = ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( !bresult || (rleng != readleng) ){
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}

	m_xmliobuf.buf = newbuf;
	m_xmliobuf.pos = 0;
	m_xmliobuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, "</XMLIO>" );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = bufleng;
	}
	m_xmliobuf.bufleng = validleng;


	return 0;
}

int CXMLIO::Read_Int( XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, int* dstint )
{
	if (!xmliobuf || !startpat || !endpat || !dstint) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	char* startptr;
	startptr = strstr( xmliobuf->buf + xmliobuf->pos, startpat );
	if( !startptr ){
//		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( xmliobuf->buf + xmliobuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
//		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - xmliobuf->buf );
	if( (endpatpos <= 0) || (endpatpos > (int)xmliobuf->bufleng) ){
		_ASSERT( 0 );
		return 1;
	}

	size_t startpatlen = strlen(startpat);//bufにstartpatが含まれていることは確定している
	char* srcchar = startptr + startpatlen;
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		_ASSERT( 0 );
		return 1;
	}


	int stepnum = 0;
	ret = GetInt( dstint, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CXMLIO::Read_Float( XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, float* dstfloat )
{
	if (!xmliobuf || !startpat || !endpat || !dstfloat) {
		_ASSERT(0);
		return 1;
	}

	int ret;
	char* startptr;
	startptr = strstr( xmliobuf->buf + xmliobuf->pos, startpat );
	if( !startptr ){
		//_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( xmliobuf->buf + xmliobuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		//_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - xmliobuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)xmliobuf->bufleng) ){
		//_ASSERT( 0 );
		return 1;
	}

	size_t startpatlen = strlen(startpat);//bufにstartpatが含まれていることは確定している
	char* srcchar = startptr + startpatlen;
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		//_ASSERT( 0 );
		return 1;
	}


	int stepnum = 0;
	ret = GetFloat( dstfloat, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CXMLIO::Read_Vec3( XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, ChaVector3* dstvec )
{
	if (!xmliobuf || !startpat || !endpat || !dstvec) {
		_ASSERT(0);
		return 1;
	}


	int ret;
	char* startptr;
	startptr = strstr( xmliobuf->buf + xmliobuf->pos, startpat );
	if( !startptr ){
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( xmliobuf->buf + xmliobuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - xmliobuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)xmliobuf->bufleng) ){
		_ASSERT( 0 );
		return 1;
	}

	size_t startpatlen = strlen(startpat);//bufにstartpatが含まれていることは確定している
	char* srcchar = startptr + startpatlen;
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		_ASSERT( 0 );
		return 1;
	}

	float xval = 0.0f;
	float yval = 0.0f;
	float zval = 0.0f;

	int srcpos = 0;
	int stepnum = 0;
	ret = GetFloat( &xval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &yval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &zval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	dstvec->x = xval;
	dstvec->y = yval;
	dstvec->z = zval;


	return 0;
}
int CXMLIO::Read_Q( XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, CQuaternion* dstq )
{
	if (!xmliobuf || !startpat || !endpat || !dstq) {
		_ASSERT(0);
		return 1;
	}


	int ret;
	char* startptr;
	startptr = strstr( xmliobuf->buf + xmliobuf->pos, startpat );
	if( !startptr ){
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( xmliobuf->buf + xmliobuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - xmliobuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)xmliobuf->bufleng) ){
		_ASSERT( 0 );
		return 1;
	}

	size_t startpatlen = strlen(startpat);//bufにstartpatが含まれていることは確定している
	char* srcchar = startptr + startpatlen;
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		_ASSERT( 0 );
		return 1;
	}

	float xval = 0.0f;
	float yval = 0.0f;
	float zval = 0.0f;
	float wval = 0.0f;

	int srcpos = 0;
	int stepnum = 0;
	ret = GetFloat( &xval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &yval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &zval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &wval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	dstq->x = xval;
	dstq->y = yval;
	dstq->z = zval;
	dstq->w = wval;

	return 0;
}

int CXMLIO::Read_Matrix(XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, ChaMatrix* dstmat)
{
	if (!xmliobuf || !startpat || !endpat || !dstmat) {
		_ASSERT(0);
		return 1;
	}


	int ret;
	char* startptr;
	startptr = strstr(xmliobuf->buf + xmliobuf->pos, startpat);
	if (!startptr) {
		_ASSERT(0);
		return 1;
	}
	char* endptr;
	endptr = strstr(xmliobuf->buf + xmliobuf->pos, endpat);
	if (!endptr || (endptr <= startptr)) {
		_ASSERT(0);
		return 1;
	}

	int endpatpos;
	endpatpos = (int)(endptr - xmliobuf->buf);
	if ((endpatpos <= 0) || (endpatpos >= (int)xmliobuf->bufleng)) {
		_ASSERT(0);
		return 1;
	}

	size_t startpatlen = strlen(startpat);//bufにstartpatが含まれていることは確定している
	char* srcchar = startptr + startpatlen;
	int srcleng = (int)(endptr - srcchar);
	if ((srcleng <= 0) || (srcleng >= 256)) {
		_ASSERT(0);
		return 1;
	}

	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;

	int srcpos = 0;
	int stepnum = 0;
	_11 = 0.0f;
	ret = GetFloat(&_11, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_12 = 0.0f;
	ret = GetFloat(&_12, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_13 = 0.0f;
	ret = GetFloat(&_13, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_14 = 0.0f;
	ret = GetFloat(&_14, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}


	srcpos += stepnum;
	stepnum = 0;
	_21 = 0.0f;
	ret = GetFloat(&_21, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_22 = 0.0f;
	ret = GetFloat(&_22, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_23 = 0.0f;
	ret = GetFloat(&_23, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_24 = 0.0f;
	ret = GetFloat(&_24, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	_31 = 0.0f;
	ret = GetFloat(&_31, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_32 = 0.0f;
	ret = GetFloat(&_32, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_33 = 0.0f;
	ret = GetFloat(&_33, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_34 = 0.0f;
	ret = GetFloat(&_34, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	_41 = 0.0f;
	ret = GetFloat(&_41, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_42 = 0.0f;
	ret = GetFloat(&_42, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_43 = 0.0f;
	ret = GetFloat(&_43, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}
	srcpos += stepnum;
	stepnum = 0;
	_44 = 0.0f;
	ret = GetFloat(&_44, srcchar, srcpos, srcleng, &stepnum);
	if (ret) {
		_ASSERT(0);
		return 1;
	}

	dstmat->SetParams(
		_11, _12, _13, _14,
		_21, _22, _23, _24,
		_31, _32, _33, _34,
		_41, _42, _43, _44
	);

	return 0;


}



int CXMLIO::Read_Str( XMLIOBUF* xmliobuf, const char* startpat, const char* endpat, char* dststr, int arrayleng )
{
	if (!xmliobuf || !startpat || !endpat || !dststr) {
		_ASSERT(0);
		return 1;
	}


	int ret;
	char* startptr;
	startptr = strstr( xmliobuf->buf + xmliobuf->pos, startpat );
	if( !startptr ){
		return 1;
	}
	char* endptr;
	endptr = strstr( xmliobuf->buf + xmliobuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - xmliobuf->buf );
	if( (endpatpos <= 0) || (endpatpos > (int)xmliobuf->bufleng) ){
		_ASSERT( 0 );
		return 1;
	}

	size_t startpatlen = strlen(startpat);//bufにstartpatが含まれていることは確定している
	char* srcchar = startptr + startpatlen;
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= arrayleng) ){
		_ASSERT( 0 );
		return 1;
	}

	ret = GetName( dststr, arrayleng, srcchar, 0, srcleng );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CXMLIO::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	if (!dstint || !srcchar || !stepnum) {
		_ASSERT(0);
		return 1;
	}

	char tempchar[256];
	ZeroMemory( tempchar, sizeof( char ) * 256 );

	strncpy_s( tempchar, 256, srcchar + pos, srcleng );

	*dstint = atoi( tempchar );

	*stepnum = srcleng;


	return 0;
}
int CXMLIO::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	if (!dstfloat || !srcchar || !stepnum) {
		_ASSERT(0);
		return 1;
	}



	char* startptr = 0;
	char* endptr = 0;
	startptr = srcchar + pos;
	int poscnt = 0;
	while( *startptr && ( (isspace( *startptr ) != 0) || (*startptr == ',') ) && (poscnt < srcleng) ){
		startptr++;
		poscnt++;
	}

	endptr = startptr;
	while( *endptr && ( ((*endptr >= '0') && (*endptr <= '9')) || (*endptr == '.') || (*endptr == '-')) && (poscnt < srcleng) ){
		endptr++;
		poscnt++;
	}
	char tempchar[256];
	ZeroMemory( tempchar, sizeof( char ) * 256 );

	int leng;
	leng = (int)(endptr - startptr);
	if( leng <= 0 ){
		_ASSERT( 0 );
		return 1;
	}

	strncpy_s( tempchar, 256, startptr, leng );

	*dstfloat = (float)atof( tempchar );

	*stepnum = poscnt;

	return 0;
}
int CXMLIO::GetName( char* dstchar, int dstleng, const char* srcchar, int pos, int srcleng )
{
	if (!dstchar || !srcchar) {
		_ASSERT(0);
		return 1;
	}


	strncpy_s( dstchar, dstleng, srcchar + pos, srcleng );
	*( dstchar + srcleng ) = 0;

	return 0;
}

int CXMLIO::SetXmlIOBuf( XMLIOBUF* srcbuf, const char* startpat, const char* endpat, XMLIOBUF* dstbuf, int delpatflag )
{
	if (!srcbuf || !startpat || !endpat || !dstbuf) {
		_ASSERT(0);
		return 1;
	}


	//strstr中にバッファ範囲を越えないようにチェック　2022/07/17

	size_t chkendpos1;
	chkendpos1 = srcbuf->pos + strlen(startpat);
	size_t chkendpos2;
	chkendpos2 = srcbuf->pos + strlen(startpat) + strlen(endpat);
	if ((chkendpos1 > srcbuf->bufleng) || (chkendpos2 > srcbuf->bufleng)) {
		//end of file
		return 1;
	}


	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr(srcbuf->buf + srcbuf->pos, startpat);
	//endptr = strstr( srcbuf->buf + srcbuf->pos, endpat );//場所移動。　2022/07/17

	size_t spatlen = strlen(startpat);
	//if (delpatflag && startpat) {
	if (delpatflag && startpat && startptr) {//2022/07/17
		startptr = startptr + spatlen;//2022/07/17
	}

	if (startptr != 0) {
		endptr = strstr(startptr, endpat);//startpatよりも後を検索する //2022/07/17
	}

	if( !startptr || !endptr ){
//		_ASSERT( 0 );
		return 1;
	}

	size_t epatlen;
	epatlen = strlen( endpat );

	int chkendpos;
	chkendpos = (int)( endptr + epatlen - srcbuf->buf );
	//if( (chkendpos >= srcbuf->bufleng) || (endptr < startptr) ){
	if( (chkendpos > srcbuf->bufleng) || (endptr < startptr) ){//2022/07/17
		//_ASSERT( 0 );
		return 1;
	}

	srcbuf->pos = chkendpos;

	dstbuf->buf = startptr;

	if (delpatflag) {//2023/09/25
		dstbuf->bufleng = (int)(endptr - startptr);//startptrはdelpatflagを考慮済
	}
	else {
		dstbuf->bufleng = (int)(endptr + epatlen - startptr);//startptrはdelpatflagを考慮済
	}

	dstbuf->pos = 0;
	dstbuf->isend = 0;

	return 0;
}

CBone* CXMLIO::FindBoneByName(CModel* srcmodel, char* bonename, int srcleng)
{
	if (!srcmodel || !bonename) {
		_ASSERT(0);
		return 0;
	}

	if (srcleng > 256){
		_ASSERT(0);
		return 0;
	}
	CBone* curbone = srcmodel->FindBoneByName(bonename);
	if (!curbone){
		//_ASSERT(0);
		return 0;
	}

	return curbone;
}

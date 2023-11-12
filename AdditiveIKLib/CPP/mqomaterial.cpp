#include "stdafx.h"
//#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <GlobalVar.h>

#include <mqomaterial.h>

#include <TexBank.h>

//extern CTexBank* g_texbank;


CMQOMaterial::CMQOMaterial()
{
	InitParams();
}
CMQOMaterial::~CMQOMaterial()
{
	DestroyObjs();
}

int CMQOMaterial::SetParams( int srcno, ChaVector4 srcsceneamb, char* srcchar, int srcleng )
{
	//"���F" col(1.000 0.792 0.651 1.000) dif(0.490) amb(0.540) emi(0.530) spc(0.020) power(0.00)


//DbgOut( L"check!!!: mqomat : SetParams : %d, %s, %d\r\n", srcno, srcchar, srcleng );

	m_materialno = srcno;

	m_sceneamb = srcsceneamb;//!!!!!!!!!!!!!!!!!

	char pat[12][20] = 
	{
		"col(",
		"dif(",
		"amb(",
		"emi(",
		"spc(",
		"power(",
		"tex(",
		"alpha(",
		"bump(",
		"shader(",
		"vcol(",
		"\"" //�I�I�Itex, alpha, bump������łȂ��Ƃ��߁B
	};

	size_t pos = 0;
	int stepnum;
	int ret;

	while( pos < srcleng ){
		while( (pos < srcleng) &&  
		( (*(srcchar + pos) == ' ') || (*(srcchar + pos) == '\t') )
		){
		pos++;
		}

		int cmp;
		int patno;
		size_t patleng;

		int isfind = 0;

		for( patno = 0; patno < 12; patno++ ){
			if( isfind == 1 )
				break;

			patleng = strlen( pat[patno] );
			//if (srcleng - pos >= patleng) {
			if ((patleng < srcleng) && (pos <= (srcleng - patleng))) {//2023/01/09 unsigned

				cmp = strncmp( pat[patno], srcchar + pos, patleng );
				if( cmp == 0 ){
					isfind = 1;
					switch( patno ){
					case 0:
						ret = SetCol( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 1:
						ret = SetDif( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 2:
						ret = SetAmb( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 3:
						ret = SetEmi( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 4:
						ret = SetSpc( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 5:
						ret = SetPower( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 6:
						ret = SetTex( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 7:
						ret = SetAlpha( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 8:
						ret = SetBump( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 9:
						ret = SetShader( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 10:
						ret = SetVcolFlag( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 11:
						//DbgOut( L"MQOMaterial : SetParams : SetName %d\r\n", pos );
						ret = SetName( srcchar, (int)pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					default:
						_ASSERT( 0 );
						return 1;
						break;
					}

				}
			}
		}

		if( isfind == 1 ){
			pos += stepnum;
		}else{
			pos++;
		}

	}

	ret = ConvParamsTo3F();
	_ASSERT( !ret );


	return 0;
}

int CMQOMaterial::ConvParamsTo3F()
{
	m_dif4f.w = m_col.w;
	m_dif4f.x = m_col.x * m_dif;
	m_dif4f.y = m_col.y * m_dif;
	m_dif4f.z = m_col.z * m_dif;

//	amb3f.x = col.x * amb;
//	amb3f.y = col.y * amb;
//	amb3f.z = col.z * amb;

	m_amb3f.x = m_sceneamb.x * m_amb;
	m_amb3f.y = m_sceneamb.y * m_amb;
	m_amb3f.z = m_sceneamb.z * m_amb;

	m_emi3f.x = m_col.x * m_emi;
	m_emi3f.y = m_col.y * m_emi;
	m_emi3f.z = m_col.z * m_emi;

	m_spc3f.x = m_col.x * m_spc;
	m_spc3f.y = m_col.y * m_spc;
	m_spc3f.z = m_col.z * m_spc;

	return 0;
}


int CMQOMaterial::InitParams()
{	
	m_materialno = -1;
	ZeroMemory ( m_name, 256 );

	m_col.w = 1.0f;
	m_col.x = 1.0f;
	m_col.y = 1.0f;
	m_col.z = 1.0f;

	m_dif = 1.0f;
	m_amb = 0.25f;
	m_emi = 0.0f;
	m_spc = 0.0f;
	m_power = 0.0f;

	ZeroMemory ( m_tex, 256 );
	ZeroMemory ( m_alpha, 256 );
	ZeroMemory ( m_bump, 256 );

	//next = 0;


	m_shader = 3;

	m_vcolflag = 0;

	m_dif4f.w = 1.0f;
	m_dif4f.x = 1.0f;
	m_dif4f.y = 1.0f;
	m_dif4f.z = 1.0f;

	m_amb3f.x = 0.25f;
	m_amb3f.y = 0.25f;
	m_amb3f.z = 0.25f;

	m_emi3f.x = 0.0f;
	m_emi3f.y = 0.0f;
	m_emi3f.z = 0.0f;

	m_spc3f.x = 0.0f;
	m_spc3f.y = 0.0f;
	m_spc3f.z = 0.0f;

	m_transparent = 0;
	m_texrule = 0;//!!!!!!!!!
	m_blendmode = 0;

	m_uanime = 0.0f;
	m_vanime = 0.0f;


	m_curtexname = 0;

	m_alphatest = 1;
	m_alphaval = 8;

	m_glowmult[0] = 1.0f;
	m_glowmult[1] = 1.0f;
	m_glowmult[2] = 1.0f;

	m_orgalpha = 1.0f;

	m_texid = -1;

	m_convnamenum = 0;
	m_ppconvname = 0;

	return 0;
}

int CMQOMaterial::DestroyObjs()
{
	if( m_convnamenum > 0 ){
		int nameno;
		for( nameno = 0; nameno < m_convnamenum; nameno++ ){
			char* delconv;
			delconv = *( m_ppconvname + nameno );
			if( delconv )
				free( delconv );
		}

		free( m_ppconvname );
		m_ppconvname = 0;
		m_convnamenum = 0;
	}

	return 0;
}

int CMQOMaterial::SetName( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//"���F"
	int step = 1;

	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"')

// "�̌��)�܂��̓X�y�[�X���������Ƃ��̂ݏI�[�Ƃ���B
//		!( (*(srcchar + pos + step) == '\"') && ( (*(srcchar + pos + step + 1) == ')' ) || (*(srcchar + pos + step + 1) == ' ' ) ) ) 

	){
		step++;
	}

	if( ((step - 1) < 256) && ((step - 1) > 0) ){
		ZeroMemory( m_name, sizeof( char ) * 256 );
		strncpy_s( m_name, 256, srcchar + pos + 1, ((size_t)step - 1) );
		m_name[step -1] = 0;
	}

//	DbgOut( L"check!!! : mqomat : SetName %s, %d, %d\r\n", srcchar, pos, srcleng );
//	DbgOut( L"check!!! : mqomat : SetName %d, %s\r\n", step, name );


	if( pos + step < srcleng )
		*stepnum = step + 1;
	else
		*stepnum = step;


	return 0;
}
int CMQOMaterial::SetCol( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//col(1.000 0.792 0.651 1.000)
	int ret;
	int step;
	int cnt;
	float dstfloat;
	*stepnum = 4;
	for( cnt = 0; cnt < 4; cnt++ ){		
		ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
		if( ret )
			return ret;

		switch( cnt ){
		case 0:
			m_col.x = dstfloat;
			break;
		case 1:
			m_col.y = dstfloat;
			break;
		case 2:
			m_col.z = dstfloat;
			break;
		case 3:
			m_col.w = dstfloat;
			m_orgalpha = dstfloat;
			break;
		default:
			break;
		}

		*stepnum += step;
	}

	(*stepnum)++;//�Ō�́j�̕�

	return 0;
}


int CMQOMaterial::SetDif( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//dif(0.490) 
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;
	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	m_dif = dstfloat;

	m_dif4f.w = m_col.w;
	m_dif4f.x = m_col.x * m_dif;
	m_dif4f.y = m_col.y * m_dif;
	m_dif4f.z = m_col.z * m_dif;


	*stepnum += step + 1;

	return 0;
}
int CMQOMaterial::SetAmb( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//amb(0.540) 
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	m_amb = dstfloat;

//	amb3f.x = col.x * amb;
//	amb3f.y = col.y * amb;
//	amb3f.z = col.z * amb;

	m_amb3f.x = m_sceneamb.x * m_amb;
	m_amb3f.y = m_sceneamb.y * m_amb;
	m_amb3f.z = m_sceneamb.z * m_amb;

	*stepnum += step + 1;
	return 0;
}
int CMQOMaterial::SetEmi( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//emi(0.530)
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	m_emi = dstfloat;

	m_emi3f.x = m_col.x * m_emi;
	m_emi3f.y = m_col.y * m_emi;
	m_emi3f.z = m_col.z * m_emi;


	*stepnum += step + 1;

	return 0;
}
int CMQOMaterial::SetSpc( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//spc(0.020) 
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	m_spc = dstfloat;

	m_spc3f.x = m_col.x * m_spc;
	m_spc3f.y = m_col.y * m_spc;
	m_spc3f.z = m_col.z * m_spc;

	*stepnum += step + 1;

	return 0;
}
int CMQOMaterial::SetPower( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//power(0.00)
	int step;
	float dstfloat;
	*stepnum = 6;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	m_power = dstfloat;

	*stepnum += step + 1;

	return 0;
}

int CMQOMaterial::SetShader( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//shader(3)
	int step;
	int dstint;
	*stepnum = 7;
	int ret;

	ret = GetInt( &dstint, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	m_shader = dstint;

	*stepnum += step + 1;

	return 0;
}

int CMQOMaterial::SetVcolFlag( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//vcol(1)
	int step;
	int dstint;
	*stepnum = 5;
	int ret;

	ret = GetInt( &dstint, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	m_vcolflag = dstint;

	*stepnum += step + 1;

	return 0;
}

int CMQOMaterial::SetTex( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//tex("sakana.jpg")
	int step = 5;
	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"') ){
		step++;
	}

	if( (step - 5 < 256) && (step - 5 > 0) ){
		strncpy_s( m_tex, 256, srcchar + pos + 5, ((size_t)step - 5) );
		m_tex[step -5] = 0;
	}

	if( pos + step < srcleng )
		*stepnum = step + 2;//�@")�̕�
	else
		*stepnum = step;


	return 0;
}
int CMQOMaterial::SetAlpha( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//alpha("sakana.jpg")
	int step = 7;
	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"') ){
		step++;
	}

	if( (step - 7 < 256) && (step - 7 > 0) ){
		strncpy_s( m_alpha, 256, srcchar + pos + 7, ((size_t)step - 7) );
		m_alpha[step -7] = 0;
	}

	if( pos + step < srcleng )
		*stepnum = step + 2;//�@")�̕�
	else
		*stepnum = step;

	return 0;
}
int CMQOMaterial::SetBump( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//bump("sakana.jpg")
	int step = 6;
	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"') ){
		step++;
	}

	if( (step - 6 < 256) && (step - 6 > 0) ){
		strncpy_s( m_bump, 256, srcchar + pos + 6, ((size_t)step - 6) );
		m_bump[step -6] = 0;
	}

	if( pos + step < srcleng )
		*stepnum = step + 2;//�@")�̕�
	else
		*stepnum = step;

	return 0;
}

int CMQOMaterial::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int flstart;
	int flend;
	int step = 0;

	while( (pos + step < srcleng) && 
	( (*(srcchar + pos + step) == ' ') || (*(srcchar + pos + step) == '\t') )
	//( *(srcchar + pos + step) ) 
	){
		step++;
	}

	flstart = pos+ step;

	while( (pos + step < srcleng) && 
		( isdigit( *(srcchar + pos + step) ) || (*(srcchar + pos + step) == '.') || (*(srcchar + pos + step) == '-') )
	){
		step++;
	}
	flend = pos + step;

	char tempchar[256];
	if( flend - flstart < 256 ){
		strncpy_s( tempchar, 256, srcchar + flstart, ((size_t)flend - flstart) );
		tempchar[flend - flstart] = 0;
		*dstfloat = (float)atof( tempchar );
	}else{
		_ASSERT( 0 );
		*dstfloat = 0.0f;
	}

	*stepnum = step;

	return 0;

}

int CMQOMaterial::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int flstart;
	int flend;
	int step = 0;

	while( (pos + step < srcleng) && 
	( (*(srcchar + pos + step) == ' ') || (*(srcchar + pos + step) == '\t') )
	//( *(srcchar + pos + step) ) 
	){
		step++;
	}

	flstart = pos+ step;

	while( (pos + step < srcleng) && 
		( isdigit( *(srcchar + pos + step) ) || (*(srcchar + pos + step) == '-') )
	){
		step++;
	}
	flend = pos + step;

	char tempchar[256];
	if( flend - flstart < 256 ){
		strncpy_s( tempchar, 256, srcchar + flstart, ((size_t)flend - flstart) );
		tempchar[flend - flstart] = 0;
		*dstint = atoi( tempchar );
	}else{
		_ASSERT( 0 );
		*dstint = 0;
	}

	*stepnum = step;

	return 0;
}



int CMQOMaterial::Dump()
{
	WCHAR wname[256] = {0};
	WCHAR wtex[256] = {0};
	WCHAR walpha[256] = {0};
	WCHAR wbump[256] = {0};

	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_name, 256, wname, 256 );
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_tex, 256, wtex, 256 );
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_alpha, 256, walpha, 256 );
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_bump, 256, wbump, 256 );


	DbgOut( L"MQOMaterial %d : name %s\r\n", m_materialno, wname );
	DbgOut( L"\tcol : r %f, g %f, b %f, a %f\r\n", m_col.x, m_col.y, m_col.z, m_col.w );
	DbgOut( L"\tdif %f\r\n", m_dif );
	DbgOut( L"\tamb %f\r\n", m_amb );
	DbgOut( L"\temi %f\r\n", m_emi );
	DbgOut( L"\tspc %f\r\n", m_spc );
	DbgOut( L"\tspc3f %f, %f, %f\r\n", m_spc3f.x, m_spc3f.y, m_spc3f.z );
	DbgOut( L"\tpower %f\r\n", m_power );
	DbgOut( L"\ttex %s\r\n", wtex );
	DbgOut( L"\talpha %s\r\n", walpha );
	DbgOut( L"\tbump %s\r\n", wbump );
	DbgOut( L"\r\n" );
	return 0;
}

int CMQOMaterial::IsSame( CMQOMaterial* compmat, int compnameflag, int cmplevel )
{
	//compnameflag == 0 �̎��́A���O�͔�r���Ȃ��B
	/***
	int materialno;
	char name[256];

	ARGBF col;
	float dif;
	float amb;
	float emi;
	float spc;
	float power;
	char tex[256];
	char alpha[256];
	char bump[256];
	***/

	int cmp;

	if( compnameflag ){
		cmp = strcmp( m_name, compmat->m_name );
		if( cmp )
			return 0;
	}
	
	if( m_shader != compmat->m_shader )
		return 0;

	if( (m_col.w != compmat->m_col.w) || (m_col.x != compmat->m_col.x) || (m_col.y != compmat->m_col.y) || (m_col.z != compmat->m_col.z) )
		return 0;

	if( m_dif != compmat->m_dif )
		return 0;

	if (m_amb != compmat->m_amb )
		return 0;

	if( m_emi != compmat->m_emi )
		return 0;

	if( m_spc != compmat->m_spc )
		return 0;

	if( m_power != compmat->m_power )
		return 0;

	if( *m_tex && *(compmat->m_tex) ){
		cmp = strcmp( m_tex, compmat->m_tex );
		if( cmp )
			return 0;
	}else{
		if( (*m_tex != 0) || (*(compmat->m_tex) != 0) )
			return 0;
	}

	if( *m_alpha && *(compmat->m_alpha) ){
		cmp = strcmp( m_alpha, compmat->m_alpha );
		if( cmp )
			return 0;
	}else{
		if( (*m_alpha != 0) || (*(compmat->m_alpha) != 0) )
			return 0;
	}

	if( *m_bump && *(compmat->m_bump) ){
		cmp = strcmp( m_bump, compmat->m_bump );
		if( cmp )
			return 0;
	}else{
		if( (*m_bump != 0) || (*(compmat->m_bump) != 0) )
			return 0;
	}

	if( cmplevel == 0 ){
		if( (m_dif4f.w != compmat->m_dif4f.w) || (m_dif4f.x != compmat->m_dif4f.x) || (m_dif4f.y != compmat->m_dif4f.y) || (m_dif4f.z != compmat->m_dif4f.z) ){
			return 0;
		}

		if( (m_amb3f.x != compmat->m_amb3f.x) || (m_amb3f.y != compmat->m_amb3f.y) || (m_amb3f.z != compmat->m_amb3f.z) ){
			return 0;
		}

		if( (m_emi3f.x != compmat->m_emi3f.x) || (m_emi3f.y != compmat->m_emi3f.y) || (m_emi3f.z != compmat->m_emi3f.z) ){
			return 0;
		}

		if( (m_spc3f.x != compmat->m_spc3f.x) || (m_spc3f.y != compmat->m_spc3f.y) || (m_spc3f.z != compmat->m_spc3f.z) ){
			return 0;
		}
	}


	return 1;
}

int CMQOMaterial::GetColorrefDiffuse( COLORREF* dstdiffuse )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( m_dif4f.x * 255.0f );
	tempg = (unsigned char)( m_dif4f.y * 255.0f );
	tempb = (unsigned char)( m_dif4f.z * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstdiffuse = RGB( tempr, tempg, tempb );

	return 0;
}
int CMQOMaterial::GetColorrefSpecular( COLORREF* dstspecular )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( m_spc3f.x * 255.0f );
	tempg = (unsigned char)( m_spc3f.y * 255.0f );
	tempb = (unsigned char)( m_spc3f.z * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstspecular = RGB( tempr, tempg, tempb );

	return 0;
}
int CMQOMaterial::GetColorrefAmbient( COLORREF* dstambient )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( m_amb3f.x * 255.0f );
	tempg = (unsigned char)( m_amb3f.y * 255.0f );
	tempb = (unsigned char)( m_amb3f.z * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstambient = RGB( tempr, tempg, tempb );

	return 0;
}
int CMQOMaterial::GetColorrefEmissive( COLORREF* dstemissive )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( m_emi3f.x * 255.0f );
	tempg = (unsigned char)( m_emi3f.y * 255.0f );
	tempb = (unsigned char)( m_emi3f.z * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstemissive = RGB( tempr, tempg, tempb );

	return 0;
}

int CMQOMaterial::CreateTexture( ID3D11DeviceContext* pd3dImmediateContext, WCHAR* dirname, int texpool )
{
	SetCurrentDirectory( dirname );
	
	WCHAR wname[256] = {0};

	if( m_tex[0] ){
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_tex, 256, wname, 256 );

		//g_texbank->AddTex( dirname, wname, m_transparent, texpool, 0, &m_texid );
		g_texbank->AddTex(pd3dImmediateContext, dirname, wname, m_transparent, texpool, &m_texid);
	}

	return 0;
}

int CMQOMaterial::AddConvName( char** ppname )
{
	*ppname = 0;

	m_convnamenum++;
	char** newconvname = 0;
	newconvname = (char**)realloc(m_ppconvname, sizeof(char*) * m_convnamenum);
	if( !newconvname ){
		DbgOut( L"mqomaterial : AddConvName : ppconvname alloc error !!!\r\n" );
		_ASSERT( 0 );
		return 1;
	}
	else {
		m_ppconvname = newconvname;
	}

	size_t leng;
	m_name[256 - 1] = 0;
	leng = strlen( m_name );
	if ((leng > 0) && (leng < 1024)) {
		char* newname = 0;
		newname = (char*)malloc(sizeof(char) * leng + 10);
		if (!newname) {
			DbgOut(L"mqomaterial : AddConvName : newname alloc error !!!\r\n");
			_ASSERT(0);
			return 1;
		}

		int addno;
		addno = m_convnamenum - 1;

		if (addno >= 1) {
			sprintf_s(newname, ((size_t)leng + 10), "%s%02d", m_name, addno);
		}
		else {
			strcpy_s(newname, ((size_t)leng + 10), m_name);
		}

		*(m_ppconvname + m_convnamenum - 1) = newname;
		*ppname = newname;

	}
	else {
		_ASSERT(0);
		*(m_ppconvname + m_convnamenum - 1) = 0;
		*ppname = 0;
		return 1;//error!!!!
	}



	return 0;
}

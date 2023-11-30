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
#include <TexElem.h>

#include <ChaVecCalc.h>
#include <Bone.h>
#include <mqoobject.h>
#include <Model.h>

#include <mqoobject.h>
#include <polymesh3.h>
#include <polymesh4.h>
#include <ExtLine.h>


//extern CTexBank* g_texbank;


#include "../../MiniEngine/ConstantBuffer.h"
#include "../../MiniEngine/RootSignature.h"
#include "../../MiniEngine/Material.h"




CMQOMaterial::CMQOMaterial() : m_descriptorHeap(),
m_commonConstantBuffer(), m_expandConstantBuffer() //2023/11/29
{
	InitParams();
}
CMQOMaterial::~CMQOMaterial()
{
	DestroyObjs();
}

int CMQOMaterial::SetParams( int srcno, ChaVector4 srcsceneamb, char* srcchar, int srcleng )
{
	//"肌色" col(1.000 0.792 0.651 1.000) dif(0.490) amb(0.540) emi(0.530) spc(0.020) power(0.00)


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
		"\"" //！！！tex, alpha, bumpよりも後でないとだめ。
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
	m_initpipelineflag = false;
	m_createdescriptorflag = false;
	//ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXCLUSTERNUM);
	ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXBONENUM);

	m_materialno = -1;
	ZeroMemory ( m_name, 256 );

	m_cbWithBone.Init();
	m_cbNoBone.Init();

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


	m_albedoMap = nullptr;
	m_normalMap = nullptr;//とりあえずnulltexture このクラスで作成するポインタ
	m_specularMap = nullptr;//とりあえずnulltexture このクラスで作成するポインタ

	return 0;
}

int CMQOMaterial::DestroyObjs()
{
	m_createdescriptorflag = false;

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

	//m_normalMap = nullptr;//とりあえずnulltexture このクラスで作成するポインタ
	if (m_normalMap) {
		delete m_normalMap;
		m_normalMap = nullptr;
	}

	//m_specularMap = nullptr;//とりあえずnulltexture このクラスで作成するポインタ
	if (m_specularMap) {
		delete m_specularMap;
		m_specularMap = nullptr;
	}

	//Texture* m_albedoMap;//bank管理の外部ポインタ
	m_albedoMap = nullptr;


	return 0;
}

int CMQOMaterial::SetName( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//"肌色"
	int step = 1;

	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"')

// "の後に)またはスペースが続いたときのみ終端とする。
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

	(*stepnum)++;//最後の）の分

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
		*stepnum = step + 2;//　")の分
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
		*stepnum = step + 2;//　")の分
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
		*stepnum = step + 2;//　")の分
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
	//compnameflag == 0 の時は、名前は比較しない。
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

int CMQOMaterial::CreateTexture( WCHAR* dirname, int texpool )
{
	SetCurrentDirectory( dirname );
	

	SetWhiteTexture();
	SetBlackTexture();
	SetDiffuseTexture();

	WCHAR wname[256] = {0};

	if( m_tex[0] ){
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_tex, 256, wname, 256 );

		//g_texbank->AddTex( dirname, wname, m_transparent, texpool, 0, &m_texid );
		g_texbank->AddTex(dirname, wname, m_transparent, texpool, &m_texid);

		CTexElem* findtex = g_texbank->GetTexElem(GetTexID());
		if(findtex){
			m_albedoMap = findtex->GetPTex();
		}else{
			_ASSERT(0);
			m_albedoMap = 0;
		}
	}
	else {
		m_albedoMap = 0;
	}


	//######################################################
	//normalMapとspecularMapは　とりあえずnulltextureでテスト
	//######################################################
	if (!m_specularMap)
	{
		const auto& nullTextureMaps = g_graphicsEngine->GetNullTextureMaps();
		char* map = nullptr;
		unsigned int mapSize;

		map = nullTextureMaps.GetNormalMap().get();
		mapSize = nullTextureMaps.GetNormalMapSize();
		Texture* texture = new Texture();
		if (!texture) {
			_ASSERT(0);
			return 1;
		}
		texture->InitFromMemory(map, mapSize);
		m_normalMap = texture;
	}
	
	if (!m_normalMap)
	{
		const auto& nullTextureMaps = g_graphicsEngine->GetNullTextureMaps();
		char* map = nullptr;
		unsigned int mapSize;

		map = nullTextureMaps.GetSpecularMap().get();
		mapSize = nullTextureMaps.GetSpecularMapSize();
		Texture* texture = new Texture();
		if (!texture) {
			_ASSERT(0);
			return 1;
		}
		texture->InitFromMemory(map, mapSize);
		m_specularMap = texture;
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


void CMQOMaterial::InitShadersAndPipelines(
	int vertextype,
	const char* fxFilePath,
	const char* vsEntryPointFunc,
	const char* vsSkinEntryPointFunc,
	const char* psEntryPointFunc,
	const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat,
	int numSrv,
	int numCbv,
	UINT offsetInDescriptorsFromTableStartCB,
	UINT offsetInDescriptorsFromTableStartSRV,
	D3D12_FILTER samplerFilter)
{

	//############################################
	// vertextype : 0-->pm4, 1-->pm3, 2-->extline
	//############################################



	if (m_initpipelineflag) {
		//###############################
		//既に初期化済の場合は　すぐにリターン
		//###############################
		return;
	}


	//テクスチャをロード。
	//InitTexture(tkmMat);

//定数バッファを作成。
	SMaterialParam matParam;
	//matParam.hasNormalMap = m_normalMap->IsValid() ? 1 : 0;
	//matParam.hasSpecMap = m_specularMap->IsValid() ? 1 : 0;
	matParam.hasNormalMap = 0;//まずはprimitive表示テストのため　NormalMapオフ !!!!!!!!!!!!!!!!
	matParam.hasSpecMap = 0;//まずはprimitive表示テストのため　SpecularMapオフ !!!!!!!!!!!!!!!!
	m_constantBuffer.Init(sizeof(SMaterialParam), &matParam);

	//ルートシグネチャを初期化。
	D3D12_STATIC_SAMPLER_DESC samplerDescArray[2];
	//デフォルトのサンプラ
	samplerDescArray[0].Filter = samplerFilter;
	samplerDescArray[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDescArray[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDescArray[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDescArray[0].MipLODBias = 0;
	samplerDescArray[0].MaxAnisotropy = 0;
	samplerDescArray[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDescArray[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDescArray[0].MinLOD = 0.0f;
	samplerDescArray[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDescArray[0].ShaderRegister = 0;
	samplerDescArray[0].RegisterSpace = 0;
	samplerDescArray[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//シャドウマップ用のサンプラ。
	samplerDescArray[1] = samplerDescArray[0];
	//比較対象の値が小さければ０、大きければ１を返す比較関数を設定する。
	samplerDescArray[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDescArray[1].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
	samplerDescArray[1].MaxAnisotropy = 1;
	samplerDescArray[1].ShaderRegister = 1;

	m_rootSignature.Init(
		samplerDescArray,
		2,
		numCbv,
		numSrv,
		8,
		offsetInDescriptorsFromTableStartCB,
		offsetInDescriptorsFromTableStartSRV
	);

	if (fxFilePath != nullptr && strlen(fxFilePath) > 0) {
		//シェーダーを初期化。
		InitShaders(fxFilePath, vsEntryPointFunc, vsSkinEntryPointFunc, psEntryPointFunc);
		//パイプラインステートを初期化。
		InitPipelineState(vertextype, colorBufferFormat);
	}

	m_initpipelineflag = true;
}

void CMQOMaterial::InitPipelineState(int vertextype, const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat)
{


	//############################################
	// vertextype : 0-->pm4, 1-->pm3, 2-->extline
	//############################################



	// 頂点レイアウトを定義する。
	//パイプラインステートを作成。
	D3D12_INPUT_ELEMENT_DESC inputElementDescsWithBone[] =
	{
		//型：PM3DISPV
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	D3D12_INPUT_ELEMENT_DESC inputElementDescsWithoutBone[] =
	{
		//型：PM3DISPV
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	D3D12_INPUT_ELEMENT_DESC inputElementDescsExtLine[] =
	{
		//型：ExtLine
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};



	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
		psoDesc.pRootSignature = m_rootSignature.Get();
		if (vertextype != 2) {
			psoDesc.InputLayout = { inputElementDescsWithBone, _countof(inputElementDescsWithBone) };//!!! WithBone
		}
		else {
			psoDesc.InputLayout = { inputElementDescsExtLine, _countof(inputElementDescsExtLine) };
		}
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vsSkinModel->GetCompiledBlob());//!!!!!!!!! Skin 
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_psModel->GetCompiledBlob());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		if (vertextype != 2) {
#ifdef SAMPLE_11
			// 背面を描画していないと影がおかしくなるため、
			// シャドウのサンプルのみカリングをオフにする。
			// 本来はアプリ側からカリングモードを渡すのがいいのだけど、
			// 書籍に記載しているコードに追記がいるので、エンジン側で吸収する。
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
#else
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
#endif
		}
		else {
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		}
		////2023/11/18
		//psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
#ifdef TK_ENABLE_ALPHA_TO_COVERAGE
		psoDesc.BlendState.AlphaToCoverageEnable = TRUE;
#endif
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		if (vertextype != 2) {
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		}
		else {
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		}

		int numRenderTarget = 0;
		for (auto& format : colorBufferFormat) {
			if (format == DXGI_FORMAT_UNKNOWN) {
				//フォーマットが指定されていない場所が来たら終わり。
				break;
			}
			psoDesc.RTVFormats[numRenderTarget] = colorBufferFormat[numRenderTarget];
			numRenderTarget++;
		}
		psoDesc.NumRenderTargets = numRenderTarget;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;

		m_skinModelPipelineState.Init(psoDesc);

		//続いて半透明マテリアル用。
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vsSkinModel->GetCompiledBlob());
		psoDesc.BlendState.IndependentBlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		m_transSkinModelPipelineState.Init(psoDesc);

	}

	//続いてスキンなしモデル用を作成。
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
		psoDesc.pRootSignature = m_rootSignature.Get();
		if (vertextype != 2) {
			//pm4, pm3
			psoDesc.InputLayout = { inputElementDescsWithoutBone, _countof(inputElementDescsWithoutBone) };//!!! WithoutBone
		}
		else {
			//extline
			psoDesc.InputLayout = { inputElementDescsExtLine, _countof(inputElementDescsExtLine) };//!!! ExtLIne
		}
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vsNonSkinModel->GetCompiledBlob());//!!!!!!!! NonSkin
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_psModel->GetCompiledBlob());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		if (vertextype != 2) {
#ifdef SAMPLE_11
			// 背面を描画していないと影がおかしくなるため、
			// シャドウのサンプルのみカリングをオフにする。
			// 本来はアプリ側からカリングモードを渡すのがいいのだけど、
			// 書籍に記載しているコードに追記がいるので、エンジン側で吸収する。
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
#else
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
#endif
		}
		else {
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		}

		////2023/11/18
		//psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
#ifdef TK_ENABLE_ALPHA_TO_COVERAGE
		psoDesc.BlendState.AlphaToCoverageEnable = TRUE;
#endif
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		if (vertextype != 2) {
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		}
		else {
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		}

		int numRenderTarget = 0;
		for (auto& format : colorBufferFormat) {
			if (format == DXGI_FORMAT_UNKNOWN) {
				//フォーマットが指定されていない場所が来たら終わり。
				break;
			}
			psoDesc.RTVFormats[numRenderTarget] = colorBufferFormat[numRenderTarget];
			numRenderTarget++;
		}
		psoDesc.NumRenderTargets = numRenderTarget;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;

		m_nonSkinModelPipelineState.Init(psoDesc);


		//続いて半透明マテリアル用。
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vsNonSkinModel->GetCompiledBlob());
		psoDesc.BlendState.IndependentBlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

		//2023/11/25 とりあえずpm3のwithalpha == true時の描画用にFUNC_ALWAYS設定
		//将来的にはm_transAlwaysNonSkinModelPipelineStateとして独立させる予定
		//psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		m_transNonSkinModelPipelineState.Init(psoDesc);
	}


}

void CMQOMaterial::InitShaders(
	const char* fxFilePath,
	const char* vsEntryPointFunc,
	const char* vsSkinEntriyPointFunc,
	const char* psEntryPointFunc
)
{
	//スキンなしモデル用のシェーダーをロードする。
	m_vsNonSkinModel = g_engine->GetShaderFromBank(fxFilePath, vsEntryPointFunc);
	if (m_vsNonSkinModel == nullptr) {
		m_vsNonSkinModel = new Shader;
		m_vsNonSkinModel->LoadVS(fxFilePath, vsEntryPointFunc);
		g_engine->RegistShaderToBank(fxFilePath, vsEntryPointFunc, m_vsNonSkinModel);
	}
	//スキンありモデル用のシェーダーをロードする。
	m_vsSkinModel = g_engine->GetShaderFromBank(fxFilePath, vsSkinEntriyPointFunc);
	if (m_vsSkinModel == nullptr) {
		m_vsSkinModel = new Shader;
		m_vsSkinModel->LoadVS(fxFilePath, vsSkinEntriyPointFunc);
		g_engine->RegistShaderToBank(fxFilePath, vsSkinEntriyPointFunc, m_vsSkinModel);
	}

	m_psModel = g_engine->GetShaderFromBank(fxFilePath, psEntryPointFunc);
	if (m_psModel == nullptr) {
		m_psModel = new Shader;
		m_psModel->LoadPS(fxFilePath, psEntryPointFunc);
		g_engine->RegistShaderToBank(fxFilePath, psEntryPointFunc, m_psModel);
	}
}

void CMQOMaterial::BeginRender(RenderContext& rc, int hasSkin, bool isline)
{
	rc.SetRootSignature(m_rootSignature);

	if (hasSkin) {
		//
		if (!isline) {
			rc.SetPipelineState(m_transSkinModelPipelineState);
		}
		else {
			rc.SetPipelineState(m_skinModelPipelineState);
		}	
	}
	else {
		//
		if (!isline) {
			rc.SetPipelineState(m_transNonSkinModelPipelineState);
		}
		else {
			rc.SetPipelineState(m_nonSkinModelPipelineState);
		}
	}

	rc.SetDescriptorHeap(m_descriptorHeap);

}

Texture& CMQOMaterial::GetDiffuseMap()
{
	return m_diffuseMap;
}

Texture& CMQOMaterial::GetAlbedoMap()
{
	if (m_albedoMap) {
		return *m_albedoMap;
	}
	else {
		return m_whitetex;
	}
}
Texture& CMQOMaterial::GetNormalMap()
{
	if (m_normalMap) {
		return *m_normalMap;
	}
	else {
		return m_blacktex;
	}
}
Texture& CMQOMaterial::GetSpecularMap()
{
	if (m_specularMap) {
		return *m_specularMap;
	}
	else {
		return m_blacktex;
	}
}

int CMQOMaterial::SetDiffuseTexture()
{
	m_diffuseMap.InitFromCustomColor(m_dif4f);
	return 0;
}

int CMQOMaterial::SetWhiteTexture()
{
	const auto& whiteTextureMaps = g_graphicsEngine->GetNullTextureMaps();
	char* map = nullptr;
	unsigned int mapSize;

	map = whiteTextureMaps.GetWhiteMap().get();
	mapSize = whiteTextureMaps.GetWhiteMapSize();
	m_whitetex.InitFromMemory(map, mapSize);

	return 0;
}
int CMQOMaterial::SetBlackTexture()
{
	const auto& blackTextureMaps = g_graphicsEngine->GetNullTextureMaps();
	char* map = nullptr;
	unsigned int mapSize;

	map = blackTextureMaps.GetReflectionMap().get();
	mapSize = blackTextureMaps.GetReflectionMapSize();
	m_blacktex.InitFromMemory(map, mapSize);
	return 0;
}



int CMQOMaterial::CreateDecl(ID3D12Device* pdev, int objecttype)
{
	//###########################################
	//vertextype : 0-->pm4, 1-->pm3, 2-->extline
	//###########################################

	if (m_createdescriptorflag) {
		//###############################
		//既に初期化済の場合は　すぐにリターン
		//###############################
		return 0;
	}



	//共通定数バッファの作成。
	if (objecttype == 0) {
		m_commonConstantBuffer.Init(sizeof(SConstantBufferWithBone), nullptr);
	}
	else {
		m_commonConstantBuffer.Init(sizeof(SConstantBufferNoBone), nullptr);
	}

	//ユーザー拡張用の定数バッファを作成。
	//if (expandData) {
	//	m_expandConstantBuffer.Init(expandDataSize, nullptr);
	//	m_expandData = expandData;
	//}
	//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
	//	m_expandShaderResourceView[i] = expandShaderResourceView[i];
	//}
	//int expandDataSize = 0;
	//m_expandConstantBuffer.Init(expandDataSize, nullptr);
	//m_expandData = nullptr;
	//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
	//	m_expandShaderResourceView[i] = nullptr;
	//}

	//2023/11/23
	// ボーンの姿勢は　メッシュ単位のSConstantBufferで　mWorld, View, Projと一緒に扱うことにした
	////2023/11/17
	////とりあえず　ボーン無しで表示テスト
	//int datanum = 1;
	//ChaMatrix dummymat;
	//dummymat.SetIdentity();
	//m_boneMatricesStructureBuffer.Init(
	//	sizeof(ChaMatrix),
	//	datanum,
	//	&dummymat
	//);


	//ディスクリプタヒープを作成。
	CreateDescriptorHeaps(objecttype);


	return 0;
}

void CMQOMaterial::CreateDescriptorHeaps(int objecttype)
{
	//###########################################
	//vertextype : 0-->pm4, 1-->pm3, 2-->extline
	//###########################################

	if (m_createdescriptorflag) {
		//###############################
		//既に初期化済の場合は　すぐにリターン
		//###############################
		return;
	}

	if (objecttype == 0) {
		//ディスクリプタヒープを構築していく。
		int srvNo = 0;
		int cbNo = 0;
		//ディスクリプタヒープにディスクリプタを登録していく。
		m_descriptorHeap.RegistShaderResource(srvNo, GetDiffuseMap());			//アルベドに乗算するテクスチャ。
		m_descriptorHeap.RegistShaderResource(srvNo + 1, GetAlbedoMap());			//アルベドマップ。
		m_descriptorHeap.RegistShaderResource(srvNo + 2, GetNormalMap());		//法線マップ。
		m_descriptorHeap.RegistShaderResource(srvNo + 3, GetSpecularMap());		//スペキュラマップ。
		//m_descriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
		for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			if (m_expandShaderResourceView[i]) {
				m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			}
		}
		srvNo += NUM_SRV_ONE_MATERIAL;
		m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);
		//if (m_expandConstantBuffer.IsValid()) {
		//	m_descriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);
		//}
		cbNo += NUM_CBV_ONE_MATERIAL;

		m_createdescriptorflag = true;

		if (m_createdescriptorflag) {
			m_descriptorHeap.Commit();
		}
	}
	else if (objecttype == 1) {
		//ディスクリプタヒープを構築していく。
		int srvNo = 0;
		int cbNo = 0;
		//ディスクリプタヒープにディスクリプタを登録していく。
		m_descriptorHeap.RegistShaderResource(srvNo, GetDiffuseMap());			//アルベドに乗算するテクスチャ。
		m_descriptorHeap.RegistShaderResource(srvNo + 1, GetAlbedoMap());			//アルベドマップ。
		m_descriptorHeap.RegistShaderResource(srvNo + 2, GetNormalMap());		//法線マップ。
		m_descriptorHeap.RegistShaderResource(srvNo + 3, GetSpecularMap());		//スペキュラマップ。
		//m_descriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
		for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			if (m_expandShaderResourceView[i]) {
				m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			}
		}
		srvNo += NUM_SRV_ONE_MATERIAL;
		m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);
		//if (m_expandConstantBuffer.IsValid()) {
		//	m_descriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);
		//}
		cbNo += NUM_CBV_ONE_MATERIAL;
		m_createdescriptorflag = true;

		//}
		if (m_createdescriptorflag) {
			m_descriptorHeap.Commit();
		}
	}
	else if (objecttype == 2) {
		//ディスクリプタヒープを構築していく。
		int srvNo = 0;
		int cbNo = 0;
		//ディスクリプタヒープにディスクリプタを登録していく。
		m_descriptorHeap.RegistShaderResource(srvNo, GetDiffuseMap());			//アルベドに乗算するテクスチャ。
		m_descriptorHeap.RegistShaderResource(srvNo + 1, GetAlbedoMap());			//アルベドマップ。
		m_descriptorHeap.RegistShaderResource(srvNo + 2, GetNormalMap());		//法線マップ。
		m_descriptorHeap.RegistShaderResource(srvNo + 3, GetSpecularMap());		//スペキュラマップ。
		//m_descriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
		for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			if (m_expandShaderResourceView[i]) {
				m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			}
		}
		srvNo += NUM_SRV_ONE_MATERIAL;
		m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);
		//if (m_expandConstantBuffer.IsValid()) {
		//	m_descriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);
		//}
		cbNo += NUM_CBV_ONE_MATERIAL;
		m_createdescriptorflag = true;

		if (m_createdescriptorflag) {
			m_descriptorHeap.Commit();
		}
	}
	else {

	}

}

void CMQOMaterial::SetFl4x4(myRenderer::RENDEROBJ renderobj)
{
	//ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXCLUSTERNUM);
	//ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXBONENUM);
	if (renderobj.pmodel && renderobj.mqoobj)
	{
		MOTINFO* curmi = 0;
		int curmotid;
		double curframe;
		curmi = renderobj.pmodel->GetCurMotInfo();

		if (renderobj.pmodel->GetTopBone() && (renderobj.pmodel->GetNoBoneFlag() == false) && curmi) {
			curmotid = curmi->motid;
			curframe = RoundingTime(curmi->curframe);

			if (curmotid > 0) {
				SetBoneMatrixReq(renderobj.pmodel->GetTopBone(false), renderobj);

				MoveMemory(&(m_cbWithBone.setfl4x4[0]), &(m_setfl4x4[0]), sizeof(float) * 16 * MAXBONENUM);
			}
		}
	}

}

void CMQOMaterial::DrawCommon(RenderContext& rc, myRenderer::RENDEROBJ renderobj,
	const Matrix& mView, const Matrix& mProj, bool isfirstmaterial)
{
	if (!renderobj.mqoobj) {
		_ASSERT(0);
		return;
	}

	CPolyMesh4* ppm4 = renderobj.mqoobj->GetPm4();
	CPolyMesh3* ppm3 = renderobj.mqoobj->GetPm3();
	CExtLine* pextline = renderobj.mqoobj->GetExtLine();
	CDispObj* pdispline = renderobj.mqoobj->GetDispLine();

	if (pdispline && pextline) {
		rc.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	else if (ppm3 || ppm4) {
		rc.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else {
		_ASSERT(0);
		return;
	}


	////定数バッファを更新する。
	if (pdispline && pextline) {
		//SConstantBufferNoBone cb;
		m_cbNoBone.mWorld = renderobj.mWorld;
		m_cbNoBone.mView = mView;
		m_cbNoBone.mProj = mProj;
		m_cbNoBone.diffusemult = pextline->GetColor();

		//ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXCLUSTERNUM);
		//ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXBONENUM);

		m_commonConstantBuffer.CopyToVRAM(m_cbNoBone);
	}
	else if (ppm3) {
		//SConstantBufferNoBone cb;
		m_cbNoBone.mWorld = renderobj.mWorld;
		m_cbNoBone.mView = mView;
		m_cbNoBone.mProj = mProj;
		if (renderobj.mqoobj && renderobj.mqoobj->GetTempDiffuseMultFlag()) {
			m_cbNoBone.diffusemult = renderobj.mqoobj->GetTempDiffuseMult();
		}
		else {
			m_cbNoBone.diffusemult = renderobj.diffusemult;
		}

		//ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXCLUSTERNUM);
		//ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXBONENUM);

		m_commonConstantBuffer.CopyToVRAM(m_cbNoBone);

	}
	else if (ppm4) {
		//SConstantBufferWithBone cb;
		m_cbWithBone.mWorld = renderobj.mWorld;
		m_cbWithBone.mView = mView;
		m_cbWithBone.mProj = mProj;
		if (ppm3 || ppm4) {
			if (renderobj.mqoobj && renderobj.mqoobj->GetTempDiffuseMultFlag()) {
				m_cbWithBone.diffusemult = renderobj.mqoobj->GetTempDiffuseMult();
			}
			else {
				m_cbWithBone.diffusemult = renderobj.diffusemult;
			}
		}
		else if (pextline) {
			m_cbWithBone.diffusemult = pextline->GetColor();
		}
		else {
			_ASSERT(0);
			m_cbWithBone.diffusemult = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		//if (isfirstmaterial) {
			SetFl4x4(renderobj);
		//}
		
		m_commonConstantBuffer.CopyToVRAM(m_cbWithBone);
	}


	//if (m_expandData) {
	//	m_expandConstantBuffer.CopyToVRAM(m_expandData);
	//}

	//if (m_boneMatricesStructureBuffer.IsInited()) {
	//	//ボーン行列を更新する。
	//	ChaMatrix dummymat;
	//	//m_boneMatricesStructureBuffer.Update(m_skeleton->GetBoneMatricesTopAddress());
	//	m_boneMatricesStructureBuffer.Update(&dummymat);
	//}
}

void CMQOMaterial::SetBoneMatrixReq(CBone* curbone, myRenderer::RENDEROBJ renderobj)
{
	if (curbone) {
		int matrixindex = curbone->GetMatrixIndex();//2023/11/30

		if (curbone->IsSkeleton() && (matrixindex >= 0) && (matrixindex < MAXBONENUM)) {
			ChaMatrix clustermat;
			clustermat.SetIdentity();

			bool currentlimitdegflag = g_limitdegflag;
			MOTINFO* curmi = 0;
			int curmotid;
			double curframe;
			curmi = renderobj.pmodel->GetCurMotInfo();
			if (renderobj.pmodel->GetTopBone() && (renderobj.pmodel->GetNoBoneFlag() == false) && curmi) {
				curmotid = curmi->motid;
				curframe = RoundingTime(curmi->curframe);
				if (curmotid > 0) {
					//CMotionPoint tmpmp = curbone->GetCurMp();
					
					CMotionPoint curmp = curbone->GetCurMp(renderobj.calcslotflag);

					if (renderobj.btflag == 0) {
						//set4x4[clcnt] = tmpmp.GetWorldMat();
						clustermat = curbone->GetWorldMat(currentlimitdegflag, curmotid, curframe, &curmp);
						MoveMemory(&(m_setfl4x4[16 * matrixindex]),//2023/11/30
							clustermat.GetDataPtr(), sizeof(float) * 16);
					}
					else if (renderobj.btflag == 1) {
						//物理シミュ
						//set4x4[clcnt] = curbone->GetBtMat();
						clustermat = curbone->GetBtMat(renderobj.calcslotflag);
						MoveMemory(&(m_setfl4x4[16 * matrixindex]),//2023/11/30
							clustermat.GetDataPtr(), sizeof(float) * 16);
					}
					else if (renderobj.btflag == 2) {
						//物理IK
						//set4x4[clcnt] = curbone->GetBtMat();
						clustermat = curbone->GetBtMat(renderobj.calcslotflag);
						MoveMemory(&(m_setfl4x4[16 * matrixindex]),//2023/11/30
							curbone->GetBtMat().GetDataPtr(), sizeof(float) * 16);
					}
					else {
						//set4x4[clcnt] = tmpmp.GetWorldMat();
						clustermat = curbone->GetWorldMat(currentlimitdegflag, curmotid, curframe, &curmp);
						MoveMemory(&(m_setfl4x4[16 * matrixindex]),//2023/11/30
							clustermat.GetDataPtr(), sizeof(float) * 16);
					}

				}
			}
		}

		if (curbone->GetChild(false)) {
			SetBoneMatrixReq(curbone->GetChild(false), renderobj);
		}
		if (curbone->GetBrother(false)) {
			SetBoneMatrixReq(curbone->GetBrother(false), renderobj);
		}
	}
}

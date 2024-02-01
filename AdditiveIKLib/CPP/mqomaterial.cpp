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
//#include "../../MiniEngine/Material.h"


static int s_alloccount = 0;

extern ChaVector4 g_lightdirforshader[LIGHTNUMMAX];
extern ChaVector4 g_lightdiffuseforshader[LIGHTNUMMAX];
extern IShaderResource* g_shadowmapforshader;



CMQOMaterial::CMQOMaterial() : m_descriptorHeap(), m_shadowdescriptorHeap(), //2023/12/14
m_commonConstantBuffer(), m_expandConstantBuffer(), //2023/11/29
m_expandConstantBuffer2(),//2023/12/10
m_shadowcommonConstantBuffer(), m_shadowexpandConstantBuffer(), m_shadowexpandConstantBuffer2(), //2023/12/14
m_whitetex(), m_blacktex(), m_diffuseMap(), 
m_cb(), m_cbMatrix(), m_cbLights(), m_cbShadow(), //2023/12/01 //2023/12/02 //2023/12/10
m_constantBuffer(), //2023/12/01
m_rootSignature(), //2023/12/01
m_shadowrootSignature(), //2023/12/14
m_ZPrerootSignature(), //2023/12/05
m_ZPreModelPipelineState(), //2023/12/05
m_InstancingrootSignature(), //2024/01/11
m_InstancingOpequePipelineState(),//2024/01/12
m_InstancingtransPipelineState(),//2024/01/12
m_InstancingzalwaysPipelineState()//2024/01/12
{
	InitParams();

	s_alloccount++;
	m_materialno = s_alloccount;
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
	m_initprezpipelineflag = false;
	m_initInstancingpipelineflag = false;
	m_createdescriptorflag = false;
	//ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXCLUSTERNUM);
	//ZeroMemory(m_setfl4x4, sizeof(float) * 16 * MAXBONENUM);
	
	m_shaderfx = -1;
	int fxno;
	for (fxno = 0; fxno < SHADERFX_MAX; fxno++) {
		m_updatefl4x4flag[fxno] = false;
		m_updatelightsflag[fxno] = false;
	}


	m_materialno = -1;
	ZeroMemory ( m_name, 256 );

	m_cb.Init();
	m_cbMatrix.Init();
	m_cbLights.Init();
	m_cbShadow.Init();

	m_col.w = 1.0f;
	m_col.x = 1.0f;
	m_col.y = 1.0f;
	m_col.z = 1.0f;

	m_dif = 1.0f;
	m_amb = 0.25f;
	m_emi = 0.0f;
	m_spc = 0.0f;
	m_power = 0.0f;

	m_enableEmission = false;
	m_emissiveScale = 1.0f;

	ZeroMemory ( m_tex, sizeof(char) * 256 );
	ZeroMemory ( m_alpha, sizeof(char) * 256 );
	ZeroMemory ( m_bump, sizeof(char) * 256 );

	ZeroMemory(m_albedotex, sizeof(char) * 256);
	ZeroMemory(m_normaltex, sizeof(char) * 256);
	ZeroMemory(m_metaltex, sizeof(char) * 256);

	int shaderindex;
	for (shaderindex = 0; shaderindex < MQOSHADER_MAX; shaderindex++) {
		m_opaquePipelineState[shaderindex].InitParams();
		m_transPipelineState[shaderindex].InitParams();
		m_zalwaysPipelineState[shaderindex].InitParams();
		m_vsMQOShader[shaderindex] = nullptr;
		m_psMQOShader[shaderindex] = nullptr;
	}


	//next = 0;



	m_shader = 3;//mqofile記述のshader

	m_shadertype = -2;//DirectX12描画用のshader //Shaderプレートメニュー用
	m_metalcoef = 0.250f;//Shaderプレートメニュー用
	m_smoothcoef = 0.250f;//Shaderプレートメニュー用
	int litno;
	for (litno = 0; litno < LIGHTNUMMAX; litno++) {
		m_lightscale[litno] = 1.0f;//Shaderプレートメニュー用
	}


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

	m_albedotexid = -1;
	m_normaltexid = -1;
	m_metaltexid = -1;

	m_convnamenum = 0;
	m_ppconvname = 0;


	m_albedoMap = nullptr;//bank管理の外部ポインタ
	m_normalMap = nullptr;//bank管理の外部ポインタ
	m_metalMap = nullptr;//bank管理の外部ポインタ

	m_settempdiffusemult = false;
	m_tempdiffusemult = ChaVector4(1.0f, 1.0f, 1.0f, 1.0f);


	m_addressU_albedo = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_addressV_albedo = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_addressU_normal = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_addressV_normal = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_addressU_metal = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_addressV_metal = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

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


	//bank管理の外部ポインタ
	m_albedoMap = nullptr;

	//bank管理の外部ポインタ
	//if (m_normalMap) {
	//	delete m_normalMap;
	//	m_normalMap = nullptr;
	//}
	m_normalMap = nullptr;

	//bank管理の外部ポインタ
	//if (m_specularMap) {
	//	delete m_specularMap;
	//	m_specularMap = nullptr;
	//}
	m_metalMap = nullptr;


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


	//if (m_emi != 0.0f) {
	//	int dbgflag1 = 1;
	//}

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

	if(m_albedotex[0]){
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_albedotex, 256, wname, 256 );

		//g_texbank->AddTex( dirname, wname, m_transparent, texpool, 0, &m_texid );
		g_texbank->AddTex(dirname, wname, m_transparent, texpool, &m_albedotexid);

		CTexElem* findtex = g_texbank->GetTexElem(GetAlbedoTexID());
		if(findtex){
			m_albedoMap = findtex->GetPTex();
		}else{
			_ASSERT(0);
			m_albedoMap = 0;
		}
	}
	else {
		//texture名にalbedoが付いていなかった場合用　最初に見つかったテクスチャをalbedoとして使う
		if (m_tex[0]) {
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_tex, 256, wname, 256);

			//g_texbank->AddTex( dirname, wname, m_transparent, texpool, 0, &m_texid );
			g_texbank->AddTex(dirname, wname, m_transparent, texpool, &m_albedotexid);

			CTexElem* findtex = g_texbank->GetTexElem(GetAlbedoTexID());
			if (findtex) {
				m_albedoMap = findtex->GetPTex();
				SetAlbedoTex(m_tex);
			}
			else {
				_ASSERT(0);
				m_albedoMap = 0;
			}
		}
		else {
			m_albedoMap = 0;
		}
	}


	if (m_normaltex[0]) {
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_normaltex, 256, wname, 256);

		//g_texbank->AddTex( dirname, wname, m_transparent, texpool, 0, &m_texid );
		g_texbank->AddTex(dirname, wname, m_transparent, texpool, &m_normaltexid);

		CTexElem* findtex = g_texbank->GetTexElem(GetNormalTexID());
		if (findtex) {
			m_normalMap = findtex->GetPTex();
		}
		else {
			_ASSERT(0);
			m_normalMap = 0;
		}
	}
	else {
		m_normalMap = 0;
	}

	if (m_metaltex[0]) {
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_metaltex, 256, wname, 256);

		//g_texbank->AddTex( dirname, wname, m_transparent, texpool, 0, &m_texid );
		g_texbank->AddTex(dirname, wname, m_transparent, texpool, &m_metaltexid);

		CTexElem* findtex = g_texbank->GetTexElem(GetMetalTexID());
		if (findtex) {
			m_metalMap = findtex->GetPTex();
		}
		else {
			_ASSERT(0);
			m_metalMap = 0;
		}
	}
	else {
		m_metalMap = 0;
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

void CMQOMaterial::InitZPreShadersAndPipelines(
	int vertextype,
	const char* fxFilePath,
	const char* vsEntryPointFunc,
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

	if ((vertextype != 0) && (vertextype != 1)) {
		return;
	}

	if (m_initprezpipelineflag) {
	//if (m_initpipelineflag) {
		//###############################
		//既に初期化済の場合は　すぐにリターン
		//###############################
		return;
	}


	//ルートシグネチャを初期化。
	D3D12_STATIC_SAMPLER_DESC samplerDescArray[2];
	//デフォルトのサンプラ
	samplerDescArray[0].Filter = samplerFilter;
	samplerDescArray[0].AddressU = GetAddressU_albedo();//2024/01/06
	samplerDescArray[0].AddressV = GetAddressV_albedo();//2024/01/06
	samplerDescArray[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescArray[0].MipLODBias = 0;
	samplerDescArray[0].MaxAnisotropy = 0;
	samplerDescArray[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDescArray[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDescArray[0].MinLOD = 0.0f;
	samplerDescArray[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDescArray[0].ShaderRegister = 0;//!!!!!!!!!
	samplerDescArray[0].RegisterSpace = 0;
	samplerDescArray[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//シャドウマップ用のサンプラ。
	samplerDescArray[1] = samplerDescArray[0];
	//比較対象の値が小さければ０、大きければ１を返す比較関数を設定する。
	samplerDescArray[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDescArray[1].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
	samplerDescArray[1].MaxAnisotropy = 1;
	samplerDescArray[1].ShaderRegister = 1;//!!!!!!!!!

	m_ZPrerootSignature.Init(
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
		InitZPreShaders(fxFilePath, vsEntryPointFunc, psEntryPointFunc);
		//パイプラインステートを初期化。
		InitZPrePipelineState(vertextype, colorBufferFormat);
	}

	m_initprezpipelineflag = true;
}


void CMQOMaterial::InitInstancingShadersAndPipelines(
	int vertextype,
	const char* fxFilePath,
	const char* vsEntryPointFunc,
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

	if ((vertextype != 0) && (vertextype != 1)) {
		return;
	}

	if (m_initInstancingpipelineflag) {
		//if (m_initpipelineflag) {
			//###############################
			//既に初期化済の場合は　すぐにリターン
			//###############################
		return;
	}


	//ルートシグネチャを初期化。
	D3D12_STATIC_SAMPLER_DESC samplerDescArray[2];
	//デフォルトのサンプラ
	samplerDescArray[0].Filter = samplerFilter;
	samplerDescArray[0].AddressU = GetAddressU_albedo();//2024/01/06
	samplerDescArray[0].AddressV = GetAddressV_albedo();//2024/01/06
	samplerDescArray[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescArray[0].MipLODBias = 0;
	samplerDescArray[0].MaxAnisotropy = 0;
	samplerDescArray[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDescArray[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDescArray[0].MinLOD = 0.0f;
	samplerDescArray[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDescArray[0].ShaderRegister = 0;//!!!!!!!!!
	samplerDescArray[0].RegisterSpace = 0;
	samplerDescArray[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//シャドウマップ用のサンプラ。
	samplerDescArray[1] = samplerDescArray[0];
	//比較対象の値が小さければ０、大きければ１を返す比較関数を設定する。
	samplerDescArray[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDescArray[1].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
	samplerDescArray[1].MaxAnisotropy = 1;
	samplerDescArray[1].ShaderRegister = 1;//!!!!!!!!!

	m_InstancingrootSignature.Init(
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
		InitInstancingShaders(fxFilePath, vsEntryPointFunc, psEntryPointFunc);
		//パイプラインステートを初期化。
		InitInstancingPipelineState(vertextype, colorBufferFormat);
	}

	m_initInstancingpipelineflag = true;
}


void CMQOMaterial::InitShadersAndPipelines(
	int srcuvnum,
	int vertextype,
	const char* fxPBRPath,
	const char* fxStdPath,
	const char* fxNoLightPath,

	const char* vsPBRFunc,
	const char* vsPBRShadowMapFunc,
	const char* vsPBRShadowRecieverFunc,

	const char* vsStdFunc,
	const char* vsStdShadowMapFunc,
	const char* vsStdShadowRecieverFunc,

	const char* vsNoLightFunc,
	const char* vsNoLightShadowMapFunc,
	const char* vsNoLightShadowRecieverFunc,

	const char* psPBRFunc,
	const char* psPBRShadowMapFunc,
	const char* psPBRShadowRecieverFunc,

	const char* psStdFunc,
	const char* psStdShadowMapFunc,
	const char* psStdShadowRecieverFunc,

	const char* psNoLightFunc,
	const char* psNoLightShadowMapFunc,
	const char* psNoLightShadowRecieverFunc,

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

	if (!fxPBRPath || !fxStdPath || !fxNoLightPath ||

		!vsPBRFunc || !vsStdFunc || !vsNoLightFunc ||
		!vsPBRShadowMapFunc || !vsStdShadowMapFunc || !vsNoLightShadowMapFunc ||
		!vsPBRShadowRecieverFunc || !vsStdShadowRecieverFunc || !vsNoLightShadowRecieverFunc ||

		!psPBRFunc || !psStdFunc || !psNoLightFunc ||
		!psPBRShadowMapFunc || !psStdShadowMapFunc || !psNoLightShadowMapFunc ||
		!psPBRShadowRecieverFunc || !psStdShadowRecieverFunc || !psNoLightShadowRecieverFunc) {
		
		_ASSERT(0);
		abort();
		return;
	}

	if ((strlen(fxPBRPath) <= 0) || (strlen(fxStdPath) <= 0) || (strlen(fxNoLightPath) <= 0) ||

		(strlen(vsPBRFunc) <= 0) || (strlen(vsStdFunc) <= 0) || (strlen(vsNoLightFunc) <= 0) ||
		(strlen(vsPBRShadowMapFunc) <= 0) || (strlen(vsStdShadowMapFunc) <= 0) || (strlen(vsNoLightShadowMapFunc) <= 0) ||
		(strlen(vsPBRShadowRecieverFunc) <= 0) || (strlen(vsStdShadowRecieverFunc) <= 0) || (strlen(vsNoLightShadowRecieverFunc) <= 0) ||

		(strlen(psPBRFunc) <= 0) || (strlen(psStdFunc) <= 0) || (strlen(psNoLightFunc) <= 0) ||
		(strlen(psPBRShadowMapFunc) <= 0) || (strlen(psStdShadowMapFunc) <= 0) || (strlen(psNoLightShadowMapFunc) <= 0) ||
		(strlen(psPBRShadowRecieverFunc) <= 0) || (strlen(psStdShadowRecieverFunc) <= 0) || (strlen(psNoLightShadowRecieverFunc) <= 0)) {
		
		_ASSERT(0);
		abort();
		return;
	}

	if (m_initpipelineflag) {
		//###############################
		//既に初期化済の場合は　すぐにリターン
		//###############################
		return;
	}

	//ルートシグネチャを初期化。
	D3D12_STATIC_SAMPLER_DESC samplerDescArray[5];
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
	samplerDescArray[0].ShaderRegister = 0;//!!!!!!!!
	samplerDescArray[0].RegisterSpace = 0;
	samplerDescArray[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	samplerDescArray[1] = samplerDescArray[0];
	samplerDescArray[1].ShaderRegister = 1;//!!!!!!!!
	samplerDescArray[1].AddressU = GetAddressU_albedo();//2024/01/06
	samplerDescArray[1].AddressV = GetAddressV_albedo();//2024/01/06

	samplerDescArray[2] = samplerDescArray[0];
	samplerDescArray[2].ShaderRegister = 2;//!!!!!!!!
	if (srcuvnum >= 2) {
		samplerDescArray[2].AddressU = GetAddressU_normal();//2024/01/06
		samplerDescArray[2].AddressV = GetAddressV_normal();//2024/01/06
	}
	else {
		samplerDescArray[2].AddressU = GetAddressU_albedo();//2024/01/06
		samplerDescArray[2].AddressV = GetAddressV_albedo();//2024/01/06
	}
	//samplerDescArray[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	//samplerDescArray[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	//samplerDescArray[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	samplerDescArray[3] = samplerDescArray[0];
	samplerDescArray[3].ShaderRegister = 3;//!!!!!!!!
	if (srcuvnum >= 2) {
		samplerDescArray[3].AddressU = GetAddressU_metal();//2024/01/06
		samplerDescArray[3].AddressV = GetAddressV_metal();//2024/01/06
	}
	else {
		samplerDescArray[3].AddressU = GetAddressU_albedo();//2024/01/06
		samplerDescArray[3].AddressV = GetAddressV_albedo();//2024/01/06
	}
	//シャドウマップ用のサンプラ。
	samplerDescArray[4] = samplerDescArray[0];
	samplerDescArray[4].ShaderRegister = 4;//!!!!!!!!
	//比較対象の値が小さければ０、大きければ１を返す比較関数を設定する。
	samplerDescArray[4].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDescArray[4].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
	samplerDescArray[4].MaxAnisotropy = 1;






	m_rootSignature.Init(
		samplerDescArray,
		5,
		numCbv,
		numSrv,
		8,
		offsetInDescriptorsFromTableStartCB,
		offsetInDescriptorsFromTableStartSRV
	);

	m_shadowrootSignature.Init(
		samplerDescArray,
		5,
		numCbv,
		numSrv,
		8,
		offsetInDescriptorsFromTableStartCB,
		offsetInDescriptorsFromTableStartSRV
	);


	//シェーダーを初期化。
	InitShaders(
		fxPBRPath,
		fxStdPath,
		fxNoLightPath,

		vsPBRFunc,
		vsPBRShadowMapFunc,
		vsPBRShadowRecieverFunc,

		vsStdFunc,
		vsStdShadowMapFunc,
		vsStdShadowRecieverFunc,

		vsNoLightFunc,
		vsNoLightShadowMapFunc,
		vsNoLightShadowRecieverFunc,

		psPBRFunc,
		psPBRShadowMapFunc,
		psPBRShadowRecieverFunc,

		psStdFunc,
		psStdShadowMapFunc,
		psStdShadowRecieverFunc,

		psNoLightFunc,
		psNoLightShadowMapFunc,
		psNoLightShadowRecieverFunc
	);

	//パイプラインステートを初期化。
	InitPipelineState(vertextype, colorBufferFormat);

	m_initpipelineflag = true;
}


void CMQOMaterial::InitPipelineState(int vertextype, const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat)
{

	int shaderindex0;
	for (shaderindex0 = 0; shaderindex0 < MQOSHADER_MAX; shaderindex0++) {
		if (!m_vsMQOShader[shaderindex0] || !m_psMQOShader[shaderindex0]) {
			_ASSERT(0);
			abort();
			return;
		}
	}

	//############################################
	// vertextype : 0-->pm4, 1-->pm3, 2-->extline
	//############################################



	// 頂点レイアウトを定義する。
	//パイプラインステートを作成。
	D3D12_INPUT_ELEMENT_DESC inputElementDescsWithBone[] =
	{
		//型：BINORMALDISPV + PM3INF
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
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
		//型：BINORMALDISPV
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
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




	int shaderindex;
	for (shaderindex = 0; shaderindex < MQOSHADER_MAX; shaderindex++)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
		if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
			(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
			(shaderindex == MQOSHADER_NOLIGHT_SHADOWMAP)) {
			psoDesc.pRootSignature = m_shadowrootSignature.Get();
		}
		else {
			psoDesc.pRootSignature = m_rootSignature.Get();
		}

		//2023/12/01 シェーダのエントリ関数として　skin有無を切り替えることはあるが　頂点フォーマットはvertextypeによって決める
		if (vertextype == 0) {//pm4
			psoDesc.InputLayout = { inputElementDescsWithBone, _countof(inputElementDescsWithBone) };//!!! WithBone
		}
		else if (vertextype == 1) {//pm3
			psoDesc.InputLayout = { inputElementDescsWithoutBone, _countof(inputElementDescsWithoutBone) };
		}
		else if (vertextype == 2) {//extline
			psoDesc.InputLayout = { inputElementDescsExtLine, _countof(inputElementDescsExtLine) };
		}
		else {
			_ASSERT(0);
			abort();
		}
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vsMQOShader[shaderindex]->GetCompiledBlob());//!!!!!!!!! Skin 
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_psMQOShader[shaderindex]->GetCompiledBlob());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		if (vertextype != 2) {
//#ifdef SAMPLE_11
//			// 背面を描画していないと影がおかしくなるため、
//			// シャドウのサンプルのみカリングをオフにする。
//			// 本来はアプリ側からカリングモードを渡すのがいいのだけど、
//			// 書籍に記載しているコードに追記がいるので、エンジン側で吸収する。
//			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
//#else
//			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
//#endif
			if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
				(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
				(shaderindex == MQOSHADER_NOLIGHT_SHADOWMAP)) {

				psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
			}
			else {
				psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
			}
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
		if (
			(shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
			(shaderindex == MQOSHADER_NOLIGHT_SHADOWMAP) ||
			(shaderindex == MQOSHADER_STD_SHADOWMAP)
			) {
			//2023/12/11 ShadowMap
			//psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32_FLOAT;
			numRenderTarget = 1;
		}
		//else if (
		//	(shaderindex == MQOSHADER_PBR_SHADOWRECIEVER) ||
		//	(shaderindex == MQOSHADER_NOLIGHT_SHADOWRECIEVER) ||
		//	(shaderindex == MQOSHADER_STD_SHADOWRECIEVER)
		//	) {

		//}
		else {
			for (auto& format : colorBufferFormat) {
				if (format == DXGI_FORMAT_UNKNOWN) {
					//フォーマットが指定されていない場所が来たら終わり。
					break;
				}
				psoDesc.RTVFormats[numRenderTarget] = colorBufferFormat[numRenderTarget];
				numRenderTarget++;
			}
		}
		psoDesc.NumRenderTargets = numRenderTarget;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;

		m_opaquePipelineState[shaderindex].Init(psoDesc);


		//続いて半透明マテリアル用。
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vsMQOShader[shaderindex]->GetCompiledBlob());
		psoDesc.BlendState.IndependentBlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		//psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		m_transPipelineState[shaderindex].Init(psoDesc);

		////2023/12/01
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		m_zalwaysPipelineState[shaderindex].Init(psoDesc);
	}
}

void CMQOMaterial::InitZPrePipelineState(int vertextype, const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat)
{


	//############################################
	// vertextype : 0-->pm4, 1-->pm3, 2-->extline
	//############################################

	if ((vertextype != 0) && (vertextype != 1)) {
		return;
	}

	// 頂点レイアウトを定義する。
	//パイプラインステートを作成。
	D3D12_INPUT_ELEMENT_DESC inputElementDescsWithBone[] =
	{
		//型：BINORMALDISPV + PM3INF
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
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
		//型：BINORMALDISPV
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
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
		psoDesc.pRootSignature = m_ZPrerootSignature.Get();

		//2023/12/01 シェーダのエントリ関数として　skin有無を切り替えることはあるが　頂点フォーマットはvertextypeによって決める
		if (vertextype == 0) {//pm4
			psoDesc.InputLayout = { inputElementDescsWithBone, _countof(inputElementDescsWithBone) };//!!! WithBone
		}
		else if (vertextype == 1) {//pm3
			psoDesc.InputLayout = { inputElementDescsWithoutBone, _countof(inputElementDescsWithoutBone) };
		}
		//else if (vertextype == 2) {//extline
		//	psoDesc.InputLayout = { inputElementDescsExtLine, _countof(inputElementDescsExtLine) };
		//}
		else {
			_ASSERT(0);
			abort();
		}
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vsZPreModel->GetCompiledBlob());//!!!!!!!!! Skin 
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_psZPreModel->GetCompiledBlob());
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

		m_ZPreModelPipelineState.Init(psoDesc);


	}

}

void CMQOMaterial::InitInstancingPipelineState(int vertextype, const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat)
{


	//############################################
	// vertextype : 0-->pm4, 1-->pm3, 2-->extline
	//############################################

	if ((vertextype != 0) && (vertextype != 1)) {
		return;
	}

	// 頂点レイアウトを定義する。
	//パイプラインステートを作成。
	D3D12_INPUT_ELEMENT_DESC inputElementDescsWithoutBone[] =
	{
		//型：BINORMALDISPV
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		//wmat : texcoord0 - texcoord4
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },

		//vpmat : texcoord5 - texcoord8
		{ "TEXCOORD", 5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 6, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 7, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 8, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },

		//diffusemult
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },

		{ "TEXCOORD", 9, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 10, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};


	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
		psoDesc.pRootSignature = m_InstancingrootSignature.Get();

		//2023/12/01 シェーダのエントリ関数として　skin有無を切り替えることはあるが　頂点フォーマットはvertextypeによって決める
		if (vertextype == 0) {//pm4
			//psoDesc.InputLayout = { inputElementDescsWithBone, _countof(inputElementDescsWithBone) };//!!! WithBone

			_ASSERT(0);//not support
			abort();
		}
		else if (vertextype == 1) {//pm3
			psoDesc.InputLayout = { inputElementDescsWithoutBone, _countof(inputElementDescsWithoutBone) };
		}
		//else if (vertextype == 2) {//extline
		//	psoDesc.InputLayout = { inputElementDescsExtLine, _countof(inputElementDescsExtLine) };
		//}
		else {
			_ASSERT(0);
			abort();
		}
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vsInstancingModel->GetCompiledBlob());//!!!!!!!!! Skin 
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_psInstancingModel->GetCompiledBlob());
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
		//if (vertextype != 2) {
		//	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//}
		//else {
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		//}

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

		m_InstancingOpequePipelineState.Init(psoDesc);


		//続いて半透明マテリアル用。
		psoDesc.BlendState.IndependentBlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		//psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		m_InstancingtransPipelineState.Init(psoDesc);

		////2023/12/01
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		m_InstancingzalwaysPipelineState.Init(psoDesc);

	}
}

void CMQOMaterial::InitShaders(
	const char* fxPBRPath,
	const char* fxStdPath,
	const char* fxNoLightPath,

	const char* vsPBRFunc,
	const char* vsPBRShadowMapFunc,
	const char* vsPBRShadowRecieverFunc,

	const char* vsStdFunc,
	const char* vsStdShadowMapFunc,
	const char* vsStdShadowRecieverFunc,

	const char* vsNoLightFunc,
	const char* vsNoLightShadowMapFunc,
	const char* vsNoLightShadowRecieverFunc,

	const char* psPBRFunc,
	const char* psPBRShadowMapFunc,
	const char* psPBRShadowRecieverFunc,

	const char* psStdFunc,
	const char* psStdShadowMapFunc,
	const char* psStdShadowRecieverFunc,

	const char* psNoLightFunc,
	const char* psNoLightShadowMapFunc,
	const char* psNoLightShadowRecieverFunc
	)
{
//###########
//PBRシェーダ
//###########
	m_vsMQOShader[MQOSHADER_PBR] = g_engine->GetShaderFromBank(fxPBRPath, vsPBRFunc);
	if (m_vsMQOShader[MQOSHADER_PBR] == nullptr) {
		m_vsMQOShader[MQOSHADER_PBR] = new Shader;
		m_vsMQOShader[MQOSHADER_PBR]->LoadVS(fxPBRPath, vsPBRFunc);
		g_engine->RegistShaderToBank(fxPBRPath, vsPBRFunc, m_vsMQOShader[MQOSHADER_PBR]);
	}
	m_psMQOShader[MQOSHADER_PBR] = g_engine->GetShaderFromBank(fxPBRPath, psPBRFunc);
	if (m_psMQOShader[MQOSHADER_PBR] == nullptr) {
		m_psMQOShader[MQOSHADER_PBR] = new Shader;
		m_psMQOShader[MQOSHADER_PBR]->LoadPS(fxPBRPath, psPBRFunc);
		g_engine->RegistShaderToBank(fxPBRPath, psPBRFunc, m_psMQOShader[MQOSHADER_PBR]);
	}

	m_vsMQOShader[MQOSHADER_PBR_SHADOWMAP] = g_engine->GetShaderFromBank(fxPBRPath, vsPBRShadowMapFunc);
	if (m_vsMQOShader[MQOSHADER_PBR_SHADOWMAP] == nullptr) {
		m_vsMQOShader[MQOSHADER_PBR_SHADOWMAP] = new Shader;
		m_vsMQOShader[MQOSHADER_PBR_SHADOWMAP]->LoadVS(fxPBRPath, vsPBRShadowMapFunc);
		g_engine->RegistShaderToBank(fxPBRPath, vsPBRShadowMapFunc, m_vsMQOShader[MQOSHADER_PBR_SHADOWMAP]);
	}
	m_psMQOShader[MQOSHADER_PBR_SHADOWMAP] = g_engine->GetShaderFromBank(fxPBRPath, psPBRShadowMapFunc);
	if (m_psMQOShader[MQOSHADER_PBR_SHADOWMAP] == nullptr) {
		m_psMQOShader[MQOSHADER_PBR_SHADOWMAP] = new Shader;
		m_psMQOShader[MQOSHADER_PBR_SHADOWMAP]->LoadPS(fxPBRPath, psPBRShadowMapFunc);
		g_engine->RegistShaderToBank(fxPBRPath, psPBRShadowMapFunc, m_psMQOShader[MQOSHADER_PBR_SHADOWMAP]);
	}

	m_vsMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxPBRPath, vsPBRShadowRecieverFunc);
	if (m_vsMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] == nullptr) {
		m_vsMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] = new Shader;
		m_vsMQOShader[MQOSHADER_PBR_SHADOWRECIEVER]->LoadVS(fxPBRPath, vsPBRShadowRecieverFunc);
		g_engine->RegistShaderToBank(fxPBRPath, vsPBRShadowRecieverFunc, m_vsMQOShader[MQOSHADER_PBR_SHADOWRECIEVER]);
	}
	m_psMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxPBRPath, psPBRShadowRecieverFunc);
	if (m_psMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] == nullptr) {
		m_psMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] = new Shader;
		m_psMQOShader[MQOSHADER_PBR_SHADOWRECIEVER]->LoadPS(fxPBRPath, psPBRShadowRecieverFunc);
		g_engine->RegistShaderToBank(fxPBRPath, psPBRShadowRecieverFunc, m_psMQOShader[MQOSHADER_PBR_SHADOWRECIEVER]);
	}

//################
//Standardシェーダ
//################
	m_vsMQOShader[MQOSHADER_STD] = g_engine->GetShaderFromBank(fxStdPath, vsStdFunc);
	if (m_vsMQOShader[MQOSHADER_STD] == nullptr) {
		m_vsMQOShader[MQOSHADER_STD] = new Shader;
		m_vsMQOShader[MQOSHADER_STD]->LoadVS(fxStdPath, vsStdFunc);
		g_engine->RegistShaderToBank(fxStdPath, vsStdFunc, m_vsMQOShader[MQOSHADER_STD]);
	}
	m_psMQOShader[MQOSHADER_STD] = g_engine->GetShaderFromBank(fxStdPath, psStdFunc);
	if (m_psMQOShader[MQOSHADER_STD] == nullptr) {
		m_psMQOShader[MQOSHADER_STD] = new Shader;
		m_psMQOShader[MQOSHADER_STD]->LoadPS(fxStdPath, psStdFunc);
		g_engine->RegistShaderToBank(fxStdPath, psStdFunc, m_psMQOShader[MQOSHADER_STD]);
	}

	m_vsMQOShader[MQOSHADER_STD_SHADOWMAP] = g_engine->GetShaderFromBank(fxStdPath, vsStdShadowMapFunc);
	if (m_vsMQOShader[MQOSHADER_STD_SHADOWMAP] == nullptr) {
		m_vsMQOShader[MQOSHADER_STD_SHADOWMAP] = new Shader;
		m_vsMQOShader[MQOSHADER_STD_SHADOWMAP]->LoadVS(fxStdPath, vsStdShadowMapFunc);
		g_engine->RegistShaderToBank(fxStdPath, vsStdShadowMapFunc, m_vsMQOShader[MQOSHADER_STD_SHADOWMAP]);
	}
	m_psMQOShader[MQOSHADER_STD_SHADOWMAP] = g_engine->GetShaderFromBank(fxStdPath, psStdShadowMapFunc);
	if (m_psMQOShader[MQOSHADER_STD_SHADOWMAP] == nullptr) {
		m_psMQOShader[MQOSHADER_STD_SHADOWMAP] = new Shader;
		m_psMQOShader[MQOSHADER_STD_SHADOWMAP]->LoadPS(fxStdPath, psStdShadowMapFunc);
		g_engine->RegistShaderToBank(fxStdPath, psStdShadowMapFunc, m_psMQOShader[MQOSHADER_STD_SHADOWMAP]);
	}

	m_vsMQOShader[MQOSHADER_STD_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxStdPath, vsStdShadowRecieverFunc);
	if (m_vsMQOShader[MQOSHADER_STD_SHADOWRECIEVER] == nullptr) {
		m_vsMQOShader[MQOSHADER_STD_SHADOWRECIEVER] = new Shader;
		m_vsMQOShader[MQOSHADER_STD_SHADOWRECIEVER]->LoadVS(fxStdPath, vsStdShadowRecieverFunc);
		g_engine->RegistShaderToBank(fxStdPath, vsStdShadowRecieverFunc, m_vsMQOShader[MQOSHADER_STD_SHADOWRECIEVER]);
	}
	m_psMQOShader[MQOSHADER_STD_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxStdPath, psStdShadowRecieverFunc);
	if (m_psMQOShader[MQOSHADER_STD_SHADOWRECIEVER] == nullptr) {
		m_psMQOShader[MQOSHADER_STD_SHADOWRECIEVER] = new Shader;
		m_psMQOShader[MQOSHADER_STD_SHADOWRECIEVER]->LoadPS(fxStdPath, psStdShadowRecieverFunc);
		g_engine->RegistShaderToBank(fxStdPath, psStdShadowRecieverFunc, m_psMQOShader[MQOSHADER_STD_SHADOWRECIEVER]);
	}

//###############
//NoLightシェーダ
//###############
	m_vsMQOShader[MQOSHADER_NOLIGHT] = g_engine->GetShaderFromBank(fxNoLightPath, vsNoLightFunc);
	if (m_vsMQOShader[MQOSHADER_NOLIGHT] == nullptr) {
		m_vsMQOShader[MQOSHADER_NOLIGHT] = new Shader;
		m_vsMQOShader[MQOSHADER_NOLIGHT]->LoadVS(fxNoLightPath, vsNoLightFunc);
		g_engine->RegistShaderToBank(fxNoLightPath, vsNoLightFunc, m_vsMQOShader[MQOSHADER_NOLIGHT]);
	}
	m_psMQOShader[MQOSHADER_NOLIGHT] = g_engine->GetShaderFromBank(fxNoLightPath, psNoLightFunc);
	if (m_psMQOShader[MQOSHADER_NOLIGHT] == nullptr) {
		m_psMQOShader[MQOSHADER_NOLIGHT] = new Shader;
		m_psMQOShader[MQOSHADER_NOLIGHT]->LoadPS(fxNoLightPath, psNoLightFunc);
		g_engine->RegistShaderToBank(fxNoLightPath, psNoLightFunc, m_psMQOShader[MQOSHADER_NOLIGHT]);
	}

	m_vsMQOShader[MQOSHADER_NOLIGHT_SHADOWMAP] = g_engine->GetShaderFromBank(fxNoLightPath, vsNoLightShadowMapFunc);
	if (m_vsMQOShader[MQOSHADER_NOLIGHT_SHADOWMAP] == nullptr) {
		m_vsMQOShader[MQOSHADER_NOLIGHT_SHADOWMAP] = new Shader;
		m_vsMQOShader[MQOSHADER_NOLIGHT_SHADOWMAP]->LoadVS(fxNoLightPath, vsNoLightShadowMapFunc);
		g_engine->RegistShaderToBank(fxNoLightPath, vsNoLightShadowMapFunc, m_vsMQOShader[MQOSHADER_NOLIGHT_SHADOWMAP]);
	}
	m_psMQOShader[MQOSHADER_NOLIGHT_SHADOWMAP] = g_engine->GetShaderFromBank(fxNoLightPath, psNoLightShadowMapFunc);
	if (m_psMQOShader[MQOSHADER_NOLIGHT_SHADOWMAP] == nullptr) {
		m_psMQOShader[MQOSHADER_NOLIGHT_SHADOWMAP] = new Shader;
		m_psMQOShader[MQOSHADER_NOLIGHT_SHADOWMAP]->LoadPS(fxNoLightPath, psNoLightShadowMapFunc);
		g_engine->RegistShaderToBank(fxNoLightPath, psNoLightShadowMapFunc, m_psMQOShader[MQOSHADER_NOLIGHT_SHADOWMAP]);
	}

	m_vsMQOShader[MQOSHADER_NOLIGHT_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxNoLightPath, vsNoLightShadowRecieverFunc);
	if (m_vsMQOShader[MQOSHADER_NOLIGHT_SHADOWRECIEVER] == nullptr) {
		m_vsMQOShader[MQOSHADER_NOLIGHT_SHADOWRECIEVER] = new Shader;
		m_vsMQOShader[MQOSHADER_NOLIGHT_SHADOWRECIEVER]->LoadVS(fxNoLightPath, vsNoLightShadowRecieverFunc);
		g_engine->RegistShaderToBank(fxNoLightPath, vsNoLightShadowRecieverFunc, m_vsMQOShader[MQOSHADER_NOLIGHT_SHADOWRECIEVER]);
	}
	m_psMQOShader[MQOSHADER_NOLIGHT_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxNoLightPath, psNoLightShadowRecieverFunc);
	if (m_psMQOShader[MQOSHADER_NOLIGHT_SHADOWRECIEVER] == nullptr) {
		m_psMQOShader[MQOSHADER_NOLIGHT_SHADOWRECIEVER] = new Shader;
		m_psMQOShader[MQOSHADER_NOLIGHT_SHADOWRECIEVER]->LoadPS(fxNoLightPath, psNoLightShadowRecieverFunc);
		g_engine->RegistShaderToBank(fxNoLightPath, psNoLightShadowRecieverFunc, m_psMQOShader[MQOSHADER_NOLIGHT_SHADOWRECIEVER]);
	}

}

void CMQOMaterial::InitZPreShaders(
	const char* fxFilePath,
	const char* vsEntryPointFunc,
	const char* psEntryPointFunc
)
{
	//スキンなしモデル用のシェーダーをロードする。
	m_vsZPreModel = g_engine->GetShaderFromBank(fxFilePath, vsEntryPointFunc);
	if (m_vsZPreModel == nullptr) {
		m_vsZPreModel = new Shader;
		m_vsZPreModel->LoadVS(fxFilePath, vsEntryPointFunc);
		g_engine->RegistShaderToBank(fxFilePath, vsEntryPointFunc, m_vsZPreModel);
	}

	m_psZPreModel = g_engine->GetShaderFromBank(fxFilePath, psEntryPointFunc);
	if (m_psZPreModel == nullptr) {
		m_psZPreModel = new Shader;
		m_psZPreModel->LoadPS(fxFilePath, psEntryPointFunc);
		g_engine->RegistShaderToBank(fxFilePath, psEntryPointFunc, m_psZPreModel);
	}
}

void CMQOMaterial::InitInstancingShaders(
	const char* fxFilePath,
	const char* vsEntryPointFunc,
	const char* psEntryPointFunc
)
{
	//スキンなしモデル用のシェーダーをロードする。
	m_vsInstancingModel = g_engine->GetShaderFromBank(fxFilePath, vsEntryPointFunc);
	if (m_vsInstancingModel == nullptr) {
		m_vsInstancingModel = new Shader;
		m_vsInstancingModel->LoadVS(fxFilePath, vsEntryPointFunc);
		g_engine->RegistShaderToBank(fxFilePath, vsEntryPointFunc, m_vsInstancingModel);
	}

	m_psInstancingModel = g_engine->GetShaderFromBank(fxFilePath, psEntryPointFunc);
	if (m_psInstancingModel == nullptr) {
		m_psInstancingModel = new Shader;
		m_psInstancingModel->LoadPS(fxFilePath, psEntryPointFunc);
		g_engine->RegistShaderToBank(fxFilePath, psEntryPointFunc, m_psInstancingModel);
	}
}

void CMQOMaterial::BeginRender(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
{
	if (!rc || !renderobj.pmodel || !renderobj.mqoobj) {
		_ASSERT(0);
		return;
	}


	bool withalpha = renderobj.forcewithalpha || renderobj.withalpha;
	CPolyMesh3* pm3 = nullptr;
	CPolyMesh4* pm4 = nullptr;
	pm3 = renderobj.mqoobj->GetPm3();
	pm4 = renderobj.mqoobj->GetPm4();

	//#####################################################################################################
	//2023/12/05
	//renderobj.shadertype : MQOSHADER_PBR, MQOSHADER_STD, MOQSHADER_NOLIGHTで指定されたようにパイプラインを設定
	//-1指定の場合には　テクスチャの設定具合をみて自動で設定
	//テクスチャが１つも無い場合について迷ったが　STDで描画するとPBRの中で明るすぎて非常に浮く　テクスチャ無しもPBRで描画することに
	//#####################################################################################################

	int tempshadertype;
	if (renderobj.shadertype == -2) {//shadertype == -2の場合はマテリアルの設定に従う
		tempshadertype = GetShaderType();
	}
	else {
		tempshadertype = renderobj.shadertype;
	}

	int shadertype;
	switch(tempshadertype) {
	case -1:
	case -2://マテリアルの設定も-2だった場合にはAUTOとみなす
		if (renderobj.pmodel->GetVRoidJointName()) {
			//VRoid特有のジョイント名を含み　シェーダタイプがAUTOの場合にはNOLIGHTでトゥーン風味に
			//NoLight
			shadertype = MQOSHADER_NOLIGHT;
		}
		else {
			if ((GetAlbedoTex() && !(GetAlbedoTex())[0]) ||
				(GetNormalTex() && (GetNormalTex())[0]) ||
				(GetMetalTex() && (GetMetalTex())[0])) {
				shadertype = MQOSHADER_PBR;
			}
			else {
				if (pm4) {
					//NoLight
					shadertype = MQOSHADER_NOLIGHT;
				}
				else {
					//Standard
					shadertype = MQOSHADER_STD;
				}
			}
		}
		break;
	case MQOSHADER_PBR:
	case MQOSHADER_STD:
	case MQOSHADER_NOLIGHT:
		shadertype = tempshadertype;
		break;
	default:
		_ASSERT(0);
		shadertype = MQOSHADER_NOLIGHT;
		break;
	}


	int shaderindex = -1;
	switch (shadertype) {
	case MQOSHADER_PBR:
		switch (renderobj.renderkind) {//renderobj.renderkindはCDispObj::Render*()関数内でセット
		case RENDERKIND_NORMAL:
			shaderindex = MQOSHADER_PBR;
			break;
		case RENDERKIND_SHADOWMAP:
			shaderindex = MQOSHADER_PBR_SHADOWMAP;
			break;
		case RENDERKIND_SHADOWRECIEVER:
			shaderindex = MQOSHADER_PBR_SHADOWRECIEVER;
			break;
		case RENDERKIND_ZPREPASS://ZPrepassの場合にはこの関数は呼ばれないはず
		case RENDERKIND_INSTANCING://Instancingの場合にはこの関数は呼ばれないはず
		default:
			shaderindex = -1;
			break;
		}
		break;
	case MQOSHADER_STD:
		switch (renderobj.renderkind) {//renderobj.renderkindはCDispObj::Render*()関数内でセット
		case RENDERKIND_NORMAL:
			shaderindex = MQOSHADER_STD;
			break;
		case RENDERKIND_SHADOWMAP:
			shaderindex = MQOSHADER_STD_SHADOWMAP;
			break;
		case RENDERKIND_SHADOWRECIEVER:
			shaderindex = MQOSHADER_STD_SHADOWRECIEVER;
			break;
		case RENDERKIND_ZPREPASS://ZPrepassの場合にはこの関数は呼ばれないはず
		case RENDERKIND_INSTANCING://Instancingの場合にはこの関数は呼ばれないはず
		default:
			shaderindex = -1;
			break;
		}
		break;
	case MQOSHADER_NOLIGHT:
		switch (renderobj.renderkind) {//renderobj.renderkindはCDispObj::Render*()関数内でセット
		case RENDERKIND_NORMAL:
			shaderindex = MQOSHADER_NOLIGHT;
			break;
		case RENDERKIND_SHADOWMAP:
			shaderindex = MQOSHADER_NOLIGHT_SHADOWMAP;
			break;
		case RENDERKIND_SHADOWRECIEVER:
			shaderindex = MQOSHADER_NOLIGHT_SHADOWRECIEVER;
			break;
		case RENDERKIND_ZPREPASS://ZPrepassの場合にはこの関数は呼ばれないはず
		case RENDERKIND_INSTANCING://Instancingの場合にはこの関数は呼ばれないはず
		default:
			shaderindex = -1;
			break;
		}
		break;
	default:
		break;
	}


	if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
		(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
		(shaderindex == MQOSHADER_NOLIGHT_SHADOWMAP)) {
		rc->SetRootSignature(m_shadowrootSignature);
	}
	else {
		rc->SetRootSignature(m_rootSignature);
	}


	if (shaderindex >= 0) {
		if (withalpha) {
			if (renderobj.zcmpalways) {
				//###########################
				//Z cmp Always 半透明常に上書き
				//###########################
				rc->SetPipelineState(m_zalwaysPipelineState[shaderindex]);
			}
			else {
				//###################
				//translucent　半透明
				//###################
				rc->SetPipelineState(m_transPipelineState[shaderindex]);
			}
		}
		else {
			//##############
			//Opaque 不透明
			//##############
			rc->SetPipelineState(m_opaquePipelineState[shaderindex]);
		}
	}
	else {
		rc->SetPipelineState(m_opaquePipelineState[MQOSHADER_NOLIGHT]);
	}

	if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
		(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
		(shaderindex == MQOSHADER_NOLIGHT_SHADOWMAP)) {
		rc->SetDescriptorHeap(m_shadowdescriptorHeap);
	}
	else {
		rc->SetDescriptorHeap(m_descriptorHeap);
	}
}

void CMQOMaterial::ZPreBeginRender(RenderContext* rc)
{
	if (!rc) {
		_ASSERT(0);
		return;
	}
	rc->SetRootSignature(m_ZPrerootSignature);
	rc->SetPipelineState(m_ZPreModelPipelineState);
	rc->SetDescriptorHeap(m_descriptorHeap);
}

void CMQOMaterial::InstancingBeginRender(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
{
	if (!rc) {
		_ASSERT(0);
		return;
	}

	bool withalpha = renderobj.forcewithalpha || renderobj.withalpha;


	rc->SetRootSignature(m_InstancingrootSignature);

	if (withalpha) {
		if (renderobj.zcmpalways) {
			//###########################
			//Z cmp Always 半透明常に上書き
			//###########################
			rc->SetPipelineState(m_InstancingzalwaysPipelineState);
		}
		else {
			//###################
			//translucent　半透明
			//###################
			rc->SetPipelineState(m_InstancingtransPipelineState);
		}
	}
	else {
		//##############
		//Opaque 不透明
		//##############
		rc->SetPipelineState(m_InstancingOpequePipelineState);
	}

	rc->SetDescriptorHeap(m_descriptorHeap);


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
		//return m_whitetex;
	}
}
Texture& CMQOMaterial::GetMetalMap()
{
	if (m_metalMap) {
		return *m_metalMap;
	}
	else {
		return m_blacktex;
		//return m_whitetex;
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
		//####
		//pm4
		//####
		EXPAND_SRV_REG__START_NO = 5;
		NUM_SRV_ONE_MATERIAL = (EXPAND_SRV_REG__START_NO + MAX_MODEL_EXPAND_SRV);
		NUM_CBV_ONE_MATERIAL = 3;
		m_commonConstantBuffer.Init(sizeof(SConstantBuffer), nullptr);
		m_expandConstantBuffer.Init(sizeof(SConstantBufferBoneMatrix), nullptr);
		m_expandConstantBuffer2.Init(sizeof(SConstantBufferShadow), nullptr);
		m_shadowcommonConstantBuffer.Init(sizeof(SConstantBuffer), nullptr);
		m_shadowexpandConstantBuffer.Init(sizeof(SConstantBufferBoneMatrix), nullptr);
		m_shadowexpandConstantBuffer2.Init(sizeof(SConstantBufferShadow), nullptr);
	}
	else if (objecttype == 1){
		//#############
		//pm3
		//#############
		EXPAND_SRV_REG__START_NO = 5;
		NUM_SRV_ONE_MATERIAL = (EXPAND_SRV_REG__START_NO + MAX_MODEL_EXPAND_SRV);
		NUM_CBV_ONE_MATERIAL = 3;
		m_commonConstantBuffer.Init(sizeof(SConstantBuffer), nullptr);
		m_expandConstantBuffer.Init(sizeof(SConstantBufferLights), nullptr);
		m_expandConstantBuffer2.Init(sizeof(SConstantBufferShadow), nullptr);
		m_shadowcommonConstantBuffer.Init(sizeof(SConstantBuffer), nullptr);
		m_shadowexpandConstantBuffer.Init(sizeof(SConstantBufferLights), nullptr);
		m_shadowexpandConstantBuffer2.Init(sizeof(SConstantBufferShadow), nullptr);
	}
	else {
		//#############
		//extline
		//#############
		EXPAND_SRV_REG__START_NO = 5;
		NUM_SRV_ONE_MATERIAL = (EXPAND_SRV_REG__START_NO + MAX_MODEL_EXPAND_SRV);
		NUM_CBV_ONE_MATERIAL = 1;
		m_commonConstantBuffer.Init(sizeof(SConstantBuffer), nullptr);
		m_shadowcommonConstantBuffer.Init(sizeof(SConstantBuffer), nullptr);
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
		{
			int srvNo = 0;
			int cbNo = 0;
			//ディスクリプタヒープにディスクリプタを登録していく。
			m_descriptorHeap.RegistShaderResource(srvNo, GetDiffuseMap());			//アルベドに乗算するテクスチャ。
			m_descriptorHeap.RegistShaderResource(srvNo + 1, GetAlbedoMap());			//アルベドマップ。
			m_descriptorHeap.RegistShaderResource(srvNo + 2, GetNormalMap());		//法線マップ。
			m_descriptorHeap.RegistShaderResource(srvNo + 3, GetMetalMap());		//Metalマップ。
			m_descriptorHeap.RegistShaderResource(srvNo + 4, *g_shadowmapforshader);//Shadowマップ。
			//m_descriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
			//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			//	if (m_expandShaderResourceView[i]) {
			//		m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			//	}
			//}
			srvNo += NUM_SRV_ONE_MATERIAL;
			m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);
			if (m_expandConstantBuffer.IsValid()) {
				m_descriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);//BoneMatrix
			}
			if (m_expandConstantBuffer2.IsValid()) {
				m_descriptorHeap.RegistConstantBuffer(cbNo + 2, m_expandConstantBuffer2);//Shadow
			}
			cbNo += NUM_CBV_ONE_MATERIAL;

			m_descriptorHeap.Commit();
		}

		{
			int srvNo = 0;
			int cbNo = 0;
			//ディスクリプタヒープにディスクリプタを登録していく。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo, GetDiffuseMap());			//アルベドに乗算するテクスチャ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 1, GetAlbedoMap());			//アルベドマップ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 2, GetNormalMap());		//法線マップ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 3, GetMetalMap());		//Metalマップ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 4, *g_shadowmapforshader);//Shadowマップ。
			//m_shadowdescriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
			//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			//	if (m_expandShaderResourceView[i]) {
			//		m_shadowdescriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			//	}
			//}
			srvNo += NUM_SRV_ONE_MATERIAL;
			m_shadowdescriptorHeap.RegistConstantBuffer(cbNo, m_shadowcommonConstantBuffer);
			if (m_expandConstantBuffer.IsValid()) {
				m_shadowdescriptorHeap.RegistConstantBuffer(cbNo + 1, m_shadowexpandConstantBuffer);//BoneMatrix
			}
			if (m_expandConstantBuffer2.IsValid()) {
				m_shadowdescriptorHeap.RegistConstantBuffer(cbNo + 2, m_shadowexpandConstantBuffer2);//Shadow
			}
			cbNo += NUM_CBV_ONE_MATERIAL;

			m_createdescriptorflag = true;

			m_shadowdescriptorHeap.Commit();
		}

		m_createdescriptorflag = true;

	}
	else if (objecttype == 1) {
		//ディスクリプタヒープを構築していく。
		{
			int srvNo = 0;
			int cbNo = 0;
			//ディスクリプタヒープにディスクリプタを登録していく。
			m_descriptorHeap.RegistShaderResource(srvNo, GetDiffuseMap());			//アルベドに乗算するテクスチャ。
			m_descriptorHeap.RegistShaderResource(srvNo + 1, GetAlbedoMap());			//アルベドマップ。
			m_descriptorHeap.RegistShaderResource(srvNo + 2, GetNormalMap());		//法線マップ。
			m_descriptorHeap.RegistShaderResource(srvNo + 3, GetMetalMap());		//Metalマップ。
			m_descriptorHeap.RegistShaderResource(srvNo + 4, *g_shadowmapforshader);//Shadowマップ。
			//m_descriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
			//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			//	if (m_expandShaderResourceView[i]) {
			//		m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			//	}
			//}
			srvNo += NUM_SRV_ONE_MATERIAL;
			m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);
			if (m_expandConstantBuffer.IsValid()) {
				m_descriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);
			}
			if (m_expandConstantBuffer2.IsValid()) {
				m_descriptorHeap.RegistConstantBuffer(cbNo + 2, m_expandConstantBuffer2);
			}
			cbNo += NUM_CBV_ONE_MATERIAL;

			m_descriptorHeap.Commit();
		}

		{
			int srvNo = 0;
			int cbNo = 0;
			//ディスクリプタヒープにディスクリプタを登録していく。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo, GetDiffuseMap());			//アルベドに乗算するテクスチャ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 1, GetAlbedoMap());			//アルベドマップ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 2, GetNormalMap());		//法線マップ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 3, GetMetalMap());		//Metalマップ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 4, *g_shadowmapforshader);//Shadowマップ。
			//m_shadowdescriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
			//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			//	if (m_expandShaderResourceView[i]) {
			//		m_shadowdescriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			//	}
			//}
			srvNo += NUM_SRV_ONE_MATERIAL;
			m_shadowdescriptorHeap.RegistConstantBuffer(cbNo, m_shadowcommonConstantBuffer);
			if (m_expandConstantBuffer.IsValid()) {
				m_shadowdescriptorHeap.RegistConstantBuffer(cbNo + 1, m_shadowexpandConstantBuffer);
			}
			if (m_expandConstantBuffer2.IsValid()) {
				m_shadowdescriptorHeap.RegistConstantBuffer(cbNo + 2, m_shadowexpandConstantBuffer2);
			}
			cbNo += NUM_CBV_ONE_MATERIAL;

			m_shadowdescriptorHeap.Commit();
		}

		m_createdescriptorflag = true;

	}
	else if (objecttype == 2) {
		//ディスクリプタヒープを構築していく。
		{
			int srvNo = 0;
			int cbNo = 0;
			//ディスクリプタヒープにディスクリプタを登録していく。
			m_descriptorHeap.RegistShaderResource(srvNo, GetDiffuseMap());			//アルベドに乗算するテクスチャ。
			m_descriptorHeap.RegistShaderResource(srvNo + 1, GetAlbedoMap());			//アルベドマップ。
			m_descriptorHeap.RegistShaderResource(srvNo + 2, GetNormalMap());		//法線マップ。
			m_descriptorHeap.RegistShaderResource(srvNo + 3, GetMetalMap());		//Metalマップ。
			m_descriptorHeap.RegistShaderResource(srvNo + 4, *g_shadowmapforshader);//Shadowマップ。
			//m_descriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
			//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			//	if (m_expandShaderResourceView[i]) {
			//		m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			//	}
			//}
			srvNo += NUM_SRV_ONE_MATERIAL;
			m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);
			//if (m_expandConstantBuffer.IsValid()) {
			//	m_descriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);
			//}
			cbNo += NUM_CBV_ONE_MATERIAL;

			m_descriptorHeap.Commit();
		}

		{
			int srvNo = 0;
			int cbNo = 0;
			//ディスクリプタヒープにディスクリプタを登録していく。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo, GetDiffuseMap());			//アルベドに乗算するテクスチャ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 1, GetAlbedoMap());			//アルベドマップ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 2, GetNormalMap());		//法線マップ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 3, GetMetalMap());		//Metalマップ。
			m_shadowdescriptorHeap.RegistShaderResource(srvNo + 4, *g_shadowmapforshader);//Shadowマップ。
			//m_shadowdescriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
			//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			//	if (m_expandShaderResourceView[i]) {
			//		m_shadowdescriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			//	}
			//}
			srvNo += NUM_SRV_ONE_MATERIAL;
			m_shadowdescriptorHeap.RegistConstantBuffer(cbNo, m_shadowcommonConstantBuffer);
			//if (m_expandConstantBuffer.IsValid()) {
			//	m_shadowdescriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);
			//}
			cbNo += NUM_CBV_ONE_MATERIAL;

			m_shadowdescriptorHeap.Commit();
		}

		m_createdescriptorflag = true;


	}
	else {

	}

}

void CMQOMaterial::SetConstShadow(SConstantBufferShadow* pcbShadow)
{
	if (!pcbShadow) {
		_ASSERT(0);
		return;
	}
	pcbShadow->Init();

	g_cameraShadow->Update();
	Vector3 lpos = g_cameraShadow->GetPosition();
	pcbShadow->lightPos = ChaVector4(lpos.x, lpos.y, lpos.z, 1.0f);
	//ChaMatrix lvp = ChaMatrix(g_cameraShadow->GetViewProjectionMatrix());
	ChaMatrix mView = g_cameraShadow->GetViewMatrix(false);
	ChaMatrix mProj = g_cameraShadow->GetProjectionMatrix();
	ChaMatrix mVP = mView * mProj;
	MoveMemory(&(pcbShadow->mLVP),
		mVP.GetDataPtr(), sizeof(float) * 16);
}


void CMQOMaterial::SetConstLights(SConstantBufferLights* pcbLights)
{
	if (!pcbLights) {
		_ASSERT(0);
		return;
	}
	pcbLights->Init();
	pcbLights->lightsnum[0] = g_nNumActiveLights;
	int lightno;
	for (lightno = 0; lightno < g_nNumActiveLights; lightno++) {//2023/12/17必要分だけ詰めて受け渡し
	//for (lightno = 0; lightno < LIGHTNUMMAX; lightno++) {
		pcbLights->directionalLight[lightno].color = 
			g_lightdiffuseforshader[lightno] * GetLightScale(g_lightNo[lightno]);//2023/12/08 materialのlightscaleを掛けるように.
		pcbLights->directionalLight[lightno].direction = g_lightdirforshader[lightno];
	}	
	//ChaVector3 cameye = ChaVector3(g_camera3D->GetPosition());
	//ChaVector3 camtarget = ChaVector3(g_camera3D->GetTarget());
	//m_cbLights.directionalLight[0].direction = ChaVector4((camtarget - cameye), 0.0f);//この向きで合っている
	pcbLights->eyePos = ChaVector4(ChaVector3(g_camera3D->GetPosition()), 0.0f);
	pcbLights->specPow = ChaVector4(5.0f, 5.0f, 5.0f, 0.0f);
}


void CMQOMaterial::SetFl4x4(myRenderer::RENDEROBJ renderobj)
{
	if (renderobj.pmodel && renderobj.mqoobj)
	{
		MOTINFO* curmi = 0;
		int curmotid;
		//double curframe;
		curmi = renderobj.pmodel->GetCurMotInfo();

		if (renderobj.pmodel->GetTopBone() && (renderobj.pmodel->GetNoBoneFlag() == false) && curmi) {
			curmotid = curmi->motid;
			if (curmotid > 0) {
				SetBoneMatrix(renderobj);//CModel::SetShaderConst()でセットしたマトリックス配列をコピーするだけ
			}
		}
	}
}

void CMQOMaterial::DrawCommon(RenderContext* rc, myRenderer::RENDEROBJ renderobj,
	const Matrix& mView, const Matrix& mProj,
	bool isfirstmaterial)
{
	if (!rc || !renderobj.mqoobj || !renderobj.pmodel) {
		_ASSERT(0);
		return;
	}

	bool withalpha = renderobj.forcewithalpha || renderobj.withalpha;
	CPolyMesh4* ppm4 = renderobj.mqoobj->GetPm4();
	CPolyMesh3* ppm3 = renderobj.mqoobj->GetPm3();
	CExtLine* pextline = renderobj.mqoobj->GetExtLine();
	CDispObj* pdispline = renderobj.mqoobj->GetDispLine();

	//if (pdispline && pextline) {
	//	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	//}
	//else if (ppm3 || ppm4) {
	//	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//}
	//else {
	//	_ASSERT(0);
	//	return;
	//}


	int tempshadertype;
	if (renderobj.shadertype == -2) {//shadertype == -2の場合はマテリアルの設定に従う
		tempshadertype = GetShaderType();
	}
	else {
		tempshadertype = renderobj.shadertype;
	}

	int shadertype;
	switch (tempshadertype) {
	case -1:
	case -2://マテリアルの設定も-2だった場合にはAUTOとみなす
		if (renderobj.pmodel->GetVRoidJointName()) {
			//VRoid特有のジョイント名を含み　シェーダタイプがAUTOの場合にはNOLIGHTでトゥーン風味に
			//NoLight
			shadertype = MQOSHADER_NOLIGHT;
		}
		else{
			if ((GetAlbedoTex() && !(GetAlbedoTex())[0]) ||
				(GetNormalTex() && (GetNormalTex())[0]) ||
				(GetMetalTex() && (GetMetalTex())[0])) {
				shadertype = MQOSHADER_PBR;
			}
			else {
				if (ppm4) {
					//NoLight
					shadertype = MQOSHADER_NOLIGHT;
				}
				else {
					//Standard
					shadertype = MQOSHADER_STD;
				}
			}
		}
		break;
	case MQOSHADER_PBR:
	case MQOSHADER_STD:
	case MQOSHADER_NOLIGHT:
		shadertype = tempshadertype;
		break;
	default:
		_ASSERT(0);
		shadertype = MQOSHADER_NOLIGHT;
		break;
	}


	int shaderindex = -1;
	switch (shadertype) {
	case MQOSHADER_PBR:
		switch (renderobj.renderkind) {//renderobj.renderkindはCDispObj::Render*()関数内でセット
		case RENDERKIND_NORMAL:
			shaderindex = MQOSHADER_PBR;
			break;
		case RENDERKIND_SHADOWMAP:
			shaderindex = MQOSHADER_PBR_SHADOWMAP;
			break;
		case RENDERKIND_SHADOWRECIEVER:
			shaderindex = MQOSHADER_PBR_SHADOWRECIEVER;
			break;
		case RENDERKIND_ZPREPASS://ZPrepassの場合にはこの関数は呼ばれないはず
		case RENDERKIND_INSTANCING://Instancingの場合にはこの関数は呼ばれないはず
		default:
			shaderindex = -1;
			break;
		}
		break;
	case MQOSHADER_STD:
		switch (renderobj.renderkind) {//renderobj.renderkindはCDispObj::Render*()関数内でセット
		case RENDERKIND_NORMAL:
			shaderindex = MQOSHADER_STD;
			break;
		case RENDERKIND_SHADOWMAP:
			shaderindex = MQOSHADER_STD_SHADOWMAP;
			break;
		case RENDERKIND_SHADOWRECIEVER:
			shaderindex = MQOSHADER_STD_SHADOWRECIEVER;
			break;
		case RENDERKIND_ZPREPASS://ZPrepassの場合にはこの関数は呼ばれないはず
		case RENDERKIND_INSTANCING://Instancingの場合にはこの関数は呼ばれないはず
		default:
			shaderindex = -1;
			break;
		}
		break;
	case MQOSHADER_NOLIGHT:
		switch (renderobj.renderkind) {//renderobj.renderkindはCDispObj::Render*()関数内でセット
		case RENDERKIND_NORMAL:
			shaderindex = MQOSHADER_NOLIGHT;
			break;
		case RENDERKIND_SHADOWMAP:
			shaderindex = MQOSHADER_NOLIGHT_SHADOWMAP;
			break;
		case RENDERKIND_SHADOWRECIEVER:
			shaderindex = MQOSHADER_NOLIGHT_SHADOWRECIEVER;
			break;
		case RENDERKIND_ZPREPASS://ZPrepassの場合にはこの関数は呼ばれないはず
		case RENDERKIND_INSTANCING://Instancingの場合にはこの関数は呼ばれないはず
		default:
			shaderindex = -1;
			break;
		}
		break;
	default:
		break;
	}


	//2023/01/02 SetRootSignatureは　BeginRender()で実行しているのでコメントアウト
	//if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
	//	(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
	//	(shaderindex == MQOSHADER_NOLIGHT_SHADOWMAP)) {
	//	rc->SetRootSignature(m_shadowrootSignature);
	//}
	//else {
	//	rc->SetRootSignature(m_rootSignature);
	//}


	int pipelineindex = 0;//!!!!!!!!!

	if (shaderindex >= 0) {
		if (withalpha) {
			if (renderobj.zcmpalways) {
				//###########################
				//Z cmp Always 半透明常に上書き
				//###########################
				pipelineindex = shaderindex + MQOSHADER_MAX * 2;
				//rc->SetPipelineState(m_zalwaysPipelineState[shaderindex]);
			}
			else {
				//###################
				//translucent　半透明
				//###################
				//rc->SetPipelineState(m_transPipelineState[shaderindex]);
				pipelineindex = shaderindex + MQOSHADER_MAX;
			}
		}
		else {
			//##############
			//Opaque 不透明
			//##############
			//rc->SetPipelineState(m_opaquePipelineState[shaderindex]);
			pipelineindex = shaderindex;
		}
	}
	else {
		//rc->SetPipelineState(m_opaquePipelineState[MQOSHADER_NOLIGHT]);
		pipelineindex = shaderindex;
	}



	if ((g_shadowmap_slotno < 0) || (g_shadowmap_slotno >= SHADOWSLOTNUM)) {
		_ASSERT(0);
		g_shadowmap_slotno = 0;
	}


	////定数バッファを更新する。
	if (pdispline && pextline) {
		m_cb.Init();
		m_cb.mWorld = renderobj.mWorld;
		m_cb.mView = mView;
		m_cb.mProj = mProj;
		//m_cb.diffusemult = renderobj.diffusemult;
		m_cb.diffusemult = pextline->GetColor();
		m_cb.materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb.shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb.UVs[0] = g_uvset;
		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer.CopyToVRAM(m_cb);
		}
		else {
			m_shadowcommonConstantBuffer.CopyToVRAM(m_cb);
		}
	}
	else if (ppm3) {
		m_cb.Init();
		m_cb.mWorld = renderobj.mWorld;
		m_cb.mView = mView;
		m_cb.mProj = mProj;
		//m_cb.diffusemult = renderobj.diffusemult;
		if (GetTempDiffuseMultFlag()) {
			m_cb.diffusemult = GetTempDiffuseMult() * renderobj.diffusemult;
		}
		else {
			m_cb.diffusemult = renderobj.diffusemult;
		}
		m_cb.ambient = ChaVector4(GetAmb3F(), 0.0f);
		if (GetEnableEmission()) {
			m_cb.emission = ChaVector4(GetEmi3F() * GetEmissiveScale(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb.emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb.metalcoef = ChaVector4(GetMetalCoef(), GetSmoothCoef(), 0.0f, 0.0f);
		m_cb.materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb.shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb.UVs[0] = g_uvset;
		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer.CopyToVRAM(m_cb);
		}
		else {
			m_shadowcommonConstantBuffer.CopyToVRAM(m_cb);
		}

		if (!GetUpdateLightsFlag(pipelineindex)) {//2023/12/04 ZAlwaysパイプライン描画のマニピュレータ表示がちらつくのでコメントアウト　パイプライン毎のフラグにすれば使える？
			SetConstLights(&m_cbLights);
			SetConstShadow(&m_cbShadow);
			if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
				m_expandConstantBuffer.CopyToVRAM(m_cbLights);
				m_expandConstantBuffer2.CopyToVRAM(m_cbShadow);
			}
			else {
				m_shadowexpandConstantBuffer.CopyToVRAM(m_cbLights);
				m_shadowexpandConstantBuffer2.CopyToVRAM(m_cbShadow);
			}
			SetUpdateLightsFlag(pipelineindex);
		}
	}
	else if (ppm4) {
		m_cb.Init();
		m_cb.mWorld = renderobj.mWorld;
		m_cb.mView = mView;
		m_cb.mProj = mProj;
		if (GetTempDiffuseMultFlag()) {
			m_cb.diffusemult = GetTempDiffuseMult() * renderobj.diffusemult;
		}
		else {
			m_cb.diffusemult = renderobj.diffusemult;
		}
		m_cb.ambient = ChaVector4(GetAmb3F(), 0.0f);
		if (GetEnableEmission()) {
			m_cb.emission = ChaVector4(GetEmi3F() * GetEmissiveScale(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb.emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb.metalcoef = ChaVector4(GetMetalCoef(), GetSmoothCoef(), 0.0f, 0.0f);
		m_cb.materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb.shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb.UVs[0] = g_uvset;
		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer.CopyToVRAM(m_cb);
		}
		else {
			m_shadowcommonConstantBuffer.CopyToVRAM(m_cb);
		}

		if (!GetUpdateFl4x4Flag(pipelineindex)) {//2023/12/01
		//if (isfirstmaterial) {
		//if (isfirstmaterial && !GetUpdateFl4x4Flag()) {
		//if (!renderobj.pmodel->GetUpdateFl4x4Flag()) {
			SetConstLights(&(m_cbMatrix.lights));
			SetFl4x4(renderobj);
			SetConstShadow(&m_cbShadow);

			if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
				m_expandConstantBuffer.CopyToVRAM(m_cbMatrix);
				m_expandConstantBuffer2.CopyToVRAM(m_cbShadow);
			}
			else {
				m_shadowexpandConstantBuffer.CopyToVRAM(m_cbMatrix);
				m_shadowexpandConstantBuffer2.CopyToVRAM(m_cbShadow);
			}

			renderobj.pmodel->SetUpdateFl4x4Flag();
			SetUpdateFl4x4Flag(pipelineindex);
		}
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

void CMQOMaterial::ZPreDrawCommon(RenderContext* rc, myRenderer::RENDEROBJ renderobj,
	const Matrix& mView, const Matrix& mProj,
	bool isfirstmaterial)
{
	if (!rc || !renderobj.mqoobj || !renderobj.pmodel) {
		_ASSERT(0);
		return;
	}

	CPolyMesh4* ppm4 = renderobj.mqoobj->GetPm4();
	CPolyMesh3* ppm3 = renderobj.mqoobj->GetPm3();
	CExtLine* pextline = renderobj.mqoobj->GetExtLine();
	CDispObj* pdispline = renderobj.mqoobj->GetDispLine();

	//if (pdispline && pextline) {
	//	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	//}
	//else if (ppm3 || ppm4) {
	//	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//}
	//else {
	//	_ASSERT(0);
	//	return;
	//}


	if ((g_shadowmap_slotno < 0) || (g_shadowmap_slotno >= SHADOWSLOTNUM)) {
		_ASSERT(0);
		g_shadowmap_slotno = 0;
	}


	////定数バッファを更新する。
	if (pdispline && pextline) {
		m_cb.mWorld = renderobj.mWorld;
		m_cb.mView = mView;
		m_cb.mProj = mProj;
		//m_cb.diffusemult = renderobj.diffusemult;
		m_cb.diffusemult = pextline->GetColor();
		m_cb.materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb.shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb.UVs[0] = g_uvset;
		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer.CopyToVRAM(m_cb);
		}
		else {
			m_shadowcommonConstantBuffer.CopyToVRAM(m_cb);
		}
	}
	else if (ppm3) {
		m_cb.mWorld = renderobj.mWorld;
		m_cb.mView = mView;
		m_cb.mProj = mProj;
		//m_cb.diffusemult = renderobj.diffusemult;
		if (GetTempDiffuseMultFlag()) {
			m_cb.diffusemult = GetTempDiffuseMult();
		}
		else {
			m_cb.diffusemult = renderobj.diffusemult;
		}
		m_cb.ambient = ChaVector4(GetAmb3F(), 0.0f);
		if (GetEnableEmission()) {
			m_cb.emission = ChaVector4(GetEmi3F(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb.emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb.materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb.shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb.UVs[0] = g_uvset;
		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer.CopyToVRAM(m_cb);
		}
		else {
			m_shadowcommonConstantBuffer.CopyToVRAM(m_cb);
		}
		//if (!GetUpdateLightsFlag()) {//2023/12/04 ZAlwaysパイプライン描画のマニピュレータ表示がちらつくのでコメントアウト　パイプライン毎のフラグにすれば使える？
			SetConstLights(&m_cbLights);
			if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
				m_expandConstantBuffer.CopyToVRAM(m_cbLights);
			}
			else {
				m_shadowexpandConstantBuffer.CopyToVRAM(m_cbLights);
			}
			//SetUpdateLightsFlag();
		//}
	}
	else if (ppm4) {
		m_cb.mWorld = renderobj.mWorld;
		m_cb.mView = mView;
		m_cb.mProj = mProj;
		if (GetTempDiffuseMultFlag()) {
			m_cb.diffusemult = GetTempDiffuseMult() * renderobj.diffusemult;
		}
		else {
			m_cb.diffusemult = renderobj.diffusemult;
		}
		m_cb.ambient = ChaVector4(GetAmb3F(), 0.0f);
		if (GetEnableEmission()) {
			m_cb.emission = ChaVector4(GetEmi3F(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb.emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb.materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb.shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb.UVs[0] = g_uvset;
		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer.CopyToVRAM(m_cb);
		}
		else {
			m_shadowcommonConstantBuffer.CopyToVRAM(m_cb);
		}
		//if (!GetUpdateFl4x4Flag()) {//2023/12/01
			//if (isfirstmaterial) {
			//if (isfirstmaterial && !GetUpdateFl4x4Flag()) {
			//if (!renderobj.pmodel->GetUpdateFl4x4Flag()) {
			SetConstLights(&(m_cbMatrix.lights));
			SetFl4x4(renderobj);

			if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
				m_expandConstantBuffer.CopyToVRAM(m_cbMatrix);
			}
			else {
				m_shadowexpandConstantBuffer.CopyToVRAM(m_cbMatrix);
			}
			renderobj.pmodel->SetUpdateFl4x4Flag();
			//SetUpdateFl4x4Flag();
		//}

	}
}

void CMQOMaterial::InstancingDrawCommon(RenderContext* rc, myRenderer::RENDEROBJ renderobj,
	const Matrix& mView, const Matrix& mProj,
	bool isfirstmaterial)
{
	if (!rc || !renderobj.mqoobj || !renderobj.pmodel) {
		_ASSERT(0);
		return;
	}

	CPolyMesh4* ppm4 = renderobj.mqoobj->GetPm4();
	CPolyMesh3* ppm3 = renderobj.mqoobj->GetPm3();
	CExtLine* pextline = renderobj.mqoobj->GetExtLine();
	CDispObj* pdispline = renderobj.mqoobj->GetDispLine();

	//if (pdispline && pextline) {
	//	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	//}
	//else if (ppm3 || ppm4) {
	//	rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//}
	//else {
	//	_ASSERT(0);
	//	return;
	//}


	if ((g_shadowmap_slotno < 0) || (g_shadowmap_slotno >= SHADOWSLOTNUM)) {
		_ASSERT(0);
		g_shadowmap_slotno = 0;
	}


	////定数バッファを更新する。
	if (pdispline && pextline) {
		m_cb.mWorld = renderobj.mWorld;
		m_cb.mView = mView;
		m_cb.mProj = mProj;
		//m_cb.diffusemult = renderobj.diffusemult;
		m_cb.diffusemult = pextline->GetColor();
		m_cb.materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb.shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb.UVs[0] = g_uvset;
		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer.CopyToVRAM(m_cb);
		}
		else {
			m_shadowcommonConstantBuffer.CopyToVRAM(m_cb);
		}
	}
	else if (ppm3) {
		m_cb.mWorld = renderobj.mWorld;//未使用
		m_cb.mView = mView;
		m_cb.mProj = mProj;
		//m_cb.diffusemult = renderobj.diffusemult;
		if (GetTempDiffuseMultFlag()) {
			m_cb.diffusemult = GetTempDiffuseMult();
		}
		else {
			m_cb.diffusemult = renderobj.diffusemult;
		}
		m_cb.ambient = ChaVector4(GetAmb3F(), 0.0f);
		if (GetEnableEmission()) {
			m_cb.emission = ChaVector4(GetEmi3F(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb.emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb.materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb.shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb.UVs[0] = g_uvset;
		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer.CopyToVRAM(m_cb);
		}
		else {
			m_shadowcommonConstantBuffer.CopyToVRAM(m_cb);
		}
		//if (!GetUpdateLightsFlag()) {//2023/12/04 ZAlwaysパイプライン描画のマニピュレータ表示がちらつくのでコメントアウト　パイプライン毎のフラグにすれば使える？
		SetConstLights(&m_cbLights);
		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_expandConstantBuffer.CopyToVRAM(m_cbLights);
		}
		else {
			m_shadowexpandConstantBuffer.CopyToVRAM(m_cbLights);
		}
		//SetUpdateLightsFlag();
	//}
	}
	else if (ppm4) {
		m_cb.mWorld = renderobj.mWorld;
		m_cb.mView = mView;
		m_cb.mProj = mProj;
		if (GetTempDiffuseMultFlag()) {
			m_cb.diffusemult = GetTempDiffuseMult() * renderobj.diffusemult;
		}
		else {
			m_cb.diffusemult = renderobj.diffusemult;
		}
		m_cb.ambient = ChaVector4(GetAmb3F(), 0.0f);
		if (GetEnableEmission()) {
			m_cb.emission = ChaVector4(GetEmi3F(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb.emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb.materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb.shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb.UVs[0] = g_uvset;
		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer.CopyToVRAM(m_cb);
		}
		else {
			m_shadowcommonConstantBuffer.CopyToVRAM(m_cb);
		}
		//if (!GetUpdateFl4x4Flag()) {//2023/12/01
			//if (isfirstmaterial) {
			//if (isfirstmaterial && !GetUpdateFl4x4Flag()) {
			//if (!renderobj.pmodel->GetUpdateFl4x4Flag()) {
		SetConstLights(&(m_cbMatrix.lights));
		SetFl4x4(renderobj);

		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_expandConstantBuffer.CopyToVRAM(m_cbMatrix);
		}
		else {
			m_shadowexpandConstantBuffer.CopyToVRAM(m_cbMatrix);
		}
		renderobj.pmodel->SetUpdateFl4x4Flag();
		//SetUpdateFl4x4Flag();
	//}

	}
}


void CMQOMaterial::SetBoneMatrix(myRenderer::RENDEROBJ renderobj)
{
	CModel* pmodel = renderobj.pmodel;
	if (!pmodel) {
		_ASSERT(0);
		return;
	}
	MOTINFO* curmi = renderobj.pmodel->GetCurMotInfo();
	if (renderobj.pmodel->GetTopBone() && (renderobj.pmodel->GetNoBoneFlag() == false) && curmi) {
		//CModel::SetShaderConst()でセットしたマトリックス配列をコピーするだけ
		renderobj.pmodel->GetBoneMatrix(m_cbMatrix.setfl4x4, MAXBONENUM);
	}
}


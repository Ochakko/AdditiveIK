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
extern ChaVector4 g_lightdirforall[LIGHTNUMMAX];//2024/02/15 有効無効に関わらずオリジナルのインデックスで格納


#include "../../MiniEngine/ConstantBuffer.h"
#include "../../MiniEngine/RootSignature.h"
//#include "../../MiniEngine/Material.h"


static int s_alloccount = 0;

extern ChaVector4 g_lightdirforshader[LIGHTNUMMAX];
extern ChaVector4 g_lightdiffuseforshader[LIGHTNUMMAX];
extern IShaderResource* g_shadowmapforshader;



CMQOMaterial::CMQOMaterial() : 
	m_descriptorHeap(), m_shadowdescriptorHeap(), //2023/12/14
	m_commonConstantBuffer(), m_expandConstantBuffer(), //2023/11/29
	m_expandConstantBuffer2(),//2023/12/10
	m_shadowcommonConstantBuffer(), m_shadowexpandConstantBuffer(), m_shadowexpandConstantBuffer2(), //2023/12/14
	m_whitetex(), m_blacktex(), m_diffuseMap(),
	m_cb(), m_cbMatrix(), m_cbLights(), m_cbShadow(), //2023/12/01 //2023/12/02 //2023/12/10
	m_rootSignature(), //2023/12/01
	m_shadowrootSignature(), //2023/12/14
	m_ZPrerootSignature(), //2023/12/05
	m_ZPreModelPipelineState(), //2023/12/05
	m_ZPreModelSkyPipelineState(), //2024/03/25
	m_InstancingrootSignature(), //2024/01/11
	m_InstancingOpequeTrianglePipelineState(),//2024/02/08
	m_InstancingtransTrianglePipelineState(),//2024/02/08
	m_InstancingtransTriangleNoZPipelineState(),//2024/02/08
	m_InstancingzalwaysTrianglePipelineState(),//2024/02/08
	m_InstancingOpequeLinePipelineState(),//2024/02/08
	m_InstancingtransLinePipelineState(),//2024/02/08
	m_InstancingzalwaysLinePipelineState()//2024/02/08
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
	
	m_refposflag = false;
	m_refposnum = 0;

	m_shaderfx = -1;
	int fxno;
	for (fxno = 0; fxno < SHADERFX_MAX; fxno++) {
		m_updatefl4x4flag[fxno] = false;
		m_updatelightsflag[fxno] = false;
	}


	m_materialno = -1;
	ZeroMemory ( m_name, 256 );

	int refposindex;
	for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
		m_cb[refposindex].Init();
		m_cbMatrix[refposindex].Init();
		m_cbLights[refposindex].Init();
		m_cbShadow[refposindex].Init();
	}

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
		int refposindex2;
		for (refposindex2 = 0; refposindex2 < REFPOSMAXNUM; refposindex2++) {
			m_opaquePipelineState[shaderindex][refposindex2].InitParams();
			m_transPipelineState[shaderindex][refposindex2].InitParams();
			m_transNoZPipelineState[shaderindex][refposindex2].InitParams();
			m_zalwaysPipelineState[shaderindex][refposindex2].InitParams();
		}
		m_vsMQOShader[shaderindex] = nullptr;
		m_psMQOShader[shaderindex] = nullptr;
	}


	//next = 0;



	m_shader = 3;//mqofile記述のshader

	m_shadertype = -2;//DirectX12描画用のshader //Shaderプレートメニュー用
	m_metalcoef = 0.250f;//Shaderプレートメニュー用
	m_smoothcoef = 0.250f;//Shaderプレートメニュー用
	m_metaladd = 0.0f;
	int litno;
	for (litno = 0; litno < LIGHTNUMMAX; litno++) {
		m_lightscale[litno] = 1.0f;//Shaderプレートメニュー用
	}
	m_specularcoef = 0.1250f;
	m_normaly0flag = true;//InitShadersAndPipelines()にてpm4:true, pm3:falseに初期化

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

	m_uvscale = ChaVectorDbl2(1.0, 1.0);
	m_uvoffset = ChaVectorDbl2(0.0, 0.0);

	m_hsvtoon.Init();

	m_shadowcasterflag = true;//2024/03/03
	m_lightingflag = true;//2024/03/07

	m_alphatestclipval = (8.0 / 255.0);//2024/03/22

	return 0;
}

void CMQOMaterial::DestroyObjs()
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

	int cbindex;
	for (cbindex = 0; cbindex < REFPOSMAXNUM; cbindex++) {
		m_commonConstantBuffer[cbindex].DestroyObjs();
		m_expandConstantBuffer[cbindex].DestroyObjs();
		m_expandConstantBuffer2[cbindex].DestroyObjs();
		m_shadowcommonConstantBuffer[cbindex].DestroyObjs();
		m_shadowexpandConstantBuffer[cbindex].DestroyObjs();
		m_shadowexpandConstantBuffer2[cbindex].DestroyObjs();
	}


	int shaderindex, refposindex;
	for (shaderindex = 0; shaderindex < MQOSHADER_MAX; shaderindex++) {
		for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
			m_opaquePipelineState[shaderindex][refposindex].DestroyObjs();
			m_transPipelineState[shaderindex][refposindex].DestroyObjs();
			m_transNoZPipelineState[shaderindex][refposindex].DestroyObjs();
			m_zalwaysPipelineState[shaderindex][refposindex].DestroyObjs();
		}
	}
	for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
		m_ZPreModelPipelineState[refposindex].DestroyObjs();
		m_ZPreModelSkyPipelineState[refposindex].DestroyObjs();
	}
	m_InstancingOpequeTrianglePipelineState.DestroyObjs();
	m_InstancingtransTrianglePipelineState.DestroyObjs();
	m_InstancingtransTriangleNoZPipelineState.DestroyObjs();
	m_InstancingzalwaysTrianglePipelineState.DestroyObjs();
	m_InstancingOpequeLinePipelineState.DestroyObjs();
	m_InstancingtransLinePipelineState.DestroyObjs();
	m_InstancingzalwaysLinePipelineState.DestroyObjs();


	int rsindex;
	for (rsindex = 0; rsindex < REFPOSMAXNUM; rsindex++) {
		m_descriptorHeap[rsindex].DestroyObjs();
		m_shadowdescriptorHeap[rsindex].DestroyObjs();
	}
	for (rsindex = 0; rsindex < REFPOSMAXNUM; rsindex++) {
		m_rootSignature[rsindex].DestroyObjs();
		m_ZPrerootSignature[rsindex].DestroyObjs();
		m_shadowrootSignature[rsindex].DestroyObjs();
	}
	m_InstancingrootSignature.DestroyObjs();

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

int CMQOMaterial::InitZPreShadersAndPipelines(
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
		return 0;
	}

	if (m_initprezpipelineflag) {
	//if (m_initpipelineflag) {
		//###############################
		//既に初期化済の場合は　すぐにリターン
		//###############################
		return 0;
	}


	//ルートシグネチャを初期化。
	D3D12_STATIC_SAMPLER_DESC samplerDescArray[6];
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
	samplerDescArray[2].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//2024/02/21 : normalmapは補間無しに
	samplerDescArray[2].ShaderRegister = 2;//!!!!!!!!
	//if (srcuvnum >= 2) {//2024/02/21　コメントアウト
	samplerDescArray[2].AddressU = GetAddressU_normal();//2024/01/06
	samplerDescArray[2].AddressV = GetAddressV_normal();//2024/01/06
	//samplerDescArray[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//samplerDescArray[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//samplerDescArray[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//}
	//else {
	//	samplerDescArray[2].AddressU = GetAddressU_albedo();//2024/01/06
	//	samplerDescArray[2].AddressV = GetAddressV_albedo();//2024/01/06
	//}
	////samplerDescArray[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	////samplerDescArray[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	////samplerDescArray[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	samplerDescArray[3] = samplerDescArray[0];
	samplerDescArray[3].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//2024/02/21 : metalmapは補間無しに
	samplerDescArray[3].ShaderRegister = 3;//!!!!!!!!
	//if (srcuvnum >= 2) {//2024/02/21　コメントアウト
	samplerDescArray[3].AddressU = GetAddressU_metal();//2024/01/06
	samplerDescArray[3].AddressV = GetAddressV_metal();//2024/01/06
	//}
	//else {
	//	samplerDescArray[3].AddressU = GetAddressU_albedo();//2024/01/06
	//	samplerDescArray[3].AddressV = GetAddressV_albedo();//2024/01/06
	//}

	samplerDescArray[4] = samplerDescArray[0];
	samplerDescArray[4].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//2024/02/21 : 補間無しに
	samplerDescArray[4].ShaderRegister = 4;//!!!!!!!!
	samplerDescArray[4].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescArray[4].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescArray[4].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;


	//シャドウマップ用のサンプラ。
	samplerDescArray[5] = samplerDescArray[0];
	samplerDescArray[5].ShaderRegister = 5;//!!!!!!!!
	//比較対象の値が小さければ０、大きければ１を返す比較関数を設定する。
	samplerDescArray[5].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDescArray[5].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
	samplerDescArray[5].MaxAnisotropy = 1;

	int refposindex;
	for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
		m_ZPrerootSignature[refposindex].Init(
			samplerDescArray,
			6,
			numCbv,
			numSrv,
			8,
			offsetInDescriptorsFromTableStartCB,
			offsetInDescriptorsFromTableStartSRV
		);
	}

	if (fxFilePath != nullptr && strlen(fxFilePath) > 0) {
		//シェーダーを初期化。
		int result;
		result = InitZPreShaders(fxFilePath, vsEntryPointFunc, psEntryPointFunc);
		if (result != 0) {
			m_initprezpipelineflag = false;
			return 1;
		}
		//パイプラインステートを初期化。
		InitZPrePipelineState(vertextype, colorBufferFormat);
	}

	m_initprezpipelineflag = true;
	return 0;
}


int CMQOMaterial::InitInstancingShadersAndPipelines(
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
		return 0;
	}

	if (m_initInstancingpipelineflag) {
		//if (m_initpipelineflag) {
			//###############################
			//既に初期化済の場合は　すぐにリターン
			//###############################
		return 0;
	}


	//ルートシグネチャを初期化。
	D3D12_STATIC_SAMPLER_DESC samplerDescArray[6];
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
	samplerDescArray[2].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//2024/02/21 : normalmapは補間無しに
	samplerDescArray[2].ShaderRegister = 2;//!!!!!!!!
	//if (srcuvnum >= 2) {//2024/02/21　コメントアウト
	samplerDescArray[2].AddressU = GetAddressU_normal();//2024/01/06
	samplerDescArray[2].AddressV = GetAddressV_normal();//2024/01/06
	//samplerDescArray[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//samplerDescArray[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//samplerDescArray[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//}
	//else {
	//	samplerDescArray[2].AddressU = GetAddressU_albedo();//2024/01/06
	//	samplerDescArray[2].AddressV = GetAddressV_albedo();//2024/01/06
	//}
	////samplerDescArray[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	////samplerDescArray[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	////samplerDescArray[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	samplerDescArray[3] = samplerDescArray[0];
	samplerDescArray[3].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//2024/02/21 : metalmapは補間無しに
	samplerDescArray[3].ShaderRegister = 3;//!!!!!!!!
	//if (srcuvnum >= 2) {//2024/02/21　コメントアウト
	samplerDescArray[3].AddressU = GetAddressU_metal();//2024/01/06
	samplerDescArray[3].AddressV = GetAddressV_metal();//2024/01/06
	//}
	//else {
	//	samplerDescArray[3].AddressU = GetAddressU_albedo();//2024/01/06
	//	samplerDescArray[3].AddressV = GetAddressV_albedo();//2024/01/06
	//}

	samplerDescArray[4] = samplerDescArray[0];
	samplerDescArray[4].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//2024/02/21 : 補間無しに
	samplerDescArray[4].ShaderRegister = 4;//!!!!!!!!
	samplerDescArray[4].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescArray[4].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescArray[4].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;


	//シャドウマップ用のサンプラ。
	samplerDescArray[5] = samplerDescArray[0];
	samplerDescArray[5].ShaderRegister = 5;//!!!!!!!!
	//比較対象の値が小さければ０、大きければ１を返す比較関数を設定する。
	samplerDescArray[5].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDescArray[5].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
	samplerDescArray[5].MaxAnisotropy = 1;

	m_InstancingrootSignature.Init(
		samplerDescArray,
		6,
		numCbv,
		numSrv,
		8,
		offsetInDescriptorsFromTableStartCB,
		offsetInDescriptorsFromTableStartSRV
	);

	if (fxFilePath != nullptr && strlen(fxFilePath) > 0) {
		//シェーダーを初期化。
		int result;
		result = InitInstancingShaders(fxFilePath, vsEntryPointFunc, psEntryPointFunc);
		if (result != 0) {
			m_initInstancingpipelineflag = false;
			return 1;
		}
		//パイプラインステートを初期化。
		InitInstancingPipelineState(vertextype, colorBufferFormat);
	}

	m_initInstancingpipelineflag = true;
	return 0;
}


int CMQOMaterial::InitShadersAndPipelines(
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
		return 1;
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
		return 1;
	}

	if (m_initpipelineflag) {
		//###############################
		//既に初期化済の場合は　すぐにリターン
		//###############################
		return 0;
	}



	//2024/02/19 MeshTypeをみて初期化
	if (vertextype == 0) {
		SetNormalY0Flag(true);
	}
	else {
		SetNormalY0Flag(false);
	}



	//ルートシグネチャを初期化。
	D3D12_STATIC_SAMPLER_DESC samplerDescArray[6];
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
	samplerDescArray[2].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//2024/02/21 : normalmapは補間無しに
	samplerDescArray[2].ShaderRegister = 2;//!!!!!!!!
	//if (srcuvnum >= 2) {//2024/02/21　コメントアウト
		samplerDescArray[2].AddressU = GetAddressU_normal();//2024/01/06
		samplerDescArray[2].AddressV = GetAddressV_normal();//2024/01/06
	//samplerDescArray[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//samplerDescArray[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//samplerDescArray[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//}
	//else {
	//	samplerDescArray[2].AddressU = GetAddressU_albedo();//2024/01/06
	//	samplerDescArray[2].AddressV = GetAddressV_albedo();//2024/01/06
	//}
	////samplerDescArray[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	////samplerDescArray[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	////samplerDescArray[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	samplerDescArray[3] = samplerDescArray[0];
	samplerDescArray[3].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//2024/02/21 : metalmapは補間無しに
	samplerDescArray[3].ShaderRegister = 3;//!!!!!!!!
	//if (srcuvnum >= 2) {//2024/02/21　コメントアウト
		samplerDescArray[3].AddressU = GetAddressU_metal();//2024/01/06
		samplerDescArray[3].AddressV = GetAddressV_metal();//2024/01/06
	//}
	//else {
	//	samplerDescArray[3].AddressU = GetAddressU_albedo();//2024/01/06
	//	samplerDescArray[3].AddressV = GetAddressV_albedo();//2024/01/06
	//}

	samplerDescArray[4] = samplerDescArray[0];
	samplerDescArray[4].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//2024/02/21 : 補間無しに
	samplerDescArray[4].ShaderRegister = 4;//!!!!!!!!
	samplerDescArray[4].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescArray[4].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescArray[4].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;


	//シャドウマップ用のサンプラ。
	samplerDescArray[5] = samplerDescArray[0];
	samplerDescArray[5].ShaderRegister = 5;//!!!!!!!!
	//比較対象の値が小さければ０、大きければ１を返す比較関数を設定する。
	samplerDescArray[5].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDescArray[5].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
	samplerDescArray[5].MaxAnisotropy = 1;


	int refposindex;
	for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
		m_rootSignature[refposindex].Init(
			samplerDescArray,
			6,
			numCbv,
			numSrv,
			8,
			offsetInDescriptorsFromTableStartCB,
			offsetInDescriptorsFromTableStartSRV
		);
		m_shadowrootSignature[refposindex].Init(
			samplerDescArray,
			6,
			numCbv,
			numSrv,
			8,
			offsetInDescriptorsFromTableStartCB,
			offsetInDescriptorsFromTableStartSRV
		);
	}


	//シェーダーを初期化。
	int result;
	result = InitShaders(
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
	if (result != 0) {
		m_initpipelineflag = false;
		return 1;
	}

	//パイプラインステートを初期化。
	InitPipelineState(vertextype, colorBufferFormat);

	m_initpipelineflag = true;
	return 0;
}


void CMQOMaterial::InitPipelineState(int vertextype, const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat)
{
	int refposindex;

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
				(shaderindex == MQOSHADER_TOON_SHADOWMAP)) {

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
			(shaderindex == MQOSHADER_TOON_SHADOWMAP) ||
			(shaderindex == MQOSHADER_STD_SHADOWMAP)
			) {
			//2023/12/11 ShadowMap
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			//psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32_FLOAT;
			numRenderTarget = 1;
		}
		//else if (
		//	(shaderindex == MQOSHADER_PBR_SHADOWRECIEVER) ||
		//	(shaderindex == MQOSHADER_TOON_SHADOWRECIEVER) ||
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

		if (vertextype == 0) {
			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
					(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
					(shaderindex == MQOSHADER_TOON_SHADOWMAP)) {
					psoDesc.pRootSignature = m_shadowrootSignature[refposindex].Get();
				}
				else {
					psoDesc.pRootSignature = m_rootSignature[refposindex].Get();
				}
				psoDesc.DepthStencilState.DepthEnable = TRUE;
				m_opaquePipelineState[shaderindex][refposindex].Init(psoDesc);
			}
		}
		else {
			if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
				(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
				(shaderindex == MQOSHADER_TOON_SHADOWMAP)) {
				psoDesc.pRootSignature = m_shadowrootSignature[0].Get();
			}
			else {
				psoDesc.pRootSignature = m_rootSignature[0].Get();
			}
			psoDesc.DepthStencilState.DepthEnable = TRUE;
			m_opaquePipelineState[shaderindex][0].Init(psoDesc);
		}


		//続いて半透明マテリアル用。
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vsMQOShader[shaderindex]->GetCompiledBlob());
		psoDesc.BlendState.IndependentBlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		//psoDesc.DepthStencilState.DepthEnable = FALSE;
		//psoDesc.DepthStencilState.DepthEnable = TRUE;

		if (vertextype == 0) {
			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
					(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
					(shaderindex == MQOSHADER_TOON_SHADOWMAP)) {
					psoDesc.pRootSignature = m_shadowrootSignature[refposindex].Get();
				}
				else {
					psoDesc.pRootSignature = m_rootSignature[refposindex].Get();
				}
				psoDesc.DepthStencilState.DepthEnable = TRUE;
				m_transPipelineState[shaderindex][refposindex].Init(psoDesc);
				psoDesc.DepthStencilState.DepthEnable = FALSE;
				m_transNoZPipelineState[shaderindex][refposindex].Init(psoDesc);
			}
		}
		else {
			if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
				(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
				(shaderindex == MQOSHADER_TOON_SHADOWMAP)) {
				psoDesc.pRootSignature = m_shadowrootSignature[0].Get();
			}
			else {
				psoDesc.pRootSignature = m_rootSignature[0].Get();
			}
			psoDesc.DepthStencilState.DepthEnable = TRUE;
			m_transPipelineState[shaderindex][0].Init(psoDesc);
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			m_transNoZPipelineState[shaderindex][0].Init(psoDesc);
		}

		////2023/12/01
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		if (vertextype == 0) {
			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
					(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
					(shaderindex == MQOSHADER_TOON_SHADOWMAP)) {
					psoDesc.pRootSignature = m_shadowrootSignature[refposindex].Get();
				}
				else {
					psoDesc.pRootSignature = m_rootSignature[refposindex].Get();
				}
				psoDesc.DepthStencilState.DepthEnable = TRUE;
				m_zalwaysPipelineState[shaderindex][refposindex].Init(psoDesc);
			}
		}
		else {
			if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
				(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
				(shaderindex == MQOSHADER_TOON_SHADOWMAP)) {
				psoDesc.pRootSignature = m_shadowrootSignature[0].Get();
			}
			else {
				psoDesc.pRootSignature = m_rootSignature[0].Get();
			}
			psoDesc.DepthStencilState.DepthEnable = TRUE;
			m_zalwaysPipelineState[shaderindex][0].Init(psoDesc);
		}
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

		int refposindex;
		for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
			psoDesc.pRootSignature = m_ZPrerootSignature[refposindex].Get();
			m_ZPreModelPipelineState[refposindex].Init(psoDesc);
		}


		psoDesc.DepthStencilState.DepthEnable = FALSE;
		//psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
			psoDesc.pRootSignature = m_ZPrerootSignature[refposindex].Get();
			m_ZPreModelSkyPipelineState[refposindex].Init(psoDesc);//!!!!!!!!!!!!! for sky
		}
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
		//	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
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
		
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_InstancingOpequeTrianglePipelineState.Init(psoDesc);
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		m_InstancingOpequeLinePipelineState.Init(psoDesc);


		//続いて半透明マテリアル用。
		psoDesc.BlendState.IndependentBlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		//psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_InstancingtransTrianglePipelineState.Init(psoDesc);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		m_InstancingtransTriangleNoZPipelineState.Init(psoDesc);
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		m_InstancingtransLinePipelineState.Init(psoDesc);

		////2023/12/01
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;


		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_InstancingzalwaysTrianglePipelineState.Init(psoDesc);
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		m_InstancingzalwaysLinePipelineState.Init(psoDesc);

	}
}

int CMQOMaterial::InitShaders(
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
	int result = 0;

	m_vsMQOShader[MQOSHADER_PBR] = g_engine->GetShaderFromBank(fxPBRPath, vsPBRFunc);
	if (m_vsMQOShader[MQOSHADER_PBR] == nullptr) {
		m_vsMQOShader[MQOSHADER_PBR] = new Shader;
		if (!m_vsMQOShader[MQOSHADER_PBR]) {
			return 1;
		}
		result = m_vsMQOShader[MQOSHADER_PBR]->LoadVS(fxPBRPath, vsPBRFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxPBRPath, vsPBRFunc, m_vsMQOShader[MQOSHADER_PBR]);
	}
	m_psMQOShader[MQOSHADER_PBR] = g_engine->GetShaderFromBank(fxPBRPath, psPBRFunc);
	if (m_psMQOShader[MQOSHADER_PBR] == nullptr) {
		m_psMQOShader[MQOSHADER_PBR] = new Shader;
		if (!m_psMQOShader[MQOSHADER_PBR]) {
			return 1;
		}
		result = m_psMQOShader[MQOSHADER_PBR]->LoadPS(fxPBRPath, psPBRFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxPBRPath, psPBRFunc, m_psMQOShader[MQOSHADER_PBR]);
	}

	m_vsMQOShader[MQOSHADER_PBR_SHADOWMAP] = g_engine->GetShaderFromBank(fxPBRPath, vsPBRShadowMapFunc);
	if (m_vsMQOShader[MQOSHADER_PBR_SHADOWMAP] == nullptr) {
		m_vsMQOShader[MQOSHADER_PBR_SHADOWMAP] = new Shader;//!!!!!! 2024/03/17
		if (!m_vsMQOShader[MQOSHADER_PBR_SHADOWMAP]) {
			return 1;
		}
		result = m_vsMQOShader[MQOSHADER_PBR_SHADOWMAP]->LoadVS(fxPBRPath, vsPBRShadowMapFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxPBRPath, vsPBRShadowMapFunc, m_vsMQOShader[MQOSHADER_PBR_SHADOWMAP]);
	}
	m_psMQOShader[MQOSHADER_PBR_SHADOWMAP] = g_engine->GetShaderFromBank(fxPBRPath, psPBRShadowMapFunc);
	if (m_psMQOShader[MQOSHADER_PBR_SHADOWMAP] == nullptr) {
		m_psMQOShader[MQOSHADER_PBR_SHADOWMAP] = new Shader;
		if (!m_psMQOShader[MQOSHADER_PBR_SHADOWMAP]) {
			return 1;
		}
		result = m_psMQOShader[MQOSHADER_PBR_SHADOWMAP]->LoadPS(fxPBRPath, psPBRShadowMapFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxPBRPath, psPBRShadowMapFunc, m_psMQOShader[MQOSHADER_PBR_SHADOWMAP]);
	}

	m_vsMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxPBRPath, vsPBRShadowRecieverFunc);
	if (m_vsMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] == nullptr) {
		m_vsMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] = new Shader;
		if (!m_vsMQOShader[MQOSHADER_PBR_SHADOWRECIEVER]) {
			return 1;
		}
		result = m_vsMQOShader[MQOSHADER_PBR_SHADOWRECIEVER]->LoadVS(fxPBRPath, vsPBRShadowRecieverFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxPBRPath, vsPBRShadowRecieverFunc, m_vsMQOShader[MQOSHADER_PBR_SHADOWRECIEVER]);
	}
	m_psMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxPBRPath, psPBRShadowRecieverFunc);
	if (m_psMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] == nullptr) {
		m_psMQOShader[MQOSHADER_PBR_SHADOWRECIEVER] = new Shader;
		if (!m_psMQOShader[MQOSHADER_PBR_SHADOWRECIEVER]) {
			return 1;
		}
		result = m_psMQOShader[MQOSHADER_PBR_SHADOWRECIEVER]->LoadPS(fxPBRPath, psPBRShadowRecieverFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxPBRPath, psPBRShadowRecieverFunc, m_psMQOShader[MQOSHADER_PBR_SHADOWRECIEVER]);
	}

//################
//Standardシェーダ
//################
	m_vsMQOShader[MQOSHADER_STD] = g_engine->GetShaderFromBank(fxStdPath, vsStdFunc);
	if (m_vsMQOShader[MQOSHADER_STD] == nullptr) {
		m_vsMQOShader[MQOSHADER_STD] = new Shader;
		if (!m_vsMQOShader[MQOSHADER_STD]) {
			return 1;
		}
		result = m_vsMQOShader[MQOSHADER_STD]->LoadVS(fxStdPath, vsStdFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxStdPath, vsStdFunc, m_vsMQOShader[MQOSHADER_STD]);
	}
	m_psMQOShader[MQOSHADER_STD] = g_engine->GetShaderFromBank(fxStdPath, psStdFunc);
	if (m_psMQOShader[MQOSHADER_STD] == nullptr) {
		m_psMQOShader[MQOSHADER_STD] = new Shader;
		if (!m_psMQOShader[MQOSHADER_STD]) {
			return 1;
		}
		result = m_psMQOShader[MQOSHADER_STD]->LoadPS(fxStdPath, psStdFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxStdPath, psStdFunc, m_psMQOShader[MQOSHADER_STD]);
	}

	m_vsMQOShader[MQOSHADER_STD_SHADOWMAP] = g_engine->GetShaderFromBank(fxStdPath, vsStdShadowMapFunc);
	if (m_vsMQOShader[MQOSHADER_STD_SHADOWMAP] == nullptr) {
		m_vsMQOShader[MQOSHADER_STD_SHADOWMAP] = new Shader;
		if (!m_vsMQOShader[MQOSHADER_STD_SHADOWMAP]) {
			return 1;
		}
		result = m_vsMQOShader[MQOSHADER_STD_SHADOWMAP]->LoadVS(fxStdPath, vsStdShadowMapFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxStdPath, vsStdShadowMapFunc, m_vsMQOShader[MQOSHADER_STD_SHADOWMAP]);
	}
	m_psMQOShader[MQOSHADER_STD_SHADOWMAP] = g_engine->GetShaderFromBank(fxStdPath, psStdShadowMapFunc);
	if (m_psMQOShader[MQOSHADER_STD_SHADOWMAP] == nullptr) {
		m_psMQOShader[MQOSHADER_STD_SHADOWMAP] = new Shader;
		if (!m_psMQOShader[MQOSHADER_STD_SHADOWMAP]) {
			return 1;
		}
		result = m_psMQOShader[MQOSHADER_STD_SHADOWMAP]->LoadPS(fxStdPath, psStdShadowMapFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxStdPath, psStdShadowMapFunc, m_psMQOShader[MQOSHADER_STD_SHADOWMAP]);
	}

	m_vsMQOShader[MQOSHADER_STD_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxStdPath, vsStdShadowRecieverFunc);
	if (m_vsMQOShader[MQOSHADER_STD_SHADOWRECIEVER] == nullptr) {
		m_vsMQOShader[MQOSHADER_STD_SHADOWRECIEVER] = new Shader;
		if (!m_vsMQOShader[MQOSHADER_STD_SHADOWRECIEVER]) {
			return 1;
		}
		result = m_vsMQOShader[MQOSHADER_STD_SHADOWRECIEVER]->LoadVS(fxStdPath, vsStdShadowRecieverFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxStdPath, vsStdShadowRecieverFunc, m_vsMQOShader[MQOSHADER_STD_SHADOWRECIEVER]);
	}
	m_psMQOShader[MQOSHADER_STD_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxStdPath, psStdShadowRecieverFunc);
	if (m_psMQOShader[MQOSHADER_STD_SHADOWRECIEVER] == nullptr) {
		m_psMQOShader[MQOSHADER_STD_SHADOWRECIEVER] = new Shader;
		if (!m_psMQOShader[MQOSHADER_STD_SHADOWRECIEVER]) {
			return 1;
		}
		result = m_psMQOShader[MQOSHADER_STD_SHADOWRECIEVER]->LoadPS(fxStdPath, psStdShadowRecieverFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxStdPath, psStdShadowRecieverFunc, m_psMQOShader[MQOSHADER_STD_SHADOWRECIEVER]);
	}

//###############
//NoLightシェーダ
//###############
	m_vsMQOShader[MQOSHADER_TOON] = g_engine->GetShaderFromBank(fxNoLightPath, vsNoLightFunc);
	if (m_vsMQOShader[MQOSHADER_TOON] == nullptr) {
		m_vsMQOShader[MQOSHADER_TOON] = new Shader;
		if (!m_vsMQOShader[MQOSHADER_TOON]) {
			return 1;
		}
		result = m_vsMQOShader[MQOSHADER_TOON]->LoadVS(fxNoLightPath, vsNoLightFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxNoLightPath, vsNoLightFunc, m_vsMQOShader[MQOSHADER_TOON]);
	}
	m_psMQOShader[MQOSHADER_TOON] = g_engine->GetShaderFromBank(fxNoLightPath, psNoLightFunc);
	if (m_psMQOShader[MQOSHADER_TOON] == nullptr) {
		m_psMQOShader[MQOSHADER_TOON] = new Shader;
		if (!m_psMQOShader[MQOSHADER_TOON]) {
			return 1;
		}
		result = m_psMQOShader[MQOSHADER_TOON]->LoadPS(fxNoLightPath, psNoLightFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxNoLightPath, psNoLightFunc, m_psMQOShader[MQOSHADER_TOON]);
	}

	m_vsMQOShader[MQOSHADER_TOON_SHADOWMAP] = g_engine->GetShaderFromBank(fxNoLightPath, vsNoLightShadowMapFunc);
	if (m_vsMQOShader[MQOSHADER_TOON_SHADOWMAP] == nullptr) {
		m_vsMQOShader[MQOSHADER_TOON_SHADOWMAP] = new Shader;
		if (!m_vsMQOShader[MQOSHADER_TOON_SHADOWMAP]) {
			return 1;
		}
		result = m_vsMQOShader[MQOSHADER_TOON_SHADOWMAP]->LoadVS(fxNoLightPath, vsNoLightShadowMapFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxNoLightPath, vsNoLightShadowMapFunc, m_vsMQOShader[MQOSHADER_TOON_SHADOWMAP]);
	}
	m_psMQOShader[MQOSHADER_TOON_SHADOWMAP] = g_engine->GetShaderFromBank(fxNoLightPath, psNoLightShadowMapFunc);
	if (m_psMQOShader[MQOSHADER_TOON_SHADOWMAP] == nullptr) {
		m_psMQOShader[MQOSHADER_TOON_SHADOWMAP] = new Shader;
		if (!m_psMQOShader[MQOSHADER_TOON_SHADOWMAP]) {
			return 1;
		}
		result = m_psMQOShader[MQOSHADER_TOON_SHADOWMAP]->LoadPS(fxNoLightPath, psNoLightShadowMapFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxNoLightPath, psNoLightShadowMapFunc, m_psMQOShader[MQOSHADER_TOON_SHADOWMAP]);
	}

	m_vsMQOShader[MQOSHADER_TOON_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxNoLightPath, vsNoLightShadowRecieverFunc);
	if (m_vsMQOShader[MQOSHADER_TOON_SHADOWRECIEVER] == nullptr) {
		m_vsMQOShader[MQOSHADER_TOON_SHADOWRECIEVER] = new Shader;
		if (!m_vsMQOShader[MQOSHADER_TOON_SHADOWRECIEVER]) {
			return 1;
		}
		result = m_vsMQOShader[MQOSHADER_TOON_SHADOWRECIEVER]->LoadVS(fxNoLightPath, vsNoLightShadowRecieverFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxNoLightPath, vsNoLightShadowRecieverFunc, m_vsMQOShader[MQOSHADER_TOON_SHADOWRECIEVER]);
	}
	m_psMQOShader[MQOSHADER_TOON_SHADOWRECIEVER] = g_engine->GetShaderFromBank(fxNoLightPath, psNoLightShadowRecieverFunc);
	if (m_psMQOShader[MQOSHADER_TOON_SHADOWRECIEVER] == nullptr) {
		m_psMQOShader[MQOSHADER_TOON_SHADOWRECIEVER] = new Shader;
		if (!m_psMQOShader[MQOSHADER_TOON_SHADOWRECIEVER]) {
			return 1;
		}
		result = m_psMQOShader[MQOSHADER_TOON_SHADOWRECIEVER]->LoadPS(fxNoLightPath, psNoLightShadowRecieverFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxNoLightPath, psNoLightShadowRecieverFunc, m_psMQOShader[MQOSHADER_TOON_SHADOWRECIEVER]);
	}


	return 0;
}

int CMQOMaterial::InitZPreShaders(
	const char* fxFilePath,
	const char* vsEntryPointFunc,
	const char* psEntryPointFunc
)
{
	int result = 0;
	//スキンなしモデル用のシェーダーをロードする。
	m_vsZPreModel = g_engine->GetShaderFromBank(fxFilePath, vsEntryPointFunc);
	if (m_vsZPreModel == nullptr) {
		m_vsZPreModel = new Shader;
		if (!m_vsZPreModel) {
			return 1;
		}
		result = m_vsZPreModel->LoadVS(fxFilePath, vsEntryPointFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxFilePath, vsEntryPointFunc, m_vsZPreModel);
	}

	m_psZPreModel = g_engine->GetShaderFromBank(fxFilePath, psEntryPointFunc);
	if (m_psZPreModel == nullptr) {
		m_psZPreModel = new Shader;
		if (!m_psZPreModel) {
			return 1;
		}
		result = m_psZPreModel->LoadPS(fxFilePath, psEntryPointFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxFilePath, psEntryPointFunc, m_psZPreModel);
	}

	return 0;
}

int CMQOMaterial::InitInstancingShaders(
	const char* fxFilePath,
	const char* vsEntryPointFunc,
	const char* psEntryPointFunc
)
{
	int result = 0;

	//スキンなしモデル用のシェーダーをロードする。
	m_vsInstancingModel = g_engine->GetShaderFromBank(fxFilePath, vsEntryPointFunc);
	if (m_vsInstancingModel == nullptr) {
		m_vsInstancingModel = new Shader;
		if (!m_vsInstancingModel) {
			return 1;
		}
		result = m_vsInstancingModel->LoadVS(fxFilePath, vsEntryPointFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxFilePath, vsEntryPointFunc, m_vsInstancingModel);
	}

	m_psInstancingModel = g_engine->GetShaderFromBank(fxFilePath, psEntryPointFunc);
	if (m_psInstancingModel == nullptr) {
		m_psInstancingModel = new Shader;
		if (!m_psInstancingModel) {
			return 1;
		}
		result = m_psInstancingModel->LoadPS(fxFilePath, psEntryPointFunc);
		if (result != 0) {
			return 1;
		}
		g_engine->RegistShaderToBank(fxFilePath, psEntryPointFunc, m_psInstancingModel);
	}

	return 0;
}

bool CMQOMaterial::DecideLightFlag(myRenderer::RENDEROBJ renderobj)
{
	bool lightflag;
	if (g_lightflag != 0) {
		if (renderobj.lightflag == -1) {
			lightflag = GetLightingFlag();
		}
		else {
			lightflag = renderobj.lightflag;
		}
	}
	else {
		lightflag = false;
	}
	return lightflag;
}

int CMQOMaterial::DecideShaderIndex(myRenderer::RENDEROBJ renderobj)
{
	//#####################################################################################################
	//2023/12/05
	//renderobj.shadertype : MQOSHADER_PBR, MQOSHADER_STD, MOQSHADER_NOLIGHTで指定されたようにパイプラインを設定
	//-1指定の場合には　テクスチャの設定具合をみて自動で設定
	//テクスチャが１つも無い場合について迷ったが　STDで描画するとPBRの中で明るすぎて非常に浮く　テクスチャ無しもPBRで描画することに
	//#####################################################################################################

	bool withalpha = renderobj.forcewithalpha || renderobj.withalpha;
	CPolyMesh3* pm3 = nullptr;
	CPolyMesh4* pm4 = nullptr;
	pm3 = renderobj.mqoobj->GetPm3();
	pm4 = renderobj.mqoobj->GetPm4();


	bool lightflag = DecideLightFlag(renderobj);
	int shadertype = MQOSHADER_TOON;

	if (lightflag) {
		int tempshadertype;
		if (renderobj.shadertype == -2) {//shadertype == -2の場合はマテリアルの設定に従う
			tempshadertype = GetShaderType();
		}
		else {
			tempshadertype = renderobj.shadertype;
		}

		switch (tempshadertype) {
		case -1:
		case -2://マテリアルの設定も-2だった場合にはAUTOとみなす
			if (renderobj.pmodel->GetVRoidJointName()) {
				//VRoid特有のジョイント名を含み　シェーダタイプがAUTOの場合にはNOLIGHTでトゥーン風味に
				//NoLight
				shadertype = MQOSHADER_TOON;
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
						shadertype = MQOSHADER_TOON;
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
		case MQOSHADER_TOON:
			shadertype = tempshadertype;
			break;
		default:
			_ASSERT(0);
			shadertype = MQOSHADER_TOON;
			break;
		}
	}
	else {
		//ライティング無しの場合には　シェーダ関数 *NoLight*()で処理
		shadertype = MQOSHADER_TOON;
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
		case RENDERKIND_ZPREPASS:
			shaderindex = MQOSHADER_TOON;
			break;
		case RENDERKIND_INSTANCING_TRIANGLE://Instancingの場合にはこの関数は呼ばれないはず
		case RENDERKIND_INSTANCING_LINE://Instancingの場合にはこの関数は呼ばれないはず
		default:
			_ASSERT(0);
			shaderindex = MQOSHADER_TOON;
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
		case RENDERKIND_ZPREPASS:
			shaderindex = MQOSHADER_TOON;
			break;
		case RENDERKIND_INSTANCING_TRIANGLE://Instancingの場合にはこの関数は呼ばれないはず
		case RENDERKIND_INSTANCING_LINE://Instancingの場合にはこの関数は呼ばれないはず
		default:
			_ASSERT(0);
			shaderindex = MQOSHADER_TOON;
			break;
		}
		break;
	case MQOSHADER_TOON:
		switch (renderobj.renderkind) {//renderobj.renderkindはCDispObj::Render*()関数内でセット
		case RENDERKIND_NORMAL:
			shaderindex = MQOSHADER_TOON;
			break;
		case RENDERKIND_SHADOWMAP:
			shaderindex = MQOSHADER_TOON_SHADOWMAP;
			break;
		case RENDERKIND_SHADOWRECIEVER:
			shaderindex = MQOSHADER_TOON_SHADOWRECIEVER;
			break;
		case RENDERKIND_ZPREPASS:
			shaderindex = MQOSHADER_TOON;
			break;
		case RENDERKIND_INSTANCING_TRIANGLE://Instancingの場合にはこの関数は呼ばれないはず
		case RENDERKIND_INSTANCING_LINE://Instancingの場合にはこの関数は呼ばれないはず
		default:
			_ASSERT(0);
			shaderindex = MQOSHADER_TOON;
			break;
		}
		break;
	default:
		break;
	}

	return shaderindex;
}


void CMQOMaterial::BeginRender(RenderContext* rc, myRenderer::RENDEROBJ renderobj, int refposindex)
//default:refposindex = 0
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


	int shaderindex = DecideShaderIndex(renderobj);


	int currentrefposindex;
	if ((refposindex < 0) || (refposindex >= REFPOSMAXNUM)) {
		currentrefposindex = 0;
	}
	else {
		if (pm4) {
			currentrefposindex = refposindex;
		}
		else {
			currentrefposindex = 0;//!!!!!!!!! pm3の場合には　REFPOSは使用しない　REFPOSはスキニングモデルのみに限定(メモリ節約のため)
		}
	}

	if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
		(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
		(shaderindex == MQOSHADER_TOON_SHADOWMAP)) {
		rc->SetRootSignature(m_shadowrootSignature[currentrefposindex]);
	}
	else {
		rc->SetRootSignature(m_rootSignature[currentrefposindex]);
	}


	if (shaderindex >= 0) {
		if (withalpha) {
			if (renderobj.zcmpalways) {
				//###########################
				//Z cmp Always 半透明常に上書き
				//###########################
				rc->SetPipelineState(m_zalwaysPipelineState[shaderindex][currentrefposindex]);
			}
			else {
				//###################
				//translucent　半透明
				//###################
				if (renderobj.zenable) {
					rc->SetPipelineState(m_transPipelineState[shaderindex][currentrefposindex]);
				}
				else {
					rc->SetPipelineState(m_transNoZPipelineState[shaderindex][currentrefposindex]);
				}
				
			}
		}
		else {
			//##############
			//Opaque 不透明
			//##############
			rc->SetPipelineState(m_opaquePipelineState[shaderindex][currentrefposindex]);
		}
	}
	else {
		rc->SetPipelineState(m_opaquePipelineState[MQOSHADER_TOON][currentrefposindex]);
	}

	if ((shaderindex == MQOSHADER_PBR_SHADOWMAP) ||
		(shaderindex == MQOSHADER_STD_SHADOWMAP) ||
		(shaderindex == MQOSHADER_TOON_SHADOWMAP)) {
		rc->SetDescriptorHeap(m_shadowdescriptorHeap[currentrefposindex]);
	}
	else {
		rc->SetDescriptorHeap(m_descriptorHeap[currentrefposindex]);
	}
}

void CMQOMaterial::ZPreBeginRender(RenderContext* rc, myRenderer::RENDEROBJ renderobj, int refposindex)
{
	if (!rc) {
		_ASSERT(0);
		return;
	}
	CPolyMesh3* pm3 = nullptr;
	CPolyMesh4* pm4 = nullptr;
	pm3 = renderobj.mqoobj->GetPm3();
	pm4 = renderobj.mqoobj->GetPm4();


	int currentrefposindex;
	if ((refposindex < 0) || (refposindex >= REFPOSMAXNUM)) {
		currentrefposindex = 0;
	}
	else {
		if (pm4) {
			currentrefposindex = refposindex;
		}
		else {
			currentrefposindex = 0;//!!!!!!!!! pm3の場合には　REFPOSは使用しない　REFPOSはスキニングモデルのみに限定(メモリ節約のため)
		}
	}


	rc->SetRootSignature(m_ZPrerootSignature[currentrefposindex]);
	if (renderobj.pmodel && !renderobj.pmodel->GetSkyFlag()) {
		rc->SetPipelineState(m_ZPreModelPipelineState[currentrefposindex]);
	}
	else {
		rc->SetPipelineState(m_ZPreModelSkyPipelineState[currentrefposindex]);
	}
	rc->SetDescriptorHeap(m_descriptorHeap[currentrefposindex]);
}

void CMQOMaterial::InstancingBeginRender(RenderContext* rc, myRenderer::RENDEROBJ renderobj)
{
	if (!rc) {
		_ASSERT(0);
		return;
	}

	bool withalpha = renderobj.forcewithalpha || renderobj.withalpha;

	rc->SetRootSignature(m_InstancingrootSignature);

	if (renderobj.renderkind == RENDERKIND_INSTANCING_TRIANGLE) {
		rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		if (withalpha) {
			if (renderobj.zcmpalways) {
				//###########################
				//Z cmp Always 半透明常に上書き
				//###########################
				rc->SetPipelineState(m_InstancingzalwaysTrianglePipelineState);
			}
			else {
				//###################
				//translucent　半透明
				//###################
				if (renderobj.zenable) {
					rc->SetPipelineState(m_InstancingtransTrianglePipelineState);
				}
				else {
					rc->SetPipelineState(m_InstancingtransTriangleNoZPipelineState);
				}
			}
		}
		else {
			//##############
			//Opaque 不透明
			//##############
			rc->SetPipelineState(m_InstancingOpequeTrianglePipelineState);
		}
	}
	else if (renderobj.renderkind == RENDERKIND_INSTANCING_LINE) {
		rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		if (withalpha) {
			if (renderobj.zcmpalways) {
				//###########################
				//Z cmp Always 半透明常に上書き
				//###########################
				rc->SetPipelineState(m_InstancingzalwaysLinePipelineState);
			}
			else {
				//###################
				//translucent　半透明
				//###################
				rc->SetPipelineState(m_InstancingtransLinePipelineState);
			}
		}
		else {
			//##############
			//Opaque 不透明
			//##############
			rc->SetPipelineState(m_InstancingOpequeLinePipelineState);
		}
	}
	else {
		_ASSERT(0);
		rc->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		if (withalpha) {
			if (renderobj.zcmpalways) {
				//###########################
				//Z cmp Always 半透明常に上書き
				//###########################
				rc->SetPipelineState(m_InstancingzalwaysTrianglePipelineState);
			}
			else {
				//###################
				//translucent　半透明
				//###################
				if (renderobj.zenable) {
					rc->SetPipelineState(m_InstancingtransTrianglePipelineState);
				}
				else {
					rc->SetPipelineState(m_InstancingtransTriangleNoZPipelineState);
				}
			}
		}
		else {
			//##############
			//Opaque 不透明
			//##############
			rc->SetPipelineState(m_InstancingOpequeTrianglePipelineState);
		}
	}

	rc->SetDescriptorHeap(m_descriptorHeap[0]);


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
	//int result = m_diffuseMap.InitFromCustomColor(m_dif4f);

	m_hsvtoon.basehsv = m_dif4f.RGB2HSV();//!!!!!!
	int result = m_diffuseMap.InitToonFromCustomColor(&m_hsvtoon);//2024/02/13 ToonTexture
	if (result != 0) {
		_ASSERT(0);
		::MessageBoxA(NULL, "diffuseMap.InitFromCustomColor error. App must exit.",
			"CMQOMaterial::SetDiffuseTexture error", MB_OK | MB_ICONERROR);
		abort();
	}
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
	if ((objecttype == 0) || (objecttype == 1)) {
		//###########
		//pm4 || pm3
		//###########
		EXPAND_SRV_REG__START_NO = 5;
		NUM_SRV_ONE_MATERIAL = (EXPAND_SRV_REG__START_NO + MAX_MODEL_EXPAND_SRV);
		NUM_CBV_ONE_MATERIAL = 3;

		int refposindex;
		for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
			m_commonConstantBuffer[refposindex].Init(sizeof(SConstantBuffer), nullptr);
			if (objecttype == 0) {
				m_expandConstantBuffer[refposindex].Init(sizeof(SConstantBufferBoneMatrix), nullptr);
			}
			else {
				m_expandConstantBuffer[refposindex].Init(sizeof(SConstantBufferLights), nullptr);
			}
			m_expandConstantBuffer2[refposindex].Init(sizeof(SConstantBufferShadow), nullptr);
			m_shadowcommonConstantBuffer[refposindex].Init(sizeof(SConstantBuffer), nullptr);
			if (objecttype == 0) {
				m_shadowexpandConstantBuffer[refposindex].Init(sizeof(SConstantBufferBoneMatrix), nullptr);
			}
			else {
				m_shadowexpandConstantBuffer[refposindex].Init(sizeof(SConstantBufferLights), nullptr);
			}
			m_shadowexpandConstantBuffer2[refposindex].Init(sizeof(SConstantBufferShadow), nullptr);
		}

	}
	else {
		//#############
		//extline
		//#############
		EXPAND_SRV_REG__START_NO = 5;
		NUM_SRV_ONE_MATERIAL = (EXPAND_SRV_REG__START_NO + MAX_MODEL_EXPAND_SRV);
		NUM_CBV_ONE_MATERIAL = 1;

		int refposindex;
		for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
			m_commonConstantBuffer[refposindex].Init(sizeof(SConstantBuffer), nullptr);
			m_shadowcommonConstantBuffer[refposindex].Init(sizeof(SConstantBuffer), nullptr);
		}
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

	if ((objecttype == 0) || (objecttype == 1)) {//pm4 || pm3
		//ディスクリプタヒープを構築していく。
		{
			int srvNo = 0;
			int cbNo = 0;
			//ディスクリプタヒープにディスクリプタを登録していく。
			int refposindex;
			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				m_descriptorHeap[refposindex].RegistShaderResource(srvNo, GetDiffuseMap());	//アルベドに乗算するテクスチャ。
				m_descriptorHeap[refposindex].RegistShaderResource(srvNo + 1, GetAlbedoMap());//アルベドマップ。
				m_descriptorHeap[refposindex].RegistShaderResource(srvNo + 2, GetNormalMap());//法線マップ。
				m_descriptorHeap[refposindex].RegistShaderResource(srvNo + 3, GetMetalMap());//Metalマップ。
				m_descriptorHeap[refposindex].RegistShaderResource(srvNo + 4, *g_shadowmapforshader);//Shadowマップ。
			}
			//m_descriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
			//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			//	if (m_expandShaderResourceView[i]) {
			//		m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			//	}
			//}
			srvNo += NUM_SRV_ONE_MATERIAL;

			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				m_descriptorHeap[refposindex].RegistConstantBuffer(cbNo, m_commonConstantBuffer[refposindex]);
				if (m_expandConstantBuffer[refposindex].IsValid()) {
					m_descriptorHeap[refposindex].RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer[refposindex]);//BoneMatrix
				}
				if (m_expandConstantBuffer2[refposindex].IsValid()) {
					m_descriptorHeap[refposindex].RegistConstantBuffer(cbNo + 2, m_expandConstantBuffer2[refposindex]);//Shadow
				}
			}

			cbNo += NUM_CBV_ONE_MATERIAL;

			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				m_descriptorHeap[refposindex].Commit();
			}
		}

		{
			int srvNo = 0;
			int cbNo = 0;
			//ディスクリプタヒープにディスクリプタを登録していく。
			int refposindex;
			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				//ディスクリプタヒープにディスクリプタを登録していく。
				m_shadowdescriptorHeap[refposindex].RegistShaderResource(srvNo, GetDiffuseMap());//アルベドに乗算するテクスチャ。
				m_shadowdescriptorHeap[refposindex].RegistShaderResource(srvNo + 1, GetAlbedoMap());//アルベドマップ。
				m_shadowdescriptorHeap[refposindex].RegistShaderResource(srvNo + 2, GetNormalMap());//法線マップ。
				m_shadowdescriptorHeap[refposindex].RegistShaderResource(srvNo + 3, GetMetalMap());//Metalマップ。
				m_shadowdescriptorHeap[refposindex].RegistShaderResource(srvNo + 4, *g_shadowmapforshader);//Shadowマップ。
			}
			//m_shadowdescriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
			//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			//	if (m_expandShaderResourceView[i]) {
			//		m_shadowdescriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			//	}
			//}
			srvNo += NUM_SRV_ONE_MATERIAL;

			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				m_shadowdescriptorHeap[refposindex].RegistConstantBuffer(cbNo, m_shadowcommonConstantBuffer[refposindex]);
				if (m_expandConstantBuffer[refposindex].IsValid()) {
					m_shadowdescriptorHeap[refposindex].RegistConstantBuffer(cbNo + 1, m_shadowexpandConstantBuffer[refposindex]);//BoneMatrix
				}
				if (m_expandConstantBuffer2[refposindex].IsValid()) {
					m_shadowdescriptorHeap[refposindex].RegistConstantBuffer(cbNo + 2, m_shadowexpandConstantBuffer2[refposindex]);//Shadow
				}
			}
			cbNo += NUM_CBV_ONE_MATERIAL;

			m_createdescriptorflag = true;

			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				m_shadowdescriptorHeap[refposindex].Commit();
			}
		}

		m_createdescriptorflag = true;
	}
	else if (objecttype == 2) {
		//ディスクリプタヒープを構築していく。
		{
			int srvNo = 0;
			int cbNo = 0;
			//ディスクリプタヒープにディスクリプタを登録していく。

			int refposindex;
			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				m_descriptorHeap[refposindex].RegistShaderResource(srvNo, GetDiffuseMap());	//アルベドに乗算するテクスチャ。
				m_descriptorHeap[refposindex].RegistShaderResource(srvNo + 1, GetAlbedoMap());//アルベドマップ。
				m_descriptorHeap[refposindex].RegistShaderResource(srvNo + 2, GetNormalMap());//法線マップ。
				m_descriptorHeap[refposindex].RegistShaderResource(srvNo + 3, GetMetalMap());//Metalマップ。
				m_descriptorHeap[refposindex].RegistShaderResource(srvNo + 4, *g_shadowmapforshader);//Shadowマップ。
				//m_descriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
			}
			//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			//	if (m_expandShaderResourceView[i]) {
			//		m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			//	}
			//}
			srvNo += NUM_SRV_ONE_MATERIAL;

			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				m_descriptorHeap[refposindex].RegistConstantBuffer(cbNo, m_commonConstantBuffer[refposindex]);
			}
			//if (m_expandConstantBuffer.IsValid()) {
			//	m_descriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);
			//}
			cbNo += NUM_CBV_ONE_MATERIAL;

			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				m_descriptorHeap[refposindex].Commit();
			}
		}

		{
			int srvNo = 0;
			int cbNo = 0;
			//ディスクリプタヒープにディスクリプタを登録していく。
				//ディスクリプタヒープにディスクリプタを登録していく。
			int refposindex;
			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				m_shadowdescriptorHeap[refposindex].RegistShaderResource(srvNo, GetDiffuseMap());//アルベドに乗算するテクスチャ。
				m_shadowdescriptorHeap[refposindex].RegistShaderResource(srvNo + 1, GetAlbedoMap());//アルベドマップ。
				m_shadowdescriptorHeap[refposindex].RegistShaderResource(srvNo + 2, GetNormalMap());//法線マップ。
				m_shadowdescriptorHeap[refposindex].RegistShaderResource(srvNo + 3, GetMetalMap());//Metalマップ。
				m_shadowdescriptorHeap[refposindex].RegistShaderResource(srvNo + 4, *g_shadowmapforshader);//Shadowマップ。
				//m_shadowdescriptorHeap.RegistShaderResource(srvNo + 4, m_boneMatricesStructureBuffer);//ボーンのストラクチャードバッファ。
			}
			//for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			//	if (m_expandShaderResourceView[i]) {
			//		m_shadowdescriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
			//	}
			//}
			srvNo += NUM_SRV_ONE_MATERIAL;
			
			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				m_shadowdescriptorHeap[refposindex].RegistConstantBuffer(cbNo, m_shadowcommonConstantBuffer[refposindex]);
			}
			//if (m_expandConstantBuffer.IsValid()) {
			//	m_shadowdescriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);
			//}
			cbNo += NUM_CBV_ONE_MATERIAL;

			for (refposindex = 0; refposindex < REFPOSMAXNUM; refposindex++) {
				m_shadowdescriptorHeap[refposindex].Commit();
			}
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


void CMQOMaterial::SetConstLights(myRenderer::RENDEROBJ renderobj, SConstantBufferLights* pcbLights)
{
	if (!pcbLights) {
		_ASSERT(0);
		return;
	}

	int chklightindex;
	int toonlightindex, toonlightindexInShader;
	int shadowlightindex, shadowlightindexInShader;
	toonlightindex = max(0, GetToonLightIndex());
	toonlightindexInShader = -1;
	shadowlightindex = max(0, g_shadowmap_lightdir[g_shadowmap_slotno] - 1);
	shadowlightindexInShader = -1;
	for (chklightindex = 0; chklightindex < g_nNumActiveLights; chklightindex++) {
		//######################################################
		//2024/02/15
		//toonlightとshadowlightのインデックスを
		//シェーダ定数内のライト(有効なライトだけ格納)のインデックスに変換
		//######################################################
		if (g_lightNo[chklightindex] == toonlightindexInShader) {
			toonlightindexInShader = chklightindex;
		}
		if (g_lightNo[chklightindex] == shadowlightindex) {
			shadowlightindexInShader = chklightindex;
		}
	}

	pcbLights->Init();
	pcbLights->lightsnum[0] = g_nNumActiveLights;
	//pcbLights->lightsnum[1] = min(g_nNumActiveLights, max(0, toonlightindexInShader));//2024/02/15
	//pcbLights->lightsnum[2] = min(g_nNumActiveLights, max(0, shadowlightindexInShader));//2024/02/15
	
	//pcbLights->lightsnum[1] = renderobj.lightflag;//2024/03/02
	pcbLights->lightsnum[1] = (DecideLightFlag(renderobj)) ? 1 : 0;//2024/03/07

	pcbLights->lightsnum[2] = 0;//2024/03/02
	pcbLights->lightsnum[3] = (GetNormalY0Flag()) ? 1 : 0;//2024/02/19
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
	pcbLights->toonlightdir = g_lightdirforall[toonlightindex];//2024/02/15


	int fogkind = g_fogparams[g_fogindex].GetFogKind();
	pcbLights->fog.w = (float)fogkind;
	if (fogkind == 0) {
		//########
		//no fog
		//########

		//set dummy value
		pcbLights->fogcolor = g_fogparams[g_fogindex].GetDistColor();
		pcbLights->fog.x = g_fogparams[g_fogindex].GetDistNear();
		pcbLights->fog.y = g_fogparams[g_fogindex].GetDistFar();
		pcbLights->fog.z = g_fogparams[g_fogindex].GetDistRate();
	}
	else if (fogkind == 1) {
		//#########
		//dist fog
		//#########
		pcbLights->fogcolor = g_fogparams[g_fogindex].GetDistColor();
		pcbLights->fog.x = g_fogparams[g_fogindex].GetDistNear();
		pcbLights->fog.y = g_fogparams[g_fogindex].GetDistFar();
		pcbLights->fog.z = g_fogparams[g_fogindex].GetDistRate();
	}
	else if (fogkind == 2) {
		//###########
		//height fog
		//###########
		pcbLights->fogcolor = g_fogparams[g_fogindex].GetHeightColor();
		pcbLights->fog.x = 0.0f;
		pcbLights->fog.y = g_fogparams[g_fogindex].GetHeightHigh();
		pcbLights->fog.z = g_fogparams[g_fogindex].GetHeightRate();
	}
	else {
		_ASSERT(0);

		//set dummy value
		pcbLights->fogcolor = g_fogparams[g_fogindex].GetDistColor();
		pcbLights->fog.x = g_fogparams[g_fogindex].GetDistNear();
		pcbLights->fog.y = g_fogparams[g_fogindex].GetDistFar();
		pcbLights->fog.z = g_fogparams[g_fogindex].GetDistRate();
	}
}


void CMQOMaterial::SetFl4x4(myRenderer::RENDEROBJ renderobj, int refposindex)
{
	if (renderobj.pmodel && renderobj.mqoobj)
	{
		MOTINFO* curmi = 0;
		int curmotid;
		//double curframe;
		if (renderobj.pmodel->GetTopBone() && (renderobj.pmodel->GetNoBoneFlag() == false) && 
			renderobj.pmodel->ExistCurrentMotion()) {
			curmotid = renderobj.pmodel->GetCurrentMotID();
			if (curmotid > 0) {
				SetBoneMatrix(renderobj, refposindex);//CModel::SetShaderConst()でセットしたマトリックス配列をコピーするだけ
			}
		}
	}
}

void CMQOMaterial::DrawCommon(RenderContext* rc, myRenderer::RENDEROBJ renderobj,
	const Matrix& mView, const Matrix& mProj,
	int refposindex)
//default:refposindex = 0
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

	int shaderindex = DecideShaderIndex(renderobj);

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
		//rc->SetPipelineState(m_opaquePipelineState[MQOSHADER_TOON]);
		pipelineindex = shaderindex;
	}



	if ((g_shadowmap_slotno < 0) || (g_shadowmap_slotno >= SHADOWSLOTNUM)) {
		_ASSERT(0);
		g_shadowmap_slotno = 0;
	}

	int currentrefposindex;
	if ((refposindex < 0) || (refposindex >= REFPOSMAXNUM)) {
		currentrefposindex = 0;
	}
	else {
		if (ppm4) {
			currentrefposindex = refposindex;
		}
		else {
			currentrefposindex = 0;//!!!!!!!!! pm3の場合には　REFPOSは使用しない　REFPOSはスキニングモデルのみに限定(メモリ節約のため)
		}
	}


	////定数バッファを更新する。
	if (pdispline && pextline) {
		m_cb[currentrefposindex].Init();
		m_cb[currentrefposindex].mWorld = renderobj.mWorld;
		m_cb[currentrefposindex].mView = mView;
		m_cb[currentrefposindex].mProj = mProj;
		//m_cb.diffusemult = renderobj.diffusemult;
		m_cb[currentrefposindex].diffusemult = pextline->GetColor();
		m_cb[currentrefposindex].ambient = ChaVector4(GetAmb3F(), (float)GetAlphaTestClipVal());
		if (GetEnableEmission()) {
			m_cb[currentrefposindex].emission = ChaVector4(GetEmi3F() * GetEmissiveScale(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb[currentrefposindex].emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb[currentrefposindex].materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb[currentrefposindex].shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb[currentrefposindex].UVs[0] = g_uvset;
		m_cb[currentrefposindex].UVs[1] = (int)(GetUVScale().x + 0.0001);
		m_cb[currentrefposindex].UVs[2] = (int)(GetUVScale().y + 0.0001);
		m_cb[currentrefposindex].Flags[0] = renderobj.pmodel->GetSkyFlag() ? 1 : 0;
		m_cb[currentrefposindex].Flags[1] = renderobj.pmodel->GetGroundFlag() ? 1 : 0;

		m_commonConstantBuffer[currentrefposindex].CopyToVRAM(m_cb[currentrefposindex]);

	}
	else if (ppm3) {
		m_cb[currentrefposindex].Init();
		m_cb[currentrefposindex].mWorld = renderobj.mWorld;
		m_cb[currentrefposindex].mView = mView;
		m_cb[currentrefposindex].mProj = mProj;
		//m_cb.diffusemult = renderobj.diffusemult;
		if (GetTempDiffuseMultFlag()) {
			m_cb[currentrefposindex].diffusemult = GetTempDiffuseMult() * renderobj.diffusemult;
		}
		else {
			m_cb[currentrefposindex].diffusemult = renderobj.diffusemult;
		}
		m_cb[currentrefposindex].ambient = ChaVector4(GetAmb3F(), (float)GetAlphaTestClipVal());
		if (GetEnableEmission()) {
			m_cb[currentrefposindex].emission = ChaVector4(GetEmi3F() * GetEmissiveScale(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb[currentrefposindex].emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb[currentrefposindex].metalcoef = ChaVector4(GetMetalCoef(), GetSmoothCoef(), GetMetalAdd(), GetSpecularCoef());
		m_cb[currentrefposindex].materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb[currentrefposindex].shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb[currentrefposindex].UVs[0] = g_uvset;
		m_cb[currentrefposindex].UVs[1] = (int)(GetUVScale().x + 0.0001);
		m_cb[currentrefposindex].UVs[2] = (int)(GetUVScale().y + 0.0001);
		m_cb[currentrefposindex].Flags[0] = renderobj.pmodel->GetSkyFlag() ? 1 : 0;
		m_cb[currentrefposindex].Flags[1] = renderobj.pmodel->GetGroundFlag() ? 1 : 0;

		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer[currentrefposindex].CopyToVRAM(m_cb[currentrefposindex]);
		}
		else {
			m_shadowcommonConstantBuffer[currentrefposindex].CopyToVRAM(m_cb[currentrefposindex]);
		}

		//if (!GetUpdateLightsFlag(pipelineindex)) {//2023/12/04 ZAlwaysパイプライン描画のマニピュレータ表示がちらつくのでコメントアウト　パイプライン毎のフラグにすれば使える？
			SetConstLights(renderobj, &(m_cbLights[currentrefposindex]));
			SetConstShadow(&(m_cbShadow[currentrefposindex]));
			if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
				m_expandConstantBuffer[currentrefposindex].CopyToVRAM(m_cbLights[currentrefposindex]);
				m_expandConstantBuffer2[currentrefposindex].CopyToVRAM(m_cbShadow[currentrefposindex]);
			}
			else {
				m_shadowexpandConstantBuffer[currentrefposindex].CopyToVRAM(m_cbLights[currentrefposindex]);
				m_shadowexpandConstantBuffer2[currentrefposindex].CopyToVRAM(m_cbShadow[currentrefposindex]);
			}
			SetUpdateLightsFlag(pipelineindex);
		//}
	}
	else if (ppm4) {
		m_cb[currentrefposindex].Init();
		m_cb[currentrefposindex].mWorld = renderobj.mWorld;
		m_cb[currentrefposindex].mView = mView;
		m_cb[currentrefposindex].mProj = mProj;
		if (GetTempDiffuseMultFlag()) {
			m_cb[currentrefposindex].diffusemult = GetTempDiffuseMult() * renderobj.diffusemult;
		}
		else {
			m_cb[currentrefposindex].diffusemult = renderobj.diffusemult;
		}
		m_cb[currentrefposindex].ambient = ChaVector4(GetAmb3F(), (float)GetAlphaTestClipVal());
		if (GetEnableEmission()) {
			m_cb[currentrefposindex].emission = ChaVector4(GetEmi3F() * GetEmissiveScale(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb[currentrefposindex].emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb[currentrefposindex].metalcoef = ChaVector4(GetMetalCoef(), GetSmoothCoef(), GetMetalAdd(), GetSpecularCoef());
		m_cb[currentrefposindex].materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb[currentrefposindex].shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb[currentrefposindex].UVs[0] = g_uvset;
		m_cb[currentrefposindex].UVs[1] = (int)(GetUVScale().x + 0.0001);
		m_cb[currentrefposindex].UVs[2] = (int)(GetUVScale().y + 0.0001);
		m_cb[currentrefposindex].Flags[0] = renderobj.pmodel->GetSkyFlag() ? 1 : 0;
		m_cb[currentrefposindex].Flags[1] = renderobj.pmodel->GetGroundFlag() ? 1 : 0;

		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer[currentrefposindex].CopyToVRAM(m_cb[currentrefposindex]);
		}
		else {
			m_shadowcommonConstantBuffer[currentrefposindex].CopyToVRAM(m_cb[currentrefposindex]);
		}

		//if (!GetUpdateFl4x4Flag(pipelineindex)) {//2023/12/01
		////if (isfirstmaterial) {
		////if (isfirstmaterial && !GetUpdateFl4x4Flag()) {
		////if (!renderobj.pmodel->GetUpdateFl4x4Flag()) {
			SetConstLights(renderobj, &(m_cbMatrix[currentrefposindex].lights));
			if (GetRefPosFlag() == false) {
				SetFl4x4(renderobj, currentrefposindex);
			}
			SetConstShadow(&(m_cbShadow[currentrefposindex]));

			if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
				m_expandConstantBuffer[currentrefposindex].CopyToVRAM(m_cbMatrix[currentrefposindex]);
				m_expandConstantBuffer2[currentrefposindex].CopyToVRAM(m_cbShadow[currentrefposindex]);
			}
			else {
				m_shadowexpandConstantBuffer[currentrefposindex].CopyToVRAM(m_cbMatrix[currentrefposindex]);
				m_shadowexpandConstantBuffer2[currentrefposindex].CopyToVRAM(m_cbShadow[currentrefposindex]);
			}

			renderobj.pmodel->SetUpdateFl4x4Flag();
			SetUpdateFl4x4Flag(pipelineindex);
		//}
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
		m_cb[0].mWorld = renderobj.mWorld;
		m_cb[0].mView = mView;
		m_cb[0].mProj = mProj;
		//m_cb.diffusemult = renderobj.diffusemult;
		m_cb[0].diffusemult = pextline->GetColor();
		m_cb[0].ambient = ChaVector4(GetAmb3F(), (float)GetAlphaTestClipVal());
		if (GetEnableEmission()) {
			m_cb[0].emission = ChaVector4(GetEmi3F(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb[0].emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb[0].materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb[0].shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb[0].UVs[0] = g_uvset;
		m_cb[0].Flags[0] = renderobj.pmodel->GetSkyFlag() ? 1 : 0;
		m_cb[0].Flags[1] = renderobj.pmodel->GetGroundFlag() ? 1 : 0;

		m_commonConstantBuffer[0].CopyToVRAM(m_cb[0]);
	}
	else if (ppm3) {
		m_cb[0].mWorld = renderobj.mWorld;//未使用
		m_cb[0].mView = mView;
		m_cb[0].mProj = mProj;
		//m_cb.diffusemult = renderobj.diffusemult;
		if (GetTempDiffuseMultFlag()) {
			m_cb[0].diffusemult = GetTempDiffuseMult();
		}
		else {
			m_cb[0].diffusemult = renderobj.diffusemult;
		}
		m_cb[0].ambient = ChaVector4(GetAmb3F(), (float)GetAlphaTestClipVal());
		if (GetEnableEmission()) {
			m_cb[0].emission = ChaVector4(GetEmi3F(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb[0].emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb[0].materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb[0].shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb[0].UVs[0] = g_uvset;
		m_cb[0].Flags[0] = renderobj.pmodel->GetSkyFlag() ? 1 : 0;
		m_cb[0].Flags[1] = renderobj.pmodel->GetGroundFlag() ? 1 : 0;

		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer[0].CopyToVRAM(m_cb[0]);
		}
		else {
			m_shadowcommonConstantBuffer[0].CopyToVRAM(m_cb[0]);
		}
		//if (!GetUpdateLightsFlag()) {//2023/12/04 ZAlwaysパイプライン描画のマニピュレータ表示がちらつくのでコメントアウト　パイプライン毎のフラグにすれば使える？
		SetConstLights(renderobj, &(m_cbLights[0]));
		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_expandConstantBuffer[0].CopyToVRAM(m_cbLights[0]);
		}
		else {
			m_shadowexpandConstantBuffer[0].CopyToVRAM(m_cbLights[0]);
		}
		//SetUpdateLightsFlag();
	//}
	}
	else if (ppm4) {
		m_cb[0].mWorld = renderobj.mWorld;
		m_cb[0].mView = mView;
		m_cb[0].mProj = mProj;
		if (GetTempDiffuseMultFlag()) {
			m_cb[0].diffusemult = GetTempDiffuseMult() * renderobj.diffusemult;
		}
		else {
			m_cb[0].diffusemult = renderobj.diffusemult;
		}
		m_cb[0].ambient = ChaVector4(GetAmb3F(), (float)GetAlphaTestClipVal());
		if (GetEnableEmission()) {
			m_cb[0].emission = ChaVector4(GetEmi3F(), 0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		else {
			m_cb[0].emission.SetZeroVec4(0.0f);//diffuse + emissiveとするのでwは0.0にしておく
		}
		m_cb[0].materialdisprate = renderobj.pmodel->GetMaterialDispRate();
		m_cb[0].shadowmaxz = ChaVector4(
			g_shadowmap_far[g_shadowmap_slotno] * g_shadowmap_projscale[g_shadowmap_slotno],
			g_shadowmap_bias[g_shadowmap_slotno], g_shadowmap_color[g_shadowmap_slotno], 0.0f);
		m_cb[0].UVs[0] = g_uvset;
		m_cb[0].Flags[0] = renderobj.pmodel->GetSkyFlag() ? 1 : 0;
		m_cb[0].Flags[1] = renderobj.pmodel->GetGroundFlag() ? 1 : 0;

		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_commonConstantBuffer[0].CopyToVRAM(m_cb[0]);
		}
		else {
			m_shadowcommonConstantBuffer[0].CopyToVRAM(m_cb[0]);
		}
		//if (!GetUpdateFl4x4Flag()) {//2023/12/01
			//if (isfirstmaterial) {
			//if (isfirstmaterial && !GetUpdateFl4x4Flag()) {
			//if (!renderobj.pmodel->GetUpdateFl4x4Flag()) {
		SetConstLights(renderobj, &(m_cbMatrix[0].lights));
		SetFl4x4(renderobj, 0);

		if (renderobj.renderkind != RENDERKIND_SHADOWMAP) {
			m_expandConstantBuffer[0].CopyToVRAM(m_cbMatrix[0]);
		}
		else {
			m_shadowexpandConstantBuffer[0].CopyToVRAM(m_cbMatrix[0]);
		}
		renderobj.pmodel->SetUpdateFl4x4Flag();
		//SetUpdateFl4x4Flag();
	//}

	}
}


void CMQOMaterial::SetBoneMatrix(myRenderer::RENDEROBJ renderobj, int refposindex)
{
	CModel* pmodel = renderobj.pmodel;
	if (!pmodel) {
		_ASSERT(0);
		return;
	}
	if (renderobj.pmodel->GetTopBone() && (renderobj.pmodel->GetNoBoneFlag() == false) && 
		renderobj.pmodel->ExistCurrentMotion()) {
		//CModel::SetShaderConst()でセットしたマトリックス配列をコピーするだけ
		renderobj.pmodel->GetBoneMatrix(m_cbMatrix[refposindex].setfl4x4, MAXBONENUM);
	}
}

int CMQOMaterial::SetRefPosFl4x4(CModel* srcmodel, int refposindex)
{
	if (!srcmodel) {
		_ASSERT(0);
		return 1;
	}
	srcmodel->GetBoneMatrix(m_cbMatrix[refposindex].setfl4x4, MAXBONENUM);
	return 0;
}


int CMQOMaterial::RemakeDiffuseTexture()
{
	//m_hsvtoon.basehsv = m_dif4f.RGB2HSV();//!!!!!!Remake時には編集した値を使う
	int result = m_diffuseMap.WriteToonToSubResource(&m_hsvtoon, nullptr);//内容を書き換えるだけ
	if (result != 0) {
		_ASSERT(0);
		::MessageBoxA(NULL, "diffuseMap.WriteToonToSubResource error. App must exit.",
			"CMQOMaterial::RemakeDiffuseTexture error", MB_OK | MB_ICONERROR);
		abort();
	}
	return 0;
}
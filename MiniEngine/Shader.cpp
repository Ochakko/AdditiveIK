#include "stdafx.h"
#include "Shader.h"
#include <stierr.h>
#include <sstream>
#include <fstream>
#include <atlbase.h>

#include <GlobalVar.h>

namespace {
	const char* g_vsShaderModelName = "vs_5_0";	//頂点シェーダーのシェーダーモデル名。
	const char* g_psShaderModelName = "ps_5_0";	//ピクセルシェーダーのシェーダモデル名。
	const char* g_csShaderModelName = "cs_5_0";	//コンピュートシェーダーのシェーダーモデル名。
}

Shader::Shader()
{
	m_blob = nullptr;	//コンパイル済みのシェーダーデータ。
	m_dxcBlob = nullptr;	//DXCコンパイラを使用したときのシェーダーデータ。
	m_isInited = false;		//初期化済み？

}
Shader::~Shader()
{
	if (m_blob) {
		m_blob->Release();
		m_blob = nullptr;	//コンパイル済みのシェーダーデータ。
	}
	if (m_dxcBlob) {
		m_dxcBlob->Release();
		m_dxcBlob = nullptr;	//DXCコンパイラを使用したときのシェーダーデータ。
	}
	m_isInited = false;		//初期化済み？
}


int Shader::Load(const char* filePath, const char* entryFuncName, const char* shaderModel)
{
	ID3DBlob* errorBlob;
#ifdef _DEBUG
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	wchar_t wfxFilePath[256] = { L"" };
	mbstowcs(wfxFilePath, filePath, 256);
	
	auto hr = D3DCompileFromFile(wfxFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryFuncName, shaderModel, compileFlags, 0, &m_blob, &errorBlob);
	
	if (FAILED(hr)) {
		if (hr == STIERR_OBJECTNOTFOUND) {
			std::wstring errorMessage;
			errorMessage = L"指定されたfxファイルが開けませんでした。";
			errorMessage += wfxFilePath;
			MessageBoxW(nullptr, errorMessage.c_str(), L"エラー", MB_OK);
		}
		if (errorBlob) {
			static char errorMessage[10 * 1024];
			sprintf_s(errorMessage, "filePath : %ws, %s", wfxFilePath, (char*)errorBlob->GetBufferPointer());
			MessageBoxA(NULL, errorMessage, "シェーダーコンパイルエラー", MB_OK);
		}

		m_isInited = false;
	}
	else {

		m_isInited = true;
	}
	
	//2024/03/17 シェーダーコンパイルに失敗した場合には　エラーとして1を返すように
	//エラー時は呼び出し側でアプリを終了させる
	int retval;
	retval = (FAILED(hr)) ? 1 : 0;
	return retval;
}
int Shader::LoadPS(const char* filePath, const char* entryFuncName)
{
	return Load(filePath, entryFuncName, g_psShaderModelName);
}
int Shader::LoadVS(const char* filePath, const char* entryFuncName)
{
	SetCurrentDirectoryW(g_basedir);
	return Load(filePath, entryFuncName, g_vsShaderModelName);
}
int Shader::LoadCS(const char* filePath, const char* entryFuncName)
{
	SetCurrentDirectoryW(g_basedir);
	return Load(filePath, entryFuncName, g_csShaderModelName);
}
//void Shader::LoadRaytracing(const wchar_t* filePath)
//{
//	std::ifstream shaderFile(filePath);
//	if (shaderFile.good() == false){
//		std::wstring errormessage = L"シェーダーファイルのオープンに失敗しました。\n";
//		errormessage += filePath;
//		MessageBoxW( nullptr, errormessage.c_str(), L"エラー", MB_OK);
//		std::abort();
//	}
//
//	std::stringstream strStream;
//	strStream << shaderFile.rdbuf();
//	std::string shader = strStream.str();
//	//シェーダーのテキストファイルから、BLOBを作成する。
//	CComPtr<IDxcLibrary> dxclib;
//	auto hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&dxclib));
//	if (FAILED(hr)) {
//		MessageBox(nullptr, L"DXCLIBの作成に失敗しました。", L"エラー", MB_OK);
//		std::abort();
//	}
//	CComPtr< IDxcIncludeHandler> includerHandler;
//	hr = dxclib->CreateIncludeHandler(&includerHandler);
//	if (FAILED(hr)) {
//		MessageBox(nullptr, L"CreateIncludeHandlerに失敗しました。", L"エラー", MB_OK);
//		std::abort();
//	}
//	
//	//dxcコンパイラの作成。
//	CComPtr<IDxcCompiler> dxcCompiler;
//	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
//	if (FAILED(hr)) {
//		MessageBox(nullptr, L"dxcコンパイラの作成に失敗しました。", L"エラー", MB_OK);
//		std::abort();
//	}
//	//ソースコードのBLOBを作成する。
//	uint32_t codePage = CP_UTF8;
//	CComPtr< IDxcBlobEncoding> sourceBlob;
//	hr = dxclib->CreateBlobFromFile(filePath, &codePage, &sourceBlob);
//	if (FAILED(hr)) {
//		MessageBox(nullptr, L"シェーダーソースのBlobの作成に失敗しました。", L"エラー", MB_OK);
//		std::abort();
//	}
//	
//	CComPtr<IDxcIncludeHandler> dxcIncludeHandler;
//	dxclib->CreateIncludeHandler(&dxcIncludeHandler);
//	const wchar_t* args[] = {
//		L"-I Assets\\shader",
//	};
//	//コンパイル。
//	CComPtr<IDxcOperationResult> result;
//	hr = dxcCompiler->Compile(
//		sourceBlob, // pSource
//		filePath, // pSourceName
//		L"",		// pEntryPoint
//		L"lib_6_3", // pTargetProfile
//		args, 1, // pArguments, argCount
//		nullptr, 0, // pDefines, defineCount
//		dxcIncludeHandler, // pIncludeHandler
//		&result); // ppResult
//	if (SUCCEEDED(hr)) {
//		result->GetStatus(&hr);
//	}
//
//	if (FAILED(hr))
//	{
//		if (result)
//		{
//			CComPtr<IDxcBlobEncoding> errorsBlob;
//			hr = result->GetErrorBuffer(&errorsBlob);
//			if (SUCCEEDED(hr) && errorsBlob)
//			{
//				std::string errormessage = "Compilation failed with errors:\n%hs\n";
//				errormessage += (const char*)errorsBlob->GetBufferPointer();
//				MessageBoxA(nullptr, errormessage.c_str(), "エラー", MB_OK);
//
//			}
//		}
//		// Handle compilation error...
//	}
//	else {
//		result->GetResult(&m_dxcBlob);
//	}
//}
//

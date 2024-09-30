#include "stdafx.h"
#include "tkEngine.h"
#include "GraphicsEngine.h"

TkEngine* g_engine = nullptr;
class ChaScene;

TkEngine::~TkEngine()
{
	if (m_graphicsEngine) {
		delete m_graphicsEngine;
		m_graphicsEngine = nullptr;
	}

	//m_tkmFileBank.DestroyObjs();//tkmファイルバンク。
	m_shaderBank.DestroyObjs();//シェーダーバンク
	m_textureBank.DestroyObjs();//テクスチャバンク。

}
void TkEngine::Init(HWND hwnd, UINT frameBufferWidth, UINT frameBufferHeight)
{
	//グラフィックエンジンの初期化。
	m_graphicsEngine = new GraphicsEngine();
	m_graphicsEngine->Init(hwnd, frameBufferWidth, frameBufferHeight);
	//ゲームパッドの初期化。
	for (int i = 0; i < GamePad::CONNECT_PAD_MAX; i++) {
		g_pad[i] = &m_pad[i];
	}
}
void TkEngine::BeginFrame()
{
	m_graphicsEngine->BeginRender();

	//for (auto& pad : m_pad) {
	//	pad.BeginFrame();
	//	pad.Update();
	//}
	
}
void TkEngine::EndFrame(ChaScene* srcchascene)
{
	m_graphicsEngine->EndRender(srcchascene);
}
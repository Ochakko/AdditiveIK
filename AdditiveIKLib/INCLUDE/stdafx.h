

#include <Windows.h>

#ifdef CONVD3DX11
// Direct3D11 includes
#include <d3dcommon.h>
#include <dxgi.h>
#include <d3d11_1.h>
#include <d3d11_2.h>
#include <d3dcompiler.h>
#ifdef USE_DIRECT3D11_3
#include <d3d11_3.h>
#endif
#ifdef USE_DIRECT3D11_4
#include <d3d11_4.h>
#endif
// DirectXMath includes
#include <DirectXMath.h>
#include <DirectXColors.h>
// WIC includes
#include <wincodec.h>
// XInput includes
#include <xinput.h>
// HRESULT translation for Direct3D and other APIs 
#include <dxerr.h>
#endif

#include <intrin.h>


//fps�`�F�b�N�̎��ɗL���ɂ���B�������������Ȃ����[�h�B
//DXUT.cpp ��Create()����m_state.m_OverrideForceVsync = 0;//(�ʏ�1)
//DXUT.cpp ��DXUTRender3DEnvironment10()��hr = pSwapChain->Present(0, 0);//(�ʏ��Present(1, dwFlags);
//#define SPEEDTEST__

//�����O�^�C�����C���ƃI�C���[�O���t�̕`����X�L�b�v����Ƃ��ɗL���ɂ���B
//#define SKIP_EULERGRAPH__

//�����V�~�����X�L�b�v����B
//#define SKIP_BULLETSIMU__


//#########################################################################
//���݂́@���[�V�����|�C���g�̃C���f�b�N�X���������̂œǂݍ��ݎ��݂̂ɉe��
//#########################################################################
//���[�V�����|�C���g�������ɃL���b�V����L���ɂ���B
//SPEEDTEST__��SKIP_EULERGRAPH__��SKIP_BULLETSIMU__�̂R��L���ɂ�����Ԃňȉ���L���ɂ���ƌ��ʂ�������B
//������̊��ł͂P�Q�L�����N�^�[�̃��[�V�����Đ����Ŕ�ׂāA�T�{����P�T�{���ɂȂ�B
#define USE_CACHE_ONGETMOTIONPOINT__


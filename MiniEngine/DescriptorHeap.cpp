#include "stdafx.h"
#include "DescriptorHeap.h"


DescriptorHeap::DescriptorHeap()
{
	InitParams();
}
DescriptorHeap::~DescriptorHeap()
{
	//for (auto& ds : m_descriptorHeap) {
	//	if (ds) {
	//		ds->Release();
	//	}
	//}

	m_uavResources.clear();
	m_constantBuffers.clear();

	int dhindex;
	for (dhindex = 0; dhindex < 2; dhindex++) {
		ID3D12DescriptorHeap* deldh = m_descriptorHeap[dhindex];
		if (deldh) {
			deldh->Release();
		}
		m_descriptorHeap[dhindex] = nullptr;
	}
}

void DescriptorHeap::InitParams()
{
	m_numShaderResource = 0;	//シェーダーリソースの数。
	m_numConstantBuffer = 0;	//定数バッファの数。
	m_numUavResource = 0;		//アンオーダーアクセスリソースの数。
	m_numSamplerDesc = 0;		//サンプラの数。
	ZeroMemory(m_descriptorHeap, sizeof(ID3D12DescriptorHeap*) * 2);					//ディスクリプタヒープ。
	ZeroMemory(m_samplerDescs, sizeof(D3D12_SAMPLER_DESC) * MAX_SAMPLER_STATE);
	ZeroMemory(m_cbGpuDescriptorStart, sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * 2);
	ZeroMemory(m_srGpuDescriptorStart, sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * 2);
	ZeroMemory(m_uavGpuDescriptorStart, sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * 2);
	ZeroMemory(m_samplerGpuDescriptorStart, sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * 2);


	m_shaderResources.clear();
	m_uavResources.clear();
	m_constantBuffers.clear();

	//m_shaderResources.resize(MAX_SHADER_RESOURCE);
	//m_uavResources.resize(MAX_SHADER_RESOURCE);
	//m_constantBuffers.resize(MAX_CONSTANT_BUFFER);
	//for (auto& srv : m_shaderResources) {
	//	srv = nullptr;
	//}
	//for (auto& uav : m_uavResources) {
	//	uav = nullptr;
	//}
	//for (auto& cbv : m_constantBuffers) {
	//	cbv = nullptr;
	//}

	m_initflag = true;
}
void DescriptorHeap::CommitSamplerHeap()
{
	const auto& d3dDevice = g_graphicsEngine->GetD3DDevice();
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	srvHeapDesc.NumDescriptors = m_numSamplerDesc;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	for (auto& ds : m_descriptorHeap) {
		auto hr = d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&ds));
		if (FAILED(hr)) {
			MessageBox(nullptr, L"DescriptorHeap::Commit ディスクリプタヒープの作成に失敗しました。", L"エラー", MB_OK);
			std::abort();
		}
	}
	int bufferNo = 0;
	for (auto& descriptorHeap : m_descriptorHeap) {
		auto cpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		auto gpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		for (int i = 0; i < m_numSamplerDesc; i++) {
			//サンプラステートをディスクリプタヒープに登録していく。
			d3dDevice->CreateSampler(&m_samplerDescs[i], cpuHandle);
			cpuHandle.ptr += g_graphicsEngine->GetSapmerDescriptorSize();
		}
		m_samplerGpuDescriptorStart[bufferNo] = gpuHandle;
		bufferNo++;
	}

}
int g_numDescriptorHeap = 0;
void DescriptorHeap::Commit()
{
	const auto& d3dDevice = g_graphicsEngine->GetD3DDevice();
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	srvHeapDesc.NumDescriptors = m_numShaderResource + m_numConstantBuffer + m_numUavResource;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	for (auto& ds : m_descriptorHeap) {
		//if (ds != nullptr) {//dsにCreate結果を格納する　dsはCreate前はnullptr. &dsがnullptrでなければ良い
			auto hr = d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&ds));
			g_numDescriptorHeap++;
			if (FAILED(hr)) {
				_ASSERT(0);
				MessageBox(nullptr, L"DescriptorHeap::Commit ディスクリプタヒープの作成に失敗しました。", L"エラー", MB_OK);
				std::abort();
			}
		//}
	}
	//定数バッファやシェーダーリソースのディスクリプタをヒープに書き込んでいく。
	int bufferNo = 0;
	for (auto& descriptorHeap : m_descriptorHeap) {
		if (descriptorHeap != nullptr) {
			auto cpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
			auto gpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

			//定数バッファを登録していく。
			int cbnum = (int)m_constantBuffers.size();//2024/02/07
			for (int i = 0; i < m_numConstantBuffer; i++) {
				//@todo bug
				if ((i < cbnum) && (m_constantBuffers[i] != nullptr)) {//2024/02/07
					m_constantBuffers[i]->RegistConstantBufferView(cpuHandle, bufferNo);
				}
				else {
					_ASSERT(0);
				}
				//次に進める。
				cpuHandle.ptr += g_graphicsEngine->GetCbrSrvDescriptorSize();
			}

			//続いてシェーダーリソース。
			int srnum = (int)m_shaderResources.size();//2024/02/07
			for (int i = 0; i < m_numShaderResource; i++) {
				if ((i < srnum) && (m_shaderResources[i] != nullptr)) {//2024/02/07
					m_shaderResources[i]->RegistShaderResourceView(cpuHandle, bufferNo);
				}
				else {
					_ASSERT(0);
				}
				//次に進める。
				cpuHandle.ptr += g_graphicsEngine->GetCbrSrvDescriptorSize();
			}

			//続いてUAV。
			int uavnum = (int)m_uavResources.size();//2024/02/07
			for (int i = 0; i < m_numUavResource; i++) {
				if ((i < uavnum) && (m_uavResources[i] != nullptr)) {//2024/02/07
					m_uavResources[i]->RegistUnorderAccessView(cpuHandle, bufferNo);
				}
				else {
					_ASSERT(0);
				}
				//次に進める。
				cpuHandle.ptr += g_graphicsEngine->GetCbrSrvDescriptorSize();
			}

			//定数バッファのディスクリプタヒープの開始ハンドルを計算。
			m_cbGpuDescriptorStart[bufferNo] = gpuHandle;
			//シェーダーリソースのディスクリプタヒープの開始ハンドルを計算。
			m_srGpuDescriptorStart[bufferNo] = gpuHandle;
			m_srGpuDescriptorStart[bufferNo].ptr += (UINT64)g_graphicsEngine->GetCbrSrvDescriptorSize() * m_numConstantBuffer;
			//UAVリソースのディスクリプタヒープの開始ハンドルを計算。
			m_uavGpuDescriptorStart[bufferNo] = gpuHandle;
			m_uavGpuDescriptorStart[bufferNo].ptr += (UINT64)g_graphicsEngine->GetCbrSrvDescriptorSize() * (m_numShaderResource + m_numConstantBuffer);

			gpuHandle.ptr += (UINT64)g_graphicsEngine->GetCbrSrvDescriptorSize() * (m_numShaderResource + m_numConstantBuffer + m_numUavResource);

			bufferNo++;
		}
	}
}

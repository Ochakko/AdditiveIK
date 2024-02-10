#pragma once

inline ID3D12DescriptorHeap* DescriptorHeap::Get()const
{
	return m_descriptorHeap;
}
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetConstantBufferGpuDescriptorStartHandle() const
{
	return m_cbGpuDescriptorStart;
}
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetShaderResourceGpuDescriptorStartHandle() const
{
	return m_srGpuDescriptorStart;
}
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetUavResourceGpuDescriptorStartHandle() const
{
	return m_uavGpuDescriptorStart;
}
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetSamplerResourceGpuDescriptorStartHandle() const
{
	return m_samplerGpuDescriptorStart;
}

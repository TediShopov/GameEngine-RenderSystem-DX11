#ifndef __HELPERS__
#define __HELPERS__

#include <d3d11.h>

 struct MatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

template< typename U>
inline D3D11_BUFFER_DESC DefaltBufferDesc() {
	D3D11_BUFFER_DESC defaultBufferDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	defaultBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	defaultBufferDesc.ByteWidth = sizeof(U);
	defaultBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	defaultBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	defaultBufferDesc.MiscFlags = 0;
	defaultBufferDesc.StructureByteStride = 0;
	return defaultBufferDesc;
	//renderer->CreateBuffer(&shadowMatrixBufferDesc, NULL, &matrixBuffer);
}

inline D3D11_SAMPLER_DESC DefaltSamplerDesc() {
	D3D11_SAMPLER_DESC defaultSampler;
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	defaultSampler.Filter = D3D11_FILTER_ANISOTROPIC;
	defaultSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSampler.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	defaultSampler.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	defaultSampler.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	defaultSampler.MipLODBias = 0.0f;
	defaultSampler.MaxAnisotropy = 1;
	defaultSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	defaultSampler.MinLOD = 0;
	defaultSampler.MaxLOD = D3D11_FLOAT32_MAX;
	return defaultSampler;
	//renderer->CreateBuffer(&shadowMatrixBufferDesc, NULL, &matrixBuffer);
}
#endif // !__HELPERS__



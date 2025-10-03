#pragma once
#include "DefaultShader.h"
#include <d3d11.h>

class NormalMapShader :
    public DefaultShader
{
	struct DisplacementParams
	{
		float height;
		float padding[3];
	};
private:
	//Buffers

	//ID3D11SamplerState* normalMapSampler;

	ShaderSamplerParam normalSampler;
	ShaderTextureParam normalTexture;

	
	ShaderSamplerParam displacementSampler;
	ShaderTextureParam displacementTexture;

	ShaderBuffer <DisplacementParams> displacementParamsBuffer;


public:
	NormalMapShader(ID3D11Device* device, HWND hwnd) : DefaultShader(device, hwnd)
	{

		D3D11_INPUT_ELEMENT_DESC polygonLayoutWithTangents[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT , 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
					{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TANGENT", 1, DXGI_FORMAT_R32G32B32_FLOAT , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
		loadVertexShaderWLayout(L"NormalMapVertexShader.cso",polygonLayoutWithTangents,5);
		loadPixelShader(L"NormalMapShader.cso");

		normalSampler.setToStage = ShaderStage::PIXEL;
		normalSampler.setToPosition = 2;
		normalSampler.Create(device);

		normalTexture.setToStage = ShaderStage::PIXEL;
		normalTexture.setToPosition = 4;

		D3D11_SAMPLER_DESC displacementSampleDesc;
		// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
		displacementSampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		displacementSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		displacementSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		displacementSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		displacementSampleDesc.MipLODBias = 0.0f;
		displacementSampleDesc.MaxAnisotropy = 1;
		displacementSampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		displacementSampleDesc.MinLOD = 0;
		displacementSampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
		displacementSampler = ShaderSamplerParam(displacementSampleDesc);

		displacementSampler.setToStage = ShaderStage::VERTEX;
		displacementSampler.setToPosition = 0;
		displacementSampler.Create(device);


		displacementTexture.setToStage = ShaderStage::VERTEX;
		displacementTexture.setToPosition = 0;

		displacementParamsBuffer.setToStage = ShaderStage::VERTEX;
		displacementParamsBuffer.setToPosition = 1;
		displacementParamsBuffer.Create(device);
	}

	void setNormalMap(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* normalMap)
	{
		normalSampler.SetTo(deviceContext);
		normalTexture.SetTo(deviceContext, normalMap);

	}

	void setDiscplacementMap(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* displacmentMap, float height) 
	{
		displacementSampler.SetTo(deviceContext);
		displacementTexture.SetTo(deviceContext, displacmentMap);

		DisplacementParams params;
		params.height = height;
		displacementParamsBuffer.SetTo(deviceContext, &params);

	}



};


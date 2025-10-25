#pragma once
#include "DefaultShader.h"
class GizmoShader :
    public DefaultShader
{
private:
//	ShaderSamplerParam normalSampler;
//	ShaderTextureParam normalTexture;
//
//	ShaderSamplerParam displacementSampler;
//	ShaderTextureParam displacementTexture;

public:
	struct GizmosShaderData
	{
		XMFLOAT4 forcedColor;
	};

	ShaderBuffer<GizmosShaderData> GizmoDataBuffer;

	GizmoShader(ID3D11Device* device, HWND hwnd) : DefaultShader(device, hwnd)
	{
		//loadVertexShaderWLayout(L"NormalMapVertexShader.cso",polygonLayoutWithTangents,5);
		//loadVertexShader(L"NormalMapShader.cso");
		loadPixelShader(L"GizmoPixelShader.cso");

		GizmoDataBuffer.Create(device, PIXEL, 5);

		//normalSampler.setToStage = ShaderStage::PIXEL;
		//normalSampler.setToPosition = 2;
		//normalSampler.Create(device);

		//normalTexture.setToStage = ShaderStage::PIXEL;
		//normalTexture.setToPosition = 4;

//		D3D11_SAMPLER_DESC displacementSampleDesc;
//		// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
//		displacementSampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
//		displacementSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//		displacementSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//		displacementSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//		displacementSampleDesc.MipLODBias = 0.0f;
//		displacementSampleDesc.MaxAnisotropy = 1;
//		displacementSampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//		displacementSampleDesc.MinLOD = 0;
//		displacementSampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
//		displacementSampler = ShaderSamplerParam(displacementSampleDesc);
//
//		displacementSampler.setToStage = ShaderStage::VERTEX;
//		displacementSampler.setToPosition = 0;
//		displacementSampler.Create(device);
//
//
//		displacementTexture.setToStage = ShaderStage::VERTEX;
//		displacementTexture.setToPosition = 0;
//
//		displacementParamsBuffer.setToStage = ShaderStage::VERTEX;
//		displacementParamsBuffer.setToPosition = 1;
//		displacementParamsBuffer.Create(device);
	}

//	void setNormalMap(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* normalMap)
//	{
//		normalSampler.SetTo(deviceContext);
//		normalTexture.SetTo(deviceContext, normalMap);
//
//	}
//
//	void setDiscplacementMap(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* displacmentMap, float height) 
//	{
//		displacementSampler.SetTo(deviceContext);
//		displacementTexture.SetTo(deviceContext, displacmentMap);
//
//		DisplacementParams params;
//		params.height = height;
//		displacementParamsBuffer.SetTo(deviceContext, &params);
//
//	}



};


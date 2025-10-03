#pragma once
#include "DefaultShader.h"
class DisplacementShader :
    public DefaultShader
{
private:
	//Buffers

	//In this case only the height property is used
	struct WaveParameters
	{
		float time;
		float frequency;
		float height;
		float speed;
	};
	ID3D11Buffer* waveParamsBuffer;
	ID3D11SamplerState* displacementTexSampler;
public:
	DisplacementShader(ID3D11Device* device, HWND hwnd):DefaultShader(device, hwnd, L"DisplacementVertexShader.cso", L"ShadowPixelShader.cso")
	{
		initWaveParamBuffer();
		initDiscplacemntSampler();
	}

	

	~DisplacementShader()
	{
		// Release the sampler state for the displacement texture.
		if (displacementTexSampler)
		{
			displacementTexSampler->Release();
			displacementTexSampler = 0;
		}

		// Release the wave params constant buffer.
		if (waveParamsBuffer)
		{
			waveParamsBuffer->Release();
			waveParamsBuffer = 0;
		}
	}

	void initWaveParamBuffer() 
	{
		auto waveParamsBufferDesc=DefaltBufferDesc< WaveParameters>();
		renderer->CreateBuffer(&waveParamsBufferDesc, NULL, &waveParamsBuffer);
	}

	void initDiscplacemntSampler() 
	{
		auto samplerDesc=DefaltSamplerDesc();
		renderer->CreateSamplerState(&samplerDesc, &displacementTexSampler);
	}

	void setDisplacementMap(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* displacementMap,float heightModifier)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		WaveParameters* timeBufferPtr;
		deviceContext->Map(waveParamsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		timeBufferPtr = (WaveParameters*)mappedResource.pData;
		timeBufferPtr->time = 0;
		timeBufferPtr->height = heightModifier;
		timeBufferPtr->frequency = 0;
		timeBufferPtr->speed = 0;

		deviceContext->Unmap(waveParamsBuffer, 0);
		deviceContext->VSSetConstantBuffers(1, 1, &waveParamsBuffer);
		// Set shader texture resource in the pixel shader.
		deviceContext->VSSetShaderResources(0, 1, &displacementMap);
		deviceContext->VSSetSamplers(0, 1, &displacementTexSampler);
	}

	

};


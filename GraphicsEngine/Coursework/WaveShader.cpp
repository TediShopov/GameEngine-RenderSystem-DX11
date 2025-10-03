#include "WaveShader.h"

 WaveShader::WaveShader(ID3D11Device* device, HWND hwnd, const wchar_t* vs) :
	DefaultShader(device, hwnd, vs, L"LightPixelShader.cso")
{
	wavesBuffer.setToStage = ShaderStage::VERTEX;
	wavesBuffer.setToPosition = 1;
	wavesBuffer.Create(device);
}

WaveShader::~WaveShader()
{



}

void WaveShader::setWaveParams(ID3D11DeviceContext* deviceContext, MultipleWaveBuffer waveParameters)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	/*MultipleWaveBuffer* timeBufferPtr;
	deviceContext->Map(waveParamsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timeBufferPtr = (MultipleWaveBuffer*)mappedResource.pData;






	(*timeBufferPtr) = waveParameters;*/

	//float steepnesAvg = (timeBufferPtr->waves[0].steepness + timeBufferPtr->waves[1].steepness + timeBufferPtr->waves[2].steepness) / 3.0f;
	waveParameters.waves[0].steepness /= 3.0f;
	waveParameters.waves[1].steepness /= 3.0f;
	waveParameters.waves[2].steepness /= 3.0f;

	wavesBuffer.SetTo(deviceContext, &waveParameters);

	/*	timeBufferPtr->time = time;
	timeBufferPtr->wavelength = wavelength;
	timeBufferPtr->steepness = steepness;
	timeBufferPtr->speed = speed;*/
	//timeBufferPtr->XZdir = XZdir;

	/*deviceContext->Unmap(waveParamsBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &waveParamsBuffer);*/
}

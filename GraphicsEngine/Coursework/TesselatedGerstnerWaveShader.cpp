#include "TesselatedGerstnerWaveShader.h"

 TesselatedGerstnerWaveShader::TesselatedGerstnerWaveShader(ID3D11Device* device, HWND hwnd, const wchar_t* hs, const wchar_t* ds)
	:TessellationShader(device, hwnd, hs, ds)
{
	//initWaveParamBuffer();
	wavesBuffer.setToStage = ShaderStage::DOMAINS;
	wavesBuffer.setToPosition = 1;
	wavesBuffer.Create(device);

	worldMatrixAndCamera.setToStage = ShaderStage::HULL;
	worldMatrixAndCamera.setToPosition = 1;
	worldMatrixAndCamera.Create(device);
}

 void TesselatedGerstnerWaveShader::setWaveParams(ID3D11DeviceContext* deviceContext, MultipleWaveBuffer waveParameters)
{
	 for (WaveParameters& wp : waveParameters.waves)
		 
	 {
		 wp.k = 2.0f * 3.14159265358979323846f / wp.wavelength;
		 wp.c = sqrt(9.8 / wp.k) * wp.speed;
	 }
	 

	wavesBuffer.SetTo(deviceContext, &waveParameters);
}

  void TesselatedGerstnerWaveShader::setWorldPositionAndCamera(ID3D11DeviceContext* deviceContext,
	  XMMATRIX worldMatrix, XMFLOAT3 camPos, float nearPlane, float farPlane)
 {
	 WorldMatrixAndCameraBuffer a;
	 a.worldMatrix = worldMatrix;
	 a.camPos = camPos;
	 a.nearPlane = nearPlane;
	 a.farPlane = farPlane;


	 worldMatrixAndCamera.SetTo(deviceContext, &a);
 }



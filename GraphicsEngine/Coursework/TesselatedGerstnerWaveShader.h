#pragma once
#include "TessellationShader.h"
#include "WaveShader.h"

struct WorldMatrixAndCameraBuffer
{
	XMMATRIX worldMatrix;
	XMFLOAT3 camPos;
	float paddingA;
	float nearPlane;
	float farPlane;
	float paddingB;
	float paddingC;
};
class TesselatedGerstnerWaveShader :
    public TessellationShader
{
public:
	ShaderBuffer<MultipleWaveBuffer> wavesBuffer;
	ShaderBuffer<WorldMatrixAndCameraBuffer> worldMatrixAndCamera;
	TesselatedGerstnerWaveShader(ID3D11Device* device, HWND hwnd, const wchar_t* hs, const wchar_t* ds);
	void setWaveParams(ID3D11DeviceContext* deviceContext,
		MultipleWaveBuffer waveParameters);

	void setWorldPositionAndCamera(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMFLOAT3 camPos,
		float nearPlane, float farPlane);
};


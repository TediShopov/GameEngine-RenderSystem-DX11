#pragma once
#include "DefaultShader.h"

struct WaveParameters
{
	float time;
	float wavelength;
	float steepness;
	float speed;
	float XZdir[2];
	float k;
	float c;
};

struct MultipleWaveBuffer 
{
	WaveParameters waves[3];
};

class WaveShader :
    public DefaultShader
{
	public:
		ShaderBuffer<MultipleWaveBuffer> wavesBuffer;

		WaveShader(ID3D11Device* device, HWND hwnd, const wchar_t* vs);
		~WaveShader();

		void setWaveParams(ID3D11DeviceContext* deviceContext,MultipleWaveBuffer waveParameters);
};


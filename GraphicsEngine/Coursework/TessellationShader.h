// Light shader.h
// Basic single light shader setup
#pragma once

//#include "DXF.h"
#include "Material.h"
#include "DefaultShader.h"
using namespace std;
using namespace DirectX;


struct TesselationFactors
{
	float edgeTesselationFactor[4];
	float insideTesselationFactor[2];
	float padding[2];
};

//public BaseShader
class TessellationShader : public DefaultShader
{

	
public:

	TessellationShader(ID3D11Device* device, HWND hwnd, const wchar_t* hs, const wchar_t* ds);
	~TessellationShader();
	ShaderBuffer<TesselationFactors> tesselationFactorBufer;
	void setTesselationFactors(ID3D11DeviceContext* deviceContext, TesselationFactors tessFactors);
};

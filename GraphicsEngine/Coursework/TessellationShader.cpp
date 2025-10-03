// tessellation shader.cpp
#include "TessellationShader.h"
#include "LightMaterialShader.h"
#include "DefaultShader.h"


TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd
                                       , const wchar_t* hs, const wchar_t* ds
) : DefaultShader(device, hwnd)
{
	//initShader(L"TessellationVertexShader.cso", hs, ds, L"ShadowPixelShader.cso");
	loadVertexShader(L"TessellationVertexShader.cso");
	loadHullShader(hs);
	loadDomainShader(ds);
	loadPixelShader(L"ShadowPixelShader.cso");

	this->shadowMatrixBuffer.setToStage = ShaderStage::DOMAINS;
	tesselationFactorBufer.setToStage = ShaderStage::HULL;
	tesselationFactorBufer.setToPosition = 0;
	tesselationFactorBufer.Create(device);
	
}


TessellationShader::~TessellationShader()
{
	
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	DefaultShader::~DefaultShader();
}



XMFLOAT4 padToVecFour(XMFLOAT3 vec, float pad)
{
	XMFLOAT4 toReturn;
	toReturn.x = vec.x; toReturn.y = vec.y; toReturn.z = vec.z;
	toReturn.w = pad;
	return toReturn;
}
void TessellationShader::setTesselationFactors(ID3D11DeviceContext* deviceContext,TesselationFactors tessFactors)
{
	/*TesselationFactors tessBuff;
	tessBuff.edgeTesselationFactor[0] = edgeTessFactors[0];
	tessBuff.edgeTesselationFactor[1] = edgeTessFactors[1];
	tessBuff.edgeTesselationFactor[2] = edgeTessFactors[2];
	tessBuff.edgeTesselationFactor[3] = edgeTessFactors[3];

	tessBuff.insideTesselationFactor = insideFactor[0];*/
	tesselationFactorBufer.SetTo(deviceContext, &tessFactors);
}



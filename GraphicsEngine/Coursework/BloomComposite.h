#pragma once
#include "TextureShader.h"

struct BloomData {

	float bloomIntensity;
	float exposure;
	float padding[2];
};
class BloomComposite :
    public TextureShader
{

public:
	//ShaderBuffer<ViggneteMask> vignetteMask;
	//ShaderBuffer<ScreenResolutionBuffer> resolutionParams;
	ShaderBuffer<BloomData> bloomDataParam;
	BloomData bloomData;
	ShaderTextureParam extractedBlurredTexture;
	ID3D11ShaderResourceView* extractedTexture;


	BloomComposite(ID3D11Device* device, HWND hwnd) : TextureShader(device, hwnd)
	{
		loadVertexShader(L"BaseTextureVertexShader.cso");
		loadPixelShader(L"BloomComposite.cso");

		bloomDataParam.setToPosition = 2;
		bloomDataParam.setToStage = PIXEL;
		bloomDataParam.Create(device);
		
		extractedBlurredTexture.setToPosition = 1;
		extractedBlurredTexture.setToStage = PIXEL;

	}

	void setIntrinsicParams(ID3D11DeviceContext* devCon) override
	{
		bloomDataParam.SetTo(devCon, &bloomData);

		extractedBlurredTexture.SetTo(devCon, extractedTexture);
	}


//	void setParameters(ID3D11DeviceContext* devCon,ID3D11ShaderResourceView* extractedTexture,float intensity, float exposure)
//	{
//		BloomData data;
//		data.bloomIntensity = intensity;
//		data.exposure = exposure;
//		bloomDataParam.SetTo(devCon, &data);
//
//		extractedBlurredTexture.SetTo(devCon, extractedTexture);
//	}

};


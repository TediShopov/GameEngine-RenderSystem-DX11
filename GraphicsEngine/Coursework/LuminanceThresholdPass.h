#pragma once
#include "TextureShader.h"


struct ThresholdConstants {

	//Simple linear threshold above which color would be extracted and passes to the bloom shader
	float threshold;
	XMFLOAT3 padding;
};
class LuminanceThresholdPass : public TextureShader
{

public:
	ShaderBuffer<ThresholdConstants> thresholdConstants;
	ThresholdConstants thresholdData;

	LuminanceThresholdPass(ID3D11Device* device, HWND hwnd) : TextureShader(device, hwnd)
	{
		loadVertexShader(L"BaseTextureVertexShader.cso");
		loadPixelShader(L"ThresholdPass.cso");
		
		thresholdConstants.setToPosition = 2;
		thresholdConstants.setToStage = PIXEL;
		thresholdConstants.Create(device);

	}
	void setIntrinsicParams(ID3D11DeviceContext* devCon) override 
	{
		thresholdConstants.SetTo(devCon, &thresholdData);
	}


//	void setThreshold(ID3D11DeviceContext* devCon,float a)
//	{
//		ThresholdConstants data;
//		data.threshold = a;
//		thresholdConstants.SetTo(devCon, &data);
//
//	}

};


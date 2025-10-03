#pragma once
#include "TextureShader.h"
class Blur :
    public TextureShader
{
public:

	//ShaderBuffer<ScreenResolutionBuffer> screenResolutionBuffer;
	ShaderBuffer<ViggneteMask> viggneteMask;
	ViggneteMask viggnete;
	Blur(ID3D11Device* device, HWND hwnd, const wchar_t* ps) :TextureShader(device, hwnd)
	{
		loadVertexShader(L"BaseTextureVertexShader.cso");
		loadPixelShader(ps);
		viggneteMask.setToPosition = 1;
		viggneteMask.setToStage = PIXEL;
		viggneteMask.Create(device);
	}
//	void setVignette(ID3D11DeviceContext* deviceContext, ViggneteMask mask) 
//	{
//		this->viggneteMask.SetTo(deviceContext,&mask);
//	}
	void setIntrinsicParams(ID3D11DeviceContext* deviceContext) override 
	{
		//vignetteMask.SetTo(deviceContext, &mask);
		this->viggneteMask.SetTo(deviceContext, &viggnete);
	}
};


#pragma once
#include "TextureShader.h"
class MagnifyPixelShader :
    public TextureShader
{
public:
	ShaderBuffer<ViggneteMask> vignetteMask;
	ViggneteMask vignette;
	//ShaderBuffer<ScreenResolutionBuffer> resolutionParams;


	MagnifyPixelShader(ID3D11Device* device, HWND hwnd) :TextureShader(device, hwnd)
	{
		loadVertexShader(L"BaseTextureVertexShader.cso");
		loadPixelShader(L"MagnifyPixelShader.cso");

		vignetteMask.Create(device,PIXEL, 1);
	}

	void setIntrinsicParams(ID3D11DeviceContext* deviceContext) override 
	{
		//vignetteMask.SetTo(deviceContext, &mask);
		vignetteMask.SetTo(deviceContext, &vignette);
	}
	//void setVignette(ID3D11DeviceContext* deviceContext,ViggneteMask mask)
	//{
	//	vignetteMask.SetTo(deviceContext, &mask);
	//}

};


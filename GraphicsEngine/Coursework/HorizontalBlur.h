//#pragma once
//#include "TextureShader.h"
//
//
//class HorizontalBlur :
//    public TextureShader
//{
//
//	
//public:
//
//	ShaderBuffer<ScreenResolutionBuffer> screenResolutionBuffer;
//	HorizontalBlur(ID3D11Device* device, HWND hwnd) :TextureShader(device,hwnd)
//	{
//		loadVertexShader(L"BaseTextureVertexShader.cso");
//		loadPixelShader(L"HorizontalBlurPixelShader.cso");
//
//		screenResolutionBuffer.setToStage = PIXEL;
//		screenResolutionBuffer.setToPosition = 0;
//		screenResolutionBuffer.Create(device);
//	}
//
//	void setScreenResolution(ID3D11DeviceContext* deviceContext, float width, float height)
//	{
//		ScreenResolutionBuffer buff;
//		buff.width = width;
//		buff.height = height;
//
//		this->screenResolutionBuffer.SetTo(deviceContext,&buff);
//	}
//private:
//
//};
//

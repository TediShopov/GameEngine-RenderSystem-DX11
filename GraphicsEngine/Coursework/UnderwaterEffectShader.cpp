#include "UnderwaterEffectShader.h"


UnderwaterEffectShader::UnderwaterEffectShader(ID3D11Device* device, HWND hwnd) : TextureShader(device, hwnd)
{
	loadVertexShader(L"BaseTextureVertexShader.cso");
	loadPixelShader(L"UnderwaterEffectPixelShader.cso");
	

	distortionBuffer.setToPosition = 1;
	distortionBuffer.setToStage = PIXEL;
	distortionBuffer.Create(device);


	vignneteMaskBuffer.setToPosition = 2;
	vignneteMaskBuffer.setToStage = PIXEL;
	vignneteMaskBuffer.Create(device);

	sampleState.setToPosition = 0;
	sampleState.setToStage = PIXEL;
	sampleState.Create(device);

	textureParam.setToPosition = 0;
	textureParam.setToStage = PIXEL;

	textureBlurred.setToPosition = 1;
	textureBlurred.setToStage = PIXEL;
}
UnderwaterEffectShader::~UnderwaterEffectShader()
{

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void UnderwaterEffectShader::setDistortionParameters(ID3D11DeviceContext* deviceContext,
	TextureDistortionBuffer buff, 
	ViggneteMask viggneteMaskBuff)
{
	distortionBuffer.SetTo(deviceContext, &buff);
	vignneteMaskBuffer.SetTo(deviceContext, &viggneteMaskBuff);
}






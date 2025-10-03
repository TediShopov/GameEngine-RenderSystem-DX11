#pragma once



#include "TextureShader.h"
struct TextureDistortionBuffer
{
	XMFLOAT3 colorOverlay;
	float time;
	float sinXFrequency;
	float offsetX;
	float sinYFrequency;
	float offsetY;

};




class UnderwaterEffectShader : public TextureShader
{
public:
	UnderwaterEffectShader(ID3D11Device* device, HWND hwnd);
	~UnderwaterEffectShader();

	void setDistortionParameters(ID3D11DeviceContext* deviceContext,
		TextureDistortionBuffer buff,
		ViggneteMask vignetteMaskBuff);

	void setBlurredTexture(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture)
	{
		textureBlurred.SetTo(deviceContext, texture);
	}
private:
	
	ShaderBuffer<TextureDistortionBuffer> distortionBuffer;
	ShaderBuffer<ViggneteMask> vignneteMaskBuffer;

	ShaderTextureParam textureBlurred;

	
};



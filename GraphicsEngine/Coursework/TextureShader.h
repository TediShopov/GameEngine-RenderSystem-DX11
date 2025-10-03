#pragma once

#include "BaseShader.h"
#include "ShaderParameter.h"

using namespace std;
using namespace DirectX;

struct ScreenResolutionBuffer
{
	float width;
	float height;
	float padding[2];

};

struct ViggneteMask
{
	float vInnerRadius;
	float vOuterRadius;
	float vPower;
	float padding;
};


class TextureShader : public BaseShader
{

public:
	ShaderBuffer<ScreenResolutionBuffer> resolutionParams;
	TextureShader(ID3D11Device* device, HWND hwnd);
	~TextureShader();

	void setMatrices(ID3D11DeviceContext* deviceContext,
		const XMMATRIX &world,
		const XMMATRIX &view,
		const XMMATRIX &projection
		);

	void setTexture(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture);

	void setResolutionParams(ID3D11DeviceContext* deviceContext,float width, float height)
	{
		ScreenResolutionBuffer res;
		res.width=width;
		res.height = height;
		resolutionParams.SetTo(deviceContext, &res);
	}
	virtual void setIntrinsicParams(ID3D11DeviceContext* deviceContext) {};


public:

	ShaderBuffer<MatrixBufferType> matrixBuffer;
	ShaderSamplerParam sampleState;
	ShaderTextureParam textureParam;
};


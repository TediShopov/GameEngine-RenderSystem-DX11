#pragma once
#include <TextureManager.h>

#include <array>
#include "BaseShader.h"
#include "ShaderParameter.h"

struct WVPBuffer
{
	XMMATRIX World; 
	XMMATRIX View; 
	XMMATRIX Projection; 
	
};
class SkyMapShader :
    public BaseShader
{

	TextureManager* texture_manager_;
public:


	ShaderBuffer<WVPBuffer> WVP;
	ID3D11Texture2D* cubeMapTexture;
	ID3D11ShaderResourceView* cubeMapSRV;
	ShaderTextureParam CubeMapResource;
	ShaderSamplerParam CubeMapSampler;


	void LoadCubeMap(ID3D11Device* device, const std::wstring& ddsFilePath);


	bool IsCubeMap(ID3D11Texture2D* texture);

	SkyMapShader::SkyMapShader(ID3D11Device* device, HWND hwnd)
		:BaseShader(device, hwnd)
	{
		//Inittialize the necesary buffers to pass to the shader
		LoadCubeMap(device, L"res/Mountains.dds");
		newinitBuff(device);


		//Load the shaders
		this->loadVertexShader(L"SkyMapVertexShader.cso");
		this->loadPixelShader(L"SkyMapPixelShader.cso");
	}

	void newinitBuff(ID3D11Device* device);


	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection);
};


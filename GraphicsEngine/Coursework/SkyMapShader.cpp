#include "SkyMapShader.h"

void SkyMapShader::LoadCubeMap(ID3D11Device* device, const std::wstring& ddsFilePath)
{
	// Create the texture and SRV (Shader Resource View)
	HRESULT hr = DirectX::CreateDDSTextureFromFile(device, ddsFilePath.c_str(),
	                                               (ID3D11Resource**)&cubeMapTexture,
	                                               &cubeMapSRV);

	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to load DDS texture file: ");
	}

	// Verify that it's a cube map texture
	if (!IsCubeMap(cubeMapTexture))
	{
		throw std::runtime_error("The DDS texture is not a valid cube map.");
	}

	// Optionally bind the SRV to the shader (for example, slot 0 in pixel shader)
	// context->PSSetShaderResources(0, 1, cubeMapSRV.GetAddressOf());
}

bool SkyMapShader::IsCubeMap(ID3D11Texture2D* texture)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	texture->GetDesc(&textureDesc);

	// Check if it's a cube map: a cube map has an ArraySize of 6 (6 faces)
	return (textureDesc.ArraySize == 6);
}

void SkyMapShader::newinitBuff(ID3D11Device* device)
{

	HRESULT res;
	//Buffers
	WVP.setToStage = ShaderStage::VERTEX;
	res = WVP.Create(device);

	//Set cube map sampler
	CubeMapSampler.setToStage = PIXEL;
	CubeMapSampler.setToPosition = 0;
	res = CubeMapSampler.Create(device);

	//Set cube map texture resource
	CubeMapResource.setToStage= ShaderStage::PIXEL;
}

void SkyMapShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view,
	const XMMATRIX& projection)
{
	HRESULT result;

	WVPBuffer* buff = new WVPBuffer();;
	buff->World = XMMatrixTranspose(world);
	buff->View = XMMatrixTranspose(view);
	buff->Projection = XMMatrixTranspose(projection);

	WVP.SetTo(deviceContext, buff);
	CubeMapSampler.SetTo(deviceContext);
	CubeMapResource.SetTo(deviceContext, cubeMapSRV);

}

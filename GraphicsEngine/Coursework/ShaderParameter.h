#pragma once
//#include <d3d11.h>
//#include <D3Dcompiler.h>
#include "DXF.h"
#include "ShaderHelper.h"
enum ShaderStage 
{
	NONE, VERTEX, HULL, DOMAINS, GEOMETRY, PIXEL, COMPUTE
};

class ShaderParam 
{
public:
	ShaderStage setToStage = ShaderStage::NONE;
	int setToPosition = 0;
};

class ShaderSamplerParam : public ShaderParam 
{
public:
	D3D11_SAMPLER_DESC description;
	ID3D11SamplerState* sampler;

	ShaderSamplerParam()
		:description(DefaltSamplerDesc())
		,sampler(NULL)
	{

	}
	ShaderSamplerParam(D3D11_SAMPLER_DESC desc)
		:description(desc)
		,sampler(NULL)
	{

	}
	ShaderSamplerParam(ShaderStage stage, int pos)
	{
		this->setToStage = stage;
		this->setToPosition = pos;
	}

	~ShaderSamplerParam()
	{
		if (sampler)
		{
			sampler->Release();
			sampler = 0;
		}
	}

	HRESULT Create(ID3D11Device* renderer)
	{
		return renderer->CreateSamplerState(&description, &sampler);
	}

	void SetTo(ID3D11DeviceContext* deviceContext)
	{
		SetSampler(deviceContext, this->setToPosition, this->setToStage);
	}

private:
	void SetSampler(ID3D11DeviceContext* deviceContext,  int shaderResourceSetPos, ShaderStage stage = ShaderStage::NONE)
	{

		if (ShaderStage::NONE)
		{
			throw new std::exception("Invalid Shader Stage Parameter");
			return;
		}

		switch (stage)
		{

		case VERTEX:
			deviceContext->VSSetSamplers(setToPosition, 1, &sampler);
			break;
		case HULL:
			deviceContext->HSSetSamplers(setToPosition, 1, &sampler);

			break;
		case DOMAINS:
			deviceContext->DSSetSamplers(setToPosition, 1, &sampler);

			break;
		case GEOMETRY:
			deviceContext->GSSetSamplers(setToPosition, 1, &sampler);
			break;
		case PIXEL:
			deviceContext->PSSetSamplers(setToPosition, 1, &sampler);
			break;
		default:
			break;
		}
	}



};

class ShaderTextureParam : public ShaderParam
{
public:
	ShaderTextureParam()
	{

	}
	ShaderTextureParam(ShaderStage stage, int pos)
	{
		this->setToStage = stage;
		this->setToPosition = pos;
	}

	

	void SetTo(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture,int numViews=1)
	{
		SetTexture(deviceContext, texture, this->setToPosition, this->setToStage, numViews);
	}

	
private:
	 void SetTexture(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, int shaderResourceSetPos,ShaderStage stage = ShaderStage::NONE,int numViews = 1)
	{
		
		if (ShaderStage::NONE)
		{
			throw new std::exception("Invalid Shader Stage Parameter");
			return;
		}

		switch (stage)
		{

		case VERTEX:
			deviceContext->VSSetShaderResources(shaderResourceSetPos, numViews, &texture);
			break;
		case HULL:
			deviceContext->HSSetShaderResources(shaderResourceSetPos, numViews, &texture);

			break;
		case DOMAINS:
			deviceContext->DSSetShaderResources(shaderResourceSetPos, numViews, &texture);

			break;
		case GEOMETRY:
			deviceContext->GSSetShaderResources(shaderResourceSetPos, numViews, &texture);
			break;
		case PIXEL:
			deviceContext->PSSetShaderResources(shaderResourceSetPos, numViews, &texture);
			break;
		default:
			break;
		}
	}

};



template<typename T> 
class ShaderBuffer :public ShaderParam
{
public:
	D3D11_BUFFER_DESC description;
	ID3D11Buffer* buffer;

	

	ShaderBuffer()
		:description(DefaltBufferDesc<T>())
	{

	}
	ShaderBuffer(D3D11_BUFFER_DESC desc)
		:description(desc)
	{
			
	}
	ShaderBuffer(ShaderStage stage, int pos)
		:setToStage(stage), setToPosition(pos)
	{
		
	}

	~ShaderBuffer() 
	{
		if (buffer)
		{
			buffer->Release();
			buffer = 0;
		}
	}

	HRESULT Create(ID3D11Device* renderer )
	{
		
		return renderer->CreateBuffer(&description, NULL, &buffer);
	}

	void Create(ID3D11Device* renderer, ShaderStage stage, int pos)
	{
		this->setToStage = stage;
		this->setToPosition = pos;
		renderer->CreateBuffer(&description, NULL, &buffer);
	}



	void SetTo(ID3D11DeviceContext* deviceContext,T* data)
	{
		
		T* shaderDataPtr;
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		shaderDataPtr = (T*)mappedResource.pData;
		(*shaderDataPtr) = (*data);

		deviceContext->Unmap(buffer, 0);
		SetShaderBuffer(deviceContext, setToStage);
		//deviceContext->PSSetConstantBuffers(1, 1, &buffer);
	}
private:
	 void SetShaderBuffer(ID3D11DeviceContext* deviceContext,ShaderStage stage = ShaderStage::NONE)
	{
		if (ShaderStage::NONE)
		{
			throw new std::exception("Invalid Shader Stage Parameter");
			return;
		}

		switch (stage)
		{
		
		case VERTEX:
			deviceContext->VSSetConstantBuffers(setToPosition, 1, &buffer);
			break;
		case HULL:
			deviceContext->HSSetConstantBuffers(setToPosition, 1, &buffer);

			break;
		case DOMAINS:
			deviceContext->DSSetConstantBuffers(setToPosition, 1, &buffer);

			break;
		case GEOMETRY:
			deviceContext->GSSetConstantBuffers(setToPosition, 1, &buffer);
			break;
		case PIXEL:
			deviceContext->PSSetConstantBuffers(setToPosition, 1, &buffer);
			break;
		case COMPUTE:
			deviceContext->CSSetConstantBuffers(setToPosition, 1, &buffer);
			break;
		default:
			break;
		}
	}


};


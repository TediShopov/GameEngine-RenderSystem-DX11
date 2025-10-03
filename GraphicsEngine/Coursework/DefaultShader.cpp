#include "DefaultShader.h"
#include "Material.h"
#include "Camera.h"
#include "DXF.h"
//
#include "ShadowMap.h"





 void DefaultShader::loadVertexShaderWLayout(const wchar_t* filename, D3D11_INPUT_ELEMENT_DESC* polygonLayout, int numElements)
{
	ID3DBlob* vertexShaderBuffer;
	//unsigned int numElements;
	vertexShaderBuffer = 0;

	// check file extension for correct loading function.
	std::wstring fn(filename);
	std::string::size_type idx;
	std::wstring extension;

	idx = fn.rfind('.');

	if (idx != std::string::npos)
	{
		extension = fn.substr(idx + 1);
	}
	else
	{
		// No extension found
		MessageBox(hwnd, L"Error finding vertex shader file", L"ERROR", MB_OK);
		exit(0);
	}

	// Load the texture in.
	if (extension != L"cso")
	{
		MessageBox(hwnd, L"Incorrect vertex shader file type", L"ERROR", MB_OK);
		exit(0);
	}

	// Reads compiled shader into buffer (bytecode).
	HRESULT result = D3DReadFileToBlob(filename, &vertexShaderBuffer);
	if (result != S_OK)
	{
		MessageBox(NULL, filename, L"File ERROR", MB_OK);
		exit(0);
	}

	// Create the vertex shader from the buffer.
	renderer->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	// Create the vertex input layout.
	renderer->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
}

XMFLOAT4 DefaultShader::padToVecFour(XMFLOAT3 vec, float pad)
 {
	 XMFLOAT4 toReturn;
	 toReturn.x = vec.x; toReturn.y = vec.y; toReturn.z = vec.z;
	 toReturn.w = pad;
	 return toReturn;
 }

 DefaultShader::DefaultShader(ID3D11Device* device, HWND hwnd)
	:BaseShader(device, hwnd)
{
	 
	 loadCubeMap(device, L"res/Mountains.dds");
	 initBuffers(device);
	 initSamplers(device);


	 this->loadVertexShader(L"ShadowVertexShader.cso");
	 this->loadPixelShader(L"ShadowPixelShader.cso");
	//initBuffers(device);
}

 DefaultShader::DefaultShader(ID3D11Device* device, HWND hwnd, const wchar_t* vs, const wchar_t* ps)
	:BaseShader(device, hwnd)
{
	 
	 loadCubeMap(device, L"res/Mountains.dds");
	 initBuffers(device);
	 initSamplers(device);
	 this->loadVertexShader(vs);
	 this->loadPixelShader(ps);	
}

LightBufferType DefaultShader::constructLightBufferData(Light** lights)
{
	LightBufferType lightBuff;

	lightBuff.lights[0].ambient = lights[0]->getAmbientColour();
	lightBuff.lights[0].diffuse = lights[0]->getDiffuseColour();
	lightBuff.lights[0].specular = lights[0]->getSpecularColour();
	lightBuff.lights[0].position = padToVecFour(lights[0]->getPosition(), 0.0f);
	lightBuff.lights[0].direction = padToVecFour(lights[0]->getDirection(), 0.0f);
	lightBuff.lights[0].attenuationFactors = padToVecFour(lights[0]->getAttenuationFactors(), 0);
	lightBuff.lights[0].cutOffs.x =lights[0]->innerCutOff;
	lightBuff.lights[0].cutOffs.y = lights[0]->outerCutOff;
	lightBuff.lights[0].cutOffs.z = 0;
	lightBuff.lights[0].cutOffs.w = 0;

				 
	lightBuff.lights[1].ambient = lights[1]->getAmbientColour();
	lightBuff.lights[1].diffuse = lights[1]->getDiffuseColour();
	lightBuff.lights[1].specular = lights[1]->getSpecularColour();
	lightBuff.lights[1].position = padToVecFour(lights[1]->getPosition(), 0.0f);
	lightBuff.lights[1].direction = padToVecFour(lights[1]->getDirection(), 0.0f);

	lightBuff.lights[1].attenuationFactors = padToVecFour(lights[1]->getAttenuationFactors(), 0);
	lightBuff.lights[1].cutOffs.x = lights[1]->innerCutOff;
	lightBuff.lights[1].cutOffs.y = lights[1]->outerCutOff;
	lightBuff.lights[1].cutOffs.z = 0;
	lightBuff.lights[1].cutOffs.w = 0;



				 
	lightBuff.lights[2].ambient = lights[2]->getAmbientColour();
	lightBuff.lights[2].diffuse = lights[2]->getDiffuseColour();
	lightBuff.lights[2].specular = lights[2]->getSpecularColour();
				 
	XMFLOAT3 revDir = XMFLOAT3(-lights[2]->getDirection().x, -lights[2]->getDirection().y, -lights[2]->getDirection().z);
	lightBuff.lights[2].position = padToVecFour(revDir, 0);
	lightBuff.lights[2].direction = padToVecFour(lights[2]->getDirection(), 0.0f);
	lightBuff.lights[2].attenuationFactors = padToVecFour(lights[2]->getAttenuationFactors(), 0);
	lightBuff.lights[2].cutOffs.x = lights[2]->innerCutOff;
	lightBuff.lights[2].cutOffs.y = lights[2]->outerCutOff;
	lightBuff.lights[2].cutOffs.z = 0;
	lightBuff.lights[2].cutOffs.w = 0;


	return lightBuff;
}

void DefaultShader::initBuffers(ID3D11Device* device)
 {

	//Buffers
	 shadowMatrixBuffer.setToStage = ShaderStage::VERTEX;
	 shadowMatrixBuffer.Create(device);


	 lightBuffer.setToStage = ShaderStage::PIXEL;
	 lightBuffer.Create(device);

	 materialBuffer.setToStage = ShaderStage::PIXEL;
	 materialBuffer.setToPosition = 1;
	 materialBuffer.Create(device);


	 fogParameters.setToPosition = 2;
	 fogParameters.setToStage = PIXEL;
	 fogParameters.Create(device);
	 
	 ssrResource.setToPosition = 3;
	 ssrResource.setToStage = PIXEL;
	 ssrResource.Create(device);

	//Constant buffer for sending if the shadow map visualized should be color coded
	 shadowDebugBuffer.setToStage = PIXEL;
	 shadowDebugBuffer.setToPosition = 4;
	 shadowDebugBuffer.Create(device);
 }

   void DefaultShader::initSamplers(ID3D11Device* device)
  {

	   diffuseSampleState.setToStage = PIXEL;
	   diffuseSampleState.setToPosition = 0;
	   diffuseSampleState.Create(device);

	   diffuseTexture.setToStage = PIXEL;
	   diffuseTexture.setToPosition = 0;


	   shadowSampler.setToStage=PIXEL;
	   shadowSampler.setToPosition = 1;
	   shadowSampler.Create(device);




	   shadowDepthTexture.push_back(ShaderTextureParam());
	   shadowDepthTexture.push_back(ShaderTextureParam());
	   shadowDepthTexture.push_back(ShaderTextureParam());

	   for (size_t i = 0; i < shadowDepthTexture.size(); i++)
	   {
		   shadowDepthTexture[i].setToStage = PIXEL;
		   shadowDepthTexture[i].setToPosition = shadowSampler.setToPosition+i;
	   }

	   skyboxSampler.setToStage=PIXEL;
	   skyboxSampler.setToPosition = 2;
	   skyboxSampler.Create(device);

	   skyboxResource.setToStage = PIXEL;
	   skyboxResource.setToPosition = 4;


	   colorTexture.setToStage = PIXEL;
	   colorTexture.setToPosition = 5;
	   
	   depthTexture.setToStage = PIXEL;
	   depthTexture.setToPosition = 6;
	




	  
  }

void DefaultShader::setShaderParametersForInstance(
	ID3D11DeviceContext* deviceContext,
	const XMMATRIX& world, 
	const Material* material,
	ID3D11ShaderResourceView* texture
	)
{
	//Set the matrices for shadow mapping
	buff->world = XMMatrixTranspose(world);

	shadowMatrixBuffer.SetTo(deviceContext, buff);

	ShadowDebugData shadowDebugData{0  };
	if (debugVisalizeShadowMaps)
		shadowDebugData.debugVisualzeShadowMaps= 255;
	shadowDebugBuffer.SetTo(deviceContext, &shadowDebugData);

	//Additional
	// Send light data to pixel shader
	MaterialBufferType* materialBuff = new MaterialBufferType();
	materialBuff->ambient = padToVecFour(material->ambient, 1.0f);
	materialBuff->diffuse = padToVecFour(material->diffuse, 1.0f);
	materialBuff->specular = padToVecFour(material->specular, 1.0f);
	materialBuff->reflectionFactor = material->reflectionFactor;
	materialBuff->emissive = material->emissive;
	
	materialBuffer.SetTo(deviceContext, materialBuff);

	diffuseSampleState.SetTo(deviceContext);
	diffuseTexture.SetTo(deviceContext, texture);
}


void DefaultShader::setShaderParameters(ID3D11DeviceContext* deviceContext,  const XMMATRIX& view, const XMMATRIX& projection,
                                       ShadowMappingLights lightData,
                                       Light** light,
                                       XMFLOAT3 camPos,
                                       int width,
                                       int height
)
{
	HRESULT result;

	//ShadowMatrixBuffer* buff=new ShadowMatrixBuffer();;
	//buff->world = XMMatrixTranspose(world);
	buff->view = XMMatrixTranspose(view);
	buff->projection = XMMatrixTranspose(projection);

	//Set the matrices for shadow mapping
	buff->lightProjection[0] = XMMatrixTranspose(lightData.lightSourceMatrices[0].lightProjection);
	buff->lightView[0] = XMMatrixTranspose(lightData.lightSourceMatrices[0].lightView);

	buff->lightProjection[1] = XMMatrixTranspose(lightData.lightSourceMatrices[1].lightProjection);
	buff->lightView[1] = XMMatrixTranspose(lightData.lightSourceMatrices[1].lightView);

	buff->lightProjection[2] = XMMatrixTranspose(lightData.lightSourceMatrices[2].lightProjection);
	buff->lightView[2] = XMMatrixTranspose(lightData.lightSourceMatrices[2].lightView);



	shadowMatrixBuffer.SetTo(deviceContext, buff);

	LightBufferType lightBuff = constructLightBufferData(light);
	lightBuffer.SetTo(deviceContext, &lightBuff);




	//Additional
	// Send light data to pixel shader
//	MaterialBufferType* materialBuff = new MaterialBufferType();
//	materialBuff->ambient = padToVecFour(material->ambient, 1.0f);
//	materialBuff->diffuse = padToVecFour(material->diffuse, 1.0f);
//	materialBuff->specular = padToVecFour(material->specular, 1.0f);
//	materialBuff->reflectionFactor = material->reflectionFactor;
//	
//	materialBuffer.SetTo(deviceContext, materialBuff);

	
	XMVECTOR  det;
	XMMATRIX inverseProjection = XMMatrixInverse(&det,projection);
	XMMATRIX inverseView = XMMatrixInverse(&det,view);

	SSRBuffer ssrBuff =
	{
		XMMatrixTranspose(view),
		XMMatrixTranspose(projection),
		XMMatrixIdentity(),
		XMMatrixTranspose(inverseView),
		XMMatrixTranspose(inverseProjection),
		padToVecFour(camPos,1.0f),
		0,
		ssrParameters.ssrWorldLength,
		ssrParameters.ssrMaxSteps,
		ssrParameters.thickness,
		ssrParameters.resolution,
		width,
		height
	};
	ssrBuff.useSSR = 0;
	if (ssrParameters.useSSR)
	{
		ssrBuff.useSSR = 255;
	}
	else
	{
		ssrBuff.useSSR = 0;
	}

	ssrResource.SetTo(deviceContext, &ssrBuff);
	// Set shader texture resource in the pixel shader.

//	diffuseSampleState.SetTo(deviceContext);
//	diffuseTexture.SetTo(deviceContext, texture);

	// set the skybox resource
	skyboxSampler.SetTo(deviceContext);
	skyboxResource.SetTo(deviceContext, cubeMapSRV, 1);


	
}

void DefaultShader::setShaderParamsNew(
	ID3D11DeviceContext* deviceContext,
	 FPCamera* camera,
	const XMMATRIX& projection,
	CascadedShadowMaps* cascadedShadowMaps,
	Light** light,
	FogParametersType fog,
	int width,
	int height)
{
	for (size_t i = 0; i < cascadedShadowMaps->getCount(); i++)
	{
		this->setShadowMap(
			deviceContext,
			cascadedShadowMaps->getShadowMap(i)->getDepthMapSRV(),
			i);
	}

	ShadowMappingLights lightsMatriceData{
		cascadedShadowMaps->getViewMatrix(0),
		cascadedShadowMaps->getOrthoMatrix(0),
		cascadedShadowMaps->getViewMatrix(1),
		cascadedShadowMaps->getOrthoMatrix(1),
		cascadedShadowMaps->getViewMatrix(2),
		cascadedShadowMaps->getOrthoMatrix(2),
	};

	//baseShader->setShaderParams(devContext, camera, lights, shadowMapData, sceenData)



	this->setShaderParameters(deviceContext,
	                                camera->getViewMatrix(),
	                                projection,
	                                lightsMatriceData,
	                                light,
	                                camera->getPosition(),
	                                width,height);

	fog.camPos = XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 1);
	this->setFogParameters(deviceContext, fog);
}

void DefaultShader::setSSRColorAndDepthTextures(		
	ID3D11DeviceContext* deviceContext,
		ID3D11ShaderResourceView* color,
		ID3D11ShaderResourceView* depth
)
{
	//This information is only needed for SSR
	colorTexture.SetTo(deviceContext, color, 1);
	depthTexture.SetTo(deviceContext, depth, 1);
		
}

void DefaultShader::setFogParameters(ID3D11DeviceContext* deviceContext, FogParametersType fog)
{
	fogParameters.SetTo(deviceContext, &fog);
}

void DefaultShader::setShadowMap(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, int pos )
 {
	  shadowSampler.SetTo(deviceContext);
	  shadowDepthTexture[pos].SetTo(deviceContext, texture);

 }

void DefaultShader::loadCubeMap(ID3D11Device* device, const std::wstring& ddsFilePath)
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
	if (!isCubeMap(cubeMapTexture))
	{
		throw std::runtime_error("The DDS texture is not a valid cube map.");
	}

	// Optionally bind the SRV to the shader (for example, slot 0 in pixel shader)
	// context->PSSetShaderResources(0, 1, cubeMapSRV.GetAddressOf());
}

bool DefaultShader::isCubeMap(ID3D11Texture2D* texture)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	texture->GetDesc(&textureDesc);

	// Check if it's a cube map: a cube map has an ArraySize of 6 (6 faces)
	return (textureDesc.ArraySize == 6);
}

//#include "LightMaterialShader.h"
//#include "AttenuationLight.h"
//#include"Material.h"
//LightMaterialShader::LightMaterialShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
//{
//	initShader(L"LightVertexShader.cso", L"LightPixelShader.cso");
//	matrixBuffer.Create(device);
//	lightBuffer.Create(device);
//	materialBuffer.Create(device);
//}
//
// LightMaterialShader::LightMaterialShader(ID3D11Device* device, HWND hwnd, const wchar_t* vs, const wchar_t* ps) 
//	 :BaseShader(device,hwnd)
// {
//	 initShader(vs,ps);
//	 matrixBuffer.Create(device);
//	 lightBuffer.Create(device);
//	 materialBuffer.Create(device);
//
// }
//
//
//LightMaterialShader::~LightMaterialShader()
//{
//	// Release the sampler state.
//	if (sampleState)
//	{
//		sampleState->Release();
//		sampleState = 0;
//	}
//
//	// Release the matrix constant buffer.
//	/*if (matrixBuffer)
//	{
//		matrixBuffer->Release();
//		matrixBuffer = 0;
//	}*/
//
//	// Release the layout.
//	if (layout)
//	{
//		layout->Release();
//		layout = 0;
//	}
//
//	// Release the light constant buffer.
//	/*if (lightBuffer)
//	{
//		lightBuffer->Release();
//		lightBuffer = 0;
//	}*/
//
//	// Release the light constant buffer.
//	/*if (materialBuffer)
//	{
//		materialBuffer->Release();
//		materialBuffer = 0;
//	}*/
//
//	//Release base shader components
//	BaseShader::~BaseShader();
//}
//
//void LightMaterialShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
//{
//	//auto matrixBufferDesc=DefaltBufferDesc<MatrixBufferType>();
//	auto samplerDesc=DefaltSamplerDesc();
//	//auto lightBufferDesc=DefaltBufferDesc< LightBufferType>();
//	//auto materialBufferDesc=DefaltBufferDesc< MaterialBufferType>();
//
//	// Load (+ compile) shader files
//	loadVertexShader(vsFilename);
//	loadPixelShader(psFilename);
//
//	
//	//renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
//	renderer->CreateSamplerState(&samplerDesc, &sampleState);
//	//renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
//	//renderer->CreateBuffer(&materialBufferDesc, NULL, &materialBuffer);
//
//
//}
//
//
//
//
//
//void LightMaterialShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, 
//	const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, 
//	ID3D11ShaderResourceView* texture, Light** light,const Material* material, const XMFLOAT3 cameraPosition)
//{
//	HRESULT result;
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	MatrixBufferType* dataPtr;
//
//	XMMATRIX tworld, tview, tproj;
//
//
//	// Transpose the matrices to prepare them for the shader.
//	tworld = XMMatrixTranspose(worldMatrix);
//	tview = XMMatrixTranspose(viewMatrix);
//	tproj = XMMatrixTranspose(projectionMatrix);
//
//
//	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	dataPtr = (MatrixBufferType*)mappedResource.pData;
//	dataPtr->world = tworld;// worldMatrix;
//	dataPtr->view = tview;
//	dataPtr->projection = tproj;
//	deviceContext->Unmap(matrixBuffer, 0);
//	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
//
//	//Additional
//	// Send light data to pixel shader
//	LightBufferType* lightPtr;
//	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	lightPtr = (LightBufferType*)mappedResource.pData;
//	lightPtr->ambient[0] = light[0]->getAmbientColour();
//	lightPtr->diffuse[0] = light[0]->getDiffuseColour();
//	lightPtr->position[0] = padToVecFour(light[0]->getPosition(), 0.0f);
//	//lightPtr->attenuationFactors[0] = light[0]->getAttenuationFactorArray();
//
//
//	lightPtr->ambient[1] = light[1]->getAmbientColour();
//	lightPtr->diffuse[1] = light[1]->getDiffuseColour();
//	lightPtr->position[1] = padToVecFour(light[1]->getPosition(), 0);
//	//lightPtr->attenuationFactors[1] = light[0]->getAttenuationFactorArray();
//
//
//	lightPtr->ambient[2] = light[2]->getAmbientColour();
//	lightPtr->diffuse[2] = light[2]->getDiffuseColour();
//	lightPtr->position[2] = padToVecFour(light[2]->getDirection(), 0);
//	//lightPtr->attenuationFactors[2] = light[0]->getAttenuationFactorArray();
//	deviceContext->Unmap(lightBuffer, 0);
//	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);
//
//
//	//Additional
//	// Send light data to pixel shader
//	MaterialBufferType* materialBufferPtr;
//	deviceContext->Map(materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	materialBufferPtr = (MaterialBufferType*)mappedResource.pData;
//	materialBufferPtr->ambient = padToVecFour(material->ambient,1.0f);
//	materialBufferPtr->diffuse = padToVecFour(material->diffuse, 1.0f);
//	materialBufferPtr->specular = padToVecFour(material->specular, 1.0f);
//	materialBufferPtr->camPos = padToVecFour(cameraPosition, 1.0f);
//
//	deviceContext->Unmap(materialBuffer, 0);
//	deviceContext->PSSetConstantBuffers(1, 1, &materialBuffer);
//
//
//
//	// Set shader texture resource in the pixel shader.
//	deviceContext->PSSetShaderResources(0, 1, &texture);
//	deviceContext->PSSetSamplers(0, 1, &sampleState);
//}

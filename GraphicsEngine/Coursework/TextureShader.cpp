#include "TextureShader.h"

#include "ShaderHelper.h"

TextureShader::TextureShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	loadVertexShader(L"BaseTextureVertexShader.cso");
	loadPixelShader(L"BaseTexturePixelShader.cso");
	matrixBuffer.setToPosition = 0;
	matrixBuffer.setToStage = VERTEX;

	matrixBuffer.Create(device);

	sampleState.setToPosition = 0;
	sampleState.setToStage = PIXEL;

	textureParam.setToPosition = 0;
	textureParam.setToStage = PIXEL;

	resolutionParams.Create(device, PIXEL, 0);

}



TextureShader::~TextureShader()
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


void TextureShader::setMatrices(ID3D11DeviceContext* deviceContext, 
	const XMMATRIX &worldMatrix, 
	const XMMATRIX &viewMatrix, 
	const XMMATRIX &projectionMatrix)
{
	XMMATRIX tworld, tview, tproj;

	MatrixBufferType matrixBuff;
	// Transpose the matrices to prepare them for the shader.
	matrixBuff.world = XMMatrixTranspose(worldMatrix);
	matrixBuff.view = XMMatrixTranspose(viewMatrix);
	matrixBuff.projection = XMMatrixTranspose(projectionMatrix);

	matrixBuffer.SetTo(deviceContext, &matrixBuff);
}

void TextureShader::setTexture(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture) 
{
	sampleState.SetTo(deviceContext);
	textureParam.SetTo(deviceContext, texture);
}






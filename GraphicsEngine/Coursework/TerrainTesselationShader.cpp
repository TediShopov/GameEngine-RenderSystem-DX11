#include "TerrainTesselationShader.h"

TerrainTesselationShader::TerrainTesselationShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, const wchar_t* hs,
	const wchar_t* ds) : TesselatedGerstnerWaveShader(device, hwnd, hs, ds)
{
	//Default parameters for fbm
	//Default fBM terrian properties
	int byteSizeOfNoise = sizeof(NoiseParams);
	int byteSizeOfNoiseExtra = sizeof(NoiseParamsExtra);

	terrainDiplacementNormalData.displacementStrength = 1;
	terrainDiplacementNormalData.uvDensity = 10;
	terrainDiplacementNormalData.EPS = 0.01f;

	fBMParams.amplitude = 10;
	fBMParams.frequency = 5;
	fBMParams.octaves = 3;
	fBMParams.lucanarity = 2.7;
	fBMParams.gain = 0.3;
	fBMParams.NoiseOne.amplitude = 9;
	fBMParams.NoiseOne.frequency = 2.25;

	fbmBuffer.setToPosition = 1;
	fbmBuffer.setToStage = DOMAINS;
	fbmBuffer.Create(renderer);

	terrainParamsBuffers.setToStage = PIXEL;
	terrainParamsBuffers.setToPosition = 5;
	terrainParamsBuffers.Create(device);

	//Initialize default values for terrain
	TerrainParameters.amplitude = 1;
	TerrainParameters.heightOne = 0.15;
	TerrainParameters.heightTwo = 0.70;
	TerrainParameters.heightThree = 1;
	TerrainParameters.rangeColorOne = XMFLOAT3(1, 1, 0);
	TerrainParameters.rangeColorTwo = XMFLOAT3(0.2, 0.6, 0.2);
	TerrainParameters.rangeColorThree = XMFLOAT3(0.2, 0.2, 0.2);

	terrainParamsBuffers.Create(device);

	destroyedTerrainSampler.setToPosition = 0;
	destroyedTerrainSampler.setToStage = DOMAINS;
	destroyedTerrainSampler.Create(device);

	terrainTextureParam1.setToStage = PIXEL;
	terrainTextureParam2.setToStage = PIXEL;
	terrainTextureParam3.setToStage = PIXEL;
	destroyedTerrainTextureParam.setToStage = DOMAINS;

	terrainTextureParam1.setToPosition = 7;
	terrainTextureParam2.setToPosition = 8;
	terrainTextureParam3.setToPosition = 9;
	destroyedTerrainTextureParam.setToPosition = 0;

	terrainNormalParam.setToPosition = 10;
	terrainNormalParam.setToStage = PIXEL;

	terrainDisplacementParam.setToPosition = 1;
	terrainDisplacementParam.setToStage = DOMAINS;
	//Initialize detroyed terrain texture
	destroyedTerrainMaskTexture;

	terrainDisplacementNormalBuffer.setToStage = DOMAINS;
	terrainDisplacementNormalBuffer.setToPosition = 2;
	terrainDisplacementNormalBuffer.Create(device);

	//HRESULT hr = device->CheckFormatSupport(DXGI_FORMAT_R8G8B8A8_UNORM, &supportFlags);
	//Create the Stagin Texture
	D3D11_TEXTURE2D_DESC stagingDesc = {};
	stagingDesc.Width = 500;
	stagingDesc.Height = 500;
	stagingDesc.MipLevels = 1;
	stagingDesc.ArraySize = 1;
	stagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0; // NO bind flags for staging resources
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	stagingDesc.SampleDesc.Count = 1;
	stagingDesc.SampleDesc.Quality = 0;

	HRESULT res = device->CreateTexture2D(&stagingDesc, nullptr, &destroyedStaging);

	// Create a dynamic texture
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = 500;  // Texture size
	desc.Height = 500;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 32-bit RGBA desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	
	res = device->CreateTexture2D(&desc, nullptr, destroyedTerrainMaskTexture.GetAddressOf());

	if (this->destroyedTerrainMaskTexture != nullptr)
	{
		res = device->CreateShaderResourceView(destroyedTerrainMaskTexture.Get(), nullptr, &destroyedTerrainMask);
	}

	markRegionDestructed(deviceContext, 25, 25, 25);
	markRegionDestructed(deviceContext, 0, 0, 15);

	// Map and modify pixels
	loadPixelShader(L"TerrainPixelShader.cso");
}

void TerrainTesselationShader::markRegionDestructed(ID3D11DeviceContext* deviceContext, int worldX, int worldY, int radius)
{
	int imageX = worldX + 500 / 2;
	int imageY = worldY + 500 / 2;
	drawCircleOnTexture(deviceContext, imageX, imageY, radius, 255, 255, 255, 255);
}

inline void TerrainTesselationShader::drawCircleOnTexture(ID3D11DeviceContext* deviceContext, int centerX, int centerY, int radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT res = deviceContext->Map(this->destroyedStaging.Get(), 0, D3D11_MAP_READ_WRITE, 0, &mapped);
	//if (SUCCEEDED(deviceContext->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
	if (SUCCEEDED(res)) {
		uint8_t* pixels = (uint8_t*)mapped.pData;
		D3D11_TEXTURE2D_DESC desc;
		destroyedStaging->GetDesc(&desc);

		for (int y = 0; y < (int)desc.Height; y++) {
			uint8_t* row = &pixels[y * mapped.RowPitch]; // Correct row access
			for (int x = 0; x < (int)desc.Width; x++) {
				float dx = x - centerX;
				float dy = y - centerY;
				if (dx * dx + dy * dy <= radius * radius) { // Check if inside circle
					uint8_t* pixel = &row[x * 4]; // 4 bytes per pixel (RGBA)
					pixel[0] = r; // R
					pixel[1] = g; // G
					pixel[2] = b; // B
					pixel[3] = a; // A
				}
			}
		}
		deviceContext->Unmap(destroyedStaging.Get(), 0);
		deviceContext->CopyResource(destroyedTerrainMaskTexture.Get(), destroyedStaging.Get());
	}
}

void TerrainTesselationShader::drawCircleOnTextureUV(ID3D11DeviceContext* deviceContext, ID3D11Texture2D* texture, float centerU, float centerV, float relRadius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	// Convert UVs to pixel coordinates
	int centerX = static_cast<int>(centerU * desc.Width);
	int centerY = static_cast<int>(centerV * desc.Height);

	//Find radius in pixels
	float radius = relRadius * max(desc.Width, desc.Height);

	// Clamp to texture bounds
	centerX = clampi(centerX, 0, static_cast<int>(desc.Width) - 1);
	centerY = clampi(centerY, 0, static_cast<int>(desc.Height) - 1);

	// Delegate to the existing pixel-space function
	drawCircleOnTexture(deviceContext, centerX, centerY, radius, r, g, b, a);
}

void TerrainTesselationShader::sendfBMParams(ID3D11DeviceContext* deviceContext, NoiseParamsExtra fbm)
{
	fbmBuffer.SetTo(deviceContext, &fBMParams);
}

void TerrainTesselationShader::sendTerrainParams(ID3D11DeviceContext* deviceContext)
{
	terrainTextureParam1.SetTo(deviceContext, terrainTexture1);
	terrainTextureParam2.SetTo(deviceContext, terrainTexture2);
	terrainTextureParam3.SetTo(deviceContext, terrainTexture3);
	terrainNormalParam.SetTo(deviceContext, terrainNormal);
	terrainDisplacementParam.SetTo(deviceContext, terrainDisplacement);
	terrainDisplacementNormalBuffer.SetTo(deviceContext, &terrainDiplacementNormalData);

	destroyedTerrainSampler.SetTo(deviceContext);
	destroyedTerrainTextureParam.SetTo(deviceContext, destroyedTerrainMask);

	TerrainParameters.amplitude = fBMParams.amplitude;
	terrainParamsBuffers.SetTo(deviceContext, &TerrainParameters);
}

int TerrainTesselationShader::clampi(int a, int min, int max)
{
	if (a < min)
		return min;
	else if (a > max)
		return max;
	else
		return a;
}
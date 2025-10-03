#pragma once
#include "TesselatedGerstnerWaveShader.h"
#include "Terrain.h"
#include <algorithm>
#include "TerrainShader.h"
#include "wrl/client.h"
struct TerrainDisplacementAndNormal {
    //A multiplier to convert from normalized coord to uv
    float uvDensity;
    float displacementStrength;
    float EPS;
    float debugColor;
};


class TerrainTesselationShader :
    public TesselatedGerstnerWaveShader
{
	
private:

	//Buffers
	ShaderBuffer<NoiseParamsExtra> fbmBuffer;
	ShaderBuffer <TerrainParams> terrainParamsBuffers;
	ShaderBuffer<WorldMatrixAndCameraBuffer> worldMatrixAndCamera;
	ShaderBuffer<TerrainDisplacementAndNormal> terrainDisplacementNormalBuffer;

	//Extra Terrain Textures
	ShaderTextureParam terrainTextureParam1;
	ShaderTextureParam terrainTextureParam2;
	ShaderTextureParam terrainTextureParam3;
	ShaderTextureParam terrainNormalParam;
	ShaderTextureParam terrainDisplacementParam;
	ShaderTextureParam destroyedTerrainTextureParam;

	ShaderSamplerParam destroyedTerrainSampler;
public:
	//ID3D11Texture2D* destroyedStaging;
	//ID3D11Texture2D* destroyedTerrainMaskTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> destroyedStaging;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> destroyedTerrainMaskTexture;

	TerrainDisplacementAndNormal terrainDiplacementNormalData;

	TerrainParams TerrainParameters;
	NoiseParamsExtra fBMParams;

	ID3D11ShaderResourceView* terrainTexture1;
	ID3D11ShaderResourceView* terrainTexture2;
	ID3D11ShaderResourceView* terrainTexture3;
	ID3D11ShaderResourceView* terrainNormal;
	ID3D11ShaderResourceView* terrainDisplacement;

	ID3D11ShaderResourceView* destroyedTerrainMask;

	TerrainTesselationShader(ID3D11Device* device, ID3D11DeviceContext* context,HWND hwnd, const wchar_t* hs,
	const wchar_t* ds);

	void markRegionDestructed( ID3D11DeviceContext* deviceContext, int centerX, int centerY, int radius );

	void drawCircleOnTexture(
    ID3D11DeviceContext* deviceContext,
    int centerX, int centerY,
    int radius,
    uint8_t r, uint8_t g, uint8_t b, uint8_t a
);
	 void drawCircleOnTextureUV(
    ID3D11DeviceContext* deviceContext,
    ID3D11Texture2D* texture,
    float centerU, float centerV,  // UV coordinates [0, 1]
    float radius,
    uint8_t r, uint8_t g, uint8_t b, uint8_t a
);
	int clampi(int a, int min, int max);
	void sendfBMParams(ID3D11DeviceContext* deviceContext,NoiseParamsExtra fbm);
	void sendTerrainParams(ID3D11DeviceContext* deviceContext);
};


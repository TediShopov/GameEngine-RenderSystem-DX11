#pragma once
#include "DefaultShader.h"
class Terrain;


	struct TerrainParams
	{
		//Fout different heights associated with different colors
		float heightOne;
		XMFLOAT3 rangeColorOne;
		float heightTwo;
		XMFLOAT3 rangeColorTwo;
		float heightThree;
		XMFLOAT3 rangeColorThree;
		float amplitude;
		XMFLOAT3 padding;
	};
class TerrainShader :
    public DefaultShader
{

private:
	//Buffers
	ShaderBuffer <TerrainParams> terrainParamsBuffers;
public:
	TerrainParams TerrainParameters;
	TerrainShader(ID3D11Device* device, HWND hwnd) : DefaultShader(device, hwnd)
	{
		loadVertexShader(L"TerrainVertexShader.cso");
		loadPixelShader(L"TerrainPixelShader.cso");

		terrainParamsBuffers.setToStage = VERTEX;
		terrainParamsBuffers.setToPosition = 1;
		terrainParamsBuffers.Create(device);

		//Initialize default values for terrain
		TerrainParameters.amplitude = 1;
		TerrainParameters.heightOne = 0.15;
		TerrainParameters.heightTwo = 0.70;
		TerrainParameters.heightThree = 1;
		TerrainParameters.rangeColorOne = XMFLOAT3(1,1,0);
		TerrainParameters.rangeColorTwo = XMFLOAT3(0.2,0.6,0.2);
		TerrainParameters.rangeColorThree = XMFLOAT3(0.2,0.2,0.2);

	}
	void SetTerrainData(ID3D11DeviceContext* device,Terrain* terrain);
};



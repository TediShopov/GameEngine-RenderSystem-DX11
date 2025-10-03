#pragma once
#include <d3d11.h>
#include <queue>
#include <directxtk/SimpleMath.h>
#include <vector>
#include "FastNoiseLite.h"
#include "BaseMesh.h"
#include "Transform.h"

struct FractalBrownianMotion
{
	int octaves;		//the different iterations of noise
	float lucanarity;	//the frequency increment  each regular step (multiplier)
	float gain;			//the amplitude decrease  each regular step (multiplier)
};

//These structs are also aligned accordingly to send to the GPU
struct alignas(16) ExtraNoiseParams
{
	float amplitude;
	float frequency;
	XMFLOAT2 padding;
};
struct NoiseParams
{
	float amplitude;
	float frequency;
	int octaves;
	float lucanarity;
	float gain;
	XMFLOAT3 padding;
};
struct alignas(16) NoiseParamsExtra
{
	float amplitude;
	float frequency;
	int octaves;
	float lucanarity;
	float gain;
	float padding;

	ExtraNoiseParams NoiseOne;
	ExtraNoiseParams NoiseTwo;
	ExtraNoiseParams NoiseThree;
	ExtraNoiseParams NoiseFour;
	ExtraNoiseParams NoiseFive;
};

class Terrain : public BaseMesh
{
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};

public:

	float sampleScale = 0;
	float offsetX = 0;
	float offsetY = 0;

	int subdivisions = 1;
	int worldSize = 1;


	FastNoiseLite noise;
	int noiseSeed = 1234123;
	float noiseFrequency = 1;
	float noiseOctaveCount;
	float uvTilesAcross = 5.0f;		
	float noiseDomainScale = 1.0 / 10.0;	//Multiplier to translate and index of the terrain map to the Perlin Noise Space

	FractalBrownianMotion fBM;
	NoiseParamsExtra extraNoise;

	//Mesh above water plane
	ID3D11Buffer* AWPVertexBuffer, * AWPIndexBuffer;
	std::vector<VertexType> AWPVertices;
	std::vector<unsigned long> AWPIndices;

	ID3D11Texture2D* destroyedStaging;
	ID3D11Texture2D* destroyedTexture;
	D3D11_MAPPED_SUBRESOURCE destroyedStaginMapped;

	Terrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~Terrain();


	//Sampling from destroyed texture
	//The resource must already be mapped at destroyedStagginMapped
	float sampleDestroyedUV(float u, float v);
	float sampleDestroyedXY(int x, int y);
	float sampleDestroyedWorldXY(int worldX, int worldY);

	void initBuffers(ID3D11Device*) override;
	void initBufferAboveWaterPlane(ID3D11Device* device);

	void randomizeSeed();

	bool Initialize(ID3D11Device*, int targetWidth, int targetHeight, float subdivisions);

	bool generateHeightMap(ID3D11Device*);
	bool generateHeightMapfBM(ID3D11Device*);
	bool generateHeightMapPerlin(ID3D11Device*);

	bool generateHeightMapExtra(ID3D11Device* device, float offsetX = 0, float offsetY = 0, float sampleScale = 1);
	float sampleHeightExtra(float x, float y);
	float getMaxHeightExtra();

	int findFirstPeakAbove(float h_min);
	//Extract a peak above some level. BFS function to extract a peak region
	int getHeightmapIndexFromIntersection(float vertexX, float vertexY);
	XMFLOAT2 getLocalHeightmapCoords(float vertexX, float vertexY);

	vector<int> extractPeakBFS(int startIndex, float h_min);
	pair<int, int> extractBoundingBoxPeak(const vector<int>& peakIndices);
	Terrain* extractPeakTerrainSubregion(XMVECTOR intersection, float heightMin = 1, int targetDimensionSize = 50);

	void extractRegion(Terrain*& peakTerrain, int& diffX, int& diffY, float OffsetX, float OffsetY, float detail);
	void removeRegion(int x, int y, int diffX, int diffY);

	bool smoothHeightMap(ID3D11Device*);
	bool update();

	void assignVerticeFromHeightmap(int index, int indexUL);
	float* getWavelength();
	float* getAmplitude();
	VertexType assignVerticeFromHeightmap(int indexUL);
	VertexType getHeightMapVertex(int index);

	int getMaxDimension()
	{
		return max(this->m_heightmapHeight, this->m_heightmapWidth);
	}
protected:
	//void assignVertexDataFromHeightmap(int index, int index3);
	HRESULT createOrUpdateBufferData(ID3D11Buffer** buffer, D3D11_BUFFER_DESC buffer_desc, D3D11_SUBRESOURCE_DATA data);

	bool calculateNormals();
	void shutdown();
	void shutdownBuffers();
	//void RenderBuffers(ID3D11DeviceContext*);

private:
	bool m_terrainGeneratedToggle;
	int m_heightmapWidth, m_heightmapHeight;
	int m_terrainWidth, m_terrainHeight;

	//ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;
	float m_frequency, m_amplitude, m_wavelength;
	HeightMapType* m_heightMap;
	HeightMapType* m_smoothedHeightMap;

	inline int getHeightmapIndex(int col, int row);
	inline float getHeightAt(int col, int row);


	float getHeight(FastNoiseLite noise, float x, float y, float freq, float amp);

	//Adapted From https://thebookofshaders.com/13/
	//arrays for our generated objects Made by directX
	//std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;
};

#include "Terrain.h"

using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;

float Terrain::getHeight(FastNoiseLite noise, float x, float y, float freq, float amp)
{
	return noise.GetNoise(x * freq, y * freq) * amp;
}

float Terrain::getHeightAt(int col, int row)
{
	return m_heightMap[getHeightmapIndex(col, row)].y;
}

int Terrain::getHeightmapIndex(int col, int row)
{
	return m_heightmapWidth * row + col;
}

VertexType Terrain::getHeightMapVertex(int index) {
	VertexType v;
	HeightMapType v1 = m_heightMap[index];
	v.position.x = v1.x;
	v.position.y = v1.y;
	v.position.z = v1.z;
	return v;
}

Terrain::Terrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext) : BaseMesh(device, deviceContext)
{
	m_terrainGeneratedToggle = false;
	//Default fBM terrian properties
	noiseDomainScale = 0.03;

	m_amplitude = 10;
	fBM.octaves = 6;
	fBM.lucanarity = 1.8;
	fBM.gain = 0.6;

	this->sampleScale = 1.0;
	this->offsetX = 0.0;
	this->offsetY = 0.0;
}

//Sampling from destroyed texture
//The resource must already be mapped at destroyedStagginMapped

inline float Terrain::sampleDestroyedUV(float u, float v)
{
	// First you need the texture dimensions
	D3D11_TEXTURE2D_DESC desc;
	destroyedStaging->GetDesc(&desc);
	UINT width = desc.Width;
	UINT height = desc.Height;

	// Convert UV to pixel coordinates
	UINT pixelX = (UINT)(u * width);
	UINT pixelY = (UINT)(v * height);

	// Clamp if needed
	pixelX = min(pixelX, width - 1);
	pixelY = min(pixelY, height - 1);

	return sampleDestroyedXY(pixelX, pixelY);
}

inline float Terrain::sampleDestroyedXY(int x, int y)
{
	// First you need the texture dimensions
	D3D11_TEXTURE2D_DESC desc;
	destroyedStaging->GetDesc(&desc);
	UINT width = desc.Width;
	UINT height = desc.Height;

	// Now access pixel
	uint8_t* data = (uint8_t*)destroyedStaginMapped.pData;
	uint32_t rowPitch = destroyedStaginMapped.RowPitch; // bytes per row

	// Suppose the texture format is DXGI_FORMAT_R8G8B8A8_UNORM (4 bytes per pixel)
	uint32_t bytesPerPixel = 4;

	uint8_t* pixelPtr = data + x * rowPitch + y * bytesPerPixel;

	return ((float)pixelPtr[0]) / 255.0f;
}

float Terrain::sampleDestroyedWorldXY(int worldX, int worldY)
{
	worldX = worldX + 500 / 2;
	worldY = worldY + 500 / 2;
	return sampleDestroyedXY(worldX, worldY);
}

void Terrain::assignVerticeFromHeightmap(int index, int indexUL)
{
	vertices[index].position = Vector3(m_heightMap[indexUL].x, m_heightMap[indexUL].y, m_heightMap[indexUL].z);
	vertices[index].normal = Vector3(m_heightMap[indexUL].nx, m_heightMap[indexUL].ny, m_heightMap[indexUL].nz);
	vertices[index].texture = Vector2(m_heightMap[indexUL].u, m_heightMap[indexUL].v);
	indices[index] = index;
}

VertexType Terrain::assignVerticeFromHeightmap(int indexUL)
{
	VertexType vertex;
	vertex.position = Vector3(m_heightMap[indexUL].x, m_heightMap[indexUL].y, m_heightMap[indexUL].z);
	vertex.normal = Vector3(m_heightMap[indexUL].nx, m_heightMap[indexUL].ny, m_heightMap[indexUL].nz);
	vertex.texture = Vector2(m_heightMap[indexUL].u, m_heightMap[indexUL].v);
	return vertex;
}

HRESULT Terrain::createOrUpdateBufferData(ID3D11Buffer** buffer, D3D11_BUFFER_DESC buffer_desc, D3D11_SUBRESOURCE_DATA data)
{
	return device->CreateBuffer(&buffer_desc, &data, buffer);
}

void Terrain::initBuffers(ID3D11Device*)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index, i, j;
	int indexBottomLeft, indexBottomRight, indexUpperLeft, indexUpperRight; //geometric indices.

	// Calculate the number of vertices in the terrain mesh.

	vertexCount = (m_heightmapWidth - 1) * (m_heightmapHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	indexCount = vertexCount;

	//Clear and initialize with default values
	this->vertices.clear();
	this->indices.clear();
	for (int i = 0; i < vertexCount; ++i)
	{
		vertices.push_back(VertexType());
		indices.push_back(0);
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	for (j = 0; j < (m_heightmapHeight - 1); j++)
	{
		for (i = 0; i < (m_heightmapWidth - 1); i++)
		{
			indexBottomLeft = (m_heightmapHeight * j) + i;
			indexBottomRight = (m_heightmapHeight * j) + (i + 1);
			indexUpperLeft = (m_heightmapHeight * (j + 1)) + i;
			indexUpperRight = (m_heightmapHeight * (j + 1)) + (i + 1);
			//Initialize the patch by creating two triangles

			//Triangle 1 --> Upper Right, Upper Left, Bottom Left (CCW)
			assignVerticeFromHeightmap(index, indexUpperRight);
			index++;
			assignVerticeFromHeightmap(index, indexUpperLeft);
			index++;
			assignVerticeFromHeightmap(index, indexBottomLeft);
			index++;
			//Triangle 2 -->   Bottom Left,Bottom Right,Upper Right (CCW)

			assignVerticeFromHeightmap(index, indexBottomLeft);
			index++;
			assignVerticeFromHeightmap(index, indexBottomRight);
			index++;
			assignVerticeFromHeightmap(index, indexUpperRight);
			index++;
		}
	}
	//Terrain Cube a structure putting walls on the sides of terrain so it reamins closed
	//Think a regular cube, but the surface of the terrain is applied on the top face

	//Front side
	//float minAmplitude = -getMaxHeightExtra();
	//float terrainCubeBottom = 2 * minAmplitude;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = createOrUpdateBufferData(&vertexBuffer, vertexBufferDesc, vertexData);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = createOrUpdateBufferData(&indexBuffer, indexBufferDesc, indexData);
}

void Terrain::initBufferAboveWaterPlane(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index, i, j;
	int indexBottomLeft, indexBottomRight, indexUpperLeft, indexUpperRight; //geometric indices.

	// Calculate the number of vertices in the terrain mesh.

	vertexCount = (m_heightmapWidth - 1) * (m_heightmapHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	indexCount = vertexCount;

	//Clear and initialize with default values
	this->AWPVertices.clear();
	this->AWPIndices.clear();

	// Initialize the index to the vertex buffer.
	index = 0;

	for (j = 0; j < (m_heightmapHeight - 1); j++)
	{
		for (i = 0; i < (m_heightmapWidth - 1); i++)
		{
			indexBottomLeft = (m_heightmapHeight * j) + i;

			indexBottomLeft = (m_heightmapHeight * j) + i;
			indexBottomRight = (m_heightmapHeight * j) + (i + 1);
			indexUpperLeft = (m_heightmapHeight * (j + 1)) + i;
			indexUpperRight = (m_heightmapHeight * (j + 1)) + (i + 1);

			VertexType vertexBottomLeft = assignVerticeFromHeightmap(indexBottomLeft);
			VertexType vertexBottomRight = assignVerticeFromHeightmap(indexBottomRight);
			VertexType vertexUpperLeft = assignVerticeFromHeightmap(indexUpperLeft);
			VertexType vertexUpperRight = assignVerticeFromHeightmap(indexUpperRight);

			float heighWP = 5;

			if (vertexBottomRight.position.y > heighWP
				|| vertexUpperLeft.position.y > heighWP
				|| vertexBottomLeft.position.y > heighWP
				|| vertexUpperRight.position.y > heighWP
				)
			{
				//Initialize the patch by creating two triangles

				//Triangle 1 --> Upper Right, Upper Left, Bottom Left (CCW)
				assignVerticeFromHeightmap(index, indexUpperRight);
				index++;
				assignVerticeFromHeightmap(index, indexUpperLeft);
				index++;
				assignVerticeFromHeightmap(index, indexBottomLeft);
				index++;
				//Triangle 2 -->   Bottom Left,Bottom Right,Upper Right (CCW)

				assignVerticeFromHeightmap(index, indexBottomLeft);
				index++;
				assignVerticeFromHeightmap(index, indexBottomRight);
				index++;
				assignVerticeFromHeightmap(index, indexUpperRight);
				index++;
			}
		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = createOrUpdateBufferData(&vertexBuffer, vertexBufferDesc, vertexData);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = createOrUpdateBufferData(&indexBuffer, indexBufferDesc, indexData);
}

void Terrain::randomizeSeed()
{
	this->noiseSeed = rand();
}

bool Terrain::Initialize(ID3D11Device*, int terrainWidth, int terrainHeight, float detail)
{
	int index;
	float height = 0.0;
	bool result;

	this->m_terrainWidth = terrainWidth;
	this->m_terrainHeight = terrainHeight;
	//The rows and columsn of the terrain heightmap are equal to the subdivison
	// of their respective dimensions
	m_heightmapWidth = detail;
	m_heightmapHeight = detail;

	m_frequency = m_heightmapWidth / 20;
	m_amplitude = 3.0;
	m_wavelength = 1;

	// Create the structure to hold the terrain data.
	m_heightMap = new HeightMapType[m_heightmapWidth * m_heightmapHeight];
	m_smoothedHeightMap = new HeightMapType[m_heightmapWidth * m_heightmapHeight];
	if (!m_heightMap)
	{
		return false;
	}

	// Initialise the data in the height map (flat).
	for (int j = 0; j < m_heightmapHeight; j++)
	{
		for (int i = 0; i < m_heightmapWidth; i++)
		{
			index = (m_heightmapHeight * j) + i;

			m_heightMap[index].x = (float)i / detail;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j / detail;

			//and use this step to calculate the texture coordinates for this point on the terrain.
			m_heightMap[index].u = (float)i * this->uvTilesAcross;
			m_heightMap[index].v = (float)j * this->uvTilesAcross;
		}
	}

	//even though we are generating a flat terrain, we still need to normalise it.
	// Calculate the normals for the terrain data.
	result = calculateNormals();
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	initBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Terrain::generateHeightMapPerlin(ID3D11Device*)
{
	bool result;

	int index;
	float height = 0.0;

	//we want a wavelength of 1 to be a single wave over the whole terrain.
	//A single wave is 2 pi which is about 6.283
	m_frequency = (6.283 / m_heightmapHeight) / m_wavelength;

	//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
	//in this case I will run a sin-wave through the terrain in one axis.

	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetSeed(noiseSeed);
	noise.SetFrequency(noiseFrequency);
	for (int j = 0; j < m_heightmapHeight; j++)
	{
		for (int i = 0; i < m_heightmapWidth; i++)
		{
			index = (m_heightmapHeight * j) + i;
			//Multiplier to translate and index of the terrain map to the Perlin Noise Space
			// This is crucial to be different than one as any INTEGER would be a 0
			double pX = i * (double)noiseDomainScale;
			double pY = j * (double)noiseDomainScale;
			height = (float)noise.GetNoise(pX, pY);

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = height;
			m_heightMap[index].z = (float)j;

			//and use this step to calculate the texture coordinates for this point on the terrain.
			m_heightMap[index].u = (float)i * (this->uvTilesAcross / m_heightmapWidth);
			m_heightMap[index].v = (float)j * this->uvTilesAcross / m_heightmapWidth;
		}
	}

	result = calculateNormals();
	if (!result)
	{
		return false;
	}
	this->initBuffers(device);
}

bool Terrain::generateHeightMapExtra(ID3D11Device* device, float offsetX, float offsetY, float sampleScale)
{
	bool result;
	int index;
	float height;

	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetSeed(noiseSeed);
	this->sampleScale = sampleScale;
	this->offsetX = offsetX;
	this->offsetY = offsetY;
	if (destroyedStaging != nullptr)
	{
		deviceContext->CopyResource(destroyedStaging, this->destroyedTexture);
		HRESULT res = deviceContext->Map(destroyedStaging, 0, D3D11_MAP_READ, 0, &destroyedStaginMapped);
		int a = 3;
	}

	for (int j = 0; j < m_heightmapHeight; j++)
	{
		for (int i = 0; i < m_heightmapWidth; i++)
		{
			index = (m_heightmapHeight * j) + i;
			float fi = i, fj = j, w = (float)m_heightmapWidth, h = (float)m_heightmapHeight;

			//The coordinate represented in the range [-0.5,0.5]
			float locX = (i - (w / 2.0f)) / w;
			float locY = (j - (h / 2.0f)) / h;

			//The coordinate represented world range [-halfTerrainDim,+halfTerainDim]
			float worldX = locX * m_terrainWidth;
			float worldY = locY * m_terrainHeight;

			float vertexX = (worldX / sampleScale) + offsetX;
			float vertexY = (worldY / sampleScale) + offsetY;
			float	sampleX = (worldX / sampleScale) + offsetX;
			float	sampleY = (worldY / sampleScale) + offsetY;

			height = sampleHeightExtra(sampleX, sampleY);

			//Apply additional informaiton from destroyed terrain texture
			if (destroyedStaging != nullptr)
			{
				float sampled = 1 - sampleDestroyedWorldXY(vertexX, vertexY);
				height *= sampled;
			}

			//Assign the vertex position as local even though global is used to sample the space
			m_heightMap[index].x = vertexX;
			m_heightMap[index].y = height;
			m_heightMap[index].z = vertexY;

			//and use this step to calculate the texture coordinates for this point on the terrain.
			m_heightMap[index].u = (float)i * (this->uvTilesAcross / m_heightmapWidth);
			m_heightMap[index].v = (float)j * this->uvTilesAcross / m_heightmapWidth;
		}
	}

	if (destroyedStaging != nullptr)
	{
		deviceContext->Unmap(destroyedStaging, 0);
	}

	result = calculateNormals();
	if (!result)
	{
		return false;
	}
	this->initBuffers(device);
	return result;
}

float Terrain::sampleHeightExtra(float sampleX, float sampleY)
{
	float height = 0;
	float currentFrequency = extraNoise.frequency;
	float currentAmplitude = extraNoise.amplitude;

	// Compute the output from the fBM noise
	for (int k = 0; k < extraNoise.octaves; k++)
	{
		height += getHeight(noise, sampleX, sampleY, currentFrequency, currentAmplitude);
		currentFrequency *= extraNoise.lucanarity;
		currentAmplitude *= extraNoise.gain;
	}

	//Multipy with other noise layer
	height *= getHeight(noise, sampleX, sampleY, extraNoise.NoiseOne.frequency, extraNoise.NoiseOne.amplitude);
	return height;
}

float Terrain::getMaxHeightExtra()
{
	float height = 0;
	float currentFrequency = extraNoise.frequency;
	float currentAmplitude = extraNoise.amplitude;

	// Compute the output from the fBM noise
	for (int k = 0; k < extraNoise.octaves; k++)
	{
		//Max height would be 1 * amplitude
		height += 1 * currentAmplitude;
		currentFrequency *= extraNoise.lucanarity;
		currentAmplitude *= extraNoise.gain;
	}

	//Multipy with other noise layer
	height *= extraNoise.NoiseOne.amplitude; return height;
}

int Terrain::findFirstPeakAbove(float h_min)
{
	for (int i = 0; i < m_heightmapHeight; i++)
	{
		for (int j = 0; j < m_heightmapWidth; ++j)
		{
			if (getHeightAt(j, i) >= h_min)
			{
				return getHeightmapIndex(j, i);
			}
		}
	}
}

int Terrain::getHeightmapIndexFromIntersection(float vertexX, float vertexY)
{
	XMFLOAT2 ij = getLocalHeightmapCoords(vertexX, vertexY);
	int index = getHeightmapIndex(ij.x, ij.y);

	return index;
}

XMFLOAT2 Terrain::getLocalHeightmapCoords(float vertexX, float vertexY)
{
	float w = (float)m_heightmapWidth, h = (float)m_heightmapHeight;
	float terw = (float)m_terrainWidth, terh = (float)m_terrainHeight;
	int i = (((vertexX - this->offsetX) * sampleScale) / m_terrainWidth) * m_heightmapWidth + (w / 2.0f);
	int j = (((vertexY - this->offsetY) * sampleScale) / m_terrainWidth) * m_heightmapHeight + (h / 2.0f);
	XMFLOAT2 toReturn(i, j);
	return toReturn;
}

// Define 4-way (N, S, E, W) movement directions
// Easir to loop with
const int dx[4] = { -1, 1, 0, 0 };
const int dy[4] = { 0, 0, -1, 1 };

vector<int> Terrain::extractPeakBFS(int startIndex, float h_min)
{
	int rows = m_heightmapHeight;
	int cols = m_heightmapWidth;

	int startX = startIndex % m_heightmapHeight;
	int startY = startIndex / m_heightmapHeight;

	int index = (m_heightmapHeight * startY) + startX;

	// Check if the start point is valid
	float startHeight = getHeightAt(startX, startY);
	if (startHeight < h_min) return {};

	vector<int> peakPoints; // Stores extracted peak points
	vector<vector<bool>> visited(rows, vector<bool>(cols, false));
	std::queue<int> q;

	// Start BFS from (startX, startY)
	q.push(getHeightmapIndex(startX, startY));
	visited[startX][startY] = true;

	while (!q.empty()) {
		int currIndex = q.front();

		int x = currIndex % m_heightmapHeight;
		int y = currIndex / m_heightmapHeight;

		q.pop();
		peakPoints.push_back(currIndex);

		// Explore 4 neighboring cells
		for (int i = 0; i < 4; ++i) {
			int nx = x + dx[i];
			int ny = y + dy[i];

			// Boundary and height check
			if (nx >= 0 && ny >= 0 && nx < rows && ny < cols &&
				!visited[nx][ny] && getHeightAt(nx, ny) >= h_min) {
				visited[nx][ny] = true;
				q.push(getHeightmapIndex(nx, ny));
			}
		}
	}
	return peakPoints;
}

pair<int, int> Terrain::extractBoundingBoxPeak(const vector<int>& peakIndices)
{
	if (peakIndices.empty()) {
		return { 0,0 };
	}; // Return invalid values if input is empty

	// Initialize min and max values
	int minCol = 99999;
	int minRow = 99999;
	int maxCol = -99999;
	int maxRow = -99999;

	// Compute (col, row) and track min/max
	for (int index : peakIndices) {
		int row = index / m_heightmapHeight;
		int col = index % m_heightmapHeight;

		minCol = min(minCol, col);
		minRow = min(minRow, row);
		maxCol = max(maxCol, col);
		maxRow = max(maxRow, row);
	}

	int min = getHeightmapIndex(minCol, minRow);
	int max = getHeightmapIndex(maxCol, maxRow);

	return { min,max };
}

Terrain* Terrain::extractPeakTerrainSubregion(XMVECTOR intersection, float heightMin, int targetDimenesionSize) {
	//float heightMin = m_amplitude - m_amplitude / 2.0f;
	//int indexPeak = findFirstPeakAbove(heightMin);
	float x = intersection.m128_f32[0];
	float z = intersection.m128_f32[2];
	int index = getHeightmapIndexFromIntersection(x, z);

	vector<int> indices = extractPeakBFS(index, heightMin);
	pair<int, int> minMax = extractBoundingBoxPeak(indices);
	if (minMax.first == minMax.second)
		return nullptr;
	int minX = minMax.first % m_heightmapHeight;
	int minY = minMax.first / m_heightmapHeight;
	int maxX = minMax.second % m_heightmapHeight;
	int maxY = minMax.second / m_heightmapHeight;

	int diffX = maxX - minX;
	int diffY = maxY - minY;

	VertexType worldMin = this->getHeightMapVertex(minMax.first);
	VertexType worldMax = this->getHeightMapVertex(minMax.second);

	VertexType vMin = this->getHeightMapVertex(minMax.first);
	vMin.position.x = std::fmin(worldMin.position.x, worldMax.position.x);
	vMin.position.y = std::fmin(worldMin.position.y, worldMax.position.y);
	vMin.position.z = std::fmin(worldMin.position.z, worldMax.position.z);

	VertexType vMax;
	vMax.position.x = std::fmax(worldMin.position.x, worldMax.position.x);
	vMax.position.y = std::fmax(worldMin.position.y, worldMax.position.y);
	vMax.position.z = std::fmax(worldMin.position.z, worldMax.position.z);

	float OffsetX = (vMin.position.x + vMax.position.x) / 2;
	float OffsetZ = (vMin.position.z + vMax.position.z) / 2;

	int diffWorldX = vMax.position.x - vMin.position.x;
	int diffWorldY = vMax.position.y - vMin.position.y;
	int diffWorldZ = vMax.position.z - vMin.position.z;

	Terrain* peakTerrain;

	//Ensure that the new space where the noise would be sample is the same
	//Extracted region aim
	// Get the max dimesion
	int currentDimesionSize = max(diffWorldX, diffWorldZ);
	int d = targetDimenesionSize / currentDimesionSize;

	extractRegion(peakTerrain, diffWorldX, diffWorldZ, OffsetX, OffsetZ, (float)d);
	removeRegion(minX, minY, maxX, maxY);
	return peakTerrain;
}

void Terrain::extractRegion(Terrain*& peakTerrain, int& diffX, int& diffY, float OffsetX, float OffsetY, float detail)
{
	peakTerrain = new Terrain(this->device, this->deviceContext);
	peakTerrain->Initialize(device, max(diffX, diffY), max(diffX, diffY), 25);

	peakTerrain->m_frequency = this->m_frequency;
	peakTerrain->m_amplitude = this->m_amplitude;
	peakTerrain->fBM = this->fBM;
	peakTerrain->extraNoise = this->extraNoise;
	peakTerrain->noiseFrequency = this->noiseFrequency;
	peakTerrain->noiseSeed = this->noiseSeed;

	peakTerrain->generateHeightMapExtra(device, OffsetX, OffsetY, 1);
}

void Terrain::removeRegion(int minX, int minY, int maxX, int maxY)
{
	for (int i = minX; i < maxX; i++)
	{
		for (int j = minY; j < maxY; j++)
		{
			int index = getHeightmapIndex(i, j);
			m_heightMap[index].y = 0;
		}
	}
	initBuffers(device);
}

bool Terrain::calculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	DirectX::SimpleMath::Vector3* normals;

	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new DirectX::SimpleMath::Vector3[(m_heightmapHeight - 1) * (m_heightmapWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (m_heightmapHeight - 1); j++)
	{
		for (i = 0; i < (m_heightmapWidth - 1); i++)
		{
			index1 = (j * m_heightmapHeight) + i;
			index2 = (j * m_heightmapHeight) + (i + 1);
			index3 = ((j + 1) * m_heightmapHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_heightmapHeight - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j < m_heightmapHeight; j++)
	{
		for (i = 0; i < m_heightmapWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_heightmapHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (m_heightmapWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_heightmapHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_heightmapHeight - 1)))
			{
				index = (j * (m_heightmapHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (m_heightmapWidth - 1)) && (j < (m_heightmapHeight - 1)))
			{
				index = (j * (m_heightmapHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_heightmapHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

void Terrain::shutdown()
{
	return;
}


bool Terrain::generateHeightMap(ID3D11Device* device)
{
	bool result;

	int index;
	float height = 0.0;

	//we want a wavelength of 1 to be a single wave over the whole terrain.
	//A single wave is 2 pi which is about 6.283
	m_frequency = (6.283 / m_heightmapHeight) / m_wavelength;

	//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
	//in this case I will run a sin-wave through the terrain in one axis.

	for (int j = 0; j < m_heightmapHeight; j++)
	{
		for (int i = 0; i < m_heightmapWidth; i++)
		{
			int isRandomChance = rand() % 2;

			int randRange = ceil(m_amplitude);
			int randLowerBound = floor(-m_amplitude / 2.0f) + 5;
			int randHeigh = (rand() % randRange) - randLowerBound;

			index = (m_heightmapHeight * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)randHeigh;

			m_heightMap[index].z = (float)j;
		}
	}

	result = calculateNormals();
	if (!result)
	{
		return false;
	}
	this->initBuffers(device);
}

bool Terrain::generateHeightMapfBM(ID3D11Device*)
{
	bool result;

	int index;
	float height = 0.0;

	//we want a wavelength of 1 to be a single wave over the whole terrain.
	//A single wave is 2 pi which is about 6.283
	m_frequency = (6.283 / m_heightmapHeight) / m_wavelength;

	//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
	//in this case I will run a sin-wave through the terrain in one axis.

	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetSeed(noiseSeed);
	noise.SetFrequency(noiseFrequency);
	for (int j = 0; j < m_heightmapHeight; j++)
	{
		for (int i = 0; i < m_heightmapWidth; i++)
		{
			index = (m_heightmapHeight * j) + i;
			double pX = i * (double)noiseDomainScale;
			double pY = j * (double)noiseDomainScale;
			height = 0;

			float amplitude = m_amplitude;
			//Start getHeightAt the noise frequency
			float frequency = noiseFrequency;
			for (int k = 0; k < fBM.octaves; k++) {
				//Set the noise frequency
				noise.SetFrequency(frequency);
				height += amplitude * noise.GetNoise(pX, pY);
				//height += amplitude * noise.GetNoise( frequency * pX, frequency * pY);

				//Increment frequency and decrement amplitude
				frequency *= fBM.lucanarity;
				amplitude *= fBM.gain;
			}

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = height;
			m_heightMap[index].z = (float)j;

			//and use this step to calculate the texture coordinates for this point on the terrain.
			m_heightMap[index].u = (float)i * (this->uvTilesAcross / m_heightmapWidth);
			m_heightMap[index].v = (float)j * this->uvTilesAcross / m_heightmapWidth;
		}
	}

	result = calculateNormals();
	if (!result)
	{
		return false;
	}
	this->initBuffers(device);
}

bool Terrain::smoothHeightMap(ID3D11Device* device)
{
	bool result;

	//Smooth the random height map
	for (int j = 0; j < m_heightmapHeight; j++)
	{
		for (int i = 0; i < m_heightmapWidth; i++)
		{
			float sum = 0.0f;
			int count = 0;

			// Iterate through neighboring cells
			for (int dj = -1; dj <= 1; dj++)
			{
				for (int di = -1; di <= 1; di++)
				{
					int neighborX = i + di;
					int neighborY = j + dj;

					// Check bounds
					if (neighborX >= 0 && neighborX < m_heightmapWidth &&
						neighborY >= 0 && neighborY < m_heightmapHeight)
					{
						int neighborIndex = (m_heightmapWidth * neighborY) + neighborX;
						sum += m_heightMap[neighborIndex].y;
						count++;
					}
				}
			}

			// Compute the average height for this vertex
			int currentIndex = (m_heightmapWidth * j) + i;
			m_smoothedHeightMap[currentIndex].y = sum / float(count);
		}
	}

	m_heightMap = m_smoothedHeightMap;

	result = calculateNormals();
	if (!result)
	{
		return false;
	}

	initBuffers(device);
	if (!result)
	{
		return false;
	}
}

bool Terrain::update()
{
	return true;
}

float* Terrain::getWavelength()
{
	return &m_wavelength;
}

float* Terrain::getAmplitude()
{
	return &m_amplitude;
}
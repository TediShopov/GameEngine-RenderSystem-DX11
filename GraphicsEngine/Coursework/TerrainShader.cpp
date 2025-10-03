#include "TerrainShader.h"
#include "Terrain.h"

void TerrainShader::SetTerrainData(ID3D11DeviceContext* device, Terrain* terrain)
{
	//Find the max amplitude of fBM

	float amp = *terrain->getAmplitude();
	float maxHeight = amp;
	for (int k = 0; k < terrain->fBM.octaves; k++) {
		maxHeight += amp;
		amp *= terrain->fBM.gain;
	}

	TerrainParameters.amplitude = maxHeight;
	terrainParamsBuffers.SetTo(device, &TerrainParameters);
}

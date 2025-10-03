#pragma once
#include "Terrain.h"
#include "TessellationQuad.h"
#include "ProceduralDestruction.h"
#include  <btBulletDynamicsCommon.h>
#include  "TerrainTesselationShader.h"
#include <map>

struct RayHitResult {
	bool success;
	long long time_taken_us;

};


// Extracts a peak patch from a heightfield, cuts with planes, instantiates destructible chunks + Bullet rigid bodies
class DestructibleTerrainPeaks
{
private:
public:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;


	//Configuration of the peak extraction and visual fidelity properties
	float simpleTerrainDetail = 300;
	float tessTerrainDetail = 75;
	float terrainSizeXZUnits = 300;
	float waterPlaneY = 12;
	float bottomPlaneY = 6.5;
	int peakTargetDim = 100;
	int destructibleChunkTargetDim = 50;

	//Benchmarks for ray-casting methods
	int rayTMin=0;
	int rayTMax=250;
	int raySampleCount=1000;
	int rayBenchmarkRepeats = 10;
	std::vector<RayHitResult> raycastResults;

	MeshInstance* terrainInstance;
	MeshInstance* tessellatedTerrainQuadInstance;

	//The extracted terrain peak via BFS
	Terrain* terrainPeak = nullptr;

	//The actual mesh that is going to be multi-plane split
	BaseMesh* destructablePeakMesh = nullptr;

	TerrainTesselationShader* terrainTesselationShader;

	SerializableMesh simpleTerrainSM;
	SerializableMesh tessTerrainSM;

	ProceduralDestruction proceduralDestruction;

	DestructibleTerrainPeaks();

	void initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, TerrainTesselationShader* terrainTesselationShader);

	void initMeshes();
	void initMeshInstances();
	void applyTerrainDetail();
	void regenerateSimpleTerrain();

	Terrain* getSimpleTerrain();

	//Inputs are in world-space. Return a map MeshInstance -> RigidBody
	std::map<MeshInstance*, btRigidBody*> fireProjectileAt(XMVECTOR origin, XMVECTOR direction);


protected:
	std::vector<XMVECTOR> rayHisPointsLocal;
	bool hasRayHit;

	std::map<MeshInstance*, btRigidBody*> spawnDestructibleChunks(XMVECTOR direction);

	void computeClosestRayIntersectionOnMesh(XMVECTOR orig, XMVECTOR dir);
	void computeClosestRayIntersectoinOnNoise(XMVECTOR orig, XMVECTOR dir); 

	//CSV Schema 
	void exportToCSV(const std::vector<RayHitResult>& raycastResults, const std::string& filename);

};


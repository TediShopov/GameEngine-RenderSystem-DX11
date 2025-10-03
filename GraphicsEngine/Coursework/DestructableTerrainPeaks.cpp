#include "DestructableTerrainPeaks.h"
#include <chrono>

 DestructibleTerrainPeaks::DestructibleTerrainPeaks()
{

}

 void DestructibleTerrainPeaks::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, TerrainTesselationShader* terrainTesselationShader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->terrainTesselationShader = terrainTesselationShader;

}

 Terrain* DestructibleTerrainPeaks::getSimpleTerrain()
{
	return (Terrain*)simpleTerrainSM.GetMesh();

}

 void DestructibleTerrainPeaks::initMeshes()
{
	// -- CHANGE SIMPLE MESH --
	simpleTerrainSM = SerializableMesh::ShapeMesh("Terrain", ProceduralTerrain, simpleTerrainDetail, terrainSizeXZUnits);
	simpleTerrainSM.CreateMesh(device, deviceContext);
	getSimpleTerrain()->Initialize(device, terrainSizeXZUnits, terrainSizeXZUnits, simpleTerrainDetail);
	getSimpleTerrain()->generateHeightMapExtra(device, 0, 0, 1);
	tessTerrainSM = SerializableMesh::ShapeMesh("TessTerrain", TesselationQuad, tessTerrainDetail, terrainSizeXZUnits);
	tessTerrainSM.CreateMesh(device, deviceContext);
	if (terrainInstance != nullptr)
	{
		terrainInstance->setMesh(simpleTerrainSM);
		regenerateSimpleTerrain();
	}
	if (tessellatedTerrainQuadInstance != nullptr)
	{
		tessellatedTerrainQuadInstance->setMesh(tessTerrainSM);
	}



}

 void DestructibleTerrainPeaks::regenerateSimpleTerrain()
{
	auto terPtr = getSimpleTerrain();

	terPtr->noiseSeed = 1337;
	terPtr->destroyedStaging = terrainTesselationShader->destroyedStaging.Get();
	terPtr->destroyedTexture = terrainTesselationShader->destroyedTerrainMaskTexture.Get();

	terPtr->extraNoise = terrainTesselationShader->fBMParams;
	terPtr->generateHeightMapExtra(device, 0, 0, 1);
}

 void DestructibleTerrainPeaks::initMeshInstances()
{
	auto terrainInstance = new MeshInstance();
	terrainInstance->transform.setPosition(0, -10, 0);
	//$terrainInstance->transform.setScale(5, 5, 5);
	this->terrainInstance = terrainInstance;
	this->terrainInstance->setMesh(simpleTerrainSM);


	tessellatedTerrainQuadInstance = new MeshInstance();
	tessellatedTerrainQuadInstance->transform.setPosition(0, -10, 0);
	tessellatedTerrainQuadInstance->setMesh(tessTerrainSM);
}

 void DestructibleTerrainPeaks::applyTerrainDetail()
{
	initMeshes();
}

 std::map<MeshInstance*, btRigidBody*> DestructibleTerrainPeaks::fireProjectileAt(XMVECTOR origin, XMVECTOR direction)
{
	std::map<MeshInstance*, btRigidBody*> renderItemToPhysicsItemMap;
	computeClosestRayIntersectionOnMesh(origin, direction);

	if (hasRayHit)
	{
		//terrainPeak = terPtr->extractPeakTerrainSubregion(this->rayIntersections[0], this->waterPlaneHeight);
		terrainPeak = static_cast<Terrain*>(terrainInstance->getMesh())->extractPeakTerrainSubregion(
			this->rayHisPointsLocal[0],
			this->bottomPlaneY,
			this->peakTargetDim);

		if (terrainPeak != nullptr)
		{
			//Plane representing the water plane
			XMVECTOR point = XMVectorSet(0, waterPlaneY, 0, 1);
			XMVECTOR up = XMVectorSet(0, 1, 0, 0);
			XMVECTOR waterPlane = XMPlaneFromPointNormal(point, up);

			destructablePeakMesh = proceduralDestruction.cutMeshClosed(waterPlane, terrainPeak)[0];

			renderItemToPhysicsItemMap = spawnDestructibleChunks(direction);

		}

	}
	return renderItemToPhysicsItemMap;
}

 std::map<MeshInstance*, btRigidBody*> DestructibleTerrainPeaks::spawnDestructibleChunks(XMVECTOR direction)
{
	//XMVECTOR planeVector = XMLoadFloat4(&plane);
	std::map<MeshInstance*, btRigidBody*> renderItemToPhysicsItemMap;
	if (destructablePeakMesh != nullptr)
	{
		XMVECTOR centerOfPeak;
		proceduralDestruction.computeVertexCentroid(this->destructablePeakMesh->vertices, centerOfPeak);

		//Mark the terrain region that has been destructed
		//Terrain* ter = this->terrainPeak;
		int worldX = XMVectorGetX(centerOfPeak);
		int worldZ = XMVectorGetZ(centerOfPeak);
		int rad = terrainPeak->getMaxDimension() / (this->terrainPeak->sampleScale * 2);
		//int rad = 50;
		terrainTesselationShader->markRegionDestructed(deviceContext, worldX, worldZ, rad);

		auto destructedInstances = proceduralDestruction.radialPlaneCutsRandom(centerOfPeak, this->destructablePeakMesh);

		for each (MeshInstance * dins in destructedInstances)
		{
			XMVECTOR dinsOrigin = dins->transform.getGlobalPosition();

			dins->transform.translate(XMVectorGetX(terrainInstance->transform.getPosition()),
				XMVectorGetY(terrainInstance->transform.getPosition()), XMVectorGetZ(terrainInstance->transform.getPosition()));

			//Initialize btRigidBody
			//For now a simple sphere with a constant radius
			btRigidBody* body;

			// Position the cube above the ground
			btTransform startTransform;
			startTransform.setIdentity();

			XMVECTOR newWorldPos = dins->transform.getPosition();

			startTransform.setOrigin(btVector3(XMVectorGetX(newWorldPos), XMVectorGetY(newWorldPos), XMVectorGetZ(newWorldPos)));

			// Create cube motion state and rigid body
			auto boxMotionState = new btDefaultMotionState(startTransform);
			btScalar mass = 250;
			btVector3 inertia(0, 0, 0);

			float radius = 1;
			btCollisionShape* sphereSimple = new btSphereShape(radius);

			sphereSimple->calculateLocalInertia(mass, inertia);
			btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, sphereSimple, inertia);

			body = new btRigidBody(boxRigidBodyCI);
			body->setActivationState(DISABLE_DEACTIVATION);
			body->activate();

			//Add central impulse away from the origin

			XMVECTOR awayFromCenter = XMVectorSubtract(dinsOrigin, centerOfPeak);
			//awayFromCenter *= 1000;
			awayFromCenter *= 1000;
			XMVECTOR awayFromRay = direction;
			//awayFromRay *= 2000;
			awayFromRay *= 200;

			XMVECTOR finalForce = XMVectorAdd(awayFromCenter, awayFromRay);
			body->applyCentralImpulse(btVector3(XMVectorGetX(finalForce), XMVectorGetY(finalForce), XMVectorGetZ(finalForce)));

			//Apply random torque
			int torqueForce = 500;
			int xTorq = rand() % torqueForce;
			int yTorq = rand() % torqueForce;
			int zTorq = rand() % torqueForce;

			body->applyTorqueImpulse(btVector3(xTorq, yTorq, zTorq));

			renderItemToPhysicsItemMap.insert({ dins,body });

		}
	}
	return renderItemToPhysicsItemMap;
}

 void DestructibleTerrainPeaks::computeClosestRayIntersectionOnMesh(XMVECTOR orig, XMVECTOR dir)
{
	XMMATRIX inverseWorld = this->terrainInstance->transform.getInverseMatrix();
	orig = XMVector3TransformCoord(orig, inverseWorld);
	dir = XMVector3TransformNormal(dir, inverseWorld);

	XMVECTOR i;

	this->rayHisPointsLocal.clear();
	this->hasRayHit = this->terrainInstance->getMesh()->rayMeshIntersect(orig, dir, &i);




	if (this->hasRayHit)
		this->rayHisPointsLocal.push_back(i);
}

void DestructibleTerrainPeaks::computeClosestRayIntersectoinOnNoise(XMVECTOR orig, XMVECTOR dir)
{
	auto start = std::chrono::high_resolution_clock::now();
	this->rayHisPointsLocal.clear();
	dir = XMVector3Normalize(dir);
	XMMATRIX inverseWorld = this->terrainInstance->transform.getInverseMatrix();
	//Locallise the vector

	orig = XMVector3TransformCoord(orig, inverseWorld);
	dir = XMVector3TransformNormal(dir, inverseWorld);


	XMVECTOR localPosition = orig;
	bool isAboveTerrain = true;

	float tIncrement = (float)(rayTMax - rayTMin) / (float)raySampleCount;

	this->hasRayHit = false;
	Terrain* ter = getSimpleTerrain();
	for (int i = 0; i < raySampleCount; i++)
	{
		localPosition +=dir*tIncrement;

		//Remember that XZ is used for sampling 
		float terrainY = ter->sampleHeightExtra(XMVectorGetX(localPosition), XMVectorGetZ(localPosition));

		//Test againt the ray global cooridnate
		if (terrainY > XMVectorGetY(localPosition))
		{
			this->hasRayHit = true;
			this->rayHisPointsLocal.push_back(localPosition);

			break;
		}


	}
	auto end = std::chrono::high_resolution_clock::now();


	RayHitResult result;
	result.success = this->hasRayHit;
	result.time_taken_us = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
	this->raycastResults.push_back(result);
	


	
}

 void DestructibleTerrainPeaks::exportToCSV(const std::vector<RayHitResult>& raycastResults, const std::string& filename) {
	std::ofstream csv_file(filename);
	if (!csv_file.is_open()) {
		std::cerr << "Failed to open CSV file!" << std::endl;
		return;
	}

	// Write CSV header
	csv_file << "Success,Time (us)\n";

	// Write each result
	for (const auto& result : raycastResults) {
		csv_file
			<< (result.success ? "Yes" : "No") << ","
			<< result.time_taken_us << "\n";
	}

	csv_file.close();
	std::cout << "Results saved to " << filename << std::endl;
}

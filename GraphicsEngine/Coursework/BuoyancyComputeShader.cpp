
#include "MeshInstance.h"
#include "BuoyancyComputeShader.h"

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btDefaultMotionState.h>

void BuoyancyComputeShader::initInputOutputBuffers(ID3D11Device* device)
{
	WaveParemetersResource.Create(device);
	WaveParemetersResource.setToPosition = 0;
	WaveParemetersResource.setToStage = ShaderStage::COMPUTE;
	BuoyancyParemetersResource.Create(device);
	BuoyancyParemetersResource.setToPosition = 1;
	BuoyancyParemetersResource.setToStage = ShaderStage::COMPUTE;
	GradientDescentParamsResource.Create(device);
	GradientDescentParamsResource.setToPosition = 2;
	GradientDescentParamsResource.setToStage = ShaderStage::COMPUTE;


	HRESULT hr;
	//Create input buffer
	std::vector<XMFLOAT3> inputData(getSamples(), XMFLOAT3()); // Example input
	D3D11_BUFFER_DESC inputBufferDesc = {};
	inputBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	inputBufferDesc.ByteWidth = sizeof(XMFLOAT3) * inputData.size();
	inputBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	inputBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	inputBufferDesc.StructureByteStride = sizeof(XMFLOAT3);
	inputBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;


	//	D3D11_SUBRESOURCE_DATA initData = {};
	//	initData.pSysMem = inputData.data();

	hr = device->CreateBuffer(&inputBufferDesc, NULL, &inputBuffer);


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = inputData.size();

	hr = device->CreateShaderResourceView(inputBuffer, &srvDesc, &inputResourcesView);


	//Create output buffer
	D3D11_BUFFER_DESC outputDesc;
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.ByteWidth = sizeof(float) * inputData.size();
	outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.CPUAccessFlags = 0;
	outputDesc.StructureByteStride = sizeof(float);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	hr = (device->CreateBuffer(&outputDesc, NULL, &outputBuffer));



	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = inputData.size();
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	hr = device->CreateUnorderedAccessView(outputBuffer, &uavDesc, &outputUnorderedAcessView);


	outputDesc.Usage = D3D11_USAGE_STAGING;
	outputDesc.BindFlags = 0;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = (device->CreateBuffer(&outputDesc, 0, &outputResultBuffer));
}

int BuoyancyComputeShader::getSamples()
{
	return (stepsAlongHullX + 1) * (stepsAlongHullZ + 1);
}

void BuoyancyComputeShader::setBuoyantBody(MeshInstance* mesh_instance)
{
	this->buoyantBodyMeshInstance = mesh_instance;
}

btRigidBody* BuoyancyComputeShader::constructBuoyantBodyHull()
{
	//Buoyant body mesh instacne is required
	if (buoyantBodyMeshInstance == nullptr || buoyantBodyMeshInstance->getMesh() == nullptr) return nullptr;

	buoyantBodyMeshInstance->getMesh()->computeBoundingBox();
	//Bounding box in model space
	XMFLOAT3 bbMax = buoyantBodyMeshInstance->getMesh()->boundingBoxMax;
	XMFLOAT3 bbMin = buoyantBodyMeshInstance->getMesh()->boundingBoxMin;

	XMVECTOR modelSpaceMax = XMVectorSet(bbMax.x, bbMax.y, bbMax.z, 1.0f);
	XMVECTOR modelSpaceMin = XMVectorSet(bbMin.x, bbMin.y, bbMin.z, 1.0f);

	XMMATRIX world = XMMatrixTranspose(buoyantBodyMeshInstance->transform.getTransformMatrix());
	XMVECTOR worldSpaceMax = XMVector4Transform(modelSpaceMax, world);
	XMVECTOR worldSpaceMin = XMVector4Transform(modelSpaceMin, world);

	 buoyantBodyWorldDimensions = XMVectorSubtract(worldSpaceMax, worldSpaceMin);
	// XMVECTOR extents = XMVectorSubtract(worldSpaceMax, worldSpaceMin);
	// buoyantBodyWorldDimensions = extents * 0.5;

	btCollisionShape* boxShape = new btBoxShape(btVector3(
		buoyantBodyWorldDimensions.m128_f32[0], 
		buoyantBodyWorldDimensions.m128_f32[1], 
		buoyantBodyWorldDimensions.m128_f32[2]));
	
	// Position the cube above the ground
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(0, 30, 15));

	// Create cube motion state and rigid body
	auto boxMotionState = new btDefaultMotionState(startTransform);
	btScalar mass = 300;
	btVector3 inertia(0, 0, 0);

	boxShape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, boxShape, inertia);

	buoyantBodyHull = new btRigidBody(boxRigidBodyCI);
	buoyantBodyHull->setActivationState(DISABLE_DEACTIVATION);
	buoyantBodyHull->activate();


	setDefaultValuesForBody(buoyantBodyWorldDimensions, getSamples());


	return buoyantBodyHull;

	

}

BuoyancyComputeShader::BuoyancyComputeShader(ID3D11Device* device, HWND hwnd,int precX, int presZ): BaseShader(device, hwnd)
{
	this->stepsAlongHullX = precX;
	this->stepsAlongHullZ = presZ;
	
	loadComputeShader(L"BuoyancyComputeShader.cso");
	//Init Buffers
	initInputOutputBuffers(device);

	//Set buoyancy shader parameters that do not depend on the buoyant body
	buoyancyParameters.gravity = 9.8;
	buoyancyParameters.fluidDensity = 0.20;

	gradientDescentParameters.eps = 0.15;
	gradientDescentParameters.learningRate = 0.5;
	gradientDescentParameters.iterations = 60;
	gradientDescentParameters.offsetAlongAxis = 0.3f;


}
float lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
}

std::vector<XMFLOAT3> BuoyancyComputeShader::generateRelativePointsAlongHull(
	float startX, float endX, int stepsX,
	float startY, float endY, int stepsY,
	float startZ, float endZ, int stepsZ
	)
{
	 std::vector<XMFLOAT3> points;
	 



    for (int i = 0; i <= stepsX; ++i) {
        float x = lerp(startX,endX,(float)i/(float)stepsX); // X-coordinate

        for (int j = 0; j < stepsY; ++j) {
			
			float y = lerp(startY, endY, (float)j/(float)stepsY); // Y-coordinate

			for (int k = 0; k <= stepsZ; ++k) {
				float z = lerp(startZ, endZ, (float)k/(float)stepsZ); // Y-coordinate
				points.push_back(XMFLOAT3(x, y, z));
			}
        }

    }

    return points;
}

void BuoyancyComputeShader::computeAndApplyBuoyantForce(btRigidBody* rigid_body,MeshInstance* mesh_instance, ID3D11DeviceContext* deviceContext, int x,int y
	, int z, MultipleWaveBuffer parameters)
{
	mesh_instance->getMesh()->computeBoundingBox();
	XMFLOAT3 min =  mesh_instance->getMesh()->boundingBoxMin;
	XMFLOAT3 max = mesh_instance->getMesh()->boundingBoxMax;

	mesh_instance->getMesh()->vertices;
	positionAlongHull =
		generateRelativePointsAlongHull(
			min.x / 2.0f, max.x / 2.0f, stepsAlongHullX,
			min.y, max.y, 1,
			min.z / 2.0f, max.z / 2.0f , stepsAlongHullZ);

	//Precalculate wave parameters for effeciency
	for (WaveParameters& wp : parameters.waves)
	 {
	     wp.k = 2.0f * 3.14159265358979323846f / wp.wavelength;
	     wp.c = sqrt(9.8 / wp.k) * wp.speed;
	 }

	 WaveParemetersResource.SetTo(deviceContext, &parameters);
	 BuoyancyParemetersResource.SetTo(deviceContext, &buoyancyParameters);
	 GradientDescentParamsResource.SetTo(deviceContext, &gradientDescentParameters);
	

	//T* shaderDataPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMFLOAT3* shaderDataPtr;
	deviceContext->Map(inputBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	shaderDataPtr = (XMFLOAT3*)mappedResource.pData;
	memcpy(shaderDataPtr, positionAlongHull.data(), positionAlongHull.size() * sizeof(XMFLOAT3));
	deviceContext->Unmap(inputBuffer, 0);


	//Set wave pareemts resoure as well

	//Set resources
	deviceContext->CSSetShaderResources(0, 1, &inputResourcesView);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &outputUnorderedAcessView,0);



	//Set shader and call dispatch
	compute(deviceContext, x, y, z);

	deviceContext->CopyResource(outputResultBuffer, outputBuffer);

	//Read the data from the buffer
	D3D11_MAPPED_SUBRESOURCE mapped_result_resource;
	HRESULT hr = deviceContext->Map(outputResultBuffer, 0, D3D11_MAP_READ, 0, &mapped_result_resource);
	buoyancyForces.clear();
	if(SUCCEEDED(hr))
	{

		float* buoyancyForcePerVertex = reinterpret_cast<float*>(mapped_result_resource.pData);
		for (size_t i = 0; i < getSamples(); i++)
		{
			buoyancyForces.push_back(buoyancyForcePerVertex[i]);
		}
		
	}
	

	//rigid_body->setGravity(btVector3(0, 0, 0));
	//Read and apply buoyant force getHeightAt point along the mesh
	for (int i = 0; i < buoyancyForces.size(); ++i)
	{

		float forceToApply = buoyancyForces[i];
			XMFLOAT3 relPoint = positionAlongHull[i];
			relPoint.y = 0;
			relPoint.x *= mesh_instance->transform.getScale().m128_f32[0];
			relPoint.z *= mesh_instance->transform.getScale().m128_f32[2];
			rigid_body->applyForce(btVector3(0, forceToApply, 0), btVector3(-relPoint.x, relPoint.y, -relPoint.z));
	}
	//Debug apply force to the lower right corner of the body
	
	btVector3 worldDim = 
		btVector3(buoyantBodyWorldDimensions.m128_f32[0],buoyantBodyWorldDimensions.m128_f32[1],buoyantBodyWorldDimensions.m128_f32[2]);
	//clean the compute shader
	deviceContext->CSSetShader(nullptr, nullptr, 0);
		
}

void BuoyancyComputeShader::setDefaultValuesForBody(XMVECTOR worldDimensions, float samples)
{
	buoyancyParameters.columnSurface = computeDefaultMaxSurfacePerColumn(worldDimensions, samples);
	buoyancyParameters.maxColumnVolume = computeDefaultMaxVolume(buoyancyParameters.columnSurface);
}

float BuoyancyComputeShader::computeDefaultMaxSurface(XMVECTOR worldDimensions)
{
	float xDim = worldDimensions.m128_f32[0];
	float zDim = worldDimensions.m128_f32[2];
	float maxSurface = (float)xDim * (float)zDim;
	//Obtain max surface per sample
	return maxSurface;
}

float BuoyancyComputeShader::computeDefaultMaxSurfacePerColumn(XMVECTOR worldDimensions, float samplesAlongHull)
{
	return computeDefaultMaxSurface(worldDimensions) / (samplesAlongHull);
}


float BuoyancyComputeShader::computeDefaultMaxVolume(float estimatedSurface)
{
	return 	estimatedSurface * abs(buoyantBodyWorldDimensions.m128_f32[1]);
}


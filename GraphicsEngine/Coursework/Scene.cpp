#include "Scene.h"
#include  <btBulletDynamicsCommon.h>

#include "DirectXMath.h"
#include "SceneJsonSerializer.h"
#include "SkyMapShader.h"
#include "TerrainTesselationShader.h"
#include "ProceduralMeshA.h"
#include <sstream>


Scene::Scene()
{
	//Default parameters debug ray initialization
	shootingRay.Origin.x = -2.5;
	shootingRay.Origin.y = 25;
	shootingRay.Origin.z = 6.5;

	shootingRay.Direction.x = 0;
	shootingRay.Direction.y = -1;
	shootingRay.Direction.z = 1;
	shootingRay.tMin = 0;
	shootingRay.tMax = 10;

	shootingRayShipRelative.Origin.x = 0.5;
	shootingRayShipRelative.Origin.y = 1.8;
	shootingRayShipRelative.Direction.x = 0.5;
	shootingRayShipRelative.tMin = 0;
	shootingRayShipRelative.tMax = 100;


}

void Scene::constructHullRigidBody()
{
	buoyancyComputeShader->setBuoyantBody(shipMeshInstance.get());
	shipHullRigidBody = buoyancyComputeShader->constructBuoyantBodyHull();
	if (shipHullRigidBody != nullptr)
		discreteDynamicsWorld->addRigidBody(shipHullRigidBody);
}


void Scene::initPhysics()
{
	// Initialize Bullet physics
	auto collisionConfiguration = new btDefaultCollisionConfiguration();
	auto dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlapingPairCache = new btDbvtBroadphase();
	auto solver = new btSequentialImpulseConstraintSolver;

	discreteDynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlapingPairCache, solver,
		collisionConfiguration);

	// Set gravity
	discreteDynamicsWorld->setGravity(btVector3(0, -9.8f, 0));

	// Create the plane collision shape (static)
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

	// Create ground motion state and rigid body
	auto groundMotionState = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape);
	auto groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->translate(btVector3(0, -60, 0));
	discreteDynamicsWorld->addRigidBody(groundRigidBody);

	// Clean up
}




void Scene::initSceneComposition(int screenWidth, int screenHeight)
{
	//auto debugTBN = new MeshInstance(assetSystem->getMesh("TangentMesh"));
	auto debugTBN = std::make_unique<MeshInstance>(assetSystem->getMesh("TangentMesh"));
	debugTBN->setMaterial(assetSystem->getMaterial("Green"));
	debugTBN->transform.setScale(1, 1, 1);

	renderSystem->DestructableComponentsCollections->addRenderItem(debugTBN.get(), 500000);
	meshInstances.push_back(std::move(debugTBN));

	//auto debugBuoyancy = new MeshInstance(assetSystem->getMesh("SphereDebug"));
	auto debugBuoyancy = std::make_unique<MeshInstance>(assetSystem->getMesh("SphereDebug"));
	debugBuoyancy->setMaterial(assetSystem->getMaterial("Green"));
	debugBuoyancy->transform.setScale(1, 1, 1);
	debugSphere = std::move(debugBuoyancy);
	//debugSphere = std::make_unique<MeshInstance>(debugBuoyancy);

	//The hull of the ship is the primary object updated by the simulation
	//auto shipHull = new MeshInstance(assetSystem->getMesh("Cube"));
	auto shipHull = std::make_unique<MeshInstance>(assetSystem->getMesh("Cube"));
	shipHull->setMaterial(assetSystem->getMaterial("ShipMaterial"));
	shipHull->transform.setPosition(-5, 20, 0);
	shipHull->transform.setScale(15,2,15);

	shipHull->transform.setRotation(0, 0, -90);

	//The ship mesh itself it a child on top of the hull mesh
	//auto shipMeshInstance = new MeshInstance(assetSystem->getMesh("Ship"));
	auto shipMeshInstance = std::make_unique<MeshInstance>(assetSystem->getMesh("Ship"));
	shipMeshInstance->setMaterial(assetSystem->getMaterial("ShipMaterial"));
	shipMeshInstance->transform.setPosition(2.5 / shipHull->transform.getScale().m128_f32[0], -2, 0);

	shipMeshInstance->transform.setRotation(0, 0, 90);
	float shipScalarScale = 0.15f;
	shipMeshInstance->transform.setScale(
		shipScalarScale * (1.0f / (shipHull->transform.getScale().m128_f32[0])),
		shipScalarScale * (1.0f / (shipHull->transform.getScale().m128_f32[1])),
		shipScalarScale * (1.0f / (shipHull->transform.getScale().m128_f32[2]))
	);
	shipMeshInstance->transform.setParent(&shipHull->transform);
	renderSystem->autoInsertInRenderCollection(shipMeshInstance.get());
	this->shipMeshInstance = std::move(shipHull);

	meshInstances.push_back((std::move(shipMeshInstance)));


	auto meshInstanceOne = std::make_unique<MeshInstance>(assetSystem->getMesh("TangentSphere"));
	meshInstanceOne->transform.setPosition(-20, 20, 35);
	meshInstanceOne->transform.setScale(15, 15, 15);
	meshInstanceOne->setMaterial(assetSystem->getMaterial("Rock"));

	auto meshInstanceTwo = std::make_unique<MeshInstance>(assetSystem->getMesh("TangentSphere"));
	meshInstanceTwo->transform.setPosition(20, 20, 35);
	meshInstanceTwo->transform.setScale(10, 10, 10);
	meshInstanceTwo->setMaterial(assetSystem->getMaterial("Gravel"));

	renderSystem->autoInsertInRenderCollection(meshInstanceOne.get());
	renderSystem->autoInsertInRenderCollection(meshInstanceTwo.get());

	meshInstances.push_back(std::move(meshInstanceOne));
	meshInstances.push_back(std::move(meshInstanceTwo));

	//Load plane for normal map testing

	//auto normalTestMeshInstance = std::make_unique<MeshInstance>(assetSystem->getMesh("TangentMesh"));
	auto normalTestMeshInstance = std::make_unique<MeshInstance>(assetSystem->getMesh("TangentMesh"));

	normalTestMeshInstance->setMaterial(assetSystem->getMaterial("NormalMaterial"));
	normalTestMeshInstance->transform.setPosition(0, -15, 70);
	normalTestMeshInstance->transform.setRoll(-90);

	//this->meshes.insert({ "NormalMesh",tMesh });
	renderSystem->autoInsertInRenderCollection(normalTestMeshInstance.get());
	this->meshInstances.push_back(std::move(normalTestMeshInstance));

	tessellationQuadInstance = std::make_unique<MeshInstance>((assetSystem->getMesh("TesselationQuad")));
	tessellationQuadInstance->setMaterial(assetSystem->getMaterial("Wave"));
	tessellationQuadInstance->transform.setPosition(0, 0, 0);
	tessellationQuadInstance->transform.setScale(10, 10, 10);

	renderSystem->setTesselatedTerrainInstance(tessellationQuadInstance.get());




	this->setRootInstances();

	this->activeMeshInstance = meshInstances[1].get();
}

void Scene::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC,
	bool FULL_SCREEN)
{
	input = in;
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);



	assetSystem = std::make_unique<AssetSystem>(*getDevice(), *getDeviceContext());
	assetSystem->loadAssets();

	renderSystem = std::make_unique<RenderSystem>(renderer, hwnd,camera,assetSystem.get(), sHeight, sWidth);

	buoyancyComputeShader = new BuoyancyComputeShader(getDevice(), hwnd, 15, 15);
	
	initPhysics();
	initLights();
	initCameras();

	initSceneComposition(screenWidth, screenHeight);

	//Setup scene buoyancy functionality
	constructHullRigidBody();
	shipHullRigidBody->setDamping(0.5, 0.3);

	//Setup destructable terrain 
	destructableTerrainPeaks.initialize(
		getDevice(),
		getDeviceContext(),
		renderSystem->getTerrainShader());


	destructableTerrainPeaks.initMeshes();
	destructableTerrainPeaks.initMeshInstances();
	destructableTerrainPeaks.terrainInstance->setMaterial(assetSystem->getMaterial("Gravel"));
	destructableTerrainPeaks.tessellatedTerrainQuadInstance->setMaterial(assetSystem->getMaterial("Gravel"));
	destructableTerrainPeaks.regenerateSimpleTerrain();
	renderSystem->setDestructableTerrain(&destructableTerrainPeaks);

	transformEditor.updateStateOfUI(&activeMeshInstance->transform);
	lightEditor.updateStateOfUI(renderSystem->lights);

	this->activeInstanceSelectorUI = new ActiveInstanceSelectorUI();
	activeInstanceSelectorUI->updateStateOfUI(this);

	timeStart = clock();

	//Ship camera values
	shipCameraRelPosition.m128_f32[0] = -4;
	shipCameraRelPosition.m128_f32[2] = -0.3;
	shipCameraFixedHeight = 15;

	//Init debug raster state

}

void Scene::initCameras()
{
	renderSystem->getCamera()->setPosition(0, 20, 0);
//	minimapCamera = new Camera();
//	minimapCamera->setPosition(50, 25, 50);
//	minimapCamera->setRotation(90, 00, 0);
}


void Scene::initLights()
{
	auto lightOne = new Light();
	lightOne->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lightOne->setDiffuseColour(0.0f, 0.8f, 0.0f, 1.0f);
	lightOne->setSpecularColour(0.0f, 1.0f, 0.0f, 1.0f);

	lightOne->setPosition(0, 15, 0);
	lightOne->setAttenuationFactors(0.2f, 0.05f, 0);
	renderSystem->lights.push_back(lightOne);
	//lights[0]->setAttenuation(0.2f, 0.1f, 0);

	auto lightTwo = new Light();
	lightTwo->setDiffuseColour(0.7f, 0.7f, 0.0f, 1.0f);
	lightTwo->setSpecularColour(1.0f, 1.0f, 0.0f, 1.0f);
	lightTwo->setDirection(0.71, 0, 0.71);
	lightTwo->innerCutOff = std::cosf(XMConvertToRadians(20.5f));
	lightTwo->outerCutOff = std::cosf(XMConvertToRadians(25.5f));
	lightTwo->setAmbientColour(0, 0, 0, 1);
	lightTwo->setPosition(5, 3, 5);
	lightTwo->setAttenuationFactors(0.2f, 0.01f, 0);
	renderSystem->lights.push_back(lightTwo);

	auto lightThree = new Light();
	lightThree->setAmbientColour(0.15f, 0.15f, 0.15f, 1);
	lightThree->setDiffuseColour(0.9f, 0.9f, 0.9f, 1.0f);
	lightThree->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	lightThree->setDirection(0, -0.466, 0.885);
	renderSystem->lights.push_back(lightThree);
}


Scene::~Scene()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();
}

void Scene::tickPhysicsSimulation()
{
	deltaClockT = clock() - timeStart;
	deltaTime = (float)(deltaClockT) / CLOCKS_PER_SEC;
	timeStart = clock();

	MultipleWaveBuffer wave_buffer = {
		renderSystem->waveParams[0],
		renderSystem->waveParams[1],
		renderSystem->waveParams[2],
	};
	buoyancyComputeShader->buoyancyParameters.worldMatrix =
		XMMatrixTranspose(shipMeshInstance->transform.getTransformMatrix());

	//shipHullRigidBody.
	buoyancyComputeShader->computeAndApplyBuoyantForce(shipHullRigidBody, shipMeshInstance.get(), renderer->getDeviceContext(),
		256, 1, 1, wave_buffer);

	if (discreteDynamicsWorld != nullptr)
	{
		// Step the simulation, calling update
		discreteDynamicsWorld->stepSimulation(deltaTime, 10);

		//Make sure to update rendering transform to visuale the physics changes
		visualizePhysicsOfDestructedComponents();

		//Track the remaining time of the desutructable componenents
		if (renderSystem->DestructableComponentsCollections != nullptr)
			renderSystem->DestructableComponentsCollections->updateTimeStep(deltaTime);

		// Print the cube's position for debugging
		btTransform cubeTransform;
		shipHullRigidBody->getMotionState()->getWorldTransform(cubeTransform);
		auto btVectorOrigin = cubeTransform.getOrigin();

		btQuaternion btQuaternionRot = shipHullRigidBody->getWorldTransform().getRotation();
		XMVECTOR rightHandedQuaternion = XMVectorSet(btQuaternionRot.getX(), btQuaternionRot.getY(),
			btQuaternionRot.getZ(), btQuaternionRot.getW());
		XMVECTOR leftHandedQuaternion = XMVectorSet(-btQuaternionRot.getX(), -btQuaternionRot.getY(),
			-btQuaternionRot.getZ(), btQuaternionRot.getW());
		btQuaternion btQuaternionLeft = btQuaternion(
			leftHandedQuaternion.m128_f32[0], leftHandedQuaternion.m128_f32[1], leftHandedQuaternion.m128_f32[2], leftHandedQuaternion.m128_f32[3]);
		auto leftAngle = btQuaternionRot.getAngle();
		auto leftAxis = btQuaternionRot.getAxis();
		auto rightAngle = btQuaternionLeft.getAngle();
		auto rightAxis = btQuaternionLeft.getAxis();

		shipMeshInstance->transform.setComposeRotationFromQuaternions(true);
		shipMeshInstance->transform.setQuaternion(leftHandedQuaternion.m128_f32[0], leftHandedQuaternion.m128_f32[1],
			leftHandedQuaternion.m128_f32[2], leftHandedQuaternion.m128_f32[3]);
		shipMeshInstance->transform.setPosition(btVectorOrigin.getX(), btVectorOrigin.getY(), btVectorOrigin.getZ());

		btVector3 cubePosition = cubeTransform.getOrigin();
	}
}

bool Scene::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.

	if (input->isKeyDown('P'))
	{
		this->changeStateOfCamera();
	}

	if (input->isKeyDown('G'))
	{
		this->renderer->setWireframeMode(!this->renderer->getWireframeState());
	}

	if (input->isKeyDown('K'))
	{
		destructableTerrainPeaks.regenerateSimpleTerrain();
	}
	if (input->isKeyDown(' '))
	{
		//auto terPtr = assetSystem->getMesh("Terrain").GetMesh();
		auto terPtr = destructableTerrainPeaks.getSimpleTerrain();

		//calculateClosestRayIntersections();
		auto parts = destructableTerrainPeaks.fireProjectileAt(XMLoadFloat3(&shootingRay.Origin), XMLoadFloat3(&shootingRay.Direction));
		for each (std::pair<MeshInstance*, btRigidBody*> p in parts)
		{
			p.first->setMaterial(assetSystem->getMaterial("Gravel"));
			discreteDynamicsWorld->addRigidBody(p.second);
			destructablePhysicsComponents.insert({ p.second, p.first });
			renderSystem->DestructableComponentsCollections->addRenderItem(p.first, 5.0f);
		}

		std::ostringstream oss;
		oss << "RayHitResult_" << destructableTerrainPeaks.simpleTerrainDetail;
		std::string filename = oss.str(); // "RayHitResultNoise_42"

	}
	applyShipForces();

	activeInstanceSelectorUI->updateStateOfUI(this);
	appTime += timer->getTime();

	if (isPhysicsPaused == false)
		tickPhysicsSimulation();

	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

void Scene::applyShipForces()
{
	applyForceToShip(getDirectionFromInput(), 500);
	steerShip();
}

ID3D11Device* Scene::getDevice()
{
	return renderer->getDevice();
}

ID3D11DeviceContext* Scene::getDeviceContext()
{
	return renderer->getDeviceContext();
}

void Scene::debugRenderBuoyancyForces(XMMATRIX view, XMMATRIX projection)
{
	// Debuging buoyanyc force on the ship
	auto positions = buoyancyComputeShader->positionAlongHull;

	auto forces = buoyancyComputeShader->buoyancyForces;


	XMVECTOR temp;
	std::vector<XMVECTOR> tempPositions;
	for (int i = 0; i < positions.size(); ++i)
	{
		temp = XMVectorSet(positions[i].x, positions[i].y, positions[i].z, 1);
		XMMATRIX mat = this->shipMeshInstance->transform.getTransformMatrix();

		//mat = XMMatrixTranspose(mat);

		temp = XMVector4Transform(temp, mat);
		positions[i].x = temp.m128_f32[0];
		positions[i].y = forces[i];
		positions[i].z = temp.m128_f32[2];
		tempPositions.push_back(XMLoadFloat3(&positions[i]));

	}
	renderDebugSphereAt(view, projection, tempPositions, assetSystem->getMaterial("Cyan"));
}

FPCamera* Scene::getCamera() { return this->camera; }

void Scene::setRootInstances()
{
	this->rootMeshInstances.clear();
	for (auto&& meshInstance : meshInstances)
	{
		if (meshInstance->transform.parent == nullptr)
		{
			this->rootMeshInstances.push_back(meshInstance.get());
		}
	}
}

void Scene::resetResources()
{
	this->rootMeshInstances.clear();
	this->meshInstances.clear();
	this->activeMeshInstance = nullptr;
}

bool Scene::render()
{
	// Clear the scene. (default blue colour)
	camera->getViewMatrix();


	auto view = camera->getViewMatrix();
	auto projection = renderer->getProjectionMatrix();

	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	renderSystem->setAppTime(appTime);

	if (buoyancyComputeShader->debugVisualizeBuoyantForces)
		debugRenderBuoyancyForces(view, projection);
	if (isRenderDebugRay)
		renderDebugSamplesAcrossRay(view, projection);
	if (isRenderingAboveAndBelowPoints)
		renderPointsAboveAndBelow(view, projection);

	//Use render system to render the scene
	bool result = renderSystem->render();

	//if (!result) return result;
	//Render GUI
	gui();
	// Present the rendered scene to the screen.
	renderer->endScene();
	return true;
}

inline void Scene::fillRenderCollections()
{
	for (size_t i = 0; i < meshInstances.size(); i++)
	{
		MeshInstance* instance = meshInstances[i].get();
		renderSystem->fillRenderCollections(instance);
	}
}



void Scene::meshInstanceTreeGuiWindow()
{
	ImGui::Begin("Mesh Instance Tree");
	activeInstanceSelectorUI->appendToImgui();
	activeInstanceSelectorUI->applyChangesTo(this);
	if (activeInstanceSelectorUI->getRawData().isNew)
	{
		this->transformEditor.updateStateOfUI(&this->activeMeshInstance->transform);
	}

	transformEditor.appendToImgui();
	transformEditor.applyChangesTo(&this->activeMeshInstance->transform);
	ImGui::End();
}


void Scene::buoyancyParametersGuiWindow()
{
	ImGui::Begin("Buoyancy Parameters");

	ImGui::Checkbox("Debug Draw Forces", &buoyancyComputeShader->debugVisualizeBuoyantForces);

	float mass = shipHullRigidBody->getMass();
	ImGui::DragFloat("Body Mass", &mass, 1, 0, 200);
	shipHullRigidBody->setMassProps(mass, shipHullRigidBody->getLocalInertia());

	ImGui::DragFloat("Fluid Density", &buoyancyComputeShader->buoyancyParameters.fluidDensity, 0.001, 0.01, 1);
	ImGui::DragFloat("Column Surface", &buoyancyComputeShader->buoyancyParameters.columnSurface, 0.1, 0, 50);
	ImGui::DragFloat("Column Volume Max", &buoyancyComputeShader->buoyancyParameters.maxColumnVolume, 0.1, 0, 50);
	float damping = shipHullRigidBody->getLinearDamping();
	float angularDamping = shipHullRigidBody->getAngularDamping();
	ImGui::DragFloat("Linear Damping", &damping, 0.01, 0, 1);
	ImGui::DragFloat("Angular Damping", &angularDamping, 0.01, 0, 1);
	shipHullRigidBody->setDamping(damping, angularDamping);
	ImGui::Text("Gradient Descent Wave Height Prediction");
	ImGui::DragFloat("GD: Eps", &buoyancyComputeShader->gradientDescentParameters.eps, 0.0001f, 0.0001f, 0.4f);
	ImGui::DragFloat("GD: LearningRate", &buoyancyComputeShader->gradientDescentParameters.learningRate, 0.01f, 0.1f, 1);
	ImGui::DragInt("GD: Iterations", &buoyancyComputeShader->gradientDescentParameters.iterations, 1, 0, 300);
	ImGui::DragFloat("GD: OffsetAlongAxis", &buoyancyComputeShader->gradientDescentParameters.offsetAlongAxis, 0.1f, 0.1f, 2);

	ImGui::End();
}


void Scene::mainGuiWindow()
{
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::Text("Press E to raise camera \nto see the plane being rendered");
	ImGui::Checkbox("Demo Window", &demoWindow);
	if (demoWindow)
	{
		ImGui::ShowDemoWindow();
	}
	ImGui::Text("MISC");
	//ImGui::Checkbox("Debug Visualize Shadow Maps", &tesselateWaveShader->debugVisalizeShadowMaps);
	//ImGui::DragFloat("Displacement Value", &displacementValue, 0.001, 0.000, 0.1);
}


void Scene::renderDebugSamplesAcrossRay(XMMATRIX view, XMMATRIX projection)
{
	XMVECTOR raySample;
	XMVECTOR Origin = XMLoadFloat3(&shootingRay.Origin);
	XMVECTOR Direction = XMLoadFloat3(&shootingRay.Direction);
	Direction = XMVector3Normalize(Direction);

	float sampleIncrement = (shootingRay.tMax - shootingRay.tMin) / this->raySamples;

	for (int i = 1; i <= this->raySamples; ++i)
	{
		std::unique_ptr<MeshInstance> newDebugPoint = std::make_unique<MeshInstance>();
		float currentIncrement = sampleIncrement * i;
		XMVECTOR currentDirectionVector = Direction * currentIncrement;
		raySample = XMVectorAdd(Origin, currentDirectionVector);

		newDebugPoint->setMesh(debugSphere->getSerializableMesh());
		newDebugPoint->setMaterial(assetSystem->getMaterial("Green"));

		newDebugPoint->transform.setPosition(
			raySample.m128_f32[0],
			raySample.m128_f32[1],
			raySample.m128_f32[2]
		);
		newDebugPoint->transform.setScale(
			this->debugSphereScale,
			this->debugSphereScale,
			this->debugSphereScale
		);

		renderSystem->addDebugInstance(std::move(newDebugPoint));
	}
}


void Scene::renderDebugSphereAt(XMMATRIX view, XMMATRIX projection, std::vector<XMVECTOR>& positions, Material* debugMaterial)
{
	for (XMVECTOR p : positions)
	{
		std::unique_ptr<MeshInstance> newDebugPoint = std::make_unique<MeshInstance>();


		newDebugPoint->setMesh(debugSphere->getSerializableMesh());
		newDebugPoint->setMaterial(debugMaterial);

		if (debugMaterial == nullptr)
			newDebugPoint->setMaterial(assetSystem->getMaterial("Yellow"));
		else
			newDebugPoint->setMaterial(debugMaterial);

		newDebugPoint->transform.setPosition(
			p.m128_f32[0],
			p.m128_f32[1],
			p.m128_f32[2]
		);
		newDebugPoint->transform.setScale(
			this->debugSphereScale,
			this->debugSphereScale,
			this->debugSphereScale
		);

		renderSystem->addDebugInstance(std::move(newDebugPoint));
	}
}

XMVECTOR Scene::getDirectionFromInput()
{
	XMVECTOR direction = XMVectorSet(0, 0, 0, 0);
	if (input->isKeyDown('W'))
	{
		//Orient in ship directoin
		//direction = XMVectorSet(0, 0, 1, 0);
		direction = XMVectorSet(1, 0, 0, 0);
	}

	XMMATRIX sm = this->shipMeshInstance->transform.getTransformMatrix();
	direction = XMVector3TransformNormal(direction, sm);
	direction = XMVector3Normalize(direction);
	return direction;
}

void Scene::applyForceToShip(XMVECTOR direction, float mag)
{
	XMVECTOR force = direction * mag;

	shipHullRigidBody->applyCentralForce(btVector3(XMVectorGetX(force), XMVectorGetY(force), XMVectorGetZ(force)));
}

void Scene::steerShip()
{
	if (input->isKeyDown('A'))
	{
		//shipHullRigidBody->applyTorque(btVector3(0, steeringTorqueAmount, 0));
		shipHullRigidBody->applyTorque(btVector3(0, steeringTorqueAmount, 0));
	}
	if (input->isKeyDown('D'))
	{
		shipHullRigidBody->applyTorque(btVector3(0, -steeringTorqueAmount, 0));
	}
}

void Scene::applyPhysicsTranform(btRigidBody* body, MeshInstance* renderInstance)
{
	// Print the cube's position for debugging
//
	btTransform BTTransform;
	body->getMotionState()->getWorldTransform(BTTransform);
	auto btVectorOrigin = BTTransform.getOrigin();

	btQuaternion btQuaternionRot = body->getWorldTransform().getRotation();
	XMVECTOR rightHandedQuaternion = XMVectorSet(btQuaternionRot.getX(), btQuaternionRot.getY(),
		btQuaternionRot.getZ(), btQuaternionRot.getW());
	XMVECTOR leftHandedQuaternion = XMVectorSet(-btQuaternionRot.getX(), -btQuaternionRot.getY(),
		-btQuaternionRot.getZ(), btQuaternionRot.getW());
	btQuaternion btQuaternionLeft = btQuaternion(
		leftHandedQuaternion.m128_f32[0], leftHandedQuaternion.m128_f32[1], leftHandedQuaternion.m128_f32[2], leftHandedQuaternion.m128_f32[3]);
	auto leftAngle = btQuaternionRot.getAngle();
	auto leftAxis = btQuaternionRot.getAxis();
	auto rightAngle = btQuaternionLeft.getAngle();
	auto rightAxis = btQuaternionLeft.getAxis();
	//XMMATRIX rotMat = XMMatrixRotationQuaternion(rightHandedQuaternion);
	renderInstance->transform.setComposeRotationFromQuaternions(true);
	renderInstance->transform.setQuaternion(leftHandedQuaternion.m128_f32[0], leftHandedQuaternion.m128_f32[1],
		leftHandedQuaternion.m128_f32[2], leftHandedQuaternion.m128_f32[3]);
	renderInstance->transform.setPosition(btVectorOrigin.getX(), btVectorOrigin.getY(), btVectorOrigin.getZ());
}

void Scene::visualizePhysicsOfDestructedComponents()
{
	for each (auto physicsMeshInstancePair in destructablePhysicsComponents)
	{
		applyPhysicsTranform(physicsMeshInstancePair.first, physicsMeshInstancePair.second);
	}
}

void Scene::renderPointsAboveAndBelow(XMMATRIX view, XMMATRIX projection)
{
	renderDebugSphereAt(view, projection, proceduralDestruction.pAbovePlane, assetSystem->getMaterial("Green"));
	renderDebugSphereAt(view, projection, proceduralDestruction.pBelowPlane, assetSystem->getMaterial("Red"));
	renderDebugSphereAt(view, projection, proceduralDestruction.pIntersections, assetSystem->getMaterial("Cyan"));
}

void Scene::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	ImGui::Begin("Debug Mode");
	ImGui::Checkbox("Is Debug Mode", &this->isDebugMode);
	ImGui::Checkbox("Is Physics Paused", &this->isPhysicsPaused);
	ImGui::Checkbox("Camera To Ship", &this->isCameraAttachedToShip);
	ImGui::End();

	shipShootRayGuiWindow();
	shipCameraGuiWindow();

	if (isDebugMode)
	{
		//Terrain gui options
		terrainGenerationGuiWindow();
		testingRayGuiWindow();
		// Build UI
		mainGuiWindow();
		meshInstanceTreeGuiWindow();
		buoyancyParametersGuiWindow();
		renderSystem->gui();

		lightEditor.appendToImgui();
		lightEditor.applyChangesTo(renderSystem->lights);
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Scene::shipShootRayGuiWindow()
{
	//Configure the ship ray's local values
	ImGui::Begin("Terrain Generation");
	ImGui::InputFloat3("Rel Position", &shootingRayShipRelative.Origin.x, -25, 25);
	ImGui::SliderFloat3("Rel Direction", &shootingRayShipRelative.Direction.x, -25, 25);
	ImGui::SliderFloat("Range Min", &shootingRayShipRelative.tMin, 0, 25);
	ImGui::SliderFloat("Range Max", &shootingRayShipRelative.tMax, shootingRayShipRelative.tMin, 25);

	shootingRay = computeGlobalShootingRay(shootingRayShipRelative);
	ImGui::End();
}

Ray3D Scene::computeGlobalShootingRay(const Ray3D& relRay)
{
	Ray3D globalRay;
	//Setup the parent hierarchy and apply to actual ray
	XMMATRIX shipMatrix = shipMeshInstance->transform.getTransformMatrix();

	XMVECTOR pos = XMLoadFloat3(&relRay.Origin);
	XMVECTOR dir = XMLoadFloat3(&relRay.Direction);

	pos = XMVector3Transform(pos, shipMatrix);
	dir = XMVector3TransformNormal(dir, shipMatrix);

	XMStoreFloat3(&globalRay.Origin, pos);
	XMStoreFloat3(&globalRay.Direction, dir);
	globalRay.tMin = relRay.tMin;
	globalRay.tMax = relRay.tMax;
	return globalRay;

}

void Scene::shipCameraGuiWindow()
{
	//Configure the ship ray's local values
	ImGui::Begin("Ship Camera");

	if (isDebugMode == false)
	{
		XMVECTOR tempPos = XMLoadFloat3(&camera->getPosition());
		//XMVECTOR tempDir = XMLoadFloat3(&camera->getRotation());
		ImGui::InputFloat3("Rel Position", &shipCameraRelPosition.m128_f32[0], -25, 25);
		ImGui::SliderFloat("Fixed Height", &shipCameraFixedHeight, -25, 25);

		//Assuem looking to the left
		//XMVECTOR dirAtShip = XMVectorSubtract(shipMeshInstance->transform.getPosition(), tempPos);
		XMVECTOR rayOrigin = XMLoadFloat3(&shootingRay.Origin);
		XMVECTOR rayDirection = XMLoadFloat3(&shootingRay.Direction);

		XMVECTOR endOfRay = XMVectorAdd(rayOrigin, (rayDirection * shootingRay.tMax));

		XMVECTOR dirAtShip = XMVectorSubtract(endOfRay, tempPos);
		XMVECTOR tempDir = XMVectorSet(1, 0, 0, 0);

		XMVECTOR shipPos = this->shipMeshInstance->transform.getPosition();
		XMVECTOR toShip = XMVectorSubtract(shipPos, tempPos);
		toShip = XMVector3Normalize(toShip);

		dirAtShip = XMVector3Normalize(dirAtShip);
		ImGui::SliderFloat3("Rel Direction", &dirAtShip.m128_f32[0], -25, 25);

		float yaw = std::atan2(XMVectorGetX(toShip), XMVectorGetZ(toShip)) * (180.0f / XM_PI);

		ImGui::SliderFloat("Yawj", &yaw, -25, 25);

		if (yaw < 0.0f)        yaw += 360.0f;
		else if (yaw >= 360)   yaw -= 360.0f;

		computeShipGlobalCameraTransform(yaw);
	}
	ImGui::End();
}

void Scene::computeShipGlobalCameraTransform(float yaw)
{
	XMVECTOR tempCameraPosition;
	XMMATRIX shipMatrix = shipMeshInstance->transform.getTransformMatrix();
	tempCameraPosition = XMVector3Transform(this->shipCameraRelPosition, shipMatrix);
	tempCameraPosition.m128_f32[1] = this->shipCameraFixedHeight;
	camera->setPosition(XMVectorGetX(tempCameraPosition), XMVectorGetY(tempCameraPosition), XMVectorGetZ(tempCameraPosition));
	camera->setRotation(0, yaw, 0);
}

void Scene::terrainGenerationGuiWindow()
{
	ImGui::Begin("Terrain Generation");

	ImGui::Checkbox("Simple Terrain", &this->isRenderingSimpleTerrain);
	ImGui::Checkbox("Extracked Peak", &this->isRenderTerrainPeak);
	ImGui::Checkbox("Destructable Peak", &this->isRenderDestructablePeak);
	ImGui::Checkbox("Tesselated Terrain", &this->isRenderingTessellatedTerrain);
	if (ImGui::CollapsingHeader("Detail"))
	{
		ImGui::SliderFloat("Simple Terrain Detail", &destructableTerrainPeaks.simpleTerrainDetail, 5, 300);
		ImGui::SliderFloat("Tess Terrain Detail", &destructableTerrainPeaks.tessTerrainDetail, 5, 300);
		ImGui::SliderFloat("Terrain Size", &destructableTerrainPeaks.terrainSizeXZUnits, 10, 1000);

		if (ImGui::Button("Apply Terrain Detail"))
		{
			applyTerrainDetail();
		}
	}

	if (ImGui::CollapsingHeader("Transform"))
	{
		XMVECTOR tempPos = destructableTerrainPeaks.tessellatedTerrainQuadInstance->transform.getPosition();
		int posRange = 150;
		ImGui::SliderFloat3("Position", &tempPos.m128_f32[0], -posRange, posRange);
		destructableTerrainPeaks.terrainInstance->transform.setPosition(XMVectorGetX(tempPos), XMVectorGetY(tempPos), XMVectorGetZ(tempPos));
		destructableTerrainPeaks.tessellatedTerrainQuadInstance->transform.setPosition(XMVectorGetX(tempPos), XMVectorGetY(tempPos), XMVectorGetZ(tempPos));
	}

	auto ter = renderSystem->getTerrainShader();
	if (ImGui::CollapsingHeader("Noise Properties"))
	{
		if (ImGui::CollapsingHeader("Noise Main FBM"))
		{
			ImGui::SliderFloat("Wave Amplitude", &ter->fBMParams.amplitude, 0.0f, 50.0f);
			ImGui::SliderFloat("Wave Frequuency", &ter->fBMParams.frequency, 0.001, 100);
			ImGui::SliderFloat("fBM: Lacunarity", &ter->fBMParams.lucanarity, 1, 10);
			ImGui::SliderFloat("fBM: Gain", &ter->fBMParams.gain, 0.0f, 1.0f);
			ImGui::SliderInt("fBM: Octaves", &ter->fBMParams.octaves, 1, 10);
		}
		if (ImGui::CollapsingHeader("Noise One"))
		{
			ImGui::SliderFloat("Noise1: Amplitude", &ter->fBMParams.NoiseOne.amplitude, 0.0f, 50.0f);
			ImGui::SliderFloat("Noise1: Frequuency", &ter->fBMParams.NoiseOne.frequency, 0.001, 100);
		}
		if (ImGui::CollapsingHeader("Noise Two"))
		{
			ImGui::SliderFloat("Noise2: Amplitude", &ter->fBMParams.NoiseTwo.amplitude, 0.0f, 50.0f);
			ImGui::SliderFloat("Noise2: Frequuency", &ter->fBMParams.NoiseTwo.frequency, 0.001, 100);
		}
		if (ImGui::CollapsingHeader("Noise Three"))
		{
			ImGui::SliderFloat("Noise3: Amplitude", &ter->fBMParams.NoiseThree.amplitude, 0.0f, 50.0f);
			ImGui::SliderFloat("Noise3: Frequuency", &ter->fBMParams.NoiseThree.frequency, 0.001, 100);
		}
		if (ImGui::CollapsingHeader("Noise Four"))
		{
			ImGui::SliderFloat("Noise4: Amplitude", &ter->fBMParams.NoiseFour.amplitude, 0.0f, 50.0f);
			ImGui::SliderFloat("Noise4: Frequuency", &ter->fBMParams.NoiseFour.frequency, 0.001, 100);
		}
		if (ImGui::CollapsingHeader("Noise Four"))
		{
			ImGui::SliderFloat("Noise5: Amplitude", &ter->fBMParams.NoiseFive.amplitude, 0.0f, 50.0f);
			ImGui::SliderFloat("Noise5: Frequuency", &ter->fBMParams.NoiseFive.frequency, 0.001, 100);
		}
	}

	if (ImGui::CollapsingHeader(" Rendering"))

	{

		ImGui::SliderFloat("UV Density", &ter->terrainDiplacementNormalData.uvDensity, 1, 15);
		ImGui::SliderFloat("Displacement Strength", &ter->terrainDiplacementNormalData.displacementStrength,0.1,5);
		ImGui::SliderFloat("EPS", &ter->terrainDiplacementNormalData.EPS, 0.00001,0.1);
		ImGui::Checkbox("Debug", &ter->debugVisalizeShadowMaps);



		ImGui::SliderFloat("Relative Height One", &ter->TerrainParameters.heightOne, 0, 1);
		ImGui::SliderFloat("Relative Height Two", &ter->TerrainParameters.heightTwo, ter->TerrainParameters.heightOne, 1);
		ImGui::SliderFloat("Relative Height Three", &ter->TerrainParameters.heightThree, ter->TerrainParameters.heightTwo, 1);

		ImGui::ColorPicker3("Range One Color", &ter->TerrainParameters.rangeColorOne.x);
		ImGui::ColorPicker3("Range Two Color", &ter->TerrainParameters.rangeColorTwo.x);
		ImGui::ColorPicker3("Range Three Color", &ter->TerrainParameters.rangeColorThree.x);

		ImGui::SliderFloat("Region Sample X ", &this->regionSampleOffsetX, 0.0f, 50.0f);
		ImGui::SliderFloat("Region Sample Y ", &this->regionSampleOffsetY, 0.0f, 50.0f);
	}

	ImGui::End();
}

void Scene::applyTerrainDetail()
{
	destructableTerrainPeaks.applyTerrainDetail();

}


void Scene::calculateAllRayIntersections()
{
}

void Scene::testingRayGuiWindow()
{
	ImGui::Begin("Testing Ray Control");

	//ImGui::Checkbox("Render", &this->isRenderDebugRay);
	ImGui::Checkbox("Render Plane A and B", &this->isRenderingAboveAndBelowPoints);

	if (ImGui::Button("AllIntersections"))
	{
		calculateAllRayIntersections();
	}

	ImGui::DragFloat("Bottom Plane Height", &this->destructableTerrainPeaks.bottomPlaneY, 0.1);
	ImGui::DragFloat("Water Plane Height", &this->destructableTerrainPeaks.waterPlaneY, 0.1);
	ImGui::DragInt("Peak Target Dimension", &this->destructableTerrainPeaks.peakTargetDim, 5);
	ImGui::DragFloat("Debug Sphere", &this->debugSphereScale, 0.5);
	ImGui::InputInt("Ray Samplers", &this->raySamples);
	ImGui::DragFloat3("Origin", &this->shootingRay.Origin.x, 0.5);
	ImGui::DragFloat3("Direction", &this->shootingRay.Direction.x, 0.01);
	XMVECTOR temp = XMLoadFloat3(&this->shootingRay.Direction);
	temp = XMVector3Normalize(temp);
	XMStoreFloat3(&this->shootingRay.Direction, temp);

	ImGui::DragFloat("Range Min", &this->shootingRay.tMin, 0.5);
	ImGui::DragFloat("Range Max", &this->shootingRay.tMax, 0.5);

	ImGui::End();
}


void Scene::changeStateOfCamera() { activeCameraMovement = !activeCameraMovement; }


// Application.h
#ifndef SCENE_H
#define SCENE_H

// Includes
#include <chrono>

#include "DXF.h"
#include <map>
#include <set>
#include <random>
#include <cmath>
#include <functional>
#include <wrl/client.h>

#include "AssetSystem.h"
#include "RenderSystem.h"

#include "Material.h"
#include "DefaultShader.h"
#include "SerializableMesh.h"
//Scene Editors
#include "LightEditorUI.h"
#include "TransformEditorUI.h"
#include "ActiveInstanceSelectorUI.h"

#include "BuoyancyComputeShader.h"

class btRigidBody;
class btDiscreteDynamicsWorld;
class SceneJsonSerializer;
struct Ray3D
{
	XMFLOAT3 Origin;
	XMFLOAT3 Direction;
	float tMin = 0;
	float tMax = 0;
};

class Scene : public BaseApplication
{
public:

	Scene();
	~Scene();

	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC,
		bool FULL_SCREEN) override;

	bool frame() override;
	bool render() override;

	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	FPCamera* getCamera();

	void resetResources();

	//--- SCENE GRAPH ---
	std::vector<MeshInstance*> rootMeshInstances;
	std::vector<std::unique_ptr<MeshInstance>> meshInstances;
	MeshInstance* activeMeshInstance;  //The instances selected by the user from the UI.

protected:

	//--- COMPUTE / SPECIAL SYSTEMS ---
	ProceduralDestruction proceduralDestruction;
	BuoyancyComputeShader* buoyancyComputeShader;
	DestructibleTerrainPeaks destructableTerrainPeaks;

	//PHYSICS
	btDiscreteDynamicsWorld* discreteDynamicsWorld;
	//The buoyant meshes rigid body. Use to apply the buoyant force from the compute shader
	btRigidBody* shipHullRigidBody;

	//--- INITIALIZATION METHODS ---
	void initPhysics();
	void initSceneComposition(int screenWidth, int screenHeight);
	void initCameras();
	void initLights();

	//--- RENDER QUEUING ---
	void fillRenderCollections();

	//--- PHYSICS ---
	void tickPhysicsSimulation();

	//--- GUI WINDOWS ---
	void gui();
	void terrainGenerationGuiWindow();
	void testingRayGuiWindow();
	void mainGuiWindow();
	void meshInstanceTreeGuiWindow();
	void buoyancyParametersGuiWindow();
	void shipShootRayGuiWindow();
	void shipCameraGuiWindow();

	//--- SCENE DEBUG RENDERS ---
	void renderDebugSamplesAcrossRay(XMMATRIX view, XMMATRIX projection);
	void renderPointsAboveAndBelow(XMMATRIX view, XMMATRIX projection);

#pragma region Ship Controller
	//--- SHIP CONTROLLER ---
	const float steeringTorqueAmount = 100000.f;

	void applyShipForces();
	void applyForceToShip(XMVECTOR direction, float mag);
	void steerShip();

	XMVECTOR getDirectionFromInput();

	//--- SHIP CAMERA ---
	XMVECTOR shipCameraRelPosition;
	float shipCameraFixedHeight;

	//--- SHIP SHOOTING RAY ---
	bool isRenderDebugRay = true;
	int raySamples = 10;
	float debugSphereScale = 1;
	float regionSampleOffsetX = 10;
	float regionSampleOffsetY = 10;

	Ray3D shootingRay;
	Ray3D shootingRayShipRelative;

	Ray3D computeGlobalShootingRay(const Ray3D& relRay);
	void computeShipGlobalCameraTransform(float yaw);
#pragma endregion

	/// --- DESTROYED TERRAIN PHSYSICS ---
	std::map<btRigidBody*, MeshInstance*>  destructablePhysicsComponents; //A container for the physics representation of the destructable components
	void applyPhysicsTranform(btRigidBody* body, MeshInstance* renderInstance); //Translate the transform from Bullet 3D to rendering terms
	void visualizePhysicsOfDestructedComponents();

private:
	Input* input;

	//--- TIMEKEEPING ---
	float appTime = 0; // Application Time Since Startup
	float deltaTime; //float point difference from previous frame()
	clock_t timeStart; // Application ticks for previous frame()
	clock_t deltaClockT; // tick difference from previous frame()

	//--- APP FLAGS ---
	bool isDebugMode;
	bool isCameraAttachedToShip;
	bool activeCameraMovement = true;
	bool isPhysicsPaused;

	//--- RENDER TOGGLES ---
	bool demoWindow;
	bool isRenderTerrainPeak = false;
	bool isRenderDestructablePeak = false;
	bool isRenderingSimpleTerrain = false;
	bool isRenderingTessellatedTerrain = true;
	bool isRenderingAboveAndBelowPoints = true;

	//--- IMPORTANT SUBSYSTEMS ---
	std::unique_ptr<AssetSystem> assetSystem;
	std::unique_ptr<RenderSystem> renderSystem;

	//--- SPECIAL INSTANCES ---
	std::unique_ptr<MeshInstance> tessellationQuadInstance;  //The buoyant mesh in the scene
	std::unique_ptr<MeshInstance> shipMeshInstance;
	std::unique_ptr<MeshInstance> debugSphere;

	//--- UI STATE ---
	TransformEditorUI transformEditor; //Synchronizes what is displayer by ImGui and actual paramters of instnaces' transform
	LightEditorUI lightEditor;
	ActiveInstanceSelectorUI* activeInstanceSelectorUI;  //Updates the activeMeshInstance to what the suer selelcted in the mesh instance tre

	//--- PRIVATE UTILITY METHODS ---
	void changeStateOfCamera();
	void constructHullRigidBody();
	void applyTerrainDetail();
	void calculateAllRayIntersections();
	void debugRenderBuoyancyForces(XMMATRIX view, XMMATRIX projection);
	void renderDebugSphereAt(XMMATRIX view, XMMATRIX projection, std::vector<XMVECTOR>& positions, Material* debugMaterial = nullptr);
	void setRootInstances();
};

#endif

#pragma once
//#include "D:\UniveristyProjects\Year 5 Masters\CMP402 Programming For Games\cmp301_coursework-TediShopov\Coursework\DXFramework\BaseApplication.h"
#include "BaseApplication.h"
#include "SerializableMesh.h"
#include "TesselatedGerstnerWaveShader.h"
#include "TessellationShader.h";
#include "TransformEditorUI.h"
#include "LightMaterialShader.h"
#include "btBulletDynamicsCommon.h"
#include "AssetSystem.h"

#include "WaveShader.h"
class SkyMapShader;
class ActiveInstanceSelectorUI;
class MeshInstance;

class TestingScene :
    public BaseApplication
{
public:
	std::unique_ptr<AssetSystem> assetSystem;

//	void addTexture(std::wstring name, std::wstring fileapth)
//	{
//		this->textureMap.insert({ name,fileapth });
//		textureMgr->loadTexture(name,fileapth);
//	}
	#pragma region INITIALIZATION METHODS
	//---INITIALIZATION METHODS//
	void initMeshes();
	void initShaders(HWND hwnd);
	void initCameras();
#pragma endregion
	#pragma region SCENE VARIABLES
	//---SCENE VARIABLES---
	//std::vector<Light*> lights;

	//Texuter paths in use
	std::map<std::wstring, std::wstring> textureMap;

	//Materials
	//std::map<std::string, Material*> materials;

	//Meshes
	std::map<std::string, SerializableMesh> meshes;

	////Mehs instances and mehs instance tree
	std::vector<MeshInstance*> rootMeshInstances;
	std::vector<MeshInstance*> meshInstances;
	MeshInstance* skyboxMesh;
	
	//The instances selected by the user from the UI. 
	//That instnace's classses will be updated by the EditorUI methods
	MeshInstance* activeMeshInstance;
	MeshInstance* tessellationQuadInstance;

//	//Resources for minimap feature
//	OrthoMesh* orthoMesh;

	//Resources for minimap feature
	//RenderTexture* sceneTexture;
	ID3D11ShaderResourceView* sceneTextureResource;
	//OrthoMesh* fullScreenOrthoMesh;


	//ShadowMap* shadowMap;
	bool tempUseShadowMapForPass = false;
	//Is camera movement allowed or not
	bool activeCameraMovement = true;
	bool demoWindow;
	bool renderSceneToTexute;

	TesselationFactors tesselationFactors;
	WaveParameters waveParams[3];

	float appTime = 0;
#pragma endregion

	TestingScene();
	~TestingScene();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);
	
	bool frame() override;
	

	ID3D11RasterizerState* _debugRasterState;

	//---UI RELATED VARIABLES---
	//Synchronizes what is displayer by ImGui and actual paramters of instnaces' transform
	TransformEditorUI transformEditor;
	//Updates the activeMeshInstance to what the suer selelcted in the mehs instance tree
	ActiveInstanceSelectorUI* activeInstanceSelectorUI;
	FPCamera* getCamera();
	//void setRootInstances();
	//void resetResources();

	//void setupBaseShaderParamters(DefaultShader* baseShader, MeshInstance* instance, XMMATRIX view, XMMATRIX projection);
	void renderTesselatedWave(XMMATRIX view, XMMATRIX projection);

	ID3D11Device* getDevice() { return renderer->getDevice(); }
	ID3D11DeviceContext* getDeviceContext() { return renderer->getDeviceContext(); }

protected:
	bool render() override;
	void gui();
private:
    Input* input;

	#pragma region SHADERS
	//--SHADERS--
	SkyMapShader* skyMapShader;
	TessellationShader* tessellationShader;

	//TESSELATED
	WaveShader* waveShader;
	WaveShader* gerstnerWaveShader;
	TesselatedGerstnerWaveShader* tesselateWaveShader;
	
#pragma endregion


};


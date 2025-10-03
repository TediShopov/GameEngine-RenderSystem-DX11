#pragma once

// Includes
#include "DXF.h"
#include "DefaultShader.h"
#include "LightMaterialShader.h"
#include "DisplacementShader.h"
#include "NormalMapShader.h"
#include "TextureShader.h"
#include "WaveShader.h"
#include "RenderTexture.h"
#include "MeshInstance.h"
#include "TangentMesh.h"
#include "TransformEditorUI.h"
#include "LightEditorUI.h"
#include "Material.h"
#include "TessellationShader.h"
#include "TessellationQuad.h"
#include "SerializableMesh.h"
#include "UnderwaterEffectShader.h"
#include "TesselatedGerstnerWaveShader.h"
#include "ActiveInstanceSelectorUI.h"
#include "CascadedShadowMaps.h"
#include "HorizontalBlur.h"
#include "MagnifyPixelShader.h"
#include "VerticalBlur.h"
#include <map>
#include <set>
#include <functional>
#include "LuminanceThresholdPass.h"
#include "BloomComposite.h"

#include "BuoyancyComputeShader.h"
#include "RenderItem.h"
#include "PostProcessPassChain.h"
#include "Terrain.h"
#include "TerrainShader.h"
#include "LimitedTimeRenderCollection.h"
#include "insideOutsideShader.h"
#include "ProceduralDestruction.h"
#include "DestructableTerrainPeaks.h"
#include <random>
#include <cmath>
#include <wrl/client.h>
class TerrainTesselationShader;
class btRigidBody;
class btDiscreteDynamicsWorld;
class SkyMapShader;
class SceneJsonSerializer;
class AssetSystem;
class RenderSystem
{
public:
	RenderSystem(D3D* renderer, HWND hwnd, FPCamera* camera, AssetSystem* assetSystem, int height, int width);
	~RenderSystem();

	bool render();

	void gui();

	void setAppTime(float time);
	void setTesselatedTerrainInstance(MeshInstance* terrainInstance);
	void setDestructableTerrain(DestructibleTerrainPeaks* destructableTerrain);

	void addDebugInstance(std::unique_ptr<MeshInstance> meshInstance);
	void fillRenderCollections(MeshInstance* instance);
	void autoInsertInRenderCollection(MeshInstance* instance);

	TerrainTesselationShader* getTerrainShader() const;
	FPCamera* getCamera() const;

	//--- RENDER CONFIG FLAGS ---
	bool enableBlur;
	bool enableBloom = true;
	bool enableMagnify;
	bool enableWaterDistortion;

	bool isPhysicsPaused;
	bool isRenderTerrainPeak = false;
	bool isRenderDestructablePeak = false;
	bool isRenderSceneToTexture = true;
	bool isPostProcessing = true;
	bool isRenderDebug = true;

	bool isRenderingWaves = true;
	bool isRenderingSimpleTerrain = false;
	bool isRenderingTessellatedTerrain = true;

	LimitedTimeRenderCollection* DestructableComponentsCollections;

	float displacementValue;

	//Lights
	std::vector<Light*> lights;

	WaveParameters waveParams[3];
protected:

	//--- INTIALIZATION ---
	void initShaders(HWND hwnd);
	void initTextures(int screenWidth, int screenHeight);
	void initRasterStates();
	void initRenderCollections();
	void initCascadedShadowMaps();

	//--- FRAME/PIPELINE PASSES ---

	//Render passes for the whole scene(mesh instance tree)
	void renderScene(const XMMATRIX view, const XMMATRIX projection);
	void renderSceneToTexture(RenderTexture& out, const XMMATRIX view, const XMMATRIX projection);
	//Cacaded shadow map pass
	void cascadedShadowMapPass(Light& directionalLight);
	void renderSkyboxPass();

	void renderTerrain();
	void renderTesselatedTerrain(const XMMATRIX view, const XMMATRIX projection);
	void renderTesselatedWave(const XMMATRIX view, const XMMATRIX projection);
	void renderDebugInstancesPass();

	//--- POST-PROCESSING HELPERS ---
	void setupPostProcessingPass();
	void setupFullSreenPostProcessing(TextureShader& textureShader, RenderTexture& renderTexture);
	void renderOnFullScreenOrtho(TextureShader& textureShader, RenderTexture& renderTexture);

	void doPostProcessingPass(TextureShader& passShader, RenderTexture& out, RenderTexture& in);

	void waterDistortionPass(RenderTexture& out, RenderTexture& in, RenderTexture& renderTexture);

	void renderShadowMapToOrthoMesh(ShadowMap* map, OrthoMesh* mesh);

	//--- UTILITY/HELPERS ---
	void setupBaseShaderParamters(DefaultShader& baseShader);
	void setupInstanceParameter(DefaultShader& baseShader, MeshInstance* instance);
	void renderDefaultTexture(RenderTexture& out);
	void renderTexture();

	// --GUI WINDOWS--
	void destructableComponentEffectGuiWindow();
	void fogParameterGuiWindow();
	void waveParametersGuiWindow();
	void postProcessingGuiWindow();
	void screenSpaceReflectionGuiWindow();

	D3D* renderer;
	FPCamera* camera;
	AssetSystem* assetSystem;
	int sWidth, sHeight;

	float appTime;

	inline ID3D11Device* getDevice() {
		return renderer->getDevice();
	};
	inline ID3D11DeviceContext* getDeviceContext() {
		return renderer->getDeviceContext();
	};

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> _debugRasterState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> _rasterizedStateCullOn;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> _rasterizedStateCullOff;

	//--- RENDER PARAMTERS ---
	TesselationFactors tesselationFactors;
	FogParametersType fogParameters;
	TextureDistortionBuffer distortionBuffer;
	ViggneteMask edgeBlurMask;
	ViggneteMask magnifyMask;
	ViggneteMask bloomBlurMask;

	PostProcessPassChain* passChain;

	//--- SHADERS ---
	std::unique_ptr<TerrainShader> terrainShader;
	std::unique_ptr<DefaultShader> defaultShadowShader;
	std::unique_ptr<NormalMapShader> normalAndDisplacementShader;
	std::unique_ptr<SkyMapShader> skyMapShader;
	std::unique_ptr<TerrainTesselationShader> terrainTesselationShader;
	std::unique_ptr<insideOutsideShader> insideOutsideShaderInstance;
	std::unique_ptr<TextureShader> textureShader;
	std::unique_ptr<WaveShader> gerstnerWaveShader;
	std::unique_ptr<TesselatedGerstnerWaveShader> tesselateWaveShader;

	//--- POST-PROCESSING SHADERS ---
	std::unique_ptr<UnderwaterEffectShader> underwaterEffectShader;
	std::unique_ptr<Blur> horizontalBlur;
	std::unique_ptr<Blur> verticalBlur;
	std::unique_ptr<MagnifyPixelShader> magnify;
	std::unique_ptr<LuminanceThresholdPass> thresholdPass;
	std::unique_ptr<BloomComposite> bloomCompositePass;

	//--- RENDER TARGETS ---
	std::unique_ptr<RenderTexture> sceneRT;
	std::unique_ptr<RenderTexture> effectRT;
	std::unique_ptr<RenderTexture> bloomTextureOne;
	std::unique_ptr<RenderTexture> bloomTextureTwo;

	std::unique_ptr<OrthoMesh> orthoMesh;
	std::unique_ptr<OrthoMesh> fullScreenOrthoMesh;

	// THE SIMPLE AND TESSELATED TERRAIN MESHES
	MeshInstance* tessellationQuadInstance;

	//Destructable Terrain Peaks
	DestructibleTerrainPeaks* destructableTerrainPeaks;


	//--DEBUG--
	//Debug visualization container. Consumed per frame
	std::vector<std::unique_ptr<MeshInstance>> debugItems;

#pragma region SCREEN SPACE REFLECTIONS
	//For storing the color values from the first pass of rendering
	ID3D11ShaderResourceView* colourShaderResourceView;
	//For storing the depth values from the first pass of rendering
	ID3D11ShaderResourceView* depthShaderResourceView;
#pragma endregion

	//CASCADED SHADOW MAPS
	bool tempUseShadowMapForPass = false;
	std::unique_ptr<CascadedShadowMaps> cascadedShadowMaps;

	//--RENDER COLLECTIONS--
	//Render collection skupecify a different render strategy for their objects
	std::map<DefaultShader*, RenderItemCollection*> renderCollections;

};

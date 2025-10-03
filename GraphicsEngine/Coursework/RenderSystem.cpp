#include "RenderSystem.h"
#include "AssetSystem.h"
#include "SkyMapShader.h"



void RenderSystem::renderTerrain()
{
	if (isRenderingSimpleTerrain)
	{
		setupBaseShaderParamters(*terrainShader);
		setupInstanceParameter(*terrainShader, destructableTerrainPeaks->terrainInstance);

		//auto terPtr = (Terrain*)meshes["Terrain"].GetMesh();
		auto terPtr = destructableTerrainPeaks->getSimpleTerrain();
		terrainShader->SetTerrainData(renderer->getDeviceContext(), terPtr);
		terPtr->sendData(getDeviceContext());

		terrainShader->render(getDeviceContext(), terPtr->getIndexCount());
	}
	//Render the extract peak

	if (isRenderTerrainPeak && destructableTerrainPeaks->terrainPeak != nullptr)
	{
		setupBaseShaderParamters(*defaultShadowShader);
		setupInstanceParameter(*defaultShadowShader, destructableTerrainPeaks->terrainInstance);
		destructableTerrainPeaks->terrainPeak->sendData(renderer->getDeviceContext());
		defaultShadowShader->render(renderer->getDeviceContext(), destructableTerrainPeaks->terrainPeak->getIndexCount());
	}
	if (isRenderDestructablePeak && destructableTerrainPeaks->destructablePeakMesh != nullptr)
	{
		setupBaseShaderParamters(*defaultShadowShader);
		setupInstanceParameter(*defaultShadowShader, destructableTerrainPeaks->terrainInstance);
		destructableTerrainPeaks->destructablePeakMesh->sendData(renderer->getDeviceContext());
		defaultShadowShader->render(renderer->getDeviceContext(), destructableTerrainPeaks->destructablePeakMesh->getIndexCount());
	}
}

void RenderSystem::renderTesselatedTerrain(const XMMATRIX view, const XMMATRIX projection)
{
	//Set textures
	terrainTesselationShader->terrainTexture1 =
		assetSystem->getTexture(L"diffuseTerrainSand");
	terrainTesselationShader->terrainTexture2 =
		assetSystem->getTexture(L"diffuseTerrainGrass");
	terrainTesselationShader->terrainTexture3 =
		assetSystem->getTexture(L"diffuseTerrainRock");
	terrainTesselationShader->terrainNormal =
		assetSystem->getTexture(L"gravelNormal");
	terrainTesselationShader->terrainDisplacement =
		assetSystem->getTexture(L"gravelDisplacement");

	auto lProj = cascadedShadowMaps->getOrthoMatrix(0);
	auto lView = cascadedShadowMaps->getViewMatrix(0);

	destructableTerrainPeaks->tessellatedTerrainQuadInstance->getMesh()->sendData(renderer->getDeviceContext(),
		D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	ID3D11ShaderResourceView* texture = nullptr;

	if (!destructableTerrainPeaks->tessellatedTerrainQuadInstance->getMaterial()->diffuseTexture.empty())
	{
		texture = assetSystem->getTexture(destructableTerrainPeaks->tessellatedTerrainQuadInstance->getMaterial()->diffuseTexture);
	}

	terrainTesselationShader->setWorldPositionAndCamera(renderer->getDeviceContext(),
		destructableTerrainPeaks->tessellatedTerrainQuadInstance->transform.getTransformMatrix(),
		camera->getPosition(),
		SCREEN_NEAR, SCREEN_DEPTH
	);

	if (terrainTesselationShader->ssrParameters.useSSR)
	{
		terrainTesselationShader->setSSRColorAndDepthTextures(
			getDeviceContext(),
			colourShaderResourceView,
			depthShaderResourceView
		);
	}

	terrainTesselationShader->setTesselationFactors(
		renderer->getDeviceContext(), tesselationFactors);
	ShadowMappingLights lightsMatriceData{
		cascadedShadowMaps->getViewMatrix(0),
		cascadedShadowMaps->getOrthoMatrix(0),
		cascadedShadowMaps->getViewMatrix(1),
		cascadedShadowMaps->getOrthoMatrix(1),
		cascadedShadowMaps->getViewMatrix(2),
		cascadedShadowMaps->getOrthoMatrix(2),
	};
	terrainTesselationShader->setShaderParameters(renderer->getDeviceContext(),
		view, projection,
		lightsMatriceData,
		lights.data(),
		camera->getPosition(),
		sWidth,
		sHeight
	);
	terrainTesselationShader->setShaderParametersForInstance(
		renderer->getDeviceContext(),
		destructableTerrainPeaks->tessellatedTerrainQuadInstance->transform.getTransformMatrix(),
		destructableTerrainPeaks->tessellatedTerrainQuadInstance->getMaterial(),
		texture
	);
	terrainTesselationShader->sendfBMParams(getDeviceContext(), terrainTesselationShader->fBMParams);

	terrainTesselationShader->sendTerrainParams(getDeviceContext());
	terrainTesselationShader->render(renderer->getDeviceContext(), destructableTerrainPeaks->tessellatedTerrainQuadInstance->getMesh()->getIndexCount());
}

void RenderSystem::setupPostProcessingPass()
{
	renderer->setZBuffer(false);
	//out->setAsRenderTarget(renderer->getDeviceContext());
}

void RenderSystem::renderTexture()
{
}

void RenderSystem::renderSkyboxPass()
{
	//Set default values for the blend state

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE; // Disable alpha-to-coverage
	blendDesc.IndependentBlendEnable = FALSE; // One blend state for all targets

	D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
	rtBlendDesc.BlendEnable = FALSE; // Disable blending
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // Enable writing to all color channels

	blendDesc.RenderTarget[0] = rtBlendDesc;

	ID3D11BlendState* blendState = nullptr;
	getDevice()->CreateBlendState(&blendDesc, &blendState);
	getDeviceContext()->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF); // Bind the blend state
	blendState->Release();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE; // Enable depth testing

	// Allow writing to the depth buffer
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//The equal here is important as the vertices will always be projected getHeightAt the far plane
	//to simulate infinite distance;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	ID3D11DepthStencilState* depthStencilState = nullptr;
	getDevice()->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	getDeviceContext()->OMSetDepthStencilState(depthStencilState, 0); // Bind the depth-stencil state
	depthStencilState->Release();

	//Set the debug raster state if needed
	renderer->getDeviceContext()->RSSetState(_debugRasterState.Get());

	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	auto ms = new MeshInstance(assetSystem->getMesh("SkyBox"));
	float scale = 1;
	ms->transform.setPosition(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ms->transform.setScale(scale, scale, scale);
	ms->getMesh()->sendData(renderer->getDeviceContext());
	skyMapShader->setShaderParameters(renderer->getDeviceContext(), ms->transform.getTransformMatrix(), viewMatrix,
		projectionMatrix);
	skyMapShader->render(getDeviceContext(), ms->getMesh()->getIndexCount());

	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	getDevice()->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	getDeviceContext()->OMSetDepthStencilState(depthStencilState, 0); // Bind the depth-stencil state

	//Reset previous raster state
	this->renderer->setWireframeMode(this->renderer->getWireframeState());
	return;
}

void RenderSystem::doPostProcessingPass(TextureShader& passShader, RenderTexture& out, RenderTexture& in)
{
	out.setAsRenderTarget(renderer->getDeviceContext());
	passShader.setResolutionParams(renderer->getDeviceContext(), (float)sWidth, (float)sHeight);
	passShader.setIntrinsicParams(renderer->getDeviceContext());
	renderOnFullScreenOrtho(passShader, in);
}

void RenderSystem::cascadedShadowMapPass(Light& directionalLight)
{
	tempUseShadowMapForPass = false;
	XMMATRIX tempView = camera->getViewMatrix();
	XMMATRIX tempProj = renderer->getProjectionMatrix();
	cascadedShadowMaps->updateAll(renderer->getDeviceContext(), camera, directionalLight,
		[&](XMMATRIX lview, XMMATRIX lortho)
		{
			renderTesselatedTerrain(lview, lortho);
			for (auto renderCollection : this->renderCollections)
			{
				//Custom setup to substitue the view and projection matrices that are the camera's by default

				ShadowMappingLights lightsMatriceData{
				};
				renderCollection.first->setShaderParameters(renderer->getDeviceContext(),
					lview,
					lortho,
					lightsMatriceData,
					lights.data(),
					camera->getPosition(),
					sWidth, sHeight);
				fogParameters.camPos = XMFLOAT4(
					camera->getPosition().x, camera->getPosition().y, camera->getPosition().z,
					1);
				renderCollection.first->setFogParameters(
					renderer->getDeviceContext(), fogParameters);

				renderCollection.second->DefaultRenderAll(renderer->getDeviceContext());

				//renderCollection.second->SetupShaderAndRenderAllItems(renderer->getDeviceContext());
			}
		});
	tempUseShadowMapForPass = true;

	renderer->setBackBufferRenderTarget();
}

void RenderSystem::renderShadowMapToOrthoMesh(ShadowMap* map, OrthoMesh* mesh)
{
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	orthoMesh->sendData(this->renderer->getDeviceContext());
	textureShader->setMatrices(renderer->getDeviceContext(),
		renderer->getWorldMatrix(),
		orthoViewMatrix,
		orthoMatrix);
	textureShader->setTexture(renderer->getDeviceContext(), map->getDepthMapSRV());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
}

void RenderSystem::renderSceneToTexture(RenderTexture& out, const XMMATRIX view, const XMMATRIX projection)
{
	// Set the render target to be the render to texture and clear it
	out.setAsRenderTarget(renderer->getDeviceContext());
	out.clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	renderScene(view, projection);
}

void RenderSystem::renderScene(const XMMATRIX view, const XMMATRIX projection)
{
	//renderer->setBackBufferRenderTarget();
	// Get matrices

	renderSkyboxPass();
	if (defaultShadowShader->ssrParameters.useSSR)
		defaultShadowShader->setSSRColorAndDepthTextures(
			getDeviceContext(),
			colourShaderResourceView,
			depthShaderResourceView
		);
	renderCollections.at(defaultShadowShader.get())->SetupShaderAndRenderAllItems(renderer->getDeviceContext());
	for (auto renderCollection : this->renderCollections)
	{
		renderCollection.second->SetupShaderAndRenderAllItems(renderer->getDeviceContext());
	}

	if (isRenderingWaves)
		renderTesselatedWave(view, projection);
	if (isRenderingTessellatedTerrain)
		renderTesselatedTerrain(view, projection);
	renderTerrain();

	if (isRenderDebug)
		renderDebugInstancesPass();



	getDeviceContext()->RSSetState(_rasterizedStateCullOff.Get());
	DestructableComponentsCollections->SetupShaderAndRenderAllItems(getDeviceContext());
	getDeviceContext()->RSSetState(_rasterizedStateCullOn.Get());
}


void RenderSystem::waterDistortionPass(RenderTexture& out, RenderTexture& in, RenderTexture& renderTexture)
{
	distortionBuffer.time = appTime;
	out.setAsRenderTarget(renderer->getDeviceContext());
	setupFullSreenPostProcessing(*underwaterEffectShader, in);
	underwaterEffectShader->setBlurredTexture(renderer->getDeviceContext(), renderTexture.getShaderResourceView());
	underwaterEffectShader->setDistortionParameters(renderer->getDeviceContext(), distortionBuffer, edgeBlurMask);
	underwaterEffectShader->render(renderer->getDeviceContext(), fullScreenOrthoMesh->getIndexCount());
}



void RenderSystem::renderTesselatedWave(const XMMATRIX view, const XMMATRIX projection)
{
	auto lProj = cascadedShadowMaps->getOrthoMatrix(0);
	auto lView = cascadedShadowMaps->getViewMatrix(0);

	tessellationQuadInstance->getMesh()->sendData(renderer->getDeviceContext(),
		D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	ID3D11ShaderResourceView* texture = nullptr;

	if (!tessellationQuadInstance->getMaterial()->diffuseTexture.empty())
	{
		texture = assetSystem->getTexture(tessellationQuadInstance->getMaterial()->diffuseTexture);
	}

	waveParams[0].time = appTime;
	waveParams[1].time = appTime;
	waveParams[2].time = appTime;

	MultipleWaveBuffer buff;
	buff.waves[0] = waveParams[0];
	buff.waves[1] = waveParams[1];
	buff.waves[2] = waveParams[2];

	tesselateWaveShader->setWaveParams(
		renderer->getDeviceContext(), buff);

	tesselateWaveShader->setWorldPositionAndCamera(renderer->getDeviceContext(),
		tessellationQuadInstance->transform.getTransformMatrix(),
		camera->getPosition(),
		SCREEN_NEAR, SCREEN_DEPTH
	);

	if (tesselateWaveShader->ssrParameters.useSSR)
	{
		tesselateWaveShader->setSSRColorAndDepthTextures(
			getDeviceContext(),
			colourShaderResourceView,
			depthShaderResourceView
		);
	}

	tesselateWaveShader->setTesselationFactors(
		renderer->getDeviceContext(), tesselationFactors);

	ShadowMappingLights lightsMatriceData{
		cascadedShadowMaps->getViewMatrix(0),
		cascadedShadowMaps->getOrthoMatrix(0),
		cascadedShadowMaps->getViewMatrix(1),
		cascadedShadowMaps->getOrthoMatrix(1),
		cascadedShadowMaps->getViewMatrix(2),
		cascadedShadowMaps->getOrthoMatrix(2),
	};

	tesselateWaveShader->setShaderParameters(renderer->getDeviceContext(),
		view, projection,
		lightsMatriceData,
		lights.data(),
		camera->getPosition(),
		sWidth,
		sHeight
	);
	tesselateWaveShader->setShaderParametersForInstance(
		renderer->getDeviceContext(),
		tessellationQuadInstance->transform.getTransformMatrix(),
		tessellationQuadInstance->getMaterial(),
		texture
	);
	tesselateWaveShader->render(renderer->getDeviceContext(), tessellationQuadInstance->getMesh()->getIndexCount());
}



void RenderSystem::setupFullSreenPostProcessing(TextureShader& textureShader, RenderTexture& renderTexture)
{
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix(); // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix(); // Default camera position for orthographic rendering
	textureShader.setMatrices(renderer->getDeviceContext(), renderer->getWorldMatrix(), orthoViewMatrix, orthoMatrix);
	textureShader.setTexture(renderer->getDeviceContext(), renderTexture.getShaderResourceView());
}

void RenderSystem::renderOnFullScreenOrtho(TextureShader& textureShader, RenderTexture& renderTexture)
{
	setupFullSreenPostProcessing(textureShader, renderTexture);
	fullScreenOrthoMesh->sendData(renderer->getDeviceContext());
	textureShader.render(renderer->getDeviceContext(), fullScreenOrthoMesh->getIndexCount());
}

 void RenderSystem::setupBaseShaderParamters(DefaultShader& baseShader)
{
	baseShader.setShaderParamsNew(
		this->renderer->getDeviceContext(),
		this->camera,
		renderer->getProjectionMatrix(),
		cascadedShadowMaps.get(),
		lights.data(),
		fogParameters,
		sWidth,
		sHeight);
}

 void RenderSystem::setupInstanceParameter(DefaultShader& baseShader, MeshInstance* instance)
{
	instance->getMesh()->sendData(renderer->getDeviceContext());
	ID3D11ShaderResourceView* texture = nullptr;

	if (!instance->getMaterial()->diffuseTexture.empty())
	{
		texture = assetSystem->getTexture(instance->getMaterial()->diffuseTexture);
	}

	baseShader.setShaderParametersForInstance(renderer->getDeviceContext(), instance->transform.getTransformMatrix(),
		instance->getMaterial(), texture);
}

 void RenderSystem::initShaders(HWND hwnd)
{
	bloomBlurMask.vInnerRadius = 1;
	bloomBlurMask.vOuterRadius = 0;
	bloomBlurMask.vPower = 2.5;

	ID3D11Device* d = renderer->getDevice();
	skyMapShader = std::make_unique<SkyMapShader>(d, hwnd);
	defaultShadowShader = std::make_unique< DefaultShader>(d, hwnd);
	insideOutsideShaderInstance = std::make_unique< insideOutsideShader>(d, hwnd);
	normalAndDisplacementShader = std::make_unique< NormalMapShader>(d, hwnd);
	gerstnerWaveShader = std::make_unique< WaveShader>(d, hwnd,
		L"GerstenVertexShader.cso");
	tesselateWaveShader = std::make_unique< TesselatedGerstnerWaveShader>(d, hwnd,
		L"TessellationQuadHullShader.cso",
		L"GerstnerWavesDomainShader.cso");

	terrainShader = std::make_unique< TerrainShader>(d, hwnd);
	terrainTesselationShader = std::make_unique< TerrainTesselationShader>(d, getDeviceContext(), hwnd,
		L"QuadFBMHullShader.cso",
		L"QuadFBMDomainShader.cso"
	);

	//--- POST PROCESSING SHADERS
	textureShader = std::make_unique< TextureShader>(d, hwnd);
	underwaterEffectShader = std::make_unique<UnderwaterEffectShader>(d, hwnd);
	magnify = std::make_unique< MagnifyPixelShader>(d, hwnd);
	thresholdPass = std::make_unique<	LuminanceThresholdPass>(d, hwnd);
	bloomCompositePass = std::make_unique< BloomComposite>(d, hwnd);
	horizontalBlur = std::make_unique< Blur>(d, hwnd,L"HorizontalBlurPixelShader.cso");
	verticalBlur = std::make_unique< Blur>(d, hwnd,L"VerticalBlurPixelShader.cso");

	////--- COMPUTE SHADERS
	//buoyancyComputeShader = new BuoyancyComputeShader(d, hwnd, 15, 15);

}

  void RenderSystem::initTextures(int screenWidth, int screenHeight)
 {
	 sceneRT = std::make_unique< RenderTexture>(getDevice(), sWidth, sHeight, 1, 100);
	 effectRT = std::make_unique< RenderTexture>(getDevice(), sWidth, sHeight, 1, 100);
	 bloomTextureOne = std::make_unique< RenderTexture>(getDevice(), sWidth, sHeight, 1, 100);
	 bloomTextureTwo = std::make_unique< RenderTexture>(getDevice(), sWidth, sHeight, 1, 100);
	 passChain = new PostProcessPassChain(renderer->getDevice(), sWidth, sHeight, 0.1, 200);
 }

   void RenderSystem::initRasterStates()
  {
	  CD3D11_RASTERIZER_DESC rasterDesc;
	  rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	  rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	  rasterDesc.ScissorEnable = false;
	  rasterDesc.MultisampleEnable = false;
	  rasterDesc.AntialiasedLineEnable = false;
	  rasterDesc.DepthBias = 0.0f;
	  rasterDesc.DepthBiasClamp = 0.0f;
	  renderer->getDevice()->CreateRasterizerState(&rasterDesc, _debugRasterState.GetAddressOf());

	  // (1) Default state: Culling enabled (BACK faces culled)
	  D3D11_RASTERIZER_DESC descCullOn = {};
	  descCullOn.FillMode = D3D11_FILL_SOLID;
	  descCullOn.CullMode = D3D11_CULL_BACK;  // Cull back faces
	  descCullOn.FrontCounterClockwise = TRUE; // DX11 defaults to counter-clockwise winding
	  getDevice()->CreateRasterizerState(&descCullOn, _rasterizedStateCullOn.GetAddressOf());

	  // (2) Culling disabled (draw all triangles)
	  D3D11_RASTERIZER_DESC descCullOff = {};
	  descCullOff.FillMode = D3D11_FILL_SOLID;
	  descCullOff.CullMode = D3D11_CULL_NONE;  // Disable culling
	  descCullOff.FrontCounterClockwise = TRUE;
	  getDevice()->CreateRasterizerState(&descCullOff, _rasterizedStateCullOff.GetAddressOf());
  }

   void RenderSystem::initRenderCollections()
  {
	  //Setting up render collection. Those are collections that have somethings different for their setup,
	  // which in all the cases is additional parameters that need to be passed.
	  //Render collection have a shader, a setup funciton and draw all function.
	  DestructableComponentsCollections = new LimitedTimeRenderCollection(
		  insideOutsideShaderInstance.get(),
		  [&](DefaultShader* sh) {
			  setupBaseShaderParamters(*sh);
		  },
		  [&](DefaultShader* ls, MeshInstance* instnace)
		  {
			  setupInstanceParameter(*ls, instnace);
			  insideOutsideShaderInstance->setInstanceOnTheInside(getDeviceContext());
		  });

	  auto normalMapRenderCollection = new RenderItemCollection(normalAndDisplacementShader.get(),
		  [&](DefaultShader* sh) { setupBaseShaderParamters(*sh); },
		  [&](DefaultShader* ls, MeshInstance* instnace)
		  {
			  setupInstanceParameter(*ls, instnace);
			  normalAndDisplacementShader->setNormalMap(
				  renderer->getDeviceContext(),
				  assetSystem->getTexture(
					  instnace->getMaterial()->normalTexture));
			  float displacement = displacementValue;
			  if (instnace->getMaterial()->displacementTexture.empty())
				  displacement = 0;
			  normalAndDisplacementShader->setDiscplacementMap(
				  renderer->getDeviceContext(),
				  assetSystem->getTexture(
					  instnace->getMaterial()->displacementTexture),
				  displacement);
			  //0.00);
		  });

	  auto gertsnerWaveRenderCollection = new RenderItemCollection(gerstnerWaveShader.get(),
		  [&](DefaultShader* sh)
		  {
			  setupBaseShaderParamters(*sh);
		  },
		  [&](DefaultShader* ls, MeshInstance* instnace)
		  {
			  setupInstanceParameter(*ls, instnace);
			  waveParams[0].time = appTime;
			  waveParams[1].time = appTime;
			  waveParams[2].time = appTime;
			  MultipleWaveBuffer buff;
			  buff.waves[0] = waveParams[0];
			  buff.waves[1] = waveParams[1];
			  buff.waves[2] = waveParams[2];
			  gerstnerWaveShader->setWaveParams(
				  renderer->getDeviceContext(), buff);
		  });

	  auto defaultRenderCollection = new RenderItemCollection(defaultShadowShader.get(),
		  [&](DefaultShader* sh) { setupBaseShaderParamters(*sh); },
		  [&](DefaultShader* ls, MeshInstance* instnace)
		  {
			  setupInstanceParameter(*ls, instnace);
		  });

	  this->renderCollections.insert({ defaultShadowShader.get(), defaultRenderCollection});
	  this->renderCollections.insert({ normalMapRenderCollection->shaderToRenderWith, normalMapRenderCollection });
	  this->renderCollections.insert({ DestructableComponentsCollections->shaderToRenderWith, DestructableComponentsCollections });
	  this->renderCollections.insert({ gertsnerWaveRenderCollection->shaderToRenderWith, gertsnerWaveRenderCollection });
  }

   void RenderSystem::initCascadedShadowMaps()
  {
	  cascadedShadowMaps = std::make_unique<CascadedShadowMaps>();
	  cascadedShadowMaps->calculateSubFrustrums(renderer->getDevice(), camera, SCREEN_NEAR, SCREEN_NEAR + SCREEN_DEPTH,
		  (float)XM_PI / 4.0f, sWidth, sHeight);
  }

    void RenderSystem::setAppTime(float time)
   {
	   this->appTime = time;
   }

    void RenderSystem::renderDebugInstancesPass()
   {
	   for each (auto && instance in this->debugItems)
	   {
		   setupBaseShaderParamters(*defaultShadowShader);
		   setupInstanceParameter(*defaultShadowShader, instance.get());
		   instance->getMesh()->sendData(getDeviceContext());
		   defaultShadowShader->render(getDeviceContext(), instance->getMesh()->getVertexCount());
	   }
   }

    void RenderSystem::setTesselatedTerrainInstance(MeshInstance* terrainInstance)
   {
	   this->tessellationQuadInstance = terrainInstance;
   }

    void RenderSystem::setDestructableTerrain(DestructibleTerrainPeaks* destructableTerrain)
   {
	   this->destructableTerrainPeaks = destructableTerrain;
   }

    void RenderSystem::addDebugInstance(std::unique_ptr<MeshInstance> meshInstance)
   {
	   this->debugItems.push_back(std::move(meshInstance));
   }

    RenderSystem::RenderSystem(D3D* renderer, HWND hwnd, FPCamera* camera, AssetSystem* assetSystem, int height, int width)
   {
	   //this->renderer = std::make_unique<D3D>{ renderer; };
	   this->renderer = renderer;
	   this->sWidth = width;
	   this->sHeight = height;
	   this->camera = camera;
	   this->assetSystem = assetSystem;
	   initTextures(width, height);
	   initShaders(hwnd);
	   initRasterStates();
	   initRenderCollections();
	   initCascadedShadowMaps();


	   fogParameters.fogDensity = 0.1f;
	   fogParameters.fogColor = XMFLOAT4(1, 0, 0, 1);
	   fogParameters.fogEnd = 200.0f;
	   fogParameters.padding = 0;

	   magnifyMask.vInnerRadius = 0.05;
	   magnifyMask.vOuterRadius = 0.150;

	   edgeBlurMask.vInnerRadius = 0.3;
	   edgeBlurMask.vOuterRadius = 0.6;


	   waveParams[0].steepness = 0.2f;
	   waveParams[0].wavelength = 64;
	   waveParams[0].speed = 0.5f;
	   waveParams[0].XZdir[0] = 1.0f;
	   waveParams[0].XZdir[1] = 0.0f;

	   waveParams[1].steepness = 0.180f;
	   waveParams[1].wavelength = 31;
	   waveParams[1].speed = 0.8f;
	   waveParams[1].XZdir[0] = 1.0f;
	   waveParams[1].XZdir[1] = -0.3f;

	   waveParams[2].steepness = 0.240f;
	   waveParams[2].wavelength = 16;
	   waveParams[2].speed = 0.9f;
	   waveParams[2].XZdir[0] = 1.0f;
	   waveParams[2].XZdir[1] = 0.7f;


	   this->tesselationFactors.edgeTesselationFactor[0] = 4;
	   this->tesselationFactors.edgeTesselationFactor[1] = 4;
	   this->tesselationFactors.edgeTesselationFactor[2] = 4;
	   this->tesselationFactors.edgeTesselationFactor[3] = 4;

	   this->tesselationFactors.insideTesselationFactor[0] = 4;
	   this->tesselationFactors.insideTesselationFactor[1] = 4;


	   distortionBuffer.offsetX = 0.020;
	   distortionBuffer.offsetY = 0.020;

	   distortionBuffer.sinXFrequency = 20;
	   distortionBuffer.sinYFrequency = 20;

	   distortionBuffer.colorOverlay = XMFLOAT3(15.0f / 255.0f, 50.0f / 255.0f, 110.0f / 255.0f);

	   displacementValue = 0.05f;

	   float len = 200;

	   orthoMesh = std::make_unique< OrthoMesh>(renderer->getDevice(), renderer->getDeviceContext(), len, len, -2.25 * len,
		   sHeight * 0.5 - len * 0.5);
	   fullScreenOrthoMesh = std::make_unique< OrthoMesh>(renderer->getDevice(), renderer->getDeviceContext(),
		   sWidth, sHeight,
		   0, 0);
   }

    void RenderSystem::fillRenderCollections(MeshInstance* instance)
   {
	   if (instance->getMaterial() != nullptr && instance->getMaterial()->name == "Wave")
	   {
		   if (instance->getSerializableMesh()._type == SerializableMeshType::Plane ||
			   instance->getSerializableMesh()._type == SerializableMeshType::TesselationQuad)
		   {
			   this->renderCollections.at(gerstnerWaveShader.get())->push_back(instance);
			   return;
		   }
	   }


	   if (instance->getMaterial() == nullptr)
	   {
		   return;
	   }

	   if (!instance->getMaterial()->normalTexture.empty())
	   {
		   this->renderCollections.at(normalAndDisplacementShader.get())->push_back(instance);
		   return;
	   }
	   this->renderCollections.at(defaultShadowShader.get())->push_back(instance);
   }

    void RenderSystem::autoInsertInRenderCollection(MeshInstance* instance)
   {
	   if (instance->getMaterial() != nullptr && instance->getMaterial()->name == "Wave")
	   {
		   if (instance->getSerializableMesh()._type == SerializableMeshType::Plane ||
			   instance->getSerializableMesh()._type == SerializableMeshType::TesselationQuad)
		   {
			   this->renderCollections.at(gerstnerWaveShader.get())->push_back(instance);

			   return;
		   }
	   }

	   //if (instance->getSerializableMesh()._type == SerializableMeshType::TesselationQuad)
	   //{
	   //	this->tessellationQuadInstance = instance;
	   //}

	   if (instance->getMaterial() == nullptr)
	   {
		   return;
	   }

	   if (!instance->getMaterial()->normalTexture.empty())
	   {
		   this->renderCollections.at(normalAndDisplacementShader.get())->push_back(instance);
		   return;
	   }
	   this->renderCollections.at(defaultShadowShader.get())->push_back(instance);
   }

    bool RenderSystem::render()
   {
	   // Generate the view matrix based on the camera's position.
	   camera->update();


	   renderer->setBackBufferRenderTarget();
	   passChain->Reset();

	   XMMATRIX viewMatrix;
	   XMMATRIX projectionMatrix;
	   //	if (input->isKeyDown('V'))
	   //	{
	   //		projectionMatrix = cascadedShadowMaps->getOrthoMatrix(0);
	   //		viewMatrix = cascadedShadowMaps->getViewMatrix(0);
	   //	}
	   //	else if (input->isKeyDown('B'))
	   //	{
	   //		projectionMatrix = cascadedShadowMaps->getOrthoMatrix(1);
	   //		viewMatrix = cascadedShadowMaps->getViewMatrix(1);
	   //	}
	   //	else if (input->isKeyDown('N'))
	   //	{
	   //		projectionMatrix = cascadedShadowMaps->getOrthoMatrix(2);
	   //		viewMatrix = cascadedShadowMaps->getViewMatrix(2);
	   //	}
	   //	else
	   //	{
	   //		viewMatrix = camera->getViewMatrix();
	   //		projectionMatrix = renderer->getProjectionMatrix();
	   //	}
	   viewMatrix = camera->getViewMatrix();
	   projectionMatrix = renderer->getProjectionMatrix();

	   defaultShadowShader->ssrParameters.useSSR = false;
	   tesselateWaveShader->ssrParameters.useSSR = false;
	   //	//	//do an object render pass for color and depth buffer
	   renderSceneToTexture(*passChain->Out, viewMatrix, projectionMatrix);
	   colourShaderResourceView = passChain->Out->getShaderResourceView();
	   depthShaderResourceView = passChain->Out->getDepthShaderResourceView();
	   passChain->Swap();
	   defaultShadowShader->ssrParameters.useSSR = true;
	   tesselateWaveShader->ssrParameters.useSSR = true;
	   //renderSkyboxPass();
	   //  //Do cascaded shadow map pass for the direction light in the scene
	   cascadedShadowMapPass(*lights[2]);
	   //renderer->resetViewport();
	   renderer->resetViewport();

	   if (isRenderSceneToTexture)
		   renderSceneToTexture(*passChain->Out, viewMatrix, projectionMatrix);
	   else
		   renderScene(viewMatrix, projectionMatrix);

	   if (isRenderSceneToTexture)
	   {
		   this->renderer->setZBuffer(false);
		   if (isPostProcessing)
		   {
			   if (enableBloom)
			   {
				   passChain->Swap();

				   thresholdPass->thresholdData.threshold = 1;
				   doPostProcessingPass(*thresholdPass, *bloomTextureOne, *passChain->In);
				   //thresholdPass(bloomTextureOne, passChain->In);
				   //horizontalBlurPass(bloomTextureTwo, bloomTextureOne, bloomBlurMask);

				   horizontalBlur->viggnete = bloomBlurMask;
				   doPostProcessingPass(*horizontalBlur, *bloomTextureTwo, *bloomTextureOne);

				   verticalBlur->viggnete = bloomBlurMask;
				   doPostProcessingPass(*verticalBlur, *bloomTextureOne, *bloomTextureTwo);
				   //verticalBlurPass(bloomTextureOne, bloomTextureTwo, bloomBlurMask);
				   //$bloomCompositePass(passChain->Out, passChain->In, bloomTextureOne, 5, 5);
				   bloomCompositePass->bloomData.bloomIntensity = 5;
				   bloomCompositePass->bloomData.exposure = 5;
				   bloomCompositePass->extractedTexture = bloomTextureOne->getShaderResourceView();
				   doPostProcessingPass(*bloomCompositePass, *passChain->Out, *passChain->In);
			   }

			   if (enableBlur)
			   {
				   passChain->Swap();
				   horizontalBlur->viggnete = edgeBlurMask;
				   doPostProcessingPass(*horizontalBlur, *passChain->Out, *passChain->In);

				   passChain->Swap();
				   verticalBlur->viggnete = edgeBlurMask;
				   doPostProcessingPass(*verticalBlur, *passChain->Out, *passChain->In);
			   }

			   if (enableWaterDistortion)
			   {
				   passChain->Swap();

				   distortionBuffer.time = appTime;
				   underwaterEffectShader->setBlurredTexture(renderer->getDeviceContext(), passChain->In->getShaderResourceView());
				   underwaterEffectShader->setDistortionParameters(renderer->getDeviceContext(), distortionBuffer, edgeBlurMask);
				   doPostProcessingPass(*underwaterEffectShader, *passChain->Out, *passChain->In);
			   }

			   if (enableMagnify)
			   {
				   passChain->Swap();
				   //magnifyPass(passChain->Out, passChain->In);
				   magnify->vignette = magnifyMask;
				   doPostProcessingPass(*magnify, *passChain->Out, *passChain->In);
			   }
		   }
		   this->renderer->setZBuffer(true);
		   renderer->setBackBufferRenderTarget();
		   renderDefaultTexture(*passChain->Out);
	   }

	   //Debug items are consumed on frame
	   this->debugItems.clear();



	   return true;
   }

    void RenderSystem::renderDefaultTexture(RenderTexture& out)
   {
	   renderer->setZBuffer(false);
	   setupFullSreenPostProcessing(*textureShader, out);
	   fullScreenOrthoMesh->sendData(renderer->getDeviceContext());
	   textureShader->render(renderer->getDeviceContext(), fullScreenOrthoMesh->getIndexCount());
	   renderer->setZBuffer(true);
   }

    TerrainTesselationShader* RenderSystem::getTerrainShader() const
   {
	   return terrainTesselationShader.get();

   }

    FPCamera* RenderSystem::getCamera() const
   {
	   return camera;

   }

    RenderSystem::~RenderSystem()
   {
	   for (auto rc : this->renderCollections)
	   {
		   rc.second->clear();
		   delete rc.second;
	   }
	   this->renderCollections.clear();

   }

   void RenderSystem::gui()
{
	fogParameterGuiWindow();
	waveParametersGuiWindow();
	postProcessingGuiWindow();
	screenSpaceReflectionGuiWindow();
	destructableComponentEffectGuiWindow();
}


void RenderSystem::destructableComponentEffectGuiWindow()
{
	ImGui::Begin("Destructable Components");

	ImGui::SliderFloat("Rainbow Power", &insideOutsideShaderInstance->rainbowColors.power, 1, 25);
	ImGui::InputFloat("Normal Amplitude", &insideOutsideShaderInstance->rainbowColors.noiseAmplitude, 1, 25);
	ImGui::InputFloat("Norma Frequency", &insideOutsideShaderInstance->rainbowColors.noiseFrequency, 50, 1500);
	ImGui::InputFloat("Peturbation", &insideOutsideShaderInstance->rainbowColors.normalStrength, 0, 10);
	ImGui::End();
}

void RenderSystem::fogParameterGuiWindow()
{
	ImGui::Begin("Fog Parameters");
	//ImGui::DragFloat3("", waveParams[i].XZdir, 0.1f, -1.0f, 1.0f);
	ImGui::ColorEdit4("Fog Color", &fogParameters.fogColor.x);
	ImGui::DragFloat("Fog End", &fogParameters.fogEnd);
	ImGui::DragFloat("Fog Density", &fogParameters.fogDensity);

	ImGui::End();
}

void RenderSystem::waveParametersGuiWindow()
{
	ImGui::Begin("Wave Parameters");
	ImGui::Checkbox("Is Rendering Waves", &isRenderingWaves);
	for (size_t i = 0; i < 3; i++)
	{
		std::string waveNum = "Wave" + std::to_string(i) + "Properties";
		ImGui::Text(waveNum.c_str());
		ImGui::DragFloat(("Wave Steepness" + std::to_string(i)).c_str(), &waveParams[i].steepness, 0.01, 0.0f, 1.0f);
		ImGui::DragFloat(("Wavelength" + std::to_string(i)).c_str(), &waveParams[i].wavelength, 1.0f, 0.01, 100.0f);
		ImGui::DragFloat(("Wave Speed" + std::to_string(i)).c_str(), &waveParams[i].speed, 0.01f, 0, 5);
		ImGui::DragFloat2(("Wave Dir XZ" + std::to_string(i)).c_str(), waveParams[i].XZdir, 0.01f, 0, 5);
	}
	ImGui::End();
}

void RenderSystem::postProcessingGuiWindow()
{
	ImGui::Begin("Post Processing Effects");

	ImGui::Checkbox("Render Scene To Texture", &this->isRenderSceneToTexture);

	ImGui::Checkbox("Enable Post Processing", &this->isPostProcessing);

	ImGui::BeginGroup();
	ImGui::Checkbox("Enable Bloom", &enableBloom);
	ImGui::DragFloat("Bloom Blur IR", &bloomBlurMask.vInnerRadius, 0.01, 0.025, 0.9);
	//float reaminder = (1 - edgeBlurMask.vInnerRadius) - 0.2;
	ImGui::DragFloat("Bloom Blur OR", &bloomBlurMask.vOuterRadius, 0.01, 0, 1);
	ImGui::DragFloat("Bloom Blur Power", &bloomBlurMask.vPower, 0.5, 1, 15);
	ImGui::EndGroup();

	ImGui::BeginGroup();
	ImGui::BeginGroup();
	ImGui::Checkbox("Enable Blur", &enableBlur);
	ImGui::DragFloat("Blur Inner Radius", &edgeBlurMask.vInnerRadius, 0.01, 0.025, 0.9);
	//float reaminder = (1 - edgeBlurMask.vInnerRadius) - 0.2;
	ImGui::DragFloat("Blur Outer Radius ", &edgeBlurMask.vOuterRadius, 0.01, 0, 1);
	ImGui::EndGroup();

	ImGui::BeginGroup();
	ImGui::Checkbox("Enable Magnify", &enableMagnify);
	ImGui::DragFloat("Magnify Inner Radius", &magnifyMask.vInnerRadius, 0.05, 0.05, 1.0);
	ImGui::DragFloat("Magnify Outer Radius ", &magnifyMask.vOuterRadius, 0.05, magnifyMask.vInnerRadius, 1.0);
	ImGui::EndGroup();

	ImGui::Checkbox("Enable Water Dsitortion", &enableWaterDistortion);

	ImGui::EndGroup();

	ImGui::Text("Underwatter PostProcessing Parameters");
	auto color = new float[3];
	color[0] = distortionBuffer.colorOverlay.x;
	color[1] = distortionBuffer.colorOverlay.y;
	color[2] = distortionBuffer.colorOverlay.z;

	ImGui::ColorEdit3("Underwater Color", color);
	//ImGui::ColorPicker3("Underwater Color",color);

	distortionBuffer.colorOverlay.x = color[0];
	distortionBuffer.colorOverlay.y = color[1];
	distortionBuffer.colorOverlay.z = color[2];

	ImGui::DragFloat("OffsetX", &distortionBuffer.offsetX, 0.0001f, 0, 1);
	ImGui::DragFloat("OffsetY", &distortionBuffer.offsetY, 0.0001f, 0, 1);
	ImGui::DragFloat("SinXFrequency", &distortionBuffer.sinXFrequency);
	ImGui::DragFloat("SinYFrequency", &distortionBuffer.sinYFrequency);

	//ImGui::DragFloat("Power", &edgeBlurMask.vPower, 0.05, 0.05, 1.0);

	ImGui::End();
}


void RenderSystem::screenSpaceReflectionGuiWindow()
{
	ImGui::Begin("Screen Space Reflections");
	ImGui::Checkbox("Use Reflections", &tesselateWaveShader->ssrParameters.useSSR);

	auto waterMar = assetSystem->getMaterial("Wave");
	ImGui::DragFloat("Water Reflection Factor", &waterMar->reflectionFactor, 0.0001, 0.0001, 1);
	ImGui::DragFloat("Reflection Ray Length", &tesselateWaveShader->ssrParameters.ssrWorldLength, 0.1f, 0.001f, 500);
	ImGui::DragInt("Steps", &tesselateWaveShader->ssrParameters.ssrMaxSteps, 1, 0, 3000);
	ImGui::DragFloat("Resolution", &tesselateWaveShader->ssrParameters.resolution, 0.0001, 0.0001, 1);
	ImGui::DragFloat("Thickness In Units", &tesselateWaveShader->ssrParameters.thickness, 0.1f, 0.01f, 150.0f);

	ImGui::End();
}

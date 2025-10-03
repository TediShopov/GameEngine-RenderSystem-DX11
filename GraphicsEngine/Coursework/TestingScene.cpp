#include "TestingScene.h"

#include "MeshInstance.h"
#include "SkyMapShader.h"

void TestingScene::initMeshes()
{
	//Create the skybox mesh that could be retrieved by its name
	SerializableMesh skyboxTemp = SerializableMesh::ShapeMesh("SkyBox", SerializableMeshType::Cube, 100);
	skyboxTemp.CreateMesh(this->getDevice(), this->getDeviceContext());
	meshes.insert({ skyboxTemp.name,skyboxTemp });
	
	SerializableMesh sphere = SerializableMesh::ShapeMesh("Sphere", SerializableMeshType::Sphere, 100);
	sphere.CreateMesh(this->getDevice(), this->getDeviceContext());
	meshes.insert({sphere.name, sphere});
}

void TestingScene::initShaders(HWND hwnd)
{
	//Initialize the testing shader
	skyMapShader = new SkyMapShader(getDevice(), hwnd);

}

void TestingScene::initCameras()
{
	//Initialize camera to be slightly above the water plane for convenience
	camera->setPosition(0, 20, 0);
}

TestingScene::TestingScene()
{
	waveParams[0].steepness = 0.2;
	waveParams[0].wavelength = 64;
	waveParams[0].speed = 2.0f;
	waveParams[0].XZdir[0] = 1.0f;
	waveParams[0].XZdir[1] = 0.0f;

	waveParams[1].steepness = 0.15;
	waveParams[1].wavelength = 31;
	waveParams[1].speed = 4.0f;
	waveParams[1].XZdir[0] = 1.0f;
	waveParams[1].XZdir[1] = -0.3f;

	waveParams[2].steepness = 0.05;
	waveParams[2].wavelength = 16;
	waveParams[2].speed = 8.0f;
	waveParams[2].XZdir[0] = 1.0f;
	waveParams[2].XZdir[1] = 0.7f;

	this->tesselationFactors.edgeTesselationFactor[0] = 4;
	this->tesselationFactors.edgeTesselationFactor[1] = 4;
	this->tesselationFactors.edgeTesselationFactor[2] = 4;
	this->tesselationFactors.edgeTesselationFactor[3] = 4;

	this->tesselationFactors.insideTesselationFactor[0] = 4;
	this->tesselationFactors.insideTesselationFactor[1] = 4;

}

TestingScene::~TestingScene()
{
}

void TestingScene::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC,
                        bool FULL_SCREEN)
{
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);
	input = in;
	initCameras();
	initMeshes();
	initShaders(hwnd);


	//Debug raster state
		CD3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterDesc.ScissorEnable = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.DepthClipEnable = false;
		rasterDesc.DepthBias = 0.0f;
		rasterDesc.DepthBiasClamp = 0.0f;
		renderer->getDevice()->CreateRasterizerState(&rasterDesc, &_debugRasterState);
}

bool TestingScene::frame()
{

	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	if (input->isKeyDown('G'))
	{
		this->renderer->setWireframeMode(!this->renderer->getWireframeState());
	}

//	activeInstanceSelectorUI->updateStateOfUI(this);
	appTime += timer->getTime();
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
	
}

void TestingScene::renderTesselatedWave(XMMATRIX view, XMMATRIX projection)
{
	return;
}

bool TestingScene::render()
{
	// Generate the view matrix based on the camera's position.
	camera->update();

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	renderer->resetViewport();
	renderer->setBackBufferRenderTarget();


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
	renderer->getDeviceContext()->RSSetState(_debugRasterState);


	XMMATRIX	viewMatrix = camera->getViewMatrix();
	XMMATRIX	projectionMatrix = renderer->getProjectionMatrix();

	auto ms = new MeshInstance(assetSystem->getMesh("SkyBox"));
	float scale = 1;
	ms->transform.setPosition(camera->getPosition().x,camera->getPosition().y,camera->getPosition().z);
	ms->transform.setScale(scale, scale, scale);

	ms->getMesh()->sendData(renderer->getDeviceContext());
	skyMapShader->setShaderParameters(renderer->getDeviceContext(), ms->transform.getTransformMatrix(), viewMatrix, projectionMatrix);
	skyMapShader->render(getDeviceContext(), ms->getMesh()->getIndexCount());
	renderer->setZBuffer(true);

	//Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();

	return true;

}

void TestingScene::gui()
{

	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::Checkbox("Render Scene To Texture", &this->renderSceneToTexute);

	ImGui::Text("Press E to raise camera \nto see the plane being rendered");
	ImGui::Checkbox("Demo Window", &demoWindow);
	if (demoWindow)
	{
		ImGui::ShowDemoWindow();
	}
	
	
	/*ImGui::DragFloat3("Light One Position", lightOnePos, 0.1f, -100, 100);
	ImGui::DragFloat3("Light Two Position", lightTwoPos, 0.1f, -100, 100);
	ImGui::DragFloat3("Light Three Direciton", lightDir, 0.1f, -100, 100);*/

	//lightEditor.appendToImgui();
	//lightEditor.applyChangesTo(this->lights);
	

//	transformEditor.appendToImgui();
//	transformEditor.applyChangesTo(&this->activeMeshInstance->transform);


//	ImGui::Begin("Mesh Instance Tree");
//	activeInstanceSelectorUI->appendToImgui();
//	activeInstanceSelectorUI->applyChangesTo(this);
//	if (activeInstanceSelectorUI->getRawData().isNew)
//	{
//		this->transformEditor.updateStateOfUI(&this->activeMeshInstance->transform);
//	}
//	ImGui::End();




	for (size_t i = 0; i < 3; i++)
	{
		std::string waveNum = "Wave" + std::to_string(i) + "Properties";
		ImGui::Text(waveNum.c_str());
		ImGui::DragFloat(("Wave Steepness" + std::to_string(i)).c_str(), &waveParams[i].steepness, 0.01, 0.0f, 1.0f);
		ImGui::DragFloat(("Wavelength" + std::to_string(i)).c_str(), &waveParams[i].wavelength, 1.0f, 0.0f, 100.0f);
		ImGui::DragFloat(("Wave Speed" + std::to_string(i)).c_str(), &waveParams[i].speed, 1.0f, -100.0f, 100.0f);
		ImGui::DragFloat2(("Wave Dir XZ" + std::to_string(i)).c_str(), waveParams[i].XZdir, 0.1f, -1.0f, 1.0f);
	}

	







	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

#include "AssetSystem.h"


void AssetSystem::initMaterials()
{
	const float baseMatCOLOR = 0.5f;
	auto baseMat = new Material();
	baseMat->name = "Base";
	baseMat->ambient = XMFLOAT3(baseMatCOLOR * 0.3, baseMatCOLOR * 0.3, baseMatCOLOR * 0.3);
	baseMat->diffuse = XMFLOAT3(baseMatCOLOR, baseMatCOLOR, baseMatCOLOR);
	baseMat->specular = XMFLOAT3(0.9f, 0.9f, 0.9f);
	baseMat->shininess = 16;
	baseMat->diffuseTexture = L"diffuseBrick";
	this->materials.insert({ baseMat->name, baseMat });

	const float R = 29.0f / 255.0f;
	const float G = 50.0f / 255.0f;
	const float B = 170.0f / 255.0f;

	auto wave = new Material();
	wave->name = "Wave";
	wave->ambient = XMFLOAT3(0.1, 0.1, 0.1);
	wave->diffuse = XMFLOAT3(0.1, 0.1, 0.8);
	wave->specular = XMFLOAT3(1, 1, 1);
	wave->reflectionFactor = 0.015;
	wave->diffuseTexture = L"default";

	// --- DEBUG MATERIALS WITH SIMPEL COLORS --
	XMFLOAT3 debugAmbient = XMFLOAT3(0.1, 0.1, 0.1);
	XMFLOAT3 debugSpecular = XMFLOAT3(1, 1, 1);

	auto blue = new Material();
	blue->name = "Blue";
	blue->ambient = debugAmbient;
	blue->specular = debugSpecular;
	blue->diffuse = XMFLOAT3(0, 0, 1);
	blue->diffuseTexture = L"default";
	blue->normalTexture = L"shipNormal";

	auto green = blue->Copy();
	green->name = "Green";
	green->diffuse = XMFLOAT3(0, 1, 0);
	green->emissive = XMFLOAT3(0, 1, 5);

	auto red = blue->Copy();
	red->name = "Red";
	red->diffuse = XMFLOAT3(1, 0, 0);
	red->emissive = XMFLOAT3(1, 0, 5);

	auto cyan = blue->Copy();
	cyan->name = "Cyan";
	cyan->diffuse = XMFLOAT3(0, 1, 1);
	cyan->emissive = XMFLOAT3(0, 1, 5);

	auto magenta = blue->Copy();
	magenta->name = "Magenta";
	magenta->diffuse = XMFLOAT3(1, 0, 1);

	auto yellow = blue->Copy();
	yellow->name = "Yellow";
	yellow->diffuse = XMFLOAT3(1, 1, 0);

	materials.insert({ blue->name, blue });
	materials.insert({ green->name, green });
	materials.insert({ red->name, red });
	materials.insert({ cyan->name, cyan });
	materials.insert({ magenta->name, magenta });
	materials.insert({ yellow->name, yellow });
	wave->shininess = 16;
	this->materials.insert({ wave->name, wave });

	{
		auto normalMapMaterial = new Material();
		normalMapMaterial->name = "NormalMaterial";
		normalMapMaterial->ambient = XMFLOAT3(baseMatCOLOR * 0.3, baseMatCOLOR * 0.3, baseMatCOLOR * 0.3);
		normalMapMaterial->diffuse = XMFLOAT3(baseMatCOLOR, baseMatCOLOR, baseMatCOLOR);
		normalMapMaterial->specular = XMFLOAT3(0.9f, 0.9f, 0.9f);
		//normalMapMaterial->texturepath = L"res/brickwall.jpg";
		normalMapMaterial->shininess = 16;
		normalMapMaterial->diffuseTexture = L"diffuseBrick";
		normalMapMaterial->normalTexture = L"normalBrick";
		this->materials.insert({ normalMapMaterial->name, normalMapMaterial });
	}

	{
		auto shipMaterial = new Material();
		shipMaterial->name = "ShipMaterial";
		shipMaterial->ambient = XMFLOAT3(0, 0, 0);
		shipMaterial->diffuse = XMFLOAT3(baseMatCOLOR, baseMatCOLOR, baseMatCOLOR);
		shipMaterial->specular = XMFLOAT3(0.9f, 0.9f, 0.9f);
		//normalMapMaterial->texturepath = L"res/brickwall.jpg";
		shipMaterial->shininess = 16;
		shipMaterial->diffuseTexture = L"shipDiffuse";
		//shipMaterial->diffuseTexture = L"default";
		shipMaterial->normalTexture = L"shipNormal";
		this->materials.insert({ shipMaterial->name, shipMaterial });
	}

	{
		auto displacementMapMaterial = new Material(*baseMat);
		displacementMapMaterial->name = "DisplacementMaterial";
		displacementMapMaterial->diffuseTexture = L"diffuseBrick";
		displacementMapMaterial->displacementTexture = L"displacementMap";
		this->materials.insert({ displacementMapMaterial->name, displacementMapMaterial });
	}

	{
		auto rockMaterial = new Material(*baseMat);
		rockMaterial->name = "Rock";
		rockMaterial->diffuseTexture = L"rockDiffuse";
		rockMaterial->normalTexture = L"rockNormal";

		rockMaterial->displacementTexture = L"rockDisplacement";
		this->materials.insert({ rockMaterial->name, rockMaterial });
	}

	{
		auto gravelMaterial = new Material(*baseMat);
		gravelMaterial->name = "Gravel";
		gravelMaterial->diffuseTexture = L"gravelDiffuse";
		gravelMaterial->normalTexture = L"gravelNormal";

		gravelMaterial->displacementTexture = L"gravelDisplacement";
		this->materials.insert({ gravelMaterial->name, gravelMaterial });
	}
}

void AssetSystem::initRenderCollections()
{
}

void AssetSystem::initMeshes()
{
	{
		SerializableMesh skyboxTemp = SerializableMesh::ShapeMesh("SkyBox", SerializableMeshType::Cube, 100);
		skyboxTemp.CreateMesh(&device,&deviceContext);
		meshes.insert({ skyboxTemp.name, skyboxTemp });
	}

	{
		SerializableMesh plane = SerializableMesh::ShapeMesh("Plane", SerializableMeshType::Plane, 100);
		plane.CreateMesh(&device,&deviceContext);
		meshes.insert({ plane.name, plane });
	}

	{
		SerializableMesh debugPlane = SerializableMesh::ShapeMesh("DebugPlane", SerializableMeshType::Plane, 10);
		debugPlane.CreateMesh(&device,&deviceContext);
		meshes.insert({ debugPlane.name, debugPlane });
	}

	{
		std::string cottageModelStr("res/models/cottage_fbx.fbx");
		SerializableMesh cottage = SerializableMesh::CustomMesh("Cottage", cottageModelStr);
		cottage.CreateMesh(&device,&deviceContext);
		meshes.insert({ cottage.name, cottage });
	}

	{

		std::string shipModelStr("res/models/02_barkas.FBX");

		SerializableMesh shipModel = SerializableMesh::CustomMesh("Ship", shipModelStr);
		shipModel.generateTangentMesh = true;
		shipModel.CreateMesh(&device,&deviceContext);
		meshes.insert({ shipModel.name, shipModel });
	}
	{
		SerializableMesh sphere = SerializableMesh::ShapeMesh("Sphere", SerializableMeshType::Sphere, 100);
		sphere.CreateMesh(&device,&deviceContext);
		meshes.insert({ sphere.name, sphere });
	}


	{
		SerializableMesh debugSphere = SerializableMesh::ShapeMesh("SphereDebug",
			SerializableMeshType::Sphere, 5);
		debugSphere.CreateMesh(&device,&deviceContext);
		meshes.insert({ debugSphere.name, debugSphere });
	}


	{
		SerializableMesh cube = SerializableMesh::ShapeMesh("Cube", SerializableMeshType::Cube, 24);
		cube.CreateMesh(&device,&deviceContext);
		meshes.insert({ cube.name, cube });
	}


	{
		SerializableMesh tangentSphere = SerializableMesh::ShapeMesh("TangentSphere", SerializableMeshType::Sphere, 100);
		tangentSphere.generateTangentMesh = true;
		tangentSphere.CreateMesh(&device,&deviceContext);
		meshes.insert({ tangentSphere.name, tangentSphere });
	}


	{
		SerializableMesh tangentMesh = SerializableMesh::ShapeMesh("TangentMesh", SerializableMeshType::Plane, 5);
		tangentMesh.generateTangentMesh = true;
		tangentMesh.CreateMesh(&device,&deviceContext);
		meshes.insert({ tangentMesh.name, tangentMesh });
	}

	{
		SerializableMesh tesselationQuad = SerializableMesh::ShapeMesh("TesselationQuad",
			SerializableMeshType::TesselationQuad, 150, 250);
		tesselationQuad.CreateMesh(&device,&deviceContext);
		tesselationQuad.GetMesh()->sendData(&deviceContext,
			D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		meshes.insert({ tesselationQuad.name, tesselationQuad });
	}
}


void AssetSystem::initTextures()
{
	this->textureMap.insert({ L"default", L"default" });

	addTexture(L"res/brick1.dds", L"res/brick1.dds");
	addTexture(L"diffuseBrick", L"res/brickwall.jpg");
	addTexture(L"normalBrick", L"res/brickwall_normal.png");
	addTexture(L"shipDiffuse", L"res/Wood_025_basecolor.jpg");
	addTexture(L"shipNormal", L"res/Wood_025_normal.png");

	//Terrain textures
	addTexture(L"diffuseTerrainSand", L"res/terrain_sand.jpg");
	addTexture(L"diffuseTerrainGrass", L"res/terrain_grass.jpg");
	addTexture(L"diffuseTerrainRock", L"res/terrain_rock.jpg");

	addTexture(L"rockDiffuse", L"res/Rock_047_BaseColor.jpg");
	addTexture(L"rockNormal", L"res/Rock_047_Normal.png");
	addTexture(L"rockDisplacement", L"res/Rock_047_Height.png");

	addTexture(L"gravelDiffuse", L"res/Gravel_001_BaseColor.jpg");
	addTexture(L"gravelNormal", L"res/Gravel_001_Normal.png");
	addTexture(L"gravelDisplacement", L"res/Gravel_001_Height.png");

//	sceneRT = new RenderTexture(&device, sWidth, sHeight, 1, 100);
//	effectRT = new RenderTexture(&device, sWidth, sHeight, 1, 100);
//	bloomTextureOne = new RenderTexture(getDevice(), sWidth, sHeight, 1, 100);
//	bloomTextureTwo = new RenderTexture(getDevice(), sWidth, sHeight, 1, 100);
//
//	// Build RenderTexture, this will be our alternative render target.
//	minimapTexture = new RenderTexture(&device, screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
//
//	passChain = new PostProcessPassChain(renderer->getDevice(), sWidth, sHeight, 0.1, 200);
}

 void AssetSystem::addTexture(std::wstring name, std::wstring fileapth)
{
	this->textureMap.insert({ name, fileapth });
	textureManager->loadTexture(name, fileapth);
}

 ID3D11ShaderResourceView* AssetSystem::getTexture(std::wstring textureName)
{
	return textureManager->getTexture(textureName);
}

 Material* AssetSystem::getMaterial(std::string materialName)
{
	return materials.at(materialName);
}

 SerializableMesh AssetSystem::getMesh(std::string meshName)
{
	 auto foundMesh = meshes.find(meshName);
	 ASSERT_WITH_MSG(foundMesh != meshes.end(), "Mesh Not Found: " + meshName);
	 return (*foundMesh).second; }

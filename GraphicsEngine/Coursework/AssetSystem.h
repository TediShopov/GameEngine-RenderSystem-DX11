#pragma once
#include "Material.h"
#include "SerializableMesh.h"
#include "TextureManager.h"


#include "DXF.h"
#include <map>
#include <set>
#include <functional>
#include "LuminanceThresholdPass.h"
#include "BloomComposite.h"

#include "RenderItem.h"
#include <random>
#include <cmath>
#include <wrl/client.h>
//#include <cerrno>

#define ASSERT_WITH_MSG(expr, msg) \
    do { \
        if (!(expr)) { \
            std::cerr << "Assertion failed: " << msg << std::endl; \
            assert(expr); \
        } \
    } while (0)

using namespace std;
 class AssetSystem
{
	//	public:
	//  void loadDefaults(ID3D11Device*, ID3D11DeviceContext*);
	//  const SerializableMesh& mesh(std::string_view) const;
	//  Material* material(std::string_view);
	//  ID3D11ShaderResourceView* texture(std::wstring_view);
	//
	//  // shaders grouped here too
	//  SkyMapShader& sky();
	//  DefaultShader& defaultLit();
	//  NormalMapShader& normalDisplace();
	//  WaveShader& wave();
	//  TerrainTesselationShader& terrainTess();


protected:
	ID3D11Device& device;
	ID3D11DeviceContext& deviceContext;
	//Textuer Manager
	std::unique_ptr<TextureManager> textureManager;
	//Texuter paths in use
	std::map<std::wstring, std::wstring> textureMap;

	//Materials
	std::map<std::string, Material*> materials;

	//Meshes
	std::map<std::string, SerializableMesh> meshes;


	





	void initMaterials();
	void initRenderCollections();
	void initMeshes();
	//void initShaders(HWND hwnd);

	void initTextures();
	//void initCameras();
	//void initLights();
	//void initShadowMap();
	//void initCascadedShadowMaps();

public:
	AssetSystem(ID3D11Device& device, ID3D11DeviceContext& context)
		:device(device), deviceContext(context)
	{
		textureManager = std::make_unique<TextureManager>(&device, &context);
	}
		
	~AssetSystem()
	{
		for (auto mesh : meshes)
		{
			delete mesh.second.GetMesh();
		}

		for (auto material : materials)
		{
			delete material.second;
		}
		this->materials.clear();

		this->meshes.clear();
		this->textureMap.clear();
	}
	void loadAssets()
	{
		initMaterials();
		initMeshes();
		initTextures();
	}


	void addTexture(std::wstring name, std::wstring fileapth);
	ID3D11ShaderResourceView* getTexture(std::wstring textureName);
	Material* getMaterial(std::string materialName);
	SerializableMesh getMesh(std::string meshName);
};


#pragma once
#include "simpleson/json.h"
#include "Scene.h"
#include <sstream>
#include <set>
#include <map>
class Scene;
class FPCamera;
class Transform;
class Light;
class Material;
class MeshInstance;
class TextureManager;
class ID3D11Device;
class ID3D11ShaderResourceView;
class BaseMesh;

class SceneJsonSerializer {

public:
	void serializeScene(std::string filepath,const Scene& scene);
	void deserializeScene(std::string filepath, Scene* scene);
	//Serialize into a file,  then deserializes. The resulting scene is compared to the initial ones to check  correctness
	bool ensureSame(std::string filepath, Scene* scene);
private:
//	const Scene* _readScene;
//	 Scene* _writeScene;

	template<typename C, typename T>
	 std::vector<json::jobject> toJsonArray(
		 C collection,
		 T convertFunc)
	{
		std::vector<json::jobject> objectsJson;
		for (auto obj : collection)
		{
			json::jobject objJson = convertFunc(obj);
			objectsJson.push_back(objJson);
		}
		return objectsJson;
	}


	 static json::jobject get_entry(const json::jobject& obj, std::string s);


	//static std::vector<float> glmToVec(glm::vec3 vec);
	static json::jobject transformJson(const Transform& t);
	static json::jobject cameraJson( FPCamera* FPCamera);
	static json::jobject lightJson( Light* light);
	static json::jobject meshJson(std::pair<std::string, SerializableMesh> pair);
	static json::jobject textureJson( std::pair<std::wstring,std::wstring>  ID3D11ShaderResourceView);
	static json::jobject materialJson(std::pair<std::string, Material*> pair);
	static json::jobject meshInstanceJson(const MeshInstance* rootInstance,const  Scene& scene);


	std::string getJsonString(const Scene& scene);
	void		jsonToCamera(json::jobject cameraJson, FPCamera* FPCamera);
	void		jsonToLight(json::jobject lightsJson, Light* light);
	SerializableMesh jsonToMesh(json::jobject meshesJson,  ID3D11Device* device);
	std::pair<std::wstring, std::wstring> jsonToTexture(json::jobject texturesJson,AssetSystem& assetSystem);
	Material* jsonToMaterial(json::jobject materialsJson);

	void jsonToTransform(json::jobject jobj, Transform& transform);
	void jsonToMeshInstance(json::jobject obj,Scene* scene ,MeshInstance* parent = nullptr);
	void remove_unnecessary_escapings(std::string& str);


};


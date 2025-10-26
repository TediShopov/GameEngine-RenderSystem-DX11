#include "SceneJsonSerializer.h"
#include <algorithm>

#include <wchar.h>
#include <locale.h>
#include <codecvt>
#include <algorithm>
#include <future>

#define JSONARR012(jsonArr) std::stof(jsonArr.array(0)), std::stof(jsonArr.array(1)), std::stof(jsonArr.array(2))

#define JSONARR0123(jsonArr) std::stof(jsonArr.array(0)), std::stof(jsonArr.array(1)), std::stof(jsonArr.array(2)), std::stof(jsonArr.array(3))



std::wstring StringToWChar(std::string str)
{
	std::wstring wstr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str.c_str());
	return wstr;
}

std::vector<float> XMFloat3ToVec(XMFLOAT3 vec)
{
	std::vector<float> vecFloat;
	vecFloat.push_back(vec.x);
	vecFloat.push_back(vec.y);
	vecFloat.push_back(vec.z);
	return vecFloat;
}

std::vector<float> XMFloat4ToVec(XMFLOAT4 vec)
{
	std::vector<float> vecFloat;
	vecFloat.push_back(vec.x);
	vecFloat.push_back(vec.y);
	vecFloat.push_back(vec.z);
	vecFloat.push_back(vec.w);

	return vecFloat;
}


std::string SceneJsonSerializer::getJsonString(const Scene& scene)
{
	auto renderSystem = scene.GetRenderSystem();
	auto assetSystem = scene.GetAssetSystem();

	//Final json to return
	json::jobject json;


	//Write All Resource Composing the scene
	json::jobject camJson = cameraJson(renderSystem->getCamera());
	std::vector<json::jobject> allLights =		toJsonArray(renderSystem->lights, lightJson);
	std::vector<json::jobject> meshesJsons	=	toJsonArray(assetSystem->getMeshes(), meshJson);
	std::vector<json::jobject> texturesJsons=	toJsonArray(assetSystem->getTextureMap(), textureJson);
	std::vector<json::jobject> materialsJson=	toJsonArray(assetSystem->getMaterials(), materialJson);

	//Write BaseMesh Instances
	//Iteratoe over each transform root nodes and then use get all children method
	std::vector<json::jobject> meshInstancesJson;

	//scene.setRootInstances();
	for (auto rootInstance : scene.rootMeshInstances)
	{
		meshInstancesJson.push_back(meshInstanceJson(rootInstance,  scene));
	}


	json["FPCamera"] = camJson;
	json["Lights"] = allLights;
	json["Meshes"] = meshesJsons;
	json["TexturePaths"] = texturesJsons;
	json["Materials"] = materialsJson;
	json["MeshInstances"] = meshInstancesJson;

	return json.as_string();
}
void SceneJsonSerializer::jsonToCamera(json::jobject cameraJson, FPCamera* cam)
{
	json::jobject position = get_entry(cameraJson, "Position");
	json::jobject rotation = get_entry(cameraJson, "Rotation");

	//this->jsonToTransform(get_entry(cameraJson, "Transform"), this->_scene->FPCamera.transform);
	cam->setPosition(std::stof(position.array(0)), std::stof(position.array(1)), std::stof(position.array(2)));
	cam->setRotation(std::stof(rotation.array(0)), std::stof(rotation.array(1)), std::stof(rotation.array(2)));
}
void SceneJsonSerializer::jsonToLight(json::jobject lightsJson, Light* light)
{
	//jsonToTransform(get_entry(lightsJson, "Transform"), light->);
	json::jobject position = get_entry(lightsJson, "Position");
	json::jobject direction = get_entry(lightsJson, "Direction");

	//this->jsonToTransform(get_entry(cameraJson, "Transform"), this->_scene->FPCamera.transform);
	light->setPosition(JSONARR012(position));
	light->setDirection(JSONARR012(direction));
	//Type - not supported yet
	//Color component
	json::jobject ambient = get_entry(lightsJson, "Ambient");
	light->setAmbientColour(JSONARR0123(ambient));
	json::jobject diffuse = get_entry(lightsJson, "Diffuse");
	light->setDiffuseColour(JSONARR0123(diffuse));
	json::jobject specular = get_entry(lightsJson, "Specular");
	light->setSpecularColour(JSONARR0123(specular));
	/*std::string lightType = json::parsing::unescape_characters(lightsJson.get("Type").c_str());
	if (lightType == "Directional")
	{
		light->Type = LightType::DIRECTIONAL;

	}
	else if (lightType == "Point")
	{
		light->Type = LightType::POINT;
	}
	else if (lightType == "Spot")
	{
		light->Type = LightType::SPOTLIGHT;
	}*/

}
SerializableMesh SceneJsonSerializer::jsonToMesh(json::jobject meshesJson, ID3D11Device* device)
{
	//Add BaseMesh to meshes in scene
	SerializableMesh baseMesh;

	auto name = meshesJson["Name"].as_string();
	remove_unnecessary_escapings(name);

	auto filename = meshesJson["Filename"].as_string();
	remove_unnecessary_escapings(filename);

	
	

	SerializableMeshType type= (SerializableMeshType) std::stoi(meshesJson["Type"]);


	if (type != SerializableMeshType::Custom)
	{
		int res = (SerializableMeshType)std::stoi(meshesJson["ResolutionParam"]);
		int size = (SerializableMeshType)std::stoi(meshesJson["Size"]);

		baseMesh = SerializableMesh::ShapeMesh(name,type, res, size);
	}
	else
	{
		baseMesh = SerializableMesh::CustomMesh(name,filename);
	}

	auto tangentMesh = meshesJson["GenerateTangents"].is_true();

	baseMesh.generateTangentMesh = tangentMesh;


	//baseMesh.CreateMesh(device,);

	return baseMesh;
}
std::pair<std::wstring, std::wstring> SceneJsonSerializer::jsonToTexture(json::jobject texturesJson, AssetSystem& assetSystem )
{
	auto uid = texturesJson["Name"].as_string();
	auto filename = texturesJson["Filename"].as_string();
	remove_unnecessary_escapings(filename);
	remove_unnecessary_escapings(uid);

	std::wstring filenameW = std::wstring(StringToWChar(filename));
	std::wstring uidW =  std::wstring(StringToWChar(uid));


	return	 std::pair<const wchar_t*,const wchar_t*>(uidW.c_str(),filenameW.c_str());
}



Material* SceneJsonSerializer::jsonToMaterial(json::jobject materialsJson)
{
	Material* mat = new Material();
	//Add ID3D11ShaderResourceView to ID3D11ShaderResourceView in scene

	//Name
	auto name = materialsJson["Name"].as_string();
	mat->name = name;

	//Blend
	auto blend = materialsJson["Blend"].is_true();
	mat->blend = blend;

	//TexturePath
//Manualy remove some unnessecasary escapings

	if (materialsJson.has_key("DiffuseTexture"))
	{
		std::string diffuse = materialsJson["DiffuseTexture"];
		remove_unnecessary_escapings(diffuse);
		std::wstring str = StringToWChar(diffuse);
		//assetSystem.textureManager.get().at(str.c_str());
		mat->diffuseTexture = str.c_str();
	}

	if (materialsJson.has_key("NormalTexture"))
	{
		std::string normal = materialsJson["NormalTexture"];
		remove_unnecessary_escapings(normal);
		std::wstring str = wstring(StringToWChar(normal));
		//this->_scene->textureMap.at(str.c_str());

		mat->normalTexture = str.c_str();
	}


	if (materialsJson.has_key("DisplacementTexture"))
	{
		std::string displacement = materialsJson["DisplacementTexture"];
		remove_unnecessary_escapings(displacement);
		std::wstring str = wstring(StringToWChar(displacement));
		//this->_scene->textureMap.at(str.c_str());

		mat->displacementTexture = str.c_str();
	}


	//Color components
	mat->ambient = XMFLOAT3(JSONARR012(get_entry(materialsJson, "Ambient")));
	mat->diffuse = XMFLOAT3(JSONARR012(get_entry(materialsJson, "Diffuse")));
	mat->specular = XMFLOAT3(JSONARR012(get_entry(materialsJson, "Specular")));
	mat->emissive = XMFLOAT3(JSONARR012(get_entry(materialsJson, "Emissive")));

	mat->shininess = std::stof(materialsJson["Shininess"]);
	mat->reflectionFactor = std::stof(materialsJson["ReflectionFactor"]);
	return mat;
}
void SceneJsonSerializer::jsonToTransform(json::jobject jobj, Transform& transform)
{
	transform.setPosition(JSONARR012(get_entry(jobj, "Position")));
	transform.setRotation(JSONARR012(get_entry(jobj, "Rotation")));
	transform.setScale(JSONARR012(get_entry(jobj, "Scale")));
	transform.setQuaternion(JSONARR0123(get_entry(jobj,"Quaternion")));
	transform.setComposeRotationFromQuaternions(jobj["IsFromQuaternion"].is_true());



}

void SceneJsonSerializer::jsonToMeshInstance(json::jobject obj, Scene * scene, MeshInstance* parent)
{
	MeshInstance* meshInstance = new MeshInstance();

	meshInstance->name = obj["Name"].as_string();
	meshInstance->render = obj["Render"].is_true();

	//Transform

	this->jsonToTransform(get_entry(obj, "Transform"), meshInstance->transform);

	//Material
	std::string matString = json::parsing::unescape_characters(obj.get("Material").c_str());
	remove_unnecessary_escapings(matString);
	if (matString!="" && matString!="None")
		meshInstance->setMaterial(scene->GetAssetSystem()->getMaterial(matString));

	//BaseMesh
	std::string meshString = json::parsing::unescape_characters(obj.get("BaseMesh").c_str());
	remove_unnecessary_escapings(meshString);
	
	//TODO CHECK WHAT HAPPENS IF MESH IN NULL
	meshInstance->setMesh(scene->GetAssetSystem()->getMesh(meshString));

//	if (_scene->meshes.find(meshString) !=_scene->meshes.end())
//	{
//		meshInstance->setMesh(this->_scene->meshes.at(meshString));
//	}


	//Set Parent
	if (parent != nullptr)
	{
		meshInstance->transform.setParent(&parent->transform);
	}

	//Inserat getHeightAt BaseMesh instances
	scene->meshInstances.push_back(std::unique_ptr<MeshInstance>(meshInstance));

	//Init Children
	for (int i = 0; i < get_entry(obj, "Children").size(); ++i)
	{
		this->jsonToMeshInstance(get_entry(obj, "Children").array(i), scene, meshInstance);
	}


}





json::jobject SceneJsonSerializer::transformJson(const Transform& t)
{
	json::jobject transform;
	XMFLOAT3 pos, scl;
	XMStoreFloat3(&pos,t.getPosition());
	XMStoreFloat3(&scl, t.getScale());

	auto quaternion = t.getQuaternion().m128_f32;

	transform["Position"] = std::vector<float>{pos.x,pos.y,pos.z};
	transform["Rotation"] = std::vector<float>{t.getPitch(),t.getYaw(),t.getRoll()};
	transform["Scale"] = std::vector<float>{scl.x,scl.y,scl.z};
	transform["IsFromQuaternion"].set_boolean(t.getComposeRotationFromQuaternions());
	transform["Quaternion"] = std::vector<float>{ quaternion[0],quaternion[1],quaternion[2],quaternion[3] };

	return transform;
}

json::jobject SceneJsonSerializer::get_entry(const json::jobject& obj, std::string s)
{
	return obj[s];
}

json::jobject SceneJsonSerializer::cameraJson( FPCamera* cam)
{
	json::jobject cameraJson;
	cameraJson["Position"] = XMFloat3ToVec(cam->getPosition());
	cameraJson["Rotation"] = XMFloat3ToVec(cam->getRotation());

	/*cameraJson["Fov"] = FPCamera->getProjectionParams().fov;
	cameraJson["NearPlane"] = FPCamera->getProjectionParams().nearPlane;
	cameraJson["FarPlane"] = FPCamera->getProjectionParams().farPlane;*/
	return cameraJson;
}

json::jobject SceneJsonSerializer::lightJson(Light* light)
{
	json::jobject currenLightJson;
	currenLightJson["Position"] = XMFloat3ToVec(light->getPosition());;
	currenLightJson["Direction"] = XMFloat3ToVec(light->getDirection());;
	currenLightJson["Type"] = "Point";
	currenLightJson["Diffuse"] = XMFloat4ToVec(light->getDiffuseColour());
	currenLightJson["Ambient"] = XMFloat4ToVec(light->getAmbientColour());
	currenLightJson["Specular"] = XMFloat4ToVec(light->getSpecularColour());
	return currenLightJson;
}

json::jobject SceneJsonSerializer::meshJson(std::pair< std::string, SerializableMesh> pair)
{
	json::jobject meshesJson;
	SerializableMesh mesh = pair.second;


	meshesJson["Name"] = mesh.name;
	meshesJson["Filename"] = mesh.filepath;
	meshesJson["Type"] = mesh._type;
	meshesJson["ResolutionParam"] = mesh._resolutionParam;
	meshesJson["Size"] = mesh._size;
	meshesJson["GenerateTangents"].set_boolean(mesh.generateTangentMesh);

	if (mesh._type!=SerializableMeshType::Custom)
	{
		meshesJson["ResolutionParam"] = mesh._resolutionParam;
	}
	return meshesJson;
}

std::string wcharToString(const wchar_t* strToSet)
{
	if (strToSet == nullptr)
	{
		return string();
	}
	std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t> converter;
	std::wstring wstring(strToSet);
	std::string string = converter.to_bytes(wstring);
	return string;
}

json::jobject SceneJsonSerializer::textureJson( std::pair<std::wstring, std::wstring> texturePath)
{
	json::jobject textureJson;
	//if (texturePath.first != nullptr && texturePath.second != nullptr)
	//{
	//	
	///*	std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t> converter;
	//	std::wstring texturePathName(texturePath.first);
	//	std::string texturePathNameW = converter.to_bytes(texturePathName);

	//	std::wstring texturePathPath(texturePath.first);
	//	std::string texturePathPathW = converter.to_bytes(texturePathPath);*/

	//	textureJson["Name"] = wcharToString(texturePath.first);
	//	textureJson["Filename"] = wcharToString(texturePath.second);
	//}

	textureJson["Name"] = wcharToString(texturePath.first.c_str());
	textureJson["Filename"] = wcharToString(texturePath.second.c_str());
	return textureJson;;
}




json::jobject SceneJsonSerializer::materialJson(std::pair<std::string, Material*> pair)
{
	if (pair.second ==nullptr)
	{
		throw exception("Tried to serialize a null material");
	}

	json::jobject materialJson;
	materialJson["Name"] = pair.first;
	materialJson["Blend"].set_boolean(pair.second->blend);
	if (!pair.second->diffuseTexture.empty())
	{
		materialJson["DiffuseTexture"] = wcharToString(pair.second->diffuseTexture.c_str());
	}

	if (!pair.second->normalTexture.empty())
	{
		materialJson["NormalTexture"] = wcharToString(pair.second->normalTexture.c_str());
	}
	
	if (!pair.second->displacementTexture.empty())
	{
		materialJson["DisplacementTexture"] = wcharToString(pair.second->displacementTexture.c_str());
	}
	
	materialJson["Shininess"] = pair.second->shininess;
	materialJson["ReflectionFactor"] = pair.second->reflectionFactor;
	materialJson["Diffuse"] = XMFloat3ToVec(pair.second->diffuse);
	materialJson["Ambient"] = XMFloat3ToVec(pair.second->ambient);
	materialJson["Specular"] = XMFloat3ToVec(pair.second->specular);
	materialJson["Emissive"] = XMFloat3ToVec(pair.second->emissive);
	return materialJson;
}

json::jobject SceneJsonSerializer::meshInstanceJson(const MeshInstance* rootInstance,const Scene& scene)
{
	
	json::jobject instanceJson;
	instanceJson["Name"] = rootInstance->name;
	instanceJson["Render"].set_boolean(rootInstance->render);
	instanceJson["Transform"] = transformJson(rootInstance->transform);
	std::string materialName;
	 Material* foundMaterial = nullptr;
	 instanceJson["Material"] = "";
	for (auto& it : scene.GetAssetSystem()->getMaterials()) {

		
		if (it.second == rootInstance->getMaterial()) {
			materialName = it.first;
			foundMaterial = it.second;
			instanceJson["Material"] = materialName;
			break;
		}
	}



	std::string foundMeshPath;

	const BaseMesh* foundMesh = nullptr;
	for (auto& it : scene.GetAssetSystem()->getMeshes()) {

	
		if (it.second.GetMesh() == rootInstance->getMesh()) {
			foundMeshPath = it.first;
			foundMesh = it.second.GetMesh();
			break;
		}
	}

	if (foundMesh != nullptr)
	{
		instanceJson["BaseMesh"] = foundMeshPath;

	}
	else
	{
		instanceJson["BaseMesh"] = "";

	}


	//Perform DFS 
	auto childrenTransforms = rootInstance->transform.getChildrenTransforms();
	std::vector<json::jobject> chlidrenList;
	for (auto childTransform : childrenTransforms)
	{
		//chlidrenList.push_back(meshInstanceJson(childrenTransforms,scene))
		
		//Find BaseMesh instance pointer by iterating through allmesh instances and searching for the one with this transform
		auto found = std::find_if(
			scene.meshInstances.begin(), scene.meshInstances.end(), 
			[childTransform](const std::unique_ptr<MeshInstance>& m) { return (m.get()->transform) == *childTransform; });
		if (found != scene.meshInstances.end())
		{
			//childrenMeshInstances.push_back(&(*found));
			chlidrenList.push_back(SceneJsonSerializer::meshInstanceJson((*found).get(), scene));
		}
		else {

		}
	}

	instanceJson["Children"] = chlidrenList;

	return instanceJson;
}

void SceneJsonSerializer::remove_unnecessary_escapings(std::string& str)
{
	auto found = str.find_first_of("\\");
	while (found != std::string::npos)
	{
		str = str.erase(found, 1);
		found = str.find_first_of("\\");
	}
	return;
}

void SceneJsonSerializer::serializeScene(std::string filepath, const Scene& scene)
{
	std::string filename(filepath);
	std::ofstream filestream(filename);
	std::string fileOutput = getJsonString(scene);
	if (filestream)
	{
		filestream << fileOutput;
	}
	filestream.close();
}

std::string slurp(std::ifstream& in) {
	std::ostringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

void SceneJsonSerializer::deserializeScene(std::string filepath, Scene* scene)
{
	auto writeScene = scene;
	auto renderSystem = writeScene->GetRenderSystem();
	auto assetSystem = writeScene->GetAssetSystem();
	writeScene->resetResources();
	//Read all file contents in string
	std::ifstream ifs(filepath);
	std::string s = slurp(ifs);
	//Translate to json
	json::jobject jsonScene = json::jobject::parse(s);

	//Load FPCamera
	this->jsonToCamera(json::jobject::parse(jsonScene.get("FPCamera")), writeScene->getCamera());
	//Lights -- currently supoortess only one light
	//this->jsonToLight(get_entry(jsonScene, "Lights").array(0), writeScene->lights[0]);
	this->jsonToLight(get_entry(jsonScene, "Lights").array(0), renderSystem->lights[0]);


	std::vector<std::future<void>> meshFutures;


	json::jobject meshesJson = get_entry(jsonScene, "Meshes");
	meshFutures.reserve(meshesJson.size());
	std::mutex meshMutex;
	for (size_t i = 0; i < meshesJson.size(); i++)
	{
		meshFutures.emplace_back(std::async(std::launch::async, [&,i]() {
			std::string name = meshesJson.array(i).as_object()["Name"];
			std::string filename = meshesJson.array(i).as_object()["Filename"];

			remove_unnecessary_escapings(name);
			remove_unnecessary_escapings(filename);

			//BaseMesh* mesh = this->jsonToMesh(meshesJson.array(i).as_object(),this->_scene->getDevice());
			SerializableMesh mesh = this->jsonToMesh(meshesJson.array(i).as_object(), writeScene->getDevice());
			mesh.CreateMesh(writeScene->getDevice(), writeScene->getDeviceContext());

			std::lock_guard<std::mutex> lock(meshMutex);
			assetSystem->addMesh(name, mesh);


			}));


	}

	//Load Textures 
	std::vector<std::future<void>> textureFutures; 
	json::jobject texturesJson = get_entry(jsonScene, "TexturePaths");
	textureFutures.reserve(texturesJson.size());
	for (size_t i = 0; i < texturesJson.size(); i++)
	{
		textureFutures.emplace_back(std::async(std::launch::async, [&, i]() {
			auto t = this->jsonToTexture(texturesJson.array(i), *assetSystem);
			if (t.first != L"default")
				assetSystem->addTexture(t.first, t.second);

			}));


	}


	// Wait for all mesh futures
    for (auto& f : meshFutures)
        f.get();
	// Wait for all texture futures
    for (auto& t : textureFutures)
        t.get();
	//Load Materials
	json::jobject materialsJson = jsonScene["Materials"];
	for (size_t i = 0; i < materialsJson.size(); i++)
	{
		Material* mat = this->jsonToMaterial(materialsJson.array(i));
		assetSystem->addMaterial(mat->name, mat);
	}






	//BaseMesh Instances -- Level 1 (Roots)
	json::jobject meshInstancesJson = get_entry(jsonScene, "MeshInstances");
	for (size_t i = 0; i < meshInstancesJson.size(); i++)
	{
		this->jsonToMeshInstance(meshInstancesJson.array(i),scene);
	}





	writeScene->setRootInstances();

	//writeScene->initRenderCollections();
	writeScene->fillRenderCollections();
	writeScene->assignSpecialInstances();
	writeScene->activeMeshInstance = writeScene->meshInstances[1].get();

}

bool SceneJsonSerializer::ensureSame(std::string filepath, Scene* scene) 
{
	//Initial/pre-serializatoin scene

	auto iMeshes = scene->GetAssetSystem()->getMeshes();
	auto iMaterials = scene->GetAssetSystem()->getMaterials();
	auto iTextures = scene->GetAssetSystem()->getTextureMap();

	//Check the properties of the mesh instances
	std::vector<MeshInstance> iMeshInstances;
	for (auto& mi : scene->meshInstances)
		iMeshInstances.push_back(*mi);

	//Additionally, check the tranform scene hierarchy 

	Transform iTransformHieararchy;
	for (auto& mi : scene->rootMeshInstances)
		iTransformHieararchy.addChild(mi->transform.cloneHierarchy());

	auto iCamera = *scene->getCamera();
	std::vector<Light> iLights;
	for (auto& l : scene->GetRenderSystem()->lights)
		iLights.push_back(*l);


	this->serializeScene(filepath, *scene);
	this->deserializeScene(filepath,scene);


	//Final/deserialized scene

	auto fMeshes = scene->GetAssetSystem()->getMeshes();
	auto fMaterials = scene->GetAssetSystem()->getMaterials();
	auto fTextures = scene->GetAssetSystem()->getTextureMap();


	std::vector<MeshInstance> fMeshInstances;
	for (auto& mi : scene->meshInstances)
		fMeshInstances.push_back(*(mi.get()));


	Transform fTransformHieararchy;
	for (auto& mi : scene->rootMeshInstances)
		fTransformHieararchy.addChild(mi->transform.cloneHierarchy());

	auto fCamera = *scene->getCamera();

	std::vector<Light> fLights;
	for (auto& l : scene->GetRenderSystem()->lights)
		fLights.push_back(*l);
	
	for (auto& pair : iMeshes)
	{
		auto found = fMeshes.find(pair.first);
		if (found == fMeshes.end() || !(pair.second == found->second))
			return false;
	}

	for (auto& pair : iMaterials)
	{
		auto found = iMaterials.find(pair.first);
		if (found == iMaterials.end() || !(*pair.second == *found->second))
			return false;
	}
	for (auto& pair : iTextures)
	{
		auto found = fTextures.find(pair.first);
		if (found == fTextures.end() || !(pair.second == found->second))
			return false;
	}

	if ((iMeshInstances == fMeshInstances) == false)
		return false;

	if ((iTransformHieararchy.compareHierarchy(fTransformHieararchy)) == false)
		return false;

	if ((iCamera == fCamera) == false)
		return false;

	if ((iLights == fLights) == false)
		return false;

	return true;



}

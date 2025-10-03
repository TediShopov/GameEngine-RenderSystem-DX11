//#include "SceneJsonSerializer.h"
//#include <algorithm>
//
//#include <wchar.h>
//#include <locale.h>
//#include <codecvt>
//#include <algorithm>
//#define JSONARR012(jsonArr) std::stof(jsonArr.array(0)), std::stof(jsonArr.array(1)), std::stof(jsonArr.array(2))
//#define JSONARR0123(jsonArr) std::stof(jsonArr.array(0)), std::stof(jsonArr.array(1)), std::stof(jsonArr.array(2)), std::stof(jsonArr.array(3))
//
//
//
//std::wstring StringToWChar(std::string str)
//{
//	std::wstring wstr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str.c_str());
//	return wstr;
//}
//
//std::vector<float> XMFloat3ToVec(XMFLOAT3 vec)
//{
//	std::vector<float> vecFloat;
//	vecFloat.push_back(vec.x);
//	vecFloat.push_back(vec.y);
//	vecFloat.push_back(vec.z);
//	return vecFloat;
//}
//
//std::vector<float> XMFloat4ToVec(XMFLOAT4 vec)
//{
//	std::vector<float> vecFloat;
//	vecFloat.push_back(vec.x);
//	vecFloat.push_back(vec.y);
//	vecFloat.push_back(vec.z);
//	vecFloat.push_back(vec.w);
//
//	return vecFloat;
//}
//
//
//std::string SceneJsonSerializer::getJsonString()
//{
//
//	//Final json to return
//	json::jobject json;
//
//	//Write All Resource Composing the scene
//	json::jobject camJson = cameraJson(this->_scene->getCamera());
//	std::vector<json::jobject> allLights =		toJsonArray(_scene->lights, lightJson);
//	std::vector<json::jobject> meshesJsons	=	toJsonArray(_scene->meshes, meshJson);
//	std::vector<json::jobject> texturesJsons=	toJsonArray(_scene->textureMap,textureJson);
//	std::vector<json::jobject> materialsJson=	toJsonArray(_scene->materials, materialJson);
//
//	//Write BaseMesh Instances
//	//Iteratoe over each transform root nodes and then use get all children method
//	std::vector<json::jobject> meshInstancesJson;
//
//	_scene->setRootInstances();
//	for (auto rootInstance : _scene->rootMeshInstances)
//	{
//		meshInstancesJson.push_back(meshInstanceJson(rootInstance,  _scene));
//	}
//
//
//	json["FPCamera"] = camJson;
//	json["Lights"] = allLights;
//	json["Meshes"] = meshesJsons;
//	json["TexturePaths"] = texturesJsons;
//	json["Materials"] = materialsJson;
//	json["MeshInstances"] = meshInstancesJson;
//
//	return json.as_string();
//}
//void SceneJsonSerializer::jsonToCamera(json::jobject cameraJson, FPCamera* cam)
//{
//	json::jobject position = get_entry(cameraJson, "Position");
//	json::jobject rotation = get_entry(cameraJson, "Rotation");
//
//	//this->jsonToTransform(get_entry(cameraJson, "Transform"), this->_scene->FPCamera.transform);
//	cam->setPosition(std::stof(position.array(0)), std::stof(position.array(1)), std::stof(position.array(2)));
//	cam->setRotation(std::stof(rotation.array(0)), std::stof(rotation.array(1)), std::stof(rotation.array(2)));
//}
//void SceneJsonSerializer::jsonToLight(json::jobject lightsJson, Light* light)
//{
//	//jsonToTransform(get_entry(lightsJson, "Transform"), light->);
//	json::jobject position = get_entry(lightsJson, "Position");
//	json::jobject direction = get_entry(lightsJson, "Direction");
//
//	//this->jsonToTransform(get_entry(cameraJson, "Transform"), this->_scene->FPCamera.transform);
//	light->setPosition(JSONARR012(position));
//	light->setDirection(JSONARR012(direction));
//	//Type - not supported yet
//	//Color component
//	json::jobject ambient = get_entry(lightsJson, "Ambient");
//	light->setAmbientColour(JSONARR0123(ambient));
//	json::jobject diffuse = get_entry(lightsJson, "Diffuse");
//	light->setDiffuseColour(JSONARR0123(diffuse));
//	json::jobject specular = get_entry(lightsJson, "Specular");
//	light->setSpecularColour(JSONARR0123(specular));
//	/*std::string lightType = json::parsing::unescape_characters(lightsJson.get("Type").c_str());
//	if (lightType == "Directional")
//	{
//		light->Type = LightType::DIRECTIONAL;
//
//	}
//	else if (lightType == "Point")
//	{
//		light->Type = LightType::POINT;
//	}
//	else if (lightType == "Spot")
//	{
//		light->Type = LightType::SPOTLIGHT;
//	}*/
//
//}
//SerializableMesh SceneJsonSerializer::jsonToMesh(json::jobject meshesJson, ID3D11Device* device)
//{
//	//Add BaseMesh to meshes in scene
//	SerializableMesh baseMesh;
//
//	auto name = meshesJson["Name"].as_string();
//	remove_unnecessary_escapings(name);
//
//	auto filename = meshesJson["Filename"].as_string();
//	remove_unnecessary_escapings(filename);
//
//	
//	
//
//	SerializableMeshType type= (SerializableMeshType) std::stoi(meshesJson["Type"]);
//
//
//	if (type != SerializableMeshType::Custom)
//	{
//		int res = (SerializableMeshType)std::stoi(meshesJson["ResolutionParam"]);
//
//		baseMesh = SerializableMesh::ShapeMesh(name,type, res);
//	}
//	else
//	{
//		baseMesh = SerializableMesh::CustomMesh(name,filename);
//	}
//
//	auto tangentMesh = meshesJson["GenerateTangents"].is_true();
//	baseMesh.generateTangentMesh = tangentMesh;
//
//
//	//baseMesh.CreateMesh(device,);
//
//	return baseMesh;
//}
//std::pair<std::wstring, std::wstring> SceneJsonSerializer::jsonToTexture(json::jobject texturesJson, TextureManager* txtMng)
//{
//	auto uid = texturesJson["Name"].as_string();
//	auto filename = texturesJson["Filename"].as_string();
//	remove_unnecessary_escapings(filename);
//	remove_unnecessary_escapings(uid);
//
//	std::wstring* filenameW= new std::wstring(StringToWChar(filename));
//	std::wstring* uidW = new std::wstring(StringToWChar(uid));
//
//	if (uid == "default")
//	{
//		int a = 3;
//		return	 std::pair<const wchar_t*, const wchar_t*>(uidW->c_str(), filenameW->c_str());
//	}
//
//	txtMng->loadTexture(uidW->c_str(), filenameW->c_str());
//	//TODO no way to save string, add wchar_t dicto to scene.h
//	return	 std::pair<const wchar_t*,const wchar_t*>(uidW->c_str(),filenameW->c_str());
//}
//
//
//
//Material* SceneJsonSerializer::jsonToMaterial(json::jobject materialsJson)
//{
//	Material* mat = new Material();
//	//Add ID3D11ShaderResourceView to ID3D11ShaderResourceView in scene
//
//	//Name
//	auto name = materialsJson["Name"].as_string();
//	mat->name = name;
//
//	//Blend
//	auto blend = materialsJson["Blend"].is_true();
//	mat->blend = blend;
//	
//		//TexturePath
//	//Manualy remove some unnessecasary escapings
//
//	if (materialsJson.has_key("DiffuseTexture"))
//	{
//		std::string diffuse = materialsJson["DiffuseTexture"];
//		remove_unnecessary_escapings(diffuse);
//		std::wstring str=  StringToWChar(diffuse);
//		this->_scene->textureMap.at(str.c_str());
//		mat->diffuseTexture = str.c_str();
//	}
//
//		if (materialsJson.has_key("NormalTexture"))
//		{
//			std::string normal = materialsJson["NormalTexture"];
//			remove_unnecessary_escapings(normal);
//			std::wstring str =  wstring(StringToWChar(normal));
//			this->_scene->textureMap.at(str.c_str());
//
//			mat->normalTexture = str.c_str();
//		}
//	
//	
//		if (materialsJson.has_key("DisplacementTexture"))
//		{
//			std::string displacement = materialsJson["DisplacementTexture"];
//			remove_unnecessary_escapings(displacement);
//			std::wstring str =  wstring(StringToWChar(displacement));
//			this->_scene->textureMap.at(str.c_str());
//
//			mat->displacementTexture = str.c_str();
//		}
//	
//
//	//Color components
//	mat->ambient = XMFLOAT3(JSONARR012(get_entry(materialsJson, "Ambient")));
//	mat->diffuse = XMFLOAT3(JSONARR012(get_entry(materialsJson, "Diffuse")));
//	mat->specular = XMFLOAT3(JSONARR012(get_entry(materialsJson, "Specular")));
//
//	mat->shininess = std::stof(materialsJson["Shininess"]);
//	return mat;
//}
//void SceneJsonSerializer::jsonToTransform(json::jobject jobj, Transform& transform)
//{
//	transform.setPosition(JSONARR012(get_entry(jobj, "Position")));
//	transform.setRotation(JSONARR012(get_entry(jobj, "Rotation")));
//	transform.setScale(JSONARR012(get_entry(jobj, "Scale")));
//}
//
//void SceneJsonSerializer::jsonToMeshInstance(json::jobject obj, MeshInstance* parent)
//{
//	MeshInstance* meshInstance = new MeshInstance();
//
//	//Transform
//	this->jsonToTransform(get_entry(obj, "Transform"), meshInstance->transform);
//
//	//Material
//	std::string matString = json::parsing::unescape_characters(obj.get("Material").c_str());
//	remove_unnecessary_escapings(matString);
//	if (matString!="" && matString!="None")
//	{
//		meshInstance->setMaterial(this->_scene->materials.at(matString));
//	}
//
//	//BaseMesh
//	std::string meshString = json::parsing::unescape_characters(obj.get("BaseMesh").c_str());
//	remove_unnecessary_escapings(meshString);
//	if (_scene->meshes.find(meshString) !=_scene->meshes.end())
//	{
//		meshInstance->setMesh(this->_scene->meshes.at(meshString));
//	}
//
//
//	//Set Parent
//	if (parent != nullptr)
//	{
//		meshInstance->transform.setParent(&parent->transform);
//	}
//
//	//Inserat getHeightAt BaseMesh instances
//	this->_scene->meshInstances.push_back(std::unique_ptr<MeshInstance>(meshInstance));
//
//	//Init Children
//	for (int i = 0; i < get_entry(obj, "Children").size(); ++i)
//	{
//		this->jsonToMeshInstance(get_entry(obj, "Children").array(i), meshInstance);
//	}
//
//
//}
//
//
//
//
//
//json::jobject SceneJsonSerializer::transformJson(const Transform& t)
//{
//	json::jobject transform;
//	XMFLOAT3 pos, scl;
//	XMStoreFloat3(&pos,t.getPosition());
//	XMStoreFloat3(&scl, t.getScale());
//
//	transform["Position"] = std::vector<float>
//	{
//		pos.x,pos.y,pos.z
//	};
//	transform["Rotation"] = std::vector<float>
//	{
//		t.getPitch(),
//		t.getYaw(),
//		t.getRoll()
//	};
//	transform["Scale"] = std::vector<float>
//	{
//		scl.x,scl.y,scl.z
//	};
//
//	return transform;
//}
//
//json::jobject SceneJsonSerializer::get_entry(const json::jobject& obj, std::string s)
//{
//	return obj[s];
//}
//
//json::jobject SceneJsonSerializer::cameraJson( FPCamera* cam)
//{
//	json::jobject cameraJson;
//	cameraJson["Position"] = XMFloat3ToVec(cam->getPosition());
//	cameraJson["Rotation"] = XMFloat3ToVec(cam->getRotation());
//
//	/*cameraJson["Fov"] = FPCamera->getProjectionParams().fov;
//	cameraJson["NearPlane"] = FPCamera->getProjectionParams().nearPlane;
//	cameraJson["FarPlane"] = FPCamera->getProjectionParams().farPlane;*/
//	return cameraJson;
//}
//
//json::jobject SceneJsonSerializer::lightJson(Light* light)
//{
//	json::jobject currenLightJson;
//	currenLightJson["Position"] = XMFloat3ToVec(light->getPosition());;
//	currenLightJson["Direction"] = XMFloat3ToVec(light->getDirection());;
//	currenLightJson["Type"] = "Point";
//	currenLightJson["Diffuse"] = XMFloat4ToVec(light->getDiffuseColour());
//	currenLightJson["Ambient"] = XMFloat4ToVec(light->getAmbientColour());
//	currenLightJson["Specular"] = XMFloat4ToVec(light->getSpecularColour());
//	return currenLightJson;
//}
//
//json::jobject SceneJsonSerializer::meshJson(std::pair< std::string, SerializableMesh> pair)
//{
//	json::jobject meshesJson;
//	SerializableMesh mesh = pair.second;
//
//
//	meshesJson["Name"] = mesh.name;
//	meshesJson["Filename"] = mesh.filepath;
//	meshesJson["Type"] = mesh._type;
//	meshesJson["GenerateTangents"].set_boolean(mesh.generateTangentMesh);
//
//	if (mesh._type!=SerializableMeshType::Custom)
//	{
//		meshesJson["ResolutionParam"] = mesh._resolutionParam;
//	}
//	return meshesJson;
//}
//
//std::string wcharToString(const wchar_t* strToSet)
//{
//	if (strToSet == nullptr)
//	{
//		return string();
//	}
//	std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t> converter;
//	std::wstring wstring(strToSet);
//	std::string string = converter.to_bytes(wstring);
//	return string;
//}
//
//json::jobject SceneJsonSerializer::textureJson( std::pair<std::wstring, std::wstring> texturePath)
//{
//	json::jobject textureJson;
//	//if (texturePath.first != nullptr && texturePath.second != nullptr)
//	//{
//	//	
//	///*	std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t> converter;
//	//	std::wstring texturePathName(texturePath.first);
//	//	std::string texturePathNameW = converter.to_bytes(texturePathName);
//
//	//	std::wstring texturePathPath(texturePath.first);
//	//	std::string texturePathPathW = converter.to_bytes(texturePathPath);*/
//
//	//	textureJson["Name"] = wcharToString(texturePath.first);
//	//	textureJson["Filename"] = wcharToString(texturePath.second);
//	//}
//
//	textureJson["Name"] = wcharToString(texturePath.first.c_str());
//	textureJson["Filename"] = wcharToString(texturePath.second.c_str());
//	return textureJson;;
//}
//
//
//
//
//json::jobject SceneJsonSerializer::materialJson(std::pair<std::string, Material*> pair)
//{
//	if (pair.second ==nullptr)
//	{
//		throw exception("Tried to serialize a null material");
//	}
//
//	json::jobject materialJson;
//	materialJson["Name"] = pair.first;
//	materialJson["Blend"].set_boolean(pair.second->blend);
//	if (!pair.second->diffuseTexture.empty())
//	{
//		materialJson["DiffuseTexture"] = wcharToString(pair.second->diffuseTexture.c_str());
//	}
//
//	if (!pair.second->normalTexture.empty())
//	{
//		materialJson["NormalTexture"] = wcharToString(pair.second->normalTexture.c_str());
//	}
//	
//	if (!pair.second->displacementTexture.empty())
//	{
//		materialJson["DisplacementTexture"] = wcharToString(pair.second->displacementTexture.c_str());
//	}
//	
//	materialJson["Shininess"] = pair.second->shininess;
//	materialJson["Diffuse"] = XMFloat3ToVec(pair.second->diffuse);
//	materialJson["Ambient"] = XMFloat3ToVec(pair.second->ambient);
//	materialJson["Specular"] = XMFloat3ToVec(pair.second->specular);
//	return materialJson;
//}
//
//json::jobject SceneJsonSerializer::meshInstanceJson(const MeshInstance* rootInstance, Scene* scene)
//{
//	
//	json::jobject meshInstanceJson;
//	meshInstanceJson["Transform"] = transformJson(rootInstance->transform);
//	std::string materialName;
//	 Material* foundMaterial = nullptr;
//	 meshInstanceJson["Material"] = "";
//	for (auto& it : scene->materials) {
//
//		
//		if (it.second == rootInstance->getMaterial()) {
//			materialName = it.first;
//			foundMaterial = it.second;
//			meshInstanceJson["Material"] = materialName;
//			break;
//		}
//	}
//
//
//
//	std::string foundMeshPath;
//
//	const BaseMesh* foundMesh = nullptr;
//	for (auto& it : scene->meshes) {
//
//	
//		if (it.second.GetMesh() == rootInstance->getMesh()) {
//			foundMeshPath = it.first;
//			foundMesh = it.second.GetMesh();
//			break;
//		}
//	}
//
//	if (foundMesh != nullptr)
//	{
//		meshInstanceJson["BaseMesh"] = foundMeshPath;
//
//	}
//	else
//	{
//		meshInstanceJson["BaseMesh"] = "";
//
//	}
//
//
//	//Perform DFS 
//	auto childrenTransforms = rootInstance->transform.getChildrenTransforms();
//	std::vector<json::jobject> chlidrenList;
//	for (auto childTransform : childrenTransforms)
//	{
//		//Find BaseMesh instance pointer by iterating through allmesh instances and searching for the one with this transform
////		auto found = std::find_if(scene->meshInstances.begin(), scene->meshInstances.end(), [childTransform](MeshInstance* m) { return &(m->transform) == childTransform; });
////		if (found != scene->meshInstances.end())
////		{
////			//childrenMeshInstances.push_back(&(*found));
////			chlidrenList.push_back(SceneJsonSerializer::meshInstanceJson((*found).get(), scene));
////
////		}
////		else
////		{
////			//Referenced transform had no meshinstace
////		}
//	}
//
//	meshInstanceJson["Children"] = chlidrenList;
//
//	return meshInstanceJson;
//}
//
//void SceneJsonSerializer::remove_unnecessary_escapings(std::string& str)
//{
//	auto found = str.find_first_of("\\");
//	while (found != std::string::npos)
//	{
//		str = str.erase(found, 1);
//		found = str.find_first_of("\\");
//	}
//	return;
//}
//
//void SceneJsonSerializer::serializeScene(std::string filepath, Scene* scene)
//{
//	this->_scene = scene;
//	std::string filename(filepath);
//	std::ofstream filestream(filename);
//	if (filestream)
//	{
//		filestream << getJsonString();
//	}
//	filestream.close();
//}
//
//std::string slurp(std::ifstream& in) {
//	std::ostringstream sstr;
//	sstr << in.rdbuf();
//	return sstr.str();
//}
//
//void SceneJsonSerializer::deserializeScene(std::string filepath, Scene* scene)
//{
//	this->_scene = scene;
//	this->_scene->resetResources();
//	//Read all file contents in string
//	std::ifstream ifs(filepath);
//	std::string s = slurp(ifs);
//	//Translate to json
//	json::jobject jsonScene = json::jobject::parse(s);
//
//	//Load FPCamera
//	this->jsonToCamera(json::jobject::parse(jsonScene.get("FPCamera")), this->_scene->getCamera());
//	//Lights -- currently supoortess only one light
//	this->jsonToLight(get_entry(jsonScene, "Lights").array(0), this->_scene->lights[0]);
//
//	//this->_scene->lights.push_back(&this->_scene->white_point_light);
//	//Load Meshes
//	json::jobject meshesJson = get_entry(jsonScene, "Meshes");
//	for (size_t i = 0; i < meshesJson.size(); i++)
//	{
//		std::string name = meshesJson.array(i).as_object()["Name"];
//		std::string filename = meshesJson.array(i).as_object()["Filename"];
//
//		remove_unnecessary_escapings(name);
//		remove_unnecessary_escapings(filename);
//
//		//BaseMesh* mesh = this->jsonToMesh(meshesJson.array(i).as_object(),this->_scene->getDevice());
//		SerializableMesh mesh=this->jsonToMesh(meshesJson.array(i).as_object(), this->_scene->getDevice());
//		mesh.CreateMesh(_scene->getDevice(), _scene->getDeviceContext());
//		this->_scene->meshes.insert({name,mesh });
//	}
//
//	//Load Textures 
//	json::jobject texturesJson = get_entry(jsonScene, "TexturePaths");
//	for (size_t i = 0; i < texturesJson.size(); i++)
//	{
//		//Add ID3D11ShaderResourceView to ID3D11ShaderResourceView in scene		 
//		auto t = this->jsonToTexture(texturesJson.array(i),this->_scene->getTextureManager());
//		this->_scene->textureMap.insert(t);
//	}
//
//	//Load Materials
//	json::jobject materialsJson = jsonScene["Materials"];
//	for (size_t i = 0; i < materialsJson.size(); i++)
//	{
//		Material* mat = this->jsonToMaterial(materialsJson.array(i));
//		this->_scene->materials.insert({ mat->name,mat });
//	}
//
//	//BaseMesh Instances -- Level 1 (Roots)
//	json::jobject meshInstancesJson = get_entry(jsonScene, "MeshInstances");
//	for (size_t i = 0; i < meshInstancesJson.size(); i++)
//	{
//		this->jsonToMeshInstance(meshInstancesJson.array(i));
//	}
//
//	this->_scene->setRootInstances();
//	this->_scene->initRenderCollections();
//	this->_scene->fillRenderCollections();
//	this->_scene->activeMeshInstance = this->_scene->meshInstances[0].get();
//}

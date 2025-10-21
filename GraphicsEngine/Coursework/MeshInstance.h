#pragma once
#include "DXF.h"
#include "SerializableMesh.h"
#include "Transform.h"
#include "Material.h"
class MeshInstance
{
private:
	SerializableMesh _mesh;
	//BaseMesh* _mesh;
	Material* _material;

public:
	//Transform _transform;
	MeshInstance()
	{
		
	}
	MeshInstance(SerializableMesh m):_mesh(m)
	{

	}
	~MeshInstance()
	{
		
	}

	bool operator==(const MeshInstance& other) const
	{
		bool equalTransforms = this->transform == other.transform;
		bool equalMaterials = true;

		//Either material contents are matching or the material poitners are bot null
		if (this->_material != nullptr && other._material != nullptr)
			equalMaterials = (*this->_material) == (*other._material);
		else
			equalMaterials = this->_material == nullptr && other._material == nullptr;

		bool equalMeshes = this->_mesh == other._mesh;
		return equalTransforms && equalMaterials && equalMeshes;
	}



	Transform transform;

	BaseMesh* getMesh() const { return _mesh.GetMesh(); }
	//BaseMesh* getMesh() const { return _mesh; }
	SerializableMesh getSerializableMesh() const { return _mesh; }
	const Material* getMaterial() const { return this->_material; }
	void setMesh( SerializableMesh m) { this->_mesh = m; }
	//void setMesh(BaseMesh* m) { this->_mesh = m; }
void setMaterial(Material* material) { this->_material = material; }
};


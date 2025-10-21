#pragma once
#include "DXF.h"






class Material
{
private:
	/*static Material _default;
	static Material GetDefault() 
	{
		return default
	}*/
	

public:
	Material():blend(false), shininess(0), reflectionFactor(0)
	{
		
	}

	Material* Copy()
	{
		Material* copyOf = new Material();
		copyOf->name = name;

		copyOf->blend = blend;
		copyOf->shininess = shininess;
		copyOf->reflectionFactor = reflectionFactor;

		copyOf->ambient = ambient;
		copyOf->diffuse = diffuse;
		copyOf->specular= specular;

		copyOf->diffuseTexture = diffuseTexture;
		copyOf->normalTexture = normalTexture;
		copyOf->displacementTexture = displacementTexture;

		return copyOf;
	}

	 bool XMFloat3NearEqual(const XMFLOAT3& a, const XMFLOAT3& b, float epsilon = 1e-5f)
{
		 return (std::fabs(a.x - b.x) < epsilon) &&
			 (std::fabs(a.y - b.y) < epsilon) &&
			 (std::fabs(a.z - b.z) < epsilon);
}

	bool operator== (const Material& other)
	{
		return
			blend == other.blend &&
			shininess == other.shininess &&
			reflectionFactor == other.reflectionFactor &&
			diffuseTexture == other.diffuseTexture &&
			normalTexture == other.normalTexture &&
			displacementTexture == other.displacementTexture &&
			XMFloat3NearEqual(ambient, other.ambient) &&
			XMFloat3NearEqual(diffuse, other.diffuse) &&
			XMFloat3NearEqual(specular, other.specular);
	}

	bool blend;
	float shininess;

	std::string name;

	XMFLOAT3 ambient;
	XMFLOAT3 diffuse;
	XMFLOAT3 specular;
	float reflectionFactor;
	XMFLOAT3 emissive;

	std::wstring diffuseTexture;
	std::wstring normalTexture;
	std::wstring displacementTexture;

};

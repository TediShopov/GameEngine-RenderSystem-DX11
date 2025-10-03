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
	Material()
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

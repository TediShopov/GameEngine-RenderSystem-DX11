#pragma once
#include "MeshInstance.h"
#include "DefaultShader.h"
#include <functional>
class RenderItemCollection : public vector<MeshInstance*>
{
public:
	DefaultShader* shaderToRenderWith;

	std::function<void(DefaultShader* shaderToRenderWith)> shaderSetup;
	std::function<void(DefaultShader* shaderToRenderWith, MeshInstance* mesh_instance)> drawInstance;
	RenderItemCollection(DefaultShader* shader,
		std::function<void(DefaultShader* shaderToRenderWith)> shaderSetupFunction,
		std::function<void(DefaultShader* shaderToRenderWith,MeshInstance*)>  drawInstanceFunctionParameter)
		:shaderToRenderWith(shader),drawInstance(drawInstanceFunctionParameter), shaderSetup(shaderSetupFunction)
	{

	}

	RenderItemCollection(DefaultShader* shader) :shaderToRenderWith(shader)
	{

	}
	void DefaultSetup()
	{
		shaderSetup(shaderToRenderWith);
	}

	void DefaultRenderAll(ID3D11DeviceContext* deviceContext)
	{
		for (vector<MeshInstance*>::iterator i = this->begin(); i < this->end(); i++) {
			MeshInstance* instance = *i;
			drawInstance(shaderToRenderWith, instance);
			shaderToRenderWith->render(deviceContext, instance->getMesh()->getIndexCount());
		}
	}
	void SetupShaderAndRenderAllItems(ID3D11DeviceContext* deviceContext)
	{
		DefaultSetup();
		DefaultRenderAll(deviceContext);

	}
};

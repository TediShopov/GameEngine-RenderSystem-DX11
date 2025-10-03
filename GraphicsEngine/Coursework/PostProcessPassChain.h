#pragma once
#include "OrthoMesh.h"
#include "DXF.h"
#include "RenderTexture.h"
class PostProcessPassChain
{
	RenderTexture one;
	RenderTexture two;

	


public:
	RenderTexture* Out;
	RenderTexture* In;
	void Swap()
	{
		RenderTexture* _temp;
		_temp = Out;
		Out = In;
		In = _temp;
	}
	void Reset() {
		Out = &two;
		In = &one;
	}
	PostProcessPassChain(ID3D11Device* device, int width, int height, float nears, float fars)
		:one(device,  width,  height,  nears,  fars),
		two(device, width, height, nears, fars)
	{
		
		Out = &two;
		In = &one;
	}
};


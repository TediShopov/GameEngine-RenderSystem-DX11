#pragma once
#include "AttenuationLight.h"
#include "Transform.h"
class TransformLight :
    public AttenuationLight
{
public:
    Transform transform;
	TransformLight()
	{

	}
};


#pragma once
#include "light.h"
//TODO should this be in default light class?
class AttenuationLight :
    public Light
{
protected:
    float factorConstant, factorLinear, factorQuadratic;
    XMFLOAT4 factorArr;
public:
    AttenuationLight();
    float getConstantFactor();
    float getLinearFactor();
    float getQuadraticFactor();

    //Used to send data to gpu eficiently. [0] = constant, [1]=linear, [2]=quadratic, [3]=padding to 4byte.
    XMFLOAT4 getAttenuationFactorArray();

    void setAttenuation(float factorConstant, float factorLinear, float factorQuadratic);

};


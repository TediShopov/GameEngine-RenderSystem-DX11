#include "AttenuationLight.h"

 AttenuationLight::AttenuationLight() :
    factorConstant(0), factorLinear(0), factorQuadratic(0)
{

}

inline float AttenuationLight::getConstantFactor() { return factorConstant; }

inline float AttenuationLight::getLinearFactor() { return factorLinear; }

inline float AttenuationLight::getQuadraticFactor() { return factorQuadratic; }


//Used to send data to gpu eficiently. [0] = constant, [1]=linear, [2]=quadratic, [3]=padding to 4byte.

 XMFLOAT4 AttenuationLight::getAttenuationFactorArray()
{
    factorArr.x = factorConstant;
    factorArr.y = factorLinear;
    factorArr.z = factorQuadratic;
    factorArr.w = 0;
    return factorArr;
}

 void AttenuationLight::setAttenuation(float factorConstant, float factorLinear, float factorQuadratic)
{
    this->factorConstant = factorConstant;
    this->factorLinear = factorLinear;
    this->factorQuadratic = factorQuadratic;
}

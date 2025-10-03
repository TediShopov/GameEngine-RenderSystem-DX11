// Light pixel shader
// Calculate l_diffuse lighting for a single directional light (also texturing)

#include "Light.hlsli"
#include "ShadowMaps.hlsli"
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);


Texture2D normalTexture : register(t4);
SamplerState normalSampler : register(s2);


SamplerState shadowSampler : register(s1);
Texture2D depthMapTexture1 : register(t1);
Texture2D depthMapTexture2 : register(t2);
Texture2D depthMapTexture3 : register(t3);


cbuffer LightBuffer : register(b0)
{

	Light lights[3];
	
};

cbuffer MaterialBuffer  : register(b1)
{
	float4 m_ambient;
	float4 m_diffuse;
	float4 m_specular;
};

cbuffer FogParameters : register(b2)
{
	float4 camPos;
	float4 fogColor;
	float fogStart;
	float fogEnd;
	float fogDensity;
};


cbuffer DebugData : register(b4)
{
    int debug;
};

ColorComponents multiplyColorsByMaterial(ColorComponents colors)
{
	colors.ambient *= m_ambient;
	colors.specular *= m_specular;
	colors.diffuse *= m_diffuse;
    return colors;
}

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
	float4 lightViewPostion1 : TEXCOORD2;
	float4 lightViewPostion2 : TEXCOORD3;
	float4 lightViewPostion3 : TEXCOORD4;
	float3x3 TBN : TBNMat;

};

float calculateExpSquaredFogFactor(float3 worldPosition)
{
	float distanceToFrag = length(worldPosition - camPos);
	float distRatio = 4.0 * distanceToFrag / fogEnd;
	return exp(-distRatio * fogDensity * distRatio * fogDensity);
}


float4 main(InputType input) : SV_TARGET
{
    //return float4(1, 0, 0, 1);
	float3 newNormal= normalTexture.Sample(normalSampler, input.tex);
	// obtain normal from normal map in range [0,1]
	
	// transform normal vector to range [-1,1]
	newNormal = normalize(newNormal * 2.0 - 1.0);
	newNormal = normalize(mul(newNormal,input.TBN));
	input.normal = newNormal;

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	float4 textureColour = texture0.Sample(sampler0, input.tex);

	ColorComponents colors;
	colors.ambient = float4(0, 0, 0, 1);
	colors.diffuse = float4(0, 0, 0, 1);
	colors.specular = float4(0, 0, 0, 1);

	ColorComponents directionalColors = calculateDirectional(lights[2], camPos.xyz, input.worldPosition, input.normal);
	ColorComponents point1Colors = calculatePointLight(lights[0], camPos.xyz, input.worldPosition, input.normal);
	ColorComponents spotLightColors = calculateSpotLight(lights[1], camPos.xyz, input.worldPosition, input.normal);

	colors = addComponents(colors, directionalColors);
	colors = addComponents(colors, point1Colors);
	colors = addComponents(colors, spotLightColors);

    ParallelSplitShadowMapData shadowMapData;
    shadowMapData.lightVP1 = input.lightViewPostion1;
    shadowMapData.lightVP2= input.lightViewPostion2;
    shadowMapData.lightVP3 = input.lightViewPostion3;
    shadowMapData.depthTexture1 = depthMapTexture1;
    shadowMapData.depthTexture2 = depthMapTexture2;
    shadowMapData.depthTexture3 = depthMapTexture3;
    shadowMapData.shadowMapSampler = shadowSampler;
    int shadowMapIndex = getShadowMapIndex(shadowMapData) ;

	float4 pixelColor = colors.ambient * textureColour;
	//If index is negative apply shadow
	if(shadowMapIndex == -1)
	{
        colors = multiplyColorsByMaterial(colors);
        pixelColor = colors.ambient*textureColour;
    }
	//if not the index indicates on which shadow map was tested
	else
	{
		//Debug Variant
		if(debug == 255)
		{
            if (shadowMapIndex == 0)
                return float4(1, 0, 0, 1);
            if (shadowMapIndex == 1)
                return float4(1, 1, 0, 1);
            if (shadowMapIndex == 2)
                return float4(1, 0, 1, 1);
		}
		else
		{
            colors = multiplyColorsByMaterial(colors);
            pixelColor = getLightColor(colors) * textureColour;
			
		}
	}
	 

	float fogFactor = calculateExpSquaredFogFactor(input.worldPosition);


	if (fogFactor != 0)
	{
		return lerp(fogColor, pixelColor, fogFactor);
	}
	return fogColor;
}




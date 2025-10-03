// Light pixel shader
// Calculate l_diffuse lighting for a single directional light (also texturing)
#include "Light.hlsli"
#include "ScreenSpaceReflections.hlsli"
#include "ShadowMaps.hlsli"
SamplerState sampler0 : register(s0);
Texture2D texture0 : register(t0);

//Sampler and texture for all the depth maps
SamplerState shadowSampler : register(s1);
Texture2D depthMapTexture1 : register(t1);
Texture2D depthMapTexture2 : register(t2);
Texture2D depthMapTexture3 : register(t3);

//Skybox cube and sampler
TextureCube skymap : register(t4);
SamplerState skymapSampler : register(s2);

//Color and depth textures to be used for SSR
Texture2D colorTexture : register(t5);
Texture2D depthTexture : register(t6);


cbuffer MultipleLights : register(b0)
{

	Light lights[3];
};

cbuffer MaterialBuffer  : register(b1)
{
	float4 m_ambient;
	float4 m_diffuse;
	float4 m_specular;
	float reflectionFactor;
	float3 emissive;
};

cbuffer FogParameters : register(b2)
{

	float4 camPos;
	float4 fogColor;
	float fogStart;
	float fogEnd;
	float fogDensity;
};

cbuffer SSRCameraData : register(b3)
{
    SSRCameraData camera_data;
    SSRParameters ssr_parameters;
	
};
cbuffer DebugData : register(b4)
{
    int debug;
};
struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
	float4 lightViewPostion1 : TEXCOORD2;
	float4 lightViewPostion2 : TEXCOORD3;
	float4 lightViewPostion3 : TEXCOORD4;

};



float calculateExpSquaredFogFactor(float3 worldPosition)
{
	float distanceToFrag = length(worldPosition - camPos);
	float distRatio = 4.0 * distanceToFrag / fogEnd;
	return exp(-distRatio * fogDensity* distRatio* fogDensity);
}

float4 blendEnvironmentReflection(float4 baseColor, float3 worldPosition, float3 worldNormal)
{
    // Sample reflected color from the cube map
    float4 reflectedColor = skymap.Sample(skymapSampler, getReflectionVector(
	camera_data.cameraPosition,worldPosition,worldNormal));
		// Mix base color with reflected light
    float4 finalColor = lerp(baseColor, reflectedColor, 
	float4(reflectionFactor,reflectionFactor,reflectionFactor,1));
    return finalColor;

	
}




ColorComponents multiplyColorsByMaterial(ColorComponents colors)
{
	colors.ambient *= m_ambient;
	colors.specular *= m_specular;
	colors.diffuse *= m_diffuse;
    return colors;
}

float4 main(InputType input) : SV_TARGET
{

    float4 pixelColor = (0, 0, 0, 0);
    if (emissive.r >= 0.0f)
    {
        pixelColor.xy = emissive.xy;
        return pixelColor * emissive.b;
    }
	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	float4 textureColour = texture0.Sample(sampler0, input.tex);

	
	 ColorComponents colors;
	 colors.ambient = float4(0, 0, 0, 1);
	 colors.diffuse = float4(0, 0, 0, 1);
	 colors.specular = float4(0, 0, 0, 1);

	 ColorComponents directionalColors= calculateDirectional(lights[2], camPos.xyz, input.worldPosition, input.normal);
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
	 
	if(ssr_parameters.useSSR != 0)
	{
		//Get the reflected color from the screen space reflections algorithm
        float4 ssrColor = screenSpaceReflections(
			input.worldPosition, 
		    input.normal,
		    camera_data,
		    ssr_parameters,
		    depthTexture,
		    colorTexture,
		    shadowSampler,
		    skymap,
		    skymapSampler
		);

		//Compute the reflection coefficient based (Fresnel Effect)
        float3 viewDir = normalize(camera_data.cameraPosition - input.worldPosition);
        float reflectionCoefficient = 
			computeReflectionCoefficient(viewDir, input.normal, reflectionFactor);

		//Linearly interpolate between color and reflection based on coefficient
        pixelColor = lerp(pixelColor, ssrColor, 1-reflectionCoefficient);
    }
	

	 float fogFactor= calculateExpSquaredFogFactor(input.worldPosition);
	//Apply linear fog
	if (fogFactor!=0)
	{
		return lerp(fogColor, pixelColor, fogFactor);
	}
	return pixelColor * emissive.b;
	
}




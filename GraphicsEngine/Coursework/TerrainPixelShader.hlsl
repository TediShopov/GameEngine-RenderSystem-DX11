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


Texture2D terrainTexture1 : register(t7);
Texture2D terrainTexture2 : register(t8);
Texture2D terrainTexture3 : register(t9);
Texture2D terrainNormalTexture : register(t10);


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
	float3 padding;
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
cbuffer TerrainParams : register(b5)
{
    float rangeOneHeight;
    float3 rangeOneColor;
    float rangeTwoHeight;
    float3 rangeTwoColor;
    float rangeThreeHeight;
    float3 rangeThreeColor;
    float mamplitude;

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
    float3 localPosition : TEXCOORD5;
	float4 color : COLOR;
	float3x3 TBN : TBNMat;

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

bool IsTriangleBackfacing(float3 worldPos,float3 faceNormal, float3 cameraPos) {
    
    
    // Compute view direction (camera to triangle)
    float3 viewDir = normalize(cameraPos - worldPos);
    
    // If dot product > 0, triangle is backfacing
    return (dot(faceNormal, viewDir) > 0.0);
}

float4 main(InputType input) : SV_TARGET
{
	if(debug > 0)
    {
        float4 normalColor = float4((input.normal + 1) / 2,1);
		return normalColor;

    }
	float4 pixelColor;
	float3 newNormal= terrainNormalTexture.Sample(shadowSampler, input.tex);

	newNormal = normalize(newNormal * 2.0 - 1.0);
	newNormal = normalize(mul(newNormal,input.TBN));
	input.normal = newNormal;

	//Range is from -amplitude to + amplitude
    float height = (input.worldPosition.y / mamplitude) +0.5;
    float4 color;


    float d1 =  1- abs(height - rangeOneHeight);
    float d2 =  1- abs(height - rangeTwoHeight);
    float d3 = 1- abs(height - rangeThreeHeight);

	if(height <= rangeOneHeight)
		color = float4(rangeOneColor, 1);
	else if(height <= rangeTwoHeight)
		color = lerp(float4(rangeOneColor, 1), float4(rangeTwoColor, 1), float4(d2, d2, d2, 1));
	else if(height <= rangeThreeHeight)

		color = lerp(float4(rangeTwoColor, 1), float4(rangeThreeColor, 1), float4(d3, d3, d3, 1));
	else 
        color = float4(rangeThreeColor, 1);

    input.color = color;




        float4 textureColour;
	if(height <= rangeOneHeight)
		textureColour = terrainTexture1.Sample(sampler0, input.tex);
	else if(height <= rangeTwoHeight)
		textureColour = lerp(
		terrainTexture1.Sample(sampler0,input.tex),
		terrainTexture2.Sample(sampler0,input.tex),
		float4(d2, d2, d2, 1));
	else if(height <= rangeThreeHeight)
		textureColour = lerp(
		terrainTexture2.Sample(sampler0,input.tex),
		terrainTexture3.Sample(sampler0,input.tex),
		float4(d3, d3, d3, 1));
	else 
		textureColour = terrainTexture3.Sample(sampler0, input.tex);

    //return input.color;
	
	 ColorComponents colors;
	 colors.ambient = float4(0, 0, 0, 1);
	 colors.diffuse = float4(0, 0, 0, 1);
	 colors.specular = float4(0, 0, 0, 1);

	 ColorComponents directionalColors= calculateDirectional(lights[2], camPos.xyz, input.worldPosition, input.normal);
	 ColorComponents point1Colors = calculatePointLight(lights[0], camPos.xyz, input.worldPosition, input.normal);
	 ColorComponents spotLightColors = calculateSpotLight(lights[1], camPos.xyz, input.worldPosition, input.normal);


	//pixelColor = getLightColor(colors) * textureColour;
	pixelColor =   textureColour;
    //return pixelColor;
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
        colors = addComponents(colors, point1Colors);
        colors = addComponents(colors, spotLightColors);
        colors = multiplyColorsByMaterial(colors);
        pixelColor = getLightColor(colors)  * textureColour;
    }
	//if not the index indicates on which shadow map was tested
	else
	{
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
            colors = addComponents(colors, directionalColors);
            colors = addComponents(colors, point1Colors);
            colors = addComponents(colors, spotLightColors);
            colors = multiplyColorsByMaterial(colors);
            pixelColor = getLightColor(colors) * textureColour;
        }
	}
	 float fogFactor= calculateExpSquaredFogFactor(input.worldPosition);
	//Apply linear fog
	if (fogFactor!=0)
	{
		return lerp(fogColor, pixelColor, fogFactor);
	}
	return pixelColor;
}




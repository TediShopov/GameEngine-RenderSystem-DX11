// Light pixel shader
// Calculate l_diffuse lighting for a single directional light (also texturing)
#include "Light.hlsli"
#include "ScreenSpaceReflections.hlsli"
#include "ShadowMaps.hlsli"
#include "FastNoiseLite.hlsli"
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
cbuffer LinearColorGradient : register(b5)
{
	// contains the color in the first 3 values and STOP point in the gradient in the fourt
    float4 lcolors[8];
    float power;
    float noiseAmplitude;
    float noiseFrequency;
    float normalStrength;
    //float paddingAA;

	
	
};
struct NoiseParams
{
    float amplitude;
    float frequency;
    //float2 padding;
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
	float3x3 TBN : TBNMat;
	bool ifFront : SV_IsFrontFace;

};


bool IsNormalBackfacing(float3 worldPos,float3 faceNormal, float3 cameraPos) {
    
    
    // Compute view direction (camera to triangle)
    float3 viewDir = normalize(cameraPos - worldPos);
    
    // If dot product > 0, triangle is backfacing
    return (dot(faceNormal, viewDir) > 0.0);
}

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


float getNoise(fnl_state noise, float2 vec )
{
	return (fnlGetNoise2D(noise, vec.x * noiseFrequency, vec.y * noiseFrequency)) * noiseAmplitude;
	
}
float2 computeGradient( fnl_state noise, float2 uv, float eps = 0.01)
{
    float n = fnlGetNoise2D(noise, uv.x  * noiseFrequency, uv.y * noiseFrequency) * noiseAmplitude;
    float n_x = fnlGetNoise2D(noise, (uv.x + eps) * noiseFrequency, uv.
    y * noiseFrequency) * noiseAmplitude;
    float n_y = fnlGetNoise2D(noise, uv.x * noiseFrequency, (uv.y + eps) * noiseFrequency) * noiseAmplitude;

    float2 gradient = float2(
    (n_x - n) / eps,
    (n_y - n) / eps);
    return gradient;

}
float3 gradientToNormal(float2 gradient, float strength)
{
     // Flip gradient direction (so normal points "out" from the surface)
    float2 flippedGradient = -gradient * strength;
    
    // Create tangent-space normal (Z = 1, X/Y = gradient)
    float3 normal = normalize(float3(flippedGradient.x, flippedGradient.y, 1.0));
    return normal;

    
}
float3 computeNoiseNormal(float2 uv,fnl_state noiseState,  float eps = 0.01 )
{
    float2 grad = computeGradient(noiseState, uv,eps);
    return gradientToNormal(grad, normalStrength);
    
}


float4 computeLinearGradient(float t,float4 colors[8])
{
	//Remainder used for testing 
    //t = fmod(t,1);
    for (int i = 0; i < 7; i++)
    {
        if (t >= colors[i].a && t <= colors[i+1].a)
        {
            float segmentT = (t - colors[i].a) / (colors[i+1].a - colors[i].a);
            return float4(lerp(colors[i].xyz, colors[i + 1].xyz, segmentT), 1.0f);
        }
    }
    
    return colors[7]; // White (fallback)
}



ColorComponents multiplyColorsByMaterial(ColorComponents colors)
{
	colors.ambient *= m_ambient;
	colors.specular *= m_specular;
	colors.diffuse *= m_diffuse;
    return colors;
}

float4 getDebugTexture(InputType input)
{
    fnl_state noise = fnlCreateState(1337);
    noise.noise_type = FNL_NOISE_PERLIN;
    return getNoise(noise, input.tex);
    //return float4(computeNoiseNormal(input.tex, noise, 0.01).xyz, 1);
	
}
float3 getTBNNormal(InputType input, float3 baseNormal)
{

    // Perturb the base normal in tangent space (optional)
    //return mul(baseNormal, transpose(input.TBN));
    return mul(baseNormal, input.TBN);
}

float4 getDebugNormalColor(InputType input)
{
    return float4((input.normal.xyz + 1)*0.5, 1);
	
}



float3 computeWorldNormal(InputType input,float2 uv,fnl_state noiseState,  float eps = 0.01 )
{
    float3 normal =computeNoiseNormal(uv, noiseState, eps);
    normal = getTBNNormal(input,normal);
    return normal;
    
}
float4 getIridiscenceColor(InputType input)
{

	
    float3 lightVector = lights[2].direction;
    //float3 positionToVertex = input.worldPosition - camera_data.cameraPosition;
    float3 positionToVertex =   camera_data.cameraPosition- input.worldPosition;
    positionToVertex = normalize(positionToVertex);

		//Sample normal from noise
		// Create and configure noise state
    fnl_state noise = fnlCreateState(1337);
    noise.noise_type = FNL_NOISE_PERLIN;

    //Normal in world space after sampling from noise and applying TBN
    input.normal = computeWorldNormal(input,input.tex,noise, 0.01);
		
    //Reflect the viewing vector
    float3 reflected = reflect(positionToVertex, input.normal);
		
		
	//Dot from camera to light source
    float NDotL = dot(lightVector, reflected);

	//Change range form -1 to 1 to 0 to 1
    NDotL = (NDotL + 1) / 2.0f;
    NDotL = NDotL * power;

    return computeLinearGradient(NDotL, lcolors);
}


float4 main(InputType input) : SV_TARGET
{

	 float4 pixelColor;
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

            if (input.ifFront == false)
            {
                input.normal = -input.normal;
                //return getIridiscenceColor(input);
                colors.diffuse *= getIridiscenceColor(input);
                //colors = multiplyColorsByMaterial(colors);
                pixelColor = getLightColor(colors) * textureColour;


            }
            else
            {
            colors = multiplyColorsByMaterial(colors);
            pixelColor = getLightColor(colors) * textureColour;
                
            }
			
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
	return pixelColor;
	
}




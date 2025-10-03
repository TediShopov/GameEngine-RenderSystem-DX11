#include "FastNoiseLite.hlsli"

Texture2D destroyedTerrainTexture: register(t0);
SamplerState domainSampler : register(s0);
Texture2D displacementTexture: register(t1);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[3];
    matrix lightProjectionMatrix[3];
};

struct NoiseParams
{
    float amplitude;
    float frequency;
    //float2 padding;
};
cbuffer NoiseParams : register(b1)
{
    //Wave 1
    float amplitude;
    float frequency;
    int octaves;
    float lucanarity;
    float gain;
    //float padding;

    //Wave 2;
    NoiseParams NoiseOne;
    NoiseParams NoiseTwo;
    NoiseParams NoiseThree;
    NoiseParams NoiseFour;
    NoiseParams NoiseFive;

};

cbuffer DisplacementAndNormalParams : register(b2)
{
    //A multiplier to convert from normalized coord to uv
    float uvDensity;
    float displacementStrength;
    float EPS;
    float padding;

};


struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};




struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos1: TEXCOORD2;
    float4 lightViewPos2: TEXCOORD3;
    float4 lightViewPos3 : TEXCOORD4;
    float3 localPosition : TEXCOORD5;
    float3 color : COLOR;
    float3x3 TBN : TBNMat;
};


float getNoise(fnl_state noise, float3 vec, NoiseParams noise_params)
{
	return (fnlGetNoise2D(noise, vec.x * noise_params.frequency, vec.z * noise_params.frequency)) * noise_params.amplitude;
	
}

float getfBMTerrainHeight(float3 vec,fnl_state noise)
{

    float height = 0;
    float currentFrequency = frequency;
    float currentAmplitude = amplitude;

    // Compute the output from the fBM noise
    for (int k = 0; k < octaves; k++)
    {
        height += (fnlGetNoise2D(noise, vec.x * currentFrequency, vec.z * currentFrequency)) * currentAmplitude;
        currentFrequency *= lucanarity;
        currentAmplitude *= gain;
    }

    //Multipy with other noise layer
	height *= getNoise(noise, vec, NoiseOne);
    return height;
}

float3 computeNormal(float3 position, fnl_state noise) {
    float3 dx = float3(EPS, 0, 0);
    float3 dz = float3(0, 0, EPS);

    // Sample the height at three nearby points
    float hL = getfBMTerrainHeight(position - dx, noise);
    float hR = getfBMTerrainHeight(position + dx, noise);
    float hD = getfBMTerrainHeight(position - dz, noise);
    float hU = getfBMTerrainHeight(position + dz, noise);

    // Compute tangent vectors
    float3 tangentX = normalize(float3(2 * EPS, hR - hL, 0));  // X-direction
    float3 tangentZ = normalize(float3(0, hU - hD, 2 * EPS));  // Z-direction

    // Compute normal using the cross-product
    return normalize(cross(tangentZ, tangentX));
}
float2 computeTexFromXZ(float3 localPos)
{
    float2 texUV = (localPos.xz + 250) / 500.0f * uvDensity;
    return texUV;
}

float3 computeGlobal(float3 localPos,fnl_state noise)
{
    //sample from the world-destroyed texture
    float2 worldUV = (localPos.xz + 250.0f) / 500.0f;
    float destroyFactor = 1 - destroyedTerrainTexture.SampleLevel(domainSampler, worldUV, 0).r;

    

    //get the terrain height from the noise
    float hNoise = getfBMTerrainHeight(localPos, noise) * destroyFactor;
    float3 baseNormal = computeNormal(localPos,noise);

    //modify the global posiiton by the noise height
    float3 globalPosition = localPos;
    globalPosition.y = hNoise;

    //compute normal from the noise dx,dz

    //sample from displacement texture and push along direction
    float tDisp = displacementTexture.SampleLevel(domainSampler,computeTexFromXZ(localPos) , 0).r;
    globalPosition += baseNormal * tDisp * displacementStrength;
    return globalPosition;
    
}


[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
     OutputType o;

    float3 p01 = lerp(patch[0].position.xyz, patch[1].position.xyz, uvwCoord.y);
    float3 p23 = lerp(patch[3].position.xyz, patch[2].position.xyz, uvwCoord.y);
    float3 localPos = lerp(p01, p23, uvwCoord.x);
    o.localPosition = localPos;

    
    o.tex = computeTexFromXZ(localPos);

    fnl_state noise = fnlCreateState();
    noise.seed       = 1337;
    noise.noise_type = FNL_NOISE_PERLIN;

    float3 dx = float3(EPS,0,0), dz = float3(0,0,EPS);

    float3 globalCoord = computeGlobal(localPos,  noise);
    float3 gL = computeGlobal(localPos - dx, noise);
    float3 gR  = computeGlobal(localPos + dx,noise);
    float3 gD  = computeGlobal(localPos - dz, noise);
    float3 gU = computeGlobal(localPos + dz, noise);

    // finite‐difference tangents in world‐space
    float3 T = normalize(float3(2 * EPS, (gR - gL).y, 0));
    float3 B = normalize(float3(0, (gU - gD).y, 2 * EPS));
    float3 N = normalize(cross(B, T));

    // Gram–Schmidt orthonormalise
    T = normalize(T - N * dot(N, T));
    B = cross(N, T);

    o.TBN = float3x3(T, B, N);

    // world‐space position & normal
    float4 worldPos4 = mul(float4(globalCoord,1), worldMatrix);

    o.worldPosition = worldPos4.xyz;
    o.normal        = normalize(mul(N, (float3x3)worldMatrix));
    o.position  = mul(worldPos4, viewMatrix);
    o.position  = mul(o.position, projectionMatrix);

    float4 lp = mul(worldPos4, lightViewMatrix[0]);
    o.lightViewPos1  = mul(lp, lightProjectionMatrix[0]);
    lp               = mul(worldPos4, lightViewMatrix[1]);
    o.lightViewPos2  = mul(lp, lightProjectionMatrix[1]);
    lp               = mul(worldPos4, lightViewMatrix[2]);
    o.lightViewPos3  = mul(lp, lightProjectionMatrix[2]);


    return o;
}


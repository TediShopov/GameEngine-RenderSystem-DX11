
cbuffer ScreenResolution : register(b0)
{
    float width;
    float height;
    float padding[2];

};
cbuffer ViggneteMask : register(b1)
{
    float vInnerRadius;
    float vOuterRadius;
    float vPower;

}

Texture2D shaderTexture : register(t0);


SamplerState SampleType : register(s0);

//float calculateVignette(float2 originalTex, float innerRadius, float outerRadius)
//{
//    float2 relTexCoords = originalTex - 0.5f;
//    return  smoothstep(outerRadius, innerRadius, length(relTexCoords));
//}


float calculateVignette(float2 originalTex, float innerRadius, float outerRadius)
{
    float2 relTexCoords = originalTex - 0.5f;
    return  smoothstep(innerRadius, outerRadius, length(relTexCoords));
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
    
  
    float4 originalPixel = shaderTexture.Sample(SampleType, input.tex);

    float2 modifiedTex = input.tex;

    

    //Achieves magnifying effect
    modifiedTex -= 0.5f;
    modifiedTex *= 0.80f;
    modifiedTex += 0.5f;
  

    float4 magnifiedPixel = shaderTexture.Sample(SampleType, modifiedTex);
    float revAr = height / width;
    input.tex -= 0.5f;
    input.tex.y *= revAr;

    input.tex += 0.5f;

    float vignette = calculateVignette(input.tex, vInnerRadius, vOuterRadius);
    
    return vignette * originalPixel + (1 - vignette) * magnifiedPixel;
   return lerp(originalPixel, magnifiedPixel, vignette);

}
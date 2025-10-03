
cbuffer ViggneteMask : register(b0)
{
    float vInnerRadius;
    float vOuterRadius;
    float vPower;

}

cbuffer ScreenResolution : register(b1)
{
    float width;
    float height;
    //float padding[2];
};
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ThresholdParameters : register(b2)
{
    float threshold;
    float3 padding;
};


struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
    float3 hdrColor = shaderTexture.Sample(SampleType, input.tex);

    //Probably a better representation of real life luminance 
    float3 luminance = dot(hdrColor, float3(0.2126, 0.7152, 0.0722));

    //Thresholdign
    float3 birhgtColor = hdrColor * float3(luminance > threshold);
    
    return float4(birhgtColor, 1.0);

}
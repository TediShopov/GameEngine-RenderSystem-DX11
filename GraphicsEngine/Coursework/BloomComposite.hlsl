
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

cbuffer BloomData : register(b2)
{
    float bloomIntensity;
    float exposure;
    //float padding[2];
};
Texture2D shaderTexture : register(t0);
Texture2D extractedBlurredTexture : register(t1);
SamplerState SampleType : register(s0);


struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


//https://64.github.io/tonemapping/
float3 aces_approx(float3 v)
{
    v *= 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}

float4 main(InputType input) : SV_TARGET
{
     // Sample original HDR color
    float3 hdrColor = shaderTexture.Sample(SampleType, input.tex).rgb;
    
    // Sample bloom
    float3 bloomColor = extractedBlurredTexture.Sample(SampleType, input.tex).rgb;
    
    // Combine
    //float3 c = hdrColor + bloomColor * bloomIntensity;
    float3 c = hdrColor + bloomColor * 1;
    
    // Simple Reinhard
    //c = c / (c + float3(1.0, 1.0, 1.0));

    // Extended Reinhard
    //c = (c * (1 + c / ()));

    // ACES approximation
    c = aces_approx(c);
    return float4(c, 1.0);
}

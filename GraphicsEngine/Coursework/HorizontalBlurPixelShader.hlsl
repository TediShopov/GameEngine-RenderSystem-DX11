Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

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

float calculateVignette(float2 originalTex, float innerRadius, float outerRadius)
{
    float2 relTexCoords = originalTex - 0.5f;
    return  smoothstep(innerRadius, outerRadius, length(relTexCoords));
}


float4 main(InputType input) : SV_TARGET
{
     float weight0, weight1, weight2, weight3, weight4;
    float4 colour;

    // Create the weights that each neighbor pixel will contribute to the blur.
    weight0 = 0.382928;
    weight1 = 0.241732;
    weight2 = 0.060598;
    weight3 = 0.005977;
    weight4 = 0.000229;

    // Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);


 
    float texelSize = 1.0f / width;

    float texPower = texelSize * vPower;

    // Add the horizontal pixels to the colour by the specific weight of each.
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texPower * -4.0f, 0.0f)) * weight4;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texPower * -3.0f, 0.0f)) * weight3;

    colour += shaderTexture.Sample(SampleType, input.tex + float2(texPower * -2.0f, 0.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texPower * -1.0f, 0.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex) * weight0;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texPower * 1.0f, 0.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texPower * 2.0f, 0.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texPower * 3.0f, 0.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texPower * 4.0f, 0.0f)) * weight4;


    // Set the alpha channel to one.
    colour.a = 1.0f;


    float vignette = calculateVignette(input.tex, vInnerRadius, vOuterRadius);


    float4 textureColor = shaderTexture.Sample(SampleType, input.tex);

    colour = vignette * colour + (1 - vignette) * textureColor;
    return colour;
}
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

Texture2D gizmosTexture : register(t1);
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

float4 main(InputType input) : SV_TARGET
{
    //float4 gizmoTexColor = gizmosTexture.Sample(gizmos)
    float4 inTex = shaderTexture.Sample(SampleType, input.tex);
    float4 gizmo = gizmosTexture.Sample(SampleType, input.tex);
    
    return float4(lerp(inTex.xyz, gizmo.xyz, float3(gizmo.a, gizmo.a, gizmo.a)),1);
    return float4(1, 0, 1, 0);
}

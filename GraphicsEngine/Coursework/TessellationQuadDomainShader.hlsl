// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
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

//struct OutputType
//{
//    float4 position : SV_POSITION;
//    float2 tex : TEXCOORD0;
//    float3 normal : NORMAL;
//    float3 worldPosition : TEXCOORD1;
//};


struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    
};
[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    OutputType output;

   

    float3 v1 = lerp(patch[0].position.xyz, patch[1].position.xyz, uvwCoord.y);
    float3 v2 = lerp(patch[3].position.xyz, patch[2].position.xyz, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);
    //output.tex = input.tex;
    
  /* output.tex = input.tex;

    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    output.worldPosition = mul(input.position, worldMatrix).xyz;*/

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Send the input color into the pixel shader.

    return output;
}


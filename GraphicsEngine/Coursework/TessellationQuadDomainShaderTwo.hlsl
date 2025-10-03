// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
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
    float4 lightViewPos : TEXCOORD2;
};
[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition, normal;
    float2 tex;
    OutputType output;



    float3 v1 = lerp(patch[0].position.xyz, patch[1].position.xyz, uvwCoord.y);
    float3 v2 = lerp(patch[3].position.xyz, patch[2].position.xyz, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);
    // output.worldPosition = mul(inputposition, worldMatrix).xyz;

    float3 n1 = lerp(patch[0].normal.xyz, patch[1].normal.xyz, uvwCoord.y);
    float3 n2 = lerp(patch[3].normal.xyz, patch[2].normal.xyz, uvwCoord.y);
    normal = lerp(n1, n1, uvwCoord.x);

    float3 u = lerp(patch[0].normal.xyz, patch[1].normal.xyz, uvwCoord.y);
    float3 v = lerp(patch[3].normal.xyz, patch[2].normal.xyz, uvwCoord.y);
    tex.x = u; tex.y = v;
    /* float3 t1 = lerp(patch[0].normal.xyz, patch[1].normal.xyz, uvwCoord.y);
    float3 t2 = lerp(patch[3].normal.xyz, patch[2].normal.xyz, uvwCoord.y);
    normal = lerp(n1, n1, uvwCoord.x);*/
    output.tex = tex;



    output.normal = mul(normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    output.worldPosition = mul(vertexPosition, worldMatrix).xyz;

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);


    // Calculate the position of the vertice as viewed by the light source.
    output.lightViewPos = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

    // Send the input color into the pixel shader.

    return output;
}

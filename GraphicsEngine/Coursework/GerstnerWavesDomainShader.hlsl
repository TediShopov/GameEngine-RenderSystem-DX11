//// Tessellation domain shader
//// After tessellation the domain shader processes the all the vertices
//
//cbuffer MatrixBuffer : register(b0)
//{
//    matrix worldMatrix;
//    matrix viewMatrix;
//    matrix projectionMatrix;
//    matrix lightViewMatrix[3];
//    matrix lightProjectionMatrix[3];
//
//};
//
//struct ConstantOutputType
//{
//    float edges[4] : SV_TessFactor;
//    float inside[2] : SV_InsideTessFactor;
//};
//
//struct InputType
//{
//    float4 position : POSITION;
//    float2 tex : TEXCOORD0;
//    float3 normal : NORMAL;
//};
//
//struct WaveParams
//{
//    float time;
//    float wavelength;
//    float steepness;
//    float speed;
//    float2 XZdir;
//    float2 padding;
//};
//
//
//
//cbuffer WaveParametersBuffer : register(b1)
//{
//    WaveParams waveParams[3];
//};
//
//
////const float _PI_= 3.14159265358979323846;
////Returns the positon ofsset of the vertex, and changes tanges and bitangent vectors
//float3 GerstnerWave(WaveParams wvParams, float3 inputPoint, inout float3 tangent, inout float3 bitangent)
//{
//    float2 dir = normalize(wvParams.XZdir);
//
//    float k = 2.0f * 3.14159265358979323846f / wvParams.wavelength;
//    float c = sqrt(9.8 / k);
//    float f = k * (dot(dir, inputPoint.xz) -c * wvParams.time);
//
//    float a = wvParams.steepness / k;
//
//    //Calculate Point offset that is to be returned
//    float3 pointOffset;
//    pointOffset.x = dir.x * (a * cos(f));
//    pointOffset.y = a * sin(f);
//    pointOffset.z = dir.y * (a * cos(f));
//
//    tangent += float3(
//        -dir.x * dir.x * (wvParams.steepness * sin(f)),
//        dir.x * (wvParams.steepness * cos(f)),
//        -dir.x * dir.y * (wvParams.steepness * sin(f))
//        );
//
//
//    bitangent += float3(
//        -dir.x * dir.y * (wvParams.steepness * sin(f)),
//        dir.y * (wvParams.steepness * cos(f)),
//        -dir.y * dir.y * (wvParams.steepness * sin(f))
//        );
//
//    return pointOffset;
//}
//
//
//struct OutputType
//{
//    float4 position : SV_POSITION;
//    float2 tex : TEXCOORD0;
//    float3 normal : NORMAL;
//    float3 worldPosition : TEXCOORD1;
//    float4 lightViewPos1 : TEXCOORD2;
//    float4 lightViewPos2 : TEXCOORD3;
//    float4 lightViewPos3 : TEXCOORD4;
//
//};
//[domain("quad")]
//OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
//{
//    float3 vertexPosition, normal;
//    float2 tex;
//    OutputType output;
//
//
//
//    float3 v1 = lerp(patch[0].position.xyz, patch[1].position.xyz, uvwCoord.y);
//    float3 v2 = lerp(patch[3].position.xyz, patch[2].position.xyz, uvwCoord.y);
//    vertexPosition = lerp(v1, v2, uvwCoord.x);
//
//    float2 uv1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
//    float2 uv2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
//    tex = lerp(uv1, uv2, uvwCoord.x);
//  
//    output.tex = tex;
//
//  
//    //output.position = mul(float4(vertexPosition, 1), worldMatrix);
//
//    //float3 startingPoint = output.position.xyz;
//
//    float3 startingPoint = vertexPosition;
//
//    float3 tangent = float3(1, 0, 0);
//    float3 binormal = float3(0, 0, 1);
//
//    //float3 tangent = 0;
//    //float3 binormal = 0;
//
//
//    //normalize steepnes of waves to prevent looping
//  
//    float3 p = startingPoint;
//    p += GerstnerWave(waveParams[0], startingPoint, tangent, binormal);
//    p += GerstnerWave(waveParams[1], startingPoint, tangent, binormal);
//    p += GerstnerWave(waveParams[2], startingPoint, tangent, binormal);
//
//  
//    float4 NewVertexPosition = float4(p, 1.0f);
//
//    normal = normalize(cross(binormal, tangent));
//    output.normal = mul(normalize(normal), (float3x3)worldMatrix);
//    //output.normal = normal;
//
//    NewVertexPosition = mul(NewVertexPosition, worldMatrix);
//
//    output.worldPosition = NewVertexPosition;
//    // Calculate the position of the vertex against the world, view, and projection matrices.
//   // output.position = mul(NewVertexPosition, worldMatrix);
//    output.position = NewVertexPosition;
//    output.position = mul(output.position, viewMatrix);
//    output.position = mul(output.position, projectionMatrix);
//
//
//
//    // Calculate the position of the vertice as viewed by the light source.
//
//    output.lightViewPos1 = mul(NewVertexPosition, lightViewMatrix[0]);
//    output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix[0]);
//
//    // Calculate the position of the vertice as viewed by the light source.
//    output.lightViewPos2 = mul(NewVertexPosition, lightViewMatrix[1]);
//    output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix[1]);
//
//    // Calculate the position of the vertice as viewed by the light source.
//    output.lightViewPos3 = mul(NewVertexPosition, lightViewMatrix[2]);
//    output.lightViewPos3 = mul(output.lightViewPos3, lightProjectionMatrix[2]);
//    // Send the input color into the pixel shader.
//   
//
//    return output;
//}
// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[3];
    matrix lightProjectionMatrix[3];

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

struct WaveParams
{
    float time;
    float wavelength;
    float steepness;
    float speed;
    float2 XZdir;
    float k;
    float c;
};



cbuffer WaveParametersBuffer : register(b1)
{
    WaveParams waveParams[3];
};


//const float _PI_= 3.14159265358979323846;
//Returns the positon ofsset of the vertex, and changes tanges and bitangent vectors
float3 GerstnerWave(WaveParams waveParams, float3 inputPoint, inout float3 tangent, inout float3 bitangent)
{


    float2 dir = normalize(waveParams.XZdir);


    /*float k = 2.0f * 3.14159265358979323846f / waveParams.wavelength;
    float c = sqrt(9.8 / k) * waveParams.speed;*/
    float f = waveParams.k * (dot(dir, inputPoint.xz) - waveParams.c * waveParams.time);

    //float f = k * (dot(dir, inputPoint.xz) - waveParams.speed * waveParams.time);
    float a = waveParams.steepness / waveParams.k;


    //Calculate Point offset that is to be returned
    float3 pointOffset;
    pointOffset.x = dir.x * (a * cos(f));
    pointOffset.y = a * sin(f);
    pointOffset.z = dir.y * (a * cos(f));

    tangent += float3(
        -dir.x * dir.x * (waveParams.steepness * sin(f)),
        dir.x * (waveParams.steepness * cos(f)),
        -dir.x * dir.y * (waveParams.steepness * sin(f))
        );
    bitangent += float3(
        -dir.x * dir.y * (waveParams.steepness * sin(f)),
        dir.y * (waveParams.steepness * cos(f)),
        -dir.y * dir.y * (waveParams.steepness * sin(f))
        );

    return pointOffset;

}


struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos1 : TEXCOORD2;
    float4 lightViewPos2 : TEXCOORD3;
    float4 lightViewPos3 : TEXCOORD4;

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

    float2 uv1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 uv2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    tex = lerp(uv1, uv2, uvwCoord.x);

    output.tex = tex;


    output.position = mul(float4(vertexPosition, 1), worldMatrix);

    float3 startingPoint = output.position.xyz;
    float3 tangent = float3(1, 0, 0);
    float3 binormal = float3(0, 0, 1);



    float3 p = startingPoint;
    p += GerstnerWave(waveParams[0], startingPoint, tangent, binormal);
    p += GerstnerWave(waveParams[1], startingPoint, tangent, binormal);
    p += GerstnerWave(waveParams[2], startingPoint, tangent, binormal);


    float4 NewVertexPosition = float4(p, 1.0f);

    normal = normalize(cross(binormal, tangent));
    output.normal = mul(normalize(normal), (float3x3)worldMatrix);
    output.normal = normalize(output.normal);


    output.worldPosition = NewVertexPosition;

    // Calculate the position of the vertex against the world, view, and projection matrices.
   // output.position = mul(NewVertexPosition, worldMatrix);
    output.position = NewVertexPosition;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);



    // Calculate the position of the vertice as viewed by the light source.

    output.lightViewPos1 = mul(NewVertexPosition, lightViewMatrix[0]);
    output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix[0]);

    // Calculate the position of the vertice as viewed by the light source.
    output.lightViewPos2 = mul(NewVertexPosition, lightViewMatrix[1]);
    output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix[1]);

    // Calculate the position of the vertice as viewed by the light source.
    output.lightViewPos3 = mul(NewVertexPosition, lightViewMatrix[2]);
    output.lightViewPos3 = mul(output.lightViewPos3, lightProjectionMatrix[2]);
    // Send the input color into the pixel shader.


    return output;
}
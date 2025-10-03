// Tessellation Hull Shader
// Prepares control points for tessellation
struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

cbuffer TesselationFactors : register(b0)
{
    float4 edgeTesselationFactor;
    float2 insideTesselationFactor;
};
cbuffer WorldMatrixAndCamera : register(b1)
{
    float4x4 worldMatrix;

    float3 camPos;
    float paddingA;

    float nearClip;
    float farClip;

    float paddingB;
    float paddingC;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float tweeningFunction(float input) 
{
    return input*input*input;
}


float getTesselationFactor(float3 pos1, float3 pos2) 
{
    float3 globalPosition1 = mul(worldMatrix, pos1);
    float3 globalPosition2 = mul(worldMatrix, pos2);


    float3 edgeMidPoint = (globalPosition1 + globalPosition2) / 2.0f;
    float dist = distance(edgeMidPoint, camPos);
    float tesselationScale = 1-((dist - nearClip) / (farClip - nearClip));
    float tesselationFactor = tweeningFunction(tesselationScale) * 63 + 1;
    tesselationFactor = clamp(tesselationFactor, 1, 64);
    return tesselationFactor;

}


ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;


   
    // Set the tessellation factors for the three edges of the triangle.
    output.edges[0] = getTesselationFactor(inputPatch[0].position, inputPatch[1].position);

    //UP  T1- V0 V3
    output.edges[1] = getTesselationFactor(inputPatch[0].position, inputPatch[3].position);

    //RIGHT T2 V3 V2 
    output.edges[2] = getTesselationFactor(inputPatch[2].position, inputPatch[3].position);

    //Down T3 v1 V2
    output.edges[3] = getTesselationFactor(inputPatch[2].position, inputPatch[1].position);

    float avgTessFactorX = (output.edges[0] + output.edges[2])/2.0f;
    float avgTessFactorY = (  output.edges[1] + output.edges[3]) / 2.0f;

    output.inside[0] = avgTessFactorY;
    output.inside[1] = avgTessFactorX;

    


    return output;
}


[domain("quad")]
//[partitioning("integer")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;


    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    output.tex = patch[pointId].tex;
    output.normal = patch[pointId].normal;




    return output;
}

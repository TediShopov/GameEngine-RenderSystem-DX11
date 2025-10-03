// Standard issue vertex shader, apply matrices, pass info to pixel shader
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
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
	float3 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

OutputType main(InputType input) 
{
    OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	//Set the pos to xyww instead of xyzw. Z will always be 1 (furthest from camera).
    output.position = output.position.xyww;
    output.tex = input.position;

    return output;

}
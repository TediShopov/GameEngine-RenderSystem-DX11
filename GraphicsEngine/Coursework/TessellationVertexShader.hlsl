// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
// 
// //Moved To Domain Shader
//cbuffer MatrixBuffer : register(b0)
//{
//	matrix worldMatrix;
//	matrix viewMatrix;
//	matrix projectionMatrix;
//};

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	//float3 worldPosition : TEXCOORD1;
};

OutputType main(InputType input)
{
	OutputType output;
	output.position = input.position;
	output.tex = input.tex;
	output.normal = input.normal;
	return output;
}
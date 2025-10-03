Texture2D texture1 : register(t0);
SamplerState sampler1 : register(s0);

// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix[3];
	matrix lightProjectionMatrix[3];
};


cbuffer DisplacementParametersBuffer : register(b1)
{
	float height;
	float padding[3];
};

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 tangent : TANGENT0;
	float4 bitangent : TANGENT1;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
	float4 lightViewPos1 : TEXCOORD2;
	float4 lightViewPos2 : TEXCOORD3; 
	float4 lightViewPos3 : TEXCOORD4;
	float3x3 TBN : TBNMat;
};

float getHeight(float2 uv)
{
	float4 textureColor = texture1.SampleLevel(sampler1, uv, 0);
	float textureColorAvg = (textureColor.x + textureColor.y + textureColor.z) / 3;
	textureColorAvg *= height;
	return textureColorAvg;
}

OutputType main(InputType input)
{
	OutputType output;

	float3 T = normalize(mul(input.tangent, worldMatrix));
	float3 B = normalize(mul(input.bitangent, worldMatrix));
	float3 N = normalize(mul(input.normal, worldMatrix));

	output.TBN = float3x3(T, B, N);


	float3 heightDisplacement = float3(0, 0, 1);
	heightDisplacement = normalize(mul(heightDisplacement, output.TBN));
	input.position += float4(heightDisplacement.xyz* getHeight(input.tex),1);
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.worldPosition = mul(input.position, worldMatrix).xyz;


	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos1 = mul(input.position, worldMatrix);
	output.lightViewPos1 = mul(output.lightViewPos1, lightViewMatrix[0]);
	output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix[0]);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos2 = mul(input.position, worldMatrix);
	output.lightViewPos2 = mul(output.lightViewPos2, lightViewMatrix[1]);
	output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix[1]);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos3 = mul(input.position, worldMatrix);
	output.lightViewPos3 = mul(output.lightViewPos3, lightViewMatrix[2]);
	output.lightViewPos3 = mul(output.lightViewPos3, lightProjectionMatrix[2]);


	return output;
}
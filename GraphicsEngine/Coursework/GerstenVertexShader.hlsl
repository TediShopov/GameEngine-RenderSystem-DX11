
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct WaveParams 
{
	float time;
	float wavelength;
	float steepness;
	float speed;
	float2 XZdir;
	float2 padding;
};



cbuffer WaveParametersBuffer : register(b1)
{
	WaveParams waveParams[3];
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
};

//Returns the positon ofsset of the vertex, and changes tanges and bitangent vectors
float3 GerstnerWave(WaveParams waveParams, float3 inputPoint,inout float3 tangent, inout float3 bitangent) 
{
	float2 dir = normalize(waveParams.XZdir);

	
	float k = 2.0f * 3.14 / waveParams.wavelength;
	float f = k * (dot(dir, inputPoint.xz) - waveParams.speed * waveParams.time);
	float a = waveParams.steepness / k;


	//Calculate Point offset that is to be returned
	float3 pointOffset;
	pointOffset.x = dir.x * (a * cos(f));
	pointOffset.y = a * sin(f);
	pointOffset.z = dir.y * (a * cos(f));

	 tangent += float3(
		 - dir.x * dir.x * (waveParams.steepness * sin(f)),
		dir.x * (waveParams.steepness * cos(f)),
		-dir.x * dir.y * (waveParams.steepness * sin(f))
		);
	 bitangent += float3(
		-dir.x * dir.y * (waveParams.steepness * sin(f)),
		dir.y * (waveParams.steepness * cos(f)),
		 - dir.y * dir.y * (waveParams.steepness * sin(f))
		);

	return pointOffset;

}


OutputType main(InputType input)
{
	OutputType output;
	float3 startingPoint = input.position.xyz;
	float3 tangent = float3(1, 0, 0);
	float3 binormal = float3(0, 0, 1);


	//normalize steepnes of waves to prevent looping
	
	output.position = mul(input.position, worldMatrix);



	float3 p = output.position;
	p += GerstnerWave(waveParams[0], startingPoint, tangent, binormal);
	p += GerstnerWave(waveParams[1], startingPoint, tangent, binormal);
	p += GerstnerWave(waveParams[2], startingPoint, tangent, binormal); 
	
	input.normal = normalize(cross(binormal, tangent));
	output.position = float4(p,1);


	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.worldPosition = mul(input.position, worldMatrix).xyz;

	return output;
}
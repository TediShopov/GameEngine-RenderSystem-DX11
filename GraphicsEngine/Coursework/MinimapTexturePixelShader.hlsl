// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

cbuffer PlayerPositionBuffer : register(b0)
{
	float4 postion;
};

bool inCircle(float2 pos, float2 circlePos, float rad)
{
	float dist = pos.x * pos.x
		+ pos.y * pos.y;
	//return true;

	if ((pos.x - circlePos.x) * (pos.x - circlePos.x) +
		(pos.y - circlePos.y) * (pos.y - circlePos.y) <= rad * rad)
		return true;
	else
		return false;
}


float4 main(InputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	float4 textureColor = texture0.Sample(Sampler0, input.tex);


	float2 clipPostion=  input.tex;


	float2 circlePos = postion.xy;
	float circleRad = 0.05;

	if (inCircle(clipPostion, circlePos, circleRad))
	{
		textureColor = float4(0, 0, 0, 1);
	}

	return textureColor;
}
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

cbuffer ScreenResolution : register(b0)
{
    float width;
    float height;
    float padding[2];

};
cbuffer TextureDistortionBuffer : register(b1)
{
	float3 colorOverlay;
	float time;
	float sinUFrequency;
	float offsetU;
	float sinVFrequency;
	float offsetV;
};




float4 main(InputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.


	float2 modifiedTex = input.tex;

	float X = input.tex.x * sinUFrequency + time;
	float Y = input.tex.y * sinVFrequency + time;
	modifiedTex.y += cos(Y+X) * offsetU * cos(Y);
	modifiedTex.x += sin(X-Y) * offsetV* sin(Y);

	float4 textureColor = texture0.Sample(Sampler0, modifiedTex);
	textureColor = lerp(textureColor ,float4(colorOverlay, 1),0.5);

	return textureColor;
}
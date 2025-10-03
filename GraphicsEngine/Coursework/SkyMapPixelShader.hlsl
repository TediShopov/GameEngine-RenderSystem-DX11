

TextureCube skymap : register(t0);
SamplerState skymapSampler : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float3 tex : TEXCOORD0;
	float3 normal : NORMAL;
};
float4 main(InputType input):SV_TARGET 
{
    return skymap.Sample(skymapSampler, input.tex);
    //return float4(1.00f, 0.00f, 0.00f, 1.0f);

}
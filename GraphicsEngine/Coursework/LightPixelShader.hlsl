// Light pixel shader
// Calculate l_diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4 l_ambient[3];
	float4 l_diffuse[3];
	float4 l_position[3];		//float3 contains the l_position (of direction for a directional light), 4  value is the padding
	//float4 attenuationFactors[3];	//consta, linear and quadratic factors, and a padding
};

cbuffer MaterialBuffer  : register(b1)
{
	
	float4 m_ambient;
	float4 m_diffuse;
	float4 m_specular;
	
};


cbuffer FogParameters : register(b4)
{
	float4 camPos;
	float4 fogColor;
	float fogStart;
	float fogEnd;
	float fogDensity;
	float padding;
}
struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
};

float calculateAttenuation(float dist, float3 attFactors)
{
	float attenuation = 1 / (attFactors[0] + attFactors[1] * dist + attFactors[2] * dist * dist);
	return attenuation;
}

float4 calculateDirecitonalLighting(float3 lightVector, float3 normal, float4 ldiffuse)
{
	float3 lightDirection = normalize(lightVector);

	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(m_diffuse * ldiffuse * intensity);
	return colour;
}


// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculatePointLighting(float3 lightVector, float3 normal, float4 ldiffuse)
{
	float distance = length(lightVector);
	float3 lightDirection = normalize(lightVector);
	//float attenuation = calculateAttenuation(distance, attFactors);

	//ldiffuse = ldiffuse * attenuation;


	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(m_diffuse * ldiffuse * intensity);
	return colour;
}

float4 calcSpecularLight(InputType input,float3 lightDir, float3 specColour) 
{
	lightDir = normalize(lightDir);
	float3 viewDir = normalize(camPos - input.worldPosition);
	float3 reflectDir = reflect(-lightDir, input.normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	float3 specular =  0.5f * spec * specColour;

	return float4(specular.xyz,1);
}


float4 main(InputType input) : SV_TARGET
{

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	float4 textureColour = texture0.Sample(sampler0, input.tex);

	float4 ambient = (l_ambient[0] + l_ambient[1] + l_ambient[2]) * m_ambient;
	float4 diffuse = calculatePointLighting(l_position[0] - input.worldPosition, input.normal, l_diffuse[0]) +
		calculatePointLighting(l_position[1] - input.worldPosition, input.normal, l_diffuse[1]) +
		//Third light's posiiton values are treated like a direction 
		calculateDirecitonalLighting(l_position[2], input.normal, l_diffuse[2]);



	
	float4 spec = calcSpecularLight(input,l_position[2],  l_diffuse[2]);
	

	float4 lightColour =
		(
			ambient + diffuse +spec
		)
	;
	return lightColour * textureColour;
}




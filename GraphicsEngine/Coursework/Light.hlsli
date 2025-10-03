
struct Light
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 position;
	float4 direction;
	float4 attenuationFactors;
	float4 cutOffs;
};
struct ColorComponents 
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

ColorComponents addComponents(ColorComponents c1, ColorComponents c2)
{
	c1.ambient += c2.ambient;
	c1.diffuse += c2.diffuse;
	c1.specular += c2.specular;
	return c1;

}

float calculateAttenuation(float dist, float3 attFactors)
{
	float attenuation = 1 / (attFactors[0] + attFactors[1] * dist + attFactors[2] * dist * dist);
	return attenuation;
}

ColorComponents calculateDirectional(Light light, float3 camPos, float3 worldPos, float3 normal)
{
	ColorComponents colors;

	float3 lightVector = -light.direction;
	float3 lightDirection = normalize(lightVector);
	colors.ambient = light.ambient;

	//Diffuse
	float intensity = saturate(dot(normal, lightDirection));
	float4 diff = saturate(light.diffuse * intensity);
	colors.diffuse = diff;

	float3 viewDir = normalize(camPos - worldPos);
	float3 reflectDir = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	float3 specular = 0.5f * spec * light.specular;


	colors.specular = float4(specular.xyz, 1);
	return colors;

}

ColorComponents calculatePointLight(Light light, float3 camPos, float3 worldPos, float3 normal)
{
	ColorComponents colors;

	float3 lightVector = light.position - worldPos;
	float3 lightDirection = normalize(lightVector);
	float attenuation = calculateAttenuation(length(lightVector), light.attenuationFactors);

	colors.ambient = light.ambient;

	//Diffuse

	float4 ldiffuse = light.diffuse * attenuation;
	float intensity = saturate(dot(normal, lightDirection));
	colors.diffuse = saturate(ldiffuse * intensity);


	float3 viewDir = normalize(camPos - worldPos);
	float3 reflectDir = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	float3 specular = 0.5f * spec * attenuation * light.specular;

	colors.specular = float4(specular.xyz, 1);


	return colors;
}

ColorComponents calculateSpotLight(Light light, float3 camPos, float3 worldPos, float3 normal)
{
	ColorComponents colors;
	colors.ambient = light.ambient;
	float3 lightVector = light.position - worldPos;

	float attenuation = calculateAttenuation(length(lightVector), light.attenuationFactors);


	float3 lightDir = normalize(light.position - worldPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOffs.x - light.cutOffs.y;
	float intensity = clamp((theta - light.cutOffs.y) / epsilon, 0.0, 1.0);


	float3 viewDir = normalize(camPos - worldPos);
	float3 reflectDir = reflect(light.direction, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	float3 specular = 0.5f * spec * attenuation * light.specular * intensity;


	colors.diffuse = light.diffuse * intensity * attenuation;
	colors.specular = float4(specular.xyz,1);


	return colors;
}

float4 getLightColor(ColorComponents colors)
{
    float4 lightColour = saturate(colors.ambient + colors.diffuse + colors.specular);
    return lightColour;
}

int returnAsOne() 
{
	return 1;
}
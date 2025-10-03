
struct ParallelSplitShadowMapData
{
    float4 lightVP1;
    float4 lightVP2;
    float4 lightVP3;
    Texture2D depthTexture1;
    Texture2D depthTexture2;
    Texture2D depthTexture3;
    SamplerState shadowMapSampler;

};
float2 getDepthTexCoord(float4 lightViewPos) 
{
	float2 depthTexCoord= lightViewPos.xy/ lightViewPos.w;
	/*depthTexCoord *= float2(0.5, -0.5);
	depthTexCoord += float2(0.5f, 0.5f);*/

	depthTexCoord *= float2(0.5, -0.5);
	depthTexCoord += float2(0.5f, 0.5f);
	return depthTexCoord;
}


bool isOutsideShadowMap(float2 depthTexCoord) 
{
	// Determine if the projected coordinates are in the 0 to 1 range.If not don't do lighting.
	return depthTexCoord.x < 0.f || depthTexCoord.x > 1.f || depthTexCoord.y < 0.f || depthTexCoord.y > 1.f;
}

float getDepthValue(float2 depthTexCoord, Texture2D depthMap, SamplerState state)
{
	if (isOutsideShadowMap(depthTexCoord))
	{
		return -1;
	}

	return depthMap.Sample(state, depthTexCoord).r;
}


//Returns the sampled depth value from the 3 parallel split shadow maps
float3 getSampledShadowMapDepthValues(ParallelSplitShadowMapData data)
{
	float3 depthValues;
	depthValues[0] = getDepthValue(getDepthTexCoord(data.lightVP1), data.depthTexture1, data.shadowMapSampler);
	depthValues[1] = getDepthValue(getDepthTexCoord(data.lightVP2), data.depthTexture2, data.shadowMapSampler);
	depthValues[2] = getDepthValue(getDepthTexCoord(data.lightVP3), data.depthTexture3, data.shadowMapSampler);
    return depthValues;
}


//Calculates and return the position of the fragment as viewed from the light sources.
//There are 3 splits in the parallel split implementation so returns 3 values;
float3 calculateFragmentPositionInLightSpace(float4 lightVP1, float4 lightVP2, float4 lightVP3)
{
	float3 lightDepthValues;
	float shadowMapBias = 0.005f;
	
	lightDepthValues[0] = lightVP1.z/ lightVP1.w;
	lightDepthValues[0] -= shadowMapBias;

	lightDepthValues[1] = lightVP2.z / lightVP2.w;
	lightDepthValues[1] -= shadowMapBias;

	lightDepthValues[2] = lightVP3.z / lightVP3.w;
	lightDepthValues[2] -= shadowMapBias;
    return lightDepthValues;

}

int getShadowMapIndex(ParallelSplitShadowMapData data)
{
	float3 fragmentInLightSpaces= calculateFragmentPositionInLightSpace(
	data.lightVP1,
	data.lightVP2,
	data.lightVP3
	);

	float3 depthValues = getSampledShadowMapDepthValues(data);

	 if (fragmentInLightSpaces[0] < depthValues[0])
	 {
		return 0;
    }
	 else if (isOutsideShadowMap(getDepthTexCoord(data.lightVP1)))
	 {
		 if (fragmentInLightSpaces[1] < depthValues[1])
		 {
            return 1;
		 }
		 else if (isOutsideShadowMap(getDepthTexCoord(data.lightVP2)))
		 {
			 if (fragmentInLightSpaces[2] < depthValues[2])
			 {
                return 2;
			 }
		 }
	 }
    return -1;
}


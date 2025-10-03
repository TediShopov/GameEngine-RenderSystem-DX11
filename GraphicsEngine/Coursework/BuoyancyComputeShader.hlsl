
//struct WaveParams 
//{;
//	float time;
//	float wavelength;
//	float steepness;
//	float speed;
//	float2 XZdir;
//	float2 padding;
//};

struct WaveParams
{
    float time;
    float wavelength;
    float steepness;
    float speed;
    float2 XZdir;
    float k;
    float c;
};


cbuffer WaveParametersBuffer : register(b0)
{
	WaveParams waveParams[3];
};
cbuffer BuoyancyParameters : register(b1)
{
    matrix worldMatrix;
    float fluidDensity;
    float gravity;
    float columnSurface;
    float columnMaxVolume;

};
cbuffer GradientDescentParameters : register(b2)
{
    float eps;
    float learningRate;
    float offsetAlongAxis;
    int iterations;
};

StructuredBuffer<float3> inputHullPoints : register(t0);
RWStructuredBuffer<float> outputBuoyancyForce : register(u0);



float3 GerstnerWave(WaveParams waveParams, float3 inputPoint, inout float3 tangent, inout float3 bitangent)
{


    float2 dir = normalize(waveParams.XZdir);


    /*float k = 2.0f * 3.14159265358979323846f / waveParams.wavelength;
    float c = sqrt(9.8 / k) * waveParams.speed;*/
    float f = waveParams.k * (dot(dir, inputPoint.xz) - waveParams.c * waveParams.time);

    //float f = k * (dot(dir, inputPoint.xz) - waveParams.speed * waveParams.time);
    float a = waveParams.steepness / waveParams.k;


    //Calculate Point offset that is to be returned
    float3 pointOffset;
    pointOffset.x = dir.x * (a * cos(f));
    pointOffset.y = a * sin(f);
    pointOffset.z = dir.y * (a * cos(f));

    tangent += float3(
        -dir.x * dir.x * (waveParams.steepness * sin(f)),
        dir.x * (waveParams.steepness * cos(f)),
        -dir.x * dir.y * (waveParams.steepness * sin(f))
        );
    bitangent += float3(
        -dir.x * dir.y * (waveParams.steepness * sin(f)),
        dir.y * (waveParams.steepness * cos(f)),
        -dir.y * dir.y * (waveParams.steepness * sin(f))
        );

    return pointOffset;

}


//Returns the positon ofsset of the vertex, and changes tanges and bitangent vectors
//float3 GerstnerWave(WaveParams waveParams, float3 inputPoint,inout float3 tangent, inout float3 bitangent) 
//{
//	float2 dir = normalize(waveParams.XZdir);
//
//	
//	float k = 2.0f * 3.14 / waveParams.wavelength;
//	float f = k * (dot(dir, inputPoint.xz) - waveParams.speed * waveParams.time);
//	float a = waveParams.steepness / k;
//
//
//	//Calculate Point offset that is to be returned
//	float3 pointOffset;
//	pointOffset.x = dir.x * (a * cos(f));
//	pointOffset.y = a * sin(f);
//	pointOffset.z = dir.y * (a * cos(f));
//
//	 tangent += float3(
//		 - dir.x * dir.x * (waveParams.steepness * sin(f)),
//		dir.x * (waveParams.steepness * cos(f)),
//		-dir.x * dir.y * (waveParams.steepness * sin(f))
//		);
//	 bitangent += float3(
//		-dir.x * dir.y * (waveParams.steepness * sin(f)),
//		dir.y * (waveParams.steepness * cos(f)),
//		 - dir.y * dir.y * (waveParams.steepness * sin(f))
//		);
//
//	return pointOffset;
//
//	
//}



//Keep in mind gerstner wave method offset the vertices not only along y but along xz as well
//so if you want to sample the Y-coordinate at a certain plane position, you could negate the offset in
//xz of the global coordinate to be passed as the offset depends only on the wave params
float3 getGestnerWaveXZOffset(WaveParams waveParams, float3 inputPoint) 
{
	float2 dir = normalize(waveParams.XZdir);
	
	float k = 2.0f * 3.14 / waveParams.wavelength;
	float f = k * (dot(dir, inputPoint.xz) - waveParams.speed * waveParams.time);
	float a = waveParams.steepness / k;

	//Calculate Point offset that is to be returned
	float3 pointOffset;
	pointOffset.x = dir.x * (a * cos(f));
	pointOffset.z = dir.y * (a * cos(f));
	return pointOffset;
}
float3 CombinedGerstnerWave(int waveParamSize,float3 worldInput,inout float3 tangent,inout float3 bitangent)
{
    //Y would stricly be determined by the the offset of the gerstner wave fuctions;
    worldInput.y = 0;
    float3 startingWorldInput = worldInput;
    float3 finalPoint = startingWorldInput;

	//Define initial tangent and bitanget values
    tangent = float3(1, 0, 0);
     bitangent = float3(0, 0, 1);
    for (int i = 0; i < waveParamSize; ++i)
    {
        finalPoint += GerstnerWave(waveParams[i], startingWorldInput, tangent, bitangent);
    }
    return finalPoint;
}


float3 ReverseMapGerstnerWaveWGradientDescent(float3 targetPosition, inout int actualIterations) {
    float3 position = targetPosition; // Start with the target position
    float3 reverseMappedPosition = targetPosition;
    float3 inputOffseX = float3(offsetAlongAxis, 0, 0);
    float3 inputOffseZ = float3(0, 0, offsetAlongAxis);

    for (int i = 0; i < iterations; ++i) {
        // Compute the current wave position and gradients
        float3 bitangent, tangent;
        float3 displacedPosition = CombinedGerstnerWave(3, reverseMappedPosition, tangent,bitangent);
        float3 displacedPositionXOffset= CombinedGerstnerWave(3, reverseMappedPosition + inputOffseX, tangent,bitangent);
        float3 displacedPositionZOffset= CombinedGerstnerWave(3, reverseMappedPosition + inputOffseZ, tangent,bitangent);

        float3 gradientRespectingX = displacedPositionXOffset - displacedPosition;
        float3 gradientRespectingZ = displacedPositionZOffset - displacedPosition;

        float3 error = displacedPosition - targetPosition;
        // Break if the error is below a threshold
        if (abs(error.x) < eps && abs(error.z) < eps)
        {
            break;
        }

        float changeX = -learningRate*(gradientRespectingX.x * error.x + gradientRespectingX.z * error.z);
        float changeZ = -learningRate*(gradientRespectingZ.x * error.x + gradientRespectingZ.z * error.z);

        reverseMappedPosition.x += changeX;
        reverseMappedPosition.z += changeZ;
        actualIterations = i;
    }

    return reverseMappedPosition;
}

[numthreads(1, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    float4 hullRelativePosition = float4(inputHullPoints[id.x].x, inputHullPoints[id.x].y, inputHullPoints[id.x].z,1);
    float4 hullWorldPositon = mul(hullRelativePosition,worldMatrix); 
	//Wave assummed to start at y = 0
    int iter = 0;
    float3 tangent;
    float3 binormal;
    
    float3 reverseMappedGerstner= ReverseMapGerstnerWaveWGradientDescent(hullWorldPositon.xyz,  iter);
    float3 waveWorldPos= CombinedGerstnerWave(3, reverseMappedGerstner, tangent,binormal);
    
   float3 normal = normalize(cross(binormal, tangent));
    normal = mul(normalize(normal), (float3x3) worldMatrix);
    float3 error = waveWorldPos - hullWorldPositon.xyz;

    if(abs(error.x) < eps && abs(error.z) < eps)
    {
	//calculate buoyancy
    float submergedY = max(0, (waveWorldPos.y - hullWorldPositon.y));
   // float submergedY = (waveWorldPos.y - hullWorldPositon.y);
    float volume = min(columnMaxVolume,submergedY * columnSurface);
    float buoyantForce = fluidDensity * gravity * volume;

    outputBuoyancyForce[id.x].x = buoyantForce;
   // outputBuoyancyForce[id.x].y = normal;
   // outputBuoyancyForce[id.x].z = buoyantForce;
    }
    else
    {
        
        outputBuoyancyForce[id.x] = -length(float2(error.x,error.z));
    }


}
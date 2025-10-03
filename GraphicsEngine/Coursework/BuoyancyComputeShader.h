#pragma once
#include <vector>

#include "BaseShader.h"
#include "ShaderParameter.h"
#include "WaveShader.h"
class btRigidBody;
//#include "D:\cmp301_coursework-TediShopov\Coursework\include\BaseShader.h"
class MeshInstance;
struct BuoyancyParameters  
{
    XMMATRIX worldMatrix;
    float fluidDensity; float gravity;
    float columnSurface;
	float maxColumnVolume;
};

struct GradientDescentParameters 
{
    float eps;
    float learningRate;
    float offsetAlongAxis;
    int iterations;
};

class BuoyancyComputeShader :
    public BaseShader
{

	//--BUOYANT BODY PROPERTIES--
	btRigidBody* buoyantBodyHull;
	MeshInstance* buoyantBodyMeshInstance;
	XMVECTOR buoyantBodyWorldDimensions;


	ID3D11Buffer* inputBuffer = nullptr;
	ID3D11Buffer* outputBuffer = nullptr;
	ID3D11Buffer* outputResultBuffer;
	ID3D11ShaderResourceView* inputResourcesView;
	ID3D11UnorderedAccessView* outputUnorderedAcessView;
	ShaderBuffer<MultipleWaveBuffer> WaveParemetersResource;
	ShaderBuffer<BuoyancyParameters> BuoyancyParemetersResource;
	ShaderBuffer<GradientDescentParameters> GradientDescentParamsResource;

	void initInputOutputBuffers(ID3D11Device* device );



	//Buoyancy forces applied getHeightAt a vertex.
public:

	bool debugVisualizeBuoyantForces = false;
	BuoyancyParameters buoyancyParameters;
	GradientDescentParameters gradientDescentParameters;
	std::vector<float> buoyancyForces;
	//
	//Samples along the hull
	int stepsAlongHullX;
	int stepsAlongHullZ;
	int getSamples();


	//Controllable parameters for debugging engine forces
	XMFLOAT3 relativePointToApplyDebugForce;
	float debugRelativeForce=0;
	float debugCentralForce=0;
	bool useCentraForceToKeepBodyAfloat = false;
	std::vector<XMFLOAT3> positionAlongHull;

	void setBuoyantBody(MeshInstance* mesh_instance);

	//Contruct the buoyant body hull as a bounding box in WORLD SPACE
	// by translating the model space buoyant box
	btRigidBody* constructBuoyantBodyHull();

	BuoyancyComputeShader(ID3D11Device* device, HWND hwnd, int xPrecision, int zPrecision);
	//Points must be in the range of 0-1 getHeightAt the XY-dimensions and all getHeightAt the z dimensions
	std::vector<XMFLOAT3> generateRelativePointsAlongHull(
	float startX, float endX, int stepsX,
	float startY, float endY, int stepsY,
	float startZ, float endZ, int stepsZ);



	void computeAndApplyBuoyantForce(btRigidBody* rigid_body,MeshInstance* mesh_instance, ID3D11DeviceContext* deviceContext, int x,int y
	, int z, MultipleWaveBuffer parameters);

	//This sets the column surface and max volume (A column is the mathematical object that
	// is projected from each sample point of the hull
	void setDefaultValuesForBody(XMVECTOR worldDimensions, float samples);
	static float computeDefaultMaxSurface(XMVECTOR worldDimensions);
	//Computer the max surface of a singular projected column from a sample along a hull
	static float computeDefaultMaxSurfacePerColumn(XMVECTOR worldDimensions, float samples);
	//Computes the default max volume based on the calculated world space BB
	float computeDefaultMaxVolume(float estimatedSurface);

};


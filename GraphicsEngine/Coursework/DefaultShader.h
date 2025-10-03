#pragma once
#include "BaseShader.h"
#include "ShaderParameter.h"
#include "CascadedShadowMaps.h"
//using namespace std;
//using namespace DirectX;
class Material;
class Light;

struct ShadowMatrixBuffer
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
	XMMATRIX lightView[3];
	XMMATRIX lightProjection[3];
};
//Only purpose is to debug visualize the shadow maps
struct ShadowDebugData
{
	int debugVisualzeShadowMaps;
	XMFLOAT3 padding;
	
};

struct LightType
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT4 position;
	XMFLOAT4 direction;
	XMFLOAT4 attenuationFactors;
	XMFLOAT4 cutOffs;
};

struct LightBufferType
{
	LightType lights[3];
};

struct MaterialBufferType
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	float reflectionFactor;
	XMFLOAT3 emissive;
	//XMFLOAT3 padding;
};

struct FogParametersType
{
	XMFLOAT4 camPos;
	XMFLOAT4 fogColor;
	float fogStart;
	float fogEnd;
	float fogDensity;
	float padding;
};

struct SSRBuffer
{
	// SSRCameraData
	XMMATRIX cameraViewMatrix;
	XMMATRIX cameraProjMatrix;
	XMMATRIX cameraWorldMatrix;
	XMMATRIX cameraInverseViewMatrix;
	XMMATRIX cameraInverseProjMatrix;
	XMFLOAT4 cameraPosition;
	// SSRParameters
	int useSSR = 0;
	float maxLengthInWorldUnits;
	int maxSteps;
	float thicknessInUnits;
	float resolution = 1;
	int width = 0;
	int height = 0;
	float padding;
};

struct ShadowMappingLightSource
{
	XMMATRIX lightView;
	XMMATRIX lightProjection;
};

struct ShadowMappingLights
{
	ShadowMappingLightSource lightSourceMatrices[3];
};
struct SSRParameters
{
	//The first render pass to capture depth and color buffers (useSSR = off)
	//to be used with Screen-Space reflections on the second pass (useSSR = on)
	bool useSSR = true;
	float ssrWorldLength = 200;
	int ssrMaxSteps = 3000;
	float resolution = 1;
	float thickness = 1.0f;
};

//This is the default shader used for rendering objects in the scene. Used to be called
//ShadowShader as it uses implements the cascading shadows algorithms (the shadow maps along different
//slices need to be passed)
class DefaultShader :
	public BaseShader
{
protected:
	//Buffers
	#pragma region Constant Buffers
	ShaderBuffer<LightBufferType> lightBuffer;
	ShaderBuffer<MaterialBufferType> materialBuffer;
	ShaderBuffer<FogParametersType> fogParameters;
	ShaderBuffer<SSRBuffer> ssrResource;
	ShaderBuffer<ShadowMatrixBuffer> shadowMatrixBuffer;
	ShaderBuffer<ShadowDebugData> shadowDebugBuffer;

	ShadowMatrixBuffer* buff = new ShadowMatrixBuffer();
#pragma endregion

	//Samplers
	#pragma region Samplers
	ShaderSamplerParam shadowSampler;
	ShaderSamplerParam diffuseSampleState;
	ShaderSamplerParam skyboxSampler;

#pragma endregion

	//Textures
	#pragma region Textures
	ID3D11Texture2D* cubeMapTexture;
	ID3D11ShaderResourceView* cubeMapSRV;
	ShaderTextureParam skyboxResource;
	ShaderTextureParam diffuseTexture;

	//Color and depth textures for SSR
	ShaderTextureParam colorTexture;
	ShaderTextureParam depthTexture;

	//Parallel split shadow maps
	std::vector<ShaderTextureParam> shadowDepthTexture;
#pragma endregion


	#pragma region Utility

	//Utility function that supplies polygonLayout as well
	//Used for normal and displacement maps that require the tanget and bitangets
	void loadVertexShaderWLayout(const wchar_t* filename, D3D11_INPUT_ELEMENT_DESC* polygonLayout, int numElements);

	//Sky env map utility functions
	void loadCubeMap(ID3D11Device* device, const std::wstring& ddsFilePath);

	bool isCubeMap(ID3D11Texture2D* texture);

	//Utility function to pad XMFLOAT3, to XMFLOAT4
	XMFLOAT4 padToVecFour(XMFLOAT3 vec, float pad);

	//Construct light buffer data from an array of lights
	LightBufferType constructLightBufferData(Light** lights);
#pragma endregion
public:
	//Enable debug visualization of shadow maps
	//Each fragment that is not shadowed is colored based on
	//the index depth map used for the comparison.
	bool debugVisalizeShadowMaps;
	//SSR exposed parameters (easier to control from imgui when public)
	SSRParameters ssrParameters;

	DefaultShader(ID3D11Device* device, HWND hwnd);
	DefaultShader(ID3D11Device* device, HWND hwnd, const wchar_t* vs, const wchar_t* ps);

	//--INITIALIZATION--
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initBuffers(ID3D11Device* device);
	void initSamplers(ID3D11Device* device);

	//A different method for passing to the shader only the data that is specific to the instance
	void setShaderParametersForInstance( ID3D11DeviceContext* deviceContext,
		const XMMATRIX& world,
		const Material* material,
		ID3D11ShaderResourceView* texture
	);

	//Passing the shader generic shader parameters that are not specific to an instance
	void setShaderParameters(
		ID3D11DeviceContext* deviceContext,
		const XMMATRIX& view,
		const XMMATRIX& projection,
		ShadowMappingLights lightData,
		Light** light,
		XMFLOAT3 camPos,
		int width,
		int height);


		void setShaderParamsNew(
			ID3D11DeviceContext* deviceContext,
			 FPCamera* camera,
			const XMMATRIX& projection,
			//const XMMATRIX& view,
			//XMFLOAT3 camPos,
			//ShadowMappingLights shadowMappingLights,
			CascadedShadowMaps* shadowMappingLights,

			Light** light,
			FogParametersType fog,
			int width,
			int height);


	//Used for Screen-Space Reflections
	void setSSRColorAndDepthTextures(
		ID3D11DeviceContext* deviceContext,
		ID3D11ShaderResourceView* color,
		ID3D11ShaderResourceView* depth
	);

	void setFogParameters(
		ID3D11DeviceContext* deviceContext,
		FogParametersType fog);

	void setShadowMap(
		ID3D11DeviceContext* deviceContext,
		ID3D11ShaderResourceView* texture,
		int pos);



};

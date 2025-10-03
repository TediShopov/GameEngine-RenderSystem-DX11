//#pragma once
//
//#include "DXF.h"
//#include "ShaderBuffer.h"
//using namespace std;
//using namespace DirectX;
//class AttenuationLight;
//class Material;
//
//struct LightBufferType
//{
//	XMFLOAT4 ambient[3];
//	XMFLOAT4 diffuse[3];
//	XMFLOAT4 position[3];
//	//XMFLOAT4 attenuationFactors[3];
//};
//
//struct MaterialBufferType
//{
//	XMFLOAT4 ambient;
//	XMFLOAT4 diffuse;
//	XMFLOAT4 specular;
//	XMFLOAT4 camPos;
//};
//
//class LightMaterialShader : public BaseShader
//{
//private:
//	
//
//public:
//	LightMaterialShader(ID3D11Device* device, HWND hwnd);
//	LightMaterialShader(ID3D11Device* device, HWND hwnd, const wchar_t* vs, const wchar_t* ps);
//	~LightMaterialShader();
//
//	ShaderBuffer<MatrixBufferType> matrixBuffer;
//	ShaderBuffer<LightBufferType> lightBuffer;
//	ShaderBuffer<MaterialBufferType> materialBuffer;
//
//
//	virtual void setShaderParameters(
//		ID3D11DeviceContext* deviceContext,
//		const XMMATRIX& world,
//		const XMMATRIX& view,
//		const XMMATRIX& projection,
//		ID3D11ShaderResourceView* texture,
//		Light** light,
//		const Material* material,
//		const XMFLOAT3 camPos);
//
//
//	
//protected:
//	XMFLOAT4 padToVecFour(XMFLOAT3 vec, float pad)
//	{
//		XMFLOAT4 toReturn;
//		toReturn.x = vec.x; toReturn.y = vec.y; toReturn.z = vec.z;
//		toReturn.w = pad;
//		return toReturn;
//	}
//
//	
//	void initShader(const wchar_t* vs, const wchar_t* ps);
//	/*ID3D11Buffer* matrixBuffer;
//	ID3D11Buffer* lightBuffer;
//	ID3D11Buffer* materialBuffer;*/
//
//	ID3D11SamplerState* sampleState;
//
//private:
//	
//
//};
//

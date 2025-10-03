#pragma once
#include "DXF.h"
#include "ShadowMap.h"
#include <functional>
//#include "DefaultShader.h"

struct BoundBox		
{
	float top=		-D3D11_FLOAT32_MAX;
	float left=		D3D11_FLOAT32_MAX;
	float right=	-D3D11_FLOAT32_MAX;
	float down=		D3D11_FLOAT32_MAX;
	float fars=		-D3D11_FLOAT32_MAX;
	float nears=	D3D11_FLOAT32_MAX;
};
class Subfrustrum
{

private:
	XMMATRIX generateViewMatrixFirDirectionalLight(XMFLOAT3 lightDir)
	{
	
		Light directionalLightInstance;
		directionalLightInstance.setDirection(lightDir.x, lightDir.y, lightDir.z);

	
		directionalLightInstance.setPosition(
			- lightDir.x * 200, - lightDir.y * 200,- lightDir.z * 200);

		XMVECTOR pos =XMLoadFloat3(&directionalLightInstance.getPosition());
		XMVECTOR dir = XMLoadFloat3(&lightDir);

		directionalLightInstance.generateViewMatrix();
		return directionalLightInstance.getViewMatrix();
	}


	XMMATRIX generateOrthoMatrix(BoundBox box)
	{
		return XMMatrixOrthographicOffCenterLH(box.left,box.right,box.down,box.top,0.1f, 400);
	}


	BoundBox generateBoundingBox(XMMATRIX lightViewMatrix)
	{
		BoundBox boundBox;

		for (size_t i = 0; i < frustrumPoints.size(); i++)
		{
			//In View Space 
			XMVECTOR point = XMLoadFloat4(&frustrumPoints[i]);
			point = XMVector4Transform(point, lightViewMatrix);
			XMStoreFloat4(&frustrumPoints[i], point);


			boundBox.left =min(boundBox.left, frustrumPoints[i].x);
			boundBox.right = max(boundBox.right, frustrumPoints[i].x);

			boundBox.down = min(boundBox.down, frustrumPoints[i].y);
			boundBox.top = max(boundBox.top, frustrumPoints[i].y);

			boundBox.nears = min(boundBox.nears, frustrumPoints[i].z);
			boundBox.fars = max(boundBox.fars, frustrumPoints[i].z);
		}

		return boundBox;
	}

	void returnFrustrumPointToViewSpace(XMMATRIX cameraViewMatrix)
	{
		for (size_t i = 0; i < frustrumPoints.size(); i++)
		{
			XMVECTOR point = XMLoadFloat4(&frustrumPoints[i]);
			point = XMVector4Transform(point, cameraViewMatrix);
			XMStoreFloat4(&frustrumPoints[i], point);
		}

		//In View Space 
		XMVECTOR point = XMLoadFloat4(&frustrumCenterPoint);
		//Transforf to world by 
		point = XMVector4Transform(point, cameraViewMatrix);
		XMStoreFloat4(&frustrumCenterPoint, point);
	}

	void transformFrustrumPointsToWolrdSpace(XMMATRIX cameraViewMatrix)
	{
		XMMATRIX inverseCameraViewMatrix = XMMatrixInverse(nullptr, cameraViewMatrix);

		for (size_t i = 0; i < frustrumPoints.size(); i++)
		{
			//In View Space 
			XMVECTOR point = XMLoadFloat4(&frustrumPoints[i]);
			//Transforf to world by 
			point = XMVector4Transform(point, inverseCameraViewMatrix);

			XMStoreFloat4(&frustrumPoints[i], point);
		}


		//In View Space 
		XMVECTOR point = XMLoadFloat4(&frustrumCenterPoint);
		//Transforf to world by 
		point = XMVector4Transform(point, inverseCameraViewMatrix);
		XMStoreFloat4(&frustrumCenterPoint, point);
	}

	void calculateViewSpaceCoordinates()
	{
		float tanHalfHFOV = tan(FovXRad * 0.5);
		float tanHalfVFOV = tan(FovYRad * 0.5);


		float xNear = nearPlane * tanHalfHFOV;
		float xFar = farPlane * tanHalfHFOV;
		float yNear = nearPlane * tanHalfVFOV;
		float yFar = farPlane * tanHalfVFOV;


		//Near Face
	//Top left
		frustrumPoints.push_back(XMFLOAT4(xNear, yNear, nearPlane, 1.0f));
		//Top right
		frustrumPoints.push_back(XMFLOAT4(-xNear, yNear, nearPlane, 1.0f));
		//Bottom right
		frustrumPoints.push_back(XMFLOAT4(-xNear, -yNear, nearPlane, 1.0f));
		//Bottom left
		frustrumPoints.push_back(XMFLOAT4(xNear, -yNear, nearPlane, 1.0f));

		//Near Face
		//Top left
		frustrumPoints.push_back(XMFLOAT4(xFar, yFar, farPlane, 1.0f));
		//Top right
		frustrumPoints.push_back(XMFLOAT4(-xFar, yFar, farPlane, 1.0f));
		//Bottom right
		frustrumPoints.push_back(XMFLOAT4(-xFar, -yFar, farPlane, 1.0f));
		//Bottom left
		frustrumPoints.push_back(XMFLOAT4(xFar, -yFar, farPlane, 1.0f));
	}

public:
	float nearPlane;
	float farPlane;
	float FovYRad;
	float FovXRad;

	XMFLOAT4 frustrumCenterPoint;
	std::vector<XMFLOAT4> frustrumPoints;

	Subfrustrum(float frustrumNear, float frustrumFar,float fovYRad, float screenwidth, float screenheight)
		:nearPlane(frustrumNear), farPlane(frustrumFar), FovYRad(fovYRad)
	{
		float aspectRatio=  (float)screenwidth / (float)screenheight;
		//FovXRad = (1 / (std::tanf(fovYRad * 0.5))) / aspectRatio;
		FovXRad = FovYRad * aspectRatio;
		float distance = (frustrumFar - frustrumNear);
		frustrumCenterPoint = XMFLOAT4A(0, 0, nearPlane + (distance /2.0f), 1.0f);
		calculateViewSpaceCoordinates();
	}

	

	std::pair<XMMATRIX, XMMATRIX> generateMatrices(const Camera* camera,Light& light) 
	{
		transformFrustrumPointsToWolrdSpace(camera->getViewMatrix());

		//Subfrustrum Points are in world space
		XMMATRIX lightViewMatirx = generateViewMatrixFirDirectionalLight(light.getDirection());

		//Translate points to LIGHTS view space, and generates a bounding box
		BoundBox box = generateBoundingBox(lightViewMatirx);

		//Based on bounding box we create the light orthogonal matrix
		XMMATRIX lightOrthogonalMatrix = generateOrthoMatrix(box);
		auto toReturn= std::pair<XMMATRIX,XMMATRIX>({ lightViewMatirx,lightOrthogonalMatrix });

		//Return the pooints back to view space
		returnFrustrumPointToViewSpace(camera->getViewMatrix());
		
		return toReturn;
	}


};

class CascadedShadowMaps
{
	std::vector<ShadowMap*> shadowMaps;
	std::vector<Subfrustrum> frustrums;
	std::vector<std::pair<XMMATRIX, XMMATRIX>> generatedLightMatrices;


public:
	CascadedShadowMaps()
	{
		resetGenratedLightMatrices();

	}

	void resetGenratedLightMatrices() 
	{
		generatedLightMatrices.clear();
		generatedLightMatrices.push_back({ XMMatrixIdentity() ,XMMatrixIdentity() });
		generatedLightMatrices.push_back({ XMMatrixIdentity() ,XMMatrixIdentity() });
		generatedLightMatrices.push_back({ XMMatrixIdentity() ,XMMatrixIdentity() });
	}

	void calculateSubFrustrums(ID3D11Device* device, Camera* camera, float frustrumNear, float frustrumFar, float fovYRad, float screenwidth, float screenheight)
	{
		float screenAspect = screenwidth / screenheight;
		//float fovX = 2 * std::atanf(std::tanf(fovYRad * 0.5) / screenAspect);
		float fovX = (1 / (std::tanf(fovYRad * 0.5))) / screenAspect;
		float distance = frustrumFar - frustrumNear;
		frustrums.push_back(Subfrustrum(0,frustrumNear+distance*0.1f,fovYRad,screenwidth,screenheight));
		frustrums.push_back(Subfrustrum(frustrumNear + distance * 0.1f, frustrumNear +distance * 0.5f, fovYRad, screenwidth, screenheight));
		frustrums.push_back(Subfrustrum(frustrumNear + distance * 0.5f, frustrumNear + distance, fovYRad, screenwidth, screenheight));


		float dimension = 4000;
		for (auto frustrum : this->frustrums)
		{
			shadowMaps.push_back(new ShadowMap(device,dimension,dimension));
		}
	}

	Subfrustrum prepareShadowMap(ID3D11DeviceContext* deviceContext,int index)
	{
		shadowMaps[index]->BindDsvAndSetNullRenderTarget(deviceContext);
		return frustrums[index];
	}

	void updateAll(ID3D11DeviceContext* deviceContext,const Camera* c, Light& l, const  std::function<void (XMMATRIX, XMMATRIX)>& renderFunc)
	{
		l.generateViewMatrix();
		//l->generateOrthoMatrix();

		resetGenratedLightMatrices();
		for (size_t i = 0; i < frustrums.size(); i++)
		{
			Subfrustrum frustrum = this->prepareShadowMap(deviceContext, i);
			auto matrices = frustrum.generateMatrices(c, l);
			generatedLightMatrices[i] = matrices;
			renderFunc(matrices.first, matrices.second);
		}
	}


	int getCount() { return frustrums.size(); }
	XMMATRIX getViewMatrix(int index) { return generatedLightMatrices[index].first; }
	XMMATRIX getOrthoMatrix(int index) { return generatedLightMatrices[index].second; }

	ShadowMap* getShadowMap(int index) { return shadowMaps[index]; }


};


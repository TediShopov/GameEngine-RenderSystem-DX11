#pragma once


#include "DXF.h"
#include "DefaultShader.h"
#include "RenderTexture.h"
#include "MeshInstance.h"
#include "TangentMesh.h"
#include "LimitedTimeRenderCollection.h"
#include <random>
#include <cmath>
class ProceduralDestruction
{
public:
	const float DOT_EPSILON = 0.05;

	//Temporary buffers to hold the output of cut mesh functions
	std::vector<XMVECTOR> pAbovePlane;
	std::vector<XMVECTOR> pBelowPlane;
	std::vector<XMVECTOR> pIntersections;

	//Triangle clipping without capping; Returns two meshes above and below
	std::vector<BaseMesh*> cutMeshOpen(XMVECTOR plane, BaseMesh* Mesh, 
	std::vector<XMVECTOR>* outNewlyAddedPoints = nullptr,
	std::vector<XMVECTOR>* outAbove = nullptr,
	std::vector<XMVECTOR>* outBelow = nullptr);

	//Same as cutting mesh open + a cap constructed via a triangle fan
	std::vector<BaseMesh*> cutMeshClosed(XMVECTOR plane, BaseMesh* Mesh, 
	std::vector<XMVECTOR>* outNewlyAddedPoints = nullptr,
	std::vector<XMVECTOR>* outAbove = nullptr,
	std::vector<XMVECTOR>* outBelow = nullptr);

	//Cuts the mesh multiple time by rotating a plane on the XZ axis.
	std::vector<MeshInstance*> radialPlaneCutsXZEvenlySpaced(XMVECTOR point,BaseMesh* toSplit);

	//Cuts the mesh multiple times by planes with a random direction and offset from centerl
	std::vector<MeshInstance*> radialPlaneCutsRandom(XMVECTOR point,BaseMesh* toSplit);

	void computeVertexCentroid(std::vector<VertexType>& vertices,  DirectX::XMVECTOR& vertexCenter);
protected:


	void classifyVerticesRelativeToPlane(XMVECTOR plane, 
		BaseMesh* Mesh,
		std::vector<XMVECTOR>& outAbove,
		std::vector<XMVECTOR>& outBelow);

	void classifyVerticesAndEdgeIntersections(XMVECTOR plane,
		BaseMesh* Mesh,
		std::vector<XMVECTOR>& pAbove,
		std::vector<XMVECTOR>& pBelow,
		std::vector<XMVECTOR>& pIntersections
	);

	void addTriangle(ProceduralMeshData& aboveMesh,  VertexType one, VertexType two, VertexType three);
	void addTriangleFacingDirection(ProceduralMeshData& aboveMesh,  VertexType one, VertexType two, VertexType three, XMVECTOR desiredDirection);
	void addTriangleTwoSided(ProceduralMeshData& aboveMesh, BaseMesh* Mesh, VertexType one, VertexType two, VertexType three);

	//Expects and even list of intersections vertices produced by edge cuts
	//Center of triangle fan is the first point. If the list isn't even the last pair is skipped
	void buildTriangleFan(ProceduralMeshData& meshData,VertexType center, std::vector<VertexType>& outPoints, XMVECTOR triangleDirection);

	//Substract origin from vertex positions. Normals and UV's are unchanged
	void translateMeshToOrigin(ProceduralMeshData& aboveMesh, XMVECTOR origin);

	bool isAboveOrOnPlane(XMVECTOR plane, XMVECTOR point);

	//Cycling indexing in [0, maxsize). Negative input supported.
	int indexWrap(int desired, int maxSize);

	VertexType calculateIntersectionVertex(XMVECTOR plane,VertexType one, VertexType two);

	VertexType lerpVertex(VertexType one, VertexType two, float a);
	//Projection point on a plane
	XMVECTOR projectOnPlane(XMVECTOR point,XMVECTOR plane);

	XMVECTOR randomPointOnUnitSphere(double radius = 1.0);
};


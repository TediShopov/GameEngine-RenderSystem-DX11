#include "ProceduralDestruction.h"

bool ProceduralDestruction::isAboveOrOnPlane(XMVECTOR plane, XMVECTOR point)
{
	XMVECTOR distance = XMPlaneDot(plane, point);
	return XMVectorGetX(distance) >= 0;
}

void ProceduralDestruction::classifyVerticesRelativeToPlane(XMVECTOR plane, BaseMesh* Mesh, std::vector<XMVECTOR>& pAbove, std::vector<XMVECTOR>& pBelow)
{
	pAbove.clear();
	pBelow.clear();

	int intersectionCount = 0;
	for (int i = 0; i < Mesh->indices.size(); i += 3)
	{
		auto A = XMLoadFloat3(&Mesh->vertices[i].position);
		auto B = XMLoadFloat3(&Mesh->vertices[i + 1].position);
		auto C = XMLoadFloat3(&Mesh->vertices[i + 2].position);

		A.m128_f32[3] = 1;
		B.m128_f32[3] = 1;
		C.m128_f32[3] = 1;
		XMVECTOR P;
		float t, u, v;

		if (isAboveOrOnPlane(plane, A))
			pAbovePlane.push_back(A);
		else
			pBelowPlane.push_back(A);

		if (isAboveOrOnPlane(plane, B))
			pAbovePlane.push_back(B);
		else
			pBelowPlane.push_back(B);

		if (isAboveOrOnPlane(plane, C))
			pAbovePlane.push_back(C);
		else
			pBelowPlane.push_back(C);
	}
}

std::vector<BaseMesh*> ProceduralDestruction::cutMeshOpen(XMVECTOR plane, BaseMesh* Mesh,
	std::vector<XMVECTOR>* newlyAddedPoints,
	std::vector<XMVECTOR>* above,
	std::vector<XMVECTOR>* below
)
{
	ProceduralMeshData aboveMesh;

	ProceduralMeshData belowMesh;

	XMVECTOR origin;
	computeVertexCentroid(Mesh->vertices, origin);
	origin.m128_f32[1] += 25;

	if (below != nullptr)
		below->push_back(origin);

	XMVECTOR projectedOnPlane = projectOnPlane(origin, plane);
	if (newlyAddedPoints != nullptr)
		newlyAddedPoints->push_back(projectedOnPlane);

	for (int i = 0; i < Mesh->indices.size(); i += 3)
	{
		auto A = XMLoadFloat3(&Mesh->vertices[i].position);
		auto B = XMLoadFloat3(&Mesh->vertices[i + 1].position);
		auto C = XMLoadFloat3(&Mesh->vertices[i + 2].position);

		A.m128_f32[3] = 1;
		B.m128_f32[3] = 1;
		C.m128_f32[3] = 1;

		//+1 for each vertex above or on the plane
		int planeIntersectionScore = 0;

		//In Order A B C
		XMVECTOR triVertices[3] = { A,B,C };
		bool aboveOrOnPlaneVertex[3] = { isAboveOrOnPlane(plane,A),isAboveOrOnPlane(plane,B),isAboveOrOnPlane(plane,C) };

		planeIntersectionScore += isAboveOrOnPlane(plane, A) + isAboveOrOnPlane(plane, B) + isAboveOrOnPlane(plane, C);

		XMVECTOR temp;

		//3 means that all of triangle is above plane
		if (planeIntersectionScore == 3)
		{
			addTriangle(aboveMesh,
				Mesh->vertices[i],
				Mesh->vertices[i + 1],
				Mesh->vertices[i + 2]);
		}
		else if (planeIntersectionScore == 0)
		{
			addTriangle(belowMesh,
				Mesh->vertices[i],
				Mesh->vertices[i + 1],
				Mesh->vertices[i + 2]
			);
		}
		else
		{
			bool neededValue = true;
			ProceduralMeshData* meshOnSingledSide = &belowMesh;
			ProceduralMeshData* meshOnOtherSide = &aboveMesh;
			// On vertex is below the plane, two are above
			if (planeIntersectionScore == 2)
			{
				neededValue = false;
				meshOnSingledSide = &belowMesh;
				meshOnOtherSide = &aboveMesh;
			}
			else {
				neededValue = true;
				meshOnSingledSide = &aboveMesh;
				meshOnOtherSide = &belowMesh;
			}

			int iSingled = 999;
			for (int j = 0; j < 3; j++)
			{
				if (aboveOrOnPlaneVertex[j] == neededValue)
				{
					iSingled = j;
				}
			}
			int iOtherOne = indexWrap(iSingled - 1, 3);
			int iOtherTwo = indexWrap(iSingled + 1, 3);

			//This is the only triangle added to the singled out mesh
			//Triangle one - singled vertex plus two intesections

			VertexType intersectionSingledAndOne = calculateIntersectionVertex(
				plane, Mesh->vertices[i + iSingled], Mesh->vertices[i + iOtherOne]);

			VertexType intersectionSingledAndTwo = calculateIntersectionVertex(
				plane, Mesh->vertices[i + iSingled], Mesh->vertices[i + iOtherTwo]);

			if (isnan(intersectionSingledAndOne.position.x))
				continue;

			if (isnan(intersectionSingledAndTwo.position.x))
				continue;

			VertexType midPointOther = lerpVertex(
				Mesh->vertices[i + iOtherOne],
				Mesh->vertices[i + iOtherTwo],
				0.5f
			);

			addTriangleTwoSided(*meshOnSingledSide, Mesh,
				Mesh->vertices[i + iSingled],
				intersectionSingledAndOne,
				intersectionSingledAndTwo);

			//Triangle two - other vertex one, mid-point, intersection
			addTriangleTwoSided(*meshOnOtherSide, Mesh,
				Mesh->vertices[i + iOtherOne],
				midPointOther,
				intersectionSingledAndOne);

			//Triangle three - other vertex two, mid-point, intersection
			addTriangleTwoSided(*meshOnOtherSide, Mesh,
				Mesh->vertices[i + iOtherTwo],
				midPointOther,
				intersectionSingledAndTwo);

			//Triangle four - intersection, intersection, midpoint
			addTriangleTwoSided(*meshOnOtherSide, Mesh,
				intersectionSingledAndOne,
				midPointOther,
				intersectionSingledAndTwo);

			continue;
			// On vertex is above the plane, two are below
		}
	}

	std::vector<BaseMesh*> meshes;
	ProceduralMeshA* AboveMesh = new ProceduralMeshA(
		Mesh->device, Mesh->deviceContext,
		aboveMesh.vertices, aboveMesh.indices);

	ProceduralMeshA* BelowMesh = new ProceduralMeshA(
		Mesh->device, Mesh->deviceContext,
		belowMesh.vertices, belowMesh.indices);

	meshes.push_back(AboveMesh);
	meshes.push_back(BelowMesh);

	return meshes;
}

std::vector<BaseMesh*> ProceduralDestruction::cutMeshClosed(XMVECTOR plane,
	BaseMesh* Mesh,
	std::vector<XMVECTOR>* intersectionPoints,
	std::vector<XMVECTOR>* aboveP,
	std::vector<XMVECTOR>* belowPlanePoints)
{
	ProceduralMeshData aboveMesh;

	ProceduralMeshData belowMesh;

	XMVECTOR origin;
	computeVertexCentroid(Mesh->vertices, origin);
	//origin.m128_f32[1] += 25;

	if (belowPlanePoints != nullptr)
		belowPlanePoints->push_back(origin);

	XMVECTOR projectedOnPlane = projectOnPlane(origin, plane);
	if (aboveP != nullptr)
		aboveP->push_back(projectedOnPlane);

	std::vector<VertexType> intersectionVertices;

	for (int i = 0; i < Mesh->indices.size(); i += 3)
	{
		auto A = XMLoadFloat3(&Mesh->vertices[i].position);
		auto B = XMLoadFloat3(&Mesh->vertices[i + 1].position);
		auto C = XMLoadFloat3(&Mesh->vertices[i + 2].position);

		A.m128_f32[3] = 1;
		B.m128_f32[3] = 1;
		C.m128_f32[3] = 1;

		//+1 for each vertex above or on the plane
		int planeIntersectionScore = 0;

		//In Order A B C
		XMVECTOR triVertices[3] = { A,B,C };
		bool aboveOrOnPlaneVertex[3] = { isAboveOrOnPlane(plane,A),isAboveOrOnPlane(plane,B),isAboveOrOnPlane(plane,C) };

		planeIntersectionScore += isAboveOrOnPlane(plane, A) + isAboveOrOnPlane(plane, B) + isAboveOrOnPlane(plane, C);

		XMVECTOR temp;

		//3 means that all of triangle is above plane
		if (planeIntersectionScore == 3)
		{
			//Add in order of reading to preserce triangle orientation

			addTriangle(aboveMesh,
				Mesh->vertices[i],
				Mesh->vertices[i + 1],
				Mesh->vertices[i + 2]);
		}
		else if (planeIntersectionScore == 0)
		{
			//Add in order of reading to preserce triangle orientation
			addTriangle(belowMesh,
				Mesh->vertices[i],
				Mesh->vertices[i + 1],
				Mesh->vertices[i + 2]
			);
		}
		else
		{
			bool neededValue = true;
			ProceduralMeshData* meshOnSingledSide = &belowMesh;
			ProceduralMeshData* meshOnOtherSide = &aboveMesh;
			// On vertex is below the plane, two are above
			if (planeIntersectionScore == 2)
			{
				neededValue = false;
				meshOnSingledSide = &belowMesh;
				meshOnOtherSide = &aboveMesh;
			}
			else {
				neededValue = true;
				meshOnSingledSide = &aboveMesh;
				meshOnOtherSide = &belowMesh;
			}

			int iSingled = 999;
			for (int j = 0; j < 3; j++)
			{
				if (aboveOrOnPlaneVertex[j] == neededValue)
				{
					iSingled = j;
				}
			}
			int iOtherOne = indexWrap(iSingled - 1, 3);
			int iOtherTwo = indexWrap(iSingled + 1, 3);

			//This is the only triangle added to the singled out mesh
			//Triangle one - singled vertex plus two intesections

			VertexType intersectionSingledAndOne = calculateIntersectionVertex(
				plane, Mesh->vertices[i + iSingled], Mesh->vertices[i + iOtherOne]);
			VertexType intersectionSingledAndTwo = calculateIntersectionVertex(
				plane, Mesh->vertices[i + iSingled], Mesh->vertices[i + iOtherTwo]);

			if (isnan(intersectionSingledAndOne.position.x))
				continue;
			if (isnan(intersectionSingledAndTwo.position.x))
				continue;

			VertexType midPointOther = lerpVertex(
				Mesh->vertices[i + iOtherOne],
				Mesh->vertices[i + iOtherTwo],
				0.5f
			);

			//Add to intersection vertices collection
			intersectionVertices.push_back(intersectionSingledAndOne);
			intersectionVertices.push_back(intersectionSingledAndTwo);

			if (intersectionPoints != nullptr)
			{
				XMVECTOR temp = XMLoadFloat3(&intersectionSingledAndOne.position);
				intersectionPoints->push_back(temp);
				temp = XMLoadFloat3(&intersectionSingledAndTwo.position);
				intersectionPoints->push_back(temp);
			}

			//Plane from the vertices as ordered in the mesh
			XMVECTOR desiredDirection = XMPlaneFromPoints(triVertices[0], triVertices[1], triVertices[2]);
			//XMVECTOR desiredDirection = XMPlaneFromPoints(triVertices[0], triVertices[2], triVertices[1]);

			//Set the plane's fourth component to 0 - only normal is needed
			desiredDirection.m128_f32[3] = 0;

			//Face the same as the original triangle

			//T
			addTriangleFacingDirection(*meshOnSingledSide,
				Mesh->vertices[i + iSingled],
				intersectionSingledAndOne,
				intersectionSingledAndTwo,
				desiredDirection);

			//Triangle two - other vertex one, mid-point, intersection
			addTriangleFacingDirection(*meshOnOtherSide,
				Mesh->vertices[i + iOtherOne],
				midPointOther,
				intersectionSingledAndOne,
				desiredDirection);

			//Triangle three - other vertex two, mid-point, intersection
			addTriangleFacingDirection(*meshOnOtherSide,
				Mesh->vertices[i + iOtherTwo],
				midPointOther,
				intersectionSingledAndTwo,
				desiredDirection

			);

			//Triangle four - intersection, intersection, midpoint
			addTriangleFacingDirection(*meshOnOtherSide,
				intersectionSingledAndOne,
				midPointOther,
				intersectionSingledAndTwo,
				desiredDirection
			);

			continue;
			// On vertex is above the plane, two are below
		}
	}

	VertexType centerVertex;
	centerVertex.position;
	XMStoreFloat3(&centerVertex.position, projectedOnPlane);
	centerVertex.texture.x = 0;
	centerVertex.texture.y = 0;

	//FOR ABOVE MESH
	//VertexType aboveMeshCenter;
	XMVECTOR aboveMeshCenterOrigin;
	computeVertexCentroid(aboveMesh.vertices, aboveMeshCenterOrigin);
	//Desired direction is from the center of the plane toe the center of the newcly created mesh
	// Or in other words inward
	//XMVECTOR abovedesiredDirection = aboveMeshCenterOrigin - projectedOnPlane;
	XMVECTOR abovedesiredDirection = projectedOnPlane - aboveMeshCenterOrigin;
	//buildTriangleFan(&aboveMesh, centerVertex, intersectionVertices, abovedesiredDirection);
	buildTriangleFan(aboveMesh, centerVertex, intersectionVertices, -plane);

	XMVECTOR belowMeshCenterOrigin;
	computeVertexCentroid(belowMesh.vertices, belowMeshCenterOrigin);
	//Desired direction is from the center of the plane toe the center of the newcly created mesh
	// Or in other words inward
	//jXMVECTOR belowdesiredDirection = belowMeshCenterOrigin - projectedOnPlane;
	XMVECTOR belowdesiredDirection = projectedOnPlane - belowMeshCenterOrigin;
	//buildTriangleFan(&belowMesh, centerVertex, intersectionVertices, belowdesiredDirection);
	buildTriangleFan(belowMesh, centerVertex, intersectionVertices, plane);

	BaseMesh* above = nullptr;
	BaseMesh* below = nullptr;

	std::vector<BaseMesh*> meshes;
	ProceduralMeshA* AboveMesh = new ProceduralMeshA(
		Mesh->device, Mesh->deviceContext,
		aboveMesh.vertices, aboveMesh.indices);

	above = AboveMesh;

	ProceduralMeshA* BelowMesh = new ProceduralMeshA(
		Mesh->device, Mesh->deviceContext,
		belowMesh.vertices, belowMesh.indices);

	below = BelowMesh;

	meshes.push_back(above);
	meshes.push_back(below);
	return meshes;
}

void ProceduralDestruction::buildTriangleFan(ProceduralMeshData& meshData, VertexType center, std::vector<VertexType>& points, XMVECTOR triangleDirection)
{
	if (points.size() <= 0) return;

	for (int i = 0; i < points.size() - 1; i += 2)
	{
		//Make it two-sided for now. TODO needs to change for the different meshes
			//These triangles are to be added facing inwards of the mesh.

		addTriangleFacingDirection(meshData,
			center,
			points[i],
			points[i + 1],
			triangleDirection);
	}
}

XMVECTOR ProceduralDestruction::projectOnPlane(XMVECTOR point, XMVECTOR plane)
{
	// Extract and normalize the plane normal
	XMVECTOR normal = XMVector3Normalize(XMVectorSet(plane.m128_f32[0], plane.m128_f32[1], plane.m128_f32[2], 0.0f));
	float d = plane.m128_f32[3];

	// Calculate the signed distance from the point to the plane
	//XMVECTOR distance = XMVector3Dot(point, normal) + XMVectorReplicate(d);
	float distance = XMVector3Dot(point, normal).m128_f32[0] + d;

	// Project the point onto the plane
	//XMVECTOR projectedPoint = point - distance * normal;
	XMVECTOR projectedPoint = XMVectorSubtract(point, normal * distance);

	return projectedPoint;
}

void ProceduralDestruction::addTriangle(ProceduralMeshData& aboveMesh, VertexType one, VertexType two, VertexType three)
{
	aboveMesh.vertices.push_back(one);
	aboveMesh.vertices.push_back(two);
	aboveMesh.vertices.push_back(three);

	aboveMesh.indices.push_back(aboveMesh.indices.size());
	aboveMesh.indices.push_back(aboveMesh.indices.size());
	aboveMesh.indices.push_back(aboveMesh.indices.size());
}
void ProceduralDestruction::addTriangleFacingDirection(ProceduralMeshData& aboveMesh, VertexType one, VertexType two, VertexType three, XMVECTOR desiredDirection)
{
	desiredDirection.m128_f32[3] = 0;
	desiredDirection = XMVector3Normalize(desiredDirection);
	XMVECTOR tempPointOne, tempPointTwo, tempPointThree;
	tempPointOne = XMLoadFloat3(&one.position);
	tempPointTwo = XMLoadFloat3(&two.position);
	tempPointThree = XMLoadFloat3(&three.position);
	XMVECTOR plane = XMPlaneFromPoints(tempPointOne, tempPointTwo, tempPointThree);

	//float dot = XMVector3Dot(plane, desiredDirection).m128_f32[0];
	float dot = XMPlaneDotNormal(plane, desiredDirection).m128_f32[0];

	//Facing general direction in order P1, P2, P3
	if (dot >= DOT_EPSILON)
	{
		XMStoreFloat3(&one.normal, -plane);
		XMStoreFloat3(&two.normal, -plane);
		XMStoreFloat3(&three.normal, -plane);
		addTriangle(aboveMesh, one, two, three);
	}
	//Flip to face opposite direction. P1, P3, P2
	else
	{
		XMStoreFloat3(&one.normal, plane);
		XMStoreFloat3(&two.normal, plane);
		XMStoreFloat3(&three.normal, plane);
		addTriangle(aboveMesh, one, three, two);
	}
}
void ProceduralDestruction::addTriangleTwoSided(ProceduralMeshData& aboveMesh, BaseMesh* Mesh, VertexType one, VertexType two, VertexType three)
{
	aboveMesh.vertices.push_back(one);
	aboveMesh.vertices.push_back(two);
	aboveMesh.vertices.push_back(three);

	aboveMesh.indices.push_back(aboveMesh.indices.size());
	aboveMesh.indices.push_back(aboveMesh.indices.size());
	aboveMesh.indices.push_back(aboveMesh.indices.size());

	aboveMesh.vertices.push_back(one);
	aboveMesh.vertices.push_back(three);
	aboveMesh.vertices.push_back(two);

	aboveMesh.indices.push_back(aboveMesh.indices.size());
	aboveMesh.indices.push_back(aboveMesh.indices.size());
	aboveMesh.indices.push_back(aboveMesh.indices.size());
}

VertexType ProceduralDestruction::calculateIntersectionVertex(XMVECTOR plane, VertexType a, VertexType b)
{
	XMVECTOR aVec = XMLoadFloat3(&a.position);
	aVec = XMVectorSetW(aVec, 1);
	XMVECTOR bVec = XMLoadFloat3(&b.position);
	bVec = XMVectorSetW(bVec, 1);

	XMVECTOR cVec = XMPlaneIntersectLine(plane, aVec, bVec);

	if (XMVector3IsNaN(cVec))
	{
		int a = 312;
		VertexType nan;
		nan.position.x = cVec.m128_f32[0];
		nan.position.y = cVec.m128_f32[1];
		nan.position.z = cVec.m128_f32[2];
		return nan;
	}

	float tUpper = XMVector3Dot(XMVectorSubtract(cVec, aVec), XMVectorSubtract(bVec, aVec)).m128_f32[0];
	float tLower = XMVector3Dot(XMVectorSubtract(bVec, aVec), XMVectorSubtract(bVec, aVec)).m128_f32[0];

	if (tLower == 0)
		return lerpVertex(a, b, 0);
	float t = tUpper / tLower;

	return lerpVertex(a, b, t);
}

VertexType ProceduralDestruction::lerpVertex(VertexType a, VertexType b, float t)
{
	VertexType interpolated;
	interpolated.position.x = (1 - t) * a.position.x + t * b.position.x;
	interpolated.position.y = (1 - t) * a.position.y + t * b.position.y;
	interpolated.position.z = (1 - t) * a.position.z + t * b.position.z;

	interpolated.normal.x = (1 - t) * a.normal.x + t * b.normal.x;
	interpolated.normal.y = (1 - t) * a.normal.y + t * b.normal.y;
	interpolated.normal.z = (1 - t) * a.normal.z + t * b.normal.z;

	interpolated.texture.x = (1 - t) * a.texture.x + t * b.texture.x;
	interpolated.texture.y = (1 - t) * a.texture.y + t * b.texture.y;

	return interpolated;
}

int ProceduralDestruction::indexWrap(int desired, int maxSize)
{
	if (desired < 0)
	{
		desired = maxSize + desired;
	}
	if (desired >= maxSize)
	{
		desired = desired - maxSize;
	};
	return desired;
}

void ProceduralDestruction::translateMeshToOrigin(ProceduralMeshData& aboveMesh, XMVECTOR origin)
{
	XMVECTOR currentVertex = XMVectorSet(0, 0, 0, 1);
	for (int i = 0; i < aboveMesh.vertices.size(); i++)
	{
		aboveMesh.vertices[i].position.x -= XMVectorGetX(origin);
		aboveMesh.vertices[i].position.y -= XMVectorGetY(origin);
		aboveMesh.vertices[i].position.z -= XMVectorGetZ(origin);
	}
}

void ProceduralDestruction::computeVertexCentroid(std::vector<VertexType>& vertices, DirectX::XMVECTOR& vertexCenter)
{
	XMVECTOR currentVertex;
	vertexCenter = XMVectorSet(0, 0, 0, 1);
	for (int i = 0; i < vertices.size(); i++)
	{
		currentVertex = XMLoadFloat3(&vertices[i].position);
		vertexCenter += currentVertex;
	}
	vertexCenter /= vertices.size();
}

void ProceduralDestruction::classifyVerticesAndEdgeIntersections(XMVECTOR plane, BaseMesh* Mesh, std::vector<XMVECTOR>& pAbove, std::vector<XMVECTOR>& pBelow, std::vector<XMVECTOR>& pIntersections)
{
	pAbove.clear();
	pBelow.clear();
	pIntersections.clear();

	int intersectionCount = 0;
	for (int i = 0; i < Mesh->indices.size(); i += 3)
	{
		auto A = XMLoadFloat3(&Mesh->vertices[i].position);
		auto B = XMLoadFloat3(&Mesh->vertices[i + 1].position);
		auto C = XMLoadFloat3(&Mesh->vertices[i + 2].position);
		A.m128_f32[3] = 1;
		B.m128_f32[3] = 1;
		C.m128_f32[3] = 1;

		bool isAboveA = isAboveOrOnPlane(plane, A);
		bool isAboveB = isAboveOrOnPlane(plane, B);
		bool isAboveC = isAboveOrOnPlane(plane, C);

		//AB
		if (isAboveA != isAboveB)	//Checking for difference in sign
		{
			//Intersection on AB
			XMVECTOR intersection = XMPlaneIntersectLine(plane, A, B);
			pIntersections.push_back(intersection);

			if (isAboveOrOnPlane(plane, A))
				pAbovePlane.push_back(A);
			else
				pBelowPlane.push_back(A);

			if (isAboveOrOnPlane(plane, B))
				pAbovePlane.push_back(B);
			else
				pBelowPlane.push_back(B);
		}
		if (isAboveA != isAboveC)	//Checking for difference in sign
		{
			//Intersection on AC
			XMVECTOR intersection = XMPlaneIntersectLine(plane, A, C);
			pIntersections.push_back(intersection);
			if (isAboveOrOnPlane(plane, A))
				pAbovePlane.push_back(A);
			else
				pBelowPlane.push_back(A);

			if (isAboveOrOnPlane(plane, C))
				pAbovePlane.push_back(C);
			else
				pBelowPlane.push_back(C);
		}
		if (isAboveB != isAboveC)	//Checking for difference in sign
		{
			//Intersection on BC
			XMVECTOR intersection = XMPlaneIntersectLine(plane, B, C);
			pIntersections.push_back(intersection);

			if (isAboveOrOnPlane(plane, B))
				pAbovePlane.push_back(B);
			else
				pBelowPlane.push_back(B);

			if (isAboveOrOnPlane(plane, C))
				pAbovePlane.push_back(C);
			else
				pBelowPlane.push_back(C);
		}
	}
}

std::vector<MeshInstance*> ProceduralDestruction::radialPlaneCutsXZEvenlySpaced(XMVECTOR point, BaseMesh* toSplit)
{
	std::vector<MeshInstance*> destructrableComponents;
	if (toSplit == nullptr) return destructrableComponents;

	XMVECTOR planeVector;

	//Generate plane from intersection points
	//And normals in a circle in XY - plane
	//int planeCount = 3;
	int planeCount = 3;
	float incrementDegrees = 360 / planeCount;
	//float incrementDegrees = 45;

	std::vector<BaseMesh*> splits;
	std::vector<BaseMesh*> nextSplits;
	splits.push_back(toSplit);

	pIntersections.clear();
	pAbovePlane.clear();
	pBelowPlane.clear();

	for (int i = 0; i < planeCount; i++)
	{
		float deg = (i + 1) * incrementDegrees;
		float rads = DirectX::XMConvertToRadians(deg);

		XMVECTOR normal;
		normal = XMVectorSet(1 * cosf(rads), 1 * sinf(rads), 0, 0);
		normal = XMVector3Normalize(normal);
		planeVector = XMPlaneFromPointNormal(point, normal);

		for (int j = 0; j < splits.size(); j++)
		{
			auto subsplits = cutMeshClosed(planeVector, splits[j]);
			//Add all subsplits to the next splits
			for (int k = 0; k < subsplits.size(); k++)
			{
				nextSplits.push_back(subsplits[k]);
			}
		}

		splits = nextSplits;
		//Debug Visualize all splits
		nextSplits.clear();
	}

	int debugiter = 0;

	for each (BaseMesh * s in splits)
	{
		if (s->vertices.size() <= 0)
			continue;
		auto serializableMesh = SerializableMesh::ProceduralMesh(
			"De", s, true, toSplit->device, toSplit->deviceContext);
		MeshInstance* ms = new MeshInstance();
		ms->setMesh(serializableMesh);
		destructrableComponents.push_back(ms);
		debugiter++;
	}

	return destructrableComponents;
}

std::vector<MeshInstance*> ProceduralDestruction::radialPlaneCutsRandom(XMVECTOR point, BaseMesh* toSplit)
{
	std::vector<MeshInstance*> destructrableComponents;
	if (toSplit == nullptr) return destructrableComponents;

	XMVECTOR planeVector;

	std::vector<BaseMesh*> splits;
	std::vector<BaseMesh*> nextSplits;
	splits.push_back(toSplit);
	int randomPlaneCount = 5;

	for (int i = 0; i < randomPlaneCount; i++)
	{
		XMVECTOR rNormal = randomPointOnUnitSphere(1);
		XMVECTOR rPosition = XMVectorAdd(point, (rNormal));
		planeVector = XMPlaneFromPointNormal(rPosition, rNormal);

		for (int j = 0; j < splits.size(); j++)
		{
			auto subsplits = cutMeshClosed(planeVector, splits[j]);
			//auto subsplits = splitMesh(planeVector, splits[j]);
			//Add all subsplits to the next splits
			for (int k = 0; k < subsplits.size(); k++)
			{
				nextSplits.push_back(subsplits[k]);
			}
		}

		splits = nextSplits;
		nextSplits.clear();
	}
	for each (BaseMesh * s in splits)
	{
		if (s->vertices.size() <= 0)
			continue;

		XMVECTOR origin;
		this->computeVertexCentroid(s->vertices, origin);
		s->translateMeshToOrigin(s->device, origin);

		auto serializableMesh = SerializableMesh::ProceduralMesh(
			"De", s, true, toSplit->device, toSplit->deviceContext);

		MeshInstance* ms = new MeshInstance();
		XMVECTOR vertexCenter;
		computeVertexCentroid(s->vertices, vertexCenter);
		ms->setMesh(serializableMesh);
		ms->transform.setPosition(XMVectorGetX(origin), XMVectorGetY(origin), XMVectorGetZ(origin));
		//ms->setMaterial(assetSystem->getMaterial("Rock"));

		destructrableComponents.push_back(ms);
	}

	return destructrableComponents;
}

XMVECTOR ProceduralDestruction::randomPointOnUnitSphere(double radius)
{
	//Using C++ Random library is good practice even though
	// the sample size is too small to make a difference
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<> dist(0.0, 1.0);

	float u = dist(gen);
	float v = dist(gen);

	float theta = 2.0 * XM_PI * u;
	float phi = acos(2.0 * v - 1.0);

	float x = radius * sinf(phi) * cosf(theta);
	float y = radius * sinf(phi) * sinf(theta);
	float z = radius * cosf(phi);

	XMVECTOR toReturn = XMVectorSet(x, y, z, 1);
	return toReturn;
}
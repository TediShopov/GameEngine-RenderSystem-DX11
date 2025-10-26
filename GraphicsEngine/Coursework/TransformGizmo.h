#pragma once
#include "Input.h"
#include "Transform.h"
#include "MeshInstance.h"
enum class GizmoState {
	IDLE, HOVERED, ACTIVE
};
struct TransformDragState {
	XMVECTOR globalRayGizmoHit;
	XMVECTOR localRayGizmoHit;
};

class TransformGizmo : public MeshInstance 
{
public:
	int sWidth = 0;
	int sHeight = 0;
	GizmoState state;
	XMVECTOR axis;

	TransformGizmo(XMFLOAT3 a,int w, int h);
	TransformGizmo(XMFLOAT3 a,int w, int h,SerializableMesh m);

	GizmoState updateState(const Input& in, const Camera& camera, XMMATRIX projection, MeshInstance* target);

	void update(const Input& in, const Camera& camera, XMMATRIX projection,MeshInstance* target);

	void computeLocalIntersection(std::pair<XMVECTOR,XMVECTOR> origDir);
	bool rayPlaneIntersect(FXMVECTOR rayOrigin, FXMVECTOR rayDir, FXMVECTOR plane, XMVECTOR& hit);

	//Computes the plane that the gizmos lies in. Constructed from the camera forward vectors (usually)
	//Is constrcuted from the the cross vectors of the gizmos axis to prevent instability in floating point operations
	XMVECTOR computeDragPlane(const Camera& camera);
	std::pair<XMVECTOR,XMVECTOR> computeGlobalRayFromCursor(const Input& in,XMMATRIX view, XMMATRIX projection);
	

private:
	MeshInstance* target;
	std::vector<DirectX::XMVECTOR> rayHisPointsLocal;
	XMVECTOR hitPointToOriginOnHit;
	bool hasRayHit = false;
};


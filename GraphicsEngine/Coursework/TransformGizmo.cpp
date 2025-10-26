#include "TransformGizmo.h"

 TransformGizmo::TransformGizmo(XMFLOAT3 a, int w, int h) : MeshInstance(), sWidth(w), sHeight(h), target(nullptr), axis(XMVectorSet(a.x, a.y, a.z, 0)) {

}

 TransformGizmo::TransformGizmo(XMFLOAT3 a, int w, int h, SerializableMesh m) : MeshInstance(m), sWidth(w), sHeight(h), target(nullptr), axis(XMVectorSet(a.x, a.y, a.z, 0))
{
}

 GizmoState TransformGizmo::updateState(const Input& in, const Camera& camera, XMMATRIX projection, MeshInstance* target)
{
	target = target;
	this->transform.setPosition(target->transform.getGlobalPosition());
	//Test Intersectoin
	auto globalVec = computeGlobalRayFromCursor(in, camera.getViewMatrix(), projection);
	computeLocalIntersection(globalVec);

	if (state != GizmoState::ACTIVE)
	{
		if (this->hasRayHit)
			state = GizmoState::HOVERED;
		else
			state = GizmoState::IDLE;

		if (in.isLeftMouseDown() && state == GizmoState::HOVERED)
		{

			auto dragPlane = computeDragPlane(camera);

			auto globalVec = computeGlobalRayFromCursor(in, camera.getViewMatrix(), projection);

			XMVECTOR hitPoint;
			rayPlaneIntersect(globalVec.first, globalVec.second, dragPlane, hitPoint);
			hitPointToOriginOnHit = XMVectorSubtract(hitPoint, this->transform.getGlobalPosition());


			state = GizmoState::ACTIVE;
		}

	}
	else
	{
		if (in.isLeftMouseDown() == false)
		{
			state = GizmoState::IDLE;
		}
	}
	return state;







}

 void TransformGizmo::update(const Input& in, const Camera& camera, XMMATRIX projection, MeshInstance* target)
{
	target = target;

	XMVECTOR hitPoint;
	auto dragPlane = computeDragPlane(camera);

	auto globalVec = computeGlobalRayFromCursor(in, camera.getViewMatrix(), projection);

	if (rayPlaneIntersect(globalVec.first, globalVec.second, dragPlane, hitPoint))
	{
		// Compute projected delta along gizmoAxis
		auto newRelativePosition = XMVectorAdd(this->transform.getGlobalPosition(), this->hitPointToOriginOnHit);
		XMVECTOR delta = XMVectorSubtract(hitPoint, newRelativePosition);
		XMVECTOR projected = XMVectorMultiply(XMVector3Dot(delta, this->axis), this->axis);
		target->transform.setPosition(XMVectorAdd(this->transform.getGlobalPosition(), projected));
	}
}

//Computes the plane that the gizmos lies in. Constructed from the camera forward vectors (usually)
//Is constrcuted from the the cross vectors of the gizmos axis to prevent instability in floating point operations

 XMVECTOR TransformGizmo::computeDragPlane(const Camera& camera)
{
	XMVECTOR globalAxis = XMVector4Transform(this->axis, this->transform.getTransformMatrix());
	globalAxis = XMVector4Normalize(globalAxis);
	XMVECTOR cameraForward = camera.getForwardVector();
	// Ensure the plane normal isn't parallel to the gizmo axis
	float dot = fabsf(XMVectorGetX(XMVector3Dot(cameraForward, globalAxis)));
	XMVECTOR planeNormal = (dot > 0.95f)
		? XMVector3Normalize(XMVector3Cross(globalAxis, XMVectorSet(0, 1, 0, 0)))
		: cameraForward;
	planeNormal = XMVector3Normalize(planeNormal);
	XMVECTOR dragPlane = XMPlaneFromPointNormal(this->transform.getGlobalPosition(), planeNormal);
	return dragPlane;





}

 bool TransformGizmo::rayPlaneIntersect(FXMVECTOR rayOrigin, FXMVECTOR rayDir, FXMVECTOR plane, XMVECTOR& hit)
{
	float denom = XMVectorGetX(XMPlaneDotNormal(plane, rayDir));
	if (fabsf(denom) < 1e-6f) return false;

	float t = -XMVectorGetX(XMPlaneDotCoord(plane, rayOrigin)) / denom;
	if (t < 0.0f) return false;

	hit = XMVectorAdd(rayOrigin, (rayDir * t));
	return true;
}

 std::pair<XMVECTOR, XMVECTOR> TransformGizmo::computeGlobalRayFromCursor(const Input& in, XMMATRIX view, XMMATRIX projection)
{
	int mouseX = in.getMouseX();
	int mouseY = in.getMouseY();


	float ndcX = (2.0f * mouseX / sWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseY / sHeight); // flip Y

	XMMATRIX invProj = XMMatrixInverse(nullptr, projection);
	XMVECTOR rayClip = XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);

	XMVECTOR rayView = XMVector3TransformCoord(rayClip, invProj);
	rayView = XMVectorSetW(rayView, 0.0f); // direction, not point

	XMMATRIX invView = XMMatrixInverse(nullptr, view);

	XMVECTOR rayDirWorld = XMVector3TransformNormal(rayView, invView);
	rayDirWorld = XMVector3Normalize(rayDirWorld);

	XMVECTOR rayOriginWorld = XMVector3TransformCoord(XMVectorZero(), invView); // camera position
	return { rayOriginWorld,rayDirWorld };





}

 void TransformGizmo::computeLocalIntersection(std::pair<XMVECTOR, XMVECTOR> origDir)
{
	XMMATRIX inverseWorld = this->transform.getInverseMatrix();
	auto localOrigin = XMVector3TransformCoord(origDir.first, inverseWorld);
	auto localDir = XMVector3TransformNormal(origDir.second, inverseWorld);
	localDir = XMVector3Normalize(localDir);

	XMVECTOR i;

	this->rayHisPointsLocal.clear();
	this->hasRayHit = this->getMesh()->rayMeshIntersect(localOrigin, localDir, &i);

	if (this->hasRayHit)
		this->rayHisPointsLocal.push_back(i);

}

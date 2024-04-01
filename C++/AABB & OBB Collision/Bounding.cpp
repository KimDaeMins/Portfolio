#include "Bounding.h"

//흠....
CBounding::CBounding(_float3 Center, _float3 Extents, _float4 Orientation)
{
	m_eType = CBounding::TYPE_OBB;
	m_BaseCenter = Center;
	m_BaseExtents = Extents;
	m_BaseOrientation = Orientation;
}

CBounding::CBounding(_float3 Center, _float3 Extents)
{
	m_eType = CBounding::TYPE_AABB;
	m_BaseCenter = Center;
	m_BaseExtents = Extents;
	m_Orientation = _float4(0.f, 0.f, 0.f, 1.f);
}

CBounding::CBounding(_float3 Center, _float Radius)
{
	m_eType = CBounding::TYPE_SPHERE;
	m_BaseCenter = Center;
	m_BaseRadius = Radius;
}

void CBounding::Update(_matrix M)
{
	if (m_eType == CBounding::TYPE_AABB)
	{
		XMVECTOR vCenter = XMLoadFloat3(&m_BaseCenter);
		XMVECTOR vExtents = XMLoadFloat3(&m_BaseExtents);

		Remove_Rotation(M);

		XMVECTOR Corner = XMVectorMultiplyAdd(vExtents, g_BoxOffset[0], vCenter);
		Corner = XMVector3Transform(Corner, M);

		XMVECTOR Min, Max;
		Min = Max = Corner;

		for (size_t i = 1; i < CORNER_COUNT; ++i)
		{
			Corner = XMVectorMultiplyAdd(vExtents, g_BoxOffset[i], vCenter);
			Corner = XMVector3Transform(Corner, M);

			Min = XMVectorMin(Min, Corner);
			Max = XMVectorMax(Max, Corner);
		}

		XMStoreFloat3(&m_Center, XMVectorScale(XMVectorAdd(Min, Max), 0.5f));
		vExtents = XMVectorScale(XMVectorSubtract(Max, Min), 0.5f);
		XMStoreFloat3(&m_Extents, vExtents);
		m_Radius = XMVectorGetX(XMVector3Length(XMVectorScale(vExtents, m_CorrectionValue)));
	}
	else if (m_eType == CBounding::TYPE_OBB)
	{
		XMVECTOR vCenter = XMLoadFloat3(&m_BaseCenter);
		XMVECTOR vExtents = XMLoadFloat3(&m_BaseExtents);
		XMVECTOR vOrientation = XMLoadFloat4(&m_BaseOrientation);

		vCenter = XMVector3Transform(vCenter, M);
		XMMATRIX nM;
		nM.r[0] = XMVector3Normalize(M.r[0]);
		nM.r[1] = XMVector3Normalize(M.r[1]);
		nM.r[2] = XMVector3Normalize(M.r[2]);
		nM.r[3] = g_XMIdentityR3;
		XMVECTOR Rotation = XMQuaternionRotationMatrix(nM);
		vOrientation = XMQuaternionMultiply(vOrientation, Rotation);

		XMVECTOR dX = XMVector3Length(M.r[0]);
		XMVECTOR dY = XMVector3Length(M.r[1]);
		XMVECTOR dZ = XMVector3Length(M.r[2]);
		XMVECTOR VectorScale = XMVectorSelect(dY, dX, g_XMSelect1000);
		VectorScale = XMVectorSelect(dZ, VectorScale, g_XMSelect1100);
		vExtents = XMVectorMultiply(vExtents, VectorScale);

		XMStoreFloat3(&m_Center, vCenter);
		XMStoreFloat3(&m_Extents, vExtents);
		XMStoreFloat4(&m_Orientation, vOrientation);
		m_Radius = XMVectorGetX(XMVector3Length(XMVectorScale(vExtents, m_CorrectionValue)));
	}
	if (m_eType == CBounding::TYPE_SPHERE)
	{
		XMVECTOR vCenter = XMLoadFloat3(&m_BaseCenter);

		// 센터값 변경
		XMVECTOR C = XMVector3Transform(vCenter, M);

		XMVECTOR dX = XMVector3Dot(M.r[0], M.r[0]);
		XMVECTOR dY = XMVector3Dot(M.r[1], M.r[1]);
		XMVECTOR dZ = XMVector3Dot(M.r[2], M.r[2]);

		XMVECTOR d = XMVectorMax(dX, XMVectorMax(dY, dZ));
		XMStoreFloat3(&m_Center, C);

		// Radius값 변경
		float Scale = sqrtf(XMVectorGetX(d));
		m_Radius = m_BaseRadius * Scale;
	}
}


_bool CBounding::Collision(shared_ptr<CBounding> pTargetBounding)
{
	TYPE targetType = pTargetBounding->Get_Type();
	_bool isCollision = false;

	if (!SpheretoSphere(pTargetBounding))
		return false;

	if (m_eType == TYPE_AABB)
		switch (targetType)
		{
		case Engine::CBounding::TYPE_AABB:
			isCollision = AABBtoAABB(pTargetBounding);
		case Engine::CBounding::TYPE_OBB:
			isCollision = OBBtoOBB(pTargetBounding);
		case Engine::CBounding::TYPE_SPHERE:
			isCollision = AABBtoSphere(pTargetBounding);
		}
	else if (m_eType == TYPE_OBB)
		switch (targetType)
		{
		case Engine::CBounding::TYPE_AABB:
			isCollision = OBBtoOBB(pTargetBounding);
		case Engine::CBounding::TYPE_OBB:
			isCollision = OBBtoOBB(pTargetBounding);
		case Engine::CBounding::TYPE_SPHERE:
			isCollision = OBBtoSphere(pTargetBounding);
		}
	else if(m_eType == TYPE_SPHERE)
		switch (targetType)
		{
		case Engine::CBounding::TYPE_AABB:
			isCollision = pTargetBounding->AABBtoSphere(shared_from_this());
		case Engine::CBounding::TYPE_OBB:
			isCollision = pTargetBounding->OBBtoSphere(shared_from_this());
		case Engine::CBounding::TYPE_SPHERE:
			isCollision = true;
		}

	return isCollision;
}

_bool CBounding::AABBtoAABB(shared_ptr<CBounding> _Bounding)
{
	XMVECTOR CenterA = XMLoadFloat3(&m_Center);
	XMVECTOR ExtentsA = XMLoadFloat3(&m_Extents);

	XMVECTOR CenterB = XMLoadFloat3(&_Bounding->Get_Center());
	XMVECTOR ExtentsB = XMLoadFloat3(&_Bounding->Get_Extents());

	XMVECTOR MinA = XMVectorSubtract(CenterA, ExtentsA);
	XMVECTOR MaxA = XMVectorAdd(CenterA, ExtentsA);

	XMVECTOR MinB = XMVectorSubtract(CenterB, ExtentsB);
	XMVECTOR MaxB = XMVectorAdd(CenterB, ExtentsB);

	// for each i in (x, y, z) if a_min(i) > b_max(i) or b_min(i) > a_max(i) then return false
	XMVECTOR Disjoint = XMVectorOrInt(XMVectorGreater(MinA, MaxB), XMVectorGreater(MinB, MaxA));

	return !DirectX::Internal::XMVector3AnyTrue(Disjoint);
}

_bool CBounding::OBBtoOBB(shared_ptr<CBounding> _Bounding)
{
	_float			distance[2];
	_float			verdictDistance;
	OBBDESC			OBBDesc[2];

	OBBDesc[0] = Compute_OBBDesc(shared_from_this());
	OBBDesc[1] = Compute_OBBDesc(_Bounding);


	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			distance[0] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vExtentDirs[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vExtentDirs[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vExtentDirs[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			distance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vExtentDirs[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vExtentDirs[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vExtentDirs[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			verdictDistance = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter)
				- XMLoadFloat3(&OBBDesc[0].vCenter), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			if (distance[0] + distance[1] < verdictDistance)
				return false;
		}
	}
	return true;
}

_bool CBounding::OBBtoSphere(shared_ptr<CBounding> _Bounding)
{
	XMVECTOR SphereCenter = XMLoadFloat3(&_Bounding->Get_Center());
	_float radius = _Bounding->Get_Radius();
	XMVECTOR SphereRadius = XMVectorReplicatePtr(&radius);

	XMVECTOR BoxCenter = XMLoadFloat3(&m_Center);
	XMVECTOR BoxExtents = XMLoadFloat3(&m_Extents);
	XMVECTOR BoxOrientation = XMLoadFloat4(&m_Orientation);

	assert(DirectX::Internal::XMQuaternionIsUnit(BoxOrientation));

	// Transform the center of the sphere to be local to the box.
	// BoxMin = -BoxExtents
	// BoxMax = +BoxExtents
	SphereCenter = XMVector3InverseRotate(XMVectorSubtract(SphereCenter, BoxCenter), BoxOrientation);

	// Find the distance to the nearest point on the box.
	// for each i in (x, y, z)
	// if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
	// else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2

	XMVECTOR d = XMVectorZero();

	// Compute d for each dimension.
	XMVECTOR LessThanMin = XMVectorLess(SphereCenter, XMVectorNegate(BoxExtents));
	XMVECTOR GreaterThanMax = XMVectorGreater(SphereCenter, BoxExtents);

	XMVECTOR MinDelta = XMVectorAdd(SphereCenter, BoxExtents);
	XMVECTOR MaxDelta = XMVectorSubtract(SphereCenter, BoxExtents);

	// Choose value for each dimension based on the comparison.
	d = XMVectorSelect(d, MinDelta, LessThanMin);
	d = XMVectorSelect(d, MaxDelta, GreaterThanMax);

	// Use a dot-product to square them and sum them together.
	XMVECTOR d2 = XMVector3Dot(d, d);

	return XMVector4LessOrEqual(d2, XMVectorMultiply(SphereRadius, SphereRadius)) ? true : false;
}

_bool CBounding::AABBtoSphere(shared_ptr<CBounding> _Bounding)
{
	XMVECTOR SphereCenter = XMLoadFloat3(&_Bounding->Get_Center());
	_float radius = _Bounding->Get_Radius();
	XMVECTOR SphereRadius = XMVectorReplicatePtr(&radius);

	XMVECTOR BoxCenter = XMLoadFloat3(&m_Center);
	XMVECTOR BoxExtents = XMLoadFloat3(&m_Extents);

	XMVECTOR BoxMin = XMVectorSubtract(BoxCenter, BoxExtents);
	XMVECTOR BoxMax = XMVectorAdd(BoxCenter, BoxExtents);

	// Find the distance to the nearest point on the box.
	// for each i in (x, y, z)
	// if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
	// else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2

	XMVECTOR d = XMVectorZero();

	// Compute d for each dimension.
	XMVECTOR LessThanMin = XMVectorLess(SphereCenter, BoxMin);
	XMVECTOR GreaterThanMax = XMVectorGreater(SphereCenter, BoxMax);

	XMVECTOR MinDelta = XMVectorSubtract(SphereCenter, BoxMin);
	XMVECTOR MaxDelta = XMVectorSubtract(SphereCenter, BoxMax);

	// Choose value for each dimension based on the comparison.
	d = XMVectorSelect(d, MinDelta, LessThanMin);
	d = XMVectorSelect(d, MaxDelta, GreaterThanMax);

	// Use a dot-product to square them and sum them together.
	XMVECTOR d2 = XMVector3Dot(d, d);

	return XMVector3LessOrEqual(d2, XMVectorMultiply(SphereRadius, SphereRadius));
}

_bool CBounding::SpheretoSphere(shared_ptr<CBounding> _Bounding)
{
	XMVECTOR vCenterA = XMLoadFloat3(&m_Center);
	XMVECTOR vRadiusA = XMVectorReplicatePtr(&m_Radius);

	XMVECTOR vCenterB = XMLoadFloat3(&_Bounding->Get_Center());
	_float radiusB = _Bounding->Get_Radius();
	XMVECTOR vRadiusB = XMVectorReplicatePtr(&radiusB);

	XMVECTOR Delta = XMVectorSubtract(vCenterB, vCenterA);
	XMVECTOR DistanceSquared = XMVector3LengthSq(Delta);

	XMVECTOR RadiusSquared = XMVectorAdd(vRadiusA, vRadiusB);
	RadiusSquared = XMVectorMultiply(RadiusSquared, RadiusSquared);

	return XMVector3LessOrEqual(DistanceSquared, RadiusSquared);
}

CBounding::OBBDESC CBounding::Compute_OBBDesc(shared_ptr<CBounding> _Bounding)
{
	OBBDESC		OBBDesc;
	ZeroMemory(&OBBDesc, sizeof(OBBDESC));

	OBBDesc.vCenter = _Bounding->Get_Center();

	_vector		vPoints[8];
	XMVECTOR vCenter = XMLoadFloat3(&OBBDesc.vCenter);
	XMVECTOR vExtents = XMLoadFloat3(&_Bounding->Get_Extents());
	XMVECTOR vOrientation = XMLoadFloat4(&_Bounding->Get_Orientation());

	for (size_t i = 0; i < CORNER_COUNT; ++i)
	{
		vPoints[i] = XMVectorAdd(XMVector3Rotate(XMVectorMultiply(vExtents, g_BoxOffset[i]), vOrientation), vCenter);
	}

	XMStoreFloat3(&OBBDesc.vExtentDirs[0], (vPoints[2] + vPoints[5]) * 0.5f - XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vExtentDirs[1], (vPoints[2] + vPoints[7]) * 0.5f - XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vExtentDirs[2], (vPoints[2] + vPoints[0]) * 0.5f - XMLoadFloat3(&OBBDesc.vCenter));

	XMStoreFloat3(&OBBDesc.vAlignAxis[0], XMVector3Normalize(vPoints[2] - vPoints[3]));
	XMStoreFloat3(&OBBDesc.vAlignAxis[1], XMVector3Normalize(vPoints[2] - vPoints[1]));
	XMStoreFloat3(&OBBDesc.vAlignAxis[2], XMVector3Normalize(vPoints[2] - vPoints[6]));

	return OBBDesc;
}
_bool CBounding::Collision_Ray(_fvector _Origin, _fvector _Diraction, _float& _Distance)
{
	if (m_eType == TYPE_AABB)
	{
		assert(DirectX::Internal::XMVector3IsUnit(_Diraction));

		// Load the box.
		XMVECTOR vCenter = XMLoadFloat3(&m_Center);
		XMVECTOR vExtents = XMLoadFloat3(&m_Extents);

		// Adjust ray origin to be relative to center of the box.
		XMVECTOR TOrigin = XMVectorSubtract(vCenter, _Origin);

		// Compute the dot product againt each axis of the box.
		// Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
		XMVECTOR AxisDotOrigin = TOrigin;
		XMVECTOR AxisDotDirection = _Diraction;

		// if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
		XMVECTOR IsParallel = XMVectorLessOrEqual(XMVectorAbs(AxisDotDirection), g_RayEpsilon);

		// Test against all three axii simultaneously.
		XMVECTOR InverseAxisDotDirection = XMVectorReciprocal(AxisDotDirection);
		XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(AxisDotOrigin, vExtents), InverseAxisDotDirection);
		XMVECTOR t2 = XMVectorMultiply(XMVectorAdd(AxisDotOrigin, vExtents), InverseAxisDotDirection);

		// Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
		// use the results from any directions parallel to the slab.
		XMVECTOR t_min = XMVectorSelect(XMVectorMin(t1, t2), g_FltMin, IsParallel);
		XMVECTOR t_max = XMVectorSelect(XMVectorMax(t1, t2), g_FltMax, IsParallel);

		// t_min.x = maximum( t_min.x, t_min.y, t_min.z );
		// t_max.x = minimum( t_max.x, t_max.y, t_max.z );
		t_min = XMVectorMax(t_min, XMVectorSplatY(t_min));  // x = max(x,y)
		t_min = XMVectorMax(t_min, XMVectorSplatZ(t_min));  // x = max(max(x,y),z)
		t_max = XMVectorMin(t_max, XMVectorSplatY(t_max));  // x = min(x,y)
		t_max = XMVectorMin(t_max, XMVectorSplatZ(t_max));  // x = min(min(x,y),z)

		// if ( t_min > t_max ) return false;
		XMVECTOR NoIntersection = XMVectorGreater(XMVectorSplatX(t_min), XMVectorSplatX(t_max));

		// if ( t_max < 0.0f ) return false;
		NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(XMVectorSplatX(t_max), XMVectorZero()));

		// if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return false;
		XMVECTOR ParallelOverlap = XMVectorInBounds(AxisDotOrigin, vExtents);
		NoIntersection = XMVectorOrInt(NoIntersection, XMVectorAndCInt(IsParallel, ParallelOverlap));

		if (!DirectX::Internal::XMVector3AnyTrue(NoIntersection))
		{
			// Store the x-component to *pDist
			XMStoreFloat(&_Distance, t_min);
			return true;
		}

		_Distance = 0.f;
		return false;
	}
	else if (m_eType == TYPE_OBB)
	{
		assert(DirectX::Internal::XMVector3IsUnit(_Diraction));

		static const XMVECTORU32 SelectY = { { { XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0 } } };
		static const XMVECTORU32 SelectZ = { { { XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0 } } };

		// Load the box.
		XMVECTOR vCenter = XMLoadFloat3(&m_Center);
		XMVECTOR vExtents = XMLoadFloat3(&m_Extents);
		XMVECTOR vOrientation = XMLoadFloat4(&m_Orientation);

		assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

		// Get the boxes normalized side directions.
		XMMATRIX R = XMMatrixRotationQuaternion(vOrientation);

		// Adjust ray origin to be relative to center of the box.
		XMVECTOR TOrigin = XMVectorSubtract(vCenter, _Origin);

		// Compute the dot product againt each axis of the box.
		XMVECTOR AxisDotOrigin = XMVector3Dot(R.r[0], TOrigin);
		AxisDotOrigin = XMVectorSelect(AxisDotOrigin, XMVector3Dot(R.r[1], TOrigin), SelectY);
		AxisDotOrigin = XMVectorSelect(AxisDotOrigin, XMVector3Dot(R.r[2], TOrigin), SelectZ);

		XMVECTOR AxisDotDirection = XMVector3Dot(R.r[0], _Diraction);
		AxisDotDirection = XMVectorSelect(AxisDotDirection, XMVector3Dot(R.r[1], _Diraction), SelectY);
		AxisDotDirection = XMVectorSelect(AxisDotDirection, XMVector3Dot(R.r[2], _Diraction), SelectZ);

		// if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
		XMVECTOR IsParallel = XMVectorLessOrEqual(XMVectorAbs(AxisDotDirection), g_RayEpsilon);

		// Test against all three axes simultaneously.
		XMVECTOR InverseAxisDotDirection = XMVectorReciprocal(AxisDotDirection);
		XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(AxisDotOrigin, vExtents), InverseAxisDotDirection);
		XMVECTOR t2 = XMVectorMultiply(XMVectorAdd(AxisDotOrigin, vExtents), InverseAxisDotDirection);

		// Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
		// use the results from any directions parallel to the slab.
		XMVECTOR t_min = XMVectorSelect(XMVectorMin(t1, t2), g_FltMin, IsParallel);
		XMVECTOR t_max = XMVectorSelect(XMVectorMax(t1, t2), g_FltMax, IsParallel);

		// t_min.x = maximum( t_min.x, t_min.y, t_min.z );
		// t_max.x = minimum( t_max.x, t_max.y, t_max.z );
		t_min = XMVectorMax(t_min, XMVectorSplatY(t_min));  // x = max(x,y)
		t_min = XMVectorMax(t_min, XMVectorSplatZ(t_min));  // x = max(max(x,y),z)
		t_max = XMVectorMin(t_max, XMVectorSplatY(t_max));  // x = min(x,y)
		t_max = XMVectorMin(t_max, XMVectorSplatZ(t_max));  // x = min(min(x,y),z)

		// if ( t_min > t_max ) return false;
		XMVECTOR NoIntersection = XMVectorGreater(XMVectorSplatX(t_min), XMVectorSplatX(t_max));

		// if ( t_max < 0.0f ) return false;
		NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(XMVectorSplatX(t_max), XMVectorZero()));

		// if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return false;
		XMVECTOR ParallelOverlap = XMVectorInBounds(AxisDotOrigin, vExtents);
		NoIntersection = XMVectorOrInt(NoIntersection, XMVectorAndCInt(IsParallel, ParallelOverlap));

		if (!DirectX::Internal::XMVector3AnyTrue(NoIntersection))
		{
			// Store the x-component to *pDist
			XMStoreFloat(&_Distance, t_min);
			return true;
		}

		_Distance = 0.f;
		return false;
	}
	else if (m_eType == TYPE_SPHERE)
	{
		assert(DirectX::Internal::XMVector3IsUnit(_Diraction));

		XMVECTOR vCenter = XMLoadFloat3(&m_Center);
		XMVECTOR vRadius = XMVectorReplicatePtr(&m_Radius);

		// l is the vector from the ray origin to the center of the sphere.
		XMVECTOR l = XMVectorSubtract(vCenter, _Origin);

		// s is the projection of the l onto the ray direction.
		XMVECTOR s = XMVector3Dot(l, _Diraction);

		XMVECTOR l2 = XMVector3Dot(l, l);

		XMVECTOR r2 = XMVectorMultiply(vRadius, vRadius);

		// m2 is squared distance from the center of the sphere to the projection.
		XMVECTOR m2 = XMVectorNegativeMultiplySubtract(s, s, l2);

		XMVECTOR NoIntersection;

		// If the ray origin is outside the sphere and the center of the sphere is
		// behind the ray origin there is no intersection.
		NoIntersection = XMVectorAndInt(XMVectorLess(s, XMVectorZero()), XMVectorGreater(l2, r2));

		// If the squared distance from the center of the sphere to the projection
		// is greater than the radius squared the ray will miss the sphere.
		NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(m2, r2));

		// The ray hits the sphere, compute the nearest intersection point.
		XMVECTOR q = XMVectorSqrt(XMVectorSubtract(r2, m2));
		XMVECTOR t1 = XMVectorSubtract(s, q);
		XMVECTOR t2 = XMVectorAdd(s, q);

		XMVECTOR OriginInside = XMVectorLessOrEqual(l2, r2);
		XMVECTOR t = XMVectorSelect(t1, t2, OriginInside);

		if (XMVector4NotEqualInt(NoIntersection, XMVectorTrueInt()))
		{
			// Store the x-component to *pDist.
			XMStoreFloat(&_Distance, t);
			return true;
		}

		_Distance = 0.f;
		return false;
	}
}

_matrix CBounding::Remove_Rotation(_matrix TransformMatrix)
{
	_matrix		ResultMatrix = XMMatrixIdentity();

	ResultMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	ResultMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	ResultMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));
	ResultMatrix.r[3] = TransformMatrix.r[3];

	return ResultMatrix;
}


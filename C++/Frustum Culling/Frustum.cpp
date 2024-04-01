#include "..\Public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{

}

HRESULT CFrustum::NativeConstruct()
{
	m_vPosition[0] = _float3(-1.f, 1.f, 0.f);
	m_vPosition[1] = _float3(1.f, 1.f, 0.f);
	m_vPosition[2] = _float3(1.f, -1.f, 0.f);
	m_vPosition[3] = _float3(-1.f, -1.f, 0.f);

	m_vPosition[4] = _float3(-1.f, 1.f, 1.f);
	m_vPosition[5] = _float3(1.f, 1.f, 1.f);
	m_vPosition[6] = _float3(1.f, -1.f, 1.f);
	m_vPosition[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	shared_ptr<CPipeLine>	pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix		ViewMatrix = pPipeLine->Get_Transform(CPipeLine::TS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	_matrix		ProjMatrix = pPipeLine->Get_Transform(CPipeLine::TS_PROJ);
	ProjMatrix = XMMatrixInverse(nullptr, ProjMatrix);

	for (_uint i = 0; i < 8; ++i)
	{
		XMStoreFloat3(&m_vWorldPos[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPosition[i]), ProjMatrix));
		XMStoreFloat3(&m_vWorldPos[i], XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPos[i]), ViewMatrix));
	}

	XMStoreFloat4(&m_WorldPlane[0], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[1]),
		XMLoadFloat3(&m_vWorldPos[5]),
		XMLoadFloat3(&m_vWorldPos[6])));

	XMStoreFloat4(&m_WorldPlane[1], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[4]),
		XMLoadFloat3(&m_vWorldPos[0]),
		XMLoadFloat3(&m_vWorldPos[3])));

	XMStoreFloat4(&m_WorldPlane[2], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[4]),
		XMLoadFloat3(&m_vWorldPos[5]),
		XMLoadFloat3(&m_vWorldPos[1])));

	XMStoreFloat4(&m_WorldPlane[3], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[3]),
		XMLoadFloat3(&m_vWorldPos[2]),
		XMLoadFloat3(&m_vWorldPos[6])));

	XMStoreFloat4(&m_WorldPlane[4], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[5]),
		XMLoadFloat3(&m_vWorldPos[4]),
		XMLoadFloat3(&m_vWorldPos[7])));

	XMStoreFloat4(&m_WorldPlane[5], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[0]),
		XMLoadFloat3(&m_vWorldPos[1]),
		XMLoadFloat3(&m_vWorldPos[2])));
}

_bool CFrustum::isIn_WorldSpace(_fvector vWorldPos, _float fRadius)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlane[i]), vWorldPos)))
			return false;
	}

	return true;
}

_vector CFrustum::Get_WorldPos(_uint Index)
{
	if (Index > 7)
		return _vector();

	return XMLoadFloat3(&m_vWorldPos[Index]);
}

_vector CFrustum::Get_LocalPos(_fmatrix WorldMatrixInv, _uint Index)
{
	return  XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPos[Index]), WorldMatrixInv);
}

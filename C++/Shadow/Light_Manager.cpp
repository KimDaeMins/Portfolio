#include "..\Public\Light_Manager.h"
#include "Light.h"
#include "VIBuffer_Rect.h"
#include "Target_Manager.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

const LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iIndex) const
{
	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::NativeConstruct()
{
	m_pVIBuffer = CVIBuffer_Rect::Create(TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"));
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	D3D11_VIEWPORT		ViewportDesc;

	_uint			iNumViewport = 1;

	g_DeviceContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());
	m_TransformMatrix._11 = ViewportDesc.Width;
	m_TransformMatrix._22 = ViewportDesc.Height;
	m_TransformMatrix._41 = 0.0f;
	m_TransformMatrix._42 = 0.0f;

	XMStoreFloat4x4(&m_OrthoMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHTDESC & LightDesc)
{
	shared_ptr<CLight>			pLight = CLight::Create(LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render()
{
	auto	pTarget_Manager = GET_INSTANCE(CTarget_Manager);

	m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));
	_matrix View = Get_LightMatrix(TYPE_VIEW);
	_matrix Proj = Get_LightMatrix(TYPE_PROJ);

	m_pVIBuffer->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(View), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_LightProjMatrix", &XMMatrixTranspose(Proj), sizeof(_float4x4));

	m_pVIBuffer->Set_ShaderResourceView("g_NormalTexture", pTarget_Manager->Get_SRV(TEXT("Target_Normal")));
	m_pVIBuffer->Set_ShaderResourceView("g_DepthTexture", pTarget_Manager->Get_SRV(TEXT("Target_Depth")));
	m_pVIBuffer->Set_ShaderResourceView("g_LightDepthTexture", pTarget_Manager->Get_SRV(TEXT("Target_LightDepth")));

	auto			pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix		ViewMatrixInverse = XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::TS_VIEW));
	_matrix		ProjMatrixInverse = XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::TS_PROJ));

	m_pVIBuffer->Set_RawValue("g_ViewMatrixInverse", &XMMatrixTranspose(ViewMatrixInverse), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_float4x4));

	m_pVIBuffer->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4));




	for (auto& pLight : m_Lights)
	{
		pLight->Render(m_pVIBuffer);
	}







	return S_OK;
}

_matrix CLight_Manager::Get_LightMatrix(MATRIXTYPE _eType)
{
	if (_eType == TYPE_VIEW)
	{
		return XMLoadFloat4x4(&m_LightViewMatrix);
	}
	else
	{
		return XMLoadFloat4x4(&m_LightProjMatrix);
	}
}

void CLight_Manager::Setting_LightMatrix()
{
	for (auto& Light : m_Lights)
	{
		Light->SettingLight(&m_LightViewMatrix, &m_LightProjMatrix);
	}
}
#include "..\Public\Light.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Frustum.h"
#include "Transform.h"
#include "Renderer.h"
#include "Target_Manager.h"

CLight::CLight()
{
}

HRESULT CLight::NativeConstruct(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(shared_ptr <CVIBuffer_Rect> pVIBuffer)
{

	_uint			iPassIndex = 0;

	if (LIGHTDESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		iPassIndex = 1;
		pVIBuffer->Set_RawValue("g_vLightDir", &_float4(m_LightDesc.vDirection, 0.f), sizeof(_float4));
	}
	else
	{
		iPassIndex = 2;
		pVIBuffer->Set_RawValue("g_vLightPos", &_float4(m_LightDesc.vPosition, 1.f), sizeof(_float4));
		pVIBuffer->Set_RawValue("g_fRange", &m_LightDesc.fRadiuse, sizeof(_float));

	}

	pVIBuffer->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4));
	pVIBuffer->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4));
	pVIBuffer->Set_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4));

	pVIBuffer->Render(iPassIndex);

	return S_OK;
}
void CLight::SettingLight(_float4x4 * View, _float4x4 * Proj)
{
	if (m_LightDesc.eType == LIGHTDESC::TYPE_DIRECTIONAL)
	{
		//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		//CCamera* pCamera = (CCamera*)pGameInstance->Get_GameObjectPtr(3, TEXT("Layer_Camera"));
		//CCamera::CAMERADESC Desc = pCamera->Get_CameraDesc();
		//CTransform* pTransform = pCamera->Get_Transform();
		//CFrustum* pFrustum = GET_INSTANCE(CFrustum);

		//_float FrustumDepth = Desc.fFar - Desc.fNear;
		//_vector		vEye = pTransform->Get_State(CTransform::STATE_POSITION);
		//_vector		vDir = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_LOOK));

		//_vector NearPos = vEye + vDir * Desc.fNear;
		//_vector FarPos = vEye + vDir * Desc.fFar;
		//_vector Center = (NearPos + FarPos) * 0.5f;
		//_vector LightDir = XMVector3Normalize(XMLoadFloat3(&m_LightDesc.vDirection));
		//_vector LightPos = XMVectorSetY(Center - LightDir * (FarPos - NearPos) * 1.f, 600.f);
		//LightPos = XMVectorSet(0.f, 600.f, 0.f, 1.f);
		//
		//_float4x4 WorldMatrix;
		//_float4		vTmp;
		////라업룩포
		//_vector Right, Up;
		//Right = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), LightDir);
		//Up = XMVector3Cross(LightDir, Right);
		//XMStoreFloat4(&vTmp, Right);
		//memcpy(WorldMatrix.m[0], &vTmp, sizeof(_float4));
		//XMStoreFloat4(&vTmp, Up);
		//memcpy(WorldMatrix.m[1], &vTmp, sizeof(_float4));
		//XMStoreFloat4(&vTmp, LightDir);
		//memcpy(WorldMatrix.m[2], &vTmp, sizeof(_float4));
		//XMStoreFloat4(&vTmp, LightPos);
		//memcpy(WorldMatrix.m[3], &vTmp, sizeof(_float4));

		//_matrix ViewMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMatrix));
		//_vector Near = XMVectorSet(9999.f, 9999.f, 0.f, 1.f);
		//_vector Far = XMVectorSet(-9999.f, -9999.f, -9999.f, 1.f);

		//for (_uint i = 0; i < 8; ++i)
		//{
		//	//월드상의 절두체 포지션에 빛을 구성하는 행렬의 인벌스를 곱해야됨.
		//	_vector LocalFrustumPos = pFrustum->Get_WorldPos(i);

		//	Far = XMVectorMax(Far, LocalFrustumPos);
		//	Near = XMVectorMin(Near, LocalFrustumPos);
		//}

		//_float FrustumWidth = XMVectorGetX(Far) - XMVectorGetX(Near);
		//_float FrustumHeight = XMVectorGetY(Far) - XMVectorGetY(Near);

		//_matrix ProjMatrix = XMMatrixOrthographicLH(1280.f, 1280.f, 0.2f,1100.f);

		//RELEASE_INSTANCE(CFrustum);
		//RELEASE_INSTANCE(CGameInstance);
		//XMStoreFloat4x4(View, ViewMatrix);
		//XMStoreFloat4x4(Proj, ProjMatrix);
		_matrix mView = XMMatrixLookAtLH(_vector(XMVectorSet(-250.f, 500.f, -500.f, 1.f)), _vector(XMVectorSet(0.f, 0.f, -250.f, 1.f)), _vector(XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		_matrix mProj = XMMatrixOrthographicLH(1280.f, 1280.f, 0.2f, 1100.f);

		XMStoreFloat4x4(View, mView);
		XMStoreFloat4x4(Proj, mProj);
	}
}
shared_ptr<CLight> CLight::Create(const LIGHTDESC & LightDesc)
{
	shared_ptr<CLight>	pInstance = make_shared<CLight>();

	if (FAILED(pInstance->NativeConstruct(LightDesc)))
	{
		MSG_BOX("Failed To Creating CLight");
	}
	return pInstance;
}

#include "stdafx.h"
#include "..\public\InteractionUI.h"
#include "GameInstance.h"


CInteractionUI::CInteractionUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CInteractionUI::CInteractionUI(const CInteractionUI& rhs)
	: CGameObject(rhs)
{
}

HRESULT CInteractionUI::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CInteractionUI::NativeConstruct(void* pArg)
{

	if (false == __super::Get_DontDestroy())
	{
		if (FAILED(SetUp_Component()))
			return E_FAIL;
	}
	__super::Set_DontDestroy(true);


	m_ProjMatrix =  XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f);

	return S_OK;
}

_int CInteractionUI::Tick(_float fTimeDelta)
{
	if (m_CollisionTrigger)
	{
		if (m_UIAddPosY < 2.f)
			m_UIAddPosY += 0.05f;

		//작동합니다.
		_Vector3 Pos = m_Position;
		Pos.y += 2.f;
		Pos.y += m_UIAddPosY;
		m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Pos, 1.f));

		if (m_BackUISize > 80.f)
		{
			m_BackUIAdd = -1.5f;
		}
		else if (m_BackUISize < 0.f)
		{
			m_BackUIAdd = 1.5f;
		}

		m_BackUISize += m_BackUIAdd;


	}
	else
	{

		if (m_UIAddPosY > 0.f)
			m_UIAddPosY -= 0.05f;
		else
			m_Rendering = false;

		_Vector3 Pos = m_Position;
		Pos.y += 2.f;
		Pos.y += m_UIAddPosY;
		m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Pos, 1.f));

		m_BackUISize = 0.f;
	}

	return _int();
}

_int CInteractionUI::LateTick(_float fTimeDelta)
{
	if (__super::Get_Dead() == true)
	{
		return 0;
	}


	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return 0;

	return _int();
}

HRESULT CInteractionUI::Render(_uint iRenderGroup)
{

	if (m_Rendering)
	{
		if (FAILED(SetUp_ConstantTable_UIBack()))
			return E_FAIL;

		//랜더링 m_pRectBufferComBack
		m_pRectBufferComBack->Render(m_pShaderComBack, 12);

		if (FAILED(SetUp_ConstantTable_UI()))
			return E_FAIL;

		//랜더링 m_pRectBufferCom
		m_pRectBufferComBack->Render(m_pShaderComBack, 1);
	}
	return S_OK;
}

HRESULT CInteractionUI::SetUp_Component()
{
	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pRectBufferCom)))
		return E_FAIL;

	/* For.Com_Shader*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Shader_Rect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Texture_Input_Space"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBufferBack"), (CComponent**)&m_pRectBufferComBack)))
		return E_FAIL;

	/* For.Com_Shader*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Shader_Rect"), TEXT("Com_ShaderBack"), (CComponent**)&m_pShaderComBack)))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Texture_Rounded_Glow"), TEXT("Com_TextureBack"), (CComponent**)&m_pTextureComBack)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractionUI::SetUp_ConstantTable_UIBack()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_Vector4	vTargetPos = XMVector3TransformCoord(m_pTransformCom->Get_State(STATE_POSITION), pGameInstance->Get_Transform(TS_VIEW));
	vTargetPos = XMVector3TransformCoord(vTargetPos, pGameInstance->Get_Transform(TS_PROJ));
	//뷰포트위치 어떻게 계산하더라요...?
	//-1 -> 0 1 -> g_WinCX
	vTargetPos.x = (vTargetPos.x * 0.5f * m_fWinCX) + (m_fWinCX * 0.5f);
	vTargetPos.y = -(vTargetPos.y * 0.5f * m_fWinCY) + (m_fWinCY * 0.5f);

	_Matrix IdentityMatrix;
	_Matrix		BackUIMatrix;
	BackUIMatrix._11 = m_BackUISize * m_fRatio1280;
	BackUIMatrix._22 = m_BackUISize * m_fRatio1280;
	BackUIMatrix._41 = (vTargetPos.x - (m_fWinCX * 0.5f)) * m_fRatio1280;
	BackUIMatrix._42 = (-vTargetPos.y + (m_fWinCY * 0.5f)) * m_fRatio1280;
	m_pShaderComBack->Set_RawValue("g_WorldMatrix", &BackUIMatrix, sizeof(_Matrix));
	m_pShaderComBack->Set_RawValue("g_ViewMatrix", &IdentityMatrix, sizeof(_Matrix));
	m_pShaderComBack->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_Matrix));
	m_pShaderComBack->Set_RawValue("g_vMtrlDiffuse", &_float4(0.737f, 0.898f, 0.925f, 1.f), sizeof(_float4));
	if (FAILED(m_pTextureComBack->SetUp_OnShader(m_pShaderComBack, "g_DiffuseTexture")))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CInteractionUI::TriggerCollision(_Vector3 Position, _uint ObjectType)
{
	if (m_ObjectType != ObjectType)
	{
		m_ObjectType = ObjectType;
		m_UIAddPosY = 0.f;
		m_BackUISize = 0.f;
		m_BackUIAdd = 1.5f;
		m_Position = Position;
		m_CollisionTrigger = true;
		m_Rendering = true;
	}
}

void CInteractionUI::CollisionEnd()
{
	m_CollisionTrigger = false;
	m_ObjectType = 9999;
}

HRESULT CInteractionUI::SetUp_ConstantTable_UI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_Vector4	vTargetPos = XMVector3TransformCoord(m_pTransformCom->Get_State(STATE_POSITION), pGameInstance->Get_Transform(TS_VIEW));
	vTargetPos = XMVector3TransformCoord(vTargetPos, pGameInstance->Get_Transform(TS_PROJ));
	//뷰포트위치 어떻게 계산하더라요...?
	//-1 -> 0 1 -> g_WinCX
	vTargetPos.x = (vTargetPos.x * 0.5f * m_fWinCX) + (m_fWinCX * 0.5f);
	vTargetPos.y = -(vTargetPos.y * 0.5f * m_fWinCY) + (m_fWinCY * 0.5f);

	_Matrix IdentityMatrix;
	_Matrix		UIMatrix;
	UIMatrix._11 = 30 * m_fRatio1280;
	UIMatrix._22 = 20 * m_fRatio1280;
	UIMatrix._41 = (vTargetPos.x - (m_fWinCX * 0.5f)) * m_fRatio1280;
	UIMatrix._42 = (-vTargetPos.y + (m_fWinCY * 0.5f)) * m_fRatio1280;


	m_pShaderCom->Set_RawValue( "g_WorldMatrix", &UIMatrix, sizeof(_Matrix));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &IdentityMatrix, sizeof(_Matrix));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_Matrix));
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CInteractionUI* CInteractionUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CInteractionUI* pInstance = new CInteractionUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CInteractionUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CInteractionUI::Clone(void* pArg)
{
	CInteractionUI* pInstance = new CInteractionUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CInteractionUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteractionUI::Free()
{

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRectBufferCom);
	Safe_Release(m_pTextureComBack);
	Safe_Release(m_pRectBufferComBack);
	Safe_Release(m_pShaderComBack);	
}

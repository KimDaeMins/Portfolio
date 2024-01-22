#include "stdafx.h"
#include "..\public\Stump.h"

#include "GameInstance.h"

CStump::CStump(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CStump::CStump(const CStump& rhs)
	: CGameObject(rhs)
{
}

HRESULT CStump::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CStump::NativeConstruct(void* pArg)
{

	if (pArg == nullptr)
		return E_FAIL;

	PUSHSTUMPDATA Data = *(PUSHSTUMPDATA*)pArg;

	if (FAILED(SetUp_Component()))
		return E_FAIL;
	m_Type = (_uint)OBJECT_TYPE::OBJECT;
	m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.Position, 1.f));
	if (Data.Type == CStump::GRASS)
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Grass_Bottom"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Bush_Bottom"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}


	return S_OK;
}


_int CStump::Tick(_float fTimeDelta)
{

	return _int();
}

_int CStump::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
		return 0;



	return _int();
}

HRESULT CStump::Render(_uint iRenderGroup)
{
	if (CRenderer::RENDER_NONALPHA == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom->Set_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			//m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);

			m_pModelCom->Render(m_pShaderCom, i, 0);
		}
	}

	else if (CRenderer::RENDER_DYNAMICSHADOW == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom->Render(m_pShaderCom, i, 2);
		}
	}


	return S_OK;
}

// HRESULT CStump::Render_Shadow()
// {
// 	if (FAILED(SetUp_ConstantTable()))
// 		return E_FAIL;
// 
// 	_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();
// 
// 	for (_uint i = 0; i < iNumMaterials; ++i)
// 	{
// 		m_pModelCom->Render(i, 2);
// 	}
// 
// 	return S_OK;
// }

HRESULT CStump::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader*/
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStump::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrix");

	m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightTransform(TS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightTransform(TS_PROJ), sizeof(_float4x4));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CStump* CStump::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CStump* pInstance = new CStump(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CStump");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CStump::Clone(void* pArg)
{
	CStump* pInstance = new CStump(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CStump");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStump::Free()
{

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

#include "stdafx.h"
#include "..\public\Well.h"

#include "GameInstance.h"
#include "Player_Coin.h"
#include "ImGui_Manager.h"

CWell::CWell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWell::CWell(const CWell& rhs)
	: CGameObject(rhs)
{
}

HRESULT CWell::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CWell::NativeConstruct(void* pArg)
{
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		WellData = *(PUSHWELLDATA*)pArg;

		m_pTransformCom->Set_State(STATE_POSITION, _Vector3(WellData.Position.x, WellData.Position.y, WellData.Position.z));
	}

	

	CRigid::PUSHRIGIDDATA Data;
	Data.StaticRigid = true;
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid, &Data)))
		return E_FAIL;
	m_pRigid->Set_Host(this);

	wstring ColliderName_Rigid = L"Rigid";
	m_pRigid->Create_SphereCollider(ColliderName_Rigid, 1.95f,
		Vector3(0.f, 1.f, 0.f));
	m_pRigid->Set_isQuery(ColliderName_Rigid, false);
	m_pRigid->Set_isSimulation(ColliderName_Rigid, true);
	m_pRigid->Set_isTrigger(ColliderName_Rigid, false);

	m_pRigid->Attach_Shape();
	m_pRigid->SetUp_Filtering(tagFilterGroup::PAIRCOLLISIONOBJECT);
	m_Type = (_uint)OBJECT_TYPE::WELL;

	wstring ColliderName_Trigger = L"Trigger";
	m_pRigid->Create_SphereCollider(ColliderName_Trigger, 2.f,
		Vector3(0.f, 1.f, 0.f));
	m_pRigid->Set_isQuery(ColliderName_Trigger, false);
	m_pRigid->Set_isSimulation(ColliderName_Trigger, false);
	m_pRigid->Set_isTrigger(ColliderName_Trigger, true);

	m_pRigid->Attach_Shape();
	m_pRigid->SetUp_Filtering(tagFilterGroup::PAIRCOLLISIONOBJECT);
	m_Type = (_uint)OBJECT_TYPE::WELL;


	return S_OK;
}	


_int CWell::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

	while (m_pRigid->Get_Collision_Trigger_Stay(&pGameObject))
	{
		_uint Type = pGameObject->Get_Type();

		if (Type == (_uint)OBJECT_TYPE::PLAYERCOIN)
		{
			if (((CPlayer_Coin*)pGameObject)->GetIsCounted() == false)
			{
				((CPlayer_Coin*)pGameObject)->SetIsCounted();
				m_TotalCoin = m_TotalCoin + 1;
				m_UsingCoin = m_UsingCoin + 1;

				if (FAILED(pGameInstance->Add_GameObjectToLayer(_TEXT("Layer_Well_Effect"), TEXT("Prototype_GameObject_WellSphere"), this)))
					return E_FAIL;

				if (FAILED(pGameInstance->Add_GameObjectToLayer(_TEXT("Layer_Well_Effect"), TEXT("Prototype_GameObject_WellCylinder"), this)))
					return E_FAIL;
			}
		}
	}


#ifdef _DEBUG
CImGUI_Manager::GetInstance()->Printf(_TEXT("Well Total Coin %d"), m_TotalCoin);
#endif // _DEBUG

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int CWell::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;


	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
		return 0;


	return _int();
}

HRESULT CWell::Render(_uint iRenderGroup)
{
	if (CRenderer::RENDER_NONALPHA == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		/* 장치에 월드변환 행렬을 저장한다. */
		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom->Set_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

			m_pModelCom->Render(m_pShaderCom, i, 0);
		}
		//
		//#ifdef _DEBUG
		//	m_pAABBCom->Render();
		//	m_pOBBCom->Render();
		//#endif // _DEBUG
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

HRESULT CWell::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Well"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader*/
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWell::SetUp_ConstantTable()
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


CWell* CWell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWell* pInstance = new CWell(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CWell");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CWell::Clone(void* pArg)
{
	CWell* pInstance = new CWell(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CWell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWell::Free()
{

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);

}

#include "stdafx.h"
#include "..\public\Shield.h"

#include "GameInstance.h"
#include "Player.h"


CShield::CShield(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CItem(pDevice, pDeviceContext)
{
}

CShield::CShield(const CShield& rhs)
	: CItem(rhs)
{
}

HRESULT CShield::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShield::NativeConstruct(void* pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (false == __super::Get_DontDestroy())
	{
		if (FAILED(SetUp_Component()))
			return E_FAIL;

		Create_Status((_uint)DAMEGETYPE_TYPE::NORMAL, 5.f);
	}


	m_pRigid->Clear_Flag();

	if (FAILED(SetUp_BoneMatrix()))
		return E_FAIL;

	//아이템에 저장한후 아이템정보만 슥슥바꿔준다. 쉴드는 레이트틱이 맞다고 보긴하는데 어떻게 해야할지는 모르겠다.
	//랜더에서 트랜스폼 변경 -> 틱 -> 레이트틱 -> 피직스틱 -> 랜더에서 트랜스폼변경 구조가 되고있다 지금은.
	//StateShield 일때 m_Type = SHIELD;
	//StateParry 중 특정프레임 이상일때 m_Type = SHIELDPARRY;
	//둘중 하나의 상태가 끝났다면 m_Type = OBJ_END; 
	//유지해야됨. UsingType이 필요없는데 걍 아이템말고 오브젝트로 둘걸 그랬음
	m_Type = (_uint)OBJECT_TYPE::OBJ_END;

	m_UsingType = USING_TYPE::TYPE_END;
	return S_OK;
}

_int CShield::Item_Tick(_float fTimeDelta)
{
	if (__super::Item_Tick(fTimeDelta) < 0)
		return _int();

	return _int();
}

_int CShield::Item_LateTick(_float fTimeDelta)
{

	if (__super::Item_LateTick(fTimeDelta) < 0)
		return _int();

	if (nullptr == m_pRendererCom)
		return -1;



	//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
	//	return 0;


	return _int();
}

HRESULT CShield::Item_Render(_uint iRenderGroup)
{
	_Matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_pBoneMatrix);
	_Matrix		PivotMatrix = XMLoadFloat4x4(&m_PivotMatrix);
	_Matrix		TargetWorldMatrix = XMLoadFloat4x4(m_pTargetWorldMatrix);

	_Matrix World = (CombinedTransformationMatrix * PivotMatrix) * TargetWorldMatrix;
	m_pTransformCom->Set_WorldMatrix(World);
	m_pTransformCom->Set_Scale(_Vector3(1.f, 1.f, 1.f));

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

HRESULT CShield::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Shield"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CRigid::PUSHRIGIDDATA Data;
	Data.StaticRigid = true;
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid, &Data)))
		return E_FAIL;

	wstring ColliderName = L"Rigid";
	m_pRigid->Set_Host(this);
	m_pRigid->Create_BoxCollider(ColliderName, _Vector3(1.f, 0.4f , 2.f), _Vector3(0.f, -0.2f, 0.f));
	m_pRigid->Set_isQuery(ColliderName, false);
	m_pRigid->Set_isSimulation(ColliderName, true);
	m_pRigid->Set_isTrigger(ColliderName, false);
/*	m_pRigid->Set_Gravity(false);*/
	m_pRigid->Attach_Shape();
	m_pRigid->SetUp_Filtering(tagFilterGroup::PLAYERSHIELD);

	return S_OK;
}

HRESULT CShield::SetUp_BoneMatrix()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CModel* pPlayerModel = (CModel*)pGameInstance->Get_ComponentPtr(TEXT("Layer_Player"), TEXT("Com_Model"));
	if (nullptr == pPlayerModel)
		return E_FAIL;

	m_OffsetMatrix = pPlayerModel->Get_OffsetMatrix("hand.L");
	m_pBoneMatrix = pPlayerModel->Get_CombinedMatrixPtr("hand.L");
	m_PivotMatrix = pPlayerModel->Get_PivotMatrix();

	CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr(TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return E_FAIL;

	m_pTargetWorldMatrix = pPlayerTransform->Get_WorldFloat4x4Ptr();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CShield::SetUp_ConstantTable()
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

CShield* CShield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CShield* pInstance = new CShield(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CShield::Clone(void* pArg)
{
	CShield* pInstance = new CShield(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShield::Free()
{
	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRigid);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);		
}

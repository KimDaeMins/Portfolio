#include "stdafx.h"
#include "..\public\Coin.h"

#include "GameInstance.h"
#include "Player.h"

// TODO : 우리 재화 클래스

CCoin::CCoin(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CCoin::CCoin(const CCoin& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCoin::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CCoin::NativeConstruct(void* pArg)
{
	if (!pArg)
		return E_FAIL;
	PUSHCOINDATA Data = *(PUSHCOINDATA*)pArg;
	m_Type = (_uint)OBJECT_TYPE::OBJECT;
	m_eCoinType = Data.eCoinType;
	if (FAILED(SetUp_Component()))
		return E_FAIL;


	m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.Position, 1.f));
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid)))
		return E_FAIL;

	m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_Z, true);
	m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_X, true);
	m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_Y, true);
	wstring ColliderName = L"Rigid";
	m_pRigid->Set_Host(this);
	m_pRigid->Create_SphereCollider(ColliderName, 0.3f, _Vector3(0.f, 0.3f, 0.f));
	m_pRigid->Attach_Shape();
	m_pRigid->Set_Gravity(true);
	m_pRigid->SetUp_Filtering(tagFilterGroup::OBJECT);
	m_pRigid->Set_Filter(ColliderName, 1, tagFilterGroup::PLANE);
	m_pRigid->Set_Mess(0.1f);
	m_pRigid->Set_ActorLocalPos(_Vector3(0.f, 0.3f, 0.f));
	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pRigid->Set_Material(ColliderName, 1.f, 1.f, 1.f);
	m_pRigid->Set_isQuery(ColliderName, false);
	//함써보고싶었는데 되려나?
	m_pRigid->Add_Force(Data.Force, CRigid::FORCEMODE::IMPULSE);
	return S_OK;
}

_int CCoin::Tick(_float fTimeDelta)
{
	m_pTransformCom->Turn(_Vector3(0.f, 1.f, 0.f), fTimeDelta);

	CGameObject* Object = nullptr;	

// 	_Vector3 CurPos = m_pTransformCom->Get_State(STATE_POSITION);
// 
// 	// 최고치 찍었을때부터 무게 증가시켜서 빠르게 떨어지게 할 예정
// 	if (CurPos.y - m_PrePos.y < 0.f)
// 	{
// 		_Vector3 Force = _Vector3(0.0f, -1.5f, 0.f);
// 
// 		if (m_Mass < 300.f)
// 			m_Mass = 300.f;
// 
// 		m_pRigid->Set_Mess(m_Mass);
// 
// 		if (!m_bIsDownStart)
// 		{
// 			m_pRigid->Add_Force(Force, CRigid::FORCEMODE::IMPULSE);
// 			m_bIsDownStart = true;
// 		}
// 	}
// 
// 	// 이전 위치 갱신		
// 	m_PrePos = CurPos;

	if (m_Chase)
	{
		_Vector3 TargetPos = m_TragerTransform->Get_State(STATE_POSITION);

		m_pTransformCom->Move_Pos(TargetPos, fTimeDelta);

		CPhysicsSystem* PhysX = GET_INSTANCE(CPhysicsSystem);
		if (PhysX->OverlapTest(m_pTransformCom->Get_State(STATE_POSITION), 0.6f, tagFilterGroup::PLAYER, QUERYTYPE::SIMULATION))
		{
			CInventory* pInventory = GET_INSTANCE(CInventory);
			switch (m_eCoinType)
			{
			case COINTYPE::SMALL:
				pInventory->Add_Gold(3);//4개
				break;
			case COINTYPE::MEDIUM:
				pInventory->Add_Gold(6);//2개
				break;
			case COINTYPE::BIG:
				pInventory->Add_Gold(26);//1개		
				break;
			}
			m_bDead = true;
			RELEASE_INSTANCE(CInventory);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Play(L"gen_prop_obj_gem_pu_yellow_", MULTY, 21, 5, m_pTransformCom);
			RELEASE_INSTANCE(CGameInstance);
		}
		RELEASE_INSTANCE(CPhysicsSystem);
		
	}
	else if (m_Down >= 3)
	{
		while (m_pRigid->Get_Collision_Trigger_Enter(&Object))
		{
			if (Object->Get_Type() == (_uint)OBJECT_TYPE::PLAYER)
			{
				m_pRigid->Set_Gravity(false);
				m_Chase = true;
				m_TragerTransform = Object->Get_Transform();
				m_pRigid->Detach_Shape();
			}
		}
	}
	else
	{
		while (m_pRigid->Get_Collision_Simulation_Stay(&Object))
		{
			if (Object->Get_Type() == (_uint)OBJECT_TYPE::FLOOR)
			{
				++m_Down;
				if (m_Down >= 3)
				{
					wstring Name = L"Trigger";
					m_pRigid->Create_SphereCollider(Name, 10.f, _Vector3(0.f, 0.f, 0.f));
					m_pRigid->Set_isSimulation(Name, false);
					m_pRigid->Set_isTrigger(Name, true);
					m_pRigid->SetUp_Filtering(Name, tagFilterGroup::OBJECT);
					m_pRigid->Set_Filter(Name, 1, tagFilterGroup::PLAYER);
					m_pRigid->Attach_Shape();
				}
			}
		}

		while (m_pRigid->Get_Collision_Simulation_Enter(&Object))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Play(L"gen_prop_obj_gem_land_", MULTY, 21, 5, m_pTransformCom);
			RELEASE_INSTANCE(CGameInstance);
		}
	}

	if (XMVectorGetY(m_pTransformCom->Get_State(STATE_POSITION)) < -200.f)
		m_bDead = true;	

	return _int();
}

_int CCoin::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;


	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
		return 0; 


	return _int();
}

HRESULT CCoin::Render(_uint iRenderGroup)
{
	if (CRenderer::RENDER_NONALPHA == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		/* 장치에 월드변환 행렬을 저장한다. */
		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{			
			m_pModelCom->Set_MtrlColor(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_vMtrlDiffuse");
			m_pModelCom->Set_MtrlColor(m_pShaderCom, i, aiTextureType_EMISSIVE, "g_vMtrlEmissive");

			m_pModelCom->Render(m_pShaderCom, i, 3);
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

//HRESULT CCoin::Render_Shadow()
//{
//	if (FAILED(SetUp_ConstantTable()))
//		return E_FAIL;
//
//	_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();
//
//	for (_uint i = 0; i < iNumMaterials; ++i)
//	{
//		m_pModelCom->Render(i, 2);
//	}
//
//	return S_OK;
//}

HRESULT CCoin::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC Desc;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec =XMConvertToRadians(180.f);
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &Desc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (COINTYPE::SMALL == m_eCoinType)
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Coin_PickUp_Small"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (COINTYPE::MEDIUM == m_eCoinType)
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Coin_PickUp_Medium"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (COINTYPE::BIG == m_eCoinType)
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Coin_PickUp_Big"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCoin::SetUp_ConstantTable()
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


CCoin* CCoin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCoin* pInstance = new CCoin(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CCoin");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CCoin::Clone(void* pArg)
{
	CCoin* pInstance = new CCoin(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CCoin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCoin::Free()
{

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);
}

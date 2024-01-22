#include "stdafx.h"
#include "DropItem.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "Player.h"
#include "Effect_SoulBright.h"
CDropItem::CDropItem(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    :CGameObject(pDevice, pDeviceContext)
{
}

CDropItem::CDropItem(const CDropItem& rhs)
    :CGameObject(rhs)
{
}

HRESULT CDropItem::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CDropItem::NativeConstruct(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	PUSHDROPITEMDATA Data = *(PUSHDROPITEMDATA*)pArg;
	m_DropItemType = Data.eType;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.Position, 1.f));
	m_Type = (_uint)OBJECT_TYPE::OBJECT;

	m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_Z, true);
	m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_X, true);
	m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_Y, true);

	wstring ColliderName = L"Rigid";
	m_pRigid->Set_Host(this);
	m_pRigid->Set_Gravity(true);
	m_pRigid->Set_Mess(0.1f);
	m_pRigid->Create_SphereCollider(ColliderName, 0.4f, _Vector3(0.f, 0.4f, 0.f));
	m_pRigid->Attach_Shape();
	m_pRigid->Set_ActorLocalPos(_Vector3(0.f, 0.4f, 0.f));
	m_pRigid->SetUp_Filtering(tagFilterGroup::OBJECT);
	m_pRigid->Set_Filter(ColliderName, 1, tagFilterGroup::PLANE);
	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pRigid->Set_Material(ColliderName, 1.f, 1.f, 0.8f);
	m_pRigid->Set_isQuery(ColliderName, false);
	m_pRigid->Add_Force(Data.Force, CRigid::FORCEMODE::IMPULSE);

	return S_OK;
}

_int CDropItem::Tick(_float fTimeDelta)
{
	m_pTransformCom->Turn(_Vector3(0.f, 1.f, 0.f), fTimeDelta);

	CGameObject* Object = nullptr;
	if (m_Chase)
	{
		_Vector3 TargetPos = m_TragetTransform->Get_State(STATE_POSITION);
		m_pTransformCom->Move_Pos(TargetPos, fTimeDelta);

		CPhysicsSystem* PhysX = GET_INSTANCE(CPhysicsSystem);
		CRigid* Rigid;
		if (Rigid = PhysX->Overlap(m_pTransformCom->Get_State(STATE_POSITION), 0.4f, tagFilterGroup::PLAYER, QUERYTYPE::SIMULATION))
		{
			CInventory* pInventory = GET_INSTANCE(CInventory);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			switch (m_DropItemType)
			{
			case Client::CDropItem::HP:
				pGameInstance->Play(L"gen_prop_obj_heart_pu_", MULTY, 21, 4, m_pTransformCom);
				static_cast<CPlayer*>(Rigid->Get_Host())->Add_PlayerHp(15.f);
				break;
			case Client::CDropItem::MP:
				pGameInstance->Play(L"gen_prop_obj_mana_pickup_", MULTY, 21, 8, m_pTransformCom);
				static_cast<CPlayer*>(Rigid->Get_Host())->Add_PlayerMp(5.f);
				break;
			}
			m_bDead = true;
			RELEASE_INSTANCE(CInventory);
			RELEASE_INSTANCE(CGameInstance);
		}
		RELEASE_INSTANCE(CPhysicsSystem);

	}
	else
	{
		m_CreateEffectDelay += fTimeDelta;

		if (m_CreateEffectDelay > 1.5f)
		{
			m_CreateEffectDelay = 0.f;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CSoulBright::PUSHSOULBRIGHTDATA Data;
			Data.HostObject = this;
			Data.vColor = _float4(0.3f, 0.4f, 1.f, 1.f);
			Data.StartScale = 0.1f;
			Data.EndScale = 0.6f;
			Data.bBillBoard = true;
			pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_SoulBright", &Data);
			RELEASE_INSTANCE(CGameInstance);
		}
		if (m_ChaseWaitTime > 1.5f)
		{
			while (m_pRigid->Get_Collision_Trigger_Enter(&Object))
			{
				if (Object->Get_Type() == (_uint)OBJECT_TYPE::PLAYER)
				{
					m_pRigid->Set_Gravity(false);
					m_Chase = true;
					m_TragetTransform = Object->Get_Transform();
					m_pRigid->Detach_Shape();
				}
			}
		}
		else
		{
			if (m_DropItemType == CDropItem::MP)
			{
				while (m_pRigid->Get_Collision_Simulation_Enter(&Object))
				{
					if (Object->Get_Type() == (_uint)OBJECT_TYPE::FLOOR)
					{
						CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
						pGameInstance->Play(L"gen_prop_obj_mana_land_", MULTY, 21, 8, m_pTransformCom);
						RELEASE_INSTANCE(CGameInstance);
					}
					else
					{
						CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
						pGameInstance->Play(L"gen_prop_obj_heart_land_", MULTY, 21, 4, m_pTransformCom);
						RELEASE_INSTANCE(CGameInstance);
					}
				}
			}
			m_ChaseWaitTime += fTimeDelta;
			if (m_ChaseWaitTime > 1.5f)
			{
				wstring Name = L"Trigger";
				m_pRigid->Create_SphereCollider(Name, 4.f, _Vector3(0.f, 0.f, 0.f));
				m_pRigid->Set_isSimulation(Name, false);
				m_pRigid->Set_isTrigger(Name, true);
				m_pRigid->SetUp_Filtering(Name, tagFilterGroup::OBJECT);
				m_pRigid->Set_Filter(Name, 1, tagFilterGroup::PLAYER);
				m_pRigid->Attach_Shape();
			}
		}
	}
	
	if (XMVectorGetY(m_pTransformCom->Get_State(STATE_POSITION)) < -200.f)
		m_bDead = true;

    return _int();
}

_int CDropItem::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;


		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return 0;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
			return 0;

	if (m_DropItemType == MP)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
			return 0;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_GLOW, this)))
			return 0;
		SoulTrailUpdate(fTimeDelta);
	}

    return _int();
}

HRESULT CDropItem::Render(_uint iRenderGroup)
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

	else if (CRenderer::RENDER_NONLIGHT == iRenderGroup)
	{
		// trail render test : nonlight
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_pShaderComTrail->Set_RawValue("g_WorldMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
		pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderComTrail, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderComTrail, "g_ProjMatrix");
		RELEASE_INSTANCE(CGameInstance);

		m_pShaderComTrail->Set_RawValue("g_vMtrlDiffuse", &_float4(0.72f, 0.93f, 1.f, 1.f), sizeof(_float4));
		m_pTrailCom[0]->Render(m_pShaderComTrail, 6);
		m_pTrailCom[1]->Render(m_pShaderComTrail, 6);
		m_pTrailCom[2]->Render(m_pShaderComTrail, 6);
	}

	else if (CRenderer::RENDER_GLOW == iRenderGroup)
	{
		// trail render test : glow
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_pShaderComTrail->Set_RawValue("g_WorldMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
		pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderComTrail, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderComTrail, "g_ProjMatrix");
		RELEASE_INSTANCE(CGameInstance);

		m_pShaderComTrail->Set_RawValue("g_vGlowColor1", &_float4(0.23f, 0.44f, 0.46f, 1.f), sizeof(_float4));
		m_pShaderComTrail->Set_RawValue("g_vGlowColor2", &_float4(0.f, 0.f, 0.f, 0.f), sizeof(_float4));
		m_pTrailCom[0]->Render(m_pShaderComTrail, 8);
		m_pTrailCom[1]->Render(m_pShaderComTrail, 8);
		m_pTrailCom[2]->Render(m_pShaderComTrail, 8);
	}
	return S_OK;
}

void CDropItem::SoulTrailUpdate(_float fTimeDelta)
{
	_Vector4 vPos = m_pTransformCom->Get_State(STATE_POSITION);
	_Vector3 vRight = m_pTransformCom->Get_State(STATE_RIGHT);
	_Vector3 vUp = m_pTransformCom->Get_State(STATE_UP);
	vPos.y += 0.1f;
	_float3 vUpPos, vDownPos;

	XMStoreFloat3(&vUpPos, vPos + (vRight * 0.1f + vUp * 0.15f));
	XMStoreFloat3(&vDownPos, vPos - (vRight * 0.1f + vUp * 0.05f));

	m_pTrailCom[0]->AddNewTrail(vUpPos, vDownPos, fTimeDelta * 60.f);

	XMStoreFloat3(&vUpPos, vPos - (vRight * 0.1f + vUp * 0.15f));
	XMStoreFloat3(&vDownPos, vPos + (vRight * 0.1f + vUp * 0.05f));

	m_pTrailCom[1]->AddNewTrail(vUpPos, vDownPos, fTimeDelta * 60.f);

	XMStoreFloat3(&vUpPos, vPos + vUp * 0.2f);
	XMStoreFloat3(&vDownPos, vPos);

	m_pTrailCom[1]->AddNewTrail(vUpPos, vDownPos, fTimeDelta * 60.f);

	m_pTrailCom[0]->Update(fTimeDelta * 60.f, &/*m_pTransformCom->Get_WorldMatrix()*/XMMatrixIdentity());
	m_pTrailCom[1]->Update(fTimeDelta * 60.f, &/*m_pTransformCom->Get_WorldMatrix()*/XMMatrixIdentity());
	m_pTrailCom[2]->Update(fTimeDelta * 60.f, &/*m_pTransformCom->Get_WorldMatrix()*/XMMatrixIdentity());
}

HRESULT CDropItem::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC Desc;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.f);
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &Desc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid)))
		return E_FAIL;

	/* For.Com_Model */
	if (DROPITEMTYPE::HP == m_DropItemType)
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Heart"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (DROPITEMTYPE::MP == m_DropItemType)
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Soul"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer_Trail1"), (CComponent**)&m_pTrailCom[0])))
			return E_FAIL;
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer_Trail2"), (CComponent**)&m_pTrailCom[1])))
			return E_FAIL;
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer_Trail3"), (CComponent**)&m_pTrailCom[2])))
			return E_FAIL;
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Rect"), TEXT("Com_Shader_Trail"), (CComponent**)&m_pShaderComTrail)))
			return E_FAIL;
		m_pTrailCom[0]->Set_TrailSubtraction(true);
		m_pTrailCom[0]->Set_AliveTime(20.f);
		m_pTrailCom[0]->Set_Duration(10.f);
		m_pTrailCom[1]->Set_TrailSubtraction(true);
		m_pTrailCom[1]->Set_AliveTime(20.f);
		m_pTrailCom[0]->Set_Duration(10.f);
		m_pTrailCom[2]->Set_TrailSubtraction(true);
		m_pTrailCom[2]->Set_AliveTime(20.f);
		m_pTrailCom[0]->Set_Duration(10.f);
	}
	else
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CDropItem::SetUp_ConstantTable()
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

CDropItem* CDropItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CDropItem* pInstance = new CDropItem(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CDropItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CDropItem::Clone(void* pArg)
{
	CDropItem* pInstance = new CDropItem(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CDropItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDropItem::Free()
{

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);
	Safe_Release(m_pTrailCom[0]);
	Safe_Release(m_pTrailCom[1]);
	Safe_Release(m_pTrailCom[2]);	
	Safe_Release(m_pShaderComTrail);
}


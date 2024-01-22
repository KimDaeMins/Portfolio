#include "stdafx.h"
#include "..\public\ThrowItem.h"

#include "GameInstance.h"
#include "BombDust.h"

CThrowItem::CThrowItem(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CThrowItem::CThrowItem(const CThrowItem& rhs)
	: CGameObject(rhs)
{
}

HRESULT CThrowItem::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CThrowItem::NativeConstruct(void* pArg)
{
	if (!pArg)
		return E_FAIL;
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	Create_Status(0, 30.f, 20.f, 20.f);
	CTransform::TRANSFORMDESC Desc;
	Desc.fSpeedPerSec = 3.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.f);
	m_pTransformCom->Set_TransformDesc(Desc);

	PUSHTHROWITEMDATA Data = *(PUSHTHROWITEMDATA*)pArg;
	m_Type = (_uint)OBJECT_TYPE::OBJECT;//나중에수정
	m_ItemType = Data.ItemType;
	m_LockOn = Data.LockOn;
	m_NearObject = Data.NearObject;
	m_StartPos = Data.Position + Data.Look * 1.05f;//1.05f는 내크기 + 플레이어라디우스
	m_StartPos.y += 2.5f; //2.5f는 플레이어기준 머리쪽? 예상
	BeforePos = m_StartPos;
	m_pTransformCom->Set_State(STATE_POSITION, _Vector4(m_StartPos, 1.f));
	if (m_LockOn)
		m_EndPos = Data.NearObject->Get_Transform()->Get_State(STATE_POSITION);
	else
		m_EndPos = Data.Position + Data.Look * m_ThrowLength;


	//switch (m_ItemType)
	//{
	//case ITEMTYPE::TYPE_ICE:
	//	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_IceExplosion"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//		return E_FAIL;
	//	m_OnTopTime = 0.4f;
	//	break;
	//case ITEMTYPE::TYPE_FIRE:
	//	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FireExplosion"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//		return E_FAIL;
	//	m_OnTopTime = 0.4f;
	//	break;
	//case ITEMTYPE::TYPE_FIRECRACKER:
	//	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FireCrackerExplosion"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//		return E_FAIL;
	//	m_OnTopTime = 0.25f;
	//	break;
	//}

	if(m_ItemType == ITEMTYPE::TYPE_FIRECRACKER)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Play(L"pl_itm_wep_firecracker_fizzle_", MULTY, 10, 5, m_pTransformCom);
		pGameInstance->Play(L"pl_itm_wep_firecracker_toss_", MULTY, 11, 3, m_pTransformCom);
		RELEASE_INSTANCE(CGameInstance);
	}
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid)))
		return E_FAIL;

	wstring ColliderName = L"Rigid";
	m_pRigid->Set_Host(this);
	m_pRigid->Create_SphereCollider(ColliderName, 0.3f, _Vector3(0.f, 0.3f, 0.f));
	m_pRigid->Attach_Shape();
	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pRigid->Set_Gravity(false);
	m_pRigid->SetUp_Filtering(tagFilterGroup::NONEHITPLAYEROBJECT);
	m_pRigid->Set_ActorLocalPos(_Vector3(0.f, 0.3f, 0.f));
	m_pRigid->Set_Mess(0.0000001f);
	m_pRigid->Set_isQuery(ColliderName, false);

	m_pRigid->Set_Material(ColliderName, 0.8f, 0.8f, 0.5f);//추후조정
	m_pRigid->Add_Torque(_Vector3(5.f, 2.f, 0.f), CRigid::IMPULSE);
	m_OnTopTime = 0.15f;
	return S_OK;
}


_int CThrowItem::Tick(_float fTimeDelta)
{
	CGameObject* pGameObject = nullptr;


	//타이머감소
	m_Time += fTimeDelta;

	if (m_Bomb)//다음프레임에 데드를 트루로 바꾼다
	{

		if (m_DeathTime == 0.f)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CBombDust::PUSHBOMBDUSTDATA Data;

			
				pGameInstance->Play(L"pl_itm_wep_firecracker_xpl_", MULTY, 10, 3, m_pTransformCom);
				Data.Diffuse = { 1.f, 1.f, 1.f, 1.f };
				Data.MainPos = m_pTransformCom->Get_State(STATE_POSITION);
				Data.NumBombDust = 36;
				if (FAILED(pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_BombDustEffect", &Data)))
					MSG_BOX("?");

			RELEASE_INSTANCE(CGameInstance);
		}

		m_DeathTime += fTimeDelta;

		if (m_DeathTime > 0.1f)
		{
			m_bDead = true;
			m_pRigid->Detach_Shape();
			m_pRigid->Detach_Rigid();
		}
		//여기서 트리거랑 닿은애들이 있다면 처리한다.

		while (m_pRigid->Get_Collision_Trigger_Enter(&pGameObject))
		{
				if (pGameObject->Get_Type() == (_uint)OBJECT_TYPE::OBJECT || pGameObject->Get_Type() == (_uint)OBJECT_TYPE::MONSTER || pGameObject->Get_Type() == (_uint)OBJECT_TYPE::PLAYER)
				{
					_Vector3 TargetPos = pGameObject->Get_Transform()->Get_State(STATE_POSITION);
					_Vector3 MyPos = m_pTransformCom->Get_State(STATE_POSITION);

					_Vector3 Force = TargetPos - MyPos;
					Force.y = 0.f;
					Force.Normalize();
					Force.y = 0.2f;

					CRigid* Rigid = (CRigid*)pGameObject->Get_ComponentPtr(TEXT("Com_RigidBody"));
					Rigid->Add_Force(Force, CRigid::IMPULSE);
					//피해주는함수
				}
		
		}
	}

	if (!m_Bound && !m_Bomb)
	{
		while (m_pRigid->Get_Collision_Simulation_Enter(&pGameObject))
		{
			if (pGameObject->Get_Type() == (_uint)OBJECT_TYPE::FLOOR || pGameObject->Get_Type() == (_uint)OBJECT_TYPE::OBJECT || pGameObject->Get_Type() == (_uint)OBJECT_TYPE::MONSTER)
			{
				m_Bound = true;
			}
		}
	}
	if (m_Bound)
	{
		if (m_NearObject)//느리다싶으면 NativeConstruct에서 속도를 조정하세요
		{
			if (m_NearObject->Get_Dead())
				m_NearObject = nullptr;
			else
			m_pTransformCom->Move_Pos(m_NearObject->Get_Transform()->Get_State(STATE_POSITION), fTimeDelta);
		}
		m_pRigid->Set_Gravity(true);
	}
	else
	{
			//포지션 주는거면 이전포지션 저장하는 방식으로 가서 하면되고.
			_Vector3 Position = ParabolaBetweenTwoDots(m_Time, m_EndPos, m_StartPos, m_StartPos.y + 1.f, m_OnTopTime);
			_Vector3 Velocity = Position - BeforePos;
			BeforePos = Position;
			m_pRigid->Set_LinearVelocity(Velocity * 60.f);
	}

	//내 물체가 공격트리거에 닿았거나, 시간이 지났다면? 붐
	if (!m_Bomb)
	{
		while (m_pRigid->Get_Collision_Trigger_Enter(&pGameObject))
		{
			//터질때 오브젝트타입을 Attack으로 바꿀거임 기존 시뮬레이션쉐이프는 제거하고 새로 만들어서 적용 Bomb는 트루
			if (pGameObject->Get_Type() == (_uint)OBJECT_TYPE::ATTACK || pGameObject->Get_Type() == (_uint)OBJECT_TYPE::MONSTER)
			{
				Set_Type((_uint)OBJECT_TYPE::ATTACK);
				m_pRigid->Detach_Shape();

				wstring Bomb = L"Trigger";
				m_pRigid->Create_SphereCollider(Bomb, 10.f);
				m_pRigid->Set_isSimulation(Bomb, false);
				m_pRigid->Set_isTrigger(Bomb, true);
				m_pRigid->Set_isQuery(Bomb, false);
				m_pRigid->Attach_Shape();
				m_pRigid->SetUp_Filtering((_uint)tagFilterGroup::MONSTER);

				m_Bomb = true;
			}
		}
		if (m_Time >= 2.5f )//더 늦게 터치고싶으면 이거바꾸세요
		{
			Set_Type((_uint)OBJECT_TYPE::ATTACK);
			m_pRigid->Detach_Shape();

			wstring Bomb = L"Trigger";
			m_pRigid->Create_SphereCollider(Bomb, 10.f);
			m_pRigid->Set_isSimulation(Bomb, false);
			m_pRigid->Set_isTrigger(Bomb, true);
			m_pRigid->Set_isQuery(Bomb, false);
			m_pRigid->Attach_Shape();
			m_pRigid->SetUp_Filtering((_uint)tagFilterGroup::MONSTER);
			m_Bomb = true;
		}
	}
	return _int();
}

_int CThrowItem::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pModelCom)
		return -1;

		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return 0;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
			return 0;



	return _int();
}

HRESULT CThrowItem::Render(_uint iRenderGroup)
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

// HRESULT CThrowItem::Render_Shadow()
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

HRESULT CThrowItem::SetUp_Component()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Model_Firecracker"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

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

HRESULT CThrowItem::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
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


CThrowItem* CThrowItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CThrowItem* pInstance = new CThrowItem(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CThrowItem::Clone(void* pArg)
{
	CThrowItem* pInstance = new CThrowItem(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CGrass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThrowItem::Free()
{
	__super::Free();
	//Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);

}

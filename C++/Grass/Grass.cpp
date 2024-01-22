#include "stdafx.h"
#include "..\public\Grass.h"

#include "GameInstance.h"
#include "Treetop.h"
#include "Stump.h"

CGrass::CGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CGrass::CGrass(const CGrass& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGrass::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CGrass::NativeConstruct(void* pArg)
{
	if (FAILED(SetUp_Component()))
		return E_FAIL;
	m_Type = (_uint)OBJECT_TYPE::OBJECT;

	if (pArg)
	{
		PUSHGRASSDATA Data = *(PUSHGRASSDATA*)pArg;
		m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.Position, 1.f));
		//m_pTransformCom->Set_State(STATE_POSITION, XMVectorSet(3.933f, 4.07f, 10.55f, 1.f));
	}
	else
	{
		m_pTransformCom->Set_State(STATE_POSITION, XMVectorSet(5.f, _float(rand() % 10) / 10.f , -5.f, 1.f));
	}

	m_RenderTransform->Set_State(STATE_POSITION, m_pTransformCom->Get_State(STATE_POSITION));

	CRigid::PUSHRIGIDDATA Data;
	//Data.LockFlag[CRigid::PO][CRigid::L_Y] = true;
	Data.StaticRigid = false;
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid, &Data)))
		return E_FAIL;

	m_pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_X, false);
	m_pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Y, true);
	m_pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Z, false);
	m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_Z, true);
	m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_X, true);
	m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_Y, true);
	wstring ColliderName = L"Rigid";
	m_pRigid->Set_Host(this);
	m_pRigid->Create_CapsuleCollider(ColliderName, 0.3f, 1.f, _Vector3(0.f, 1.5f, 0.f));
	m_pRigid->Attach_Shape();
	m_pRigid->Set_Gravity(false);
	m_pRigid->SetUp_Filtering(tagFilterGroup::OBJECT);
	m_pRigid->Set_Mess(0.00001f);
	m_pRigid->Set_ActorLocalPos(_Vector3(0.f, 0.5f ,0.f));
	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pRigid->Set_isQuery(ColliderName, false);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if(!pGameInstance->GetCheckEmptyObjectList(TEXT("Layer_Player")))
	{
		m_pPlayerTeansform = (CTransform*)pGameInstance->Get_ComponentPtr(L"Layer_Player", L"Com_Transform");
		m_PlayerRadius = ((CRigid*)pGameInstance->Get_ComponentPtr(L"Layer_Player", L"Com_RigidBody"))->Get_Radius(ColliderName);
	}
	m_MyRadius = m_pRigid->Get_Radius(ColliderName);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}
 
_int CGrass::Tick(_float fTimeDelta)
{
	CGameObject* pGameObject = nullptr;
	while (m_pRigid->Get_Collision_Trigger_Enter(&pGameObject))
	{
		if (pGameObject->Get_Type() == (_uint)OBJECT_TYPE::ATTACK)
		{
			if (pGameObject->Get_DamegeType() == (_uint)DAMEGETYPE_TYPE::NORMAL)
			{
				m_bDead = true;
				m_pRigid->Detach_Shape();
			}
		}
	}
	if (m_bDead)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Play(L"gen_prop_brk_grass_", MULTY, 3, 26);
		CTreetop::PUSHTREETOPPDATA TreeTopData;
		TreeTopData.Position = m_pTransformCom->Get_State(STATE_POSITION);
		TreeTopData.LifeTime = 2.f;
		TreeTopData.Force = _Vector3(_float(rand() % 4) / 10.f, 1.3f, _float(rand() % 4) / 10.f);//일단대충랜덤
		TreeTopData.Torque = _Vector3(2.3f, 2.3f, 2.3f);
		TreeTopData.Type = CTreetop::GRASS;

		pGameInstance->Add_GameObjectToLayer(TEXT("Layer_Object"), TEXT("Prototype_GameObject_Treetop"), &TreeTopData);


		CStump::PUSHSTUMPDATA StumpData;
		StumpData.Position = m_pTransformCom->Get_State(STATE_POSITION);
		StumpData.Type = CStump::GRASS;

		pGameInstance->Add_GameObjectToLayer(TEXT("Layer_Object"), TEXT("Prototype_GameObject_Stump"), &StumpData);
		RELEASE_INSTANCE(CGameInstance);
	}

	if (m_pRigid->Get_Sleep())
	{
		return _uint();
	}

	//실제 작동되는 내위치
	_Vector3 ActorPos = m_pRigid->Get_ActorPos();
	//랜더링되는 내 위치
	_Vector3 MyPos = m_RenderTransform->Get_State(STATE_POSITION);

	//이동량제거
	m_pRigid->Set_LinearVelocity(_Vector3(0.f, 0.f, 0.f));
	//플레이어 위치
	_Vector3 PlayerPos = m_pPlayerTeansform->Get_State(STATE_POSITION);
	//실제 작동되는 내위치 보다 조금 위
	_Vector3 ActorLocalPos = m_pRigid->Get_ActorLocalPos();
	//랜더링되는 내 위치
	_float	 RealPosY = MyPos.y;

	//랜더링트랜스폼 회전
	_Vector3 Up = Vec3Normalize(ActorPos - MyPos + ActorLocalPos);
	_Vector3 Right = { 1.f , 0.f , 0.f };
	_Vector3 Look;
	Right.Cross(Up, Look);
	Right = Up.Cross(Look);
	m_RenderTransform->Set_Rotate(Vec3Normalize(Right), Vec3Normalize(Up), Vec3Normalize(Look));

	//계산에서의 y값혼동을 막기위해 0으로 설정
	ActorPos.y = 0.f;
	MyPos.y = 0.f;

	if ((MyPos - ActorPos).Length() > m_PlayerRadius * 2.f + m_MyRadius)
	{
		_Vector3 Dir = PlayerPos;
		Dir.y = 0.f;
		ActorPos = -(Vec3Normalize(Dir - ActorPos) * (m_PlayerRadius + m_MyRadius)) + PlayerPos;
	}
	//딸려오는 오류를 막기위해 2중으로 계산함.
	if ((MyPos - ActorPos).Length() > m_PlayerRadius * 2.f + m_MyRadius)
	{
		ActorPos = MyPos;
		ActorPos.y = RealPosY;
		m_pTransformCom->Set_State(STATE_POSITION, _Vector4(ActorPos, 1.f));
	}

	//내 실제위치를 랜더링위치로 천천히옮김
	ActorPos += Vec3Normalize(MyPos - ActorPos) * 5.f * fTimeDelta;
	//내 실제위치와 랜더링위치가 다르다면 이동시킨다.
	ActorPos.y = RealPosY;
	if (ActorPos != m_RenderTransform->Get_State(STATE_POSITION))
	{
		ActorPos.y = 0.f;
		//내 실제위치와 랜더링위치의 차이가 얼마 안난다면 랜더링위치로 고정시킨다.
		if ((MyPos - ActorPos).Length() < 0.17f)
		{
			ActorPos = MyPos;
			m_pRigid->Set_LinearVelocity(_Vector3(0.f, 0.f, 0.f));
		}
			ActorPos.y = RealPosY;
		m_pTransformCom->Set_State(STATE_POSITION, _Vector4(ActorPos, 1.f));
	}
	else//내 실제위치와 랜더링위치가 같다면 강제로 재운다
	{
		m_pRigid->Set_Sleep(true);
	}

	return _int();
}

_int CGrass::LateTick(_float fTimeDelta)
{

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
		return 0;



	return _int();
}

HRESULT CGrass::Render(_uint iRenderGroup)
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

// HRESULT CGrass::Render_Shadow()
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

HRESULT CGrass::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_RenderTransform"), (CComponent**)&m_RenderTransform)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Grass_Base"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrass::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_RenderTransform->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrix");

	m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightTransform(TS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightTransform(TS_PROJ), sizeof(_float4x4));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CGrass* CGrass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGrass* pInstance = new CGrass(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CGrass");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGrass::Clone(void* pArg)
{
	CGrass* pInstance = new CGrass(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CGrass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrass::Free()
{

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_RenderTransform);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);

}

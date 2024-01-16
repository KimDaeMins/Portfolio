#include "stdafx.h"
#include "Bat.h"
#include "GameInstance.h"
#include "Bat_Head.h"
#include "Layer.h"
#include "Object_Manager.h"
#include "BatIdle.h"
#include "BatAttack.h"
#include "BatPursuit.h"
#include "BatSleep.h"
#include "BatTransition.h"
#include "BatTransition_2.h"
#include "BatHurt.h"
#include "BatRepeat.h"
#include "MonsterHpBar.h"
#include "MonsterHitPoint.h"
#include "Coin.h"
#include "Inventory.h"
#include "Skeleton.h"
#include "Sword.h"
CBat::CBat(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CBat::CBat(const CBat& rhs)
	: CMonster(rhs)
{
}

HRESULT CBat::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBat::NativeConstruct(void* pArg)
{
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (pArg == nullptr)
	{
		m_pTransformCom->Set_State(STATE_POSITION, XMVectorSet(15.f, 0.f, 10.f, 1.f));
		m_pTransformCom->Rotation(Vector3(0, 1, 0), XMConvertToRadians(90.f));
		Create_Status(_uint(DAMEGETYPE_TYPE::NORMAL), 0.f, 20.f, 20.f);
	}
	else
	{
		PUSHMONSTERDATA Data = *(PUSHMONSTERDATA*)pArg;
		if (m_bIsVoid = Data.Void)
			Create_Status(_uint(DAMEGETYPE_TYPE::VOIDTYPE), Data.Dmg, 20.f, 20.f);
		else
			Create_Status(_uint(DAMEGETYPE_TYPE::NORMAL), Data.Dmg, 20.f, 20.f);
		m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.Position, 1.f));
		m_pTransformCom->Rotation(Vector3(0, 1, 0), XMConvertToRadians(Data.YRotate));
		m_DropLevel = Data.DropLevel;
		m_DropType = Data.DropType;
		m_bIsWave = Data.isWave;
	}

	m_pStateMachine->SetHostObject(this);


	m_pStateMachine->Add_State(STATE_SLEEP, CBatSleep::Create(STATE_SLEEP));
	m_pStateMachine->Add_State(STATE_TRANSITION, CBatTransition::Create(STATE_TRANSITION));
	m_pStateMachine->Add_State(STATE_TRANSITION_2, CBatTransition_2::Create(STATE_TRANSITION_2));
	m_pStateMachine->Add_State(STATE_IDLE, CBatIdle::Create(STATE_IDLE));
	m_pStateMachine->Add_State(STATE_PURSUIT, CBatPursuit::Create(STATE_PURSUIT));
	m_pStateMachine->Add_State(STATE_ATTACK, CBatAttack::Create(STATE_ATTACK));
	m_pStateMachine->Add_State(STATE_HURT, CBatHurt::Create(STATE_HURT));
	m_pStateMachine->Add_State(STATE_REPEAT, CBatRepeat::Create(STATE_REPEAT));

	//여기서 모델을 만든다
	if (m_bIsVoid)
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Void_Bat"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		m_pModelCom->Set_Animator(m_pAnimatorCom);

		m_pStateMachine->SetCurrentState(STATE_TRANSITION_2);
	}
	else
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Bat"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		m_pModelCom->Set_Animator(m_pAnimatorCom);

		PUSHMONSTERDATA Data = *(PUSHMONSTERDATA*)pArg;
		if (pArg)
		{
			if (Data.isWave || Data.Void)
			{
				m_pStateMachine->SetCurrentState(STATE_TRANSITION_2);
			}
			else
			{
				m_pStateMachine->SetCurrentState(STATE_SLEEP);
			}
		}
		else
		{
			m_pStateMachine->SetCurrentState(STATE_SLEEP);
		}
	}
	m_Type = (_uint)OBJECT_TYPE::MONSTER;
	//-----------------------------------------------------
	// Create_Status( _DamegeType, _Damege, _Hp, _MaxHp )
	//-----------------------------------------------------


	m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_X, true);
	m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_Z, true);
	wstring ColliderName = L"Rigid";
	m_pRigid->Set_Host(this);
	m_pRigid->Create_BoxCollider(ColliderName, Vector3(0.8f, 0.5f, 0.8f), Vector3(0.f, 0.5f, 0.f));
	m_pRigid->Attach_Shape();
	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pRigid->Set_Gravity(true);
	m_pRigid->SetUp_Filtering(tagFilterGroup::MONSTER);
	m_pRigid->Set_Kinematic(false);
	m_pRigid->Set_Mess(0.6f);
	m_pRigid->Set_ActorLocalPos(_Vector3(0.f, 0.5f, 0.f));//이게 중간이 아니면 아래로 밀리나?

	Set_Culling(true);
	//m_pHead->Set_Culling(false);

	m_pHitPoint = static_cast<CMonsterHitPoint*>(pGameInstance->Add_GameObjectToLayerAndReturn(TEXT("Layer_BatHitPoint"), TEXT("Prototype_GameObject_MonsterHitPoint"), this));
	m_pHitPoint->SetUp_BoneMatrix(this, "tail.4");
	m_pHitPoint->SetUp_BoxCollider(Vector3(0.8f, 1.3f, 0.8f));
	m_pHitPoint->Set_TrailBoundary(STATE_LOOK, -1, 1.f, 0.15f);
	m_pHitPoint->Get_TrailBuffer()->Set_AliveTime(8.f);
	m_pHitPoint->Get_TrailBuffer()->Set_TrailSubtraction(true);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CBat::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
	if (XMVectorGetY(m_pTransformCom->Get_State(STATE_POSITION)) < -200.f)
		m_bDead = true;

	m_pStateMachine->Update_CurretState(fTimeDelta);
	if(STATE_SLEEP != m_pStateMachine->GetCurrentState()) // 죽었을때 추가해라 &&로 죽기전에
		m_pAnimatorCom->Update_Animation(fTimeDelta);

	//m_pTransformCom->MoveUsingAnimation(m_pAnimatorCom, nullptr, true);

	if (!m_bDead && Get_Hp() <= 0)
		Dead();

	return 0;
}

_int CBat::LateTick(_float fTimeDelta)
{
	_uint bCheck = __super::LateTick(fTimeDelta);
	CGameObject* pObj = nullptr;
	if (bCheck != -1)
	{
		while (m_pRigid->Get_Collision_Trigger_Stay(&pObj))
		{
			CMonsterHitPoint* pHitPoint = dynamic_cast<CMonsterHitPoint*>(pObj);
			if (pHitPoint != nullptr)
			{
				CSkeleton* pSkeleton = dynamic_cast<CSkeleton*>(pHitPoint->Get_HostObj());
				if (pSkeleton)
				{
					if (pSkeleton->Get_ModelType() != 1)
					{
						continue;
					}
				}
				else
					continue;
			}
			if (pObj->Get_Type() == (_int)OBJECT_TYPE::ATTACK)
			{
				Add_Hp(-1 * pObj->Get_Damege());
				Set_HitFlashing();

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->Play(L"en_sharedhit_", MULTY, 3, 4);
				RELEASE_INSTANCE(CGameInstance);

				CSword* pSword = dynamic_cast<CSword*>(pObj);
				if (pSword)
					pSword->Create_HitEffect();
				m_fEvadingTime = 0.f;

				if ((m_pStateMachine->GetCurrentState() == STATE_PURSUIT) || (m_pStateMachine->GetCurrentState() == STATE_IDLE))
				{
					m_pStateMachine->SetCurrentState(STATE_HURT);
				}
				if (m_pStateMachine->GetCurrentState() == STATE_HURT)
				{
					Set_Dir();
					m_pAnimatorCom->SetUp_Animation(CBat::HURT, false);

					m_iRepeatStack++;
					if (m_iRepeatStack > 2)
						m_pStateMachine->SetCurrentState(CBat::STATE_REPEAT);
				}

				break;
			}
		}
	}

	while (m_pRigid->Get_Collision_Trigger_Enter(&pObj))
	{
		if (pObj->Get_Type() == (_int)OBJECT_TYPE::ENTER_ATTACK)
		{
			Add_Hp(-1 * pObj->Get_Damege());
			Set_HitFlashing();
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Play(L"en_sharedhit_", MULTY, 3, 4);
			RELEASE_INSTANCE(CGameInstance);
			if ((m_pStateMachine->GetCurrentState() == STATE_PURSUIT) || (m_pStateMachine->GetCurrentState() == STATE_IDLE))
			{
				m_pStateMachine->SetCurrentState(STATE_HURT);
			}
			if (m_pStateMachine->GetCurrentState() == STATE_HURT)
			{
				Set_Dir();
				m_pAnimatorCom->SetUp_Animation(CBat::HURT, false);

				m_iRepeatStack++;
				if (m_iRepeatStack > 2)
					m_pStateMachine->SetCurrentState(CBat::STATE_REPEAT);
			}

			break;
		}
	}
	
	if (nullptr == m_pRendererCom)
		return -1;

	if (/*false == m_bIsVoid && false == m_bIsWave*/true)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return 0;
	}
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_GLOW, this)))
			return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
		return 0;

	return 1;
}

HRESULT CBat::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model2 */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Bat_Head"), TEXT("Com_Model2"), (CComponent**)&m_pModelCom2)))
		return E_FAIL;

	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachine)))
		return E_FAIL;

	/* For.Com_Animator */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Animator"), TEXT("Com_Animator"), (CComponent**)&m_pAnimatorCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid)))
		return E_FAIL;

	// Anim
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_AnimMesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// NonAnim
	/* For.Com_Shader2 */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader2"), (CComponent**)&m_pShaderCom2)))
		return E_FAIL;

	/* For.Com_Texture_Emit*/
	if (FAILED(Add_Component(TEXT("Prototype_Component_Texture_Bat_Emit"), TEXT("Com_Texture_Emit"), (CComponent**)&m_pEmitTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBat::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pModelCom2)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (STATE_SLEEP == m_pStateMachine->GetCurrentState())
	{
		m_pTransformCom->Bind_OnShader(m_pShaderCom2, "g_WorldMatrix");
		pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom2, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom2, "g_ProjMatrix");

		m_pShaderCom2->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightTransform(TS_VIEW), sizeof(_float4x4));
		m_pShaderCom2->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightTransform(TS_PROJ), sizeof(_float4x4));
	}
	else
	{
		m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
		pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrix");

		m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightTransform(TS_VIEW), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightTransform(TS_PROJ), sizeof(_float4x4));
	}


	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrixInverse", true);
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrixInverse", true);

	m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBat::Render(_uint iRenderGroup)
{
	if (CRenderer::RENDER_NONALPHA == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		if (false == m_bIsWave && false == m_bIsVoid && false == m_bHitFlashing)
		{
			if (STATE_SLEEP == m_pStateMachine->GetCurrentState())
			{
				_uint	iNumMaterials = m_pModelCom2->Get_NumMaterials();

				for (_uint i = 0; i < iNumMaterials; ++i)
				{
					m_pModelCom2->Set_ShaderResourceView(m_pShaderCom2, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

					m_pModelCom2->Render(m_pShaderCom2, i, 0);
				}
			}
			else
			{
				_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

				for (_uint i = 0; i < iNumMaterials; ++i)
				{
					m_pModelCom->Set_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
					m_pShaderCom->Set_ShaderResourceView("g_EmissionTexture", m_pEmitTexture->Get_SRV());
					m_pModelCom->Render(m_pShaderCom, i, 25);
				}
			}
		}
		else
		{
			_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

			for (_uint i = 0; i < iNumMaterials; ++i)
			{
				m_pModelCom->Render(m_pShaderCom, i, 31);
			}
		}
	}

	if (CRenderer::RENDER_NONLIGHT == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		if (m_bIsVoid)
		{
			_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

			//글로우셋팅
// 			m_pModelCom->Set_RawValue("g_vMtrlDiffuse", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
// 			m_pModelCom->Set_RawValue("g_vGlowColor_Front", &_float4(4.8f, 0.f, 6.f, 1.f), sizeof(_float4));
// 			m_pModelCom->Set_RawValue("g_vGlowColor_Back", &_float4(2.5f, 0.f, 5.f, 1.f), sizeof(_float4));

			if (m_bHitFlashing)
			{
				for (_uint i = 0; i < iNumMaterials; ++i)
				{
					//랜더링
					if (false == m_bHitFlashColorToggle)
					{
						m_pModelCom->Render(m_pShaderCom, i, 9);			// 피격 효과
					}
					else
					{
						m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
						m_pModelCom->Render(m_pShaderCom, i, 18);
					}
				}
			}
			else
			{
				m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
				for (_uint i = 0; i < iNumMaterials; ++i)
				{
					m_pModelCom->Render(m_pShaderCom, i, 11);
				}

				// edge
				m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4));
				for (_uint i = 0; i < iNumMaterials; ++i)
				{
					m_pModelCom->Render(m_pShaderCom, i, 12);
				}
			}

			//m_pHitPoint->SubRender();
		}
		//else if (STATE_SLEEP == m_pStateMachine->GetCurrentState())
		//{
		//	_uint	iNumMaterials = m_pModelCom2->Get_NumMaterials();

		//	for (_uint i = 0; i < iNumMaterials; ++i)
		//	{
		//		m_pModelCom2->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		//		//TODO : Shadow Pass 확인
		//		/*if (true == m_bIsShadow)
		//			m_pModelCom->Render(i, 1);
		//		else*/
		//		m_pModelCom2->Render(i, 0);
		//	}
		//}
		else if (STATE_SLEEP != m_pStateMachine->GetCurrentState())
		{
			_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

			for (_uint i = 0; i < iNumMaterials; ++i)
			{

				if (m_bHitFlashing)
				{
					if (false == m_bHitFlashColorToggle)
					{
						m_pModelCom->Render(m_pShaderCom, i, 9);			// 피격 효과
					}
					else
					{
						m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
						m_pModelCom->Render(m_pShaderCom, i, 18);
					}
				}
// 				else
// 				{
// 					m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
// 					m_pModelCom->Render(i, 0);
// 				}

			}
		}

		if (true == m_bIsWave)
		{
			if (false == m_bHitFlashing)
			{
				_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

				for (_uint i = 0; i < iNumMaterials; ++i)
				{
					m_pShaderCom->Set_RawValue("g_vRimColor", &_float4(0.8f, 0.f, 0.6f, 1.f), sizeof(_float3));
					m_pModelCom->Render(m_pShaderCom, i, 28);
				}
			}
		}
	}

	if (CRenderer::RENDER_GLOW == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		if (m_bIsVoid && false == m_bHitFlashing)
		{
			_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();
		
// 			//글로우셋팅
// 			m_pModelCom->Set_RawValue("g_vMtrlDiffuse", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
// 			m_pModelCom->Set_RawValue("g_vGlowColor_Front", &_float4(4.8f, 0.f, 6.f, 1.f), sizeof(_float4));
// 			m_pModelCom->Set_RawValue("g_vGlowColor_Back", &_float4(2.5f, 0.f, 5.f, 1.f), sizeof(_float4));
		
			for (_uint i = 0; i < iNumMaterials; ++i)
			{
				m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
				for (_uint i = 0; i < iNumMaterials; ++i)
				{
					m_pModelCom->Render(m_pShaderCom, i, 11);
				}

				// edge
				m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_float4(0.5f, 0.f, 1.f, 1.f), sizeof(_float4));
				for (_uint i = 0; i < iNumMaterials; ++i)
				{
					m_pModelCom->Render(m_pShaderCom, i, 12);
				}
			}
		}
		//else if (STATE_SLEEP != m_pStateMachine->GetCurrentState())
		//{
		//	_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();
		//
		//	for (_uint i = 0; i < iNumMaterials; ++i)
		//	{
		//		if (m_bHitFlashing)
		//		{
		//			if (false == m_bHitFlashColorToggle)
		//			{
		//				m_pModelCom->Set_RawValue("g_vMtrlDiffuse", &_float4(1.f, 0.5f, 0.f, 1.f), sizeof(_float4));
		//				m_pModelCom->Render(i, 2); // 피격 효과 glow
		//			}
		//		}
		//	}
		//}

	}
	else if (CRenderer::RENDER_DYNAMICSHADOW == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		if (STATE_SLEEP == m_pStateMachine->GetCurrentState())
		{
			_uint	iNumMaterials = m_pModelCom2->Get_NumMaterials();

			for (_uint i = 0; i < iNumMaterials; ++i)
			{
				m_pModelCom2->Render(m_pShaderCom2, i, 1);
			}
		}
		else
		{
			_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

			for (_uint i = 0; i < iNumMaterials; ++i)
			{
				m_pModelCom->Render(m_pShaderCom2, i, 1);
			}
		}
	}

	m_pHitPoint->SubRender(iRenderGroup);
	return S_OK;
}

void CBat::Dead()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//m_pHead->Set_Dead(true);
	m_pHitPoint->Set_Dead(true);
	Set_Dead(true);
	DeadEffect(2);
	if (m_bIsVoid)
	{
		pGameInstance->Play(L"en_tonguebatVoid_death_", MULTY, 33, 4, m_pTransformCom);
		pGameInstance->Play(L"en_tonguebatVoid_death_vo_", MULTY, 34, 7, m_pTransformCom);
	}
	else
	{
		pGameInstance->Play(L"en_tonguebat_death_", MULTY, 33, 4, m_pTransformCom);
		pGameInstance->Play(L"en_tonguebat_death_vo_", MULTY, 34, 7, m_pTransformCom);
	}

	switch (m_DropType)
	{
	case Client::CMonster::COIN:
		Create_Gold(2 * Get_DropLevel(), 1 * Get_DropLevel(), 0);
		break;
	case Client::CMonster::MANA:
		Create_Soul();
		break;
	default:
		break;
	}

	CInventory* pInventory = GET_INSTANCE(CInventory);
	if (pInventory->Get_CharmDatas() & (_uint)CHARM_TYPE::DROPHP)
		Create_Heart();
	RELEASE_INSTANCE(CInventory);

	RELEASE_INSTANCE(CGameInstance);
}

void CBat::Set_HitType(bool attacking)
{
	if (attacking)
	{
		m_pHitPoint->Set_Type((_uint)OBJECT_TYPE::ATTACK);
	}
	else
	{
		m_pHitPoint->Set_Type((_uint)OBJECT_TYPE::OBJ_END);
	}
}

//void CBat::Create_BatHead()
//{
//	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
//
//	static _int objCount = 0;
//
//	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Layer_Bat_Head_For"), TEXT("Prototype_GameObject_Bat_Head"))))
//		return;
//	
//	m_pHead = dynamic_cast<CBat_Head*>(pGameInstance->Get_GameObject(TEXT("Layer_Bat_Head_For"), objCount));
//	m_pHead->Get_Transform()->Set_WorldMatrix(Get_Transform()->Get_WorldMatrix());
//
//	Safe_AddRef(m_pHead);
//
//	RELEASE_INSTANCE(CGameInstance);
//}

void CBat::Move(float intervalTime)
{
	if (m_pAnimatorCom->Get_CurKeyFrame() <= 17)
	{
		m_pTransformCom->Set_SpeedPerSec(4.8f);
	}
	else
	{
		m_pTransformCom->Set_SpeedPerSec(2.4f);
	}

	m_pTransformCom->Go_Straight(intervalTime);
}

void CBat::Reset_RepeatPoint()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pGameInstance->Get_GameObject(TEXT("Layer_Player"));
	RELEASE_INSTANCE(CGameInstance);

	m_vRevolPoint = pTarget->Get_Transform()->Get_State(STATE_POSITION);
	m_vPlayerLookPoint = -1 * XMVector3Normalize(pTarget->Get_Transform()->Get_State(STATE_LOOK));
}

_bool CBat::Repeat(float intervalTime)
{
	Vector3 batPosition = m_pTransformCom->Get_State(STATE_POSITION);
	Vector3 dis = m_vRevolPoint - batPosition;
	dis.Normalize();
	m_pTransformCom->LookAtDir(dis);

	if (m_pAnimatorCom->Get_CurKeyFrame() <= 17)
	{
		m_pTransformCom->Set_SpeedPerSec(6.f);
	}
	else
	{
		m_pTransformCom->Set_SpeedPerSec(3.f);
	}

	m_pTransformCom->Go_Left(intervalTime);

	if (m_vPlayerLookPoint.Dot(dis) > cosf(XMConvertToRadians((180.f * 0.5f))))
		return true;
	else
		return false;
}

CBat* CBat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBat* pInstance = new CBat(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBat::Clone(void* pArg)
{
	CBat* pInstance = new CBat(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CBat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBat::Free()
{

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);

	Safe_Release(m_pEmitTexture);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom2);
	Safe_Release(m_pStateMachine);
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pRigid);

	//이놈은 shader 1개 더들고 있어서따로 Release 하나 해줌 기본 m_pShaderCom은 GameObject에서 하고있다.
	Safe_Release(m_pShaderCom2);

	


}

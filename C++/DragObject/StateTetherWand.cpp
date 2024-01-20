#include "stdafx.h"
#include "..\public\StateTetherWand.h"

#include "GameInstance.h"
#include "Player.h"
#include "TetherWand.h"
#include "TetherBeam.h"
#include "Monster.h"
#include "Effect_SoulBright.h"

CStateTetherWand::CStateTetherWand()
{
}

HRESULT CStateTetherWand::Enter(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//--------------------------------------
	// 여기서 필요한 컴포넌트를 불러온다
	//--------------------------------------
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;

	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	pAnimator->SetUp_Animation(59, false);
	pAnimator->Set_AnimationSpeed(1.0f);
	pAnimator->Set_IsBlending(ON);
	pAnimator->Set_BlendingTime(14.f);

	m_UsingTag = *(KEYTAG*)pArg;
	CInventory* pInventory = GET_INSTANCE(CInventory);
	pInventory->Get_UsingItem(m_UsingTag)->Set_Type((_uint)OBJECT_TYPE::USING);
	RELEASE_INSTANCE(CInventory);

	CTransform* pTransform = (CTransform*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Transform"));
	if (nullptr == pTransform)
		return E_FAIL;

	if (!(m_TargetLook = static_cast<CPlayer*>(m_pGameObject)->TargetLook()))
	{
		_vector			vMoveTargetDir = static_cast<CPlayer*>(m_pGameObject)->Get_MoveTargetDir();
		pTransform->LookAtDir(vMoveTargetDir);
	}

	m_Dir = Vec3Normalize(_Vector3(pTransform->Get_State(STATE_LOOK)));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateTetherWand::UpdateCurrentState(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//--------------------------------------
	// 여기서 필요한 컴포넌트를 불러온다
	//--------------------------------------
	CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
	if (nullptr == pStateMachine)
		return E_FAIL;
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	CInventory* pInventory = GET_INSTANCE(CInventory);

	CTransform* pTransform = m_pGameObject->Get_Transform();
	if (nullptr == pTransform)
		return E_FAIL;

	if (pAnimator->Get_CurrentAnimationIndex() == 59 && /*pAnimator->Get_IsBlending() == false*/pAnimator->Get_CurKeyFrame() >= 8 && m_Once)
	{
		((CTetherWand*)pInventory->Get_UsingItem(m_UsingTag))->SetUp_Anim(1);
		((CTetherWand*)pInventory->Get_UsingItem(m_UsingTag))->Update_Animation(fTimeDelta);
		((CTetherWand*)pInventory->Get_UsingItem(m_UsingTag))->Anim_Render(true);
		((CTetherWand*)pInventory->Get_UsingItem(m_UsingTag))->Set_LookDir(m_Dir);
		m_Once = false;
	}
	else if (m_Once == false && !m_Attack)
	{
		CAnimator* WeaponAnimator = (CAnimator*)pInventory->Get_UsingItem(m_UsingTag)->Get_ComponentPtr(TEXT("Com_Animator1"));
		pAnimator->PlayAnimationFrameSound(16, 18, L"pl_itm_wep_forcewand_start_", MULTY, 25, 3, 0.5f);
		if (WeaponAnimator->Get_IsFinished())
		{
			((CTetherWand*)pInventory->Get_UsingItem(m_UsingTag))->SetUp_Anim(0);
			((CTetherWand*)pInventory->Get_UsingItem(m_UsingTag))->Anim_Render(false);

			CModel* pModel = (CModel*)m_pGameObject->Get_ComponentPtr(L"Com_Model");
			pModel->Update_CombinedTransformationMatrix();
			//_Matrix					OffsetMatrix = pModel->Get_OffsetMatrix("hand.R");
			_Matrix					BoneMatrix = XMLoadFloat4x4(pModel->Get_CombinedMatrixPtr("hand.R"));
			_Matrix					PivotMatrix = pModel->Get_PivotMatrix();
			_Matrix					WorldMatrix = m_pGameObject->Get_Transform()->Get_WorldMatrix();

			_Vector3 Origin = ((/*OffsetMatrix * */BoneMatrix * PivotMatrix)* WorldMatrix).Translation();
			if (m_TargetLook && static_cast<CPlayer*>(m_pGameObject)->Get_Target())
			{
				wstring Rigid = L"Rigid";
				//스태틱몬스터라면 문제가있으려나
				_Vector3 ShapeCenter = 
				((CRigid*)static_cast<CPlayer*>(m_pGameObject)->Get_Target()->Get_ComponentPtr(L"Com_RigidBody"))->Get_ActorPos() +
				((CRigid*)static_cast<CPlayer*>(m_pGameObject)->Get_Target()->Get_ComponentPtr(L"Com_RigidBody"))->Get_LocalPos(Rigid);
				m_Dir = Vec3Normalize(ShapeCenter - Origin);
			}
			//내 손의 좌표, 내 손에서 뼈1까지의 Dir , 최대치Distance, 백버퍼, 1번
			CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
			CPhysicsSystem::RAYCASTBUFFER Buffer;
			_float Scale = 40.f;
			CTetherBeam::PUSHTENTAKLEDATA Data;
			CGameObject* HitObejct = nullptr;
			if (pPhysX->Raycast(Buffer, Origin, m_Dir, 40.f, tagFilterGroup::BOSS | tagFilterGroup::MONSTER | tagFilterGroup::PLANE , QUERYTYPE::SIMULATION))
			{
				pGameInstance->Play(L"pl_itm_wep_forcewand_grab_enemy_", MULTY, 26, 3);
				wstring Rigid = L"Rigid";
				Scale = Buffer.Distance/* - 4.f*/;//이정도 스케일 늘어나야되고 (1은 보정치)
				int iA = 0;
				HitObejct = ((CRigid*)Buffer.actor->userData)->Get_Host();
				if (HitObejct && HitObejct->Get_Type() != (_uint)OBJECT_TYPE::FLOOR)
				{
					m_Dir = Vec3Normalize(((CRigid*)Buffer.actor->userData)->Get_ActorPos() + ((CRigid*)Buffer.actor->userData)->Get_LocalPos(Rigid) - Origin);
					Data.DragGameObject = HitObejct;
				}
				else
				{
					m_Dir = Vec3Normalize(Buffer.Position - Origin);
				}
			}

			
			if (Data.DragGameObject)
				Data.DragAble = ((CMonster*)HitObejct)->Get_DragAble();
			else
				Data.DragAble = false;
			if (Data.DragAble)
			{
				if (static_cast<CPlayer*>(m_pGameObject)->Check_PlayerMp(14.f))
					static_cast<CPlayer*>(m_pGameObject)->Add_PlayerMp(-14.f);
				else
				{
					Data.DragAble = false;
					Data.DragGameObject = nullptr;
				}
			}

			// data.draggameobject가 nullptr이 아닐때 이펙트 발생
			if (nullptr != Data.DragGameObject)
			{
				_Vector4 vPos = Data.DragGameObject->Get_Transform()->Get_State(STATE_POSITION);	
				// Effect
				CSoulBright::PUSHSOULBRIGHTDATA Data;
				Data.HostObject = nullptr;
				Data.vColor = _float4(0.8f, 0.2f, 1.f, 1.f);
				Data.StartScale = 5.f;
				Data.EndScale = 12.f;
				Data.bBillBoard = true;
				Data.vPosition = vPos;
				pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_SoulBright", &Data);

			}


			Data.LocalPosition = _Vector3();
			Data.Look = m_Dir;
			Data.Scale = Scale;
			m_pDragger = pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_KKK", L"Prototype_GameObject_TetherBeam", &Data);



			RELEASE_INSTANCE(CPhysicsSystem);
			m_Attack = true;
		}

		((CTetherWand*)pInventory->Get_UsingItem(m_UsingTag))->Update_Animation(fTimeDelta);
	}
	if (pAnimator->Get_CurrentAnimationIndex() == 59 && pAnimator->Get_IsFinished())
	{
		pAnimator->SetUp_Animation(58, false);
	}
	if (pAnimator->Get_CurrentAnimationIndex() == 58 && pAnimator->Get_IsFinished())
	{
		pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
	}

	if (pAnimator->Get_CurrentAnimationIndex() == 58 && pAnimator->Get_CurKeyFrame() >= 8 && pAnimator->Get_CurKeyFrame() <= 30)
	{
		pAnimator->PlayAnimationFrameSound(7, 10, L"pl_itm_wep_forcewand_return_", MULTY, 25, 3, 0.5f);
		if (m_pDragger)
		{
			((CTetherBeam*)m_pDragger)->DragObject(1);
			if (m_pDragger->Get_Dead())
			{
				m_pDragger = nullptr;
			}
		}
	}
	RELEASE_INSTANCE(CInventory);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateTetherWand::Exit()
{
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	pAnimator->Set_BlendingTime(6.f);
	m_Once = true;
	m_Attack = false;
	if (m_pDragger)
	{
		m_pDragger->Set_Dead(true);
		m_pDragger = nullptr;
	}

	CInventory* pInventory = GET_INSTANCE(CInventory);
	((CTetherWand*)pInventory->Get_UsingItem(m_UsingTag))->Anim_Render(false);
	RELEASE_INSTANCE(CInventory);
	return S_OK;
}

CStateTetherWand* CStateTetherWand::Create(_uint iState)
{
	CStateTetherWand* pInstance = new CStateTetherWand();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateTetherWand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateTetherWand::Free()
{
}

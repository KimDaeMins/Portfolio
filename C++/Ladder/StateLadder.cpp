#include "stdafx.h"
#include "..\public\StateLadder.h"

#include "GameInstance.h"
#include "Player.h"
#include "StateItemMotionChange.h"


CStateLadder::CStateLadder()
{
}

HRESULT CStateLadder::Enter(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//--------------------------------------
	// 여기서 필요한 컴포넌트를 불러온다
	//--------------------------------------
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;

	CTransform* pTransform = m_pGameObject->Get_Transform();

	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------

	CRigid* pPlayerRigid = (CRigid*)m_pGameObject->Get_ComponentPtr(TEXT("Com_RigidBody"));
	wstring PlayerRigidName = L"Rigid";
	//pPlayerRigid->Set_isSimulation(PlayerRigidName, false);

	pPlayerRigid->Set_Gravity(false);
	CRigid* pRigid = (CRigid*)pArg;
	wstring ColliderName = L"Trigger";
	pTransform->Set_State(STATE_POSITION, _Vector4(pRigid->Get_ActorPos(), 1.f));

	if(pRigid->Get_Host()->Get_Type() == (_uint)OBJECT_TYPE::LADDER_DOWN)
		pTransform->LookAtDir(-pRigid->Get_LocalPos(ColliderName));
	else
		pTransform->LookAtDir(pRigid->Get_LocalPos(ColliderName));

	if (pRigid->Get_Host()->Get_Type() == (_uint)OBJECT_TYPE::LADDER_DOWN)
	{
		pAnimator->Set_IsBlending(OFF);
		pAnimator->Set_MoveUsingAnimDir(1.f);
		pAnimator->SetUp_Animation(3, false, true);
		pGameInstance->StopSound(SINGLE, PLAYER);
		pGameInstance->Play(L"pl_gen_ladder_topmount", SINGLE, PLAYER, 1);
		pPlayerRigid->Set_LinearVelocity(_Vector3(0.f, 0.f, 0.f));
		pPlayerRigid->Set_isSimulation(PlayerRigidName, false);
	}
	else
	{
		pAnimator->SetUp_Animation(4, true);
		pAnimator->Set_UseDiff(false);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateLadder::UpdateCurrentState(_float fTimeDelta)
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

	CRigid* pRigid = (CRigid*)m_pGameObject->Get_ComponentPtr(TEXT("Com_RigidBody"));
	if (nullptr == pRigid)
		return E_FAIL;
	CTransform* pTransform = m_pGameObject->Get_Transform();
	if (nullptr == pTransform)
		return E_FAIL;
	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	_uint iCurrentState = pStateMachine->GetCurrentState();
	_uint iAnimationIndex = pAnimator->Get_CurrentAnimationIndex();

	//pAnimator->Pause_Animation(true);

	if (iAnimationIndex == 2)
	{
		pAnimator->PlayAnimationFrameSound(5, 7,  L"pl_gen_ladder_topdismount", SINGLE, PLAYER, 1, 0.5f);
		if (pAnimator->Get_IsFinished())
		{
			CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
			CPhysicsSystem::RAYCASTBUFFER Buffer;
			_Vector3 Origin = pTransform->Get_State(STATE_POSITION);
			Origin.y += 0.5f;
			if (pPhysX->Raycast(Buffer, Origin, _Vector3(0.f, -1.f, 0.f), 2.7f, tagFilterGroup::PLANE, QUERYTYPE::SIMULATION))
			{
				pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Y, true);
				pTransform->Set_State(STATE_POSITION, Buffer.Position);
			}
			pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			RELEASE_INSTANCE(CPhysicsSystem);

		}
		else if (pAnimator->Get_CurKeyFrame() > 20)
		{
			wstring Rigid = L"Rigid";
			pRigid->Set_isSimulation(Rigid, true);
		}
	}
	//위에서 사다리를 타는경우
	if (iAnimationIndex == 3)
	{
		pAnimator->PlayAnimationFrameSound(15,17, L"pl_gen_ladder_topmount", SINGLE, PLAYER, 1, 0.5f);
		if (pAnimator->Get_IsFinished())
		{
			pAnimator->Set_MoveUsingAnimDir(1.f);
			pAnimator->SetUp_Animation(++iAnimationIndex, true);
			pAnimator->Set_UseDiff(false);
			wstring ColliderName = L"Rigid";
			pRigid->Set_isSimulation(ColliderName, true);
		}
	}
	//3번이 끝난경우 or 아래서 사다리를 타는경우
	if (iAnimationIndex == 4)
	{
		CGameObject* pGameObject = nullptr;

		pAnimator->Pause_Animation(true);

		if (pGameInstance->Key_Pressing(DIK_W))
		{
			pAnimator->PlayAnimationFrameSound(15,17, L"pl_gen_ladder_climb", SINGLE, PLAYER, 1, 0.5f);
			pAnimator->PlayAnimationFrameSound(30,32, L"pl_gen_ladder_climb", SINGLE, PLAYER, 1, 0.5f);
			pAnimator->Pause_Animation(false);
			pAnimator->Update_Animation(fTimeDelta);
			pTransform->Go_Up(fTimeDelta);
			pAnimator->Pause_Animation(true);

			while (pRigid->Get_Collision_Trigger_Stay(&pGameObject))
			{
				if (pGameObject->Get_Type() == (_uint)OBJECT_TYPE::LADDER_DOWN)
				{
					wstring ColliderName = L"Rigid";
					pRigid->Set_isSimulation(ColliderName, false);
					m_LadderRigid = (CRigid*)pGameObject->Get_ComponentPtr(TEXT("Com_RigidBody"));
					pAnimator->SetUp_Animation(2, false);
					pAnimator->Set_UseDiff(true);
					pAnimator->Pause_Animation(false);
				}
			}
		}
		else if (pGameInstance->Key_Pressing(DIK_S))
		{
			pAnimator->PlayAnimationFrameSound(16,18, L"pl_gen_ladder_climb", SINGLE, PLAYER, 1, 0.5f);
			pAnimator->PlayAnimationFrameSound(30,32, L"pl_gen_ladder_climb", SINGLE, PLAYER, 1, 0.5f);
			pAnimator->Pause_Animation(false);
			pAnimator->Update_Animation(-fTimeDelta);
			pTransform->Go_Down(fTimeDelta);
			pAnimator->Pause_Animation(true);

			while (pRigid->Get_Collision_Trigger_Stay(&pGameObject))
			{
				if (pGameObject->Get_Type() == (_uint)OBJECT_TYPE::LADDER_UP)
				{
					m_LadderRigid = (CRigid*)pGameObject->Get_ComponentPtr(TEXT("Com_RigidBody"));

					CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
					CPhysicsSystem::RAYCASTBUFFER Buffer;
					_Vector3 Origin = pTransform->Get_State(STATE_POSITION);
					Origin.y += 0.5f;
					if (pPhysX->Raycast(Buffer, Origin, _Vector3(0.f, -1.f, 0.f), 2.7f, tagFilterGroup::PLANE, QUERYTYPE::SIMULATION))
					{
						pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Y, true);
						pTransform->Set_State(STATE_POSITION, Buffer.Position);
						pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
					}
					else
					{
						pStateMachine->SetCurrentState(CPlayer::STATE_FALL);
					}
				}
				RELEASE_INSTANCE(CPhysicsSystem);
			}
		}

	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateLadder::Exit()
{
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	CRigid* pRigid = (CRigid*)m_pGameObject->Get_ComponentPtr(TEXT("Com_RigidBody"));
	if (nullptr == pRigid)
		return E_FAIL;

	pAnimator->Set_MoveUsingAnimDir(1.f);
	pAnimator->Set_IsBlending(ON);
	pAnimator->Set_UseDiff(true);
	pAnimator->Pause_Animation(false);
	wstring Rigid = L"Rigid";
	pRigid->Set_isSimulation(Rigid, true);
	pRigid->Set_Gravity(true);

	m_LadderRigid = nullptr;

	return S_OK;
}

CStateLadder* CStateLadder::Create(_uint iState)
{
	CStateLadder* pInstance = new CStateLadder();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateLadder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateLadder::Free()
{
}

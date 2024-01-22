#include "stdafx.h"
#include "..\public\StateThrowItem.h"

#include "GameInstance.h"
#include "Player.h"


#include "ThrowItem.h"

CStateThrowItem::CStateThrowItem()
{
}

HRESULT CStateThrowItem::Enter(void* pArg)
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
	pAnimator->SetUp_Animation(49, false);
	pAnimator->Set_AnimationSpeed(4.f);
	pAnimator->Set_IsBlending(ON);
	pAnimator->Set_BlendingTime(14.f);

	m_ItemType = *(_uint*)pArg;

	if (!static_cast<CPlayer*>(m_pGameObject)->TargetLook())
	{
		CTransform* pTransform = (CTransform*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Transform"));
		if (nullptr == pTransform)
			return E_FAIL;

		_vector			vMoveTargetDir = static_cast<CPlayer*>(m_pGameObject)->Get_MoveTargetDir();
		pTransform->LookAtDir(vMoveTargetDir);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateThrowItem::UpdateCurrentState(_float fTimeDelta)
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


	CTransform* pTransform = (CTransform*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Transform"));
	if (nullptr == pTransform)
		return E_FAIL;

	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	_uint iCurrentState = pStateMachine->GetCurrentState();

		if (pAnimator->Get_IsFinished())
		{
			//음 6프레임이후부터 날아가는데 보고 어색하면 바꾸기
			_uint CurrentAnimIndex = pAnimator->Get_CurrentAnimationIndex();
			if (CurrentAnimIndex != 50)
			{
				pAnimator->Set_AnimationSpeed(2.f);
				pAnimator->SetUp_Animation(50, false);
				pAnimator->Set_IsBlending(OFF);

				if (true)//핸드에올라온 아이템이 갯수가 있다면?
				{
					CThrowItem::PUSHTHROWITEMDATA Data;
					Data.ItemType = (CThrowItem::ITEMTYPE)m_ItemType;
					Data.LockOn = ((CPlayer*)m_pGameObject)->Get_KeepTargetLook();
					Data.Look = pTransform->Get_State(STATE_LOOK);
					Data.NearObject = ((CPlayer*)m_pGameObject)->Get_Target(); //니어오브젝트 확인가능할떄쯤 적용(락온)
					Data.Position = pTransform->Get_State(STATE_POSITION);
					if (FAILED(pGameInstance->Add_GameObjectToLayer(L"Layer_ThrowItem", L"Prototype_GameObject_ThrowItem", &Data)))
						return E_FAIL;
					
				}
			}
			else
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
		}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateThrowItem::Exit()
{
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	pAnimator->Set_BlendingTime(6.f);
	pAnimator->Set_IsBlending(OFF);

	return S_OK;
}

CStateThrowItem* CStateThrowItem::Create(_uint iState)
{
	CStateThrowItem* pInstance = new CStateThrowItem();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateThrowItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateThrowItem::Free()
{
}

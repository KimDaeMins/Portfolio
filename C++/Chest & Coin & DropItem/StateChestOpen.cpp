#include "stdafx.h"
#include "..\public\StateChestOpen.h"

#include "GameInstance.h"
#include "Player.h"



CStateChestOpen::CStateChestOpen()
{
}

HRESULT CStateChestOpen::Enter(void* pArg)
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
	pAnimator->SetUp_Animation(28, false);
	pAnimator->Set_AnimationSpeed(1.0f);
	pAnimator->Set_IsBlending(ON);
	pAnimator->Set_BlendingTime(14.f);


	CTransform* pTransform = (CTransform*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Transform"));
	if (nullptr == pTransform)
		return E_FAIL;

	//요래하면 되겠지요
	pTransform->LookAtDir(*(_Vector3*)pArg);


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateChestOpen::UpdateCurrentState(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//--------------------------------------
	// 여기서 필요한 컴포넌트를 불러온다
	//--------------------------------------
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	CTransform* pTransform = m_pGameObject->Get_Transform();
	if (nullptr == pTransform)
		return E_FAIL;

	pAnimator->PlayAnimationFrameSound(1,3, L"gen_prop_int_chest_open", SINGLE, PLAYER, 1, 0.5f);
	if (pAnimator->Get_IsFinished())
	{
		CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
		if (nullptr == pStateMachine)
			return E_FAIL;

		pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateChestOpen::Exit()
{
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	pAnimator->Set_BlendingTime(6.f);
	return S_OK;
}

CStateChestOpen* CStateChestOpen::Create(_uint iState)
{
	CStateChestOpen* pInstance = new CStateChestOpen();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateChestOpen");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateChestOpen::Free()
{
}

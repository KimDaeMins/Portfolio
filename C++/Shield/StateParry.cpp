#include "stdafx.h"
#include "..\public\StateParry.h"

#include "GameInstance.h"
#include "Player.h"
#include "Camera_Fly.h"


CStateParry::CStateParry()
{
}

HRESULT CStateParry::Enter(void* pArg)
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

	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	pAnimator->SetUp_Animation(29, false);
	pAnimator->Set_AnimationSpeed(1.f);
	pAnimator->Set_IsBlending(ON);
	pGameInstance->StopSound(SINGLE, PLAYER);
	pGameInstance->Play(L"pl_gen_parry_whoosh_", SINGLE, PLAYER, 3);

	CCamera_Fly* pCam = (CCamera_Fly*)pGameInstance->Get_GameObject(TEXT("Layer_Camera"));
	pCam->Set_OnDolly(50.5f, 0.5f);

	RELEASE_INSTANCE(CGameInstance);

	CInventory* pInventory = GET_INSTANCE(CInventory);
	pInventory->Get_Geer(GEAR_TYPE::SHIELD)->Set_Type((_uint)OBJECT_TYPE::OBJ_END);
	RELEASE_INSTANCE(CInventory);

	static_cast<CPlayer*>(m_pGameObject)->Add_PlayerSp(-50.f);


	return S_OK;
}

HRESULT CStateParry::UpdateCurrentState(_float fTimeDelta)
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

	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	_uint iCurrentState = pStateMachine->GetCurrentState();

	if (iCurrentState == CPlayer::STATE_PARRY)
	{
		if(pAnimator->Get_CurKeyFrame() > 25 && pAnimator->Get_CurKeyFrame() < 28)
		{						
		
			CInventory* pInventory = GET_INSTANCE(CInventory);
			pInventory->Get_Geer(GEAR_TYPE::SHIELD)->Set_Type((_uint)OBJECT_TYPE::SHIELDPARRY);
			RELEASE_INSTANCE(CInventory);
		}

		if (pAnimator->Get_IsFinished())
		{
			if (pGameInstance->Key_Down(DIK_SEMICOLON))
			{
				if (false == static_cast<CPlayer*>(m_pGameObject)->Check_LowSp())
					pStateMachine->SetCurrentState(CPlayer::STATE_SHIELD);
			}
			else
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateParry::Exit()
{
	CInventory* pInventory = GET_INSTANCE(CInventory);
	pInventory->Get_Geer(GEAR_TYPE::SHIELD)->Set_Type((_uint)OBJECT_TYPE::OBJ_END);
	RELEASE_INSTANCE(CInventory);
	return S_OK;
}

CStateParry* CStateParry::Create(_uint iState)
{
	CStateParry* pInstance = new CStateParry();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateParry");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateParry::Free()
{
}

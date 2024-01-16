#include "stdafx.h"
#include "..\public\StateAttack.h"

#include "GameInstance.h"
#include "Player.h"
#include "StateItemMotionChange.h"
#include "Sword.h"


CStateAttack::CStateAttack()
{
}

HRESULT CStateAttack::Enter(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_UsingTag = *(KEYTAG*)pArg;
	//--------------------------------------
	// 여기서 필요한 컴포넌트를 불러온다
	//--------------------------------------
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;

	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	pAnimator->SetUp_Animation(44, false);   // COMBO 1
	pAnimator->Set_AnimationSpeed(1.4f);
	pAnimator->Set_IsBlending(ON);
	pAnimator->Set_BlendingTime(14.f);

	if (!static_cast<CPlayer*>(m_pGameObject)->TargetLook())
	{
		CTransform* pTransform = (CTransform*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Transform"));
		if (nullptr == pTransform)
			return E_FAIL;

		_vector			vMoveTargetDir = static_cast<CPlayer*>(m_pGameObject)->Get_MoveTargetDir();
		pTransform->LookAtDir(vMoveTargetDir);
	}
	//// 강제 transform 화전 세팅
	//((CRigid*)m_pGameObject->Get_ComponentPtr(L"Com_RigidBody"))->Set_Transform((pTransform->Get_WorldMatrix()));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateAttack::UpdateCurrentState(_float fTimeDelta)
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
	CInventory* pInventory = GET_INSTANCE(CInventory);
	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	_uint iCurrentState = pStateMachine->GetCurrentState();

	if (iCurrentState == CPlayer::STATE_ATTACK)
	{
		if (pAnimator->PlayAnimationFrameSound(14, 16, L"pl_itm_wep_sword_com1_", SINGLE, PLAYER, 3, 0.5f))
		{
			pInventory->Get_UsingItem(m_UsingTag)->Set_Type((_uint)OBJECT_TYPE::ATTACK);
			pInventory->Get_UsingItem(m_UsingTag)->Set_DamegeType((_uint)DAMEGETYPE_TYPE::NORMAL);
		}

		if (pAnimator->Get_CurKeyFrame() > 35)
			pInventory->Get_UsingItem(m_UsingTag)->Set_Type((_uint)OBJECT_TYPE::OBJ_END);

		if (pAnimator->Get_IsFinished())
		{
			pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
		}
		else if (40 < pAnimator->Get_CurKeyFrame())
		{
			if (m_KeyTag != KEYTAG::KEY_END)
			{
				CStateItemMotionChange::MOTIONDATA Data(m_KeyTag);
				pStateMachine->SetCurrentState(CPlayer::STATE_ITEMMOTIONCHANGE, &Data);
			}
			else if (pGameInstance->Key_Pressing(DIK_W) || pGameInstance->Key_Pressing(DIK_S) || pGameInstance->Key_Pressing(DIK_A) || pGameInstance->Key_Pressing(DIK_D))
				pStateMachine->SetCurrentState(CPlayer::STATE_WALK);
			else if (pGameInstance->Key_Pressing(DIK_SEMICOLON))
			{
				if (false == static_cast<CPlayer*>(m_pGameObject)->Check_LowSp())
					pStateMachine->SetCurrentState(CPlayer::STATE_SHIELD);
			}
		}
		else if (30 < pAnimator->Get_CurKeyFrame())
		{
			if (m_bNextAttack)
			{
				CStateItemMotionChange::MOTIONDATA Data(m_KeyTag, USING_TYPE::SWORD, 1);
				pStateMachine->SetCurrentState(CPlayer::STATE_ITEMMOTIONCHANGE, &Data);
			}
			else if (pGameInstance->Key_Down(DIK_J) && !((CPlayer*)m_pGameObject)->Get_OnTep())
			{
				m_KeyTag = KEYTAG::KEY_J;
			}
			else if (pGameInstance->Key_Down(DIK_K) && !((CPlayer*)m_pGameObject)->Get_OnTep())
			{
				m_KeyTag = KEYTAG::KEY_K;
			}
			else if (pGameInstance->Key_Down(DIK_L) && !((CPlayer*)m_pGameObject)->Get_OnTep())
			{
				m_KeyTag = KEYTAG::KEY_L;
			}
		}
		else if (pGameInstance->Key_Down(DIK_J) && !((CPlayer*)m_pGameObject)->Get_OnTep())
		{
			m_bNextAttack = ON;
			m_KeyTag = KEYTAG::KEY_J;
		}
		else if (pGameInstance->Key_Down(DIK_K) && !((CPlayer*)m_pGameObject)->Get_OnTep())
		{
			m_bNextAttack = ON;
			m_KeyTag = KEYTAG::KEY_K;
		}
		else if (pGameInstance->Key_Down(DIK_L) && !((CPlayer*)m_pGameObject)->Get_OnTep())
		{
			m_bNextAttack = ON;
			m_KeyTag = KEYTAG::KEY_L;
		}

		if (29 < pAnimator->Get_CurKeyFrame())
		{
			// Trail Off
			CInventory* pInventory = GET_INSTANCE(CInventory);
			CSword* pSword = static_cast<CSword*>(pInventory->Get_UsingItem(m_UsingTag));
			if (nullptr != pSword)
				pSword->Set_TrailActive(OFF);
			RELEASE_INSTANCE(CInventory);

		}
		else if (15 < pAnimator->Get_CurKeyFrame())
		{
			// Trail On
			CInventory* pInventory = GET_INSTANCE(CInventory);
			CSword* pSword = static_cast<CSword*>(pInventory->Get_UsingItem(m_UsingTag));
			if (nullptr != pSword)
				pSword->Set_TrailActive(ON);
			RELEASE_INSTANCE(CInventory);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	RELEASE_INSTANCE(CInventory);
	return S_OK;
}

HRESULT CStateAttack::Exit()
{
	m_bNextAttack = OFF;
	m_KeyTag = KEYTAG::KEY_END;
	m_UsingTag = KEYTAG::KEY_END;
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	CInventory* pInventory = GET_INSTANCE(CInventory);
	pInventory->Get_UsingItem(m_UsingTag)->Set_Type((_uint)OBJECT_TYPE::OBJ_END);
	// trail off
	CSword* pSword = static_cast<CSword*>(pInventory->Get_UsingItem(m_UsingTag));
	if (nullptr != pSword)
		pSword->Set_TrailActive(OFF);
	RELEASE_INSTANCE(CInventory);
	pAnimator->Set_BlendingTime(6.f);
	pAnimator->Set_IsBlending(ON);
	return S_OK;
}

CStateAttack* CStateAttack::Create(_uint iState)
{
	CStateAttack* pInstance = new CStateAttack();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateAttack::Free()
{
}

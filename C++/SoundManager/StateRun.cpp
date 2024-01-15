#include "stdafx.h"
#include "..\public\StateRun.h"

#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "StateItemMotionChange.h"


CStateRun::CStateRun()
{
}

HRESULT CStateRun::Enter(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//--------------------------------------
	// 여기서 필요한 컴포넌트를 불러온다
	//--------------------------------------
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;

	CTransform* pTransform = (CTransform*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Transform"));
	if (nullptr == pTransform)
		return E_FAIL;

	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	pAnimator->SetUp_Animation(37);
	pAnimator->Set_IsBlending(OFF);
	pTransform->Set_RotationPerSec(XMConvertToRadians(25.f));

	static_cast<CPlayer*>(m_pGameObject)->Set_UseWorldUpdate(true);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateRun::UpdateCurrentState(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CInventory* pInventory = GET_INSTANCE(CInventory);
	//--------------------------------------
	// 여기서 필요한 컴포넌트를 불러온다
	//--------------------------------------
	CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
	if (nullptr == pStateMachine)
		return E_FAIL;

	CTransform* pTransform = m_pGameObject->Get_Transform();
	if (nullptr == pTransform)
		return E_FAIL;

	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;

	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	_uint iCurrentState = pStateMachine->GetCurrentState();

	if (iCurrentState == CPlayer::STATE_RUN)
	{
		// 이동 및 방향 전환 파트
		if (pGameInstance->Key_Down(DIK_SPACE))
		{
			if (false == static_cast<CPlayer*>(m_pGameObject)->Check_LowSp())
				pStateMachine->SetCurrentState(CPlayer::STATE_ROLL);
			else
				pStateMachine->SetCurrentState(CPlayer::STATE_HOP);
		}
		else if (pGameInstance->Key_Down(DIK_P) && pInventory->Get_Potion_Useable()
			&& ((pInventory->Get_CharmDatas() & (_uint)CHARM_TYPE::CONVERTPOTION && static_cast<CPlayer*>(m_pGameObject)->Get_PlayerMp() < static_cast<CPlayer*>(m_pGameObject)->Get_PlayerMaxMp())
				|| (!(pInventory->Get_CharmDatas() & (_uint)CHARM_TYPE::CONVERTPOTION) && m_pGameObject->Get_Hp() < m_pGameObject->Get_MaxHp())))
		{
			pStateMachine->SetCurrentState(CPlayer::STATE_POTION);
		}
		else if (pGameInstance->Key_Down(DIK_J) && !((CPlayer*)m_pGameObject)->Get_OnTep())
		{
			CStateItemMotionChange::MOTIONDATA Data(KEYTAG::KEY_J);
			pStateMachine->SetCurrentState(CPlayer::STATE_ITEMMOTIONCHANGE, &Data);
		}
		else if (pGameInstance->Key_Down(DIK_K) && !((CPlayer*)m_pGameObject)->Get_OnTep())
		{
			CStateItemMotionChange::MOTIONDATA Data(KEYTAG::KEY_K);
			pStateMachine->SetCurrentState(CPlayer::STATE_ITEMMOTIONCHANGE, &Data);
		}
		else if (pGameInstance->Key_Down(DIK_L) && !((CPlayer*)m_pGameObject)->Get_OnTep())
		{
			CStateItemMotionChange::MOTIONDATA Data(KEYTAG::KEY_L);
			pStateMachine->SetCurrentState(CPlayer::STATE_ITEMMOTIONCHANGE, &Data);
		}
		else if (pGameInstance->Key_Down(DIK_SEMICOLON) && pInventory->Get_Geer(GEAR_TYPE::SHIELD))
		{
			if (false == static_cast<CPlayer*>(m_pGameObject)->Check_LowSp())
				pStateMachine->SetCurrentState(CPlayer::STATE_SHIELD);
		}
		else if (!pGameInstance->Key_Pressing(DIK_SPACE))
			pStateMachine->SetCurrentState(CPlayer::STATE_WALK);
		else
		{
			pAnimator->PlayAnimationFrameSound(6, 8, L"pl_gen_footstep", SINGLE, PLAYER, 1, 0.5f);
			pAnimator->PlayAnimationFrameSound(31, 33, L"pl_gen_footstep", SINGLE, PLAYER, 1, 0.5f);
			_vector		vMoveTargetDir = static_cast<CPlayer*>(m_pGameObject)->Get_MoveTargetDir();			// 방향키가 향하고있는 방향

			if (0 >= XMVectorGetX(XMVector3Length(vMoveTargetDir)))
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			else
			{
				if (!static_cast<CPlayer*>(m_pGameObject)->TargetLookLerp(fTimeDelta))
				{
					_vector		vPlayerLook = pTransform->Get_State(STATE_LOOK);
					_vector		vPlayerLookNoY = XMVector3Normalize(XMVectorSet(XMVectorGetX(vPlayerLook), 0.f, XMVectorGetZ(vPlayerLook), 0.f));

					_vector		vNewPlayerLook = XMVectorSet(0.f, 0.f, 0.f, 0.f);
					_vector		vRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);
					_vector		vUp = XMVectorSet(0.f, 0.f, 0.f, 0.f);

					_vector			vMoveTargetDirDotPLookNoY = XMVector3Dot(vMoveTargetDir, vPlayerLookNoY);
					_vector			vMoveTargetDirRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vMoveTargetDir);			// 방향키 방향의 right
					_vector			vMoveTargetDirRightDotPLookNoY = XMVector3Dot(vMoveTargetDirRight, vPlayerLookNoY);
					if (0.f < XMVectorGetX(vMoveTargetDirDotPLookNoY))
					{
						// lerp 파트
						vNewPlayerLook = XMQuaternionSlerp(vPlayerLookNoY, vMoveTargetDir, fTimeDelta * 16.f);
						pTransform->Set_State(STATE_LOOK, XMVector3Normalize(vNewPlayerLook));

						vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), pTransform->Get_State(STATE_LOOK));
						pTransform->Set_State(STATE_RIGHT, XMVector3Normalize(vRight));

						vUp = XMVector3Cross(pTransform->Get_State(STATE_LOOK), vRight);
						pTransform->Set_State(STATE_UP, XMVector3Normalize(vUp));

						XMVECTOR vEps = XMVectorSet(0.2f, 1.f, 0.2f, 0.0f);
						if (XMVector3NearEqual(vPlayerLookNoY, vMoveTargetDir, vEps))
						{
							vNewPlayerLook = vMoveTargetDir;
							pTransform->Set_State(STATE_LOOK, XMVector3Normalize(vNewPlayerLook));

							vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), pTransform->Get_State(STATE_LOOK));
							pTransform->Set_State(STATE_RIGHT, XMVector3Normalize(vRight));

							vUp = XMVector3Cross(pTransform->Get_State(STATE_LOOK), vRight);
							pTransform->Set_State(STATE_UP, XMVector3Normalize(vUp));
						}
					}
					else
					{
						if (0.f < XMVectorGetX(vMoveTargetDirRightDotPLookNoY))
						{
							pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -16.f);
						}
						else
						{
							pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 16.f);
						}
					}
					pTransform->Move_Dir(vMoveTargetDir, fTimeDelta * 1.33f);
				}
				else
				{
					pStateMachine->SetCurrentState(CPlayer::STATE_WALK);
					pStateMachine->Update_CurretState(fTimeDelta);
				}
				/*pTransform->Go_Straight(fTimeDelta * 1.33f);*/
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	RELEASE_INSTANCE(CInventory);
	return S_OK;
}

HRESULT CStateRun::Exit()
{
	CTransform* pTransform = (CTransform*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Transform"));
	if (nullptr == pTransform)
		return E_FAIL;

	static_cast<CPlayer*>(m_pGameObject)->Set_UseWorldUpdate(false);
	pTransform->Set_RotationPerSec(XMConvertToRadians(45.f));
	return S_OK;
}

CStateRun* CStateRun::Create(_uint iState)
{
	CStateRun* pInstance = new CStateRun();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateRun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateRun::Free()
{
}

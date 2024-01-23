#include "stdafx.h"
#include "..\public\StateShield.h"

#include "GameInstance.h"
#include "Player.h"
#include "StateItemMotionChange.h"



CStateShield::CStateShield()
{
}

HRESULT CStateShield::Enter(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//--------------------------------------
	// 여기서 필요한 컴포넌트를 불러온다
	//--------------------------------------
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL; 

	CModel* pModel = (CModel*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Model"));
	if (nullptr == pModel)
		return E_FAIL;

	CRigid* pRigid = (CRigid*)m_pGameObject->Get_ComponentPtr(TEXT("Com_RigidBody"));
	if (nullptr == pModel)
		return E_FAIL;
	CTransform* pTransform = m_pGameObject->Get_Transform();
	if (nullptr == pTransform)
		return E_FAIL;

	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	pAnimator->SetUp_Animation(32);
	pGameInstance->StopSound(SINGLE, PLAYER);
	pGameInstance->Play(L"pl_gen_shield_up_", SINGLE, PLAYER, 3);
	m_RightControllBone = pModel->Get_ControllMatrixPtr("leg_upper.R");
	m_LeftControllBone = pModel->Get_ControllMatrixPtr("leg_upper.L");

	m_RightRotate = 0.f;
	m_LeftRotate = 0.f;
	m_LeftAdd = 60.f;
	m_RightAdd = -60.f;
	m_OnShieldTime = 0.f;
	m_ParryTime = 0.f;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateShield::UpdateCurrentState(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CInventory* pInventory = GET_INSTANCE(CInventory);
	//--------------------------------------
	// 여기서 필요한 컴포넌트를 불러온다
	//--------------------------------------
	CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
	if (nullptr == pStateMachine)
		return E_FAIL;

	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	CTransform* pTransform = m_pGameObject->Get_Transform();
	if (nullptr == pTransform)
		return E_FAIL;

	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	_uint iCurrentState = pStateMachine->GetCurrentState();
	m_OnShieldTime += fTimeDelta;
	if(pAnimator->Get_IsBlending())
	{
	}
	else if (iCurrentState == CPlayer::STATE_SHIELD)
	{
		pInventory->Get_Geer(GEAR_TYPE::SHIELD)->Set_Type((_uint)OBJECT_TYPE::SHIELD);
		if (30 < pAnimator->Get_CurKeyFrame())
		{
			pAnimator->Set_IsBlending(OFF);
			pAnimator->SetUp_Animation(32);
		}

		if (!pGameInstance->Key_Pressing(DIK_SEMICOLON))
		{
			if (m_OnShieldTime < 0.2f)
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_PARRY);
			}
			else
			{
				pGameInstance->StopSound(SINGLE, PLAYER);
				pGameInstance->Play(L"pl_gen_shield_down_", SINGLE, PLAYER, 3);
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
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
		else if (pGameInstance->Key_Down(DIK_SPACE))
		{
			if (0.f < static_cast<CPlayer*>(m_pGameObject)->Get_PlayerSp())
				pStateMachine->SetCurrentState(CPlayer::STATE_ROLL);
			else
				pStateMachine->SetCurrentState(CPlayer::STATE_HOP);
		}
		else if (pGameInstance->Key_Pressing(DIK_W) || pGameInstance->Key_Pressing(DIK_S) || pGameInstance->Key_Pressing(DIK_A) || pGameInstance->Key_Pressing(DIK_D))
		{
			CTransform* pTransform = (CTransform*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Transform"));
			if (nullptr == pTransform)
				return E_FAIL;
			_vector		vMoveTargetDir = static_cast<CPlayer*>(m_pGameObject)->Get_MoveTargetDir();			// 방향키가 향하고있는 방향

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
			}

			pTransform->Move_Dir(vMoveTargetDir, fTimeDelta * 0.6f);

			if (m_RightRotate < -m_MaxRotate)
				m_RightAdd = 60.f;
			else if (m_RightRotate > 0.f)
				m_RightAdd = -60.f;

			if (m_LeftRotate > m_MaxRotate)
				m_LeftAdd = -60.f;
			else if (m_LeftRotate < 0.f)
				m_LeftAdd = 60.f;

			//고정상수는 다리 이동속도(2면 왕복이 1초) 
			m_RightRotate += m_RightAdd * fTimeDelta * 1.7f;
			m_LeftRotate += m_LeftAdd * fTimeDelta * 1.7f;


			*m_RightControllBone = m_RightControllBone->CreateFromAxisAngle(_Vector3(1.f, 0.f, 0.f), XMConvertToRadians(m_RightRotate));
			*m_LeftControllBone = m_LeftControllBone->CreateFromAxisAngle(_Vector3(1.f, 0.f, 0.f), XMConvertToRadians(m_LeftRotate));
			//pModle->Update_Animation(fTimeDelta);
		}
		// todo : 공격을 맞고 스태미나가 0이 되면 stagger 상태로 변경
	}
	RELEASE_INSTANCE(CInventory);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateShield::Exit()
{
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;

	pAnimator->Set_IsBlending(ON);
	*m_RightControllBone = m_RightControllBone->CreateFromAxisAngle(_Vector3(1.f, 0.f, 0.f), 0.f);
	*m_LeftControllBone = m_LeftControllBone->CreateFromAxisAngle(_Vector3(1.f, 0.f, 0.f), 0.f);

	CInventory* pInventory = GET_INSTANCE(CInventory);
	pInventory->Get_Geer(GEAR_TYPE::SHIELD)->Set_Type((_uint)OBJECT_TYPE::OBJ_END);
	RELEASE_INSTANCE(CInventory);

	return S_OK;
}

CStateShield* CStateShield::Create(_uint iState)
{
	CStateShield* pInstance = new CStateShield();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateShield::Free()
{
}

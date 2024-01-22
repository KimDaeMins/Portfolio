#include "stdafx.h"
#include "..\public\StateIdle.h"

#include "GameInstance.h"
#include "Player.h"
#include "StateItemMotionChange.h"
#include "TeleportObject.h"
#include "WaveAltarFlame.h"
#include "Well.h"
#include "StateGetItem.h"
#include "UI_Item.h"
CStateIdle::CStateIdle()
{
}

HRESULT CStateIdle::Enter(void* pArg)
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

	pAnimator->SetUp_Animation(25);
	pAnimator->Set_IsBlending(ON);
	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CStateIdle::UpdateCurrentState(_float fTimeDelta)
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

	CTransform* pTranform = (CTransform*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Transform"));
	if (nullptr == pTranform)
		return E_FAIL;

	//--------------------------------------
	// 여기서 필요한 처리를 진행한다
	//--------------------------------------
	_uint iCurrentState = pStateMachine->GetCurrentState();

	if (iCurrentState == CPlayer::STATE_IDLE)
	{
		if (pGameInstance->Key_Pressing(DIK_W) || pGameInstance->Key_Pressing(DIK_S) || pGameInstance->Key_Pressing(DIK_A) || pGameInstance->Key_Pressing(DIK_D))
			pStateMachine->SetCurrentState(CPlayer::STATE_WALK);
		else if (pGameInstance->Key_Down(DIK_SPACE))
		{
			CRigid* pRigid = (CRigid*)m_pGameObject->Get_ComponentPtr(TEXT("Com_RigidBody"));
			if (nullptr == pRigid)
				return E_FAIL;
			_bool IsNotTriggerAction = true; 
			CGameObject* pGameObject = nullptr;

			//첫번째로 닿고있던 녀석만 적용됨
			while (pRigid->Get_Collision_Trigger_Stay(&pGameObject) && IsNotTriggerAction)
			{
				_uint Type = pGameObject->Get_Type();

				if (Type == (_uint)OBJECT_TYPE::LADDER_UP || Type == (_uint)OBJECT_TYPE::LADDER_DOWN)
				{
					wstring ColliderName = L"Trigger";
					pStateMachine->SetCurrentState(CPlayer::STATE_LADDER, pGameObject->Get_ComponentPtr(L"Com_RigidBody"));
					IsNotTriggerAction = false;
				}
				else if (Type == (_uint)OBJECT_TYPE::CHEST)
				{
					_Vector3 Position = m_pGameObject->Get_Transform()->Get_State(STATE_POSITION);
					_Vector3 TargetPos = pGameObject->Get_Transform()->Get_State(STATE_POSITION);
					pStateMachine->SetCurrentState(CPlayer::STATE_CHESTOPEN, &Vec3Normalize(TargetPos - Position));
					IsNotTriggerAction = false;
				}
				else if (Type == (_uint)OBJECT_TYPE::TELEPORT)
				{
					if (((CTeleportObject*)pGameObject)->OpenAbleCheck())
					{
						pStateMachine->SetCurrentState(CPlayer::STATE_KNEEL, pGameObject);
						IsNotTriggerAction = false;
					}
				}
				else if (Type == (_uint)OBJECT_TYPE::SHOPITEM)
				{
					// 상점 구매 UI를 띄우고 게임 시간을 멈추게 한다 (우린 안멈춤)
					pStateMachine->SetCurrentState(CPlayer::STATE_SHOPUI, pGameObject);
					IsNotTriggerAction = false;
				}
				else if (Type == (_uint)OBJECT_TYPE::WAVEALTARFLAME)
				{
					static_cast<CWaveAltarFlame*>(pGameObject)->Burn();
					IsNotTriggerAction = false;
				}
				else if (Type == (_uint)OBJECT_TYPE::WELL)
				{
					pStateMachine->SetCurrentState(CPlayer::STATE_WELLUI, pGameObject);
					IsNotTriggerAction = false;
				}
			}

			if (IsNotTriggerAction)
				static_cast<CPlayer*>(m_pGameObject)->TargetLookLerp(fTimeDelta);
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
		else
		{
			static_cast<CPlayer*>(m_pGameObject)->TargetLookLerp(fTimeDelta);

			CRigid* pRigid = (CRigid*)m_pGameObject->Get_ComponentPtr(TEXT("Com_RigidBody"));
			if (nullptr == pRigid)
				return E_FAIL;

			CGameObject* pGameObject = nullptr;

			//첫번째로 닿고있던 녀석만 적용됨
			while (pRigid->Get_Collision_Trigger_Stay(&pGameObject))
			{
				_uint Type = pGameObject->Get_Type();

				if (Type == (_uint)OBJECT_TYPE::WELL)
				{
					CGameObject* pGameObject = pGameInstance->Get_GameObject(L"Layer_Well");
					if (nullptr == pGameObject)
						return E_FAIL;

					_uint iCoinCount = ((CWell*)pGameObject)->GetUsingCoin();
					_uint iCharmSlotSize = ((CPlayer*)m_pGameObject)->GetCharmSlotSize();

					if (iCoinCount == iCharmSlotSize + 1)
					{
						pGameInstance->Play(L"pl_itm_msc_coin_flipToWell_wellBlast_reward", MULTY, 31, 1);

						((CWell*)pGameObject)->ZeroUsingCoin();
						CUI_Item::PUSHUIITEMDATA Data;

						Data.iItemNum = CUI_Item::UIITEM_CHARMSLOT;
						Data.iItemCnt = 1;
						static_cast<CStateGetItem*>(pStateMachine->Get_State(CPlayer::STATE_GETITEM))->Set_ItemData(&Data);
						pStateMachine->SetCurrentState(CPlayer::STATE_GETITEM);
						((CPlayer*)m_pGameObject)->SizeUp_Charm();
					}
				}
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	RELEASE_INSTANCE(CInventory);
	return S_OK;
}

HRESULT CStateIdle::Exit()
{
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;

	pAnimator->Set_IsBlending(ON);
	return S_OK;
}

CStateIdle* CStateIdle::Create(_uint iState)
{
	CStateIdle* pInstance = new CStateIdle();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateIdle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateIdle::Free()
{
}

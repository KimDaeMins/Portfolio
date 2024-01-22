#include "stdafx.h"
#include "..\public\StateItemMotionChange.h"

#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"
#include "Weapon.h"
#include "ConsumableItem.h"


CStateItemMotionChange::CStateItemMotionChange()
{
}

HRESULT CStateItemMotionChange::Enter(void* pArg)
{
	MOTIONDATA Data = *(MOTIONDATA*)pArg;

	CInventory* pInventory = GET_INSTANCE(CInventory);
	CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
	if (nullptr == pStateMachine)
	{
		RELEASE_INSTANCE(CInventory);
		return E_FAIL;
	}

	//비교할때는 내 현재 스테이트에서 USING_TYPE이 뭔지, 내가 몇번째 콤보인지, 내가누른녀석의 USING_TYPE이 뭔지.
	//Sword -> Sword면 콤보를 받아서 다음 SwordState로 이동
	//Sword -> Stick 이면 콤보체크 없이 첫번째 Stick으로 이동
	//End -> Sitck 이면 첫번째 Stick으로 이동

	USING_TYPE NextType;
	pInventory->Get_ItemType(Data.KeyNum, &NextType);

	if (NextType == USING_TYPE::TYPE_END)
	{
		pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
		RELEASE_INSTANCE(CInventory);
		return S_OK;
	}

	//무조건 첫번쨰로 가는 곳 BeforeType이 END인데 DeforeType과 NextType이 같은경우는 없을거임 사용자의 실수가 아닌이상
	if (Data.BeforeType == USING_TYPE::TYPE_END || Data.BeforeType != NextType)
	{
		_uint ItemType;
		switch (NextType)
		{
		case USING_TYPE::BANKBROKE:
			if (pInventory->Use_ConsumableItem(Data.KeyNum))
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_BANKBROCK);
			}
			else
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			break;
		case USING_TYPE::THROWITEM:
			if (pInventory->Use_ConsumableItem(Data.KeyNum , &ItemType))
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_THROWITEM, &ItemType);
			}
			else
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			break;
		case USING_TYPE::EATITEM:
			if (pInventory->Use_ConsumableItem(Data.KeyNum, &ItemType))
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_EATITEM, &ItemType);
			}
			else
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			break;
		case USING_TYPE::COIN:
			if (pInventory->Use_ConsumableItem(Data.KeyNum, &ItemType))
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_COINTOSS, &ItemType);
			}
			else
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			break;
		case USING_TYPE::SWORD:
			pStateMachine->SetCurrentState(CPlayer::STATE_ATTACK, &Data.KeyNum);
			break;
		case USING_TYPE::WAND:
			if (pInventory->Get_CharmDatas() & (_uint)CHARM_TYPE::ROCKETWAND)
			{
				if (static_cast<CPlayer*>(m_pGameObject)->Check_PlayerMp(40.f))
					pStateMachine->SetCurrentState(CPlayer::STATE_WAND, &Data.KeyNum);
				else
					pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			else
			{
				if (static_cast<CPlayer*>(m_pGameObject)->Check_PlayerMp(12.5f))
					pStateMachine->SetCurrentState(CPlayer::STATE_WAND, &Data.KeyNum);
				else
					pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			break;
		case USING_TYPE::TETHERWAND:
			pStateMachine->SetCurrentState(CPlayer::STATE_TETHERWAND, &Data.KeyNum);
			break;
		case USING_TYPE::ICE_DAGGER:
			if (static_cast<CPlayer*>(m_pGameObject)->Check_PlayerMp(50.f))
				pStateMachine->SetCurrentState(CPlayer::STATE_ICEDAGGER, &Data.KeyNum);
			else
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			break;
		case USING_TYPE::STICK:
			pStateMachine->SetCurrentState(CPlayer::STATE_STICKATTACK, &Data.KeyNum);
			break;
		default:
			RELEASE_INSTANCE(CInventory);
			return E_FAIL;
		}
	}
	else
	{
		switch (NextType)
		{
			_uint ItemType;
		case USING_TYPE::BANKBROKE:
			if (pInventory->Use_ConsumableItem(Data.KeyNum))
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_BANKBROCK);
			}
			else
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			break;
		case USING_TYPE::THROWITEM:
			if (pInventory->Use_ConsumableItem(Data.KeyNum, &ItemType))
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_THROWITEM, &ItemType);
			}
			else
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			break;
		case USING_TYPE::EATITEM:
			if (pInventory->Use_ConsumableItem(Data.KeyNum, &ItemType))
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_EATITEM, &ItemType);
			}
			else
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			break;
		case USING_TYPE::COIN:
			if (pInventory->Use_ConsumableItem(Data.KeyNum, &ItemType))
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_COINTOSS, &ItemType);
			}
			else
			{
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			break;
		case USING_TYPE::SWORD:
			switch (Data.Combo)
			{
			case 1:
				pStateMachine->SetCurrentState(CPlayer::STATE_ATTACK2, &Data.KeyNum);
				break;
			case 2:
				pStateMachine->SetCurrentState(CPlayer::STATE_ATTACK3, &Data.KeyNum);
				break;
			case 3:
				pStateMachine->SetCurrentState(CPlayer::STATE_ATTACK, &Data.KeyNum);
				break;
			default:
				return E_FAIL;
			}
			break;
		case USING_TYPE::STICK:
			switch (Data.Combo)
			{
			case 1:
				pStateMachine->SetCurrentState(CPlayer::STATE_STICKATTACK2, &Data.KeyNum);
				break;
			case 2:
				pStateMachine->SetCurrentState(CPlayer::STATE_STICKATTACK, &Data.KeyNum);
				break;
			default:
				return E_FAIL;
			}
			break;
		case USING_TYPE::TETHERWAND:
			pStateMachine->SetCurrentState(CPlayer::STATE_TETHERWAND, &Data.KeyNum);
			break;
		case USING_TYPE::WAND:
			if (static_cast<CPlayer*>(m_pGameObject)->Check_PlayerMp(12.5f))
				pStateMachine->SetCurrentState(CPlayer::STATE_WAND, &Data.KeyNum);
			else
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			break;
		case USING_TYPE::ICE_DAGGER:
			if (static_cast<CPlayer*>(m_pGameObject)->Check_PlayerMp(12.5f))
				pStateMachine->SetCurrentState(CPlayer::STATE_ICEDAGGER, &Data.KeyNum);
			else
				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			break;
		default:
			RELEASE_INSTANCE(CInventory);
			return E_FAIL;
		}
	}

	//마지막으로 쓴 아이템 변경
	pInventory->Use_Item(Data.KeyNum);

	RELEASE_INSTANCE(CInventory);
	return S_OK;
}

HRESULT CStateItemMotionChange::UpdateCurrentState(_float fTimeDelta)
{
	//업데이트를 하지않는다
	return S_OK;
}

HRESULT CStateItemMotionChange::Exit()
{
	//Exit도 따로 구현하지않는다.
	return S_OK;
}

CStateItemMotionChange* CStateItemMotionChange::Create(_uint iState)
{
	CStateItemMotionChange* pInstance = new CStateItemMotionChange();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateItemMotionChange");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateItemMotionChange::Free()
{
}

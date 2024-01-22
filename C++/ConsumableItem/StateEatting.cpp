#include "stdafx.h"
#include "..\public\StateEatting.h"

#include "GameInstance.h"
#include "Player.h"

CStateEatting::CStateEatting()
{
}

HRESULT CStateEatting::Enter(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//--------------------------------------
	// ���⼭ �ʿ��� ������Ʈ�� �ҷ��´�
	//--------------------------------------
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	CModel* pModel = (CModel*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Model"));
	if (nullptr == pAnimator)
		return E_FAIL;
	//--------------------------------------
	// ���⼭ �ʿ��� ó���� �����Ѵ�
	//--------------------------------------
	pAnimator->Set_IsBlending(ON);
	pAnimator->SetUp_Animation(20, false);

	m_ItemType = *(_uint*)pArg;
	m_HealMaxAmount = 100.f;				// ������ ����
	m_UsingHeal = false;
	m_HealAmount = 0.f;
	m_HealAccumulate = 0.f;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStateEatting::UpdateCurrentState(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//--------------------------------------
	// ���⼭ �ʿ��� ������Ʈ�� �ҷ��´�
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

	CInventory* pInventory = GET_INSTANCE(CInventory);
	//--------------------------------------
	// ���⼭ �ʿ��� ó���� �����Ѵ�
	//--------------------------------------
	_uint iCurrentState = pStateMachine->GetCurrentState();
	pAnimator->PlayAnimationFrameSound(18, 20, L"pl_itm_con_flask_MP", SINGLE, PLAYER, 1, 0.5f);
	if (pAnimator->Get_CurKeyFrame() >= 30 && pAnimator->Get_CurKeyFrame() <= 65)
	{
		if ((_uint)CONSUMABLEITEM_TYPE::REDFRUIT == m_ItemType)
		{
			if (!m_UsingHeal)
			{
				m_HealAmount = m_HealMaxAmount / 30.f;
				m_UsingHeal = true;
			}

			if (m_HealMaxAmount > m_HealAccumulate)
			{
				m_HealAccumulate += m_HealAmount;
				if (m_HealMaxAmount < m_HealAccumulate)
				{
					m_HealAmount = m_HealAccumulate - m_HealMaxAmount;
				}
				static_cast<CPlayer*>(m_pGameObject)->Add_PlayerHp(m_HealAmount);
			}
		}
		else if ((_uint)CONSUMABLEITEM_TYPE::BLUEFRUIT == m_ItemType)
		{
			if (!m_UsingHeal)
			{
				m_HealAmount = m_HealMaxAmount / 30.f;
				m_UsingHeal = true;
			}

			if (m_HealMaxAmount > m_HealAccumulate)
			{
				m_HealAccumulate += m_HealAmount;
				if (m_HealMaxAmount < m_HealAccumulate)
				{
					m_HealAmount = m_HealAccumulate - m_HealMaxAmount;
				}
				static_cast<CPlayer*>(m_pGameObject)->Add_PlayerMp(m_HealAmount);
			}
		}
		else if ((_uint)CONSUMABLEITEM_TYPE::PEPPER == m_ItemType)
		{
			if (!m_UsingHeal)
			{
				// �÷��̾� ���� ����(���ݷ�)
				m_UsingHeal = true;
			}
		}
		else if ((_uint)CONSUMABLEITEM_TYPE::HERB == m_ItemType)
		{
			if (!m_UsingHeal)
			{
				// �÷��̾� ���� ����(���¹̳� ȸ�� �ӵ� �����ε� ���ҷ��� ���̰ų� �ص� �ɵ�, �ƴϸ� �̼� ����)
				m_UsingHeal = true;
			}
		}
	}
	else if (pAnimator->Get_CurKeyFrame() >= 70 || pAnimator->Get_IsFinished())
	{
		pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
	}

	RELEASE_INSTANCE(CGameInstance);
	RELEASE_INSTANCE(CInventory);
	return S_OK;
}

HRESULT CStateEatting::Exit()
{
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;

	pAnimator->Set_IsBlending(ON);

	return S_OK;
}

CStateEatting* CStateEatting::Create(_uint iState)
{
	CStateEatting* pInstance = new CStateEatting();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateEatting");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateEatting::Free()
{
}

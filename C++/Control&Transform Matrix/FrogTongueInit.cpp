#include "stdafx.h"
#include "FrogTongueInit.h"
#include "Frog.h"
#include "GameInstance.h"

CFrogTongueInit* CFrogTongueInit::Create(_uint iState)
{
	CFrogTongueInit* pInstance = new CFrogTongueInit();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CFrogTongueInit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CFrogTongueInit::Enter(void* pArg)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
	if (nullptr == pStateMachine)
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	
	((CFrog*)m_pGameObject)->Set_Dir();
	m_AnimSpeed = 1.4f;
	pAnimator->Set_AnimationSpeed(m_AnimSpeed);

	m_ControlTranslationMatrix = pAnimator->Get_ControllTranslationMatrixPtr("hook");
	CRigid* pPlayerRigid = (CRigid*)pInstance->Get_ComponentPtr(TEXT("Layer_Player"), TEXT("Com_RigidBody"));
	_Vector3 PlayerCenterRigid = pPlayerRigid->Get_ActorPos() + pPlayerRigid->Get_ActorLocalPos();
	_Vector3 TongueMaxMove = m_pGameObject->Get_Transform()->Get_State(STATE_POSITION);
	//�� ���Ϸ� �����ٴ� �����Ͽ� �̰� ���ƾߵ�
	TongueMaxMove.y += PlayerCenterRigid.y;
	TongueMaxMove += m_pGameObject->Get_Transform()->Get_State(STATE_LOOK) * 6.f;

	//z�� ���ǹ����� �ٶ󺸴� �༮�� ���̸�ŭ ������
	m_TickMove = _Vector3(0.f, 0.f, 1.f) * (PlayerCenterRigid - TongueMaxMove).Length();

	//1�ʴ� �̵���? // 3.7�������� �ִԽ��ǵ尡 1�϶� �෹�̼� 51, TimeAcc 35�� 51/15�Ѱ��� 0.3���� ��������
	m_TickMove *= m_AnimSpeed * 3.7f;

	pAnimator->Set_IsBlending(false);
	pAnimator->SetUp_Animation(CFrog::TONGUE_INIT, false);

	return S_OK;
}

HRESULT CFrogTongueInit::UpdateCurrentState(_float fTimeDelta)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
	if (nullptr == pStateMachine)
		return E_FAIL;
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	CTransform* pTransform = m_pGameObject->Get_Transform();
	if (nullptr == pTransform)
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);
	
	if (pAnimator->Get_CurKeyFrame() >= 35)
	{
		pAnimator->PlayAnimationFrameSound(35, 37, L"en_frogBig_attackTongue_out_", MULTY, 42, 3, 0.5f, pTransform);
		_Vector3 FrameMove = m_TickMove * fTimeDelta;
		m_ControlTranslationMatrix->Translation(m_ControlTranslationMatrix->Translation() + (FrameMove / 100.f));
	}
	
	if (pAnimator->Get_IsFinished())
	{
		pStateMachine->SetCurrentState(CFrog::STATE_TONGUE_STAY);

	}

	return S_OK;
}

HRESULT CFrogTongueInit::Exit()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	pAnimator->Set_AnimationSpeed(1.f);
	pAnimator->Set_IsBlending(true);
	return S_OK;
}

void CFrogTongueInit::Free()
{
}

#include "stdafx.h"
#include "..\public\SpiderTank_LeftHandStump.h"

#include "GameInstance.h"
#include "SpiderTank.h"
#include "SpiderTank_AttackZone.h"
#include "SpiderTank_Body.h"


CSpiderTank_LeftHandStump::CSpiderTank_LeftHandStump()
{
}

HRESULT CSpiderTank_LeftHandStump::Enter(void* pArg)
{

	//--------------------------------------
	// 여기서 필요한 컴포넌트를 불러온다
	//--------------------------------------
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	CTransform* pTransform = m_pGameObject->Get_Transform();
	if (nullptr == pTransform)
		return E_FAIL;

	pAnimator->Set_IsBlending(ON);
	pAnimator->Set_AnimationSpeed(1.0f);
	pAnimator->Set_BlendingTime(6.f);//필요에따라 블랜딩타임을 조절해봅시다

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

;
	_Vector3 TargetPos = pGameInstance->Get_GameObject(L"Layer_Player")->Get_Transform()->Get_State(STATE_POSITION);
	_Vector3 MyPos = m_pGameObject->Get_Transform()->Get_State(STATE_POSITION);
	_Vector3 MyDir = m_pGameObject->Get_Transform()->Get_State(STATE_LOOK);
	_Vector3 ToTarget = TargetPos - MyPos - MyDir * 3.f;
	_bool  Right = MyDir.Cross(Vec3Normalize(ToTarget)).y > 0 ? true : false;
	ToTarget.y = 0.f;
	MyDir.y = 0.f;
	_float YAngle = XMConvertToDegrees(acosf(Vec3Normalize(ZeroY(MyDir)).Dot(Vec3Normalize(ZeroY(ToTarget)))));

	if ((Vec3Normalize(ZeroY(MyDir)) - Vec3Normalize(ZeroY(ToTarget))).Length() <= 0.003f)
	{
		YAngle = 0.f;
	}

	if(YAngle < 20.f)
		pAnimator->SetUp_Animation((_uint)CSpiderTank::ANIMATIONS::STUMP_FRONT, false);
	else if (Right)
		pAnimator->SetUp_Animation((_uint)CSpiderTank::ANIMATIONS::STUMP_RIGHT, false);
	else
		pAnimator->SetUp_Animation((_uint)CSpiderTank::ANIMATIONS::STUMP_LEFT, false);

	_Matrix RootCombined = *pAnimator->Get_CombinedMatrixPtr("root");
	_Matrix World = m_pGameObject->Get_Transform()->Get_WorldMatrix();
	_Matrix HandCombined = *pAnimator->Get_CombinedMatrixPtr("impact_location");

	m_HandLook = (HandCombined * World).Translation() - (RootCombined * World).Translation();
	m_HandLook = Vec3Normalize(ZeroY(m_HandLook));
	m_NowRotate = 0.f;
	m_LerpTime = 0.f;

	m_ControllImpact = pAnimator->Get_ControllTranslationMatrixPtr("impact_location");
	m_ControllShoulder = pAnimator->Get_ControllTranslationMatrixPtr("shoulder_L");
	((CSpiderTank*)m_pGameObject)->Set_Simulate(CSpiderTank_Body::ARM_L, false);
	((CSpiderTank*)m_pGameObject)->Set_Simulate(CSpiderTank_Body::ARM_R, false);
	RELEASE_INSTANCE(CGameInstance);
	m_bIsSounded = false;
	return S_OK;
}

HRESULT CSpiderTank_LeftHandStump::UpdateCurrentState(_float fTimeDelta)
{
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;

	if (!m_bIsSounded && !pAnimator->Get_IsBlending())
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Play(L"en_boss_spidertank_meleeBasicForward_vo", MULTY, 28, 1);
		pGameInstance->Play(L"en_boss_spidertank_meleeBasicForward", MULTY, 29, 1);
		m_bIsSounded = true;
		RELEASE_INSTANCE(CGameInstance);
	}

	if (pAnimator->Get_IsFinished())
	{
		CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
		if (nullptr == pStateMachine)
			return E_FAIL;

		pStateMachine->SetCurrentState((_uint)CSpiderTank::STATE::STATE_WALK);
	}
	else if (pAnimator->Get_CurKeyFrame() > 100)
	{
		if (m_LerpTime >= 1.f)
			m_LerpTime = 1.f;
		else
			m_LerpTime += fTimeDelta * 2.f;

		_float Rotate = CEasing::Lerp(m_NowRotate, 0.f, m_LerpTime, CEasing::EaseType::easeLiner);

		*m_ControllShoulder = _Matrix::CreateRotationY(XMConvertToRadians(Rotate));
		*m_ControllImpact = _Matrix::CreateRotationY(XMConvertToRadians(Rotate));

		if (Rotate <= 0.f)
		{
			*m_ControllShoulder = _Matrix();
			*m_ControllImpact = _Matrix();
		}
		else
		{
			*m_ControllShoulder = _Matrix::CreateRotationY(XMConvertToRadians(Rotate));
			*m_ControllImpact = _Matrix::CreateRotationY(XMConvertToRadians(Rotate));
		}
	}
	else if (!m_Created && pAnimator->Get_CurKeyFrame() >= 65)
	{
		m_Created = true;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CSpiderTank_AttackZone::PUSHSPIDERTANKATTACKZONEDATA Data;
		Data.Attack = 20.f;
		Data.Extent = 7.f;
		Data.Life = 0.2f;
		Data.Dir = _Vector3(1.f, 0.f, 0.f);
		CModel* pModel = (CModel*)m_pGameObject->Get_ComponentPtr(L"Com_Model");
		_Matrix Combined = *pModel->Get_CombinedMatrixPtr("impact_location");
		_Matrix Offset = pModel->Get_OffsetMatrix("impact_location");
		_Matrix Pivot = pModel->Get_PivotMatrix();
		_Matrix World = m_pGameObject->Get_Transform()->Get_WorldMatrix();
		Data.Position = ((Combined * Pivot) * World).Translation();
		pGameInstance->Add_GameObjectToLayer(L"Layer_BossObject", L"Prototype_GameObject_SpiderTank_AttackZone", &Data);
		RELEASE_INSTANCE(CGameInstance);
	}
	else if (pAnimator->Get_CurKeyFrame() >= 50 && pAnimator->Get_CurKeyFrame() < 60)
	{
		_Matrix RootCombined = *pAnimator->Get_CombinedMatrixPtr("root");
		_Matrix RootWorld = RootCombined * m_pGameObject->Get_Transform()->Get_WorldMatrix();
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_Vector3 PlayerPos = pGameInstance->Get_GameObject(L"Layer_Player")->Get_Transform()->Get_State(STATE_POSITION);
		RELEASE_INSTANCE(CGameInstance);
		_Vector3 ToPlayerDir = ToPlayerDir = Vec3Normalize(ZeroY(PlayerPos - RootWorld.Translation()));
		
		_Matrix ImpactCombined = *pAnimator->Get_CombinedMatrixPtr("impact_location");
		_Matrix ImpactWorld = ImpactCombined * m_pGameObject->Get_Transform()->Get_WorldMatrix();
		_Vector3 ImpactDir = Vec3Normalize(ZeroY(ImpactWorld.Translation() - RootWorld.Translation()));

		if (XMConvertToDegrees(acosf(ImpactDir.Dot(ToPlayerDir))) >= 0.75f)//내 손과 플레이어각이 0.75도보다 크거나 같을때 때(이동량(각) 의 4분의3정도를 넘어갔을 때)
		{
			_float Right = ImpactDir.Cross(ToPlayerDir).y > 0.f ? 1.f : -1.f;

			// 			if (XMConvertToDegrees(acosf(m_HandLook.Dot(ImpactDir))) < 20.f//내 손이 20도각 범위안에 있거나
			// 				|| XMConvertToDegrees(acosf(m_HandLook.Dot(ToPlayerDir))) < 20.f)//플레이어가 최초 20도각 범위안에 있을때
			// 			{
			_float Radian = XMConvertToRadians(35.f) * fTimeDelta * 6.f * Right;
			_Matrix Creat = _Matrix::CreateRotationY(Radian);// 10프레임동안 움직일것이니 6배를 해준다. 10프레임동안 20이동
			
			m_ControllImpact->Right(_Vector3::TransformNormal(m_ControllImpact->Right(), Creat));
			m_ControllImpact->Up(_Vector3::TransformNormal(m_ControllImpact->Up(), Creat));
			m_ControllImpact->Backward(_Vector3::TransformNormal(m_ControllImpact->Forward(), Creat));

			m_ControllShoulder->Right(_Vector3::TransformNormal(m_ControllShoulder->Right(), Creat));
			m_ControllShoulder->Up(_Vector3::TransformNormal(m_ControllShoulder->Up(), Creat));
			m_ControllShoulder->Backward(_Vector3::TransformNormal(m_ControllShoulder->Forward(), Creat));

			m_NowRotate = XMConvertToDegrees(QuaternionToEuler(CreateQuaternionFromMatrix(*m_ControllShoulder)).y);
			int iA = 10;
			//			}
		}
	}

	if (pAnimator->Get_CurKeyFrame() == 60)
	{
		((CSpiderTank*)m_pGameObject)->Set_LeftHandEffect(true);
		((CSpiderTank*)m_pGameObject)->Set_HandSteamEmitting(true);
	}
	else if (pAnimator->Get_CurKeyFrame() == 65)
	{
		((CSpiderTank*)m_pGameObject)->Set_HandSteamEmitting(false);
	}

	return S_OK;
}

HRESULT CSpiderTank_LeftHandStump::Exit()
{
	((CSpiderTank*)m_pGameObject)->Set_Simulate(CSpiderTank_Body::ARM_L, true);
	((CSpiderTank*)m_pGameObject)->Set_Simulate(CSpiderTank_Body::ARM_R, true);
	m_Created = false;

	return S_OK;
}

CSpiderTank_LeftHandStump* CSpiderTank_LeftHandStump::Create(_uint iState)
{
	CSpiderTank_LeftHandStump* pInstance = new CSpiderTank_LeftHandStump();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CSpiderTank_LeftHandStump");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpiderTank_LeftHandStump::Free()
{
}

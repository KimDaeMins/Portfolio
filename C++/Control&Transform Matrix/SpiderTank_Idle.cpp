#include "stdafx.h"
#include "..\public\SpiderTank_Idle.h"

#include "GameInstance.h"
#include "SpiderTank.h"
#include "ImGui_Manager.h"

unsigned int APIENTRY EntryThread(void* pArg)
{
	PUSHTHREADDATA pState = *(PUSHTHREADDATA*)pArg;
	CSpiderTank_Idle* Me = static_cast<CSpiderTank_Idle*>(pState.MyAddress);
	EnterCriticalSection(Me->Get_CriticalSection());

	Me->SetUp_Pattern(pState);

	LeaveCriticalSection(Me->Get_CriticalSection());

	return 0;
}
CSpiderTank_Idle::CSpiderTank_Idle()
{
}

HRESULT CSpiderTank_Idle::Enter(void* pArg)
{

	//--------------------------------------
	// 여기서 필요한 컴포넌트를 불러온다
	//--------------------------------------
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;

	if (m_isFinished && !m_Threading)//끝났는지, 업데이트에서 피니쉬 체크를 했는지를 판단함.
	{
		m_Threading = true;
		m_isFinished = false;
		InitializeCriticalSection(&m_CS);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		Data.MyMatrix = m_pGameObject->Get_Transform()->Get_WorldMatrix();
		Data.PlayerPosition = pGameInstance->Get_GameObject(L"Layer_Player")->Get_Transform()->Get_State(STATE_POSITION);
		Data.PatternDistance = _Vector4(9.f, 18.f, 25.f, 50.f);
		Data.MyAddress = this;
		m_hThread = (HANDLE)_beginthreadex(nullptr, 0, EntryThread, &Data, 0, nullptr);
		RELEASE_INSTANCE(CGameInstance);
	}


	pAnimator->Set_IsBlending(ON); // Idle로 갈땐 무조건 블랜딩을 먹여야합니다.
	pAnimator->SetUp_Animation((_uint)CSpiderTank::ANIMATIONS::IDLE, true);
	pAnimator->Set_AnimationSpeed(1.0f);
	pAnimator->Set_BlendingTime(6.f);//필요에따라 블랜딩타임을 조절해봅시다

	CModel* pModel = (CModel*)m_pGameObject->Get_ComponentPtr(L"Com_Model");
	m_pCombined = pModel->Get_CombinedMatrixPtr("head");
	m_pControllMatrix = pAnimator->Get_ControllMatrixPtr("head");

	return S_OK;
}

HRESULT CSpiderTank_Idle::UpdateCurrentState(_float fTimeDelta)
{
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	m_WaitTime += fTimeDelta;

#pragma region HEADSPIN
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CRigid* PlayerRigid = static_cast<CRigid*>(pGameInstance->Get_ComponentPtr(L"Layer_Player", L"Com_RigidBody"));
	wstring ColliderName = L"Rigid";
	_Vector3 PlayerPos = PlayerRigid->Get_ActorPos() + PlayerRigid->Get_LocalPos(ColliderName);
	_Vector3 MyPos = m_pGameObject->Get_Transform()->Get_State(STATE_POSITION);
	_Vector3 MyDir = m_pGameObject->Get_Transform()->Get_State(STATE_LOOK);
	_Vector3 ToTarget = PlayerPos - MyPos;
	_Matrix Combined = *m_pCombined;
	_Matrix World = m_pGameObject->Get_Transform()->Get_WorldMatrix();
	_Matrix TransformationMatrix = m_pControllMatrix->Invert() * Combined;
	_Matrix WorldAnimMatrix = TransformationMatrix * World;

	
	ToTarget = PlayerPos - WorldAnimMatrix.Translation();

	//왜 Look이랑 각도계산을 하지않냐면 Identity로 바꿔버리니까

	// Y축 회전량 따오기
	_float YRadian;
	if ((Vec3Normalize(ZeroY(MyDir)) - Vec3Normalize(ZeroY(ToTarget))).Length() <= 0.005f)
	{
		YRadian = 0.f;
	}
	else
	{
		YRadian = acosf(Vec3Normalize(ZeroY(MyDir)).Dot(Vec3Normalize(ZeroY(ToTarget))));
		// Y축 회전량 최대치 보정
		_float AngleLock = 45.f;
		if (YRadian > XMConvertToRadians(AngleLock))
			YRadian = XMConvertToRadians(AngleLock);

		if (ZeroY(MyDir).Cross(ToTarget).y < 0)
			YRadian *= -1.f;
	}
	// X축 회전량 따오기.
	_float XRadian;
	if (ToTarget.y == 0.f)
	{
		XRadian = 0.f;
	}
	else
	{
		XRadian = acosf(Vec3Normalize(ZeroY(ToTarget)).Dot(Vec3Normalize(ToTarget)));

		if (WorldAnimMatrix.Translation().y > PlayerPos.y)
			XRadian *= -1.f;
	}

	TransformationMatrix.Translation(_Vector3(0.f, 0.f, 0.f));
	_Matrix RotationMatrix = _Matrix::CreateFromYawPitchRoll(YRadian, XRadian, 0.f);
	_Matrix Sync = _Matrix::CreateRotationY(XMConvertToRadians(180.f));
	*m_pControllMatrix = RotationMatrix * Sync * TransformationMatrix.Invert();
	RELEASE_INSTANCE(CGameInstance);
#pragma endregion

	if (m_isFinished && m_WaitTime > 0.5f) 
	{
		WaitForSingleObject(m_hThread, INFINITE);

		DeleteObject(m_hThread);

		DeleteCriticalSection(&m_CS);

		/*int iA = 10;*/
		CloseHandle(m_hThread);
		m_hThread = NULL;

		CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
		if (nullptr == pStateMachine)
			return E_FAIL;
		
		_uint PatternIndex = static_cast<CSpiderTank*>(m_pGameObject)->Get_Pattern();
		if (PatternIndex != 9999)
		{
			pStateMachine->SetCurrentState(PatternIndex);
		}
		else
			pStateMachine->SetCurrentState(m_PatternCount + 1);		
		//pStateMachine->SetCurrentState(CSpiderTank::STATE_DEAD);


		m_WaitTime = 0.f;
		m_isFinished = true;
		m_Threading = false;
	}


	return S_OK;
}

HRESULT CSpiderTank_Idle::Exit()
{
	*m_pControllMatrix = _Matrix();
	return S_OK;
}

CSpiderTank_Idle* CSpiderTank_Idle::Create(_uint iState)
{
	CSpiderTank_Idle* pInstance = new CSpiderTank_Idle();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CSpiderTank_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpiderTank_Idle::Free()
{
}

void CSpiderTank_Idle::SetUp_Pattern(PUSHTHREADDATA Data)
{
	vector<_int>		SkillTable;
	_Matrix MyMatrix = Data.MyMatrix;
	_Vector3 MyDir = Vec3Normalize(MyMatrix.Forward());
	_Vector3 ToTarget = Data.PlayerPosition - MyMatrix.Translation() - MyDir * 3.f;
	_Vector3 ToTargetDir = Vec3Normalize(ToTarget);
	SPIDERPATTERNINFO pSpiderInfo = {};
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float Angle = XMConvertToDegrees(acosf(MyDir.Dot(ToTargetDir)));

	cout << "ToTargetDistance : " << ToTarget.Length() << endl;
	cout << "Angle : " << Angle << endl;

	if (ToTarget.Length() < Data.PatternDistance.x)
	{

		_bool InJumpZone = false;


		_Vector3 LeftMove = _Vector3::Transform(_Vector3(21.f, 0.f, 8.3f), MyMatrix);
		_Vector3 RightMove = _Vector3::Transform(_Vector3(-21.f, 0.f, 8.3f), MyMatrix);
		_Vector3 BackMove = _Vector3::Transform(_Vector3(0.f, 0.f, -15.3f), MyMatrix);

		LeftMove.y += 5.f;
		RightMove.y += 5.f;
		BackMove.y += 5.f;

		CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
		CPhysicsSystem::RAYCASTBUFFER Buffer;
		if (pPhysX->Raycast(LeftMove, _Vector3(0.f, -1.f, 0.f), 5.2f, Buffer, 2)
			|| pPhysX->Raycast(RightMove, _Vector3(0.f, -1.f, 0.f), 5.2f, Buffer, 2)
			|| pPhysX->Raycast(BackMove, _Vector3(0.f, -1.f, 0.f), 5.2f, Buffer, 2))
		{
			if (acosf(MyDir.Dot(ToTargetDir)) > acosf(cosf(XMConvertToRadians(150.f))))
			{
				//1				
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 1, DATATYPE_BOSS);
			}
			else if (acosf(MyDir.Dot(ToTargetDir)) < acosf(cosf(XMConvertToRadians(30.f))))
			{
				if (MyDir.Cross(ToTarget).y > 0.f)//오
				{
					//2
					pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 2, DATATYPE_BOSS);
				}
				else//왼
				{
					//3
					pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 3, DATATYPE_BOSS);
				}
			}
			else if (acosf(MyDir.Dot(ToTargetDir)) < acosf(cosf(XMConvertToRadians(55.f))))
			{
				if (MyDir.Cross(ToTarget).y > 0.f)//오
				{
					//4
					pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 4, DATATYPE_BOSS);
				}
				else//왼
				{
					//5
					pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 5, DATATYPE_BOSS);
				}
			}
			else
			{
				//6
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 6, DATATYPE_BOSS);
			}
		}
		else if(acosf(MyDir.Dot(ToTargetDir)) < acosf(cosf(XMConvertToRadians(30.f))))
		{
			if (MyDir.Cross(ToTarget).y > 0.f)//오
			{
				//7
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 7, DATATYPE_BOSS);
			}
			else//왼
			{
				//8
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 8, DATATYPE_BOSS);
			}
		}
		else if(acosf(MyDir.Dot(ToTargetDir)) < acosf(cosf(XMConvertToRadians(50.f))))
		{
			if (MyDir.Cross(ToTarget).y > 0.f)//오
			{
				//9
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 9, DATATYPE_BOSS);
			}
			else//왼
			{
				//10
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 10, DATATYPE_BOSS);
			}
		}
		else
		{
			//11
			pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 11, DATATYPE_BOSS);
		}
		RELEASE_INSTANCE(CPhysicsSystem);
	}
	else if (ToTarget.Length() < Data.PatternDistance.y)
	{
		if (acosf(MyDir.Dot(ToTargetDir)) < acosf(cosf(XMConvertToRadians(55.f))))
		{
			if (MyDir.Cross(ToTarget).y > 0.f)//오
			{
				//12
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 12, DATATYPE_BOSS);
			}
			else//왼
			{
				//13
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 13, DATATYPE_BOSS);
			}
		}
		else
		{
			//14
			pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 14, DATATYPE_BOSS);
		}
			
	}
	else if (ToTarget.Length() < Data.PatternDistance.z)//공격이많아지는거리
	{
		if (acosf(MyDir.Dot(ToTargetDir)) < acosf(cosf(XMConvertToRadians(45.f))))
		{
			if (MyDir.Cross(ToTarget).y > 0.f)//오
			{
				if (pGameInstance->Get_LayerSize(L"Layer_Monster") < 3)
				{
					//15
					pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 15, DATATYPE_BOSS);
				}
				else
				{
					//16
					pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 16, DATATYPE_BOSS);
				}
			}
			else//왼
			{
				if (pGameInstance->Get_LayerSize(L"Layer_Monster") < 3)
				{
					//17
					pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 17, DATATYPE_BOSS);
				}
				else
				{
					//18
					pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 18, DATATYPE_BOSS);
				}
			}
		}
		else//뒤쪽
		{
			if (pGameInstance->Get_LayerSize(L"Layer_Monster") < 3)
			{
				//19
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 19, DATATYPE_BOSS);
			}
			else
			{
				//20
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 20, DATATYPE_BOSS);
			}
		}
	}
	else if (ToTarget.Length() < Data.PatternDistance.w)//초원거리
	{
		if (acosf(MyDir.Dot(ToTargetDir)) < acosf(cosf(XMConvertToRadians(45.f))))
		{
			if (pGameInstance->Get_LayerSize(L"Layer_Monster") < 3)
			{
				//21
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 21, DATATYPE_BOSS);
			}
			else
			{
				//22
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 22, DATATYPE_BOSS);
			}
		}
		else
		{
			if (pGameInstance->Get_LayerSize(L"Layer_Monster") < 3)
			{
				//23
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 23, DATATYPE_BOSS);
			}
			else
			{
				//24
				pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 24, DATATYPE_BOSS);
			}
		}
	}
	else//초원거리
	{
		if (pGameInstance->Get_LayerSize(L"Layer_Monster") < 3)
		{
			//25
			pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 25, DATATYPE_BOSS);
		}
		else
		{
			//26
			pSpiderInfo = *pGameInstance->Get_RowDataToSpiderPattern(TEXT("SpiderPattern"), 26, DATATYPE_BOSS);
		}
	}

	SkillTable = { pSpiderInfo.Pattern[0], pSpiderInfo.Pattern[1], pSpiderInfo.Pattern[2], pSpiderInfo.Pattern[3], pSpiderInfo.Pattern[4]
	, pSpiderInfo.Pattern[5], pSpiderInfo.Pattern[6], pSpiderInfo.Pattern[7], pSpiderInfo.Pattern[8], pSpiderInfo.Pattern[9]
	, pSpiderInfo.Pattern[10], pSpiderInfo.Pattern[11] };

	m_PatternCount = (_uint)pGameInstance->GetPriority(SkillTable);
	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;
}
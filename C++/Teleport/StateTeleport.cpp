#include "stdafx.h"
#include "StateTeleport.h"
#include "GameInstance.h"
#include "Player.h"
#include "AnimationEffect.h"
CStateTeleport::CStateTeleport()
{
}

HRESULT CStateTeleport::Enter(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	RELEASE_INSTANCE(CGameInstance);
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	RELEASE_INSTANCE(CPhysicsSystem);
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
	pAnimator->Set_AnimationSpeed(1.f);
	pAnimator->Set_IsBlending(OFF);
	pAnimator->SetUp_Animation(24, false);
	pAnimator->Set_BlendingTime(13.f);

	pGameInstance->StopSound(SINGLE, PLAYER);
	pGameInstance->Play(L"pl_gen_hyperdash_01", SINGLE, PLAYER, 1);
	
	_vector			vMoveTargetDir = static_cast<CPlayer*>(m_pGameObject)->Get_MoveTargetDir();
	m_pGameObject->Get_Transform()->LookAtDir(vMoveTargetDir);
	
	m_StartPos = m_pGameObject->Get_Transform()->Get_State(STATE_POSITION);
	m_PlayerDir = m_pGameObject->Get_Transform()->Get_State(STATE_LOOK);

	_Vector3 NonBlockEndPos = m_StartPos + m_PlayerDir * 9.59f;
	_Vector3 EndPos = NonBlockEndPos;
	wstring Key = L"Rigid";
	_float PlayerRadius = static_cast<CRigid*>(m_pGameObject->Get_ComponentPtr(L"Com_RigidBody"))->Get_Radius(Key);
	_float	 Length = (m_StartPos - EndPos).Length();
	_float	 Gap = Length / 1.6f; //30도까진 체크할것.
	CPhysicsSystem::RAYCASTBUFFER Buffer;
	_Vector3 Origin = m_StartPos;
	Origin.y += 0.2f;//바닥과 닿아있는 상태이니까 
	if (pPhysX->Raycast(Buffer, Origin, m_PlayerDir, Length, tagFilterGroup::WALL | tagFilterGroup::PLANE, QUERYTYPE::SIMULATION)
		&& Buffer.Shape->getQueryFilterData().word0 & tagFilterGroup::WALL)
	{
		//바로 앞에벽이있다면 갭을 좀 낮게주자..
		Gap *= 0.6f;
	}
	EndPos.y += Gap;
	if (pPhysX->Raycast(Buffer, EndPos, _Vector3(0.f, -1.f, 0.f), Gap * 2, tagFilterGroup::PLANE | tagFilterGroup::WALL, QUERYTYPE::SIMULATION)
		&& Buffer.Shape->getQueryFilterData().word0 & tagFilterGroup::PLANE)
	{
		
			m_TeleportEndPos = Buffer.Position;
			m_Teleport = true;
	}
	//충돌감지가 안됐다면
	else
	{
		vector<CPhysicsSystem::RAYCASTBUFFER> Buffers = pPhysX->RaycastAll(Origin, m_PlayerDir, Length, tagFilterGroup::PLANE | tagFilterGroup::WALL, QUERYTYPE::SIMULATION);
		
		if (Buffers.empty())
		{
			//대각선아래로 땅이 있는데 끊겼는데 벽은 낮게만들어서 ?
			//MSG_BOX("벽을 낮게 만들었다.");
		}
		else
		{
			while (true)
			{
				if (Buffers.empty())
				{
					//왜이렇게생각하냐면 모든막힌곳엔 벽이있으니까 
					//MSG_BOX("그럼 내가 서있는곳은 땅이아닌것인가");
					break;
				}
				CGameObject* LastTarget = static_cast<CRigid*>(Buffers.back().actor->userData)->Get_Host();

				//마지막 충돌체가 땅이라면 - 언덕이다.. 위로된 언덕이다. 아래로된 언덕이였으면 이미 맞았을거임 첫레이캐스트에서
				if (LastTarget->Get_Type() == (_uint)OBJECT_TYPE::FLOOR)
				{
					_Vector3 Dir = Buffers.back().Normal;
					_float Radian = acosf(Vec3Normalize(ZeroY(Dir)).Dot(Vec3Normalize(Dir)));
					_Matrix Rotation = _Matrix::CreateRotationX(-Radian);
					Dir = _Vector3::TransformNormal(m_PlayerDir, Rotation);
					EndPos = Buffers.back().Position + Dir * (Length - Buffers.back().Distance);
					EndPos.y += Gap;

					if (pPhysX->Raycast(Buffer, EndPos, _Vector3(0.f, -1.f, 0.f), Gap * 2, tagFilterGroup::PLANE | tagFilterGroup::WALL, QUERYTYPE::SIMULATION))
					{
						//위에 땅이 있다
						if (Buffers.back().Shape->getQueryFilterData().word0 == tagFilterGroup::PLANE)
						{
							m_TeleportEndPos = Buffer.Position;
							m_Teleport = true;
							break;
						}
						//위에 벽이있다.
						else
						{
							//그뱡향으로 레이를 쏴서 맞은녀석
							EndPos = Buffers.back().Position;
							EndPos.y += 0.2f;
							Buffers = pPhysX->RaycastAll(EndPos, Dir, Length, tagFilterGroup::WALL, QUERYTYPE::SIMULATION);
							if (Buffers.empty())
							{
								//전방으로 쐇는데 마지막으로 맞은게 땅이고 
								//대각선위로 땅이 있는데 땅의 Normal으로 쭉 쐇는데 벽이없고
								//텔레포트 끝자락에서 위에서 아래로 레이를쐇는데 벽은있었다?
								//어떻게하면 이런경우가 나오지? 안나올거임
								//MSG_BOX("치명적 계산미스인가 오류인가");
							}
							else
							{
								_Vector3 LastRayPos = Buffers.back().Position - PlayerRadius * m_PlayerDir;
								LastRayPos.y += Gap * 2;
								if (pPhysX->Raycast(Buffer, LastRayPos, _Vector3(0.f, -1.f, 0.f), Gap * 4, tagFilterGroup::PLANE, QUERYTYPE::SIMULATION))
								{
									m_TeleportEndPos = Buffer.Position;
									m_Teleport = true;
									break;
								}
								else
								{
									//MSG_BOX("치명적 계산미스인가 오류인가");
								}
							}
						}
					}
					else//맵을 벗어나버렸다면 무조건 막는 벽이 있을것
					{
						Buffers = pPhysX->RaycastAll(Origin, Dir, Length, tagFilterGroup::PLANE | tagFilterGroup::WALL, QUERYTYPE::SIMULATION);
						if (Buffers.empty())
						{
							//대각선위로 땅이 있는데 땅의 Normal으로 쭉 쐇는데 벽도없고 땅도없고
							//텔레포트 끝자락에서 위에서 아래로 레이를쐇는데 땅도없었다?
							//어떻게하면 이런경우가 나오지? 안나올거임
							//MSG_BOX("치명적 계산미스인가 오류인가");
						}
						else
						{
							_Vector3 LastRayPos = Buffers.back().Position - PlayerRadius * m_PlayerDir;
							LastRayPos.y += Gap;
							if (pPhysX->Raycast(Buffer, LastRayPos, _Vector3(0.f, -1.f, 0.f), Gap * 2, tagFilterGroup::PLANE, QUERYTYPE::SIMULATION))
							{
								m_TeleportEndPos = Buffer.Position;
								m_Teleport = true;
								break;
							}
							else
							{
								//MSG_BOX("치명적 계산미스인가 오류인가");
							}
						}

					}
				}
				//마지막 충돌체가 벽이라면 -
				else
				{
			
					_Vector3 LastRayPos = Buffers.back().Position - PlayerRadius * m_PlayerDir;
					LastRayPos.y += Gap;
					if (pPhysX->Raycast(Buffer, LastRayPos, _Vector3(0.f, -1.f, 0.f), Gap * 2, tagFilterGroup::PLANE, QUERYTYPE::SIMULATION))
					{
						_Vector3 CheckPos = Buffer.Position;
						CheckPos.y += PlayerRadius + 0.1f;
						CheckPos -= m_PlayerDir * 0.1f;
						if (pPhysX->OverlapTest(CheckPos, tagFilterGroup::WALL | tagFilterGroup::PLANE, QUERYTYPE::SIMULATION))
						{
							Buffers.pop_back();
							continue;
						}
// 						_Vector3 CheckPos = NonBlockEndPos;
// 						CheckPos.y += 0.2f;
// 						vector<CPhysicsSystem::RAYCASTBUFFER> CheckBuffers = pPhysX->RaycastAll(CheckPos, -m_PlayerDir, Length, tagFilterGroup::PLANE | tagFilterGroup::WALL, QUERYTYPE::SIMULATION);
// 
// 						if (CheckBuffers.back().actor == Buffers.back().actor)
// 						{
// 							m_TeleportEndPos = Buffer.Position;
// 							m_Teleport = false;
// 						}
// 						else
// 						{
							m_TeleportEndPos = Buffer.Position;
							m_Teleport = true;
// 						}
						break;
					}
					else
					{
						Buffers.pop_back();
						continue;
					}
				}
			}
		}
	}
	wstring Name = L"Rigid";
	static_cast<CRigid*>(m_pGameObject->Get_ComponentPtr(L"Com_RigidBody"))->Set_isSimulation(Name, false);
	m_TeleportLength = (NonBlockEndPos - m_StartPos).Length();//원래가야할 길이
	m_FrameMove = (Vec3Normalize((m_TeleportEndPos - m_StartPos)) * m_TeleportLength) / 12.f;
	m_TeleportLength = (m_TeleportEndPos - m_StartPos).Length();//실제가는길이
	if (m_TeleportLength < 0.3f)
		m_NearTeleport = true;
	m_Frame = 0;

 	if(m_Teleport)
 		static_cast<CPlayer*>(m_pGameObject)->Add_PlayerSp(-20.f);
	return S_OK;
}

HRESULT CStateTeleport::UpdateCurrentState(_float fTimeDelta)
{
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	
	if (pAnimator->Get_IsBlending())//블랜딩중이면 리턴
	{
		return S_OK;
	}
	else
	{
		if (!m_CreateEffect)
		{
			//1.이펙트생성
			_Vector3		vPosition = m_StartPos;
			_Vector3		vLook;
			if (!m_NearTeleport)
				vLook = m_TeleportEndPos - m_StartPos;
			else
				vLook = m_PlayerDir;
			_Vector3		vRight = _Vector3(0.f, 1.f, 0.f).Cross(vLook);
			_Matrix LookAtMatrix;
			LookAtMatrix.Right(Vec3Normalize(vRight));
			LookAtMatrix.Up(Vec3Normalize(vLook.Cross(vRight)));
			LookAtMatrix.Backward(Vec3Normalize(vLook));
			LookAtMatrix.Translation(_Vector3(m_pGameObject->Get_Transform()->Get_State(STATE_POSITION)));
			LookAtMatrix = MatrixScaling(LookAtMatrix, _Vector3(0.75f, 0.75f, 0.75f));
			CAnimationEffect::PUSHANIMATIONEFFECTDATA Data(wstring(L"Prototype_Component_Model_Teleport"), 9999.f, LookAtMatrix, _Vector4(0.752f, 0.537f, 1.f, 1.f) * 0.5f, _Vector4(0.752f, 0.537f ,1.f, 1.f) * 0.5f);
			Data.AnimationTable.push(0);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_Effect = pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_AnimEffect", L"Prototype_GameObject_AnimationEffect", &Data);
				RELEASE_INSTANCE(CGameInstance);
			//2.객체랜더링 삭제 및 생성 로직
			static_cast<CPlayer*>(m_pGameObject)->Set_IsRendering(false);
			m_CreateEffect = true;
		}

		if (m_Teleport)//3.이동량을 12프레임으로 나눠서  방향으로 이동하자 강제이동
		{
			if (m_Frame >= 13)
			{
				//텔레포트매쉬 셋데드쳐야됨(있다면)
				if (m_Effect)
				{
					m_Effect->Set_Dead(true);
					m_Effect = nullptr;
				}
				CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
				if (nullptr == pStateMachine)
					return E_FAIL;

				pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
			}
			else
				++m_Frame;


			if (m_TeleportLength > (m_FrameMove * (_float)m_Frame).Length())
			{
				m_pGameObject->Get_Transform()->Set_State(STATE_POSITION, m_StartPos + m_FrameMove * (_float)m_Frame);
			}
			else
			{
				//텔레포트매쉬 셋데드쳐야됨
				if (m_Effect && m_Frame > 3)
				{
					m_Effect->Set_Dead(true);
					m_Effect = nullptr;
				}
				m_pGameObject->Get_Transform()->Set_State(STATE_POSITION, m_TeleportEndPos);
			}
		}
		else
		{
			CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
			if (nullptr == pStateMachine)
				return E_FAIL;

			pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
		}

		//4.혹시 애니메이션이 먼저끝났다면 아이들상태로 이럴일은 없을 것 같긴한데... 과감히지웁니다
// 		if (pAnimator->Get_IsFinished())
// 		{
// 			CStateMachine* pStateMachine = (CStateMachine*)m_pGameObject->Get_ComponentPtr(TEXT("Com_StateMachine"));
// 			if (nullptr == pStateMachine)
// 				return E_FAIL;
// 
// 			pStateMachine->SetCurrentState(CPlayer::STATE_IDLE);
// 		}
	}
	

    return S_OK;
}

HRESULT CStateTeleport::Exit()
{
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;
	pAnimator->Set_BlendingTime(14.f);
	pAnimator->Set_AnimationSpeed(1.f);
	static_cast<CPlayer*>(m_pGameObject)->Set_IsRendering(true);
	wstring Name = L"Rigid";
	static_cast<CRigid*>(m_pGameObject->Get_ComponentPtr(L"Com_RigidBody"))->Set_isSimulation(Name, true);
	m_CreateEffect = false;
	m_Teleport = false;
	m_NearTeleport = false;
	return S_OK;
}

CStateTeleport* CStateTeleport::Create(_uint iState)
{
	CStateTeleport* pInstance = new CStateTeleport();

	if (FAILED(pInstance->NativeConstruct(iState)))
	{
		MSG_BOX("Failed to Created CStateTeleport");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateTeleport::Free()
{
}

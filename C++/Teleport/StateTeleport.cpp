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
	// ���⼭ �ʿ��� ������Ʈ�� �ҷ��´�
	//--------------------------------------
	CAnimator* pAnimator = (CAnimator*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Animator"));
	if (nullptr == pAnimator)
		return E_FAIL;

	CTransform* pTransform = (CTransform*)m_pGameObject->Get_ComponentPtr(TEXT("Com_Transform"));
	if (nullptr == pTransform)
		return E_FAIL;

	//--------------------------------------
	// ���⼭ �ʿ��� ó���� �����Ѵ�
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
	_float	 Gap = Length / 1.6f; //30������ üũ�Ұ�.
	CPhysicsSystem::RAYCASTBUFFER Buffer;
	_Vector3 Origin = m_StartPos;
	Origin.y += 0.2f;//�ٴڰ� ����ִ� �����̴ϱ� 
	if (pPhysX->Raycast(Buffer, Origin, m_PlayerDir, Length, tagFilterGroup::WALL | tagFilterGroup::PLANE, QUERYTYPE::SIMULATION)
		&& Buffer.Shape->getQueryFilterData().word0 & tagFilterGroup::WALL)
	{
		//�ٷ� �տ������ִٸ� ���� �� ��������..
		Gap *= 0.6f;
	}
	EndPos.y += Gap;
	if (pPhysX->Raycast(Buffer, EndPos, _Vector3(0.f, -1.f, 0.f), Gap * 2, tagFilterGroup::PLANE | tagFilterGroup::WALL, QUERYTYPE::SIMULATION)
		&& Buffer.Shape->getQueryFilterData().word0 & tagFilterGroup::PLANE)
	{
		
			m_TeleportEndPos = Buffer.Position;
			m_Teleport = true;
	}
	//�浹������ �ȵƴٸ�
	else
	{
		vector<CPhysicsSystem::RAYCASTBUFFER> Buffers = pPhysX->RaycastAll(Origin, m_PlayerDir, Length, tagFilterGroup::PLANE | tagFilterGroup::WALL, QUERYTYPE::SIMULATION);
		
		if (Buffers.empty())
		{
			//�밢���Ʒ��� ���� �ִµ� ����µ� ���� ���Ը��� ?
			//MSG_BOX("���� ���� �������.");
		}
		else
		{
			while (true)
			{
				if (Buffers.empty())
				{
					//���̷��Ի����ϳĸ� ��縷������ ���������ϱ� 
					//MSG_BOX("�׷� ���� ���ִ°��� ���̾ƴѰ��ΰ�");
					break;
				}
				CGameObject* LastTarget = static_cast<CRigid*>(Buffers.back().actor->userData)->Get_Host();

				//������ �浹ü�� ���̶�� - ����̴�.. ���ε� ����̴�. �Ʒ��ε� ����̿����� �̹� �¾������� ù����ĳ��Ʈ����
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
						//���� ���� �ִ�
						if (Buffers.back().Shape->getQueryFilterData().word0 == tagFilterGroup::PLANE)
						{
							m_TeleportEndPos = Buffer.Position;
							m_Teleport = true;
							break;
						}
						//���� �����ִ�.
						else
						{
							//�ד������� ���̸� ���� �����༮
							EndPos = Buffers.back().Position;
							EndPos.y += 0.2f;
							Buffers = pPhysX->RaycastAll(EndPos, Dir, Length, tagFilterGroup::WALL, QUERYTYPE::SIMULATION);
							if (Buffers.empty())
							{
								//�������� �i�µ� ���������� ������ ���̰� 
								//�밢������ ���� �ִµ� ���� Normal���� �� �i�µ� ���̾���
								//�ڷ���Ʈ ���ڶ����� ������ �Ʒ��� ���̸��i�µ� �����־���?
								//����ϸ� �̷���찡 ������? �ȳ��ð���
								//MSG_BOX("ġ���� ���̽��ΰ� �����ΰ�");
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
									//MSG_BOX("ġ���� ���̽��ΰ� �����ΰ�");
								}
							}
						}
					}
					else//���� ������ȴٸ� ������ ���� ���� ������
					{
						Buffers = pPhysX->RaycastAll(Origin, Dir, Length, tagFilterGroup::PLANE | tagFilterGroup::WALL, QUERYTYPE::SIMULATION);
						if (Buffers.empty())
						{
							//�밢������ ���� �ִµ� ���� Normal���� �� �i�µ� �������� ��������
							//�ڷ���Ʈ ���ڶ����� ������ �Ʒ��� ���̸��i�µ� ����������?
							//����ϸ� �̷���찡 ������? �ȳ��ð���
							//MSG_BOX("ġ���� ���̽��ΰ� �����ΰ�");
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
								//MSG_BOX("ġ���� ���̽��ΰ� �����ΰ�");
							}
						}

					}
				}
				//������ �浹ü�� ���̶�� -
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
	m_TeleportLength = (NonBlockEndPos - m_StartPos).Length();//���������� ����
	m_FrameMove = (Vec3Normalize((m_TeleportEndPos - m_StartPos)) * m_TeleportLength) / 12.f;
	m_TeleportLength = (m_TeleportEndPos - m_StartPos).Length();//�������±���
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
	
	if (pAnimator->Get_IsBlending())//�������̸� ����
	{
		return S_OK;
	}
	else
	{
		if (!m_CreateEffect)
		{
			//1.����Ʈ����
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
			//2.��ü������ ���� �� ���� ����
			static_cast<CPlayer*>(m_pGameObject)->Set_IsRendering(false);
			m_CreateEffect = true;
		}

		if (m_Teleport)//3.�̵����� 12���������� ������  �������� �̵����� �����̵�
		{
			if (m_Frame >= 13)
			{
				//�ڷ���Ʈ�Ž� �µ����ľߵ�(�ִٸ�)
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
				//�ڷ���Ʈ�Ž� �µ����ľߵ�
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

		//4.Ȥ�� �ִϸ��̼��� ���������ٸ� ���̵���·� �̷����� ���� �� �����ѵ�... ����������ϴ�
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

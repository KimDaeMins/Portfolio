#include "..\public\RigidBody.h"
//#include "CharactorController.h"
#include "Transform.h"
CRigid::CRigid(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CComponent(pDevice, pDeviceContext)
{
}

CRigid::CRigid(const CRigid& rhs)
	:CComponent(rhs)
{
}

HRESULT CRigid::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CRigid::NativeConstruct(void* pArg)
{

	PUSHRIGIDDATA Data;
	if (pArg != nullptr)
		Data = *(PUSHRIGIDDATA*)pArg;

	m_UseGravity = Data.GravityAble;
	m_Mass = Data.Mass;

	m_LockFlag[PR::PR_ROT][LOCK::LOCK_X] = Data.LockFlag[PR::PR_ROT][LOCK::LOCK_X];
	m_LockFlag[PR::PR_ROT][LOCK::LOCK_Y] = Data.LockFlag[PR::PR_ROT][LOCK::LOCK_Y];
	m_LockFlag[PR::PR_ROT][LOCK::LOCK_Z] = Data.LockFlag[PR::PR_ROT][LOCK::LOCK_Z];
	m_LockFlag[PR::PR_POS][LOCK::LOCK_X] = Data.LockFlag[PR::PR_POS][LOCK::LOCK_X];
	m_LockFlag[PR::PR_POS][LOCK::LOCK_Y] = Data.LockFlag[PR::PR_POS][LOCK::LOCK_Y];
	m_LockFlag[PR::PR_POS][LOCK::LOCK_Z] = Data.LockFlag[PR::PR_POS][LOCK::LOCK_Z];
	m_InertiaTensor = Data.InertiaTensor;
	m_AngularDamping = Data.AngularDamping;
	m_Kinematic = Data.Kinematic;
	m_StaticRigid = Data.StaticRigid;
	Create_Actor();
	Actor_Setting();
	Set_UserData(this);
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	//pPhysX->Add_Actor(m_Actor);
	pPhysX->Attach_Rigid(this);
	RELEASE_INSTANCE(CPhysicsSystem);
	return S_OK;
}

void CRigid::Create_Actor()
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	if (m_StaticRigid)
	{
		m_Actor = pPhysX->Create_StaticActor(XMMatrixIdentity());
	}
	else
	{
		m_Actor = pPhysX->Create_DynamicActor(XMMatrixIdentity());
	}

	RELEASE_INSTANCE(CPhysicsSystem);
}

void CRigid::Actor_Setting()
{
	if (!m_StaticRigid)
	{
		if (m_UseGravity)
			m_Actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
		else
			m_Actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		if (m_Kinematic)
			m_Actor->is<PxRigidDynamic>()->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		else
			m_Actor->is<PxRigidDynamic>()->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);

		PxRigidDynamicLockFlags Flag = m_LockFlag[PR::PR_POS][LOCK::LOCK_X] ? PxRigidDynamicLockFlag::eLOCK_LINEAR_X : PxRigidDynamicLockFlags();
		Flag |= m_LockFlag[PR::PR_POS][LOCK::LOCK_Y] ? PxRigidDynamicLockFlag::eLOCK_LINEAR_Y : Flag;
		Flag |= m_LockFlag[PR::PR_POS][LOCK::LOCK_Z] ? PxRigidDynamicLockFlag::eLOCK_LINEAR_Z : Flag;
		Flag |= m_LockFlag[PR::PR_ROT][LOCK::LOCK_X] ? PxRigidDynamicLockFlag::eLOCK_ANGULAR_X : Flag;
		Flag |= m_LockFlag[PR::PR_ROT][LOCK::LOCK_Y] ? PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y : Flag;
		Flag |= m_LockFlag[PR::PR_ROT][LOCK::LOCK_Z] ? PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z : Flag;

		m_Actor->is<PxRigidDynamic>()->setRigidDynamicLockFlags(Flag);

		m_Actor->setActorFlag(PxActorFlag::eVISUALIZATION, true);

		m_Actor->is<PxRigidBody>()->setMass(m_Mass);
		m_Actor->is<PxRigidBody>()->setAngularDamping(m_AngularDamping);
		m_Actor->is<PxRigidBody>()->setLinearDamping(0.3f);//테스트
		PxVec3 Inertia;
		memcpy(&Inertia, &m_InertiaTensor, sizeof(PxVec3));
		m_Actor->is<PxRigidBody>()->setMassSpaceInertiaTensor(Inertia);
	}
	
}

void CRigid::Create_BoxCollider(wstring ColliderName, _Vector3 Extents, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	m_Colliders.emplace(ColliderName, CCollider::Create_BoxCollider(Extents, LocalPos, LocalEuler));
}

void CRigid::Create_SphereCollider(wstring ColliderName, float Radian, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	m_Colliders.emplace(ColliderName, CCollider::Create_SphereCollider(Radian, LocalPos, LocalEuler));
}

void CRigid::Create_CapsuleCollider(wstring ColliderName, float Radian, float Height, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	m_Colliders.emplace(ColliderName, CCollider::Create_CapsuleCollider(Radian, Height, LocalPos, LocalEuler));
}

void CRigid::Create_MeshCollider(wstring ColliderName, float scaleFactor, CModel* model, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	m_Colliders.emplace(ColliderName, CCollider::Create_MeshCollider(scaleFactor, model, LocalPos, LocalEuler));
}

void CRigid::Create_ConvexMeshCollider(wstring ColliderName, float scaleFactor, CModel* model, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	m_Colliders.emplace(ColliderName, CCollider::Create_ConvexMeshCollider(scaleFactor, model, LocalPos, LocalEuler));
}

void CRigid::Create_NaviCollider(wstring ColliderName, float scaleFactor, CNavigation* NaviGation, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	m_Colliders.emplace(ColliderName, CCollider::Create_NaviCollider(scaleFactor, NaviGation, LocalPos, LocalEuler));
}

_bool CRigid::Get_IsStatic()
{
	return m_StaticRigid;
}

void CRigid::Set_Sleep(_bool sleep)
{
	if (m_StaticRigid)
		return;

	if (sleep)
		m_Actor->is<PxRigidDynamic>()->putToSleep();
	else
		m_Actor->is<PxRigidDynamic>()->wakeUp();
}

_bool CRigid::Get_Sleep()
{
	return 	m_Actor->is<PxRigidDynamic>()->isSleeping();
}

_bool CRigid::Get_Kinematic()
{
	if (m_StaticRigid)
		return NULL;

	return m_Kinematic;
}

void CRigid::Set_Kinematic(_bool Kinematic)
{
	if (m_StaticRigid)
		return;

	//트라이앵글매쉬나 하이트맵, 플레인은 키네마틱이 지원되지않음
	for (auto& Pair : m_Colliders)
	{
		if (*Pair.second->Get_Type() == GEOMETRYTYPE::GT_TRIANGLEMESH)
			return;
	}

	m_Kinematic = Kinematic;
	m_Actor->is<PxRigidDynamic>()->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, m_Kinematic);
}

_bool CRigid::Get_UseGravity()
{
	if (m_StaticRigid)
		return NULL;

	return m_UseGravity;
}

void CRigid::Set_Gravity(_bool _GravityAble)
{
	if (m_StaticRigid)
		return;

	if (m_UseGravity == _GravityAble)
		return;


	m_UseGravity = _GravityAble;
	m_Actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !m_UseGravity);
}

_bool CRigid::Get_FreezePosition(LOCK _Dir)
{
	if (m_StaticRigid)
		return NULL;

	return m_LockFlag[PR::PR_POS][_Dir];
}

_bool CRigid::Get_FreezeRotation(LOCK _Dir)
{
	if (m_StaticRigid)
		return NULL;

	return m_LockFlag[PR::PR_ROT][_Dir];
}

void CRigid::Set_LockFlag(PR _ROPO, LOCK _Dir, _bool Lock)
{
	if (m_StaticRigid)
		return;

	if (m_LockFlag[_ROPO][_Dir] == Lock)
		return;

	m_LockFlag[_ROPO][_Dir] = Lock;

	switch (_ROPO * 3 + _Dir)
	{
	case 0:
		m_Actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, Lock);
		break;
	case 1:
		m_Actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, Lock);
		break;
	case 2:
		m_Actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, Lock);
		break;
	case 3:
		m_Actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, Lock);
		break;
	case 4:
		m_Actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, Lock);
		break;
	case 5:
		m_Actor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, Lock);
		break;
	}
}

void CRigid::Set_LockFlag(PR _ROPO, _bool LockX, _bool LockY, _bool LockZ)
{
	if (m_StaticRigid)
		return;

	m_LockFlag[_ROPO][LOCK::LOCK_X] = LockX;
	m_LockFlag[_ROPO][LOCK::LOCK_Y] = LockY;
	m_LockFlag[_ROPO][LOCK::LOCK_Z] = LockZ;

	PxRigidDynamicLockFlags Now = m_Actor->is<PxRigidDynamic>()->getRigidDynamicLockFlags();
	PxRigidDynamicLockFlags Flag;
	switch (_ROPO)
	{
	case PR::PR_POS:
		Flag =  m_LockFlag[PR::PR_POS][LOCK::LOCK_X] ? PxRigidDynamicLockFlag::eLOCK_LINEAR_X : PxRigidDynamicLockFlags();
		Flag |= m_LockFlag[PR::PR_POS][LOCK::LOCK_Y] ? PxRigidDynamicLockFlag::eLOCK_LINEAR_Y : Flag;
		Flag |= m_LockFlag[PR::PR_POS][LOCK::LOCK_Z] ? PxRigidDynamicLockFlag::eLOCK_LINEAR_Z : Flag;
		break;
	case PR::PR_ROT:
		Flag =  m_LockFlag[PR::PR_ROT][LOCK::LOCK_X] ? PxRigidDynamicLockFlag::eLOCK_ANGULAR_X : PxRigidDynamicLockFlags();
		Flag |= m_LockFlag[PR::PR_ROT][LOCK::LOCK_Y] ? PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y : Flag;
		Flag |= m_LockFlag[PR::PR_ROT][LOCK::LOCK_Z] ? PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z : Flag;
		break;
	}
	m_Actor->is<PxRigidDynamic>()->setRigidDynamicLockFlags(Now | Flag);
}

void CRigid::Set_LockFlag(_uint LockFlag)
{
	PxRigidDynamicLockFlags Flag = PxRigidDynamicLockFlags();
	Flag |= LockFlag & PxRigidDynamicLockFlag::eLOCK_ANGULAR_X ? PxRigidDynamicLockFlag::eLOCK_ANGULAR_X : PxRigidDynamicLockFlags();
	Flag |= LockFlag & PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y ? PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y : PxRigidDynamicLockFlags();
	Flag |= LockFlag & PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z ? PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z : PxRigidDynamicLockFlags();
	Flag |= LockFlag & PxRigidDynamicLockFlag::eLOCK_LINEAR_X ? PxRigidDynamicLockFlag::eLOCK_LINEAR_X : PxRigidDynamicLockFlags();
	Flag |= LockFlag & PxRigidDynamicLockFlag::eLOCK_LINEAR_Y ? PxRigidDynamicLockFlag::eLOCK_LINEAR_Y : PxRigidDynamicLockFlags();
	Flag |= LockFlag & PxRigidDynamicLockFlag::eLOCK_LINEAR_Z ? PxRigidDynamicLockFlag::eLOCK_LINEAR_Z : PxRigidDynamicLockFlags();
	m_Actor->is<PxRigidDynamic>()->setRigidDynamicLockFlags(Flag);
}

_float CRigid::Get_Mess()
{
	if (m_StaticRigid)
		return NULL;

	return m_Mass;
}

void CRigid::Set_Mess(_float _Mass)
{
	if (m_StaticRigid)
		return;

	m_Mass = _Mass;
	m_Actor->is<PxRigidBody>()->setMass(m_Mass);

}

_float CRigid::Get_AngularDamping()
{
	if (m_StaticRigid)
		return NULL;

	return m_AngularDamping;
}

void CRigid::Set_AnglurDamping(_float _Damping)
{
	if (m_StaticRigid)
		return;

	m_AngularDamping = _Damping;
	m_Actor->is<PxRigidBody>()->setAngularDamping(m_AngularDamping);
}

_Vector3 CRigid::Get_InertiaTensor()
{
	if (m_StaticRigid)
		return _Vector3();

	return m_InertiaTensor;
}

void CRigid::Set_InertiaTensor(_Vector3 Inertia)
{
	if (m_StaticRigid)
		return;
	m_InertiaTensor = Inertia;

	PxVec3 Int;
	memcpy(&Int, &m_InertiaTensor, sizeof(PxVec3));
	m_Actor->is<PxRigidBody>()->setMassSpaceInertiaTensor(Int);
}

_Vector3 CRigid::Get_ActorPos()
{
	_Vector3 Pos;
	memcpy(&Pos, &m_Actor->getGlobalPose().p, sizeof(_Vector3));
	return Pos;
}

_Vector3 CRigid::Get_ActorLocalPos()
{
	if (m_StaticRigid)
		return _Vector3();

	_Vector3 Pos;
	memcpy(&Pos, &m_Actor->is<PxRigidDynamic>()->getCMassLocalPose().p, sizeof(_Vector3));
	return Pos;
}

void CRigid::Set_ActorPos(_Vector3 Position)
{
	if (m_Kinematic)
	{
		if (m_StaticRigid)
			return;
		PxVec3 Pos;
		memcpy(&Pos, &Position, sizeof(PxVec3));
		m_Actor->is<PxRigidDynamic>()->setKinematicTarget(PxTransform(Pos, m_Actor->getGlobalPose().q));
	}
	else
	{
		PxVec3 Pos;
		memcpy(&Pos, &Position, sizeof(PxVec3));
		m_Actor->is<PxRigidActor>()->setGlobalPose(PxTransform(Pos, m_Actor->getGlobalPose().q), false);
	}
}

void CRigid::Set_ActorLocalPos(_Vector3 Pos)
{
	if (m_StaticRigid)
		return;

	PxVec3 Position;
	memcpy(&Position, &Pos, sizeof(PxVec3));
	m_Actor->is<PxRigidDynamic>()->setCMassLocalPose(PxTransform(Position));
}

_Vector3 CRigid::Get_ActorCenter()
{ 
	PxVec3 Position;

	if(!m_StaticRigid)
		Position = m_Actor->is<PxRigidDynamic>()->getCMassLocalPose().p + m_Actor->getGlobalPose().p;
	else 
		Position = m_Actor->getGlobalPose().p;

	_Vector3 Push;
	memcpy(&Push, &Position, sizeof(_Vector3));
	return Push;
}

_Vector4 CRigid::Get_ActorRotation(_bool Euler)
{
	if (Euler)
	{
		_Vector4 Quat;
		_Vector3 Rot;
		memcpy(&Quat, &m_Actor->getGlobalPose().q, sizeof(_Vector4));
		Rot = QuaternionToEuler(Quat);
		Quat = _Vector4(Rot.x, Rot.y, Rot.z, 0.f);
		return Quat;
	}
	else
	{
		_Vector4 Quat;
		memcpy(&Quat, &m_Actor->getGlobalPose().q, sizeof(_Vector4));
		return Quat;
	}
}

void CRigid::Set_ActorRotation(_bool Euler, _Vector4 Rotation)
{
	if (Euler)
	{
		PxQuat Quat;
		_Vector3 Euler = { Rotation.x, Rotation.y, Rotation.z };
		memcpy(&Quat, &EulerToQuaternion(Euler), sizeof(PxQuat));
		
		m_Actor->is<PxRigidActor>()->setGlobalPose(PxTransform(m_Actor->getGlobalPose().p, Quat));
	}
	else
	{
		PxQuat Quat;
		memcpy(&Quat, &Rotation, sizeof(PxQuat));
		
		m_Actor->is<PxRigidActor>()->setGlobalPose(PxTransform(m_Actor->getGlobalPose().p, Quat));
	}
}

void CRigid::Add_Force(const _Vector3 _Force, FORCEMODE mode, bool autowake)
{
	if (m_StaticRigid || m_Kinematic)
		return;

	PxVec3 Force;
	memcpy(&Force, &_Force, sizeof(PxVec3));
	m_Actor->is<PxRigidBody>()->addForce(Force, (PxForceMode::Enum)mode, autowake);
}

void CRigid::Add_Torque(const _Vector3 _Torque, FORCEMODE mode, bool autowake)
{
	if (m_StaticRigid || m_Kinematic)
		return;

	PxVec3 Torque;
	memcpy(&Torque, &_Torque, sizeof(PxVec3));
	m_Actor->is<PxRigidBody>()->addTorque(Torque, (PxForceMode::Enum)mode, autowake);
}

void CRigid::ClearForce(const FORCEMODE mode)
{
	if (m_StaticRigid || m_Kinematic)
		return;

	m_Actor->is<PxRigidBody>()->clearForce((PxForceMode::Enum)mode);
}

void CRigid::ClearTorque(const FORCEMODE mode)
{
	if (m_StaticRigid || m_Kinematic)
		return;

	m_Actor->is<PxRigidBody>()->clearTorque((PxForceMode::Enum)mode);
}

void CRigid::Add_LinearVelocity(const _Vector3 _Velocity)
{
	if (m_StaticRigid || m_Kinematic)
		return;

	PxVec3 Velocity;
	memcpy(&Velocity, &_Velocity, sizeof(PxVec3));
	m_Actor->is<PxRigidBody>()->setLinearVelocity(m_Actor->is<PxRigidBody>()->getLinearVelocity() + Velocity);
}

void CRigid::Set_LinearVelocity(const _Vector3 _Velocity)
{
	if (m_StaticRigid || m_Kinematic)
		return;

	PxVec3 Velocity;
	memcpy(&Velocity, &_Velocity, sizeof(PxVec3));
	m_Actor->is<PxRigidBody>()->setLinearVelocity(Velocity, false);
}

_Vector3 CRigid::Get_LinearVelocity()
{
	if (m_StaticRigid || m_Kinematic)
		return _Vector3();

	_Vector3 Velocity;
	memcpy(&Velocity, &m_Actor->is<PxRigidBody>()->getLinearVelocity(), sizeof(_Vector3));
	return Velocity;
}

_Vector3 CRigid::Get_AngularVelocity()
{
	if (m_StaticRigid || m_Kinematic)
		return _Vector3();

	return TOVEC3(m_Actor->is<PxRigidBody>()->getAngularVelocity());
}

void CRigid::Add_AngularVelocity(const _Vector3 _Velocity)
{
	if (m_StaticRigid || m_Kinematic)
		return;

	PxVec3 Velocity;
	memcpy(&Velocity, &_Velocity, sizeof(PxVec3));
	m_Actor->is<PxRigidBody>()->setAngularVelocity(m_Actor->is<PxRigidBody>()->getAngularVelocity() + Velocity);
}

void CRigid::Set_AngularVelocity(const _Vector3 _Velocity)
{
	if (m_StaticRigid || m_Kinematic)
		return;

	PxVec3 Velocity;
	memcpy(&Velocity, &_Velocity, sizeof(PxVec3));
	m_Actor->is<PxRigidBody>()->setAngularVelocity(Velocity, false);
}

void CRigid::Attach_Shape()
{
	for (auto& Pair : m_Colliders)
	{
		Pair.second->Shape_Setting();
		Pair.second->Attach_Shape(m_Actor);
	}
}

void CRigid::Detach_Shape()
{
	for (auto& Pair : m_Colliders)
	{
		Pair.second->Detach_Shape(m_Actor);
	}
}

void CRigid::Attach_Shape(PxShape* _Shape)
{
	for (auto& Pair : m_Colliders)
	{
		if (Pair.second->Get_Shape() != _Shape)
			continue;
		Pair.second->Shape_Setting();
		Pair.second->Attach_Shape(m_Actor);
	}
}

void CRigid::Attach_Shape(wstring _ShapeName)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_ShapeName));

	if (iter == m_Colliders.end())
		return;

	iter->second->Attach_Shape(m_Actor);

	return;
}

void CRigid::Detach_Shape(PxShape* _Shape)
{
	for (auto& Pair : m_Colliders)
	{
		if (Pair.second->Get_Shape() != _Shape)
			continue;
		Pair.second->Detach_Shape(m_Actor);
	}
}

void CRigid::Detach_Shape(wstring _ShapeName)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_ShapeName));

	if (iter == m_Colliders.end())
		return;

	iter->second->Detach_Shape(m_Actor);
	
	return;
}

void CRigid::Set_Transform(_Matrix matrix)
{
	PxQuat PxQ;
	PxVec3 PxV;
	Quaternion Quat = Quaternion::CreateFromRotationMatrix(Matrix::Normalize(matrix));
	_Vector3 Pos = matrix.Translation();
	memcpy(&PxQ, &Quat, sizeof(PxQuat));
	memcpy(&PxV, &Pos, sizeof(PxVec3));

	PxTransform Transform(PxV, PxQ);
	if (!Transform.isValid())
	{
		//TODO :체크7
		//MSG_BOX("매트릭스의 포지션이 무한이거나, 매트릭스의 로테이션이 무한이거나, 매트릭스가 단위회전이 아닌경우에 이런 오류가 나옵니다");
	}
	else if (m_Kinematic && !m_StaticRigid)
	{
		m_Actor->is<PxRigidDynamic>()->setKinematicTarget(Transform);
	}
	else
	{
		m_Actor->setGlobalPose(Transform);
	}
}

void CRigid::Set_KinematicTeleport(_Matrix matrix)
{
	PxQuat PxQ;
	PxVec3 PxV;
	Quaternion Quat = Quaternion::CreateFromRotationMatrix(Matrix::Normalize(matrix));
	_Vector3 Pos = matrix.Translation();
	memcpy(&PxQ, &Quat, sizeof(PxQuat));
	memcpy(&PxV, &Pos, sizeof(PxVec3));

	PxTransform Transform(PxV, PxQ);
	m_Actor->setGlobalPose(Transform);
}

void CRigid::Set_UseTransform(_bool UseTransform)
{
	m_UseTransform = UseTransform;
}

const bool& CRigid::Get_UseTransform()
{
	return m_UseTransform;
	// // O: 여기에 return 문을 삽입합니다.
}

void CRigid::Set_UpDownMove(_bool UpDownMove)
{
	m_UpDownMove = UpDownMove;
}

void CRigid::Set_UserData(void* _UserData)
{
	m_Actor->userData = _UserData;
}

PxRigidActor* CRigid::Get_Actor()
{
	return m_Actor;
}

void CRigid::Set_Host(CGameObject* _GameObject)
{
	m_HostObject = _GameObject;
}

CGameObject* CRigid::Get_Host()
{
	return m_HostObject;
}

void CRigid::SetUp_Filtering(const _uint& FilterGroup)
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	pPhysX->SetUp_Filtering(m_Actor, FilterGroup);
	RELEASE_INSTANCE(CPhysicsSystem);
}

void CRigid::Delete_FilterGroup(const _uint& FilterGroup, const _uint& FilterGroups)
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	pPhysX->Delete_FilterGroup(FilterGroup, FilterGroups);
	RELEASE_INSTANCE(CPhysicsSystem);
}

void CRigid::Add_FilterGroup(const _uint& FilterGroup, const _uint& FilterGroups)
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	pPhysX->Add_FilterGroup(FilterGroup, FilterGroups);
	RELEASE_INSTANCE(CPhysicsSystem);
}

void CRigid::Set_FilterGroups(const _uint& FilterGroup, const _uint& FilterGroups)
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	pPhysX->Set_FilterGroups(FilterGroup, FilterGroups);
	RELEASE_INSTANCE(CPhysicsSystem);
}

void CRigid::SetStabilizationThreshold(_float Stabilization_level)
{
	if (m_StaticRigid)
		return;

	m_Actor->is<PxRigidDynamic>()->setStabilizationThreshold(Stabilization_level);
}

void CRigid::Update_Flag()
{
	//플래그들 모아둔 맵 정리하기
	m_Collision_Simulation_Flag.clear();
	if (!m_Collision_Trigger_Flag.empty())
	{
		auto Pair = m_Collision_Trigger_Flag.begin();
		list<PxActor*> Backup;
		for (; Pair != m_Collision_Trigger_Flag.end();)
		{
			if (Pair->second == PxPairFlag::eNOTIFY_TOUCH_FOUND && !((CRigid*)Pair->first->userData)->Get_Host()->Get_Dead())
			{
				Backup.push_back(Pair->first);
				Pair = m_Collision_Trigger_Flag.erase(Pair);
			}
			else if (((CRigid*)Pair->first->userData)->Get_Host()->Get_Dead() || Pair->second == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				//내 페어를 지운다.
				Pair = m_Collision_Trigger_Flag.erase(Pair);
				//상대방은 지울 필요가 없다 왜냐 - 상대방은 어차피 지워지기때문.
			}
			else
				++Pair;
		}

		if (!Backup.empty())
		{
			for (auto& Actor : Backup)
			{
				m_Collision_Trigger_Flag.emplace(Actor, PxPairFlag::eNOTIFY_TOUCH_PERSISTS);
			}
			Backup.clear();
		}
	}
	//리스트 0으로 밀고 지우기
	if (!m_Collision_Simulation_Enter.empty())
	{
		m_Collision_Simulation_Enter_Index = 0;
		m_Collision_Simulation_Enter.clear();
	}
	if (!m_Collision_Simulation_Stay.empty())
	{
		m_Collision_Simulation_Stay_Index = 0;
		m_Collision_Simulation_Stay.clear();
	}
	if (!m_Collision_Simulation_Exit.empty())
	{
		m_Collision_Simulation_Exit_Index = 0;
		m_Collision_Simulation_Exit.clear();
	}
	if (!m_Collision_Trigger_Enter.empty())
	{
		m_Collision_Trigger_Enter_Index = 0;
		m_Collision_Trigger_Enter.clear();
	}
	if (!m_Collision_Trigger_Stay.empty())
	{
		m_Collision_Trigger_Stay_Index = 0;
		m_Collision_Trigger_Stay.clear();
	}
	if (!m_Collision_Trigger_Exit.empty())
	{
		m_Collision_Trigger_Exit_Index = 0;
		m_Collision_Trigger_Exit.clear();
	}

}

void CRigid::Add_Collision_Simulation(PxActor* Actor, PxPairFlag::Enum Flag)
{
	m_Collision_Simulation_Flag.emplace(Actor, Flag);
}

void CRigid::Add_Collision_Trigger(PxActor* Actor, PxPairFlag::Enum Flag)
{
	m_Collision_Trigger_Flag.emplace(Actor, Flag);
}

void CRigid::Delete_Collision_Simulation(PxActor* Actor)
{
	auto Pair = m_Collision_Simulation_Flag.find(Actor);
	if(Pair != m_Collision_Simulation_Flag.end())
		m_Collision_Simulation_Flag.erase(Pair);
}

void CRigid::Delete_Collision_Trigger(PxActor* Actor)
{
	auto Pair = m_Collision_Trigger_Flag.find(Actor);
	if (Pair != m_Collision_Trigger_Flag.end())
		m_Collision_Trigger_Flag.erase(Pair);
}

void CRigid::CCT_Collision(CGameObject* pGameObject)
{
	if(m_NeedCCTCollisionCheck)
		m_Collision_CCT.push_back(pGameObject);
}

_bool CRigid::Get_Collision_CCT(CGameObject** pGameObject)
{
	if (!m_NeedCCTCollisionCheck)
		return false;

	if (m_Collision_CCT.empty())
		return false;

	if (m_Collision_CCT_Index >= m_Collision_CCT.size())
	{
		m_Collision_CCT_Index = 0;
		return false;
	}

	*pGameObject = m_Collision_CCT[m_Collision_CCT_Index];
	++m_Collision_CCT_Index;
	return true;
}

void CRigid::Clear_Collision_CCT()
{
	m_Collision_CCT.clear();
	m_Collision_CCT_Index = 0;
}

_bool CRigid::Get_Collision_Simulation_Enter(CGameObject** _GameObject)
{
	if (m_Collision_Simulation_Flag.empty())
		return false;

	if(m_Collision_Simulation_Enter.empty())
	{
		for (auto& Pair : m_Collision_Simulation_Flag)
		{
			if (Pair.second == PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				m_Collision_Simulation_Enter.emplace_back((CGameObject*)((CRigid*)Pair.first->userData)->Get_Host());
			}
		}
	}

	if (m_Collision_Simulation_Enter_Index >= m_Collision_Simulation_Enter.size())
	{
		m_Collision_Simulation_Enter_Index = 0;
		return false;
	}

	auto iter = m_Collision_Simulation_Enter.begin();
	for (_uint i = 0; i < m_Collision_Simulation_Enter_Index; ++i)
	{
		++iter;
	}
	*_GameObject = *iter;
	++m_Collision_Simulation_Enter_Index;
	return true;
}

_bool CRigid::Get_Collision_Simulation_Stay(CGameObject** _GameObject)
{
	if (m_Collision_Simulation_Flag.empty())
		return false;

	if (m_Collision_Simulation_Stay.empty())
	{
		for (auto& Pair : m_Collision_Simulation_Flag)
		{
			if (Pair.second == PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
			{
				m_Collision_Simulation_Stay.emplace_back((CGameObject*)((CRigid*)Pair.first->userData)->Get_Host());
			}
		}
	}

	if (m_Collision_Simulation_Stay_Index >= m_Collision_Simulation_Stay.size())
	{
		m_Collision_Simulation_Stay_Index = 0;
		return false;
	}

	auto iter = m_Collision_Simulation_Stay.begin();
	for (_uint i = 0; i < m_Collision_Simulation_Stay_Index; ++i)
	{
		++iter;
	}
	*_GameObject = *iter;
	++m_Collision_Simulation_Stay_Index;
	return true;
}

_bool CRigid::Get_Collision_Simulation_Exit(CGameObject** _GameObject)
{
	if (m_Collision_Simulation_Flag.empty())
		return false;

	if (m_Collision_Simulation_Exit.empty())
	{
		for (auto& Pair : m_Collision_Simulation_Flag)
		{
			if (Pair.second == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				m_Collision_Simulation_Exit.emplace_back((CGameObject*)((CRigid*)Pair.first->userData)->Get_Host());
			}
		}
	}

	if (m_Collision_Simulation_Exit_Index >= m_Collision_Simulation_Exit.size())
	{
		m_Collision_Simulation_Exit_Index = 0;
		return false;
	}

	auto iter = m_Collision_Simulation_Exit.begin();
	for (_uint i = 0; i < m_Collision_Simulation_Exit_Index; ++i)
	{
		++iter;
	}
	*_GameObject = *iter;
	++m_Collision_Simulation_Exit_Index;
	return true;
}

_bool CRigid::Get_Collision_Trigger_Enter(CGameObject** _GameObject)
{
	if (m_Collision_Trigger_Flag.empty())
		return false;

	if (m_Collision_Trigger_Enter.empty())
	{
		for (auto& Pair : m_Collision_Trigger_Flag)
		{
			if (Pair.second == PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				m_Collision_Trigger_Enter.emplace_back((CGameObject*)((CRigid*)Pair.first->userData)->Get_Host());
			}
		}
	}

	if (m_Collision_Trigger_Enter_Index >= m_Collision_Trigger_Enter.size())
	{
		m_Collision_Trigger_Enter_Index = 0;
		return false;
	}

	auto iter = m_Collision_Trigger_Enter.begin();
	for (_uint i = 0; i < m_Collision_Trigger_Enter_Index; ++i)
	{
		++iter;
	}
	*_GameObject = *iter;
	++m_Collision_Trigger_Enter_Index;
	return true;
}

_bool CRigid::Get_Collision_Trigger_Stay(CGameObject** _GameObject)
{
	if (m_Collision_Trigger_Flag.empty())
		return false;

	if (m_Collision_Trigger_Stay.empty())
	{
		for (auto& Pair : m_Collision_Trigger_Flag)
		{
			if (Pair.second == PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
			{
				m_Collision_Trigger_Stay.emplace_back((CGameObject*)((CRigid*)Pair.first->userData)->Get_Host());
			}
		}
	}

	if (m_Collision_Trigger_Stay_Index >= m_Collision_Trigger_Stay.size())
	{
		m_Collision_Trigger_Stay_Index = 0;
		return false;
	}

	auto iter = m_Collision_Trigger_Stay.begin();
	for (_uint i = 0; i < m_Collision_Trigger_Stay_Index; ++i)
	{
		++iter;
	}
	*_GameObject = *iter;
	++m_Collision_Trigger_Stay_Index;
	return true;
}

_bool CRigid::Get_Collision_Trigger_Exit(CGameObject** _GameObject)
{
	if (m_Collision_Trigger_Flag.empty())
		return false;

	if (m_Collision_Trigger_Exit.empty())
	{
		for (auto& Pair : m_Collision_Trigger_Flag)
		{
			if (Pair.second == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				m_Collision_Trigger_Exit.emplace_back((CGameObject*)((CRigid*)Pair.first->userData)->Get_Host());
			}
		}
	}

	if (m_Collision_Trigger_Exit_Index >= m_Collision_Trigger_Exit.size())
	{
		m_Collision_Trigger_Exit_Index = 0;
		return false;
	}

	auto iter = m_Collision_Trigger_Exit.begin();
	for (_uint i = 0; i < m_Collision_Trigger_Exit_Index; ++i)
	{
		++iter;
	}
	*_GameObject = *iter;
	++m_Collision_Trigger_Exit_Index;
	return true;
}

void CRigid::SetUp_Filtering(wstring _Key, const _uint& FilterGroup)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	iter->second->SetUp_Filtering(FilterGroup);
}

void CRigid::Set_LocalPos(wstring _Key, _Vector3 Pos)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	iter->second->Set_LocalPos(Pos);
}

_Vector3 CRigid::Get_LocalPos(wstring _Key)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return _Vector3();
	return iter->second->Get_LocalPos();
}

_Vector3 CRigid::Get_ColliderGlobalPos(wstring _Key)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return _Vector3();
	return iter->second->Get_LocalPos() + Get_ActorPos();
}

void CRigid::Set_LocalEuler(wstring _Key, _Vector3 Euler)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	iter->second->Set_LocalEuler(Euler);
}

GEOMETRYTYPE* CRigid::Get_Type(wstring _Key)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return nullptr;
	return iter->second -> Get_Type();
}

_bool CRigid::Get_isTrigger(wstring _Key)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return false;
	return iter->second->Get_isTrigger();
}

void CRigid::Set_isTrigger(wstring _Key, _bool _isTrigger)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	return iter->second->Set_isTrigger(_isTrigger);
}

_bool CRigid::Get_isQuery(wstring _Key)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return false;
	return iter->second->Get_isQuery();
}

void CRigid::Set_isQuery(wstring _Key, _bool _isQuery)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	iter->second->Set_isQuery(_isQuery);
}

_float CRigid::Get_Radius(wstring _Key)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return 0.f;
	return iter->second->Get_Radius();
}

_float CRigid::Get_Height(wstring _Key)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return 0.f;
	return iter->second->Get_Height();
}

_bool CRigid::Get_isSimulation(wstring _Key)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return false;
	return iter->second->Get_isSimulation();
}

void CRigid::Set_isSimulation(wstring _Key, _bool _isSimulation)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	return iter->second->Set_isSimulation(_isSimulation);
}

void CRigid::Set_ToBoxGeometry(wstring _Key, _Vector3 Extents, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	return iter->second->Set_ToBoxGeometry(Extents, LocalPos, LocalEuler);
}

void CRigid::Set_ToShpereGeometry(wstring _Key, float Radian, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	return iter->second->Set_ToShpereGeometry(Radian, LocalPos, LocalEuler);
}

void CRigid::Set_ToCapsuleGeometry(wstring _Key, float Radian, float Height, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	return iter->second->Set_ToCapsuleGeometry(Radian, Height, LocalPos, LocalEuler);
}

void CRigid::Set_Material(wstring _Key, _float StaticFrictionm, _float DynamicFriction, _float Restitytion)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	return iter->second->Set_Material(StaticFrictionm, DynamicFriction, Restitytion);
}

_Vector3 CRigid::Get_Material(wstring _Key)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return _Vector3();
	return iter->second->Get_Material();
}

_Vector3 CRigid::Get_Extents(wstring _Key)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return _Vector3();
	return iter->second->Get_Extents();
}

_uint CRigid::Get_Filter(wstring _Key, _uint word)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return 99999;
	return iter->second->Get_Filter(word);
}

void CRigid::Set_Filter(wstring _Key, _uint word, _uint Filter)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	return iter->second->Set_Filter(word, Filter);
}

void CRigid::Set_Geometry(wstring _Key, _float Radius)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	return iter->second->Set_Geometry(Radius);
}

void CRigid::Set_Geometry(wstring _Key, _float Radius, _float HalfHeight)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	return iter->second->Set_Geometry(Radius, HalfHeight);
}

void CRigid::Set_Geometry(wstring _Key, _Vector3 Extents)
{
	auto	iter = find_if(m_Colliders.begin(), m_Colliders.end(), CTagStringFinder(_Key));

	if (iter == m_Colliders.end())
		return;
	return iter->second->Set_Geometry(Extents);
}

void CRigid::Clear_Collider()
{
	Detach_Shape();

	for(auto& p : m_Colliders)
	{
		Safe_Release(p.second);
	}
	m_Colliders.clear();
}

_uint CRigid::Get_ColliderSize()
{
	if (m_Colliders.empty())
		return 0;

	return (_uint)m_Colliders.size();
}

void CRigid::Create_DistanceJoint(PxRigidActor* Actor0, _Vector3 JointLocalPos0, PxRigidActor* Actor1, _Vector3 JointLocalPos1, _float MaxDist, _bool Spring)
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	m_Joint = pPhysX->Create_DistanceJoint(Actor0, JointLocalPos0, Actor1, JointLocalPos1, MaxDist, Spring);
	m_JointFlag = JOINT::JOINT_DISTANCE;
	RELEASE_INSTANCE(CPhysicsSystem);
}

void CRigid::Create_FixedJoint(PxRigidActor* Actor0, _Vector3 JointLocalPos0, PxRigidActor* Actor1, _Vector3 JointLocalPos1)
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	m_Joint = pPhysX->Create_FixedJoint(Actor0, JointLocalPos0, Actor1, JointLocalPos1);
	m_JointFlag = JOINT::JOINT_FIXED;
	RELEASE_INSTANCE(CPhysicsSystem);
}

void CRigid::Create_PrismaticJoint(PxRigidActor* Actor0, _Vector3 JointLocalPos0, PxRigidActor* Actor1, _Vector3 JointLocalPos1, _Vector3 Axis, _float MinLimit, _float MaxLimit)
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	m_Joint = pPhysX->Create_PrismaticJoint(Actor0, JointLocalPos0, Actor1, JointLocalPos1, Axis, MinLimit, MaxLimit);
	m_JointFlag = JOINT::JOINT_PRISMATIC;
	RELEASE_INSTANCE(CPhysicsSystem);
}

void CRigid::Create_RevoluteJoint(PxRigidActor* Actor0, _Vector3 JointLocalPos0, PxRigidActor* Actor1, _Vector3 JointLocalPos1, _Vector3 Axis, _float MinLimit, _float MaxLimit)
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	m_Joint = pPhysX->Create_RevoluteJoint(Actor0, JointLocalPos0, Actor1, JointLocalPos1, Axis, MinLimit, MaxLimit);
	m_JointFlag = JOINT::JOINT_REVOLVE;
	RELEASE_INSTANCE(CPhysicsSystem);
}

void CRigid::Create_SphericalJoint(PxRigidActor* Actor0, _Vector3 JointLocalPos0, PxRigidActor* Actor1, _Vector3 JointLocalPos1, _Vector3 Axis, _float Limix0, _float Limix1)
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	m_Joint = pPhysX->Create_SphericalJoint(Actor0, JointLocalPos0, Actor1, JointLocalPos1, Axis, Limix0, Limix1);
	m_JointFlag = JOINT::JOINT_SPHERICAL;
	RELEASE_INSTANCE(CPhysicsSystem);
}

void CRigid::ZeroIndex()
{
	m_Collision_Simulation_Enter_Index = 0;
	m_Collision_Simulation_Stay_Index = 0;
	m_Collision_Simulation_Exit_Index = 0;
	m_Collision_Trigger_Enter_Index = 0;
	m_Collision_Trigger_Stay_Index = 0;
	m_Collision_Trigger_Exit_Index = 0;
}

HRESULT CRigid::Attach_Rigid()
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	pPhysX->Attach_Rigid(this);
	RELEASE_INSTANCE(CPhysicsSystem);
	return S_OK;
}

HRESULT CRigid::Detach_Rigid()
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	pPhysX->Detach_Rigid(this);
	RELEASE_INSTANCE(CPhysicsSystem);
	return S_OK;
}

void CRigid::Clear_Flag()
{
	m_Collision_CCT.clear();
	m_Collision_Simulation_Flag.clear();
	m_Collision_Trigger_Flag.clear();
	m_Collision_Simulation_Enter.clear();
	m_Collision_Simulation_Stay.clear();
	m_Collision_Simulation_Exit.clear();
	m_Collision_Trigger_Enter.clear();
	m_Collision_Trigger_Stay.clear();
	m_Collision_Trigger_Exit.clear();
	m_Collision_CCT_Index = 0;
	m_Collision_Simulation_Enter_Index = 0;
	m_Collision_Simulation_Stay_Index = 0;
	m_Collision_Simulation_Exit_Index = 0;
	m_Collision_Trigger_Enter_Index = 0;
	m_Collision_Trigger_Stay_Index = 0;
	m_Collision_Trigger_Exit_Index = 0;
}

bool CRigid::Fixed_Tick()
{
	//죽었으면 씬에서뺴고 리스트에서빼고 순회 ++ 해주고 끝 얘는 씬에서 다시 검사 안하니까 상관X 다른애한테 들어갈일 없을듯?
	if (!m_HostObject)
		return false;
	if (m_HostObject->Get_Dead())
		return false;

	Update_Flag();

	CTransform* Transform = m_HostObject->Get_Transform();

	//Acc 클리어함
	Transform->Clear_Acc();
	_Vector3 Position = Transform->Get_State(STATE_POSITION);
	/////////////////////Test///////////////////////////////////
// 	_Vector3 Position = Transform->Get_State(STATE_POSITION);
// 	if (Position.y < -20.f)
// 	{
// 		Position.y = 0.f;
// 		Transform->Set_State(STATE_POSITION, _Vector4(Position, 1.f));
// 		Set_LinearVelocity(_Vector3(0.f, 0.f, 0.f));
// 	}
	/////////////////////////////////////////////////////////////

	if (!m_UseTransform)
		return true;

	Quaternion Quat1 = Get_ActorRotation(false);
	Quaternion Quat2 = Transform->Get_Quaternion();
	//쿼터니언끼리 비교
	Quat1 -= Quat2;
	_Vector3 Moves = Get_ActorPos() - Position;

	//회전했거나, 움직였다면
	if (Quat1.Length() >= 0.0001f || Moves.Length() >= 0.0001f)//총이동량이 0.003보다 작으면 이 될것.
	{
		PxQuat Quat;
		PxVec3 Vec;
		memcpy(&Quat, &Quat2, sizeof(PxQuat));
		memcpy(&Vec, &Position, sizeof(PxVec3));
		
		if (m_Kinematic)
		{
			m_Actor->is<PxRigidDynamic>()->setKinematicTarget(PxTransform(Vec, Quat));
		}
		else
			m_Actor->setGlobalPose(PxTransform(Vec, Quat));
	}

	return true;
}

void CRigid::Set_World()
{
	CTransform* Transform = m_HostObject->Get_Transform();
	Transform->Set_WorldMatrix(m_Actor);
}

CRigid* CRigid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRigid* p = new CRigid(pDevice, pDeviceContext);

	if (FAILED(p->NativeConstruct_Prototype()))
	{
		Safe_Release(p);
		return nullptr;
	}

	return p;
}

CComponent* CRigid::Clone(void* pArg)
{
	CComponent* p = new CRigid(*this);

	if (FAILED(p->NativeConstruct(pArg)))
	{
		Safe_Release(p);
		return nullptr;
	}

	return p;
}

void CRigid::Free()
{
	Clear_Flag();
	Detach_Rigid();
	Detach_Shape();
	for (auto& pCollider : m_Colliders)
	{
		Safe_Release(pCollider.second);
	}
	m_Colliders.clear();
	//m_Actor->release();
	//m_Joint->release();
	__super::Free();
}

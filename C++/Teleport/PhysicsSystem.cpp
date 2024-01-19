#include "..\public\PhysicsSystem.h"
#include "GameObject.h"
#include "PhysicsLayer.h"
#include "RigidBody.h"
#include "Collider.h"
IMPLEMENT_SINGLETON(CPhysicsSystem)

PxFilterFlags FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

CPhysicsSystem::CPhysicsSystem()
{
	m_HitFlag = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eFACE_INDEX | PxHitFlag::eMESH_BOTH_SIDES /*| PxHitFlag::eUV*/;
	m_TestFilterData = PxQueryFilterData(PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eANY_HIT);
	m_QueryFilterData = PxQueryFilterData(PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER);
}

HRESULT CPhysicsSystem::NativeConstruct()
{
	m_Allocator = new PhysXAllocator();
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *m_Allocator, m_ErrorCallBack);

	PxTolerancesScale Scale;
#ifdef _DEBUG
	m_PVD = PxCreatePvd(*m_Foundation);
	m_Transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
#endif
	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, Scale, true, m_PVD);

	m_CpuDispatcher = PxDefaultCpuDispatcherCreate(0);
	m_DefaultMaterial = m_Physics->createMaterial(0.5f, 0.5f, 0.f);
	PxInitExtensions(*m_Physics, m_PVD);

	PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
// 	sceneDesc.flags = PxSceneFlag::eENABLE_CCD;
// 	sceneDesc.filterShader = FilterShaderCCD;
	sceneDesc.cpuDispatcher = m_CpuDispatcher;
	//이게 구현이 맞나
	sceneDesc.filterShader = FilterShader;
	//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.staticKineFilteringMode = PxPairFilteringMode::eKEEP;
	sceneDesc.kineKineFilteringMode = PxPairFilteringMode::eKEEP;
	m_SimulationCallback = new SimulCallBack();
	sceneDesc.simulationEventCallback = m_SimulationCallback;
	/*sceneDesc.wakeCounterResetValue = 1.f;*/
	//쓸모없는거네 디폴트로돼있대.. 광역위상알고리즘
	//sceneDesc.broadPhaseType = PxBroadPhaseType::eABP;

	m_Scene = m_Physics->createScene(sceneDesc);
#ifdef _DEBUG
	if (m_PVD->connect(*m_Transport, PxPvdInstrumentationFlag::eALL))
	{
		PxPvdSceneClient* pvdClient = m_Scene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
	}

	m_Scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.f);
	m_Scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.f);
	//m_Scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AABBS, 1.f);
	m_Scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.f);
	m_Scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_EDGES, 1.f);
	m_Scene->setVisualizationParameter(PxVisualizationParameter::eBODY_AXES, 1.f);
#endif // _DEBUG


	//meshPreprocessParams 플래그를 둘중하나로 설정가능한데 둘다써보고 속도비교하기 eDISABLE_ACTIVE_EDGES_PRECOMPUTE이게더나은듯
	PxCookingParams Param(Scale);
	Param.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE ;
	Param.midphaseDesc = PxMeshMidPhase::eBVH34;
	Param.gaussMapLimit = 255;

	m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, Param);

	m_FilterLayer = CPhysicsLayer::Create();


	m_PxControllerManager = PxCreateControllerManager(*m_Scene);
	return S_OK;
}

void CPhysicsSystem::Tick(float fFixedtime)
{

 	Fixed_Tick();
	
#pragma region 피직스 내부 시뮬레이션 함수
	m_Scene->simulate(fFixedtime);
	m_Scene->fetchResults(true);
#pragma endregion

	Apply_Tick();

}

PxMaterial* CPhysicsSystem::Create_Material(_Vector3 _Material)
{
	return m_Physics->createMaterial(XMVectorGetX(_Material), XMVectorGetY(_Material), XMVectorGetZ(_Material));;
}

PxRigidStatic* CPhysicsSystem::Create_StaticActor(_fmatrix _Matrix)
{
	PxMat44 matrix;
	memcpy(&matrix, &_Matrix, sizeof(PxMat44));
	return m_Physics->createRigidStatic(PxTransform(matrix));
}

PxRigidDynamic* CPhysicsSystem::Create_DynamicActor(_fmatrix _Matrix)
{
	PxMat44 matrix;
	memcpy(&matrix, &_Matrix, sizeof(PxMat44));
	return m_Physics->createRigidDynamic(PxTransform(matrix));
}

PxJoint* CPhysicsSystem::Create_DistanceJoint(PxRigidActor* Actor0, _Vector3 JointLocalPos0, PxRigidActor* Actor1, _Vector3 JointLocalPos1, _float MaxDist, _bool Spring)
{
	PxVec3 Pos;
	memcpy(&Pos, &JointLocalPos0, sizeof(PxVec3));
	const PxTransform jointFrame0(Pos);
	memcpy(&Pos, &JointLocalPos1, sizeof(PxVec3));
	const PxTransform jointFrame1(Pos);

// 	PxD6Joint* j = PxD6JointCreate(*m_Physics, Actor0, jointFrame0, Actor1, jointFrame1);
// 	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
// 	j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
// 	j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
// 	j->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);
// 	j->setMotion(PxD6Axis::eY, PxD6Motion::eLIMITED);
// 	j->setMotion(PxD6Axis::eZ, PxD6Motion::eLIMITED);
// 	j->setDistanceLimit(PxJointLinearLimit(PxTolerancesScale(), MaxDist));

	PxDistanceJoint* j = PxDistanceJointCreate(*m_Physics, Actor0, jointFrame0, Actor1, jointFrame1);
	j->setDistanceJointFlag(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
	j->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, Spring);
	j->setMaxDistance(MaxDist);
	return (PxJoint*)j;
}

PxJoint* CPhysicsSystem::Create_FixedJoint(PxRigidActor* Actor0, _Vector3 JointLocalPos0, PxRigidActor* Actor1, _Vector3 JointLocalPos1)
{
	PxVec3 Pos;
	memcpy(&Pos, &JointLocalPos0, sizeof(PxVec3));
	const PxTransform jointFrame0(Pos);
	memcpy(&Pos, &JointLocalPos1, sizeof(PxVec3));
	const PxTransform jointFrame1(Pos);

	return (PxJoint*)PxFixedJointCreate(*m_Physics, Actor0, jointFrame0, Actor1, jointFrame1);
}

PxJoint* CPhysicsSystem::Create_PrismaticJoint(PxRigidActor* Actor0, _Vector3 JointLocalPos0, PxRigidActor* Actor1, _Vector3 JointLocalPos1, _Vector3 Axis, _float MinLimit, _float MaxLimit)
{
	PxVec3 Pos;
	PxQuat Quat;

	memcpy(&Quat, &EulerToQuaternion(Axis), sizeof(PxQuat));
	memcpy(&Pos, &JointLocalPos0, sizeof(PxVec3));
	const PxTransform jointFrame0(Pos, Quat);
	memcpy(&Pos, &JointLocalPos1, sizeof(PxVec3));
	const PxTransform jointFrame1(Pos, Quat);

// 	PxD6Joint* j = PxD6JointCreate(*m_Physics, Actor0, jointFrame0, Actor1, jointFrame1);
// 	j->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
// 	if (MinLimit == MaxLimit)
// 		j->setMotion(PxD6Axis::eX, PxD6Motion::eLOCKED);
// 	else if (MinLimit > MaxLimit)
// 		j->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
// 	else// if(minLimit<maxLimit)
// 	{
// 		const PxJointLinearLimitPair limit(PxTolerancesScale(), MinLimit, MaxLimit);
// 		j->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);
// 		j->setLinearLimit(PxD6Axis::eX, limit);
// 	}
	PxPrismaticJoint* j = PxPrismaticJointCreate(*m_Physics, Actor0, jointFrame0, Actor1, jointFrame1);
	const PxJointLinearLimitPair limit(PxTolerancesScale(), MinLimit, MaxLimit);
	if (MinLimit < MaxLimit)
	{
		j->setLimit(limit);
		j->setPrismaticJointFlag(PxPrismaticJointFlag::eLIMIT_ENABLED, true);
	}
	return (PxJoint*)j;
}

PxJoint* CPhysicsSystem::Create_RevoluteJoint(PxRigidActor* Actor0, _Vector3 JointLocalPos0, PxRigidActor* Actor1, _Vector3 JointLocalPos1, _Vector3 Axis, _float MinLimit, _float MaxLimit)
{
	PxVec3 Pos;
	PxQuat Quat;

	memcpy(&Quat, &EulerToQuaternion(Axis), sizeof(PxQuat));
	memcpy(&Pos, &JointLocalPos0, sizeof(PxVec3));
	const PxTransform jointFrame0(Pos, Quat);
	memcpy(&Pos, &JointLocalPos1, sizeof(PxVec3));
	const PxTransform jointFrame1(Pos, Quat);


// 	PxD6Joint* j = PxD6JointCreate(*m_Physics, Actor0, jointFrame0, Actor1, jointFrame1);
// 	if (MinLimit == MaxLimit)
// 		j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLOCKED);
// 	else if (MinLimit > MaxLimit)
// 		j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
// 	else// if(minLimit<maxLimit)
// 	{
// 		const PxJointAngularLimitPair limit(XMConvertToRadians(MinLimit), XMConvertToRadians(MaxLimit));
// 		j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
// 		j->setTwistLimit(limit);
// 	}
	PxRevoluteJoint* j = PxRevoluteJointCreate(*m_Physics, Actor0, jointFrame0, Actor1, jointFrame1);
	const PxJointAngularLimitPair limit(XMConvertToRadians(MinLimit), XMConvertToRadians(MaxLimit));
	if (MinLimit < MaxLimit)
	{
		j->setLimit(limit);
		j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
	}
	return (PxJoint*)j;
	
}

PxJoint* CPhysicsSystem::Create_SphericalJoint(PxRigidActor* Actor0, _Vector3 JointLocalPos0, PxRigidActor* Actor1, _Vector3 JointLocalPos1, _Vector3 Axis, _float Limix0, _float Limix1)
{
	PxVec3 Pos;
	PxQuat Quat;

	memcpy(&Quat, &EulerToQuaternion(Axis), sizeof(PxQuat));
	memcpy(&Pos, &JointLocalPos0, sizeof(PxVec3));
	const PxTransform jointFrame0(Pos, Quat);
	memcpy(&Pos, &JointLocalPos1, sizeof(PxVec3));
	const PxTransform jointFrame1(Pos, Quat);

	const PxJointLimitCone limit(XMConvertToRadians(Limix0), XMConvertToRadians(Limix1));

// 	PxD6Joint* j = PxD6JointCreate(*m_Physics, Actor0, jointFrame0, Actor1, jointFrame1);
// 	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
// 	if (Limix0 > 0.0f && Limix1 > 0.0f)
// 	{
// 		j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
// 		j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);
// 		j->setSwingLimit(limit);
// 	}
// 	else
// 	{
// 		j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
// 		j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
// 	}

	PxSphericalJoint* j = PxSphericalJointCreate(*m_Physics, Actor0, jointFrame0, Actor1, jointFrame1);
	if (Limix0 > 0.0f && Limix1 > 0.0f)
	{
		j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
		j->setLimitCone(limit);
	}

	return (PxJoint*)j;
}

void CPhysicsSystem::Add_Actor(PxActor* _Actor)
{
	if (nullptr != _Actor)
		m_Scene->addActor(*_Actor);
}

void CPhysicsSystem::Sub_Actor(PxActor* _Actor)
{
	if(nullptr != _Actor)
		m_Scene->removeActor(*_Actor);
}

PxShape* CPhysicsSystem::Create_Shape(PxBoxGeometry _Geometry, const PxMaterial& _Material, _bool _isExclusive, PxShapeFlags _Flag)
{
	PxMaterial* materialPtr = const_cast<PxMaterial*>(&_Material);
	return m_Physics->createShape(_Geometry, &materialPtr, 1, _isExclusive, _Flag);
}

PxShape* CPhysicsSystem::Create_Shape(PxCapsuleGeometry _Geometry, const PxMaterial& _Material, _bool _isExclusive, PxShapeFlags _Flag)
{
	PxMaterial* materialPtr = const_cast<PxMaterial*>(&_Material);
	return m_Physics->createShape(_Geometry, &materialPtr, 1, _isExclusive, _Flag);
}

PxShape* CPhysicsSystem::Create_Shape(PxSphereGeometry _Geometry, const PxMaterial& _Material, _bool _isExclusive, PxShapeFlags _Flag)
{
	PxMaterial* materialPtr = const_cast<PxMaterial*>(&_Material);
	return m_Physics->createShape(_Geometry, &materialPtr, 1, _isExclusive, _Flag);
}

PxShape* CPhysicsSystem::Create_Shape(PxTriangleMeshGeometry _Geometry, const PxMaterial& _Material, _bool _isExclusive, PxShapeFlags _Flag)
{
	PxMaterial* materialPtr = const_cast<PxMaterial*>(&_Material);
	return m_Physics->createShape(_Geometry, &materialPtr, 1, _isExclusive, _Flag);
}

PxShape* CPhysicsSystem::Create_Shape(PxConvexMeshGeometry _Geometry, const PxMaterial& _Material, _bool _isExclusive, PxShapeFlags _Flag)
{
	PxMaterial* materialPtr = const_cast<PxMaterial*>(&_Material);
	return m_Physics->createShape(_Geometry, &materialPtr, 1, _isExclusive, _Flag);
}

PxTriangleMesh* CPhysicsSystem::Create_TriangleMesh(PxDefaultMemoryInputData _ReadBuffer)
{
	return m_Physics->createTriangleMesh(_ReadBuffer);
}

PxConvexMesh* CPhysicsSystem::Create_ConvexMesh(PxDefaultMemoryInputData _ReadBuffer)
{
	return m_Physics->createConvexMesh(_ReadBuffer);
}


void CPhysicsSystem::SetUp_Filtering(PxActor* _Actor, const _uint& FilterGroup)
{
	PxFilterData filterData;
	filterData.word0 = FilterGroup;
	filterData.word1 = m_FilterLayer->Get_FilterGroups(FilterGroup);

	const PxU32 numShapes = _Actor->is<PxRigidActor>()->getNbShapes();
	//PxShape** shapes = (PxShape**)SAMPLE_ALLOC(sizeof(PxShape*) * numShapes);
	PxShape** shapes = (PxShape**)m_Allocator->allocate(sizeof(PxShape*) * numShapes, 0, __FILE__, __LINE__);
	_Actor->is<PxRigidActor>()->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
		shape->setQueryFilterData(filterData);
	}
	if (shapes)
	{
		m_Allocator->deallocate(shapes);
		shapes = nullptr;
	}
}

void CPhysicsSystem::SetUp_Filtering(PxShape* _Shape, const _uint& FilterGroup)
{
	PxFilterData filterData;
	filterData.word0 = FilterGroup;
	filterData.word1 = m_FilterLayer->Get_FilterGroups(FilterGroup);

	_Shape->setSimulationFilterData(filterData);
	_Shape->setQueryFilterData(filterData);
}

void CPhysicsSystem::Create_FilterLayer(const _uint& FilterCount)
{
	m_FilterLayer->Create_FilterLayer(FilterCount);
}

void CPhysicsSystem::Delete_FilterLayer()
{
	m_FilterLayer->Delete_FilterLayer();
}

void CPhysicsSystem::Delete_FilterGroup(const _uint& FilterGroup, const _uint& FilterGroups)
{
	m_FilterLayer->Delete_FilterGroup(FilterGroup, FilterGroups);
}

void CPhysicsSystem::Add_FilterGroup(const _uint& FilterGroup, const _uint& FilterGroups)
{
	m_FilterLayer->Add_FilterGroup(FilterGroup, FilterGroups);
}

void CPhysicsSystem::Set_FilterGroups(const _uint& FilterGroup, const _uint& FilterGroups)
{
	m_FilterLayer->Set_FilterGroups(FilterGroup, FilterGroups);
}

void CPhysicsSystem::Add_QueryFilterCallback(PxQueryFilterCallback* Callback)
{
	m_QueryFilterCallBack.emplace_back(Callback);
}

bool CPhysicsSystem::Raycast(_Vector3 Pos, _Vector3 Dir, _float Dis, RAYCASTBUFFER& Buffer, _uint CustomCallbackIndex)
{
	PxRaycastBuffer rayHit;
	_bool	Hit;

	if (CustomCallbackIndex != 9999)
	{
		PxQueryFilterData filterData(PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER);
		Hit = m_Scene->raycast(TOPXVEC3(Pos), TOPXVEC3(Dir), Dis, rayHit, ((PxHitFlags)(PxHitFlag::eDEFAULT)), filterData, m_QueryFilterCallBack[CustomCallbackIndex]);
	}
	else
		Hit = m_Scene->raycast(TOPXVEC3(Pos), TOPXVEC3(Dir), Dis, rayHit);

	if (Hit)
	{
		Buffer.actor = rayHit.block.actor;
		Buffer.Shape = rayHit.block.shape;
		Buffer.Distance = rayHit.block.distance;
		Buffer.FacsIndex = rayHit.block.faceIndex;
		memcpy(&Buffer.Normal, &rayHit.block.normal, sizeof(_Vector3));
		memcpy(&Buffer.Position, &rayHit.block.position, sizeof(_Vector3));
		Buffer.U = rayHit.block.u;
		Buffer.V = rayHit.block.v;
	}
	return Hit;
}

bool CPhysicsSystem::RaycastTest(_Vector3 Pos, _Vector3 Dir, _float Dis, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = CollisionFilter;
	desc.Type = CollisionType;
	desc.QueryOnce = true;
	desc.IgnoreRigidbody = IgnoreRigid;
	PhysicsQueryFilterCallback callback(desc);
	PxRaycastBuffer buffer;
	
	return m_Scene->raycast(TOPXVEC3(Pos), TOPXVEC3(Dir), Dis, buffer, m_HitFlag, m_TestFilterData, &callback);
}

bool CPhysicsSystem::Raycast(RAYCASTBUFFER& Buffer, _Vector3 Pos, _Vector3 Dir, _float Dis, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = CollisionFilter;
	desc.Type = CollisionType;
	desc.QueryOnce = true;
	desc.IgnoreRigidbody = IgnoreRigid;
	PhysicsQueryFilterCallback callback(desc);
	PxRaycastBuffer buffer;

	_bool Hit = m_Scene->raycast(TOPXVEC3(Pos), TOPXVEC3(Dir), Dis, buffer, m_HitFlag, m_QueryFilterData, &callback);
	if (!Hit)//충돌X
		return false;

	if (Hit)
	{
		Buffer.actor = buffer.block.actor;
		Buffer.Shape = buffer.block.shape;
		Buffer.Distance = buffer.block.distance;
		Buffer.FacsIndex = buffer.block.faceIndex;
		memcpy(&Buffer.Normal, &buffer.block.normal, sizeof(_Vector3));
		memcpy(&Buffer.Position, &buffer.block.position, sizeof(_Vector3));
		Buffer.U = buffer.block.u;
		Buffer.V = buffer.block.v;
	}
	return true;

}

vector<CPhysicsSystem::RAYCASTBUFFER> CPhysicsSystem::RaycastAll(_Vector3 Pos, _Vector3 Dir, _float Dis, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = CollisionFilter;
	desc.Type = CollisionType;
	desc.QueryOnce = false;
	desc.IgnoreRigidbody = IgnoreRigid;
	PhysicsQueryFilterCallback callback(desc);
	PxRaycastHit datas[128];
	PxRaycastBuffer buffer(datas, 128);

	m_Scene->raycast(TOPXVEC3(Pos), TOPXVEC3(Dir), Dis, buffer, m_HitFlag, m_QueryFilterData, &callback);
	
	_uint numHits = buffer.getNbAnyHits();
	vector<CPhysicsSystem::RAYCASTBUFFER> hits;
	hits.resize(numHits);

	for (_uint i = 0; i < numHits; ++i)
	{
		hits[i].actor = datas[i].actor;
		hits[i].Shape = datas[i].shape;
		hits[i].Distance = datas[i].distance;
		hits[i].FacsIndex = datas[i].faceIndex;
		memcpy(&hits[i].Normal, &datas[i].normal, sizeof(_Vector3));
		memcpy(&hits[i].Position, &datas[i].position, sizeof(_Vector3));
		hits[i].U = datas[i].u;
		hits[i].V = datas[i].v;
	}

	return hits;
}

bool CPhysicsSystem::OverlapGeometryTest(const PxGeometryHolder& g, _Vector3 Pos, const Quaternion& rotation, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = CollisionFilter;
	desc.Type = CollisionType;
	desc.QueryOnce = true;
	desc.IgnoreRigidbody = IgnoreRigid;
	PhysicsQueryFilterCallback callback(desc);
	PxOverlapHit data;

	Quaternion Rotation;
	if (!g.capsule().isValid())
		Rotation = rotation;
	else
		Rotation = EulerToQuaternion(0.f, 0.f, 90.f) * rotation;

	PxTransform t(TOPXVEC3(Pos), TOPXQUAT(Rotation));

	return PxSceneQueryExt::overlapAny(*m_Scene, g.any(), t, data, m_TestFilterData, &callback);
}

CRigid* CPhysicsSystem::OverlapGeometry(const PxGeometryHolder& g, _Vector3 Pos, const Quaternion& rotation, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = CollisionFilter;
	desc.Type = CollisionType;
	desc.QueryOnce = true;
	desc.IgnoreRigidbody = IgnoreRigid;
	PhysicsQueryFilterCallback callback(desc);
	PxOverlapHit data;

	Quaternion Rotation;
	if (!g.capsule().isValid())
		Rotation = rotation;
	else
		Rotation = EulerToQuaternion(0.f, 0.f, 90.f) * rotation;

	PxTransform t(TOPXVEC3(Pos), TOPXQUAT(Rotation));

	bool result = PxSceneQueryExt::overlapAny(*m_Scene, g.any(), t, data, m_QueryFilterData, &callback);
	if (!result) return nullptr;

	return (CRigid*)data.actor->userData;
}

vector<class CRigid*> CPhysicsSystem::OverlapGeometryAll(const PxGeometryHolder& g, _Vector3 Pos, const Quaternion& rotation, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = CollisionFilter;
	desc.Type = CollisionType;
	desc.QueryOnce = false;
	desc.IgnoreRigidbody = IgnoreRigid;
	PhysicsQueryFilterCallback callback(desc);
	PxOverlapHit datas[128];
	PxOverlapBuffer buffer(datas, 128);

	Quaternion Rotation;
	if (!g.capsule().isValid())
		Rotation = rotation;
	else
		Rotation = EulerToQuaternion(0.f, 0.f, 90.f) * rotation;

	PxTransform t(TOPXVEC3(Pos), TOPXQUAT(Rotation));

	m_Scene->overlap(g.any(), t, buffer, m_QueryFilterData, &callback);
	
	_uint numHits = buffer.getNbAnyHits();
	vector<CRigid*> hits;
	hits.resize(numHits);

	for (_uint i = 0; i < numHits; ++i)
	{
		hits[i] = (CRigid*)datas[i].actor->userData;
	}


	return hits;
}

bool CPhysicsSystem::OverlapTest(_Vector3 Pos, Quaternion Quat, CCollider* c, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxShape* shape = c->Get_Shape();
	return OverlapGeometryTest(shape->getGeometry(), Pos + c->Get_LocalPos(), Quat + c->Get_LocalQuat(), CollisionFilter, CollisionType, IgnoreRigid);
}

CRigid* CPhysicsSystem::Overlap(_Vector3 Pos, Quaternion Quat, CCollider* c, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxShape* shape = c->Get_Shape();
	return OverlapGeometry(shape->getGeometry(), Pos + c->Get_LocalPos(), Quat + c->Get_LocalQuat(), CollisionFilter, CollisionType, IgnoreRigid);
}

vector<class CRigid*> CPhysicsSystem::OverlapAll(_Vector3 Pos, Quaternion Quat, CCollider* c, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxShape* shape = c->Get_Shape();
	return OverlapGeometryAll(shape->getGeometry(), Pos + c->Get_LocalPos(), Quat + c->Get_LocalQuat(), CollisionFilter, CollisionType, IgnoreRigid);
}

bool CPhysicsSystem::OverlapTest(_Vector3 Pos, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxSphereGeometry g(0.001f);
	return OverlapGeometryTest(g, Pos, Quaternion(), CollisionFilter, CollisionType, IgnoreRigid);
}

CRigid* CPhysicsSystem::Overlap(_Vector3 Pos, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxSphereGeometry g(0.001f);
	return OverlapGeometry(g, Pos, Quaternion(), CollisionFilter, CollisionType, IgnoreRigid);
}

vector<class CRigid*> CPhysicsSystem::OverlapAll(_Vector3 Pos, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxSphereGeometry g(0.001f);
	return OverlapGeometryAll(g, Pos, Quaternion(), CollisionFilter, CollisionType, IgnoreRigid);
}

bool CPhysicsSystem::OverlapTest(_Vector3 Pos, _Vector3 Rotation, _Vector3 Extents, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxBoxGeometry g(TOPXVEC3(Extents));
	return OverlapGeometryTest(g, Pos, EulerToQuaternion(Rotation), CollisionFilter, CollisionType, IgnoreRigid);
}

CRigid* CPhysicsSystem::Overlap(_Vector3 Pos, _Vector3 Rotation, _Vector3 Extents, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxBoxGeometry g(TOPXVEC3(Extents));
	return OverlapGeometry(g, Pos, EulerToQuaternion(Rotation), CollisionFilter, CollisionType, IgnoreRigid);
}

vector<class CRigid*> CPhysicsSystem::OverlapAll(_Vector3 Pos, _Vector3 Rotation, _Vector3 Extents, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxBoxGeometry g(TOPXVEC3(Extents));
	return OverlapGeometryAll(g, Pos, EulerToQuaternion(Rotation), CollisionFilter, CollisionType, IgnoreRigid);
}

bool CPhysicsSystem::OverlapTest(_Vector3 Pos, _float Radius, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxSphereGeometry g(Radius);
	return OverlapGeometryTest(g, Pos, Quaternion(), CollisionFilter, CollisionType, IgnoreRigid);
}

CRigid* CPhysicsSystem::Overlap(_Vector3 Pos, _float Radius, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxSphereGeometry g(Radius);
	return OverlapGeometry(g, Pos, Quaternion(), CollisionFilter, CollisionType, IgnoreRigid);
}

vector<class CRigid*> CPhysicsSystem::OverlapAll(_Vector3 Pos, _float Radius, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxSphereGeometry g(Radius);
	return OverlapGeometryAll(g, Pos, Quaternion(), CollisionFilter, CollisionType, IgnoreRigid);
}

bool CPhysicsSystem::OverlapTest(_Vector3 Pos, _Vector3 Rotation, _float Radius, _float HalfHeight, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxCapsuleGeometry g(Radius, HalfHeight);
	return OverlapGeometryTest(g, Pos, EulerToQuaternion(Rotation), CollisionFilter, CollisionType, IgnoreRigid);
}

CRigid* CPhysicsSystem::Overlap(_Vector3 Pos, _Vector3 Rotation, _float Radius, _float HalfHeight, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxCapsuleGeometry g(Radius, HalfHeight);
	return OverlapGeometry(g, Pos, EulerToQuaternion(Rotation), CollisionFilter, CollisionType, IgnoreRigid);
}

vector<class CRigid*> CPhysicsSystem::OverlapAll(_Vector3 Pos, _Vector3 Rotation, _float Radius, _float HalfHeight, _uint CollisionFilter, QUERYTYPE CollisionType, CRigid* IgnoreRigid)
{
	PxCapsuleGeometry g(Radius, HalfHeight);
	return OverlapGeometryAll(g, Pos, EulerToQuaternion(Rotation), CollisionFilter, CollisionType, IgnoreRigid);
}

//얘도 픽스드랑 어플라이에서만 안부르면됨.
HRESULT CPhysicsSystem::Attach_Rigid(CRigid* Rigid)
{
	auto	iter = find_if(Rigids.begin(), Rigids.end(), [&](CRigid* Member)
		{
			if (Rigid == Member)
				return true;

			return false;
		});
	
	if (iter == Rigids.end())
	{
		Add_Actor(Rigid->Get_Actor());
		Rigids.emplace_back(Rigid);
	}
	else
		return E_FAIL;

	return S_OK;
}

//단순히 빼는 코드이기때문에 픽스드틱이나 어플라이틱에서만 안부르면 상관없겠는데...?
//이거 픽스드틱에서 부를라면.... 아니다 픽스드틱에서 부르는건 안좋은 선택일듯.
HRESULT CPhysicsSystem::Detach_Rigid(CRigid* Rigid)
{
	auto	iter = find_if(Rigids.begin(), Rigids.end(), [&](CRigid* Member)
		{
			if (Rigid == Member)
				return true;

			return false;
		});

	if (iter != Rigids.end())
	{
		Sub_Actor(Rigid->Get_Actor());
		Rigids.erase(iter);
	}
	else
		return E_FAIL;

	return S_OK;
}

void CPhysicsSystem::ClearScene()
{
	const PxU32 numActor = m_Scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	//PxShape** shapes = (PxShape**)SAMPLE_ALLOC(sizeof(PxShape*) * numShapes);
	PxActor** Actors = (PxActor**)m_Allocator->allocate(sizeof(PxActor*) * numActor, 0, __FILE__, __LINE__);
	m_Scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, Actors, numActor);
	m_Scene->removeActors(Actors, numActor, false);
}

void CPhysicsSystem::ClearFlags()
{
	for (auto& Rigid : Rigids)
		Rigid->Clear_Flag();
}

const _bool& CPhysicsSystem::Get_Running()
{
	// // O: 여기에 return 문을 삽입합니다.
	return m_bIsRunning;
}


void CPhysicsSystem::Free()
{
	PxCloseExtensions();

	m_PxControllerManager->release();

	for (auto& QuaryFilterCallback : m_QueryFilterCallBack)
	{
		Safe_Delete(QuaryFilterCallback);
	}
	m_QueryFilterCallBack.clear();

	m_DefaultMaterial->release();
	m_DefaultMaterial = nullptr;
	m_Physics->release();
	m_Physics = nullptr;
	Safe_Release(m_FilterLayer);
	delete m_SimulationCallback;
	m_SimulationCallback = nullptr;

	m_CpuDispatcher->release();
	m_CpuDispatcher = nullptr;

	m_Cooking->release();
	m_Cooking = nullptr;

	if (m_PVD)
	{
		PxPvdTransport* transport = m_PVD->getTransport();
		m_PVD->release();
		m_PVD = nullptr;
		transport->release();
	}

	m_Foundation->release();
	m_Foundation = nullptr;
	Safe_Delete(m_Allocator);
}
void CPhysicsSystem::Fixed_Tick()
{
	auto iter = Rigids.begin();

	for (; iter != Rigids.end();)
	{
		if ((*iter)->Fixed_Tick())//아니라면 픽스드틱 돌리자
		{
			++iter;
		}
		else//m_Dead가 트루라면 씬에서빼고 리스트에서도 빼자
		{
			Sub_Actor((*iter)->Get_Actor());
			iter = Rigids.erase(iter);
		}
	}
}
void CPhysicsSystem::Apply_Tick()
{
	for (auto& Rigid : Rigids)
	{
		Rigid->Set_World();
	}
}
#ifdef _DEBUG

void CPhysicsSystem::Debug_Render(CDXDraw* pDxDraw)
{
	//이거아님
  	 const PxRenderBuffer& rb = m_Scene->getRenderBuffer();
   	for (PxU32 i = 0; i < rb.getNbLines(); i++)
   	{
   		const PxDebugLine& line = rb.getLines()[i];
   		// render the line
  		_vector Vec1, Vec2;
  		memcpy(&Vec1, &PxVec4(line.pos0, 1.f), sizeof(_vector));
  		memcpy(&Vec2, &PxVec4(line.pos1, 1.f), sizeof(_vector));
  		pDxDraw->Line_Render(Vec1, Vec2, DirectX::Colors::LightGreen);
   	}

}
#endif // _DEBUG

SimulCallBack::SimulCallBack(PxScene* _Scene)
{
	m_Scene = _Scene;

	PxActorTypeFlags types = PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC;

	PxU32 nbActors = m_Scene->getNbActors(types);
	if (nbActors)
	{
		if(m_Actors)
			Safe_Delete_Array(m_Actors);

		m_Actors = new PxActor * [nbActors];

		m_Scene->getActors(types, m_Actors, nbActors);
		//필요할진 모르겠음 일단 엑터를 보관하는법은 이렇게 될듯.
		//업데이트 콜백을 받아서 엑터를 초기화하는 방식이 좋을듯
		//시뮬레이션 콜백이 엑터를 갖고있던데 이걸 좀 본받아서..?
	}
}

void SimulCallBack::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void SimulCallBack::onWake(PxActor** actors, PxU32 count)
{
	for (PxU32 i = 0; i < count; ++i)
	{

	}
}

void SimulCallBack::onSleep(PxActor** actors, PxU32 count)
{
}

void SimulCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		if (pairHeader.flags & (PxContactPairHeaderFlag::eREMOVED_ACTOR_0 | PxContactPairHeaderFlag::eREMOVED_ACTOR_1) )
			continue;

		const PxContactPair& cp = pairs[i];

		CRigid* RigidA = (CRigid*)pairHeader.actors[0]->userData;
		CRigid* RigidB = (CRigid*)pairHeader.actors[1]->userData;
		if ( RigidA->Get_Host()->Get_Dead() || RigidB->Get_Host()->Get_Dead())
			continue;

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			RigidA->Add_Collision_Simulation(pairHeader.actors[1], PxPairFlag::eNOTIFY_TOUCH_FOUND);
			RigidB->Add_Collision_Simulation(pairHeader.actors[0], PxPairFlag::eNOTIFY_TOUCH_FOUND);
		}
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			RigidA->Add_Collision_Simulation(pairHeader.actors[1], PxPairFlag::eNOTIFY_TOUCH_PERSISTS);
			RigidB->Add_Collision_Simulation(pairHeader.actors[0], PxPairFlag::eNOTIFY_TOUCH_PERSISTS);
		}
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			RigidA->Add_Collision_Simulation(pairHeader.actors[1], PxPairFlag::eNOTIFY_TOUCH_LOST);
			RigidB->Add_Collision_Simulation(pairHeader.actors[0], PxPairFlag::eNOTIFY_TOUCH_LOST);
		}
	}
}

void SimulCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].flags & PxTriggerPairFlag::eREMOVED_SHAPE_OTHER)
		{
			if (pairs[i].flags & PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER)
			{
				continue;
			}
			else
			{
				((CRigid*)pairs[i].triggerActor->userData)->Delete_Collision_Trigger(pairs[i].otherActor);
					continue;
			}
		}
		else if (pairs[i].flags & PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER)
		{
			((CRigid*)pairs[i].otherActor->userData)->Delete_Collision_Trigger(pairs[i].triggerActor);
			continue;
		}
		CRigid* RigidA = (CRigid*)pairs[i].otherActor->userData;
		CRigid* RigidB = (CRigid*)pairs[i].triggerActor->userData;

		if (/*RigidA->Get_Host()->Get_Type() == 1 || RigidB->Get_Host()->Get_Type() == 1 ||*/
			RigidA->Get_Host()->Get_Dead() || RigidB->Get_Host()->Get_Dead())
			continue;

		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
		{
			((CRigid*)pairs[i].otherActor->userData)->Delete_Collision_Trigger(pairs[i].triggerActor);
			((CRigid*)pairs[i].triggerActor->userData)->Delete_Collision_Trigger(pairs[i].otherActor);
		}

		if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			RigidA->Add_Collision_Trigger(pairs[i].triggerActor, PxPairFlag::eNOTIFY_TOUCH_FOUND);
			RigidB->Add_Collision_Trigger(pairs[i].otherActor, PxPairFlag::eNOTIFY_TOUCH_FOUND);
		}

		if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			RigidA->Delete_Collision_Trigger(pairs[i].triggerActor);
			RigidB->Delete_Collision_Trigger(pairs[i].otherActor);
			RigidA->Add_Collision_Trigger(pairs[i].triggerActor, PxPairFlag::eNOTIFY_TOUCH_LOST);
			RigidB->Add_Collision_Trigger(pairs[i].otherActor, PxPairFlag::eNOTIFY_TOUCH_LOST);
		}
	}
}

void SimulCallBack::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}


PxFilterFlags FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}
	// generate contacts for all that were not filtered abov
	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.


		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
		return PxFilterFlag::eDEFAULT;
	}

	return PxFilterFlag::eKILL;
	////////////////////////////////NORTYPOLE/////////////////////////////////
	/*if (filterData0.word3 & CCD_FLAG || isCCDActive(filterData1))
	{
		pairFlags |= PxPairFlag::eSOLVE_CONTACT;
		pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
	}

	if (needsContactReport(filterData0, filterData1))
	{
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
	}

	pairFlags |= PxPairFlag::eCONTACT_DEFAULT;
	return PxFilterFlags();*/

}

PxQueryHitType::Enum PhysicsQueryFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	CRigid* Rigid = (CRigid*)actor->userData;
	CCollider* Collider = (CCollider*)shape->userData;

	if (!Rigid || !Collider)
		return PxQueryHitType::eNONE;

	bool allowTrigger = _uint(m_QueryType) & _uint(QUERYTYPE::TRIGGER);
	bool allowSimulation = _uint(m_QueryType) & _uint(QUERYTYPE::SIMULATION);
	_uint Filter = Collider->Get_Filter(0);
	bool isValidHit = Filter & m_LayerBitmask;

	if (Rigid == m_IgnoreRigid)
		return PxQueryHitType::eNONE;
	
	if (!isValidHit)
		return PxQueryHitType::eNONE;

	if(!allowTrigger && Collider->Get_isTrigger())
		return PxQueryHitType::eNONE;

	if (!allowSimulation && Collider->Get_isSimulation())
		return PxQueryHitType::eNONE;

	return m_HitType;
}

PxQueryHitType::Enum PhysicsQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
	return PxQueryHitType::eNONE;
}

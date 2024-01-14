#include "..\public\Collider.h"
#include "MeshContainer.h"
#include "Model.h"
#include "Navigation.h"
#include "Cell.h"
#include "AstarNode.h"
CCollider::CCollider()
{
}

HRESULT CCollider::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CCollider::NativeConstruct(void* pArg)
{
	m_Shape->userData = this;
	return S_OK;
}

CCollider* CCollider::Create_BoxCollider(_Vector3 Extents, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	CCollider* pCollider = new CCollider();
	pCollider->m_Extents = Extents;
	pCollider->m_LocalPos = LocalPos;
	pCollider->m_LocalEuler = LocalEuler;
	pCollider->m_Type = GEOMETRYTYPE::GT_BOX;

	PxShapeFlags Flag = PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
	CPhysicsSystem* pPhysx = GET_INSTANCE(CPhysicsSystem);
	PxVec3 Pxvector;
	memcpy(&Pxvector, &pCollider->m_Extents, sizeof(PxVec3));
	pCollider->m_Shape = pPhysx->Create_Shape(PxBoxGeometry(Pxvector), *pPhysx->Get_DefaultMtrl(), true, Flag);
	RELEASE_INSTANCE(CPhysicsSystem);

	pCollider->m_Shape->setFlag(PxShapeFlag::eVISUALIZATION, true);

	PxVec3 p;
	memcpy(&p, &LocalPos, sizeof(PxVec3));
	PxQuat q;
	memcpy(&q, &EulerToQuaternion(LocalEuler), sizeof(PxQuat));
	pCollider->m_Shape->setLocalPose(PxTransform(p, q));
	pCollider->NativeConstruct(nullptr);
	return pCollider;
}

CCollider* CCollider::Create_SphereCollider(float Radian, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	CCollider* pCollider = new CCollider();
	pCollider->m_Radius = Radian;
	pCollider->m_LocalPos = LocalPos;
	pCollider->m_LocalEuler = LocalEuler;
	pCollider->m_Type = GEOMETRYTYPE::GT_SPHERE;

	PxShapeFlags Flag = PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
	CPhysicsSystem* pPhysx = GET_INSTANCE(CPhysicsSystem);
	pCollider->m_Shape = pPhysx->Create_Shape(PxSphereGeometry(Radian), *pPhysx->Get_DefaultMtrl(), true, Flag);
	RELEASE_INSTANCE(CPhysicsSystem);
	pCollider->m_Shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	PxVec3 p;
	memcpy(&p, &LocalPos, sizeof(PxVec3));
	PxQuat q;
	memcpy(&q, &EulerToQuaternion(LocalEuler), sizeof(PxQuat));
	pCollider->m_Shape->setLocalPose(PxTransform(p, q));
	pCollider->NativeConstruct(nullptr);
	return pCollider;
}

CCollider* CCollider::Create_CapsuleCollider(float Radian, float Height, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	CCollider* pCollider = new CCollider();
	pCollider->m_Radius = Radian;
	pCollider->m_Height = Height;
	pCollider->m_LocalPos = LocalPos;
	pCollider->m_LocalEuler = LocalEuler;
	pCollider->m_Type = GEOMETRYTYPE::GT_CAPSULE;

	PxShapeFlags Flag = PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
	CPhysicsSystem* pPhysx = GET_INSTANCE(CPhysicsSystem);
	pCollider->m_Shape = pPhysx->Create_Shape(PxCapsuleGeometry(Radian, Height), *pPhysx->Get_DefaultMtrl(), true, Flag);
	RELEASE_INSTANCE(CPhysicsSystem);
	pCollider->m_Shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	PxVec3 p;
	memcpy(&p, &LocalPos, sizeof(PxVec3));
	PxQuat q;
	memcpy(&q, &EulerToQuaternion(LocalEuler), sizeof(PxQuat));
	pCollider->m_Shape->setLocalPose(PxTransform(p, q));
	pCollider->NativeConstruct(nullptr);
	return pCollider;
}

CCollider* CCollider::Create_MeshCollider(float scaleFactor, CModel* model, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	CCollider* pCollider = new CCollider();
	pCollider->m_LocalPos = LocalPos;
	pCollider->m_LocalEuler = LocalEuler;
	pCollider->m_Type = GEOMETRYTYPE::GT_TRIANGLEMESH;

	vector<CMeshContainer*> Containers;
	model->Input_MeshContainers(&Containers);
	_uint VertexCount = 0;
	_uint IndexCount = 0;
	for (auto& Container : Containers)
	{
		VertexCount += Container->Get_NumVertexCount();
		IndexCount += Container->Get_NumIndexCount();
	}
	physx::PxVec3* pPxVerts = new physx::PxVec3[VertexCount];
	physx::PxU32* pPxIndices = new physx::PxU32[IndexCount * 3];
	int VertexIndex = 0;
	int IndexIndex = 0;
	for (auto& Container : Containers)
	{
		//어차피 3의배수갯수라서 이렇게했는데 수정좀 봐야하긴함 테스트용임
		_uint Count = Container->Get_NumVertexCount();
		for (_uint i = 0; i < Count;)
		{
			PxVec3 Vec3;
			_Vector3 VectexPos1 = Container->Get_VertexPos(i);
			memcpy(&Vec3, &VectexPos1, sizeof(PxVec3));
			pPxVerts[VertexIndex + i] = Vec3;
			++i;
			_Vector3 VectexPos2 = Container->Get_VertexPos(i);
			memcpy(&Vec3, &VectexPos2, sizeof(PxVec3));
			pPxVerts[VertexIndex + i] = Vec3;
			++i;
			_Vector3 VectexPos3 = Container->Get_VertexPos(i);
			memcpy(&Vec3, &VectexPos3, sizeof(PxVec3));
			pPxVerts[VertexIndex + i] = Vec3;
			++i;
		}
		VertexIndex += Count;

		Count = Container->Get_NumIndexCount();
		for (_uint i  = 0, j = 0; j < Count; ++j)
		{
			pPxIndices[IndexIndex + i] = Container->Get_Index(j, 2);
			++i;
			pPxIndices[IndexIndex + i] = Container->Get_Index(j, 1);
			++i;
			pPxIndices[IndexIndex + i] = Container->Get_Index(j, 0);
			++i;
		}
		IndexIndex += Count * 3;
	}
	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = VertexCount;
	meshDesc.points.stride = sizeof(physx::PxVec3);
	meshDesc.points.data = pPxVerts;

	meshDesc.triangles.count = IndexCount;
	meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
	meshDesc.triangles.data = pPxIndices;

	CPhysicsSystem* PhysX = GET_INSTANCE(CPhysicsSystem);
	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = PhysX->Get_Cooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status)
	{
		Safe_Release(pCollider);
		Safe_Delete_Array(pPxVerts);
		Safe_Delete_Array(pPxIndices);
		return nullptr;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* triangleMesh = PhysX->Create_TriangleMesh(readBuffer);
	if (!triangleMesh)
	{
		Safe_Release(pCollider);
		Safe_Delete_Array(pPxVerts);
		Safe_Delete_Array(pPxIndices);
		return nullptr;
	}

	//예제파일 스케일 z에 -1한 이유가뭐지 나중에체크
	PxShapeFlags Flag = PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
	PxMeshScale scale(PxVec3(scaleFactor, scaleFactor, scaleFactor), PxQuat(PxIdentity));
	pCollider->m_Shape = PhysX->Create_Shape(PxTriangleMeshGeometry(triangleMesh, scale), *PhysX->Get_DefaultMtrl(), true, Flag);
	pCollider->m_Shape->setContactOffset(0.003f);
	pCollider->m_Shape->setRestOffset(0.002f);

	PxVec3 p;
	memcpy(&p, &LocalPos, sizeof(PxVec3));
	PxQuat q;
	memcpy(&q, &EulerToQuaternion(LocalEuler), sizeof(PxQuat));
	pCollider->m_Shape->setLocalPose(PxTransform(p, q));
	RELEASE_INSTANCE(CPhysicsSystem);
	pCollider->m_Shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	Safe_Delete_Array(pPxVerts);
	Safe_Delete_Array(pPxIndices);
	pCollider->NativeConstruct(nullptr);
	return pCollider;
}

CCollider* CCollider::Create_ConvexMeshCollider(float scaleFactor, CModel* model, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	CCollider* pCollider = new CCollider();
	pCollider->m_LocalPos = LocalPos;
	pCollider->m_LocalEuler = LocalEuler;
	pCollider->m_Type = GEOMETRYTYPE::GT_TRIANGLEMESH;

	vector<CMeshContainer*> Containers;
	model->Input_MeshContainers(&Containers);
	vector<_Vector3> RealVertex = Containers.front()->Get_RealVertex();
	_uint VertexCount = (_uint)RealVertex.size();
	physx::PxVec3* pPxVerts = new physx::PxVec3[VertexCount];
	int VertexIndex = 0;
		//어차피 3의배수갯수라서 이렇게했는데 수정좀 봐야하긴함 테스트용임
	auto iter = RealVertex.begin();
	PxVec3 Vec3;
	for (_uint i = 0; i < VertexCount; ++i)
		{
			_Vector3 VectexPos = *iter;
			++iter;
			memcpy(&Vec3, &VectexPos, sizeof(PxVec3));
			pPxVerts[i] = Vec3;
		}
	physx::PxConvexMeshDesc meshDesc;
	meshDesc.points.count = VertexCount;
	meshDesc.points.stride = sizeof(physx::PxVec3);
	meshDesc.points.data = pPxVerts;
	meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	CPhysicsSystem* PhysX = GET_INSTANCE(CPhysicsSystem);
	PxDefaultMemoryOutputStream writeBuffer;
	PxConvexMeshCookingResult::Enum result;
	bool status = PhysX->Get_Cooking()->cookConvexMesh(meshDesc, writeBuffer, &result);
	if (!status)
	{
		Safe_Release(pCollider);
		Safe_Delete_Array(pPxVerts);
		return nullptr;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxConvexMesh* ConvexMesh = PhysX->Create_ConvexMesh(readBuffer);
	if (!ConvexMesh)
	{
		Safe_Release(pCollider);
		Safe_Delete_Array(pPxVerts);
		return nullptr;
	}

	//예제파일 스케일 z에 -1한 이유가뭐지 나중에체크
	PxShapeFlags Flag = PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
	PxMeshScale scale(PxVec3(scaleFactor, scaleFactor, scaleFactor), PxQuat(PxIdentity));
	PxConvexMeshGeometry shape = PxConvexMeshGeometry(ConvexMesh, scale);
	//	shape.meshFlags |= PxConvexMeshGeometryFlag::eTIGHT_BOUNDS; //볼록객체가 많을때 시뮬성능이 향상될수있음.
	pCollider->m_Shape = PhysX->Create_Shape(shape, *PhysX->Get_DefaultMtrl(), true, Flag);
	pCollider->m_Shape->setContactOffset(0.003f);
	pCollider->m_Shape->setRestOffset(0.002f);
	PxVec3 p;
	memcpy(&p, &LocalPos, sizeof(PxVec3));
	PxQuat q;
	memcpy(&q, &EulerToQuaternion(LocalEuler), sizeof(PxQuat));
	pCollider->m_Shape->setLocalPose(PxTransform(p, q));
	RELEASE_INSTANCE(CPhysicsSystem);
	pCollider->m_Shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	Safe_Delete_Array(pPxVerts);
	pCollider->NativeConstruct(nullptr);
	return pCollider;
}

CCollider* CCollider::Create_NaviCollider(float scaleFactor, CNavigation* NaviGation, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	CCollider* pCollider = new CCollider();
	pCollider->m_LocalPos = LocalPos;
	pCollider->m_LocalEuler = LocalEuler;
	pCollider->m_Type = GEOMETRYTYPE::GT_TRIANGLEMESH;
	_uint IndexCount = NaviGation->Get_CellSize();
	_uint VertexCount = IndexCount * 3;

	physx::PxVec3* pPxVerts = new physx::PxVec3[VertexCount];
	physx::PxU32* pPxIndices = new physx::PxU32[IndexCount * 3];
	int VertexIndex = 0;
	int IndexIndex = 0;
	for (auto& Cell : *NaviGation->Get_Sell())
	{

		for (int i = 0; i < 3; ++i)
		{
			_Vector3 VectexPos = Cell->Get_Point((CCell::POINT)i)->Get_Position();
			PxVec3 Vec3;
			memcpy(&Vec3, &VectexPos, sizeof(PxVec3));
			pPxVerts[VertexIndex + i] = Vec3;
		}
		VertexIndex += 3;

		pPxIndices[IndexIndex] = IndexIndex;
		++IndexIndex;
		pPxIndices[IndexIndex] = IndexIndex;
		++IndexIndex;
		pPxIndices[IndexIndex] = IndexIndex;
		++IndexIndex;
	}
	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = VertexCount;
	meshDesc.points.stride = sizeof(physx::PxVec3);
	meshDesc.points.data = pPxVerts;

	meshDesc.triangles.count = IndexCount;
	meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
	meshDesc.triangles.data = pPxIndices;

	CPhysicsSystem* PhysX = GET_INSTANCE(CPhysicsSystem);
	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = PhysX->Get_Cooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status)
	{
		Safe_Release(pCollider);
		Safe_Delete_Array(pPxVerts);
		Safe_Delete_Array(pPxIndices);
		return nullptr;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* triangleMesh = PhysX->Create_TriangleMesh(readBuffer);
	if (!triangleMesh)
	{
		Safe_Release(pCollider);
		Safe_Delete_Array(pPxVerts);
		Safe_Delete_Array(pPxIndices);
		return nullptr;
	}

	//예제파일 스케일 z에 -1한 이유가뭐지 나중에체크
	PxShapeFlags Flag = PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
	PxMeshScale scale(PxVec3(scaleFactor, scaleFactor, scaleFactor), PxQuat(PxIdentity));
	pCollider->m_Shape = PhysX->Create_Shape(PxTriangleMeshGeometry(triangleMesh, scale), *PhysX->Get_DefaultMtrl(), true, Flag);
	pCollider->m_Shape->setContactOffset(0.003f);
	pCollider->m_Shape->setRestOffset(0.002f);

	PxVec3 p;
	memcpy(&p, &LocalPos, sizeof(PxVec3));
	PxQuat q;
	memcpy(&q, &EulerToQuaternion(LocalEuler), sizeof(PxQuat));
	pCollider->m_Shape->setLocalPose(PxTransform(p, q));
	RELEASE_INSTANCE(CPhysicsSystem);
	pCollider->m_Shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	Safe_Delete_Array(pPxVerts);
	Safe_Delete_Array(pPxIndices);
	pCollider->NativeConstruct(nullptr);
	return pCollider;
}


void CCollider::Set_ToBoxGeometry(_Vector3 Extents, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	if (m_Type != GEOMETRYTYPE::GT_BOX)
		return;

	m_Shape->setGeometry(PxBoxGeometry(Extents.x, Extents.y, Extents.z));
	
	
	PxVec3 p;
	memcpy(&p, &LocalPos, sizeof(PxVec3));
	PxQuat q;
	memcpy(&q, &EulerToQuaternion(LocalEuler), sizeof(PxQuat));
	m_Shape->setLocalPose(PxTransform(p, q));
}

void CCollider::Set_ToShpereGeometry(float Radian, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	if (m_Type != GEOMETRYTYPE::GT_SPHERE)
		return;
	m_Shape->setGeometry(PxSphereGeometry(Radian));


	PxVec3 p;
	memcpy(&p, &LocalPos, sizeof(PxVec3));
	PxQuat q;
	memcpy(&q, &EulerToQuaternion(LocalEuler), sizeof(PxQuat));
	m_Shape->setLocalPose(PxTransform(p, q));
}

void CCollider::Set_ToCapsuleGeometry(float Radian, float Height, _Vector3 LocalPos, _Vector3 LocalEuler)
{
	if (m_Type != GEOMETRYTYPE::GT_CAPSULE)
		return;

	m_Shape->setGeometry(PxCapsuleGeometry(Radian, Height));


	PxVec3 p;
	memcpy(&p, &LocalPos, sizeof(PxVec3));
	PxQuat q;
	memcpy(&q, &EulerToQuaternion(LocalEuler), sizeof(PxQuat));
	m_Shape->setLocalPose(PxTransform(p, q));
}

void CCollider::SetUp_Filtering(const _uint& FilterGroup)
{
	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);

	pPhysX->SetUp_Filtering(m_Shape, FilterGroup);
	RELEASE_INSTANCE(CPhysicsSystem);
}

void CCollider::Set_LocalPos(_Vector3 Pos)
{
	PxVec3 p;
	memcpy(&p, &Pos, sizeof(PxVec3));
	m_Shape->setLocalPose(PxTransform(p, m_Shape->getLocalPose().q));
}

void CCollider::Set_LocalEuler(_Vector3 Euler)
{
	PxQuat q;
	memcpy(&q, &EulerToQuaternion(Euler), sizeof(PxQuat));
	m_Shape->setLocalPose(PxTransform(m_Shape->getLocalPose().p, q));
}

_Vector3 CCollider::Get_LocalPos()
{
	_Vector3 Position;
	memcpy(&Position, &m_Shape->getLocalPose().p, sizeof(_Vector3));
	return Position;
}

_Vector3 CCollider::Get_LocalEuler()
{
	return QuaternionToEuler(TOQUAT(m_Shape->getLocalPose().q));
}

Quaternion CCollider::Get_LocalQuat()
{
	return TOQUAT(m_Shape->getLocalPose().q);
}

PxShape* CCollider::Get_Shape()
{
	return m_Shape;
}

GEOMETRYTYPE* CCollider::Get_Type()
{
	// // O: 여기에 return 문을 삽입합니다.
	return &m_Type;
}

_bool CCollider::Get_isTrigger()
{
	return m_isTrigger;
}

void CCollider::Set_isTrigger(_bool _isTrigger)
{
	m_isTrigger = _isTrigger;
	m_isSimulation = !_isTrigger;
	m_Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, _isTrigger);
}

_bool CCollider::Get_isQuery()
{
	return m_isQuery;
}

void CCollider::Set_isQuery(_bool _isQuery)
{
	m_isQuery = _isQuery;
	m_Shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, _isQuery);
}

_bool CCollider::Get_isSimulation()
{
	return m_isSimulation;
}

void CCollider::Set_isSimulation(_bool _isSimulation)
{
	m_isSimulation = _isSimulation;
	m_Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, _isSimulation);
}

void CCollider::Attach_Shape(PxRigidActor* _Actor)
{
	if (m_isAttach)
		return;

	_bool Attach = _Actor->attachShape(*m_Shape);
	m_isAttach = true;
}

void CCollider::Detach_Shape(PxRigidActor* _Actor)
{
	if (!m_isAttach)
		return;

	_Actor->detachShape(*m_Shape);
	m_isAttach = false;
}

void CCollider::Shape_Setting()
{
	m_Shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, m_isQuery);
	m_Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, m_isSimulation);
	m_Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
	PxVec3 PxExtents;
	switch (m_Type)
	{
	case GEOMETRYTYPE::GT_BOX:
		memcpy(&PxExtents, &m_Extents, sizeof(PxVec3));
		m_Shape->setGeometry(PxBoxGeometry(PxExtents));
		break;
	case GEOMETRYTYPE::GT_CAPSULE:
		m_Shape->setGeometry(PxCapsuleGeometry(m_Radius, m_Height));
		break;
	case GEOMETRYTYPE::GT_SPHERE:
		m_Shape->setGeometry(PxSphereGeometry(m_Radius));
		break;
	}

	PxVec3 p;
	PxQuat q;
	memcpy(&p, &m_LocalPos, sizeof(PxVec3));
	memcpy(&q, &EulerToQuaternion(m_LocalEuler), sizeof(PxQuat));
	m_Shape->setLocalPose(PxTransform(p, q));
}

_float CCollider::Get_Radius()
{
	PxSphereGeometry Geometry;
	if (m_Shape->getSphereGeometry(Geometry))
		return Geometry.radius;
	PxCapsuleGeometry CapGeometry;
	if (m_Shape->getCapsuleGeometry(CapGeometry))
		return CapGeometry.radius;
	return 0.f;
}

_float CCollider::Get_Height()
{
	PxCapsuleGeometry CapGeometry;
	if (m_Shape->getCapsuleGeometry(CapGeometry))
		return CapGeometry.halfHeight;
	return 0.f;
}

_Vector3 CCollider::Get_Extents()
{
	PxBoxGeometry Geometry;
	if (m_Shape->getBoxGeometry(Geometry))
	{
		_Vector3 Extents;
		memcpy(&Extents, &Geometry.halfExtents, sizeof(_Vector3));
		return Extents;
	}

	return _Vector3();
}

void CCollider::Set_Material(_float StaticFriction, _float DynamicFriction, _float restitution)
{
	if (m_Material)
		m_Material->release();

	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	m_Material = pPhysX->Create_Material(_Vector3(StaticFriction, DynamicFriction, restitution));
	RELEASE_INSTANCE(CPhysicsSystem);
	m_Shape->setMaterials(&m_Material, 1);
}

_Vector3 CCollider::Get_Material()
{
	PxMaterial* Material;
	m_Shape->getMaterials(&Material, sizeof(PxMaterial), 0);
	Material[0].userData;
	return _Vector3(Material[0].getStaticFriction(), Material->getDynamicFriction(), Material->getRestitution());
}

_uint CCollider::Get_Filter(_uint word)
{
	PxFilterData Data = m_Shape->getSimulationFilterData();
	switch (word)
	{
	case 0: return Data.word0;
		break;
	case 1:	return Data.word1;
		break;
	case 2:	return Data.word2;
		break;
	case 3:	return Data.word3;
		break;
	default:
		return 9999;
	}
}

void CCollider::Set_Filter(_uint word, _uint Filter)
{
	PxFilterData Data = m_Shape->getSimulationFilterData();
	switch (word)
	{
	case 0: Data.word0 = Filter;
		break;
	case 1:	Data.word1 = Filter;
		break;
	case 2:	Data.word2 = Filter;
		break;
	case 3:	Data.word3 = Filter;
		break;
	default:
		return;
	}
	m_Shape->setSimulationFilterData(Data);
	m_Shape->setQueryFilterData(Data);
}


void CCollider::Set_Geometry(_float Radius)
{
	PxSphereGeometry g(Radius);
	m_Shape->setGeometry(g);
}


void CCollider::Set_Geometry(_float Radius, _float HalfHeight)
{
	PxCapsuleGeometry g(Radius, HalfHeight);
	m_Shape->setGeometry(g);
}

void CCollider::Set_Geometry(_Vector3 Extents)
{
	PxBoxGeometry g(TOPXVEC3(Extents));
	m_Shape->setGeometry(g);
}



CCollider* CCollider::Create()
{
	CCollider* p= new CCollider();

	if (FAILED(p->NativeConstruct_Prototype()))
	{
		Safe_Release(p);
		return nullptr;
	}

	return p;
}

CComponent* CCollider::Clone(void* pArg)
{
	CComponent* p = new CCollider(*this);

	if (FAILED(p->NativeConstruct(pArg)))
	{
		Safe_Release(p);
		return nullptr;
	}

	return p;
}

void CCollider::Free()
{
	__super::Free();

	//m_Material->release();
	if(m_Shape)
		m_Shape->release();
	//해야하지않나?
}

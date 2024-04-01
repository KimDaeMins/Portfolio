#pragma once

#include "Component.h"
#include "DebugDraw.h"
#include "Model.h"
#include "Transform.h"
#include "GameObject.h"

BEGIN(Engine)

class CBounding;
class ENGINE_DLL CCollider final : public CComponent, public enable_shared_from_this<CCollider>
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };



private:
	explicit CCollider();
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg) override;
	const _float3& Get_CenterPos(TYPE eType);
	const _bool& Get_Hit() { return m_ColliderDesc.m_pHostObject->Get_Hit(); }
	void Set_Hit() { m_ColliderDesc.m_pHostObject->Set_Hit(); }
	const TYPE& Get_Type() { return m_eType; }
	shared_ptr<CGameObject> Get_Host() { return m_ColliderDesc.m_pHostObject; }
	void Check();

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
	void Draw();
#endif // _DEBUG

public:
	void MFC_Update(_fmatrix TransformMatrix);
	void Update();
	shared_ptr<class CBounding>			Get_BoundingBox() { return m_pBoundingBox; }

	_bool Collision(shared_ptr<CCollider> pTargetCollider);
	_bool Collision_Ray(TYPE eType, _fvector _Origin, _fvector _Diraction, _float& _Distance);
	_float Get_Distance() { return m_Distance; }
	void Set_Distance(_float pp) { m_Distance = pp; }
private:
	shared_ptr<class CBounding> m_pBoundingBox = nullptr;

	_bool					m_isCollision = false;

	COLLIDERDATA			m_ColliderDesc;
private:
	TYPE					m_eType = TYPE_END;
	shared_ptr<BasicEffect>			m_pEffect = nullptr;
	ComPtr<PrimitiveBatch<DirectX::VertexPositionColor>>	m_pBatch = nullptr;
	ComPtr<ID3D11InputLayout>		m_pInputLayout = nullptr;
	_float					m_Distance = 9999.f;
	shared_ptr<CTransform>				m_pTargetTransform;
	
private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);





public:
	static shared_ptr<CCollider> Create();
	virtual shared_ptr<CComponent> Clone(void* pArg) override;

};

END
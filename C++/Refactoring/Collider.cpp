#include "..\Public\Collider.h"
#include "PipeLine.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Bounding.h"
#include "Renderer.h"
#include "Graphic_Device.h"

CCollider::CCollider()
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_pInputLayout(rhs.m_pInputLayout)
	, m_pBoundingBox(rhs.m_pBoundingBox)
	, m_eType(rhs.m_eType)
	, m_pEffect(rhs.m_pEffect)
	, m_pBatch(rhs.m_pBatch)
{
}

HRESULT CCollider::NativeConstruct_Prototype()
{

	m_pEffect = make_shared<BasicEffect>(g_Device.Get());
	m_pEffect->SetVertexColorEnabled(true);

	const void*	pShaderByteCodes = nullptr;
	size_t	iShaderByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iShaderByteCodeLength);

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(g_DeviceContext.Get());



	if (FAILED(g_Device->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount, pShaderByteCodes, iShaderByteCodeLength, m_pInputLayout.GetAddressOf())))
		return E_FAIL;


	return S_OK;
}

HRESULT CCollider::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_ColliderDesc, pArg, sizeof(COLLIDERDATA));

	m_eType = (CCollider::TYPE)m_ColliderDesc.Type;

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pBoundingBox = make_shared<CBounding>(m_ColliderDesc.Pivot, _float3(m_ColliderDesc.Size.x * 0.5f, m_ColliderDesc.Size.y * 0.5f, m_ColliderDesc.Size.z * 0.5f));
		break;
	case CCollider::TYPE_OBB:
		m_pBoundingBox = make_shared<CBounding>(m_ColliderDesc.Pivot, _float3(m_ColliderDesc.Size.x * 0.5f, m_ColliderDesc.Size.y * 0.5f, m_ColliderDesc.Size.z * 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		break;
	case CCollider::TYPE_SPHERE:
		m_pBoundingBox = make_shared<CBounding>(m_ColliderDesc.Pivot, m_ColliderDesc.Radius);
		break;
	}

	m_pTargetTransform = m_ColliderDesc.m_pHostObject->Get_ComponentPtr(TEXT("Com_Transform"));

	return S_OK;
}

const _float3 & CCollider::Get_CenterPos(TYPE eType)
{
	return m_pBoundingBox->Get_Center();
}
#ifdef _DEBUG

void CCollider::Check()
{
}

HRESULT CCollider::Render()
{
	shared_ptr<CPipeLine>		pPipeLine = GET_INSTANCE(CPipeLine);
	
	g_DeviceContext->IASetInputLayout(m_pInputLayout.Get());

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView((pPipeLine->Get_Transform(CPipeLine::TS_VIEW)));
	m_pEffect->SetProjection((pPipeLine->Get_Transform(CPipeLine::TS_PROJ)));

	m_pEffect->Apply(g_DeviceContext.Get());

	m_pBatch->Begin();

	
	Draw();

	m_pBatch->End();

	return S_OK;
}

void CCollider::Draw()
{
	_vector		color = m_isCollision == false ? DirectX::Colors::LightGreen : DirectX::Colors::LightPink;

	if (m_pBoundingBox->Get_Type() == CBounding::TYPE_SPHERE)
	{
		XMVECTOR origin = XMLoadFloat3(&m_pBoundingBox->Get_Center());

		const float radius = m_pBoundingBox->Get_Radius();

		XMVECTOR xaxis = g_XMIdentityR0 * radius;
		XMVECTOR yaxis = g_XMIdentityR1 * radius;
		XMVECTOR zaxis = g_XMIdentityR2 * radius;

		DX::DrawRing(m_pBatch.Get(), origin, xaxis, zaxis, color);
		DX::DrawRing(m_pBatch.Get(), origin, xaxis, yaxis, color);
		DX::DrawRing(m_pBatch.Get(), origin, yaxis, zaxis, color);
	}
	else if (m_pBoundingBox->Get_Type() == CBounding::TYPE_AABB)
	{
		_float3 extents = m_pBoundingBox->Get_Extents();
		_float3 center = m_pBoundingBox->Get_Center();
		XMMATRIX matWorld = XMMatrixScaling(extents.x, extents.y, extents.z);
		XMVECTOR position = XMLoadFloat3(&center);

		matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

		DX::DrawCube(m_pBatch.Get(), matWorld, color);
	}
	else
	{
		_float4 orentation = m_pBoundingBox->Get_Orientation();
		_float3 extents = m_pBoundingBox->Get_Extents();
		_float3 center = m_pBoundingBox->Get_Center();
		XMMATRIX matWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&orentation));
		XMMATRIX matScale = XMMatrixScaling(extents.x, extents.y, extents.z);

		matWorld = XMMatrixMultiply(matScale, matWorld);
		XMVECTOR position = XMLoadFloat3(&center);
		matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

		DX::DrawCube(m_pBatch.Get(), matWorld, color);
	}
}

#endif // _DEBUG

void CCollider::Update()
{
	m_isCollision = false;
	
	_matrix TransformMatrix = m_pTargetTransform->Get_WorldMatrix();
	if (m_ColliderDesc.isBonecollider)
	{
		_matrix		OffsetMatrix = XMLoadFloat4x4(&m_ColliderDesc.m_OffsetMatrix);
		_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_ColliderDesc.m_pBoneMatrix);
		_matrix		PivotMatrix = XMLoadFloat4x4(&m_ColliderDesc.m_PivotMatrix);

		TransformMatrix = (OffsetMatrix * CombinedTransformationMatrix * PivotMatrix) * TransformMatrix;
	}
	m_pBoundingBox->Update(TransformMatrix);

	shared_ptr<CGameInstance> pGameInstance = GET_INSTANCE(CGameInstance);
	
	pGameInstance->Add_RenderGroup(CRenderer::RENDER_DEBUG, shared_from_this());
}

void CCollider::MFC_Update(_fmatrix TransformMatrix)
{
	m_isCollision = false;

	_matrix matrix = TransformMatrix;

	if (m_ColliderDesc.isBonecollider)
	{
		_matrix		OffsetMatrix = XMLoadFloat4x4(&m_ColliderDesc.m_OffsetMatrix);
		_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_ColliderDesc.m_pBoneMatrix);
		_matrix		PivotMatrix = XMLoadFloat4x4(&m_ColliderDesc.m_PivotMatrix);

		matrix = (OffsetMatrix * CombinedTransformationMatrix * PivotMatrix) * TransformMatrix;
	}
	m_pBoundingBox->Update(matrix);
}


_bool CCollider::Collision(shared_ptr<CCollider> pTargetCollider)
{
	_bool bCollision = m_pBoundingBox->Collision(pTargetCollider->Get_BoundingBox());
	
	if (bCollision)
		m_isCollision = true;

	return bCollision;
}

_bool CCollider::Collision_Ray(TYPE eType, _fvector _Origin, _fvector _Diraction, _float & _Distance)
{
	return m_pBoundingBox->Collision_Ray(_Origin, _Diraction, _Distance);
}

shared_ptr<CCollider> CCollider::Create()
{
	shared_ptr<CCollider>		pInstance = make_shared<CCollider>();

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CCollider");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<CComponent> CCollider::Clone(void * pArg)
{
	shared_ptr<CCollider>		pInstance = make_shared<CCollider>(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CCollider");
		return nullptr;
	}

	return pInstance;
}

#include "..\Public\HierarchyNode.h"




CHierarchyNode::CHierarchyNode()
{
}

HRESULT CHierarchyNode::NativeConstruct(const char* pName, _fmatrix TransformationMatrix, shared_ptr<CHierarchyNode> pParent, _uint iDepth)
{
	strcpy_s(m_szName, pName);
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	XMStoreFloat4x4(&m_ControlMatrix , XMMatrixIdentity());
	m_pParent = pParent;
	m_iDepth = iDepth;

	return S_OK;
}

HRESULT CHierarchyNode::Reserve_Channels(_uint iNumAnimation)
{
	m_Channels.resize(iNumAnimation);

	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix()
{
	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_ControlMatrix) * XMLoadFloat4x4(&m_TransformationMatrix)
			* XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_ControlMatrix) * XMLoadFloat4x4(&m_TransformationMatrix));
}

void CHierarchyNode::Update_CombinedTransformationMatrix(_uint iCurrentAnimIndex)
{
	if (iCurrentAnimIndex >= m_Channels.size())
		return;

	if(nullptr != m_Channels[iCurrentAnimIndex])
		XMStoreFloat4x4(&m_TransformationMatrix, m_Channels[iCurrentAnimIndex]->Get_TransformationMatrix());

	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_ControlMatrix) * XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_ControlMatrix) * XMLoadFloat4x4(&m_TransformationMatrix));
}

void CHierarchyNode::Linear_isBlending_Node(_uint BeforeAnimIndex, _uint CurrentAnimIndex, _float Ratio)
{
	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;
	//둘다있을떈 현재상태 -> 다음0의상태랑 선형보간
	if (nullptr != m_Channels[CurrentAnimIndex] && nullptr != m_Channels[BeforeAnimIndex])
	{
		const vector<KEYFRAME*>* pCurrentKeyFrames = m_Channels[CurrentAnimIndex]->Get_KeyFrames();

		vScale = XMVectorLerp(m_Channels[BeforeAnimIndex]->Get_Scale(),
			XMLoadFloat3(&(*pCurrentKeyFrames)[0]->vScale), Ratio);

		vRotation = XMQuaternionSlerp(m_Channels[BeforeAnimIndex]->Get_Rotation(),
			XMLoadFloat4(&(*pCurrentKeyFrames)[0]->vRotation), Ratio);

		vPosition = XMVectorLerp(m_Channels[BeforeAnimIndex]->Get_Position(),
			XMLoadFloat3(&(*pCurrentKeyFrames)[0]->vPosition), Ratio);

		vPosition = XMVectorSetW(vPosition, 1.f);
		XMStoreFloat4x4(&m_TransformationMatrix,
			XMLoadFloat4x4(&m_ControlMatrix) * XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f),
				vRotation, vPosition));
	}
	//현재애니메이션 채널만 존재할때는 0이랑 선형보간 -> 기본상태는 Identity니까?
	else if (nullptr != m_Channels[CurrentAnimIndex])
	{
		const vector<KEYFRAME*>* pCurrentKeyFrames = m_Channels[CurrentAnimIndex]->Get_KeyFrames();

		vScale = XMVectorLerp(XMLoadFloat3(&_float3(1.f,1.f,1.f)),
			XMLoadFloat3(&(*pCurrentKeyFrames)[0]->vScale), Ratio);

		vRotation = XMQuaternionSlerp(XMLoadFloat4(&_float4(1.f, 0.f, 0.f, 0.f)),
			XMLoadFloat4(&(*pCurrentKeyFrames)[0]->vRotation), Ratio);

		vPosition = XMVectorLerp(XMLoadFloat3(&_float3(0.f, 0.f, 0.f)),
			XMLoadFloat3(&(*pCurrentKeyFrames)[0]->vPosition), Ratio);

		vPosition = XMVectorSetW(vPosition, 1.f);
		XMStoreFloat4x4(&m_TransformationMatrix, XMLoadFloat4x4(&m_ControlMatrix) * XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));
	}
	//이전애니메이션 채널만 존재할때는 0이랑 선형보간 Why? 항등일테니까
	else if (nullptr != m_Channels[BeforeAnimIndex])
	{

		vScale = XMVectorLerp(m_Channels[BeforeAnimIndex]->Get_Scale(),
			XMLoadFloat3(&_float3(1.f, 1.f, 1.f)), Ratio);

		vRotation = XMQuaternionSlerp(m_Channels[BeforeAnimIndex]->Get_Rotation(),
			XMLoadFloat4(&_float4(1.f, 0.f, 0.f, 0.f)), Ratio);

		vPosition = XMVectorLerp(m_Channels[BeforeAnimIndex]->Get_Position(),
			XMLoadFloat3(&_float3(0.f, 0.f, 0.f)), Ratio);

		vPosition = XMVectorSetW(vPosition, 1.f);

		XMStoreFloat4x4(&m_TransformationMatrix, XMLoadFloat4x4(&m_ControlMatrix) * XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));
	}
}

void CHierarchyNode::Save_TransformationMatrix()
{
	m_BeforeTransformationMatrix = m_TransformationMatrix;
}

shared_ptr<CHierarchyNode> CHierarchyNode::Create(const char* pName, _fmatrix TransformationMatrix, shared_ptr<CHierarchyNode> pParent, _uint iDepth)
{
	shared_ptr<CHierarchyNode>	pInstance = make_shared<CHierarchyNode>();

	if (FAILED(pInstance->NativeConstruct(pName, TransformationMatrix, pParent, iDepth)))
	{
		MSG_BOX("Failed To Creating CHierarchyNode");
	}
	return pInstance;
}

#include "..\Public\HierarchyNode.h"

CHierarchyNode::CHierarchyNode()
{

}

void CHierarchyNode::Set_Name(const char* szName)
{
	strcpy_s(m_szName, szName);
}

void CHierarchyNode::Set_ParentName(const char* szParentName)
{
	strcpy_s(m_szParentName, szParentName);
}


HRESULT CHierarchyNode::NativeConstruct(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth)
{
	strcpy_s(m_szName, pName);
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	XMStoreFloat4x4(&m_ControllMatrix, XMMatrixIdentity());
	m_pParent = pParent;
	if(nullptr != pParent)
		strcpy_s(m_szParentName, pParent->Get_Name());
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
			XMLoadFloat4x4(&m_ControllMatrix) * XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix) * XMLoadFloat4x4(&m_ControllTranslationMatrix));
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_ControllMatrix) * XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_ControllTranslationMatrix));

}

void CHierarchyNode::Update_CombinedTransformationMatrix(_uint iCurrentAnimIndex, _bool isBlending)
{
	if (iCurrentAnimIndex >= m_Channels.size())
		return;

	//------------------------------------------------------------------------------------------
	// 루트 노드를 아이덴티화 시켜서 각 뼈들이 루트노드의 회전량, 이동량을 적용받지 않게 한다
	//------------------------------------------------------------------------------------------
	if (nullptr != m_Channels[iCurrentAnimIndex])
	{
		if (m_bIsRoot)
		{
			if (!isBlending)
				XMStoreFloat4x4(&m_TransformationMatrix_ForRoot, m_Channels[iCurrentAnimIndex]->Get_TransformationMatrix());

			XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixIdentity());
		}
		else
			XMStoreFloat4x4(&m_TransformationMatrix, m_Channels[iCurrentAnimIndex]->Get_TransformationMatrix());
	}
	Update_CombinedTransformationMatrix();
}

CHierarchyNode * CHierarchyNode::Create(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth)
{
	CHierarchyNode*	pInstance = new CHierarchyNode();

	if (FAILED(pInstance->NativeConstruct(pName, TransformationMatrix, pParent, iDepth)))
	{
		MSG_BOX("Failed To Creating CHierarchyNode");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHierarchyNode::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}

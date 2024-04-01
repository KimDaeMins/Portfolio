#include "..\Public\MeshContainer.h"
#include "Model.h"
#include "HierarchyNode.h"

CMeshContainer::CMeshContainer()
{

}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs)
	, m_pAIMesh(rhs.m_pAIMesh)
	, m_iNumBones(rhs.m_iNumBones)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
{

}



HRESULT CMeshContainer::NativeConstruct_Prototype(shared_ptr<class CModel> pModel, aiMesh* pMesh, _fmatrix PivotMatrix)
{	
	m_pAIMesh = pMesh;

	if (FAILED(SetUp_VerticesDesc(pModel, pMesh, PivotMatrix)))
		return E_FAIL;	

	if (FAILED(SetUp_IndicesDesc(pMesh)))
		return E_FAIL;

	m_iMaterialIndex = pMesh->mMaterialIndex;




	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::Render()
{	
	_uint		iOffset = 0;

	g_DeviceContext->IASetVertexBuffers(0, 1, m_pVB.GetAddressOf(), &m_iStride, &iOffset);
	g_DeviceContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
	g_DeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	g_DeviceContext->DrawIndexed(m_iNumPrimitive * m_iNumIndicesPerFigure, 0, 0);

	return S_OK;
}

HRESULT CMeshContainer::Create_VertexIndexBuffer()
{
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshContainer::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{
	_uint			iBoneIndex = 0;

	if (0 == m_iNumBones)
	{
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());
	}

	for (auto& pHierarchyNode : m_Bones)
	{
		_matrix		OffsetMatrix = pHierarchyNode->Get_OffsetMatix();
		_matrix		CombinedTransformationMatrix = pHierarchyNode->Get_CombinedMatix(); 

		XMStoreFloat4x4(&pBoneMatrices[iBoneIndex++], XMMatrixTranspose(OffsetMatrix * CombinedTransformationMatrix * PivotMatrix));
	}

	return S_OK;
}

HRESULT CMeshContainer::SetUp_VerticesDesc(shared_ptr<class CModel> pModel, aiMesh* pMesh, _fmatrix PivotMatrix)
{
	m_iNumVertices = pMesh->mNumVertices;	
	m_iNumVertexBuffers = 1;

	CModel::TYPE		eMeshType = pModel->Get_MeshType();
	
	if (CModel::TYPE_NONANIM == eMeshType)
	{
		m_pVertices = make_shared<void>(new VTXMESH[m_iNumVertices], ArrayDeleter<VTXMESH>);
		m_iStride = sizeof(VTXMESH);
	}

	else
	{
		m_pVertices = make_shared<void>(new VTXMESH_ANIM[m_iNumVertices], ArrayDeleter<VTXMESH_ANIM>);
		m_iStride = sizeof(VTXMESH_ANIM);		

		SetUp_SkinnedDesc(pModel, pMesh);
	}

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;


	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		VTXMESH*		pVertices = (VTXMESH*)((_byte*)m_pVertices.get() + (i * m_iStride));

		memcpy(&pVertices->vPosition, &pMesh->mVertices[i], sizeof(_float3));

		XMStoreFloat3(&pVertices->vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices->vPosition), PivotMatrix));

		memcpy(&pVertices->vNormal, &pMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices->vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices->vNormal), PivotMatrix));

		memcpy(&pVertices->vTexUV, &pMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices->vTangent, &pMesh->mTangents[i], sizeof(_float3));
	}

	m_VBSubresourceData.pSysMem = m_pVertices.get();

	return S_OK;
}

HRESULT CMeshContainer::SetUp_IndicesDesc(aiMesh * pMesh)
{
	m_iNumPrimitive = pMesh->mNumFaces;	
	m_iIndicesSize = sizeof(FACEINDICES32);
	m_iNumIndicesPerFigure = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pPrimitiveIndices = make_shared<void>(new FACEINDICES32[m_iNumPrimitive], ArrayDeleter<FACEINDICES32>);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		((FACEINDICES32*)m_pPrimitiveIndices.get())[i]._0 = pMesh->mFaces[i].mIndices[0];
		((FACEINDICES32*)m_pPrimitiveIndices.get())[i]._1 = pMesh->mFaces[i].mIndices[1];
		((FACEINDICES32*)m_pPrimitiveIndices.get())[i]._2 = pMesh->mFaces[i].mIndices[2];
	}

	m_IBSubresourceData.pSysMem = m_pPrimitiveIndices.get();

	return S_OK;
}

HRESULT CMeshContainer::Add_Bones(shared_ptr<CModel> pModel)
{
	if (0 == m_iNumBones)
	{
		shared_ptr<CHierarchyNode>		pHierarchyNode = pModel->Find_HierarchyNode(m_pAIMesh->mName.data);
		if (nullptr == pHierarchyNode)
			return E_FAIL;

		pHierarchyNode->Set_OffsetMatrix(XMMatrixIdentity());
		m_Bones.push_back(pHierarchyNode);

		m_iNumBones = 1;
		return S_OK;
	}


	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		/* 현재 이 뼈가 어떤 정점에게 영햐을 미치고 있는지?! */
		/* 이 뼈ㅑ가 얼마나 영향을 주는지?! */
		aiBone*		pBone = m_pAIMesh->mBones[i];

		shared_ptr<CHierarchyNode>		pHierarchyNode = pModel->Find_HierarchyNode(pBone->mName.data);
		if (nullptr == pHierarchyNode)
			return E_FAIL;

		_matrix		OffsetMatrix;
		memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_matrix));

		pHierarchyNode->Set_OffsetMatrix(XMMatrixTranspose(OffsetMatrix));

		m_Bones.push_back(pHierarchyNode);
	}

	return S_OK;
}

HRESULT CMeshContainer::SetUp_SkinnedDesc(shared_ptr<CModel> pModel, aiMesh * pMesh)
{
	m_iNumBones = pMesh->mNumBones;	

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		/* 현재 이 뼈가 어떤 정점에게 영햐을 미치고 있는지?! */
		/* 이 뼈ㅑ가 얼마나 영향을 주는지?! */
		aiBone*		pBone = pMesh->mBones[i];
		

		/* 현재 이 뼈가 몇개의 정점에 영향르 주는지. */
		for (_uint j = 0; j < pBone->mNumWeights; ++j)
		{
			VTXMESH_ANIM*	pVertices = (VTXMESH_ANIM*)m_pVertices.get();

			if (pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.x == 0.0f)
			{
				pVertices[pBone->mWeights[j].mVertexId].vBlendIndex.x = i;
				pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.x = pBone->mWeights[j].mWeight;
			}

			else if (pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.y == 0.0f)
			{
				pVertices[pBone->mWeights[j].mVertexId].vBlendIndex.y = i;
				pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.y = pBone->mWeights[j].mWeight;
			}

			else if (pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.z == 0.0f)
			{
				pVertices[pBone->mWeights[j].mVertexId].vBlendIndex.z = i;
				pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.z = pBone->mWeights[j].mWeight;
			}

			else
			{
				pVertices[pBone->mWeights[j].mVertexId].vBlendIndex.w = i;
				pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.w = pBone->mWeights[j].mWeight;
			}
		}
	}

	return S_OK;
}

shared_ptr<CMeshContainer> CMeshContainer::Create(shared_ptr<CModel> pModel, aiMesh* pMesh, _fmatrix PivotMatrix)
{
	shared_ptr<CMeshContainer>	pInstance = make_shared<CMeshContainer>();

	if (FAILED(pInstance->NativeConstruct_Prototype(pModel, pMesh, PivotMatrix)))
	{
		MSG_BOX("Failed To Creating CMeshContainer");
	}
	return pInstance;
}

shared_ptr<CComponent> CMeshContainer::Clone(void * pArg)
{
	shared_ptr<CMeshContainer>	pInstance = make_shared<CMeshContainer>(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Clone CMeshContainer");
	}
	return pInstance;
}

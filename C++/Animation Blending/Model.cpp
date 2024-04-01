#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Channel.h"

CModel::CModel()
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)
	, m_pEffect(rhs.m_pEffect)
	, m_PassesDesc(rhs.m_PassesDesc)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_eType(rhs.m_eType)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_HierarchyNodes(rhs.m_HierarchyNodes)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimation(rhs.m_iNumAnimation)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_AnimCut(rhs.m_AnimCut)
{
	m_MeshContainers.resize(m_iNumMaterials);

	for (auto& MtrlMeshContainer : rhs.m_MeshContainers)
	{
		for (auto& pPrototypeMeshContainer : MtrlMeshContainer)
		{
			shared_ptr<CMeshContainer> pMeshContainer = std::static_pointer_cast<CMeshContainer>(pPrototypeMeshContainer->Clone(nullptr));

			m_MeshContainers[pMeshContainer->Get_MaterialIndex()].push_back(pMeshContainer);

		}

	}


	for (auto& pPrototypeAnim : rhs.m_Animations)
	{
		m_Animations.push_back(pPrototypeAnim->Clone());
	}
	for (auto& pPrototypeAnim : rhs.m_ArmAnimations[ARM_LEFT])
	{
		m_Animations.push_back(pPrototypeAnim->Clone());
	}
	for (auto& pPrototypeAnim : rhs.m_ArmAnimations[ARM_RIGHT])
	{
		m_Animations.push_back(pPrototypeAnim->Clone());
	}
}

const _bool CModel::Get_Finished(ARM eARM)
{
	if (eARM == ARM_END)
		return m_Animations[m_iCurrentAnimIndex]->Get_Finished();
	else if (eARM == ARM_LEFT)
		return m_ArmAnimations[ARM_LEFT][m_LeftArmIndex]->Get_Finished();
	else
		return m_ArmAnimations[ARM_RIGHT][m_RightArmIndex]->Get_Finished();
}

const _uint CModel::Get_Frame(ARM eARM)
{
	if (eARM == ARM_END)
		return m_Animations[m_iCurrentAnimIndex]->Get_Frame();
	else if (eARM == ARM_LEFT)
		return m_ArmAnimations[ARM_LEFT][m_LeftArmIndex]->Get_Frame();
	else
		return m_ArmAnimations[ARM_RIGHT][m_RightArmIndex]->Get_Frame();
}

_float4x4* CModel::Get_CombinedMatrixPtr(const char* pBoneName)
{
	shared_ptr<CHierarchyNode> pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return nullptr;

	return pNode->Get_CombinedMatixPtr();
}

_float4x4* CModel::Get_ControlMatrixPtr(const char* pBoneName)
{
	shared_ptr<CHierarchyNode> pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return nullptr;

	return pNode->Get_ControlMatrixPtr();
}

_float4x4 CModel::Get_OffsetMatrix(const char* pBoneName)
{
	_float4x4		OffsetMatrix;

	shared_ptr<CHierarchyNode> pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
	else
		XMStoreFloat4x4(&OffsetMatrix, pNode->Get_OffsetMatix());

	return OffsetMatrix;
}

_float4x4* CModel::Get_CombinedMatrixPtr(_uint _BoneIndex)
{
	return m_HierarchyNodes[_BoneIndex]->Get_CombinedMatixPtr();
}

_float4x4 CModel::Get_OffsetMatrix(_uint _BoneIndex)
{
	_float4x4		OffsetMatrix;
	XMStoreFloat4x4(&OffsetMatrix, m_HierarchyNodes[_BoneIndex]->Get_OffsetMatix());
	return OffsetMatrix;
}

_matrix CModel::Get_NodeWorld(const char* pBoneName)
{
	shared_ptr<CHierarchyNode> pNode = Find_HierarchyNode(pBoneName);

	return pNode->Get_OffsetMatix() * pNode->Get_CombinedMatix() * XMLoadFloat4x4(&m_PivotMatrix);
}

vector<shared_ptr<CHierarchyNode>>* CModel::Get_NodePtr()
{
	return &m_HierarchyNodes;
}

HRESULT CModel::NativeConstruct_Prototype(TYPE eType, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix, _bool AnimCut)
{
	char		szModelPath[MAX_PATH] = "";

	strcpy_s(szModelPath, pModelFilePath);
	strcat_s(szModelPath, pModelFileName);

	/* 1.파일로부터 읽은 여러 정볼르 assimp타입에 맞도록 정리하낟. */
	/* 2.읽어온 정보들을 내가 렌더링할 수 있는 구조로 정리보관한다. */
	/* 2-1. 정점버퍼, 인덱스버퍼, 셰이더(내가만든다) */


	/* 파일로부터 읽은 여러 정볼르 assimp타입에 맞도록 정리하낟. */
	/* m_pScene이라는 놈이 다 가지고 있다. */


	_uint			iFlag = 0;

	m_eType = eType;

	if (TYPE_ANIM == eType)
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_Triangulate;
	else
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_Triangulate;

	m_pScene = m_Importer.ReadFile(szModelPath, iFlag);
	if (nullptr == m_pScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	m_MeshContainers.resize(m_pScene->mNumMaterials);


	if (FAILED(Create_MeshContainers()))
		return E_FAIL;

	if (FAILED(Create_VertexIndexBuffers()))
		return E_FAIL;

	if (FAILED(Create_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Compile_Shader(pShaderFilePath)))
		return E_FAIL;

	if (FAILED(Create_Animation()))
		return E_FAIL;

	m_AnimCut = AnimCut;

	return S_OK;
}

HRESULT CModel::NativeConstruct(void* pArg)
{
	if (TYPE_NONANIM == m_eType)
		return S_OK;

	if (FAILED(Create_HierarchyNodes(m_pScene->mRootNode)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
		{
			return pSour->Get_Depth() < pDest->Get_Depth();
		});

	for (auto& MtrlMeshContainers : m_MeshContainers)
	{
		for (auto& pMeshContainer : MtrlMeshContainers)
		{
			pMeshContainer->Add_Bones(shared_from_this());
		}
	}

	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix();
	}

	for (_uint i = 0; i < m_iNumAnimation; ++i)
	{
		vector<shared_ptr<CChannel>>* pChannels = m_Animations[i]->Get_Channels();

		for (auto& pChannel : *pChannels)
		{
			shared_ptr<CHierarchyNode> pHierarchyNode = Find_HierarchyNode(pChannel->Get_Name());
			if (nullptr == pHierarchyNode)
				return E_FAIL;

			pHierarchyNode->Add_Channel(i, pChannel);
		}
	}
	if (m_AnimCut)
	{
		if (FAILED(Create_ArmAnimation(ARM_LEFT)))
			return E_FAIL;
		if (FAILED(Create_ArmAnimation(ARM_RIGHT)))
			return E_FAIL;
		if (FAILED(Create_ArmHierachy(ARM_LEFT)))
			return E_FAIL;
		if (FAILED(Create_ArmHierachy(ARM_RIGHT)))
			return E_FAIL;
	}

	return S_OK;
}

void CModel::SetUp_Animation(_uint iAnimIndex, _bool isBlending, _bool isLoop)
{
	m_iCurrentAnimIndex = iAnimIndex;
	m_isLoop = isLoop;

	if (m_iBeforeAnimIndex != m_iCurrentAnimIndex && !m_isBlending && isBlending)
	{
		m_isBlending = true;
	}
	else if (m_iBeforeAnimIndex != m_iCurrentAnimIndex && !isBlending)
	{
		m_Animations[m_iBeforeAnimIndex]->Reset();
		m_iBeforeAnimIndex = iAnimIndex;
		m_isBlending = false;
	}
}

void CModel::SetUp_ArmAction(_uint LeftArmIndex, _uint RightArmIndex, _bool LeftArmUpdate, _bool RightArmUpdate)
{
	//인덱스가 달라지면 초기화 ( 장비사용이 끝나면 인덱스를 1000으로 만들거니까 )
	//1000일때는 리셋시키면 터지겠지요
	if (m_LeftArmIndex != LeftArmIndex && m_LeftArmIndex != 1000)
		m_ArmAnimations[ARM_LEFT][m_LeftArmIndex]->Reset();
	if (m_RightArmIndex != RightArmIndex && m_RightArmIndex != 1000)
		m_ArmAnimations[ARM_RIGHT][m_RightArmIndex]->Reset();

	m_LeftArmIndex = LeftArmIndex;
	m_RightArmIndex = RightArmIndex;
	m_RightArmUpdate = RightArmUpdate;
	m_LeftArmUpdate = LeftArmUpdate;
}

HRESULT CModel::Update_Animation(_float fTimeDelta)
{
	if (m_iCurrentAnimIndex > m_iNumAnimation)
		return E_FAIL;


	if (m_isBlending)
	{
		m_fTImeAcc += fTimeDelta;

		if (m_fTImeAcc > m_fBlendingTime)
		{
			m_fTImeAcc = 0.f;
			m_isBlending = false;
			m_Animations[m_iBeforeAnimIndex]->Reset();
			m_iBeforeAnimIndex = m_iCurrentAnimIndex;
		}

		{
			_float fRatio = m_fTImeAcc / m_fBlendingTime;
			if (m_fTImeAcc == 0.f)
				fRatio = 1.f;
			for (auto& pHierarchyNodes : m_HierarchyNodes)
			{
				pHierarchyNodes->Linear_isBlending_Node(m_iBeforeAnimIndex, m_iCurrentAnimIndex, fRatio);
			}
		}

	}
	else
	{
		m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_isLoop);
	}


	if (m_RightArmUpdate)
	{
		
		m_ArmAnimations[ARM_RIGHT][m_RightArmIndex]->Update_TransformationMatrix(fTimeDelta, false);
		for (auto& pHierarchyNodes : m_ArmHierarchyNodes[ARM_RIGHT])
			pHierarchyNodes->Update_CombinedTransformationMatrix(m_RightArmIndex);
	}
	if (m_LeftArmUpdate)
	{
		m_ArmAnimations[ARM_LEFT][m_LeftArmIndex]->Update_TransformationMatrix(fTimeDelta, false);
		for (auto& pHierarchyNodes : m_ArmHierarchyNodes[ARM_LEFT])
			pHierarchyNodes->Update_CombinedTransformationMatrix(m_LeftArmIndex);
	}

	for (auto& pHierarchyNodes : m_HierarchyNodes)
	{
		pHierarchyNodes->Update_CombinedTransformationMatrix();
	}




	return S_OK;
}

HRESULT CModel::Update_CombinedTransformationMatrix()
{
	for (auto& pHierarchyNodes : m_HierarchyNodes)
		pHierarchyNodes->Update_CombinedTransformationMatrix();

	return S_OK;
}

HRESULT CModel::Update_HierachynNode(const char* pBoneName)
{
	shared_ptr<CHierarchyNode> pNode = Find_HierarchyNode(pBoneName);
	pNode->Update_CombinedTransformationMatrix();

	return S_OK;
}


HRESULT CModel::Bind_Shader(_uint iPassIndex)
{
	g_DeviceContext->IASetInputLayout(m_PassesDesc[iPassIndex]->pInputlayout.Get());
	m_PassesDesc[iPassIndex]->pPass->Apply(0, g_DeviceContext.Get());

	return S_OK;
}

HRESULT CModel::Render(_uint iMtrlIndex, _uint iPassIndex)
{
	if (iPassIndex >= m_PassesDesc.size())
		return E_FAIL;

	_matrix		BoneMatrices[128];

	for (auto& pMeshContainer : m_MeshContainers[iMtrlIndex])
	{

		if (TYPE_NONANIM != m_eType)
		{
			_float4x4		BoneMatrices[128];
			ZeroMemory(BoneMatrices, sizeof(_float4x4) * 128);

			/* 현재 메시컨테이너에 영향을 주고있는 뼈들의 최종 렌더링행렬값들을ㅇ 받아온다. */
			pMeshContainer->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));


			/* 셰이더에 던진다. */
			if (FAILED(Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 128)))
				return E_FAIL;

		}

		/* 그려지는 정점들이 뼈들의 행렬집합에서 현재 정점에 영향ㅇ르 주는 뼈의 행렬을 찾아 곱한다음 그린다. */

		Bind_Shader(iPassIndex);

		pMeshContainer->Render();
	}

	return S_OK;
}

void CModel::MaterialInput(_uint MtrlIndex, _uint TexturePass, const _tchar* Path)
{
	if (m_Materials.size() <= MtrlIndex)
		return;
	if (m_Materials[MtrlIndex].pTexture[TexturePass])
		return;

	m_Materials[MtrlIndex].pTexture[TexturePass] = CTexture::Create(Path);

}

void CModel::SetParent(const char* pBoneName, shared_ptr<CHierarchyNode> pParent)
{
	shared_ptr<CHierarchyNode> pNode = Find_HierarchyNode(pBoneName);

	pNode->Set_Parent(pParent);
}

HRESULT CModel::Set_RawValue(const char* pConstantName, void* pData, _uint iSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iSize);
}

HRESULT CModel::Set_ShaderResourceView(const char* pConstantName, _uint iMaterialIndex, aiTextureType eTextureType)
{
	if (nullptr == m_Materials[iMaterialIndex].pTexture[eTextureType])
		return E_FAIL;

	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return pVariable->SetResource(m_Materials[iMaterialIndex].pTexture[eTextureType]->Get_SRV().Get());
}

shared_ptr<CHierarchyNode> CModel::Find_HierarchyNode(const char* pNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
		{
			return !strcmp(pNodeName, pNode->Get_Name());
		});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;
}

HRESULT CModel::Create_MeshContainers()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_iNumMeshes = m_pScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh* pMesh = m_pScene->mMeshes[i];
		if (nullptr == pMesh)
			return E_FAIL;

		/* 파일로 읽어온 정점과인덱스의 정보들을 저장한다.  */
		shared_ptr<CMeshContainer> pMeshContainer = CMeshContainer::Create(shared_from_this(), pMesh, TYPE_NONANIM == m_eType ? XMLoadFloat4x4(&m_PivotMatrix) : XMMatrixIdentity());
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_MeshContainers[pMesh->mMaterialIndex].push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Create_Materials(const char* pModelFilePath)
{
	m_iNumMaterials = m_pScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		aiMaterial* pMaterial = m_pScene->mMaterials[i];

		MESHMATERIAL		MeshMaterialDesc;
		ZeroMemory(&MeshMaterialDesc, sizeof(MESHMATERIAL));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		TexturePath;

			if (FAILED(pMaterial->GetTexture(aiTextureType(j), 0, &TexturePath)))
				continue;

			char		szFileName[MAX_PATH];
			char		szExt[MAX_PATH];

			_splitpath_s(TexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			char		szFullPath[MAX_PATH] = "";

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szPerfectPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szPerfectPath, MAX_PATH);

			MeshMaterialDesc.pTexture[j] = CTexture::Create(szPerfectPath);
			if (nullptr == MeshMaterialDesc.pTexture[j])
				return E_FAIL;
		}
		m_Materials.push_back(MeshMaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Compile_Shader(const _tchar* pShaderFilePath)
{
	_uint		iNumElements = 0;

	D3D11_INPUT_ELEMENT_DESC	Elements[D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT];
	ZeroMemory(Elements, sizeof(D3D11_INPUT_ELEMENT_DESC) * D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT);


	if (TYPE_NONANIM == m_eType)
	{
		iNumElements = 4;

		Elements[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}
	else
	{
		iNumElements = 6;

		Elements[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[4] = { "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[5] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG	
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iFlag, 0, g_Device.Get(), m_pEffect.GetAddressOf(), nullptr)))
		return E_FAIL;


	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);

	D3DX11_TECHNIQUE_DESC			TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));

	pTechnique->GetDesc(&TechniqueDesc);

	m_PassesDesc.reserve(TechniqueDesc.Passes);

	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		shared_ptr<PASSDESC>		pPassDesc = make_shared<PASSDESC>();

		pPassDesc->pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC		PassDesc;
		ZeroMemory(&PassDesc, sizeof(D3DX11_PASS_DESC));

		pPassDesc->pPass->GetDesc(&PassDesc);

		if (FAILED(g_Device->CreateInputLayout(Elements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pPassDesc->pInputlayout)))
			return E_FAIL;

		m_PassesDesc.push_back(pPassDesc);
	}

	return S_OK;
}

HRESULT CModel::Create_VertexIndexBuffers()
{
	for (auto& pMtrlMeshContainer : m_MeshContainers)
	{
		for (auto& pMeshContainer : pMtrlMeshContainer)
			pMeshContainer->Create_VertexIndexBuffer();
	}


	return S_OK;
}

HRESULT CModel::Create_HierarchyNodes(aiNode* pNode, shared_ptr<CHierarchyNode> pParent, _uint iDepth)
{
	_matrix			TransformationMatrix;
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_matrix));

	shared_ptr<CHierarchyNode> pHierarchyNode = CHierarchyNode::Create(pNode->mName.data, XMMatrixTranspose(TransformationMatrix), pParent, iDepth);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_HierarchyNodes.push_back(pHierarchyNode);

	pHierarchyNode->Reserve_Channels(m_pScene->mNumAnimations);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		Create_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth + 1);
	}

	return S_OK;
}

HRESULT CModel::Create_Animation()
{
	m_iNumAnimation = m_pScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimation; ++i)
	{
		aiAnimation* pAnim = m_pScene->mAnimations[i];

		shared_ptr<CAnimation> pAnimation = CAnimation::Create(pAnim->mName.data, pAnim->mDuration, pAnim->mTicksPerSecond);
		if (nullptr == pAnimation)
			return E_FAIL;

		/* 현재 애니메이션에 영향을 주는 뼈의 갯수. */
		for (_uint j = 0; j < pAnim->mNumChannels; ++j)
		{
			aiNodeAnim* pNodeAnim = pAnim->mChannels[j];

			shared_ptr<CChannel> pChannel = CChannel::Create(pNodeAnim->mNodeName.data);
			if (nullptr == pChannel)
				return E_FAIL;

			/* 이 뼈는 몇개의 키프레임에서 사용되고 있는지? */
			_uint	iNumMaxKeyFrames = max(pNodeAnim->mNumScalingKeys, pNodeAnim->mNumRotationKeys);
			iNumMaxKeyFrames = max(iNumMaxKeyFrames, pNodeAnim->mNumPositionKeys);

			_float3		vScale = _float3(1.f, 1.f, 1.f);
			_float4		vRotation = _float4(0.f, 0.f, 0.f, 0.f);
			_float3		vPosition = _float3(0.f, 0.f, 0.f);

			for (_uint k = 0; k < iNumMaxKeyFrames; ++k)
			{
				KEYFRAME* pKeyFrame = new KEYFRAME;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				if (pNodeAnim->mNumScalingKeys > k)
				{
					memcpy(&vScale, &pNodeAnim->mScalingKeys[k].mValue, sizeof(_float3));
					pKeyFrame->Time = pNodeAnim->mScalingKeys[k].mTime;
				}

				if (pNodeAnim->mNumRotationKeys > k)
				{
					vRotation.x = pNodeAnim->mRotationKeys[k].mValue.x;
					vRotation.y = pNodeAnim->mRotationKeys[k].mValue.y;
					vRotation.z = pNodeAnim->mRotationKeys[k].mValue.z;
					vRotation.w = pNodeAnim->mRotationKeys[k].mValue.w;
					pKeyFrame->Time = pNodeAnim->mRotationKeys[k].mTime;
				}

				if (pNodeAnim->mNumPositionKeys > k)
				{
					memcpy(&vPosition, &pNodeAnim->mPositionKeys[k].mValue, sizeof(_float3));
					pKeyFrame->Time = pNodeAnim->mPositionKeys[k].mTime;
				}

				pKeyFrame->vScale = vScale;
				pKeyFrame->vRotation = vRotation;
				pKeyFrame->vPosition = vPosition;

				pChannel->Add_KeyFrame(pKeyFrame);
			}

			pAnimation->Add_Channels(pChannel);
		}

		m_Animations.push_back(pAnimation);

	}

	return S_OK;
}

HRESULT CModel::Create_ArmAnimation(ARM eARM)
{
	for (_uint i = 0; i < m_iNumAnimation; ++i)
	{
		aiAnimation* pAnim = m_pScene->mAnimations[i];

		shared_ptr<CAnimation> pAnimation = CAnimation::Create(pAnim->mName.data, pAnim->mDuration, pAnim->mTicksPerSecond);
		if (nullptr == pAnimation)
			return E_FAIL;
		shared_ptr<CHierarchyNode> pNode[7] = {};
		if (eARM == ARM_LEFT)
		{
			pNode[0] = Find_HierarchyNode("LeftHand");
			pNode[1] = Find_HierarchyNode("LeftHandShield");
			pNode[2] = Find_HierarchyNode("LeftHandBlade");
			pNode[3] = Find_HierarchyNode("LeftShoulder_Parts");
			pNode[4] = Find_HierarchyNode("LeftForeArm");
			pNode[5] = Find_HierarchyNode("LeftArm");
			pNode[6] = Find_HierarchyNode("Root_LeftArm");
		}
		else if (eARM == ARM_RIGHT)
		{
			pNode[0] = Find_HierarchyNode("RightHand");
			pNode[1] = Find_HierarchyNode("RightHandShield");
			pNode[2] = Find_HierarchyNode("RightHandBlade");
			pNode[3] = Find_HierarchyNode("RightShoulder_Parts");
			pNode[4] = Find_HierarchyNode("RightForeArm");
			pNode[5] = Find_HierarchyNode("RightArm");
			pNode[6] = Find_HierarchyNode("Root_RightArm");
		}
		shared_ptr<CChannel> pChannel = nullptr;
		for (int k = 0; k < 7; ++k)
		{
			pChannel = pNode[k]->Get_Channel(i);
			if (nullptr == pChannel)
				continue;
			pAnimation->Add_Channels(pChannel);
		}

		m_ArmAnimations[eARM].push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Create_ArmHierachy(ARM eARM)
{
	shared_ptr<CHierarchyNode> pNode[7] = {};
	if (eARM == ARM_LEFT)
	{
		pNode[0] = Find_HierarchyNode("LeftHand");
		pNode[1] = Find_HierarchyNode("LeftHandShield");
		pNode[2] = Find_HierarchyNode("LeftHandBlade");
		pNode[3] = Find_HierarchyNode("LeftShoulder_Parts");
		pNode[4] = Find_HierarchyNode("LeftForeArm");
		pNode[5] = Find_HierarchyNode("LeftArm");
		pNode[6] = Find_HierarchyNode("Root_LeftArm");
	}
	else if (eARM == ARM_RIGHT)
	{
		pNode[0] = Find_HierarchyNode("RightHand");
		pNode[1] = Find_HierarchyNode("RightHandShield");
		pNode[2] = Find_HierarchyNode("RightHandBlade");
		pNode[3] = Find_HierarchyNode("RightShoulder_Parts");
		pNode[4] = Find_HierarchyNode("RightForeArm");
		pNode[5] = Find_HierarchyNode("RightArm");
		pNode[6] = Find_HierarchyNode("Root_RightArm");
	}

	for (int i = 0; i < 7; ++i)
	{
		m_ArmHierarchyNodes[eARM].push_back(pNode[i]);
	}

	sort(m_ArmHierarchyNodes[eARM].begin(), m_ArmHierarchyNodes[eARM].end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
		{
			return pSour->Get_Depth() < pDest->Get_Depth();
		});


	return S_OK;
}



shared_ptr<CModel> CModel::Create(TYPE eType, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix, _bool AnimCut)
{
	shared_ptr<CModel> pInstance = make_shared<CModel>();

	if (FAILED(pInstance->NativeConstruct_Prototype(eType, pShaderFilePath, pModelFilePath, pModelFileName, PivotMatrix, AnimCut)))
	{
		MSG_BOX("Failed To Creating CModel");
	}
	return pInstance;
}

shared_ptr<CComponent> CModel::Clone(void* pArg)
{
	shared_ptr<CModel> pInstance = make_shared<CModel>(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CModel");
	}
	return pInstance;
}

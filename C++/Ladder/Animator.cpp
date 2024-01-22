#include "..\Public\Animator.h"
#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Channel.h"

CAnimator::CAnimator(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_iNumAnimation(0)
	, m_iCurrentAnimIndex(0)
{
	m_PivotMatrix = {};
}

CAnimator::CAnimator(const CAnimator & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)
	, m_HierarchyNodes(rhs.m_HierarchyNodes)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimation(rhs.m_iNumAnimation)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iRootNodeIndex(rhs.m_iRootNodeIndex)
	, m_RootNodeMatrixForTransform(rhs.m_RootNodeMatrixForTransform)
	, m_RootNodeMatrixForRender(rhs.m_RootNodeMatrixForRender)
	, m_bBoneSetupSequence(rhs.m_bBoneSetupSequence)
	, m_fAnimSpeed(rhs.m_fAnimSpeed)
	, m_fBlendingTime(rhs.m_fBlendingTime)
	, m_bIsBlendingOnOff(rhs.m_bIsBlendingOnOff)
{
// 	for (auto& pPrototypeAnim : rhs.(*m_Animations))
// 	{
// 		(*m_Animations).push_back(pPrototypeAnim->Clone());
// 	}

	m_BeforeKeyFrame.resize(256);
}

const char* CAnimator::Get_RootName() const
{
	return (*m_HierarchyNodes)[m_iRootNodeIndex]->Get_Name();
}

_Matrix* CAnimator::Get_ControllMatrixPtr(const char* pBoneName)
{
	CHierarchyNode* pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return nullptr;

	return (_Matrix*)pNode->Get_ControllMatrixPtr();
}

_Matrix* CAnimator::Get_ControllTranslationMatrixPtr(const char* pBoneName)
{
	CHierarchyNode* pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return nullptr;

	return (_Matrix*)pNode->Get_ControllTranslationMatrixPtr();
}

_Matrix* CAnimator::Get_CombinedMatrixPtr(const char * pBoneName)
{
	CHierarchyNode*		pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return nullptr;

	return (_Matrix*)pNode->Get_CombinedMatixPtr();
}

_Matrix CAnimator::Get_OffsetMatrix(const char * pBoneName)
{
	_float4x4		OffsetMatrix;

	CHierarchyNode*		pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
	else
		XMStoreFloat4x4(&OffsetMatrix, pNode->Get_OffsetMatix());

	return OffsetMatrix;
}

_float4x4 CAnimator::GetDiffMatrix()
{
	//------------------------
	// 신규 알고리즘
	//------------------------
	_Matrix DiffRotation = *m_RootNodeMatrixForTransform;
	_Matrix DiffPosition = _Matrix::CreateTranslation(DiffRotation.Translation());
	DiffRotation.Translation(_Vector3(0.f, 0.f, 0.f));
	_matrix DiffMatrix = XMMatrixInverse(nullptr, DiffRotation) * XMLoadFloat4x4(&(*m_HierarchyNodes)[m_iRootNodeIndex]->Get_TransformationMatix_ForRoot()) * XMMatrixInverse(nullptr, DiffPosition);
	*m_RootNodeMatrixForTransform = (*m_HierarchyNodes)[m_iRootNodeIndex]->Get_TransformationMatix_ForRoot();

	_float4x4 DiffMatrix_For_Return;

	if (Get_IsFinished())
	{
		DiffMatrix = XMMatrixIdentity();
	}
	XMStoreFloat4x4(&DiffMatrix_For_Return, DiffMatrix);

	//------------------------
	// 과거 알고리즘
	//------------------------

	/*_matrix DiffMatrix = XMMatrixIdentity();
	_float4x4 DiffMatrix_For_Return;

	if (Get_CurKeyFrame() != 0)
	{
		DiffMatrix = XMLoadFloat4x4(&(*m_HierarchyNodes)[m_iRootNodeIndex]->Get_TranslationMatix()) - XMLoadFloat4x4(&(*m_RootNodeMatrixForTransform));

		*m_RootNodeMatrixForTransform = (*m_HierarchyNodes)[m_iRootNodeIndex]->Get_TranslationMatix();

	}
	XMStoreFloat4x4(&DiffMatrix_For_Return, DiffMatrix);*/


	return DiffMatrix_For_Return;
}

_float4x4 CAnimator::GetDiffMatrix_Gara()
{
	_matrix DiffMatrix = XMMatrixIdentity();
	_float4x4 DiffMatrix_For_Return;

	if (Get_CurKeyFrame() != 0)
	{
		DiffMatrix = XMLoadFloat4x4(&(*m_HierarchyNodes)[m_iRootNodeIndex]->Get_TransformationMatix_ForRoot()) - XMLoadFloat4x4(&(*m_RootNodeMatrixForTransform));

		*m_RootNodeMatrixForTransform = (*m_HierarchyNodes)[m_iRootNodeIndex]->Get_TransformationMatix_ForRoot();

	}
	XMStoreFloat4x4(&DiffMatrix_For_Return, DiffMatrix);

	return DiffMatrix_For_Return;
}

const _uint CAnimator::Get_CurKeyFrame()
{
	return (*m_Animations)[m_iCurrentAnimIndex]->Get_CurKeyFrame();
}

const _uint CAnimator::Get_CurKeyFrame_Cursor()
{
	return (*m_Animations)[m_iCurrentAnimIndex]->Get_CurKeyFrame_Cursor();
}

void CAnimator::Set_AnimationSpeed(_float fSpeed)
{
	(*m_Animations)[m_iCurrentAnimIndex]->Set_AnimationSpeed(fSpeed);
}

void CAnimator::Set_AnimationOriginSpeed()
{
	(*m_Animations)[m_iCurrentAnimIndex]->Set_AnimationOriginSpeed();
}

void CAnimator::Set_InitCombinedMat(vector<class CHierarchyNode*> pHierarchyNode)
{
	for (auto& pHierarchyNode : pHierarchyNode)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimIndex, false);
	}
}

void CAnimator::Set_AnimationSound(_bool AnimationSound)
{
	(*m_Animations)[m_iCurrentAnimIndex]->Set_AnimationSound(AnimationSound);
}

void CAnimator::AnimationBlending(_float fTimeDelta, _bool isLoop)
{
	vector<class CChannel*>* CurChannels = (*m_Animations)[m_iCurrentAnimIndex]->Get_Channels();
	//m_BeforeKeyFrame의 Idx 는 channel과 같음.

	m_fTimeAcc += fabs(fTimeDelta * m_fBlendingTime);


	if (m_fTimeAcc >= 1.f)
	{
		m_fTimeAcc = 1.f;
		m_bIsBlendingFinished = true;
		m_bIsBlending = false;
		m_iBeforeAnimIndex = m_iCurrentAnimIndex;
	}

	_uint iCurrentKeyFrameIndex = 0;

	/* 채널들이 키프레임들을 가지고 있으니까. 가지고와서 상태 행렬을 만들고. . */
	/* 재생된 시간에 맞는 상태 행렬을 채널에 보관해주려고. */
	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;
	int			i = 0;

	for (auto& pChannel : *CurChannels)
	{
		vector<KEYFRAME*>	KeyFrames = *pChannel->Get_KeyFrames();

		iCurrentKeyFrameIndex = pChannel->Get_KeyFrameIndex();

		// TODO : Blending쪽 이거 문제 생기면 바꾸는걸로
		/*
		비포얼chaneels 넣고 같은채널들이 블랜딩되어야함....... 
		채널 인덱스나 채널 네임 저장해놓고 같은놈들만 러프되게 만들고
		아닌놈들 따로 뭐 처리해줘야함.
		*/

		/* 키프레임사이에 있을때 뼈의 상태행렬을 선형보간으로 만들어낸다. */
		vScale = XMVectorLerp(XMLoadFloat3(&m_BeforeKeyFrame[i].vScale),
			XMLoadFloat3(&KeyFrames[0]->vScale), m_fTimeAcc);

		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_BeforeKeyFrame[i].vRotation),
			XMLoadFloat4(&KeyFrames[0]->vRotation), m_fTimeAcc);

		vPosition = XMVectorLerp(XMLoadFloat3(&m_BeforeKeyFrame[i].vPosition),
			XMLoadFloat3(&KeyFrames[0]->vPosition), m_fTimeAcc);

		vPosition = XMVectorSetW(vPosition, 1.f);

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		pChannel->Set_TransformationMatrix(TransformationMatrix);
		pChannel->Set_Scale(vScale);
		pChannel->Set_Rotation(vRotation);
		pChannel->Set_Position(vPosition);

		++i;

	}

	if (true == m_bIsBlendingFinished)
	{
		if (true == isLoop)
		{
			m_bIsBlendingFinished = false;
			m_fTimeAcc = 0.f;
		}
	}
}

void CAnimator::ClearKeyFrameValue()
{
	KEYFRAME value = { _float3(0.f, 0.f, 0.f), _float4(0.f, 0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), 0.f };
	fill(m_BeforeKeyFrame.begin(), m_BeforeKeyFrame.end(), value);
}

_bool CAnimator::PlayAnimationFrameSound(_uint Frame, _uint ResetFrame, TCHAR* pSoundKey, _uint SoundID, _uint eID, _uint SoundCount, _float SoundVolume, class CTransform* pTrans)
{
	return (*m_Animations)[m_iCurrentAnimIndex]->PlayAnimationFrameSound(Frame, ResetFrame, pSoundKey, SoundID, eID, SoundCount, SoundVolume, pTrans);
}

HRESULT CAnimator::NativeConstruct_Prototype()
{	
	return S_OK;
}

HRESULT CAnimator::NativeConstruct(void* pArg)
{
	return S_OK;
}

void CAnimator::SetUp_ModelData(class CModel* pModel)
{
	m_iRootNodeIndex = pModel->GetRootNodeIndex();
	m_HierarchyNodes = pModel->GetHierarchyNode();
	m_Animations = pModel->GetAnimations();
	(*m_HierarchyNodes)[m_iRootNodeIndex]->Set_IsRoot();
	m_RootNodeMatrixForTransform = pModel->GetRootNodeForTransform();
	m_RootNodeMatrixForRender = pModel->GetRootNodeForRender();
	m_PivotMatrix = pModel->Get_PivotMatrix();	
	m_pScene = pModel->Get_Scene();
	m_iNumAnimation = pModel->Get_NumAnimations();

// 	if (FAILED(Load_Animation(pModel)))
// 	{
// 		Create_Animation();
// 		Save_Animations(pModel);
// 	}
}

void CAnimator::SetUp_Animation(_uint iAnimIndex, _bool isLoop, _bool bUseRootRotIdentify, _bool bUseRootPosIdentify, _float fTime)
{
	m_iCurrentAnimIndex = iAnimIndex;
	m_isLoop = isLoop;

	if (m_iBeforeAnimIndex != m_iCurrentAnimIndex && true == m_bIsBlendingOnOff)
	{
		ClearKeyFrameValue();
		(*m_Animations)[m_iBeforeAnimIndex]->Set_BeforeKeyFrame(&m_BeforeKeyFrame);
		(*m_Animations)[m_iBeforeAnimIndex]->Set_InitAnim();
	}

	(*m_Animations)[m_iCurrentAnimIndex]->Set_InitAnim();

	(*m_Animations)[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTime, m_isLoop);

	for (auto& pHierarchyNode : (*m_HierarchyNodes))
	{
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimIndex, false);
	}

	*m_RootNodeMatrixForTransform = (*m_HierarchyNodes)[m_iRootNodeIndex]->Get_TransformationMatix_ForRoot();
	*m_RootNodeMatrixForRender = (*m_HierarchyNodes)[m_iRootNodeIndex]->Get_TransformationMatix_ForRoot();

	//-------------------------------------------------------------------------------------------------
	// 애니메이션의 키프레임 0에 해당하는 로테이션과 포지션을 내 월드에 바로 적용할 것인지를 결정한다
	//-------------------------------------------------------------------------------------------------
	_Matrix idenMat = XMMatrixIdentity();
	if (bUseRootRotIdentify)
	{
		memcpy(&m_RootNodeMatrixForTransform->_11, &idenMat._11, sizeof(_Vector4));
		memcpy(&m_RootNodeMatrixForTransform->_21, &idenMat._21, sizeof(_Vector4));
		memcpy(&m_RootNodeMatrixForTransform->_31, &idenMat._31, sizeof(_Vector4));
	}
	if (bUseRootPosIdentify)
	{
		memcpy(&m_RootNodeMatrixForTransform->_41, &idenMat._41, sizeof(_Vector4));
	}
}

HRESULT CAnimator::Update_Animation(_float fTimeDelta)
{
	if (m_iCurrentAnimIndex > m_iNumAnimation)
		return E_FAIL;

	/* 현재 애니메이션 상태에 맞는 뼈의 행렬들을 모두 갱신한다. */
	fTimeDelta *= m_fAnimSpeed;
	_bool bNeedRootNodeSetup = false;

	if (m_iBeforeAnimIndex != m_iCurrentAnimIndex)
	{
		m_bIsBlending = true;
	}

	if (true == m_bIsBlending && true == m_bIsBlendingOnOff)
	{
		AnimationBlending(fTimeDelta);
		for (auto& pHierarchyNode : (*m_HierarchyNodes))
		{
			pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimIndex, true);
		}
	}
	else
	{
		m_iBeforeAnimIndex = m_iCurrentAnimIndex;
		m_bIsBlending = false;
		(*m_Animations)[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_isLoop, &bNeedRootNodeSetup);

		for (auto& pHierarchyNode : (*m_HierarchyNodes))
		{
			pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimIndex, false);
		}
	}


	if (bNeedRootNodeSetup == true)
	{
		*m_RootNodeMatrixForTransform = (*m_HierarchyNodes)[m_iRootNodeIndex]->Get_TransformationMatix_ForRoot();
		*m_RootNodeMatrixForRender = (*m_HierarchyNodes)[m_iRootNodeIndex]->Get_TransformationMatix_ForRoot();
	}

	return S_OK;
}


CHierarchyNode * CAnimator::Find_HierarchyNode(const char * pNodeName)
{
	auto	iter = find_if(m_HierarchyNodes->begin(), m_HierarchyNodes->end(), [&](CHierarchyNode* pNode)
		{
			return !strcmp(pNodeName, pNode->Get_Name());
		});

	return *iter;
}


HRESULT CAnimator::Create_Animation()
{
	m_iNumAnimation = m_pScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimation; ++i)
	{
		aiAnimation* pAnim = m_pScene->mAnimations[i];

		CAnimation* pAnimation = CAnimation::Create(pAnim->mName.data, pAnim->mDuration, pAnim->mTicksPerSecond);
		if (nullptr == pAnimation)
			return E_FAIL;

		/* 현재 애니메이션에 영향을 주는 뼈의 갯수. */
		for (_uint j = 0; j < pAnim->mNumChannels; ++j)
		{
			aiNodeAnim* pNodeAnim = pAnim->mChannels[j];

			CChannel* pChannel = CChannel::Create(pNodeAnim->mNodeName.data);
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

		(*m_Animations).push_back(pAnimation);

	}


	for (_uint i = 0; i < m_iNumAnimation; ++i)
	{
		vector<class CChannel*>* pChannels = (*m_Animations)[i]->Get_Channels();

		for (auto& pChannel : *pChannels)
		{
			CHierarchyNode* pHierarchyNode = Find_HierarchyNode(pChannel->Get_Name());
			if (nullptr == pHierarchyNode)
				return E_FAIL;

			pHierarchyNode->Add_Channel(i, pChannel);
		}
	}

	return S_OK;
}

HRESULT CAnimator::Load_Animation(class CModel* pModel)
{
	//---------------------------------------------------------------------------------------------------------------------------
	// 모델 데이터가 없으면 FAIL 반환
	// 모델 데이터가 있으면 S_OK 반환
	// 
	// 모델 데이터는 Save시 항상 fbx의 이름으로 만들기 때문에 해당 fbx 이름으로 Check 한다. 
	// 
	// OPEN_EXISTING Flag 설정으로 파일이 없으면 ERROR_FILE_NOT_FOUND 오류를 발생시킨다.
	//---------------------------------------------------------------------------------------------------------------------------

	//--------------------------------------------------------------
	// 애니메이션 Read
	//--------------------------------------------------------------
	_ulong			dwByte = 0;
	char			szBuf[BUF_MAX] = "";
	char			szBuf2[BUF_MAX] = "";

	char		szFullPath[BUF_MAX] = "../../Reference/Res_Client/Resources/ModelSaveDatas/";
	strcat_s(szFullPath, pModel->Get_ModelName());

	// 모델 폴더가 존재하지 않는 경우
	if (-1 == _access(szFullPath, 0))
		return E_FAIL;

	strcat_s(szFullPath, "/");
	strcat_s(szFullPath, "Animations/");

	// 모델 폴더안에 Bones 폴더가 존재하지 않는 경우
	if (-1 == _access(szFullPath, 0))
		return E_FAIL;

	strcat_s(szFullPath, pModel->Get_ModelName());
	strcat_s(szFullPath, ".dat");
	_tchar		szPerfectPath[BUF_MAX] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, BUF_MAX);
	HANDLE		hFile = CreateFile(szPerfectPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (ERROR_FILE_NOT_FOUND == GetLastError())
		return E_FAIL;

	if (0 == hFile)
		return E_FAIL;

	// 모델에서 로드했던 Animation 개수 사용
	m_iNumAnimation = pModel->Get_NumAnimations();

	for (_uint i = 0; i < m_iNumAnimation; ++i)
	{			
		// 1. Animation 이름			
		::ReadFile(hFile, szBuf, BUF_MAX, &dwByte, nullptr);
		
		// 2. Animation Duration
		_double Duration = 0;
		::ReadFile(hFile, &Duration, sizeof(_double), &dwByte, nullptr);

		// 3. Animation TickPerSecond
		_double TickPerSec = 0;
		::ReadFile(hFile, &TickPerSec, sizeof(_double), &dwByte, nullptr);

		CAnimation* pAnimation = CAnimation::Create(szBuf, Duration, TickPerSec);
		if (nullptr == pAnimation)
			return E_FAIL;

		// 4. Channel 개수
		_uint iNumAnimChannel = 0;
		::ReadFile(hFile, &iNumAnimChannel, sizeof(_uint), &dwByte, nullptr);
		
		for (_uint j = 0; j < iNumAnimChannel; ++j)
		{
			// 5. Channel 이름			
			::ReadFile(hFile, szBuf2, BUF_MAX, &dwByte, nullptr);
			CChannel* pChannel = CChannel::Create(szBuf2);
			if (nullptr == pChannel)
				return E_FAIL;
							
			// 6. Channel KeyFrame 사이즈
			_uint iNumKeyFrames = 0;
			::ReadFile(hFile, &iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);			

			// 7. KeyFrame
			for(_uint k = 0; k < iNumKeyFrames; ++k)
			{
				KEYFRAME* pKeyFrame = new KEYFRAME;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				::ReadFile(hFile, &pKeyFrame->vScale, sizeof(_float3), &dwByte, nullptr);
				::ReadFile(hFile, &pKeyFrame->vRotation, sizeof(_float4), &dwByte, nullptr);
				::ReadFile(hFile, &pKeyFrame->vPosition, sizeof(_float3), &dwByte, nullptr);
				::ReadFile(hFile, &pKeyFrame->Time, sizeof(_double), &dwByte, nullptr);

				pChannel->Add_KeyFrame(pKeyFrame);
			}
			pAnimation->Add_Channels(pChannel);
		}
		(*m_Animations).push_back(pAnimation);
	}


	for (_uint i = 0; i < m_iNumAnimation; ++i)
	{
		vector<class CChannel*>* pChannels = (*m_Animations)[i]->Get_Channels();

		for (auto& pChannel : *pChannels)
		{
			CHierarchyNode* pHierarchyNode = Find_HierarchyNode(pChannel->Get_Name());
			if (nullptr == pHierarchyNode)
				return E_FAIL;

			pHierarchyNode->Add_Channel(i, pChannel);
		}
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CAnimator::Save_Animations(class CModel* pModel)
{
	//---------------------------------------------------------------------
	// 파일 생성(오픈) 할때 최초 Load 시 사용한 fbx의 이름으로 저장하게 함
	//---------------------------------------------------------------------
	char		szFullPath[BUF_MAX] = "../../Reference/Res_Client/Resources/ModelSaveDatas/";
	char		szPath[BUF_MAX] = "";
	strcpy_s(szPath, szFullPath);
	strcat_s(szPath, pModel->Get_ModelName());

	//폴더가 없다는 것이기 때문에 모델명으로 폴더 생성
	if (-1 == _access(szPath, 0))
		mkOnedir(szPath);

	char		szMeshConPath[BUF_MAX] = "";
	strcpy_s(szMeshConPath, szFullPath);
	strcat_s(szMeshConPath, pModel->Get_ModelName());
	strcat_s(szMeshConPath, "/");
	strcat_s(szMeshConPath, "Animations/");

	// 모델 폴더 안에 인자로 들어온 이름과 같은 폴더가 없다는 것이기 때문에 해당 폴더 이름으로 폴더 생성
	if (-1 == _access(szMeshConPath, 0))
		mkOnedir(szMeshConPath);

	// 모델 이름으로 .data 파일 생성
	strcat_s(szFullPath, pModel->Get_ModelName());
	strcat_s(szFullPath, "/");
	strcat_s(szFullPath, "Animations/");
	strcat_s(szFullPath, pModel->Get_ModelName());
	strcat_s(szFullPath, ".dat");
	_tchar		szPerfectPath[BUF_MAX] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, BUF_MAX);
	HANDLE hFile = CreateFile(szPerfectPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	//--------------------------------------------------------------
	// 애니메이션 Write
	//--------------------------------------------------------------
	_ulong			dwByte = 0;
	char			szBuf[BUF_MAX] = "";
	char			szBuf2[BUF_MAX] = "";
	
	for(auto& Anim : (*m_Animations))
	{
		// 1. Animation 이름	
		strcpy_s(szBuf, BUF_MAX, Anim->Get_Name());
		::WriteFile(hFile, szBuf, BUF_MAX, &dwByte, nullptr);

		// 2. Animation Duration
		_double Duration = Anim->Get_Duration();
		::WriteFile(hFile, &Duration, sizeof(_double), &dwByte, nullptr);

		// 3. Animation TickPerSecond
		_double TickPerSec = Anim->Get_TickPerSecond();
		::WriteFile(hFile, &TickPerSec, sizeof(_double), &dwByte, nullptr);

		// 4. Channel 개수
		vector<class CChannel*>* pChannels = Anim->Get_Channels();
		_uint iNumAnimChannel = _uint(pChannels->size());
		::WriteFile(hFile, &iNumAnimChannel, sizeof(_uint), &dwByte, nullptr);
		
		for(auto& Channel : *pChannels)
		{
			// 5. Channel 이름
			strcpy_s(szBuf2, BUF_MAX, Channel->Get_Name());
			::WriteFile(hFile, szBuf2, BUF_MAX, &dwByte, nullptr);

			// 6. Channel KeyFrame 사이즈
			_uint iNumKeyFrames = _uint(Channel->Get_KeyFrames()->size());
			::WriteFile(hFile, &iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);
			
			const vector<KEYFRAME*>* KeyFrames = Channel->Get_KeyFrames();

			// 7. KeyFrame
			for(auto& KeyFrame : *KeyFrames)
			{
				::WriteFile(hFile, &KeyFrame->vScale, sizeof(_float3), &dwByte, nullptr);
				::WriteFile(hFile, &KeyFrame->vRotation, sizeof(_float4), &dwByte, nullptr);
				::WriteFile(hFile, &KeyFrame->vPosition, sizeof(_float3), &dwByte, nullptr);
				::WriteFile(hFile, &KeyFrame->Time, sizeof(_double), &dwByte, nullptr);
			}
		}
	}

	CloseHandle(hFile);
	return S_OK;
}

void CAnimator::Pause_Animation(_bool Pause)
{
	(*m_Animations)[m_iCurrentAnimIndex]->Pause_Animation(Pause);
}

const _bool CAnimator::Get_Pause()
{
	return (*m_Animations)[m_iCurrentAnimIndex]->Get_Pause();
	// // O: 여기에 return 문을 삽입합니다.
}



CAnimator * CAnimator::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CAnimator*	pInstance = new CAnimator(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed To Creating CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CAnimator::Clone(void * pArg)
{
	CAnimator*	pInstance = new CAnimator(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimator::Free()
{
	__super::Free();

// 	for (auto& pAnimation : (*m_Animations))
// 		Safe_Release(pAnimation);
// 
// 	(*m_Animations).clear();

}

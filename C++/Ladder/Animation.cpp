#include "..\Public\Animation.h"

#include "Sound_Manager.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_Duration(rhs.m_Duration)
	, m_fTimeAcc(rhs.m_fTimeAcc)
	, m_isFinished(rhs.m_isFinished)
	, m_TickPerSecond(rhs.m_TickPerSecond)
	, m_OriginTickPerSecond(rhs.m_OriginTickPerSecond)
	, m_iCurrentKeyFrameIndex(rhs.m_iCurrentKeyFrameIndex)
	, m_iCurrentMaxKeyFrameIndex(rhs.m_iCurrentMaxKeyFrameIndex)
	, m_bIsPause(rhs.m_bIsPause)
	, m_AnimationSound(rhs.m_AnimationSound)
	, m_SoundBlockCount(rhs.m_SoundBlockCount)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pPrototypeChannel : rhs.m_Channels)
	{
		m_Channels.push_back(pPrototypeChannel->Clone());
	}	
}

void CAnimation::Set_InitAnim()
{
  	m_fTimeAcc = 0.f;
	m_isFinished = false;
	m_bIsPause = false;
	m_iCurrentKeyFrameIndex = 0;
	m_iCurrentMaxKeyFrameIndex = 0;

	m_AnimationSound = false;

	int i = 0;
	for (auto& pChannel : m_Channels)
	{
		vector<KEYFRAME*>	KeyFrames = *pChannel->Get_KeyFrames();

		m_vScale = XMLoadFloat3(&KeyFrames.front()->vScale);
		m_vRotation = XMLoadFloat4(&KeyFrames.front()->vRotation);
		m_vPosition = XMLoadFloat3(&KeyFrames.front()->vPosition);
		m_vPosition = XMVectorSetW(m_vPosition, 1.f);

		pChannel->Set_KeyFrameIndex(0);

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(m_vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), m_vRotation, m_vPosition);

		pChannel->Set_TransformationMatrix(TransformationMatrix);
		pChannel->Set_Scale(m_vScale);
		pChannel->Set_Rotation(m_vRotation);
		pChannel->Set_Position(m_vPosition);

		++i;
	}

}

void CAnimation::Set_BeforeKeyFrame(vector<KEYFRAME>* pKeyFrames)
{
	m_fTimeAcc = 0.f;
	m_isFinished = false;
	//m_AnimationSound = false;

	int i = 0;
	for (auto& pChannel : m_Channels)
	{
		vector<KEYFRAME*>	KeyFrames = *pChannel->Get_KeyFrames();

		XMStoreFloat3(&(*pKeyFrames)[i].vScale, pChannel->Get_Scale());
		XMStoreFloat4(&(*pKeyFrames)[i].vRotation, pChannel->Get_Rotation());
		XMStoreFloat3(&(*pKeyFrames)[i].vPosition, pChannel->Get_Position());

		++i;
	}

}

void CAnimation::Set_AnimationSound(_bool AnimationSound)
{
	m_AnimationSound = AnimationSound;
}

_bool CAnimation::PlayAnimationFrameSound(_uint Frame, _uint ResetFrame, TCHAR* pSoundKey, _uint SoundID, _uint eID , _uint SoundCount, _float SoundVolume, class CTransform* pTrans)
{
	if (Frame <= m_iCurrentMaxKeyFrameIndex && m_iCurrentMaxKeyFrameIndex <= ResetFrame)
	{
		if (m_AnimationSound)
			return false;

		CSound_Manager* SoundManager = GET_INSTANCE(CSound_Manager);
		if (SoundID == SINGLE)
		{
			SoundManager->StopSound(SoundID, eID);
			if (nullptr == pTrans)
			{
				SoundManager->Play(pSoundKey, SINGLE, eID, SoundCount);
				SoundManager->Set_Volume(SINGLE, eID, SoundVolume);
			}
			else // Distance ����϶��� �Ÿ������� �Ҹ�0~1�����ǰ� �س���
				SoundManager->Play(pSoundKey, SINGLE, eID, SoundCount, pTrans);
		}
		else
		{			
			_uint ID = 0;
			if (nullptr == pTrans)
			{
				ID = SoundManager->Play(pSoundKey, MULTY, eID, SoundCount);
				SoundManager->Set_Volume(MULTY, ID, SoundVolume);
			}
			else // Distance ����϶��� �Ÿ������� �Ҹ�0~1�����ǰ� �س���
				ID = SoundManager->Play(pSoundKey, MULTY, eID, SoundCount, pTrans);			
		}
		m_SoundBlockCount = ResetFrame;
		m_AnimationSound = true;
		RELEASE_INSTANCE(CSound_Manager);
		return true;
	}
	return false;
}

HRESULT CAnimation::NativeConstruct(char * pName, _double Duration, _double TickPerSecond)
{
	strcpy_s(m_szName, pName);
	m_Duration = Duration;
	m_TickPerSecond = TickPerSecond;
	m_OriginTickPerSecond = TickPerSecond;

	return S_OK;
}

HRESULT CAnimation::Update_TransformationMatrix(_float fTimeDelta, _bool isLoop, _bool* bRootNodeSetup)
{
	if (!m_bIsPause)
	m_fTimeAcc += (_float)m_TickPerSecond * fTimeDelta;

	if (m_fTimeAcc >= m_Duration)
	{
		m_isFinished = true;
		
		if (true == isLoop)
		{
			m_fTimeAcc = 0.f;
		}
	}


	if (m_fTimeAcc < 0.f)
	{
		if (isLoop)//�����϶���
		{
			m_isFinished = false;
			m_fTimeAcc = (_float)m_Duration + m_fTimeAcc;
		}
		else
		{
			m_isFinished = true;
			m_fTimeAcc = 0.f;
		}
	}

	/* ä�ε��� Ű�����ӵ��� ������ �����ϱ�. ������ͼ� ���� ����� �����. . */
	/* ����� �ð��� �´� ���� ����� ä�ο� �������ַ���. */
	for (auto& pChannel : m_Channels)
	{
		vector<KEYFRAME*>	KeyFrames = *pChannel->Get_KeyFrames();

		m_iCurrentKeyFrameIndex = pChannel->Get_KeyFrameIndex();

		if (true == m_isFinished)
		{
			m_iCurrentKeyFrameIndex = 0;
			m_iCurrentMaxKeyFrameIndex = 0;
			pChannel->Set_KeyFrameIndex(m_iCurrentKeyFrameIndex);
		}

		if (m_fTimeAcc >= KeyFrames.back()->Time)
		{
			m_vScale = XMLoadFloat3(&KeyFrames.back()->vScale);
			m_vRotation = XMLoadFloat4(&KeyFrames.back()->vRotation);
			m_vPosition = XMLoadFloat3(&KeyFrames.back()->vPosition);
			m_vPosition = XMVectorSetW(m_vPosition, 1.f);
		}

		else /* Ű�����ӻ��̿� ������ ���� ��������� ������������ ������. */
		{		
			while (m_fTimeAcc > KeyFrames[m_iCurrentKeyFrameIndex + 1]->Time)
			{
				pChannel->Set_KeyFrameIndex(++m_iCurrentKeyFrameIndex);
			}
			while (m_fTimeAcc < KeyFrames[m_iCurrentKeyFrameIndex]->Time && m_iCurrentKeyFrameIndex != 0)
			{
				pChannel->Set_KeyFrameIndex(--m_iCurrentKeyFrameIndex);
			}
			_float		fRatio = (_float)((m_fTimeAcc - KeyFrames[m_iCurrentKeyFrameIndex]->Time) /
				(KeyFrames[m_iCurrentKeyFrameIndex + 1]->Time - KeyFrames[m_iCurrentKeyFrameIndex]->Time));

			m_vScale = XMVectorLerp(XMLoadFloat3(&KeyFrames[m_iCurrentKeyFrameIndex]->vScale),
				XMLoadFloat3(&KeyFrames[m_iCurrentKeyFrameIndex + 1]->vScale), fRatio);

			m_vRotation = XMQuaternionSlerp(XMLoadFloat4(&KeyFrames[m_iCurrentKeyFrameIndex]->vRotation),
				XMLoadFloat4(&KeyFrames[m_iCurrentKeyFrameIndex + 1]->vRotation), fRatio);

			m_vPosition = XMVectorLerp(XMLoadFloat3(&KeyFrames[m_iCurrentKeyFrameIndex]->vPosition),
				XMLoadFloat3(&KeyFrames[m_iCurrentKeyFrameIndex + 1]->vPosition), fRatio);

			m_vPosition = XMVectorSetW(m_vPosition, 1.f);
		}

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(m_vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), m_vRotation, m_vPosition);

		pChannel->Set_TransformationMatrix(TransformationMatrix);
		pChannel->Set_Scale(m_vScale);
		pChannel->Set_Rotation(m_vRotation);
		pChannel->Set_Position(m_vPosition);

		if (m_iCurrentMaxKeyFrameIndex < m_iCurrentKeyFrameIndex)
			m_iCurrentMaxKeyFrameIndex = m_iCurrentKeyFrameIndex;
	}


	//�ƽ�Ű�������� ���� ������ �������� Ŭ �� �ִϸ��̼��� �޽��� �ٲٴ� �ڵ尡 �ʿ��Ѱ�?
	if (m_AnimationSound && m_SoundBlockCount < m_iCurrentMaxKeyFrameIndex)
	{
		m_AnimationSound = false;
	}

	if (true == m_isFinished)
	{
		*bRootNodeSetup = true;

		if (true == isLoop)
			m_isFinished = false;
	}

	return S_OK;
}

void CAnimation::Pause_Animation(_bool Pause)
{
	m_bIsPause = Pause;
}

const _bool& CAnimation::Get_Pause()
{
	return m_bIsPause;
	// // O: ���⿡ return ���� �����մϴ�.
}


CAnimation * CAnimation::Create(char * pName, _double Duration, _double TickPerSecond)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct(pName, Duration, TickPerSecond)))
	{
		MSG_BOX("Failed To Creating CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation * CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
	
}

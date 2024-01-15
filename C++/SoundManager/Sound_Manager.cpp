#include "..\Public\Sound_Manager.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
{
	m_vecChannelArr.resize(SOUNDID_END);
	m_vecChannelArr[0].resize(30);
	m_vecChannelArr[1].resize(MAX_CHANNEL);
	FMOD_System_Create(&m_pSystem);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, MAX_CHANNEL + 30 , FMOD_INIT_NORMAL, NULL);
	
	_LoadSoundFile();
}

void CSound_Manager::Set_MasterVolume(_float _vol)
{
	m_volume = _vol;
	for (_uint i = 0; i < SOUNDID_END; ++i)
	{
		for (auto& Channel : m_vecChannelArr[i])
		{
			FMOD_BOOL Bool;
			if (!FMOD_Channel_IsPlaying(Channel.channel, &Bool))
			{
				FMOD_Channel_SetVolume(Channel.channel, Channel.vloume * m_volume);
			}
		}
	}
}

void CSound_Manager::VolumeUp(_uint SoundID, _uint eID, _float _vol)
{
	CHANNEL& Channel = m_vecChannelArr[SoundID][eID];

	if (Channel.vloume < SOUND_MAX) {
		Channel.vloume += _vol;
		if (Channel.vloume > SOUND_MAX)
		{
			Channel.vloume = SOUND_MAX;
		}
	}	
	FMOD_Channel_SetVolume(Channel.channel, Channel.vloume * m_volume);

}

void CSound_Manager::VolumeDown(_uint SoundID, _uint eID, _float _vol)
{
	CHANNEL& Channel = m_vecChannelArr[SoundID][eID];
	if (Channel.vloume > SOUND_MIN) {
		Channel.vloume -= _vol;
		if (Channel.vloume < SOUND_MIN)
		{
			Channel.vloume = SOUND_MIN;
		}
	}

	FMOD_Channel_SetVolume(Channel.channel, Channel.vloume * m_volume);
}

void CSound_Manager::Set_Volume(_uint SoundID, _uint eID, _float _vol)
{
	CHANNEL& Channel = m_vecChannelArr[SoundID][eID];
	if (_vol > SOUND_MIN && _vol < SOUND_MAX) {
		Channel.vloume = _vol;
	}
	FMOD_Channel_SetVolume(Channel.channel, Channel.vloume * m_volume);
}

void CSound_Manager::Set_Pan(_uint SoundID, _uint eID, _float _pan)
{
	CHANNEL& Channel= m_vecChannelArr[SoundID][eID];

	if (_pan <= SOUND_PAN_MIN)
	{
		FMOD_Channel_SetPan(Channel.channel, SOUND_PAN_MIN);
	}
	else if (_pan >= SOUND_PAN_MAX)
	{
		FMOD_Channel_SetPan(Channel.channel, SOUND_PAN_MAX);
	}
	else
	{
		FMOD_Channel_SetPan(Channel.channel, _pan);
	}
}

void CSound_Manager::Pause(_uint SoundID, _uint eID, _bool _Pause)
{
	m_bPause = _Pause;
	FMOD_Channel_SetPaused(m_vecChannelArr[SoundID][eID].channel, m_bPause);
}

void CSound_Manager::PlayBGM(TCHAR* pSoundKey, _uint eID)
{	
	CHANNEL& Channel = m_vecChannelArr[SINGLE][eID];
	wstring SoundKey = pSoundKey;
	SoundKey += L".wav";
	Channel.vloume = 0.5f;
	FMOD_System_PlaySound(m_pSystem, m_mapSound[SoundKey], nullptr, FALSE, &Channel.channel);
	FMOD_Channel_SetMode(Channel.channel, FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(Channel.channel, Channel.vloume * m_volume);
	FMOD_System_Update(m_pSystem);
}

_uint CSound_Manager::Play(TCHAR* pSoundKey, _uint SoundID, _uint eID, _uint SoundCount)
{
	wstring SoundKey = pSoundKey;
	if (SoundCount > 1)
	{
		_uint i = (rand() % SoundCount) + 1;
		//언더바까지는 붙이게 하자 그게나을듯.
		/*SoundKey += L"_";*/
		SoundKey += i < 10 ? L"0" : L"";
		SoundKey += to_wstring(i);
	}
	SoundKey += L".wav";

	if (SoundID == SINGLE)
	{
		FMOD_BOOL bPlay = FALSE;
		CHANNEL& Channel = m_vecChannelArr[SoundID][eID];
		if (FMOD_Channel_IsPlaying(Channel.channel, &bPlay))
		{
			FMOD_SOUND* a = m_mapSound[SoundKey];

			Channel.vloume = 0.5f;
			FMOD_System_PlaySound(m_pSystem, m_mapSound[SoundKey], nullptr, FALSE, &Channel.channel);
			FMOD_Channel_SetVolume(Channel.channel, Channel.vloume * m_volume);
		}
		FMOD_System_Update(m_pSystem);
		return eID;
	}
	else
	{
		_uint ID = MultySoundCheckPlay(m_mapSound[SoundKey], eID);
		FMOD_System_Update(m_pSystem);

		return ID;
	}
}


_uint CSound_Manager::Play(TCHAR* pSoundKey, _uint SoundID, _uint eID, _uint SoundCount, class CTransform* pTrans)
{
	if (nullptr == pTrans)
		return 0;
	
	if(Compute_Distance(pTrans))
	{
		wstring SoundKey = pSoundKey;
		if (SoundCount > 1)
		{
			_uint i = (rand() % SoundCount) + 1;
			//언더바까지는 붙이게 하자 그게나을듯.
			/*SoundKey += L"_";*/
			SoundKey += i < 10 ? L"0" : L"";
			SoundKey += to_wstring(i);
		}
		SoundKey += L".wav";

		if (SoundID == SINGLE)
		{
			FMOD_BOOL bPlay = FALSE;
			CHANNEL& Channel = m_vecChannelArr[SoundID][eID];
			if (FMOD_Channel_IsPlaying(Channel.channel, &bPlay))
			{
				FMOD_SOUND* a = m_mapSound[SoundKey];
				FMOD_System_PlaySound(m_pSystem, m_mapSound[SoundKey], nullptr, FALSE, &Channel.channel);
			}
			FMOD_Channel_SetVolume(Channel.channel, m_fDistVolume * m_volume);
			FMOD_System_Update(m_pSystem);
			return eID;
		}
		else
		{
			// Default값 0.5f라서 그전에 짜놨던거 그대로고
			// 볼륨 입력값 넣어줬을때만 Distance기반으로 0~1 소리나게 해놨음
			_uint ID = MultySoundCheckPlay(m_mapSound[SoundKey], eID, m_fDistVolume);

			_Vector4 ListenerPos = m_pListenerTrans->Get_State(STATE_POSITION);
			_Vector4 AudioPos = pTrans->Get_State(STATE_POSITION);
			_Vector3 dir = XMVector3Normalize((AudioPos - ListenerPos));
			FMOD_Channel_SetPan(m_vecChannelArr[MULTY][eID].channel, dir.x);

			FMOD_System_Update(m_pSystem);
			return ID;
		}
	}

	return 0;
}

_uint CSound_Manager::MultySoundCheckPlay(FMOD_SOUND* Sound, _uint eID, _float Volume)
{
	FMOD_BOOL bPlay = FALSE;
	CHANNEL& Channel = m_vecChannelArr[MULTY][eID];
	if (FMOD_Channel_IsPlaying(Channel.channel, &bPlay))
	{
		Channel.vloume = Volume;
		FMOD_System_PlaySound(m_pSystem, Sound, nullptr, FALSE, &Channel.channel);
		FMOD_Channel_SetVolume(Channel.channel, Channel.vloume * m_volume);
		return eID;
	}
	else
	{
		_uint ID = eID + 1;
		if ((_uint)ID == MAX_CHANNEL)
		{
			MSG_BOX("Need : Engine -> Enigne_Enum -> MAX_CHANNEL - INCREASE");
			return MAX_CHANNEL;
		}
		return MultySoundCheckPlay(Sound, ID, Volume);
	}
}

void CSound_Manager::StopSound(_uint SoundID, _uint eID)
{
	FMOD_Channel_Stop(m_vecChannelArr[SoundID][eID].channel);
}

void CSound_Manager::StopAll()
{
	for (auto& ChannelArr : m_vecChannelArr)
	{
		for (auto& Channel : ChannelArr)
			FMOD_Channel_Stop(Channel.channel);
	}
}

void CSound_Manager::StopMultySound()
{
	for (auto& Channel : m_vecChannelArr[MULTY])
		FMOD_Channel_Stop(Channel.channel);
}

bool CSound_Manager::GetIsPlaying(_uint eID, _uint SoundID)
{
	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_vecChannelArr[eID][(_uint)SoundID].channel, &bPlay))
		return false;
	else
		FMOD_System_Update(m_pSystem);

	return true;
}

_bool CSound_Manager::Compute_Distance(CTransform* pTrans)
{
	if (nullptr == m_pListenerTrans || nullptr == pTrans)
		return false;

	_Vector4 ListenerPos = m_pListenerTrans->Get_State(STATE_POSITION);
	_Vector4 AudioPos = pTrans->Get_State(STATE_POSITION);

	_float fDist = XMVectorGetX(XMVector3Length(ListenerPos - AudioPos));		

	if (m_fDistance >= fDist)
	{
		m_fDistVolume = 0.5f - (fDist / m_fDistance); // 0~1사이로 나옴

		m_fDistVolume =  Clamp(m_fDistVolume, 0.f, 0.5f);

		return true; // 사정거리 안에 있으면 Play가능하게 True 반환
	}
	else
		return false; // 사정거리 밖에 있으면 Play못하게 False 반환
}

void CSound_Manager::_LoadSoundFile()
{
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../../Reference/Sound/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Reference/Sound/";
	char szFullPath[128] = "";
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
		if (eRes == FMOD_OK)
		{
			size_t iLength = strlen(szFilename) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			MultiByteToWideChar(CP_ACP, 0, szFilename, (int)iLength, pSoundKey, (int)iLength);
			wstring SoundKey = pSoundKey;
			Safe_Delete_Array(pSoundKey);
			m_mapSound.emplace(SoundKey, pSound);
		}
		iResult = _tfindnext64(handle, &fd);
	}
	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		//delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();
	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

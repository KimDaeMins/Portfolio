#include "stdafx.h"
#include "WaveManager.h"
#include "GameInstance.h"
#include "WaveMap_Helix.h"
#include "DropItem.h"
IMPLEMENT_SINGLETON(CWaveManager)
CWaveManager::CWaveManager()
{

}
void CWaveManager::Start()//��ŸƮ�ϱ����� ���彺������ ����ߵ�.
{
	m_bIsWaveStart = true;
	m_bIsWaveEnd = false;
	m_fDisPlayTime = 0.f;
	m_pCurWaveData = m_pWaveDatas.front();
}

_bool CWaveManager::Tick(float fTimeDelta)
{
	if (m_bIsWaveStart)//���̺갡 ���۵ƴٸ�?
	{
		if (m_pCurWaveData)//������̺갡 �ִٸ�(��ŸƮ ������ ����Ÿ ����Ʈ �ְ�����)
		{
			m_pCurWaveData->Update_SpwanEvent(fTimeDelta);//����������Ʈ - �����̴ٳ����� �����ִ� �༮�� �Ǵ��ؼ� Ʈ���޽��� ��ȯ��.
			m_fDisPlayTime += fTimeDelta;
			if (m_pCurWaveData->NextWaveReady)//��罺���� ������ ���� ���Ͱ� 2���� ���϶��
			{
				Go_NextLevel();
				m_fDisPlayTime = 0.f;
				return true;
			}


			if (m_bIsDisPlay)//���̺� ���������� �����ִٸ�?(���� ���ͼ��� �������)
			{
				if (m_fDisPlayTime >= m_fDisPlayMaxTime)//�������� ���ð��� �ƽ�Ÿ�Ӻ��� ũ�ٸ� ���̺극���Ѿ�� �ð�üũ
				{
					Go_NextLevel();
					m_fDisPlayTime = 0.f;
					return true;
				}
			}

		}
		else//���� ���̺� ����Ÿ�� ����� 0�϶� Ŀ��Ʈ���̺굥��Ÿ�� nullptr�� �Ǵϱ� ����ε�
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			//Layer_Helix
			if (pGameInstance->Get_LayerSize(L"Layer_Wave") <= 0 && pGameInstance->Get_LayerSize(L"Layer_Helix") <= 0)//���̺�ȿ� ����Ÿ�� ������
			{
				m_bIsWaveEnd = true;
				m_bIsWaveStart = false;
			}
			RELEASE_INSTANCE(CGameInstance);
			return m_bIsWaveEnd;//
		}
	}
	return false;
}

void CWaveManager::Go_NextLevel()
{
	Destroy_WaveData(m_pCurWaveData);
	m_pWaveDatas.pop_front();
	if (m_pWaveDatas.size() > 0)
	{
		m_pCurWaveData = m_pWaveDatas.front();
	}
}

_bool CWaveManager::Get_WaveEnd()
{
	return m_bIsWaveEnd;
}

_bool CWaveManager::IsIn(const _Vector3& position)
{
	float length = position.Length();
	if (length <= 28)
		return true;
	else
		return false;
}

void CWaveManager::Add_Stage(const _tchar* objTag, int iStepMaxIndex, float fSwawnMaxTime, int objSpwanCount)
{
	WAVEDATA* pData = new WAVEDATA;
	lstrcpy(pData->swObjTag, objTag);

	pData->tUserData.fDamage = 5;
	pData->tUserData.fHp = 20.f;
	pData->iStepMaxIndex = iStepMaxIndex;
	pData->iObjSpwanCount = objSpwanCount;
	pData->iStepIndex = 0;
	pData->fSpawnMaxTime = fSwawnMaxTime;
	pData->fSpawnTime = 0;
	pData->ePositionType = SPAWN_POSITION_SHAPE::RANDOM;
	m_pWaveDatas.push_back(pData);
}

void CWaveManager::Add_Stage(const WAVEUSERDATA& tMonsterData, const _tchar* objTag, int iStepMaxIndex, float fSwawnMaxTime, int objSpwanCount)
{
	WAVEDATA* pData = new WAVEDATA;
	lstrcpy(pData->swObjTag, objTag);
	pData->tUserData.iModelType = tMonsterData.iModelType;
	pData->tUserData.fDamage = tMonsterData.fDamage;
	pData->tUserData.fHp = tMonsterData.fHp;
	pData->iStepMaxIndex = iStepMaxIndex;
	pData->iObjSpwanCount = objSpwanCount;
	pData->iStepIndex = 0;
	pData->fSpawnMaxTime = fSwawnMaxTime;
	pData->fSpawnTime = 0;
	pData->ePositionType = SPAWN_POSITION_SHAPE::RANDOM;
	m_pWaveDatas.push_back(pData);
}

void CWaveManager::Add_Stage(const _tchar* objTag, SPAWN_POSITION_SHAPE ePositionType)
{
	WAVEDATA* pData = new WAVEDATA;
	lstrcpy(pData->swObjTag, objTag);
	pData->tUserData.fDamage = 5;
	pData->tUserData.fHp = 10.f;
	pData->ePositionType = ePositionType;
	switch (ePositionType)
	{
	case Client::SPAWN_POSITION_SHAPE::RANDOM:
	{
		pData->iStepMaxIndex = 1;
		pData->iObjSpwanCount = 1;
		pData->iStepIndex = 0;
		pData->fSpawnMaxTime = 0.f;
		pData->fSpawnTime = 0;
		break;
	}
	case Client::SPAWN_POSITION_SHAPE::BESIDE:
	{
		pData->iStepMaxIndex = 1;
		pData->iObjSpwanCount = 2;
		pData->iStepIndex = 0;
		pData->fSpawnMaxTime = 0.f;
		pData->fSpawnTime = 0;
		break;
	}
	case Client::SPAWN_POSITION_SHAPE::RECTANGLE:
	{
		pData->iStepMaxIndex = 1;
		pData->iObjSpwanCount = 4;
		pData->iStepIndex = 0;
		pData->fSpawnMaxTime = 0.f;
		pData->fSpawnTime = 0;
		break;
	}
	case Client::SPAWN_POSITION_SHAPE::DIAMOND:
	{
		pData->iStepMaxIndex = 1;
		pData->iObjSpwanCount = 4;
		pData->iStepIndex = 0;
		pData->fSpawnMaxTime = 0.f;
		pData->fSpawnTime = 0;
		break;
	}
	case Client::SPAWN_POSITION_SHAPE::CYCLE:
	{
		pData->iStepMaxIndex = 1;
		pData->iObjSpwanCount = 16;
		pData->iStepIndex = 0;
		pData->fSpawnMaxTime = 0.f;
		pData->fSpawnTime = 0;
		break;
	}
	default:
		break;
	}

	m_pWaveDatas.push_back(pData);
}

void CWaveManager::Add_Stage(const WAVEUSERDATA& tMonsterData, const _tchar* objTag, SPAWN_POSITION_SHAPE ePositionType)
{
	WAVEDATA* pData = new WAVEDATA;
	lstrcpy(pData->swObjTag, objTag);
	pData->tUserData.iModelType = tMonsterData.iModelType;
	pData->tUserData.fDamage = tMonsterData.fDamage;
	pData->tUserData.fHp = tMonsterData.fHp;
	pData->ePositionType = ePositionType;
	switch (ePositionType)
	{
	case Client::SPAWN_POSITION_SHAPE::RANDOM:
	{
		pData->iStepMaxIndex = 1;
		pData->iObjSpwanCount = 1;
		pData->iStepIndex = 0;
		pData->fSpawnMaxTime = 0.f;
		pData->fSpawnTime = 0;
		break;
	}
	case Client::SPAWN_POSITION_SHAPE::BESIDE:
	{
		pData->iStepMaxIndex = 1;
		pData->iObjSpwanCount = 2;
		pData->iStepIndex = 0;
		pData->fSpawnMaxTime = 0.f;
		pData->fSpawnTime = 0;
		break;
	}
	case Client::SPAWN_POSITION_SHAPE::RECTANGLE:
	{
		pData->iStepMaxIndex = 1;
		pData->iObjSpwanCount = 4;
		pData->iStepIndex = 0;
		pData->fSpawnMaxTime = 0.f;
		pData->fSpawnTime = 0;
		break;
	}
	case Client::SPAWN_POSITION_SHAPE::DIAMOND:
	{
		pData->iStepMaxIndex = 1;
		pData->iObjSpwanCount = 4;
		pData->iStepIndex = 0;
		pData->fSpawnMaxTime = 0.f;
		pData->fSpawnTime = 0;
		break;
	}
	case Client::SPAWN_POSITION_SHAPE::CYCLE:
	{
		pData->iStepMaxIndex = 1;
		pData->iObjSpwanCount = 16;
		pData->iStepIndex = 0;
		pData->fSpawnMaxTime = 0.f;
		pData->fSpawnTime = 0;
		break;
	}
	default:
		break;
	}

	m_pWaveDatas.push_back(pData);
}

void CWaveManager::Add_ItemStage(int iStepMaxIndex, float fSwawnMaxTime, int objSpwanCount)
{
	WAVEDATA* pData = new WAVEDATA;
	pData->bItem = true;
	pData->iStepMaxIndex = iStepMaxIndex;
	pData->fSpawnMaxTime = fSwawnMaxTime;
	pData->iObjSpwanCount = objSpwanCount;
	pData->fSpawnTime = 0;
	pData->iStepIndex = 0;
	pData->ePositionType = SPAWN_POSITION_SHAPE::RANDOM;
	m_pWaveDatas.push_back(pData);
}

_Vector3* CWaveManager::Get_TeleportPoint()
{
	if (m_iTeleportCount == -1)
	{
		Set_ParsingPoints();
		m_iTeleportCount++;
	}

	if (m_iTeleportCount >= m_pParsingPoints.size())
		return nullptr;
	else
	{
		_Vector3* point = m_pParsingPoints[m_iTeleportCount];
		m_iTeleportCount++;
  		return point;
	}
}

_Vector3* CWaveManager::Get_MonsterRandomPoint()
{
	if (m_iMonsterRandomSpawnCount == -1)
	{
		Set_ParsingPoints();
		m_iMonsterRandomSpawnCount++;
	}
	if (m_iMonsterRandomSpawnCount >= m_pParsingPoints.size())
	{
		Set_ParsingPoints();
		m_iMonsterRandomSpawnCount = 0;
	}
	_Vector3* point = m_pParsingPoints[m_iMonsterRandomSpawnCount];
	m_iMonsterRandomSpawnCount++;
	return point;
}

_Vector3* CWaveManager::Get_BurgerParsingPoint()
{
	if (m_iBurgerPointCount == -1)
	{
		Set_BurgerParsingPoints();
		m_iBurgerPointCount++;
	}
	if (m_iBurgerPointCount >= m_pBurgetParsingPoints.size())
	{
		Set_BurgerParsingPoints();
		m_iBurgerPointCount = 0;
	}
	_Vector3* point = m_pBurgetParsingPoints[m_iBurgerPointCount];
	m_iBurgerPointCount++;
	return point;
}

void CWaveManager::Destroy_WaveData(WAVEDATA*& pObj)
{
	if (pObj)
	{
		delete pObj;
		pObj = nullptr;
	}
}

void CWaveManager::Set_ParsingPoints()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	TRANSFORMINFO pTransInfo;
	_uint teleportPointSize = pGameInstance->Get_TableSize(TEXT("TeleportPoint"), DATATYPE_OBJECT);

	if (m_pParsingPoints.empty())
	{
		for (_uint i = 0; i < teleportPointSize; ++i)
		{
			ZeroMemory(&pTransInfo, sizeof(TRANSFORMINFO));
			pTransInfo = *pGameInstance->Get_RowDataToTransform(TEXT("TeleportPoint"), i + 1, DATATYPE_OBJECT);
			m_pParsingPoints.push_back(new _Vector3(pTransInfo.PosX, pTransInfo.PosY, pTransInfo.PosZ));
		}
	}
	for (int i = 0; i < 30; ++i)
	{
		_Vector3* teleportData = m_pParsingPoints[pGameInstance->Random_Range(0, teleportPointSize)];
		_Vector3* teleportData_2 = m_pParsingPoints[pGameInstance->Random_Range(0, teleportPointSize)];
		if (teleportData == teleportData_2)
			continue;
		else
		{
			_Vector3* tmp = teleportData;
			teleportData = teleportData_2;
			teleportData_2 = tmp;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CWaveManager::Set_BurgerParsingPoints()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	TRANSFORMINFO pTransInfo;
	_uint burgerSize = pGameInstance->Get_TableSize(TEXT("Burger"), DATATYPE_OBJECT);

	if (m_pBurgetParsingPoints.empty())
	{
		for (_uint i = 0; i < burgerSize; ++i)
		{
			ZeroMemory(&pTransInfo, sizeof(TRANSFORMINFO));
			pTransInfo = *pGameInstance->Get_RowDataToTransform(TEXT("Burger"), i + 1, DATATYPE_OBJECT);
			m_pBurgetParsingPoints.push_back(new _Vector3(pTransInfo.PosX, pTransInfo.PosY, pTransInfo.PosZ));
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CWaveManager::Free()
{
	for (auto& pObj : m_pWaveDatas)
		Destroy_WaveData(pObj);
	for (auto& data : m_pParsingPoints)
	{
		if (data)
		{
			delete data;
			data = nullptr;
		}
	}
	for (auto& data : m_pBurgetParsingPoints)
	{
		if (data)
		{
			delete data;
			data = nullptr;
		}
	}
}

_bool CWaveManager::tagWAVEDATA::Update_SpwanEvent(float fTimeDelta)
{
	if (iStepIndex < iStepMaxIndex)
	{
		if (fSpawnTime >= fSpawnMaxTime)
		{
			CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
			for (int i = 0; i < iObjSpwanCount; ++i)
			{
				_Vector3 position = _Vector3();
				//��ġ����
				switch (ePositionType)
				{
				case Client::SPAWN_POSITION_SHAPE::RANDOM:
				{
					CWaveManager* pWaveManager = GET_INSTANCE(CWaveManager);
					position = *pWaveManager->Get_MonsterRandomPoint();
					RELEASE_INSTANCE(CWaveManager);
					break;
				}
				case Client::SPAWN_POSITION_SHAPE::BESIDE:
				{
					if (i == 0)
					{
						position = Vector3(0, 0, 0) + (_Vector3(-1.f, 0, 0)) * 15;
					}
					else if (i == 1)
					{
						position = Vector3(0, 0, 0) + (_Vector3(1.f, 0, 0)) * 15;
					}
					break;
				}
				case Client::SPAWN_POSITION_SHAPE::RECTANGLE:
				{
					if (i == 0)
					{
						position = Vector3(0, 0, 0) + (_Vector3(-1.f, 0, -1.f)) * 15;
					}
					else if (i == 1)
					{
						position = Vector3(0, 0, 0) + (_Vector3(1.f, 0, 1.f)) * 15;
					}
					else if (i == 2)
					{
						position = Vector3(0, 0, 0) + (_Vector3(-1.f, 0, 1.f)) * 15;
					}
					else if (i == 3)
					{
						position = Vector3(0, 0, 0) + (_Vector3(1.f, 0, -1.f)) * 15;
					}
					break;
				}
				case Client::SPAWN_POSITION_SHAPE::DIAMOND:
				{
					if (i == 0)
					{
						position = Vector3(0, 0, 0) + (_Vector3(-1.f, 0, 0)) * 15;
					}
					else if (i == 1)
					{
						position = Vector3(0, 0, 0) + (_Vector3(1.f, 0, 0)) * 15;
					}
					else if (i == 2)
					{
						position = Vector3(0, 0, 0) + (_Vector3(0, 0, 1.f)) * 15;
					}
					else if (i == 3)
					{
						position = Vector3(0, 0, 0) + (_Vector3(0, 0, -1.f)) * 15;
					}
					break;
				}
				case Client::SPAWN_POSITION_SHAPE::CYCLE:
				{
					float btweenAngle = (360.f / iObjSpwanCount) * i;
					position = Vector3(0, 0, 0) + (_Vector3(cosf(XMConvertToRadians(btweenAngle)), 0.f, sinf(XMConvertToRadians(btweenAngle)))) * 15;
					break;
				}
				default:
					break;
				}

				//MonsterData����
				if (bItem)
				{
					CDropItem::tagPushDropItemData tData;
					ZeroMemory(&tData, sizeof(tData));

					CWaveManager* pWaveManager = GET_INSTANCE(CWaveManager);
					_Vector3 parsingPosition = *(pWaveManager->Get_BurgerParsingPoint());
					tData.Position = _Vector3(parsingPosition.x, parsingPosition.y + RandF(5.f, 7.5f), parsingPosition.z);
					RELEASE_INSTANCE(CWaveManager);
					tData.eType = CDropItem::DROPITEMTYPE::HP;
					auto helix = static_cast<CWaveMap_Helix*>(pInstance->Add_GameObjectToLayerAndReturn(TEXT("Layer_Helix"), TEXT("Prototype_GameObject_WaveMap_Helix"), &(tData.Position)));
					helix->Set_ItemData(L"Prototype_GameObject_DropItem", tData);
				}
				else
				{
					CMonster::PUSHMONSTERDATA tData;
					ZeroMemory(&tData, sizeof(tData));
					tData.Position = position;
					tData.DropLevel = 1;
					if (pInstance->Get_Random(25.f))
						tData.DropType = CMonster::ITEM_DROPTYPE::MANA;
					else
					{
						tData.DropLevel = 0;
						tData.DropType = CMonster::ITEM_DROPTYPE::TYPE_END;
					}

					tData.Hp = tUserData.fHp;
					tData.Dmg = tUserData.fDamage;
					tData.isWave = true;
					tData.iModelType = tUserData.iModelType;
					auto helix = static_cast<CWaveMap_Helix*>(pInstance->Add_GameObjectToLayerAndReturn(TEXT("Layer_Helix"), TEXT("Prototype_GameObject_WaveMap_Helix"), &(tData.Position)));
					helix->Set_MonsterData(swObjTag, tData);
					//pInstance->Add_GameObjectToLayer(TEXT("Layer_Wave"), swObjTag, &tData);
				}
			}
			++iStepIndex;
			fSpawnTime = 0;
			RELEASE_INSTANCE(CGameInstance);
		}
		else
			fSpawnTime += fTimeDelta;
	}
	else
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		if (pInstance->Get_LayerSize(TEXT("Layer_Wave")) <= 2)
		{
			NextWaveReady = true;
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	return NextWaveReady;
}

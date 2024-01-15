#include "..\Public\DataManager.h"

IMPLEMENT_SINGLETON(CDataManager)

CDataManager::CDataManager()
{
}

unordered_map<const _tchar*, vector<vector<string>>>* CDataManager::GetGameDatas(DATATYPE eType)
{
	return &m_GameData[eType];
}

_bool CDataManager::GetCheckEmpty(const _tchar* pObjTag, DATATYPE eType)
{
	DATA* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
	{
		return EMPTY;
	}
	if (pData->empty())
	{
		return EMPTY;
	}

	return NONEMPTY;
}

vector<vector<string>>* CDataManager::Get_Table(const _tchar* pObjTag, DATATYPE eType)
{
	auto	iter = find_if(m_GameData[eType].begin(), m_GameData[eType].end(), CTagFinder(pObjTag));

	if (iter == m_GameData[eType].end())
		return nullptr;

	return &(iter->second);
}

vector<string>* CDataManager::Get_RowData(const _tchar* pObjTag, const string& pObjName, DATATYPE eType)
{
	vector<vector<string>>* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
		return nullptr;

	for (int i = 0; i < pData->size(); ++i)
	{
		if (pObjName == (*pData)[i][0])
		{
			return &(pData)[i][0];
		}
	}

	return nullptr;
}

TRANSFORMINFO* CDataManager::Get_RowDataToTransform(const _tchar* pObjTag, const string& pObjName, DATATYPE eType)
{
	vector<vector<string>>* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
		return nullptr;


	ZeroMemory(&m_pTransInfo, sizeof(TRANSFORMINFO));

	for (int i = 0; i < pData->size(); ++i)
	{
		if (pObjName == (*pData)[i][0])
		{
			m_pTransInfo.PosX = stof((*pData)[i][1]);
			m_pTransInfo.PosY = stof((*pData)[i][2]);
			m_pTransInfo.PosZ = stof((*pData)[i][3]);

			m_pTransInfo.RotX = stof((*pData)[i][4]);
			m_pTransInfo.RotY = stof((*pData)[i][5]);
			m_pTransInfo.RotZ = stof((*pData)[i][6]);

			m_pTransInfo.ScaleX = stof((*pData)[i][7]);
			m_pTransInfo.ScaleY = stof((*pData)[i][8]);
			m_pTransInfo.ScaleZ = stof((*pData)[i][9]);

			return &m_pTransInfo;
		}
	}

	return nullptr;
}

TRANSFORMINFO* CDataManager::Get_RowDataToTransform(const _tchar* pObjTag, _uint RowIdx, DATATYPE eType)
{
	vector<vector<string>>* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
		return nullptr;

	if (pData->size() - 1 < RowIdx)
	{
		MSG_BOX("데이터 테이블 행보다 초과된 인덱스!");
		return nullptr;
	}


	ZeroMemory(&m_pTransInfo, sizeof(TRANSFORMINFO));

	m_pTransInfo.PosX = stof((*pData)[RowIdx][1]);
	m_pTransInfo.PosY = stof((*pData)[RowIdx][2]);
	m_pTransInfo.PosZ = stof((*pData)[RowIdx][3]);

	m_pTransInfo.RotX = stof((*pData)[RowIdx][4]);
	m_pTransInfo.RotY = stof((*pData)[RowIdx][5]);
	m_pTransInfo.RotZ = stof((*pData)[RowIdx][6]);

	m_pTransInfo.ScaleX = stof((*pData)[RowIdx][7]);
	m_pTransInfo.ScaleY = stof((*pData)[RowIdx][8]);
	m_pTransInfo.ScaleZ = stof((*pData)[RowIdx][9]);

	return &m_pTransInfo;
}

TRANSFORMINFO* CDataManager::Get_RowDataToBoxCollider(const _tchar* pObjTag, const string& pObjName, DATATYPE eType)
{
	vector<vector<string>>* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
		return nullptr;


	ZeroMemory(&m_pTransInfo, sizeof(TRANSFORMINFO));

	for (int i = 0; i < pData->size(); ++i)
	{
		if (pObjName == (*pData)[i][0])
		{
			m_pTransInfo.PosX = stof((*pData)[i][1]);
			m_pTransInfo.PosY = stof((*pData)[i][2]);
			m_pTransInfo.PosZ = stof((*pData)[i][3]);

			m_pTransInfo.RotX = stof((*pData)[i][4]);
			m_pTransInfo.RotY = stof((*pData)[i][5]);
			m_pTransInfo.RotZ = stof((*pData)[i][6]);

			m_pTransInfo.ScaleX = stof((*pData)[i][7]);
			m_pTransInfo.ScaleY = stof((*pData)[i][8]);
			m_pTransInfo.ScaleZ = stof((*pData)[i][9]);

			return &m_pTransInfo;
		}
	}

	return nullptr;
}

TRANSFORMINFO* CDataManager::Get_RowDataToBoxCollider(const _tchar* pObjTag, _uint RowIdx, DATATYPE eType)
{
	vector<vector<string>>* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
		return nullptr;

	if (pData->size()-1 < RowIdx)
	{
		MSG_BOX("데이터 테이블 행보다 초과된 인덱스!");
		return nullptr;
	}


	ZeroMemory(&m_pTransInfo, sizeof(TRANSFORMINFO));

	m_pTransInfo.PosX = stof((*pData)[RowIdx][1]);
	m_pTransInfo.PosY = stof((*pData)[RowIdx][2]);
	m_pTransInfo.PosZ = stof((*pData)[RowIdx][3]);

	m_pTransInfo.RotX = stof((*pData)[RowIdx][4]);
	m_pTransInfo.RotY = stof((*pData)[RowIdx][5]);
	m_pTransInfo.RotZ = stof((*pData)[RowIdx][6]);

	m_pTransInfo.ScaleX = stof((*pData)[RowIdx][7]);
	m_pTransInfo.ScaleY = stof((*pData)[RowIdx][8]);
	m_pTransInfo.ScaleZ = stof((*pData)[RowIdx][9]);

	return &m_pTransInfo;
}

CAPSULECOLINFO* CDataManager::Get_RowDataToCapsuleCol(const _tchar* pObjTag, const string& pObjName, DATATYPE eType)
{
	vector<vector<string>>* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
		return nullptr;

	ZeroMemory(&m_pCapsuleInfo, sizeof(CAPSULECOLINFO));

	for (int i = 0; i < pData->size()-1; ++i)
	{
		if (pObjName == (*pData)[i][0])
		{
			m_pCapsuleInfo.CenterX = stof((*pData)[i][11]) * 107.14f;
			m_pCapsuleInfo.CenterY = stof((*pData)[i][12]) * 107.14f;
			m_pCapsuleInfo.CenterZ = stof((*pData)[i][13]) * 107.14f;

			m_pCapsuleInfo.Radius = stof((*pData)[i][17]) * 68.5f;
			m_pCapsuleInfo.Height = stof((*pData)[i][18]) * 25.16f;

			return &m_pCapsuleInfo;
		}
	}

	return nullptr;
}

SPIDERPATTERNINFO* CDataManager::Get_RowDataToSpiderPattern(const _tchar* pObjTag, _uint RowIdx, DATATYPE eType)
{
	vector<vector<string>>* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
		return nullptr;

	if (pData->size() - 1 < RowIdx)
	{
		MSG_BOX("데이터 테이블 행보다 초과된 인덱스!");
		return nullptr;
	}


	ZeroMemory(&m_pSpiderPatternInfo, sizeof(SPIDERPATTERNINFO));

	m_pSpiderPatternInfo.Pattern[0] = stoi((*pData)[RowIdx][0]);
	m_pSpiderPatternInfo.Pattern[1] = stoi((*pData)[RowIdx][1]);
	m_pSpiderPatternInfo.Pattern[2] = stoi((*pData)[RowIdx][2]);
	m_pSpiderPatternInfo.Pattern[3] = stoi((*pData)[RowIdx][3]);
	m_pSpiderPatternInfo.Pattern[4] = stoi((*pData)[RowIdx][4]);
	m_pSpiderPatternInfo.Pattern[5] = stoi((*pData)[RowIdx][5]);
	m_pSpiderPatternInfo.Pattern[6] = stoi((*pData)[RowIdx][6]);
	m_pSpiderPatternInfo.Pattern[7] = stoi((*pData)[RowIdx][7]);
	m_pSpiderPatternInfo.Pattern[8] = stoi((*pData)[RowIdx][8]);
	m_pSpiderPatternInfo.Pattern[9] = stoi((*pData)[RowIdx][9]);
	m_pSpiderPatternInfo.Pattern[10] = stoi((*pData)[RowIdx][10]);
	m_pSpiderPatternInfo.Pattern[11] = stoi((*pData)[RowIdx][11]);
	return &m_pSpiderPatternInfo;
}

PUSHDEFAULTOBJECTDATA* CDataManager::Get_RowDataToDefaultObjData(const _tchar* pObjTag, const string& pObjName, DATATYPE eType)
{
	vector<vector<string>>* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
		return nullptr;	

	for (int i = 0; i < pData->size(); ++i)
	{
		if (pObjName == (*pData)[i][0])
		{
			m_pDefaultObjInfo.Position.x = stof((*pData)[i][1]);
			m_pDefaultObjInfo.Position.y = stof((*pData)[i][2]);
			m_pDefaultObjInfo.Position.z = stof((*pData)[i][3]);

			m_pDefaultObjInfo.YRotate = stof((*pData)[i][4]);

			m_pDefaultObjInfo.Scale.x = stof((*pData)[i][5]);
			m_pDefaultObjInfo.Scale.y = stof((*pData)[i][6]);
			m_pDefaultObjInfo.Scale.z = stof((*pData)[i][7]);

			if ((*pData)[i][8] == "SPHERE")
				m_pDefaultObjInfo.Type = GEOMETRYTYPE::GT_SPHERE;
			else if ((*pData)[i][8] == "CAPSULE")
				m_pDefaultObjInfo.Type = GEOMETRYTYPE::GT_CAPSULE;
			else if ((*pData)[i][8] == "BOX")
				m_pDefaultObjInfo.Type = GEOMETRYTYPE::GT_BOX;

			m_pDefaultObjInfo.Extents.x = stof((*pData)[i][9]);
			m_pDefaultObjInfo.Extents.y = stof((*pData)[i][10]);
			m_pDefaultObjInfo.Extents.z = stof((*pData)[i][11]);

			m_pDefaultObjInfo.Radius = stof((*pData)[i][12]);
			m_pDefaultObjInfo.Height = stof((*pData)[i][13]);

			m_pDefaultObjInfo.LocalPos.x = stof((*pData)[i][14]);
			m_pDefaultObjInfo.LocalPos.y = stof((*pData)[i][15]);
			m_pDefaultObjInfo.LocalPos.z = stof((*pData)[i][16]);

			m_pDefaultObjInfo.Material.x = stof((*pData)[i][17]);
			m_pDefaultObjInfo.Material.y = stof((*pData)[i][18]);
			m_pDefaultObjInfo.Material.z = stof((*pData)[i][19]);

			m_pDefaultObjInfo.Mess = stof((*pData)[i][20]);
			m_pDefaultObjInfo.LockFlag = stoi((*pData)[i][21]);

			if (((*pData)[i][22]) == "FALSE")
				m_pDefaultObjInfo.IsKinematic = false;
			else if (((*pData)[i][22]) == "TRUE")
				m_pDefaultObjInfo.IsKinematic = true;

			m_pDefaultObjInfo.CMessLocalPos.x = stof((*pData)[i][23]);
			m_pDefaultObjInfo.CMessLocalPos.y = stof((*pData)[i][24]);
			m_pDefaultObjInfo.CMessLocalPos.z = stof((*pData)[i][25]);

			if ((*pData)[i][26] == "FALSE")
				m_pDefaultObjInfo.IsPlayerGuard = false;
			else if ((*pData)[i][26] == "TRUE")
				m_pDefaultObjInfo.IsPlayerGuard = true;
	
			m_ProtoPaths.push_back(StringToTCHAR((*pData)[i][27]));
			m_pDefaultObjInfo.ModelName = m_ProtoPaths.back();

			return &m_pDefaultObjInfo;
		}
	}

	return nullptr;
}

PUSHDEFAULTOBJECTDATA* CDataManager::Get_RowDataToDefaultObjData(const _tchar* pObjTag, _uint RowIdx, DATATYPE eType)
{
	vector<vector<string>>* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
		return nullptr;

	if (pData->size() - 1 < RowIdx)
	{
		MSG_BOX("데이터 테이블 행보다 초과된 인덱스!");
		return nullptr;
	}	

	m_pDefaultObjInfo.Position.x = stof((*pData)[RowIdx][1]);
	m_pDefaultObjInfo.Position.y = stof((*pData)[RowIdx][2]);
	m_pDefaultObjInfo.Position.z = stof((*pData)[RowIdx][3]);

	m_pDefaultObjInfo.YRotate = stof((*pData)[RowIdx][4]);

	m_pDefaultObjInfo.Scale.x = stof((*pData)[RowIdx][5]);
	m_pDefaultObjInfo.Scale.y = stof((*pData)[RowIdx][6]);
	m_pDefaultObjInfo.Scale.z = stof((*pData)[RowIdx][7]);

	if ((*pData)[RowIdx][8] == "SPHERE")
		m_pDefaultObjInfo.Type = GEOMETRYTYPE::GT_SPHERE;
	else if ((*pData)[RowIdx][8] == "CAPSULE")
		m_pDefaultObjInfo.Type = GEOMETRYTYPE::GT_CAPSULE;
	else if ((*pData)[RowIdx][8] == "BOX")
		m_pDefaultObjInfo.Type = GEOMETRYTYPE::GT_BOX;

	m_pDefaultObjInfo.Extents.x = stof((*pData)[RowIdx][9]);
	m_pDefaultObjInfo.Extents.y = stof((*pData)[RowIdx][10]);
	m_pDefaultObjInfo.Extents.z = stof((*pData)[RowIdx][11]);

	m_pDefaultObjInfo.Radius = stof((*pData)[RowIdx][12]);
	m_pDefaultObjInfo.Height = stof((*pData)[RowIdx][13]);

	m_pDefaultObjInfo.LocalPos.x = stof((*pData)[RowIdx][14]);
	m_pDefaultObjInfo.LocalPos.y = stof((*pData)[RowIdx][15]);
	m_pDefaultObjInfo.LocalPos.z = stof((*pData)[RowIdx][16]);

	m_pDefaultObjInfo.Material.x = stof((*pData)[RowIdx][17]);
	m_pDefaultObjInfo.Material.y = stof((*pData)[RowIdx][18]);
	m_pDefaultObjInfo.Material.z = stof((*pData)[RowIdx][19]);

	m_pDefaultObjInfo.Mess = stof((*pData)[RowIdx][20]);
	m_pDefaultObjInfo.LockFlag = stoi((*pData)[RowIdx][21]);

	if (((*pData)[RowIdx][22]) == "FALSE")
		m_pDefaultObjInfo.IsKinematic = false;
	else if (((*pData)[RowIdx][22]) == "TRUE")
		m_pDefaultObjInfo.IsKinematic = true;

	m_pDefaultObjInfo.CMessLocalPos.x = stof((*pData)[RowIdx][23]);
	m_pDefaultObjInfo.CMessLocalPos.y = stof((*pData)[RowIdx][24]);
	m_pDefaultObjInfo.CMessLocalPos.z = stof((*pData)[RowIdx][25]);

	if ((*pData)[RowIdx][26] == "FALSE")
		m_pDefaultObjInfo.IsPlayerGuard = false;
	else if ((*pData)[RowIdx][26] == "TRUE")
		m_pDefaultObjInfo.IsPlayerGuard = true;


	m_ProtoPaths.push_back(StringToTCHAR((*pData)[RowIdx][27]));
	m_pDefaultObjInfo.ModelName = m_ProtoPaths.back();

	return &m_pDefaultObjInfo;
}

PUSHCHESTDATA* CDataManager::Get_RowDataToChestData(const _tchar* pObjTag, _uint RowIdx, DATATYPE eType)
{
	vector<vector<string>>* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
		return nullptr;

	if (pData->size() - 1 < RowIdx)
	{
		MSG_BOX("데이터 테이블 행보다 초과된 인덱스!");
		return nullptr;
	}

	m_pChestInfo.Position.x = stof((*pData)[RowIdx][1]);
	m_pChestInfo.Position.y = stof((*pData)[RowIdx][2]);
	m_pChestInfo.Position.z = stof((*pData)[RowIdx][3]);

	m_pChestInfo.YRotate = stof((*pData)[RowIdx][4]);

	m_pChestInfo.Gold = stoi((*pData)[RowIdx][5]);


	return &m_pChestInfo;
}

PUSHSPECIALCHESTDATA* CDataManager::Get_RowDataToSpecialChestData(const _tchar* pObjTag, _uint RowIdx, DATATYPE eType)
{
	vector<vector<string>>* pData = Get_Table(pObjTag, eType);

	if (nullptr == pData)
		return nullptr;

	if (pData->size() - 1 < RowIdx)
	{
		MSG_BOX("데이터 테이블 행보다 초과된 인덱스!");
		return nullptr;
	}

	m_pSpecialChestInfo.Position.x = stof((*pData)[RowIdx][1]);
	m_pSpecialChestInfo.Position.y = stof((*pData)[RowIdx][2]);
	m_pSpecialChestInfo.Position.z = stof((*pData)[RowIdx][3]);

	m_pSpecialChestInfo.YRotate = stof((*pData)[RowIdx][4]);

	m_pSpecialChestInfo.ItemType = stoi((*pData)[RowIdx][5]);

	m_pSpecialChestInfo.NumItem = stoi((*pData)[RowIdx][6]);

	return &m_pSpecialChestInfo;
}

_uint CDataManager::Get_TableSize(const _tchar* pObjTag, DATATYPE eType)
{
	if (m_GameData[eType].end() == m_GameData[eType].find(pObjTag))
	{
		return 0;
	}
	else if (m_GameData[eType][pObjTag].back().size() == 0 && eType != DATATYPE_GROUND && eType != DATATYPE_WALL)
		return _uint(m_GameData[eType][pObjTag].size() - 2);
	else
		return _uint(m_GameData[eType][pObjTag].size() - 1);
}

HRESULT CDataManager::AddGameData(const _tchar* pObjTag, const string& _filePath, DATATYPE eType)
{
	PARSER::CSV_DATA data;

	if (m_GameData[eType].end() == m_GameData[eType].find(pObjTag))
	{
		m_GameData[eType].insert(make_pair(pObjTag, vector<vector<string>>()));
	}

	PARSER::CSVParser()(_filePath, &m_GameData[eType][pObjTag]);

	return S_OK;
}

void CDataManager::Free()
{
	for (auto& Proto : m_ProtoPaths)
		Safe_Delete_Array(Proto);
	m_ProtoPaths.clear();

	for(int i = 0; i < DATATYPE_END; ++i)
	{
		for (pair< const _tchar*, vector<vector<string>> > Pair : m_GameData[i])
		{
			for (vector<string> vec : Pair.second)
			{
				vec.clear();
			}
			Pair.second.clear();
		}
		m_GameData[i].clear();
	}
}

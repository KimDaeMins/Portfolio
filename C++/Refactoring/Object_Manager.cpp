#include "..\public\Object_Manager.h"
#include "GameObject.h"
#include "Component.h"
#include "Layer.h"


IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}
CObject_Manager::~CObject_Manager()
{
}

shared_ptr<CComponent> CObject_Manager::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (auto pLayer = Find_Layer(iLevelIndex, pLayerTag); pLayer != nullptr)
	{
		return pLayer->Get_ComponentPtr(pComponentTag, iIndex);
	}

	return nullptr;
}

shared_ptr<CGameObject> CObject_Manager::Get_GameObjectPtr(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex)
{
	if (auto pLayer = Find_Layer(iLevelIndex, pLayerTag); pLayer != nullptr)
	{
		return pLayer->Get_GameObjectPtr(iIndex);
	}

	return nullptr;
}

list<shared_ptr<CGameObject>> CObject_Manager::Get_ObjectList(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (auto pLayer = Find_Layer(iLevelIndex, pLayerTag); pLayer != nullptr)
	{
		return pLayer->Get_Objects();
	}

	return list<shared_ptr<CGameObject>>();
}

HRESULT CObject_Manager::Delete_Object(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex)
{
	if (auto pLayer = Find_Layer(iLevelIndex, pLayerTag); pLayer != nullptr)
	{
		return pLayer->Delete_Object(iIndex);
	}

	return E_FAIL;
}

HRESULT CObject_Manager::Delete_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (m_pLayers[iLevelIndex].contains(pLayerTag))
	{
		m_pLayers[iLevelIndex].erase(pLayerTag);
		return S_OK;
	}

	return E_FAIL;
}

_uint CObject_Manager::Get_LayerSize(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (auto pLayer = Find_Layer(iLevelIndex, pLayerTag); pLayer != nullptr)
	{
		return pLayer->Get_Size();
	}
	return 0;
}

HRESULT CObject_Manager::Reserve_Container(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = make_shared<unordered_map<const _tchar*,shared_ptr<CLayer>>[]>(iNumLevels);
	
	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, shared_ptr<CGameObject> pPrototype)
{
	if (nullptr == pPrototype ||
		nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg, shared_ptr<CGameObject>& Out)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	if (auto pPrototype = Find_Prototype(pPrototypeTag); pPrototypeTag != nullptr)
	{
		if (auto pGameObject = pPrototype->Clone(pArg); nullptr != pGameObject)
		{
			if (auto pLayer = Find_Layer(iLevelIndex, pLayerTag); pLayer != nullptr)
			{
				pLayer->Add_GameObject(pGameObject);
			}
			else
			{
				if (auto layerTemp = CLayer::Create(); pLayer != nullptr)
				{
					layerTemp->Add_GameObject(pGameObject);

					m_pLayers[iLevelIndex].emplace(pLayerTag, layerTemp);
				}
			}

			Out = pGameObject;
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CObject_Manager::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	if (auto pPrototype = Find_Prototype(pPrototypeTag); pPrototypeTag != nullptr)
	{
		if (auto pGameObject = pPrototype->Clone(pArg); nullptr != pGameObject)
		{
			if (auto pLayer = Find_Layer(iLevelIndex, pLayerTag); pLayer != nullptr)
			{
				pLayer->Add_GameObject(pGameObject);
			}
			else
			{
				if (auto layerTemp = CLayer::Create(); pLayer != nullptr)
				{
					layerTemp->Add_GameObject(pGameObject);

					m_pLayers[iLevelIndex].emplace(pLayerTag, layerTemp);
				}
			}
			return S_OK;
		}
	}

	return E_FAIL;
}

_int CObject_Manager::Tick(_float fTimeDelta)
{
	_int iProgress = 0;

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i]) 
		{
			iProgress = Pair.second->Tick(fTimeDelta);
			if (0 > iProgress)
				return -1;
		}
	}
	return 0;
}

_int CObject_Manager::LateTick(_float fTimeDelta)
{
	_int iProgress = 0;

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			iProgress = Pair.second->LateTick(fTimeDelta);
			if (0 > iProgress)
				return -1;
		}
	}
	return 0;
}

_int CObject_Manager::SetTick(_float fTimeDelta)
{
	_int iProgress = 0;

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			iProgress = Pair.second->SetTick(fTimeDelta);
			if (0 > iProgress)
				return -1;
		}
	}
	return 0;
}

HRESULT CObject_Manager::Clear_LevelLayers(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

shared_ptr<CGameObject> CObject_Manager::Find_Prototype(const _tchar * pPrototypeTag)
{
	if (m_Prototypes.contains(pPrototypeTag))
	{
		return m_Prototypes.at(pPrototypeTag);
	}

	return nullptr;
}

shared_ptr<CLayer> CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (iLevelIndex < m_iNumLevels && m_pLayers[iLevelIndex].contains(pLayerTag))
	{
		return m_pLayers[iLevelIndex].at(pLayerTag);
	}

	return nullptr;
}
#pragma once

/* 1. 게임내에서 생성되고 사용ㅇ되는 객체(사본)들을 너희들 기준에 따라 분류하여 저장한다. */
/* 2. 원형객체을 보관한다. */



#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)

private:
	CObject_Manager();
	virtual ~CObject_Manager();
public:
	shared_ptr<class CComponent> Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex);
	shared_ptr<class CGameObject> Get_GameObjectPtr(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex);
	list<shared_ptr<class CGameObject>> Get_ObjectList(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT Delete_Object(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex);
	HRESULT Delete_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	_uint Get_LayerSize(_uint iLevelIndex, const _tchar* pLayerTag);
public:
	HRESULT Reserve_Container(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, shared_ptr<class CGameObject> pPrototype);
	HRESULT Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg, shared_ptr<class CGameObject>& Out);
	HRESULT Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg);
	_int Tick(_float fTimeDelta);
	_int LateTick(_float fTimeDelta);
	_int SetTick(_float fTimeDelta);
	HRESULT Clear_LevelLayers(_uint iLevelIndex);

private:
	/* 원형객체들을 저장한다. */
	unordered_map<const _tchar*, shared_ptr<class CGameObject>>			m_Prototypes;
	typedef unordered_map<const _tchar*, shared_ptr<class CGameObject>>	PROTOTYPES;

private:
	/* 사본객체들을 보관한다. (내 기준에 따라 나누어서. + 레벨별로 구분하여.  )*/
	shared_ptr<unordered_map<const _tchar*, shared_ptr<class CLayer>>[]>		m_pLayers;
	typedef shared_ptr<unordered_map<const _tchar*, shared_ptr<class CLayer>>[]>		LAYERS;

private:
	_uint				m_iNumLevels = 0;

private:
	shared_ptr<CGameObject> Find_Prototype(const _tchar* pPrototypeTag);
	shared_ptr<CLayer> Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
};

END
#pragma once

/* 1. ���ӳ����� �����ǰ� ��뤷�Ǵ� ��ü(�纻)���� ����� ���ؿ� ���� �з��Ͽ� �����Ѵ�. */
/* 2. ������ü�� �����Ѵ�. */



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
	/* ������ü���� �����Ѵ�. */
	unordered_map<const _tchar*, shared_ptr<class CGameObject>>			m_Prototypes;
	typedef unordered_map<const _tchar*, shared_ptr<class CGameObject>>	PROTOTYPES;

private:
	/* �纻��ü���� �����Ѵ�. (�� ���ؿ� ���� �����. + �������� �����Ͽ�.  )*/
	shared_ptr<unordered_map<const _tchar*, shared_ptr<class CLayer>>[]>		m_pLayers;
	typedef shared_ptr<unordered_map<const _tchar*, shared_ptr<class CLayer>>[]>		LAYERS;

private:
	_uint				m_iNumLevels = 0;

private:
	shared_ptr<CGameObject> Find_Prototype(const _tchar* pPrototypeTag);
	shared_ptr<CLayer> Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
};

END
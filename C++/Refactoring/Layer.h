#pragma once

#include "Base.h"
#include "GameObject.h"
/* 사본객체들을 모아놓는다. */

BEGIN(Engine)
class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;
public:
	shared_ptr<class CComponent> Get_ComponentPtr(const _tchar* pComponentTag, _uint iIndex);
	shared_ptr<class  CGameObject> Get_GameObjectPtr(_uint iIndex);
	list<shared_ptr<class CGameObject>> Get_Objects();
	HRESULT Delete_Object(_uint iIndex);
	_uint Get_Size();
public:
	HRESULT NativeConstruct();
	HRESULT Add_GameObject(shared_ptr<class CGameObject> pGameObject);
	_int Tick(_float fTimeDelta);
	_int LateTick(_float fTimeDelta);
	_int SetTick(_float fTimeDelta);
private:
	list<shared_ptr<class CGameObject>>			m_Objects;
	typedef list<shared_ptr<class CGameObject>>	OBJECTS;
public:
	static shared_ptr<CLayer> Create();
};

END
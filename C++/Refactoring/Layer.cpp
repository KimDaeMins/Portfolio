#include "..\public\Layer.h"
#include "Component.h"

CLayer::CLayer()
{
}

shared_ptr<CComponent> CLayer::Get_ComponentPtr(const _tchar * pComponentTag, _uint iIndex)
{
	if (m_Objects.size() <= iIndex)
		return nullptr;

	auto	iter = m_Objects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_ComponentPtr(pComponentTag);	
}

shared_ptr<CGameObject> CLayer::Get_GameObjectPtr(_uint iIndex)
{
	if (m_Objects.size() <= iIndex)
		return nullptr;

	auto	iter = m_Objects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return *iter;
}

list<shared_ptr<CGameObject>> CLayer::Get_Objects()
{
	return m_Objects;
}

HRESULT CLayer::Delete_Object(_uint iIndex)
{
	//쓰일일은 없을건데.. 일단 체크
	auto iter = m_Objects.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	m_Objects.erase(iter);
	
	return S_OK;
}

_uint CLayer::Get_Size()
{
	return (_uint)m_Objects.size();
}

HRESULT CLayer::NativeConstruct()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(shared_ptr<CGameObject> pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_Objects.push_back(pGameObject);

	return S_OK;
}

_int CLayer::Tick(_float fTimeDelta)
{
	//이렇게하면 프로그래스에 따라서 정리는 안되는데...
	// -> 방법은 있다 일단 false를 리턴하면 되는거니까
	// -> 추가로 Prograss가 DeadEvent를 제외하곤 현 프로젝트에선 안쓰임
	//nullptr일 때의 경우가 있나..? -> 삭제를 충분히 관리가능하면 없을듯.
	//if (pGameObject != nullptr)
	//{
	//	return pGameObject->Tick(fTimeDelta) == EVENT_DEAD;
	//}
	erase_if(m_Objects, [&](shared_ptr<CGameObject> pGameObject)
		{
			return pGameObject->Tick(fTimeDelta) == EVENT_DEAD;
		});

	return _int();
}

_int CLayer::LateTick(_float fTimeDelta)
{
	for (const auto& pGameObject : std::views::all(m_Objects)
		| std::views::filter([](shared_ptr<CGameObject> obj) {return obj != nullptr; }))
	{
		if (_int iProgress = pGameObject->LateTick(fTimeDelta); 0 > iProgress)
			return -1;
	}

	return 0;
}

_int CLayer::SetTick(_float fTimeDelta)
{
	for (const auto& pGameObject : std::views::all(m_Objects) 
		| std::views::filter([](shared_ptr<CGameObject> obj) {return obj != nullptr; }))
	{
		if (_int iProgress = pGameObject->SetTick(fTimeDelta); 0 > iProgress)
			return -1;
	}

	return 0;
}

shared_ptr<CLayer> CLayer::Create()
{
	auto	pInstance = make_shared<CLayer>();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed To Creating CLayer");
		return nullptr;
	}
	return pInstance;
}

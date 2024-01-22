#include "stdafx.h"
#include "ConsumableItem.h"

#include "GameInstance.h"
CConsumableItem::CConsumableItem(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    :CItem(pDevice, pDeviceContext)
{
}

CConsumableItem::CConsumableItem(const CConsumableItem& rhs)
    : CItem(rhs)
{
}

HRESULT CConsumableItem::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CConsumableItem::NativeConstruct(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	m_ItemType = ITEM_TYPE::CONSUMABLE;
	m_ConsumableType = *(CONSUMABLEITEM_TYPE*)pArg;

	switch (m_ConsumableType)//사용아이템타입에 따라서 어떻게쓰는지를 분류함(애니메이션 분리)
	{
	case CONSUMABLEITEM_TYPE::FIRECRACKER:
	case CONSUMABLEITEM_TYPE::FIREBOTTLE:
	case CONSUMABLEITEM_TYPE::ICEBOTTLE:
		m_UsingType = USING_TYPE::THROWITEM;
		break;
	case CONSUMABLEITEM_TYPE::REDFRUIT:
	case CONSUMABLEITEM_TYPE::BLUEFRUIT:
	case CONSUMABLEITEM_TYPE::PEPPER:
	case CONSUMABLEITEM_TYPE::HERB:
		m_UsingType = USING_TYPE::EATITEM;
		break;
	case CONSUMABLEITEM_TYPE::PIGGYBANK:
		m_UsingType = USING_TYPE::BANKBROKE;
		break;
	case CONSUMABLEITEM_TYPE::COIN:
		m_UsingType = USING_TYPE::COIN;
		break;
	}
    return S_OK;
}

_int CConsumableItem::Item_Tick(_float fTimeDelta)
{
    return _int();
}

_int CConsumableItem::Item_LateTick(_float fTimeDelta)
{
    return _int();
}

HRESULT CConsumableItem::Item_Render(_uint iRenderGroup)
{
    return S_OK;
}

void CConsumableItem::Set_ConsumableType(CONSUMABLEITEM_TYPE Type)
{
	m_ConsumableType = Type;
}

CONSUMABLEITEM_TYPE CConsumableItem::Get_ConsumableType()
{
	return m_ConsumableType;
}

void CConsumableItem::Add_Item(_uint NumItem)
{
	m_NumItem += NumItem;
}

_uint CConsumableItem::Use_Item()
{
	--m_NumItem;
	return m_NumItem;
}


CConsumableItem* CConsumableItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CConsumableItem* pInstance = new CConsumableItem(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CConsumableItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CConsumableItem::Clone(void* pArg)
{
	CConsumableItem* pInstance = new CConsumableItem(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CConsumableItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConsumableItem::Free()
{
	__super::Free();
}


#include "stdafx.h"
#include "..\public\Inventory.h"

#include "GameInstance.h"
#include "Player.h"

#include "ConsumableItem.h"
#include "Weapon.h"
#include "TabUI.h"
#include "PlayerPotionBar.h"
#include "Charm.h"

IMPLEMENT_SINGLETON(CInventory)

CInventory::CInventory()
{
	m_Geers.resize((_uint)GEAR_TYPE::TYPE_END);	
}

void CInventory::LastUseTick(_float fTimeDelta)
{
	for (auto& Geer : m_Geers)
	{
		if (Geer != nullptr)
			Geer->Item_Tick(fTimeDelta);
	}

	if (m_LastUse == nullptr)
		return;

	if ((_uint)ITEM_TYPE::EQUIPABLE != (_uint)m_LastUse->Get_ItemType())
		return;

	m_LastUse->Item_Tick(fTimeDelta);
}

void CInventory::LastUseLateTick(_float fTimeDelta)
{
	for (auto& Geer : m_Geers)
	{
		if (Geer != nullptr)
			Geer->Item_LateTick(fTimeDelta);
	}

	if (m_LastUse == nullptr)
		return;

	if ((_uint)ITEM_TYPE::EQUIPABLE != (_uint)m_LastUse->Get_ItemType())
		return;

	m_LastUse->Item_LateTick(fTimeDelta);
}

void CInventory::LastUseRender(_uint iRenderGroup)
{
	for (auto& Geer : m_Geers)
	{
		if (Geer != nullptr)
			Geer->Item_Render(iRenderGroup);
	}

	if (m_LastUse == nullptr)
		return;

	if ((_uint)ITEM_TYPE::EQUIPABLE != (_uint)m_LastUse->Get_ItemType())
		return;

	m_LastUse->Item_Render(iRenderGroup);
}

// void CInventory::LastUseRenderShadow()
// {
// 	if (m_LastUse == nullptr)
// 		return;
// 
// 	if ((_uint)ITEM_TYPE::EQUIPABLE != (_uint)m_LastUse->Get_ItemType())
// 		return;
// 
// 	m_LastUse->Item_Render_Shadow();
// }

_bool CInventory::Add_PotionMax(_uint Potion)
{
	m_iPotionMax += Potion;
	m_iPotion = m_iPotionMax;
	//여기서 모션이든 이벤트든 나타내면됨

	// 포션 UI에 적용
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayerPotionBar* pPotionBar = static_cast<CPlayerPotionBar*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 5, SORTLAYER_UI));
	pPotionBar->Set_MaxPotion(m_iPotionMax);
	pPotionBar->Set_Potion(m_iPotion);
	RELEASE_INSTANCE(CGameInstance);

	return true;
}

_bool CInventory::Add_PotionPiece(_uint Piece)
{
	m_iPotionPiece += Piece;
	while(m_iPotionPiece >= 3)
	{
		m_iPotionPiece -= 3;
		Add_PotionMax(1);
	}

	// 인벤토리 ui에 적용
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTabUI* pTabUI = static_cast<CTabUI*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 0, SORTLAYER_UI));
	if (nullptr == pTabUI)
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	pTabUI->Set_NumPotionPiece(m_iPotionPiece);

	RELEASE_INSTANCE(CGameInstance);

	return true;
}

_bool CInventory::Add_Gold(_uint Gold)
{
	m_iGold += Gold;

	// 인벤토리 ui에 적용
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTabUI* pTabUI = static_cast<CTabUI*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 0, SORTLAYER_UI));
	if (nullptr == pTabUI)
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	pTabUI->Set_NumGold(m_iGold);

	RELEASE_INSTANCE(CGameInstance);

	return true;
}

_bool CInventory::Add_ConsumableItem(_uint ConsumableItemType, _uint NumItem)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTabUI* pTabUI = static_cast<CTabUI*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 0, SORTLAYER_UI));
	if (nullptr == pTabUI)
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	_bool ItemInput = false;
	for (auto& Item : m_LookItems[(_uint)ITEM_TYPE::CONSUMABLE])
	{
		if ((_uint)((CConsumableItem*)Item)->Get_ConsumableType() == ConsumableItemType)
		{
			((CConsumableItem*)Item)->Add_Item(NumItem);
			ItemInput = true;

			// 인벤토리 UI 수치 조정
			pTabUI->Set_Consumable_Quantity((CONSUMABLEITEM_TYPE)ConsumableItemType, ((CConsumableItem*)Item)->Get_NumItem());
		}
	}
	if (!ItemInput)
	{
		//새로 클론하고 클론한녀석을 가져와야되는데....
		CItem* pItem = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", L"Prototype_GameObject_ConsumableItem", &ConsumableItemType, SORTLAYER_OBJ);

		((CConsumableItem*)pItem)->Add_Item(NumItem);
		(m_LookItems[(_uint)ITEM_TYPE::CONSUMABLE]).push_back(pItem);
		pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", L"Prototype_GameObject_ConsumableItem", pItem, SORTLAYER_OBJ);

		// 인벤토리 UI에 추가
		pTabUI->Add_Consumable((CONSUMABLEITEM_TYPE)ConsumableItemType, NumItem);
	}

	RELEASE_INSTANCE(CGameInstance);
	return true;
}

_bool CInventory::Add_Charm(CItem* Charm)
{
	m_Charms.push_back(Charm);
	return true;
}

_bool CInventory::Add_Charm(CHARM_TYPE CharmType)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTabUI* pTabUI = static_cast<CTabUI*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 0, SORTLAYER_UI));
	if (nullptr == pTabUI)
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
	
	for (auto& Charm : m_Charms)
	{
		if (static_cast<CCharm*>(Charm)->Get_CharmType() & _uint(CharmType))
		{
			MSG_BOX("이미 있는 부적이래요");
			RELEASE_INSTANCE(CGameInstance);
			return false;
		}
	}

	//새로 클론하고 클론한녀석을 가져와야되는데....
	CItem* pItem = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", L"Prototype_GameObject_Charm", &CharmType, SORTLAYER_OBJ);
	pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", L"Prototype_GameObject_Charm", pItem, SORTLAYER_OBJ);

	m_Charms.push_back(pItem);


	// 인벤토리 UI에 추가
	pTabUI->Add_Charm(CharmType);

	RELEASE_INSTANCE(CGameInstance);
	return true;
}

_bool CInventory::Add_Weapon(CItem* Weapon)
{
	(m_LookItems[(_uint)ITEM_TYPE::EQUIPABLE]).push_back(Weapon);
	

	// 인벤토리 UI에 추가합니다
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTabUI* pTabUI = static_cast<CTabUI*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 0, SORTLAYER_UI));
	if (nullptr == pTabUI)
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	EQUIPABLEITEM_TYPE		eType = static_cast<CWeapon*>(Weapon)->Get_EquipableType();

	pTabUI->Add_Equipable(eType);

	RELEASE_INSTANCE(CGameInstance);

	return true;
}

void CInventory::Fill_Potion()
{
	m_iPotion = m_iPotionMax;

	// 포션 UI에 적용
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayerPotionBar* pPotionBar = static_cast<CPlayerPotionBar*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 5, SORTLAYER_UI));
	pPotionBar->Set_Potion(m_iPotion);
	RELEASE_INSTANCE(CGameInstance);
}

void CInventory::Add_Key()
{
	++m_Key;
}

_bool CInventory::Use_Key()
{
	if(m_Key > 0)
	{
		++m_Key;
		return true;
	}
	return false;
}

_bool CInventory::SizeUp_Charm()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTabUI* pTabUI = static_cast<CTabUI*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 0, SORTLAYER_UI));
	RELEASE_INSTANCE(CGameInstance);
	if (nullptr == pTabUI)
		return false;

	(m_LookItems[(_uint)ITEM_TYPE::CHARM]).push_back(nullptr);

	// 인벤토리 UI 부적 슬롯 추가
	pTabUI->Add_CharmSlotSize();

	return _bool();
}

_uint CInventory::GetCharmSlotSize()
{
	return _uint((m_LookItems[(_uint)ITEM_TYPE::CHARM]).size());
}

CHARM_TYPE CInventory::Get_LookItemCharmType(_uint iLookItemCharmSlotIndex)
{
	if (m_LookItems[(_uint)ITEM_TYPE::CHARM].size() <= iLookItemCharmSlotIndex)
		return CHARM_TYPE::NONE;

	if (nullptr == m_LookItems[(_uint)ITEM_TYPE::CHARM][iLookItemCharmSlotIndex])
		return CHARM_TYPE::NONE;

	return (CHARM_TYPE)(static_cast<CCharm*>(m_LookItems[(_uint)ITEM_TYPE::CHARM][iLookItemCharmSlotIndex])->Get_CharmType());
}

_bool CInventory::UseAble_Gold(_uint Gold)
{
	if (Gold <= m_iGold)
		return true;
	return false;
}

_bool CInventory::Use_Gold(_uint Gold)
{
	if (UseAble_Gold(Gold))
	{
		m_iGold -= Gold;

		// 인벤토리 ui에 적용
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CTabUI* pTabUI = static_cast<CTabUI*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 0, SORTLAYER_UI));
		if (nullptr == pTabUI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return false;
		}

		pTabUI->Set_NumGold(m_iGold);

		RELEASE_INSTANCE(CGameInstance);

		return true;
	}
	return false;
}

_bool CInventory::Use_Potion(_float* HealAmount)
{
	if (m_iPotion > 0)
	{
		--m_iPotion;
		*HealAmount = m_PotionHealAmount;
		m_LastUse = m_Potion;

		// 포션 UI에 적용
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CPlayerPotionBar* pPotionBar = static_cast<CPlayerPotionBar*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 5, SORTLAYER_UI));
		pPotionBar->Set_Potion(m_iPotion);
		RELEASE_INSTANCE(CGameInstance);

		return true;
	}
	return false;
}

_bool CInventory::Get_Potion_Useable()
{
	if (m_iPotion > 0)
	{
		return true;
	}
	return false;
}

_bool CInventory::Change_Charm(_uint CharmIndex, _uint LookItemIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTabUI* pTabUI = static_cast<CTabUI*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 0, SORTLAYER_UI));
	RELEASE_INSTANCE(CGameInstance);
	if (nullptr == pTabUI)
		return false;

	if(CharmIndex <= m_Charms.size())
	{
		if (LookItemIndex < (m_LookItems[(_uint)ITEM_TYPE::CHARM]).size())
		{
			if (CharmIndex < m_Charms.size())
			{
				if (m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex] != m_Charms[CharmIndex])
				{
					// 같은 부적이 이미 다른 슬롯에 세팅되어있다면 삭제
					_uint		NewCharmType = (_uint)CHARM_TYPE::NONE;
					if (nullptr != static_cast<CCharm*>(m_Charms[CharmIndex]))
						NewCharmType = static_cast<CCharm*>(m_Charms[CharmIndex])->Get_CharmType();

					if ((_uint)CHARM_TYPE::NONE != NewCharmType)
					{
						for (_uint i = 0; i < m_LookItems[(_uint)ITEM_TYPE::CHARM].size(); ++i)
						{
							if (nullptr != m_LookItems[(_uint)ITEM_TYPE::CHARM][i] &&
								static_cast<CCharm*>(m_LookItems[(_uint)ITEM_TYPE::CHARM][i])->Get_CharmType() == NewCharmType)
							{
								//이전데잍타정리
								m_CharmDatas ^= NewCharmType;
								if (nullptr != m_LookItems[(_uint)ITEM_TYPE::CHARM][i])
								{
									static_cast<CCharm*>(m_LookItems[(_uint)ITEM_TYPE::CHARM][i])->UnEquip_Charm();
									Safe_Release(m_LookItems[(_uint)ITEM_TYPE::CHARM][i]);
									m_LookItems[(_uint)ITEM_TYPE::CHARM][i] = nullptr;
								}

								// 부적 UI에 적용
								pTabUI->Set_Charm_Equipped((CHARM_TYPE)NewCharmType, false);
								pTabUI->Set_CharmToSlot(i, CHARM_TYPE::NONE);

								//break;
							}
						}
					}

					//이전데잍타정리
					if (m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex] != nullptr)
					{
						_uint		OldCharmType = static_cast<CCharm*>(m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex])->Get_CharmType();
						m_CharmDatas ^= OldCharmType;
						static_cast<CCharm*>(m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex])->UnEquip_Charm();
						Safe_Release(m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex]);

						// 부적 UI에 적용
						pTabUI->Set_Charm_Equipped((CHARM_TYPE)OldCharmType, false);
						//pTabUI->Set_CharmToSlot(LookItemIndex, CHARM_TYPE::NONE);
					}

					//지금데이타 설정
					m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex] = m_Charms[CharmIndex];
					m_CharmDatas |= NewCharmType;
					static_cast<CCharm*>(m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex])->Equip_Charm();
					Safe_AddRef(m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex]);

					// 부적 UI에 적용
					pTabUI->Set_Charm_Equipped((CHARM_TYPE)NewCharmType, true);
					pTabUI->Set_CharmToSlot(LookItemIndex, (CHARM_TYPE)NewCharmType);

					return true;
				}
			}
			else
			{
				if (m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex] != nullptr)
				{
					_uint		CharmType = static_cast<CCharm*>(m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex])->Get_CharmType();
					m_CharmDatas ^= CharmType;
					static_cast<CCharm*>(m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex])->UnEquip_Charm();
					Safe_Release(m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex]);
					m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex] = nullptr;

					// 부적 UI에 적용
					pTabUI->Set_Charm_Equipped((CHARM_TYPE)CharmType, false);
					pTabUI->Set_CharmToSlot(LookItemIndex, CHARM_TYPE::NONE);
				}
			}
		}
	}
	return false;
}

void CInventory::Delete_Charm(_uint LookItemIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTabUI* pTabUI = static_cast<CTabUI*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 0, SORTLAYER_UI));
	RELEASE_INSTANCE(CGameInstance);
	if (nullptr == pTabUI)
		return;

	if (LookItemIndex < (m_LookItems[(_uint)ITEM_TYPE::CHARM]).size())
	{
		if (m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex] != nullptr)
		{
			_uint		CharmType = static_cast<CCharm*>(m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex])->Get_CharmType();
			m_CharmDatas ^= CharmType;
			static_cast<CCharm*>(m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex])->UnEquip_Charm();
			Safe_Release(m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex]);
			m_LookItems[(_uint)ITEM_TYPE::CHARM][LookItemIndex] = nullptr;

			// 부적 UI에 적용
			pTabUI->Set_Charm_Equipped((CHARM_TYPE)CharmType, false);
			pTabUI->Set_CharmToSlot(LookItemIndex, CHARM_TYPE::NONE);
		}
	}
}

_bool CInventory::Get_IsOn(CHARM_TYPE Type)
{
	return m_CharmDatas & (_uint)Type;
}

_bool CInventory::Get_IsOn(_uint Type)
{
	return m_CharmDatas & Type;
}

_uint CInventory::Get_CharmDatas()
{
	return m_CharmDatas;
}

//이건 앵간하면 쓰지않는게 좋겠네요?..
_bool CInventory::Change_UsingItem(KEYTAG Tag, CItem* Item)
{
	if (m_UsingItems[(_uint)Tag] == Item)
		return false;
	
	m_UsingItems[(_uint)Tag] = Item;
	return true;
}

_bool CInventory::Change_UsingItem(KEYTAG Tag, _uint SlotY, _uint SlotX)
{
	if (SlotY < 1 || SlotY > 2)
		return false;
	if (SlotX < 0 || SlotX >= (m_LookItems[SlotY]).size())
		return false;
	if (m_UsingItems[(_uint)Tag] == m_LookItems[SlotY][SlotX])
		return false;

	m_UsingItems[(_uint)Tag] = m_LookItems[SlotY][SlotX];
	return true;
}

void CInventory::Get_ItemType(KEYTAG Key, USING_TYPE* UsingType)
{
	if (m_UsingItems[(_uint)Key] != nullptr)
		*UsingType = m_UsingItems[(_uint)Key]->Get_UsingType();
	else
		*UsingType = USING_TYPE::TYPE_END;
}

CItem* CInventory::Get_UsingItem(KEYTAG Key)
{

	if (m_UsingItems[(_uint)Key] == nullptr)
		return nullptr;
	return m_UsingItems[(_uint)Key];
}

void CInventory::Create_Potion()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_Potion = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", L"Prototype_GameObject_Potion", nullptr, SORTLAYER_OBJ);
	pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", TEXT("Prototype_GameObject_Potion"), m_Potion, SORTLAYER_OBJ);
	RELEASE_INSTANCE(CGameInstance);
}

_uint CInventory::Get_LookItemsSize(ITEM_TYPE eType)
{
	if (ITEM_TYPE::TYPE_END <= eType)
		return 0;

	return (_uint)m_LookItems[(_uint)eType].size();
}

void CInventory::Use_Item(KEYTAG Key)
{
	if (m_UsingItems[(_uint)Key] == nullptr)
		return;

	m_LastUse = m_UsingItems[(_uint)Key];
}

//어차피 0이면 안쓰지만 혹시모르니까 조정해놓는단마인드
_bool CInventory::Use_ConsumableItem(KEYTAG Key, _uint* ConsumableType)
{
	if (m_UsingItems[(_uint)Key] == nullptr)
		return false;
	_uint NumItem = ((CConsumableItem*)m_UsingItems[(_uint)Key])->Use_Item();
	if(ConsumableType != nullptr)
		*ConsumableType = (_uint)((CConsumableItem*)m_UsingItems[(_uint)Key])->Get_ConsumableType();

	if (NumItem <= 0)
	{
		auto iter = find(m_LookItems[(_uint)ITEM_TYPE::CONSUMABLE].begin(), m_LookItems[(_uint)ITEM_TYPE::CONSUMABLE].end(), m_UsingItems[(_uint)Key]);
		(*iter)->Set_Dead(true);
		Safe_Release(*iter);
		m_LookItems[(_uint)ITEM_TYPE::CONSUMABLE].erase(iter);
		m_UsingItems[(_uint)Key] = nullptr;
	}
	m_LastUse = nullptr;

	// 인벤토리 UI 수치 조정
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTabUI* pTabUI = static_cast<CTabUI*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 0, SORTLAYER_UI));
	if (nullptr == pTabUI)
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	pTabUI->Set_Consumable_Quantity(Key, (CONSUMABLEITEM_TYPE)*ConsumableType, NumItem);

	RELEASE_INSTANCE(CGameInstance);

	return true;
}

_uint CInventory::Get_ItemQuantityByKey(KEYTAG Key)
{
	if (m_UsingItems[(_uint)Key] == nullptr)
		return 0;

	if (ITEM_TYPE::CONSUMABLE != m_UsingItems[(_uint)Key]->Get_ItemType())
		return 0;

	return ((CConsumableItem*)m_UsingItems[(_uint)Key])->Get_NumItem();
}

void CInventory::Add_Geer(GEAR_TYPE Geer)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CItem* pItem = nullptr;
	switch (Geer)
	{
	case GEAR_TYPE::SHIELD:
		pItem = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", L"Prototype_GameObject_Shield", nullptr, SORTLAYER_OBJ);
		m_Geers[(_uint)GEAR_TYPE::SHIELD] = pItem;
		pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", L"Prototype_GameObject_Shield", pItem, SORTLAYER_OBJ);
		break;

	case GEAR_TYPE::CAPE:
		pItem = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", L"Prototype_GameObject_Cape", nullptr, SORTLAYER_OBJ);
		m_Geers[(_uint)GEAR_TYPE::CAPE] = pItem;
		pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", L"Prototype_GameObject_Cape", pItem, SORTLAYER_OBJ);
		break;
	}

	// 인벤토리 UI에 추가합니다
	CTabUI* pTabUI = static_cast<CTabUI*>(pGameInstance->Get_GameObject(TEXT("Layer_UI_Inventory"), 0, SORTLAYER_UI));
	pTabUI->Add_Gear(Geer);

	RELEASE_INSTANCE(CGameInstance);
}

CItem* CInventory::Get_Geer(GEAR_TYPE Geer)
{
	if((_uint)Geer >= m_Geers.size())
		return nullptr;

	//없어도 nullptr이니까...
	return m_Geers[(_uint)Geer];
}



void CInventory::Free()
{
	for (auto& Geer : m_Geers)
		Safe_Release(Geer);
	m_Geers.clear();

	for (auto& Charm : m_Charms)
		Safe_Release(Charm);
	m_Charms.clear();

	Safe_Release(m_Potion);
	
	for (_uint i = 0; i < (_uint)ITEM_TYPE::TYPE_END; ++i)
	{
		for (auto& LookItem : m_LookItems[i])
		{
			Safe_Release(LookItem);
		}
		m_LookItems[i].clear();
	}
	
	for (_uint i = 0; i < (_uint)KEYTAG::KEY_END; ++i)
	{
		m_UsingItems[i] = nullptr;
	}

	m_LastUse = nullptr;
}

#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Item.h"
BEGIN(Engine)
END

BEGIN(Client)

//인벤토리와 탭키를 눌렀을 때 생기는 UI는 별개로 두는게 맞다고 생각함니다
class CInventory : public CBase
{
public:
	//enum GEER {SHIELD, GEER_END };			// 장비템은 client defines에 이미 정의해둔게 있어서 대체합니다...
	DECLARE_SINGLETON(CInventory)
protected:
	explicit CInventory();
	virtual ~CInventory() = default;
public:
	//장비형아이템만 이게돌것임.
	void LastUseTick(_float fTimeDelta);
	void LastUseLateTick(_float fTimeDelta);
	void LastUseRender(_uint iRenderGroup);
	//void LastUseRenderShadow();

///// <장착형아이템들 집어넣는코드 뺄일은 없다>
	void Add_Geer(GEAR_TYPE Geer);
	CItem* Get_Geer(GEAR_TYPE Geer);


///// <키슬롯에 올려놓지않는 아이템들>
	_bool Add_PotionMax(_uint Potion = 1);
	_bool Add_PotionPiece(_uint Piece = 1);
	_bool Add_Gold(_uint Gold);
	void Add_Key();
	void Fill_Potion();
	_bool Use_Key();	

	//bool값을 리턴하여 골드사용이 가능한지를 판단함
	_bool UseAble_Gold(_uint Gold);
	//골드사용시 부름
	_bool Use_Gold(_uint Gold);
	_uint Get_Gold() { return m_iGold; }
	//포션사용시 부름
	_bool Use_Potion(_float* HealAmount);
	_bool Get_Potion_Useable();


/////<CHARM>
	_bool Add_Charm(CItem* Charm);  //어차피 똑같은 아이템을 먹을일은 없음
	_bool Add_Charm(CHARM_TYPE CharmType);
//m_Charms에서 고른 녀석을 m_LookItems로 옮기면서 효과도 적용하는 방식인데 고민중임
	_bool Change_Charm(_uint CharmIndex, _uint LookItemIndex);
	void  Delete_Charm(_uint LookItemIndex);
	//이 참 기능이 켜져있는지 판단.
	//if(pInventory->Get_IsOn(CHARM_TYPE::CONVERTPOTION)) -> HP포션을 MP포션으로 바꿈
	_bool Get_IsOn(CHARM_TYPE Type);
	_bool Get_IsOn(_uint Type);
	_uint Get_CharmDatas();
	_bool SizeUp_Charm();
	_uint GetCharmSlotSize();
	CHARM_TYPE Get_LookItemCharmType(_uint iLookItemCharmSlotIndex);


///// <USINGITEM>
	_bool Add_ConsumableItem(_uint ConsumableItemType, _uint NumItem);
	_bool Add_Weapon(CItem* Weapon);//어차피 똑같은 아이템을 먹을일은 없음z
	_bool Change_UsingItem(KEYTAG Tag, CItem* Item);//몇번키의 아이템을 바꿀건지, 어떤아이템을 줄건지.
	_bool Change_UsingItem(KEYTAG Tag, _uint SlotY, _uint SlotX);//슬롯Y는 벡터의 배열 슬롯X는 벡터의 몇번쨰인지
	//이 키를 눌렀을떄 아이템이 어떤건지
	//아이템의 사용방식에 따른? USING_TYPE까지 리턴될거임
	void Get_ItemType(KEYTAG Key, USING_TYPE* UsingType);
	CItem*  Get_UsingItem(KEYTAG Key);
	_uint Get_LookItemsSize(ITEM_TYPE eType);
	void Use_Item(KEYTAG Key);	//아이템을 쓰게된다면 무조건 이걸 불러주세요 왜냐하면 라스트유즈된 녀석만 틱이랑 레이트틱이 돌아야 맞으니까요~
	void Create_Potion();
	_bool Use_ConsumableItem(KEYTAG Key, _uint* ConsumableType = nullptr);
	_uint Get_ItemQuantityByKey(KEYTAG Key);

private:
	_uint			m_iPotionMax = 0;					// 일단 플레이어 test쪽에서 최대포션 세팅합니다
	_uint			m_iPotion = 0;
	_float			m_PotionHealAmount = 100.f;
	_uint			m_iPotionPiece = 0;
	_uint			m_iGold = 0;						// 플레이어 test쪽에서 골드 세팅중
	_uint			m_Key = 0;
	vector<CItem*>	m_Charms;//부적들(순수 갖고있기용 미사용)
	vector<CItem*>	m_Geers; //평생장착형 아이템들인데 이렇게 부르더라구요
	vector<CItem*>  m_LookItems[(_uint)ITEM_TYPE::TYPE_END];//0 == 부적슬롯(사용중) , 1 == 소모성아이템, 2 == 무기류
	CItem*			m_UsingItems[(_uint)KEYTAG::KEY_END] = {};
	CItem*			m_LastUse = nullptr;

	CItem*			m_Potion = nullptr;

	_uint			m_CharmDatas = _uint(CHARM_TYPE::NONE);
public:	
	virtual void Free() override;
};

END
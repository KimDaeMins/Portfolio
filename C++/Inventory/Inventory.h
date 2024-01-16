#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Item.h"
BEGIN(Engine)
END

BEGIN(Client)

//�κ��丮�� ��Ű�� ������ �� ����� UI�� ������ �δ°� �´ٰ� �����Դϴ�
class CInventory : public CBase
{
public:
	//enum GEER {SHIELD, GEER_END };			// ������� client defines�� �̹� �����صа� �־ ��ü�մϴ�...
	DECLARE_SINGLETON(CInventory)
protected:
	explicit CInventory();
	virtual ~CInventory() = default;
public:
	//����������۸� �̰Ե�����.
	void LastUseTick(_float fTimeDelta);
	void LastUseLateTick(_float fTimeDelta);
	void LastUseRender(_uint iRenderGroup);
	//void LastUseRenderShadow();

///// <�����������۵� ����ִ��ڵ� ������ ����>
	void Add_Geer(GEAR_TYPE Geer);
	CItem* Get_Geer(GEAR_TYPE Geer);


///// <Ű���Կ� �÷������ʴ� �����۵�>
	_bool Add_PotionMax(_uint Potion = 1);
	_bool Add_PotionPiece(_uint Piece = 1);
	_bool Add_Gold(_uint Gold);
	void Add_Key();
	void Fill_Potion();
	_bool Use_Key();	

	//bool���� �����Ͽ� ������� ���������� �Ǵ���
	_bool UseAble_Gold(_uint Gold);
	//������ �θ�
	_bool Use_Gold(_uint Gold);
	_uint Get_Gold() { return m_iGold; }
	//���ǻ��� �θ�
	_bool Use_Potion(_float* HealAmount);
	_bool Get_Potion_Useable();


/////<CHARM>
	_bool Add_Charm(CItem* Charm);  //������ �Ȱ��� �������� �������� ����
	_bool Add_Charm(CHARM_TYPE CharmType);
//m_Charms���� �� �༮�� m_LookItems�� �ű�鼭 ȿ���� �����ϴ� ����ε� �������
	_bool Change_Charm(_uint CharmIndex, _uint LookItemIndex);
	void  Delete_Charm(_uint LookItemIndex);
	//�� �� ����� �����ִ��� �Ǵ�.
	//if(pInventory->Get_IsOn(CHARM_TYPE::CONVERTPOTION)) -> HP������ MP�������� �ٲ�
	_bool Get_IsOn(CHARM_TYPE Type);
	_bool Get_IsOn(_uint Type);
	_uint Get_CharmDatas();
	_bool SizeUp_Charm();
	_uint GetCharmSlotSize();
	CHARM_TYPE Get_LookItemCharmType(_uint iLookItemCharmSlotIndex);


///// <USINGITEM>
	_bool Add_ConsumableItem(_uint ConsumableItemType, _uint NumItem);
	_bool Add_Weapon(CItem* Weapon);//������ �Ȱ��� �������� �������� ����z
	_bool Change_UsingItem(KEYTAG Tag, CItem* Item);//���Ű�� �������� �ٲܰ���, ��������� �ٰ���.
	_bool Change_UsingItem(KEYTAG Tag, _uint SlotY, _uint SlotX);//����Y�� ������ �迭 ����X�� ������ ���������
	//�� Ű�� �������� �������� �����
	//�������� ����Ŀ� ����? USING_TYPE���� ���ϵɰ���
	void Get_ItemType(KEYTAG Key, USING_TYPE* UsingType);
	CItem*  Get_UsingItem(KEYTAG Key);
	_uint Get_LookItemsSize(ITEM_TYPE eType);
	void Use_Item(KEYTAG Key);	//�������� ���Եȴٸ� ������ �̰� �ҷ��ּ��� �ֳ��ϸ� ��Ʈ����� �༮�� ƽ�̶� ����Ʈƽ�� ���ƾ� �����ϱ��~
	void Create_Potion();
	_bool Use_ConsumableItem(KEYTAG Key, _uint* ConsumableType = nullptr);
	_uint Get_ItemQuantityByKey(KEYTAG Key);

private:
	_uint			m_iPotionMax = 0;					// �ϴ� �÷��̾� test�ʿ��� �ִ����� �����մϴ�
	_uint			m_iPotion = 0;
	_float			m_PotionHealAmount = 100.f;
	_uint			m_iPotionPiece = 0;
	_uint			m_iGold = 0;						// �÷��̾� test�ʿ��� ��� ������
	_uint			m_Key = 0;
	vector<CItem*>	m_Charms;//������(���� �����ֱ�� �̻��)
	vector<CItem*>	m_Geers; //��������� �����۵��ε� �̷��� �θ����󱸿�
	vector<CItem*>  m_LookItems[(_uint)ITEM_TYPE::TYPE_END];//0 == ��������(�����) , 1 == �Ҹ𼺾�����, 2 == �����
	CItem*			m_UsingItems[(_uint)KEYTAG::KEY_END] = {};
	CItem*			m_LastUse = nullptr;

	CItem*			m_Potion = nullptr;

	_uint			m_CharmDatas = _uint(CHARM_TYPE::NONE);
public:	
	virtual void Free() override;
};

END
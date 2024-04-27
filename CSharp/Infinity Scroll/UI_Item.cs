using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class UI_Item : UI_Base
{
    Item  _curItem;

    enum Images
    {
        ItemImage,
        EquipImage
    }
    public override void Init()
    {
        
    }

    // Start is called before the first frame update
    void Awake()
    {
        Bind<Image>(typeof(Images));

        gameObject.BindEvent(OnClickedItemClick);
    }


    public void OnClickedItemClick(PointerEventData data)
    {
        if (_curItem == null)
            return;

        UI_ItemInfo popup = Managers.Instance.TogglePopupUI<UI_ItemInfo>();
        popup.Refresh(_curItem, this);
    }
    public void SellItem()
    {
        Inventory.Instance.SellItem(_curItem);
        _curItem = null;
        Refresh();
    }

    public override void Refresh()
    {
        if (_curItem == null)
        {
            Clear();
            return;
        }

        GetImage((int)Images.ItemImage).gameObject.SetActive(true);
        GetImage((int)Images.ItemImage).sprite = _curItem._data.icon;

        if (_curItem.IsEquip)
            GetImage((int)Images.EquipImage).gameObject.SetActive(true);
        else
            GetImage((int)Images.EquipImage).gameObject.SetActive(false);

    }
    internal void Set(ItemSlot itemSlot)
    {
        if(itemSlot.item == null)
        {
            Clear();
            return;
        }

        _curItem = itemSlot.item;
        
        GetImage((int)Images.ItemImage).gameObject.SetActive(true);
        GetImage((int)Images.ItemImage).sprite = _curItem._data.icon;
        
        if(_curItem.IsEquip)
            GetImage((int)Images.EquipImage).gameObject.SetActive(true);
        else
            GetImage((int)Images.EquipImage).gameObject.SetActive(false);
    }

    internal void Clear()
    {
        _curItem = null;
        GetImage((int)Images.ItemImage).sprite = null;
        GetImage((int)Images.EquipImage).gameObject.SetActive(false);
        GetImage((int)Images.ItemImage).gameObject.SetActive(false);
    }
}

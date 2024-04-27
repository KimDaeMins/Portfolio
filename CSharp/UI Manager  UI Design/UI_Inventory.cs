using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;


public class UI_Inventory : UI_Popup
{
    enum Buttons
    {
        QuitButton
    }
    enum Texts
    {
        InvenCountText
    }
    public GameObject _orgItemPrefab;
    public float _itemHeight;
    public float _itemWidth;
    public int _horizonCount;
    //이제 추가해야할건 오프셋
    //사이간격
    private Inventory _inven;
    private List<UI_Item> _itemList;


    private ScrollRect _scroll;
    private RectTransform _scrollRect;
    public float _paddingLeft;
    public float _paddingTop;
    public float _spacingX;
    private float _offset;
    protected override void Awake()
    {
        base.Awake();
        _scroll = Util.FindChild(gameObject , "Scroll View").GetComponent<ScrollRect>();
        _scroll.horizontal = false;
        _scrollRect = _scroll.GetComponent<RectTransform>();
        if (_scroll.content.transform.TryGetComponent(out GridLayoutGroup glg))
        {
            _paddingLeft = glg.padding.left;
            _paddingTop = glg.padding.top;

            _spacingX = glg.spacing.x;
            _itemHeight += glg.spacing.y;
            glg.enabled = false;
        }
        foreach (Transform child in _scroll.content.transform)
            Managers.Instance.Destroy(child.gameObject);


        Bind<TextMeshProUGUI>(typeof(Texts));
        Bind<Button>(typeof(Buttons));

        _inven = Inventory.Instance;
    }

    // Start is called before the first frame update
    void Start()
    {
        CreateItem();
        SetContentHeight();

        GetButton((int)Buttons.QuitButton).gameObject.BindEvent(OnQuitButtonClicked);
    }

    public override void Refresh()
    {
        GetText((int)Texts.InvenCountText).text = $"{_inven.GetNowItemCount()} / {_inven.SlotLength}";
    }
    public void OnQuitButtonClicked(PointerEventData data)
    {
        Managers.Instance.ClosePopupUI(this);
    }
    private void CreateItem()
    {
        _itemList = new List<UI_Item>();

        int itemCount = (int)( _scrollRect.rect.height / _itemHeight) + 1 + 2;
        itemCount *= 3;

        for(int i = 0 ; i < itemCount ; ++i)
        {
            UI_Item item = Managers.Instance.MakeSubItem<UI_Item>("UI_Item" , _scroll.content);

            _itemList.Add(item);

            item.transform.localPosition = new Vector3
                (_paddingLeft + _itemWidth * (i % _horizonCount ) + _spacingX * ( i % _horizonCount ) ,
                -_paddingTop - _itemHeight * (i / _horizonCount ) , 0);

            SetData(_itemList[i] , i);
        }
        _offset = _itemHeight * ( _itemList.Count / _horizonCount );
    }

    private void SetContentHeight()
    {
        _scroll.content.sizeDelta = new Vector2(_scroll.content.sizeDelta.x,
            _paddingTop +  _itemHeight * ( _inven.SlotLength / _horizonCount)
            + ( _inven.SlotLength % _horizonCount != 0  ? _itemHeight : 0 ));
    }

    private bool ReLocationItem(int index , float contentY , float scrollHeight)
    {
        if (_itemList[index].transform.localPosition.y + contentY > _itemHeight * 2f)
        {
            _itemList[index].transform.localPosition -= new Vector3(0 , _offset , 0);
            ReLocationItem(index, contentY, scrollHeight);
            return true;
        }
        else if (_itemList[index].transform.localPosition.y + contentY < -scrollHeight - _itemHeight)
        {
            _itemList[index].transform.localPosition += new Vector3(0 , _offset , 0);
            ReLocationItem(index , contentY , scrollHeight);
            return true;
        }
        return false;
    }
    private void SetData(UI_Item item , int idx)
    {
        if(idx < 0 || idx >= _inven.SlotLength)
        {
            item.gameObject.SetActive(false);
            return;
        }
        item.gameObject.SetActive(true);
        item.Set(_inven._itemSlots[idx]);
    }
    // Update is called once per frame
    void Update()
    {
        float scrollHeight = _scrollRect.rect.height;
        float contentY = _scroll.content.anchoredPosition.y;
        for (int i = 0 ; i < _itemList.Count ; ++i)
        {
            bool isChanged = ReLocationItem(i, contentY, scrollHeight);
            
            if(isChanged)
            {
                int idx = (int)( -( _itemList[i].transform.localPosition.y + _paddingTop) / _itemHeight )  * _horizonCount + i % _horizonCount;
                SetData(_itemList[i] , idx);
            }
        }
    }

}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIManager : MonoBehaviour
{
    int _order = 10;

    LinkedList<UI_Popup> _popupList = new LinkedList<UI_Popup>();
    public UI_Scene SceneUI { get; private set; }

    private GameObject _root;
    public GameObject Root
    {
        get
        {
            if (_root == null)
            {
                _root = GameObject.Find("@UI_Root");
                if (_root == null)
                    _root = new GameObject { name = "@UI_Root" };
            }
            return _root;
        }
    }
    public void SetCanvas(GameObject go , bool sort = true)
    {
        UI_Popup popup = Util.FindRoot<UI_Popup>(go);

        if (popup == null)
        {
            SetSortOrder(go , sort);
            return;
        }

        SetCanvas(popup , sort);
    }
    public void SetCanvas(UI_Popup popup , bool sort = true)
    {
        _popupList.Remove(popup);
        _popupList.AddLast(popup);
        popup.gameObject.SetActive(true);

        SetSortOrder(popup.gameObject , sort);
    }
    private void SetSortOrder(GameObject go , bool sort)
    {
        Canvas canvas = go.GetOrAddComponent<Canvas>();
        canvas.renderMode = RenderMode.ScreenSpaceOverlay;
        canvas.overrideSorting = true;

        if (sort)
        {
            if (_order > 30000)
            {
                ResetSortOrder();
            }
            else
            {
                canvas.sortingOrder = _order;
                _order++;
            }
        }
        else
        {
            canvas.sortingOrder = 0;
        }
    }
    private void ResetSortOrder()
    {
        _order = 10;

        LinkedListNode<UI_Popup> currentNode = _popupList.First;

        while (currentNode != null)
        {
            UI_Popup ui = currentNode.Value;
            currentNode = currentNode.Next;
            if (ui == null)
                continue;

            if (ui.gameObject.activeSelf)
            {
                SetSortOrder(ui.gameObject , true);
            }
        }
    }
    public T MakeSubItem<T>(string name = null , Transform parent = null) where T : UI_Base
    {
        if (string.IsNullOrEmpty(name))
            name = typeof(T).Name;

        GameObject go = Managers.Instance.Instantiate($"UI/SubItem/{name}");

        if (parent != null)
            go.transform.SetParent(parent);

        return go.GetOrAddComponent<T>();
    }
    public T MakeWorldSpaceUI<T>(Transform parent = null , string name = null) where T : UI_Base
    {
        if (string.IsNullOrEmpty(name))
            name = typeof(T).Name;

        GameObject go = Managers.Instance.Instantiate($"UI/WorldSpace/{name}", parent);
        //if (parent != null)
        //    go.transform.SetParent(parent);

        Canvas canvas = go.GetOrAddComponent<Canvas>();
        canvas.renderMode = RenderMode.WorldSpace;
        canvas.worldCamera = Camera.main;

        return go.GetOrAddComponent<T>();
    }
    public T ShowSceneUI<T>(string name = null) where T : UI_Scene
    {
        if (string.IsNullOrEmpty(name))
        {
            name = typeof(T).Name;
        }

        GameObject go = Managers.Instance.Instantiate($"UI/Scene/{name}");
        go.name = name;

        T scene = Util.GetOrAddComponent<T>(go);
        SceneUI = scene;

        go.transform.SetParent(Root.transform);

        return scene;
    }
    public T TogglePopupUI<T>(string name = null) where T : UI_Popup
    {
        T popup = FindPopup<T>(name);
        if (popup == null)
        {
            popup = CreatePopupUI<T>(name);
        }
        else if(popup.gameObject.activeSelf)
        {
            ClosePopupUI(popup);
        }
        else
        {
            SetCanvas(popup);
        }

        return popup;
    }
    public void TogglePopupUI<T>(T popup) where T : UI_Popup
    {
        if(popup == null)
        {
            popup = ShowPopupUI<T>(typeof(T).Name);
        }
        else if(popup.gameObject.activeSelf)
        {
            ClosePopupUI(popup);
        }
        else
        {
            SetCanvas(popup);
        }
    }
    public T ShowPopupUI<T>(string name = null) where T : UI_Popup
    {
        T popup = FindPopup<T>(name);
        if (popup == null)
        {
            popup = CreatePopupUI<T>(name);
        }
        else
        {
            SetCanvas(popup);
        }

        return popup;
    }
    private T CreatePopupUI<T>(string name = null) where T : UI_Popup
    {
        if (string.IsNullOrEmpty(name))
        {
            name = typeof(T).Name;
        }

        GameObject go = Managers.Instance.Instantiate($"UI/Popup/{name}");

        go.name = name;
        T popup = Util.GetOrAddComponent<T>(go);
        popup.transform.SetParent(Root.transform);
        return popup;
    }
    public T FindPopup<T>(string name = null) where T : UI_Popup
    {
        LinkedListNode<UI_Popup> currentNode = _popupList.Last;

        if (string.IsNullOrEmpty(name))
        {
            name = typeof(T).Name;
        }

        while (currentNode != null)
        {
            UI_Popup ui = currentNode.Value;
            currentNode = currentNode.Previous;
            if (ui == null)
                continue;

            if (ui.name == name)
            {
                if (ui.gameObject.activeSelf)
                    return ui as T;
            }
        }
        return null;
    }
    public T GetLastPopupUI<T>()where T : UI_Popup
    {
        if (_popupList.Count == 0)
            return null;

        return _popupList.Last.Value as T;
    }
    public void OpenPopupRefresh()
    {
        LinkedListNode<UI_Popup> currentNode = _popupList.Last;
        while (currentNode != null)
        {
            UI_Popup ui = currentNode.Value;
            if (ui == null)
                break;

            if (ui.gameObject.activeSelf)
            {
                ui.Refresh();
            }
            currentNode = currentNode.Previous;
        }
    }
    public void ClosePopupUI(UI_Popup popup = null)
    {
        if (_popupList.Count == 0)
            return;

        if (popup == null)
            popup = _popupList.Last.Value;

        if (popup._useSetActive)
            HidePopupUI(popup);
        else
            DestroyPopupUI(popup);
    }
    private void HidePopupUI(UI_Popup popup)
    {
        popup.gameObject.SetActive(false);
    }
    private void DestroyPopupUI(UI_Popup popup)
    {
        _popupList.Remove(popup);
        Managers.Instance.Destroy(popup.gameObject);
        popup = null;
    }
    public void CloseAllPopupUI()
    {
        LinkedListNode<UI_Popup> currentNode = _popupList.Last;
        while(currentNode != null)
        {
            UI_Popup ui = currentNode.Value;
            currentNode = currentNode.Previous;
            if (ui == null)
                continue;
            ClosePopupUI(ui);
        }
        _order = 10;
    }
    public void Clear()
    {
        CloseAllPopupUI();
        _popupList.Clear();
        SceneUI = null;
        _root = null;
    }
}

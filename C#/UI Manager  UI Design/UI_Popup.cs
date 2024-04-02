using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class UI_Popup : UI_Base
{
    public bool _useSetActive = true;
    public void UseSetActive()
    {
        _useSetActive = true;
    }
    public void UseDestroy()
    {
        _useSetActive = false;
    }

    protected virtual void Awake()
    {
        Init();
    }

    public override void Init()
    {
        Managers.Instance.SetCanvas(this , true);
    }

    public virtual void ClosePopupUI()
    {
        Managers.Instance.ClosePopupUI(this);
    }
}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class UI_Scene : UI_Base
{
    protected virtual void Awake()
    {
        Init();
    }

    public override void Init()
    {
        Managers.Instance.SetCanvas(gameObject , false);
    }

}

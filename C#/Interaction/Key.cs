using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Key : MonoBehaviour
{
    protected UI_Interaction _go = null;

    protected LayerMask _playerLayerMask;

    [Header("DoorDefaultSetting")]
    [SerializeField] protected BoxCollider2D _doorCollider;

    protected virtual void Awake()
    {
        _playerLayerMask = LayerMask.GetMask("Player");
    }

    public bool KeyInteraction()
    {
        //키를 얻기위한 조건 true; 최대갯수등을 체크해서 막을순있음
        return true;
    }

    public void AddKey()
    {
        GameObject player = GameObject.FindWithTag("Player");
        Inventory inventory = player.GetComponent<Inventory>();
        inventory.AddKey();
        Managers.Resource.Destroy(this);
    }
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (_go)
            return;

        if (0 != ( _playerLayerMask.value & ( 1 << collision.gameObject.layer ) ))
        {
            _go = Managers.UI.ShowPopupUI<UI_Interaction>();
            _go.Refresh(transform.position);
            _go.AddDelegate(KeyInteraction);
            _go.OnEndInteraction += AddKey;
        }
    }
    private void OnTriggerExit2D(Collider2D collision)
    {
        Managers.Resource.Destroy(_go);
        _go = null;
    }
}

using System;
using System.Collections;
using System.Collections.Generic;
using Unity.IO.LowLevel.Unsafe;
using UnityEngine;

public class CharacterController : MonoBehaviour
{
    public event Action<Vector2> OnMoveEvent;  
    public event Action<Vector2> OnLookEvent;
    public event Action OnKeyDown;
    public event Action OnKeyPress;
    public event Action OnKeyUp;
    public event Action OnInteractionEvent;
    public event Action OnAttackEvent;
    public event Action OnChangeActiveEvent;
    public event Action OnUseActiveEvent;
    public event Action OnThrowWeaponEvent;
    public event Action OnChangeWeaponEvent;
    public event Action OnRollEvent;
    public event Action OnReloadEvent;
    protected bool IsAttacking { get; set; }
    protected CharacterStatsHandler Stats { get; private set; }

    protected virtual void Awake()
    {
        Stats = GetComponent<CharacterStatsHandler>();
    }

    public void CallMoveEvent(Vector2 direction)
    {
        OnMoveEvent?.Invoke(direction);
    }

    public void CallLookEvent(Vector2 direction)
    {
        OnLookEvent?.Invoke(direction);
    }

    public void CallAttackEvent()
    {
        OnAttackEvent?.Invoke();
    }

    public void CallChangeActiveEvent()
    {
        OnChangeActiveEvent?.Invoke();
    }

    public void CallUseActiveEvent()
    {
        OnUseActiveEvent?.Invoke();
    }
    public void CallThrowWeaponEvent()
    {
        OnThrowWeaponEvent?.Invoke();
    }
    public void CallChangeWeaponEvent()
    {
        OnChangeWeaponEvent?.Invoke();
    }
    public void CallKeyDown()
    {
        OnKeyDown?.Invoke();
    }
    public void CallKeyPress()
    {
        OnKeyPress?.Invoke();
    }
    public void CallKeyUp()
    {
        OnKeyUp?.Invoke();
    }
    public void CallInteractionEvent()
    {
        OnInteractionEvent?.Invoke();
    }
    public void CallReloadEvent()
    {
        OnReloadEvent?.Invoke();
    }
    public void EquipWeapon(Gun weapon)
    {
        weapon.Equip(ref OnKeyDown , ref OnKeyPress , ref OnKeyUp , ref OnRollEvent , ref OnLookEvent, ref OnReloadEvent);
    }
    public void UnEquipWeapon(Gun weapon)
    {
        weapon.UnEquip(ref OnKeyDown , ref OnKeyPress , ref OnKeyUp , ref OnRollEvent , ref OnLookEvent, ref OnReloadEvent);
    }
}
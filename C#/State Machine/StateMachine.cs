using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class StateMachine : MonoBehaviour
{
    public IState CurrentState { get; private set; }
    public uint CurrentKey { get; private set; }
    private readonly Dictionary<uint, IState> _states = new();
    public bool updateState { get; set; } = false;
    protected bool AddState(uint eState, IState state)
    {
        if (_states.ContainsKey(eState))
            return false;
        
        _states.Add(eState, state);
        return true;
    }
    public void ChangeState(uint key)
    {
        CurrentState?.Exit();
        CurrentState = _states[key];
        CurrentKey = key;
        updateState = true;
        CurrentState?.Enter();
    }
    public Coroutine ChangeState(uint key , float time)
    {
        return StartCoroutine(COChangeState(key , time));
    }
    public IEnumerator COChangeState(uint key, float time)
    {
        yield return new WaitForSeconds(time);
        ChangeState(key);
    }
    public void StopChangeState(Coroutine co)
    {
        StopCoroutine(co);
    }
    public void HandleInput()
    {
        CurrentState?.HandleInput();
    }
    public void Tick()
    {
        CurrentState?.Tick();
    }
    public void PhysicsUpdate()
    {
        CurrentState?.PhysicsUpdate();
    }
}

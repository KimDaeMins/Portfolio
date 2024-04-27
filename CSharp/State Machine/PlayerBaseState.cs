using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class PlayerBaseState : IState
{
    protected PlayerStateMachine stateMachine;
    protected Player player;
    
    protected PlayerBaseState(PlayerStateMachine stateMachine)
    {
        this.stateMachine = stateMachine;
        player = stateMachine.Player;
    }

    public virtual void Enter()
    {
        AddInputActionsCallbacks();
    }

    public virtual void Exit()
    {
        RemoveInputActionsCallbacks();
    }

    public virtual void HandleInput()
    {
    }

    public virtual void Tick()
    {
    }

    public virtual void PhysicsUpdate()
    {
    }

    protected virtual void AddInputActionsCallbacks()
    {
        player.Input.PlayerActions.Move.canceled += OnCanceledEffect;
    }
    protected virtual void RemoveInputActionsCallbacks()
    {
        player.Input.PlayerActions.Move.canceled -= OnCanceledEffect;
    }
    
    private void OnCanceledEffect(InputAction.CallbackContext context)
    {
        //player.Agent.destination
        //객체생성 파티클 위쪽 위치에
    }

    protected bool DestPosSetting()
    {
        if (Managers.Cursor.GetGroundRayPos(out var pos))
        {
            //player.Agent.SetDestination(pos);
            player.Destination = pos;
            return true;
        }

        return false;
    }

    protected void SetTrigger(int animationHash)
    {
        player.Animator.SetTrigger(animationHash);
    }
    protected void SetBool(int animationHash, bool value)
    {
        player.Animator.SetBool(animationHash, value);
    }
    protected void StartAnimation(string stateName)
    {
        player.Animator.Play(stateName);
    }

    protected float GetNormalizedTime(Animator animator, string tag)
    {
        AnimatorStateInfo currentInfo = animator.GetCurrentAnimatorStateInfo(0);
        AnimatorStateInfo nextInfo = animator.GetNextAnimatorStateInfo(0);
        Animator.StringToHash(tag);
        int a = nextInfo.tagHash;
        if (animator.IsInTransition(0) && nextInfo.IsTag(tag))
        {
            return nextInfo.normalizedTime;
        }
        else if(!animator.IsInTransition(0) && currentInfo.IsTag(tag))
        {
            return currentInfo.normalizedTime;
        }
        else
        {
            return 0;
        }
    }
    protected float GetNormalizedTime(Animator animator, int hash)
    {
        AnimatorStateInfo currentInfo = animator.GetCurrentAnimatorStateInfo(0);
        AnimatorStateInfo nextInfo = animator.GetNextAnimatorStateInfo(0);

        if (animator.IsInTransition(0) && nextInfo.tagHash == hash)
        {
            return nextInfo.normalizedTime;
        }
        else if(!animator.IsInTransition(0) && currentInfo.tagHash == hash)
        {
            return currentInfo.normalizedTime;
        }
        else
        {
            return 0;
        }
    }
}
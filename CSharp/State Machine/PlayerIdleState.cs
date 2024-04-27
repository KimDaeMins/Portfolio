using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerIdleState : PlayerBaseState
{
    public PlayerIdleState(PlayerStateMachine stateMachine) : base(stateMachine)
    {
    }
    
    public override void Enter()
    {
        base.Enter();
        SetBool(stateMachine.Player.AnimationData.IdleParameterHash, true);
    }

    public override void Exit()
    {
        base.Exit();
        SetBool(stateMachine.Player.AnimationData.IdleParameterHash, false);
    }

    public override void Tick()
    {
        if (player.IsMine && player.Input.PlayerActions.Move.IsPressed())
        {
            DestPosSetting();
        }

        if (player.Agent.remainingDistance > 0.05f)
        {
            stateMachine.ChangeState((uint)PlayerStateMachine.State.Move);
        }
    }
}

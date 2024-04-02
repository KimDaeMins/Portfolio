using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerJumpState : PlayerAirState
{
    public PlayerJumpState(PlayerStateMachine stateMachine) : base(stateMachine)
    {
    }

    public override void Enter()
    {
        stateMachine.JumpForce = player.Data.AirData.JumpForce;
        player.ForceReciver.Jump(stateMachine.JumpForce);
        base.Enter();

        StartAnimation(player.AnimationData.JumpParameterHash);
    }
    public override void Exit()
    {
        base.Exit();

        StopAnimation(player.AnimationData.JumpParameterHash);
    }

    public override void PhysicsUpdate()
    {
        base.PhysicsUpdate();

        if(player.Controller.velocity.y <= 0)
        {
            stateMachine.ChangeState(stateMachine.fallState);
            return;
        }
    }
}

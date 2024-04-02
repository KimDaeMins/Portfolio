using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerMoveState : PlayerBaseState
{
    public PlayerMoveState(PlayerStateMachine stateMachine) : base(stateMachine)
    {
    }
    public override void Enter()
    {
        base.Enter();
        SetBool(stateMachine.Player.AnimationData.MoveParameterHash, true);
    }

    public override void Exit()
    {
        base.Exit();
        player.Agent.ResetPath();
        player.transform.position = player.Destination;
        SetBool(stateMachine.Player.AnimationData.MoveParameterHash, false);
    }

    private Coroutine co;
    IEnumerator CoMoveCoolTime(float time)
    {
        yield return new WaitForSeconds(time);
        co = null;
    }
    public override void Tick()
    {
        if (player.IsMine && co == null && player.Input.PlayerActions.Move.IsPressed())
        {
            DestPosSetting();
            co = CoroutineHandler.Instance.Start_Coroutine(CoMoveCoolTime(0.1f));
        }

        if (player.Agent.remainingDistance < 0.05f)
        {
            stateMachine.ChangeState((uint)PlayerStateMachine.State.Idle);
        }
        else
        {
            player.transform.rotation = Quaternion.Slerp(
                player.transform.rotation,
                Quaternion.LookRotation(player.Agent.destination - player.transform.position),
                31.4f * Time.deltaTime
            );
        }
    }
}

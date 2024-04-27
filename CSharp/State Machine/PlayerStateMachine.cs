using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using UnityEngine;

public class PlayerStateMachine : StateMachine
{
    //이게 필요할까..?
    public enum State
    {
        Idle, Move, Attack, Hit, Victory, Dead, End
    }
    public Player Player { get; private set; }
    public void Init(Player player)
    {
        this.Player = player;

        AddState((uint)State.Idle, new PlayerIdleState(this));
        AddState((uint)State.Move, new PlayerMoveState(this));
        AddState((uint)State.Attack, new PlayerAttackState(this));
        AddState((uint)State.Hit, new PlayerHitState(this));
        AddState((uint)State.Victory, new PlayerVictoryState(this));
        AddState((uint)State.Dead, new PlayerDeadState(this));
    }

}
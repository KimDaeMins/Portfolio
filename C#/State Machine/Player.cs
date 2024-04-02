
using System.Collections;
using Google.Protobuf.Protocol;
using UnityEngine;
using UnityEngine.AI;

public class Player : MonoBehaviour
{
    [field: Header("References")]
    [field: SerializeField]
    public PlayerSO Data { get; private set; }

    [field: Header("Animations")]
    [field: SerializeField]
    public PlayerAnimationData AnimationData { get; private set; }


    public Animator Animator { get; private set; }
    public PlayerInput Input { get; private set; }
    private PlayerStateMachine _stateMachine;
    public NavMeshAgent Agent { get; private set; }
    public int Id { get; set; }
    private Vector3 _destination;

    public Vector3 Destination
    {
        get => Agent.destination;
        set
        {
            _destination = value;
            _destination.y = 0;
            _updated = true;
        }
    }

    private int _hp;

    public int Hp
    {
        get => _hp;
        set
        {
            _hp = value;
            if (_hp <= 0)
                _stateMachine.ChangeState((uint)PlayerStateMachine.State.Dead);
        }
    }

    private bool _updated = false;
    public bool IsMine { get; set; } = false;

    private void Awake()
    {
        AnimationData.Initialize();

        Animator = GetComponentInChildren<Animator>();
        Input = GetComponent<PlayerInput>();
        Agent = GetComponent<NavMeshAgent>();
        Agent.speed = Data.BaseSpeed;
        Agent.updateRotation = false;
        _stateMachine = GetComponent<PlayerStateMachine>();
        _stateMachine.Init(this);
        _stateMachine.ChangeState((uint)PlayerStateMachine.State.Idle);
    }

    public void SetState(int state)
    {
        _stateMachine.ChangeState((uint)state);
    }
    private void Start()
    {
        if (IsMine)
            StartCoroutine(nameof(SyncPos));
    }

    private void Update()
    {
        _stateMachine.HandleInput();
        _stateMachine.Tick();
        CheckUpdatedFlag();
    }

    private void FixedUpdate()
    {
        _stateMachine.PhysicsUpdate();
    }

    void OnDie()
    {
        Animator.SetTrigger("Die");
        enabled = false;
    }

    void CheckUpdatedFlag()
    {
        if (!IsMine)
            return;
        
        if (_updated)
        {

            C_Move movePacket = new C_Move
            {
                Dest = _destination.ToVec3()
            };

            Managers.Network.Send(movePacket);
            _updated = false;
        }

        if (_stateMachine.updateState)
        {
            C_SyncState statePacket = new C_SyncState
            {
                State = (int)_stateMachine.CurrentKey
            };
            _stateMachine.updateState = false;
            Managers.Network.Send(statePacket);
            _updated = false;
        }
    }

    private IEnumerator SyncPos()
    {
        while (true)
        {
            yield return new WaitForSeconds(0.15f);

            C_SyncPlayer syncData = new C_SyncPlayer()
            {
                Pos = transform.position.ToVec3()
            };
            Managers.Network.Send(syncData);
        }
    }
    public void SyncPos(Vector3 position)
    {
        Vector3 dir = position - transform.position;
        if (dir.magnitude <= 1.0f) return;
        transform.position = position;
        //transform.LookAt(transform.position + dir);
    }

    public void SyncState(int state)
    {
        if (state != _stateMachine.CurrentKey)
        {
            _stateMachine.ChangeState((uint)state);
        }
    }
}
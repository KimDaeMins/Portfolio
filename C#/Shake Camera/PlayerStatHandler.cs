using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using System;

using Photon.Pun;

public class PlayerStatHandler : MonoBehaviourPunCallbacks, IPunObservable
{
    [SerializeField] private PlayerStatSO initialStat;

    [field: SerializeField] public PlayerStat CurrentStat { get; private set; }
    public LinkedList<PlayerStat> statModifiers = new LinkedList<PlayerStat>();
 
    [SerializeField] private float healthChangeDelay = 0.5f;
    private float _timeSinceLastChange = float.MaxValue;
    private bool _invincibility = false;

    private bool _isReady = false;

    public event Action OnDamage;
    public event Action OnHeal;
    public event Action OnDeath;
    public event Action OnInvincibilityEnd;

    public Coroutine co;
    PhotonView _PV;

    private void Awake()    //테스트용 
    {
        CurrentStat = new PlayerStat();
        _PV = GetComponent<PhotonView>();
    }

    public override void OnEnable()
    {
        base.OnEnable();
        InitPlayerStat();
    }

    //TODO : 나중에 헬스시스템으로 따로 빼는게 괜찮긴할듯합니다
    public void SetInvincible(bool onoff)
    {
        if (!_PV.IsMine)
        {
            return;
        }

        if (co != null)
        {
            StopCoroutine(co);
        }

        _invincibility = onoff;
    }

    public bool ChangeHealth(float change)
    {
        if (!_PV.IsMine || change == 0  || _invincibility)
        {
            return false;
        }

        _timeSinceLastChange = 0f;
        CurrentStat.HP += change;
        CurrentStat.HP = Mathf.Clamp(CurrentStat.HP, 0.0f, CurrentStat.MaxHp);
        if (co != null)
        {
            StopCoroutine(co);
        }

        co = StartCoroutine(COInvincible(healthChangeDelay));
        
        if (change > 0)
        {
            OnHeal?.Invoke();
        }
        else
        {
            OnDamage?.Invoke();
            Camera.main.GetComponent<ShakeCamera>().Shake(ShakeType.Hit);
        }

        if (CurrentStat.HP <= 0.0f)
        {
            OnDeath?.Invoke();

            _invincibility = false;

            PhotonNetwork.Instantiate("Effects/Death", transform.position, Quaternion.identity);
        }

        return true;
    }

    public IEnumerator COInvincible(float Time)
    {
        _invincibility = true;
        yield return new WaitForSeconds(Time);
        _invincibility = false;
    }

    public void SetHealth(float change)
    {
        if (!_PV.IsMine)
        {
            return;
        }

        //이렇게 써도되겠지만 고민좀 해봅시다 네..
        CurrentStat.HP = change;
    }

    public void InitPlayerStat()
    {
        CurrentStat = new PlayerStat();
        if (!_PV.IsMine)
        {
            return;
        }

        CurrentStat.HP = initialStat.MaxHp;
        CurrentStat.MaxHp = initialStat.MaxHp;
        CurrentStat.MoveSpeed = initialStat.MoveSpeed;
        CurrentStat.JumpForce = initialStat.JumpForce;

        statModifiers.Clear();
        UpdateCharacterStats();

        _PV.RPC(nameof(ReadyRPC), RpcTarget.AllBuffered);

        OnDeath += DeathEvent;

        
    }

    [PunRPC]
    public void ReadyRPC()
    {
        _isReady = true;
    }

    public void AddStatModifier(PlayerStat statModifier)
    {
        if (!_PV.IsMine)
        {
            return;
        }

        statModifiers.AddLast(statModifier);
        UpdateCharacterStats();
    }

    public void RemoveStatModifier(PlayerStat statModifier)
    {
        if (!_PV.IsMine || !statModifiers.Contains(statModifier))
        {
            return;
        }
        
        statModifiers.Remove(statModifier);
        UpdateCharacterStats();
    }

    public void UpdateCharacterStats()
    {
        if (!_PV.IsMine)
        {
            return;
        }

        SetBaseStat();

        foreach (PlayerStat modifier in statModifiers.OrderBy(x => x.statsChangeType))
        {
            if (modifier.statsChangeType == Define.StatsChangeType.Override)
            {
                UpdateStat((o, o1) => o1, modifier);
            }
            else if (modifier.statsChangeType == Define.StatsChangeType.Add)
            {
                UpdateStat((o, o1) => o + o1, modifier);
            }
            else if (modifier.statsChangeType == Define.StatsChangeType.Multiple)
            {
                UpdateStat((o, o1) => o * o1, modifier);
            }
        }
    }

    private void SetBaseStat()
    {
        CurrentStat.MaxHp = initialStat.MaxHp;
        CurrentStat.MoveSpeed = initialStat.MoveSpeed;
        CurrentStat.JumpForce = initialStat.JumpForce;
    }

    private void UpdateStat(Func<float, float, float> operation, PlayerStat newModifier)
    {
        if(newModifier.MaxHp != 0)
            CurrentStat.MaxHp = operation(CurrentStat.MaxHp, newModifier.MaxHp);
        if (newModifier.JumpForce != 0)
            CurrentStat.JumpForce = operation(CurrentStat.JumpForce, newModifier.JumpForce);
        if (newModifier.MoveSpeed != 0)
            CurrentStat.MoveSpeed = operation(CurrentStat.MoveSpeed, newModifier.MoveSpeed);
    }

    public void OnPhotonSerializeView(PhotonStream stream, PhotonMessageInfo info)
    {
        if (!_isReady)
            return;

        if (stream.IsWriting)
        {
            stream.SendNext(CurrentStat.HP);
            stream.SendNext(CurrentStat.MaxHp);
        }
        else
        {
            CurrentStat.HP = (float)stream.ReceiveNext();
            CurrentStat.MaxHp = (float)stream.ReceiveNext();
        }
    }

    public void Hit(int damage)
    {
        ChangeHealth(-1 * damage);
    }

    private void DeathEvent()
    {
        GetComponent<PhotonView>().RPC("RPCSetActive", RpcTarget.All, false);
        GameManager.Instance.AddKnockoutPlayer(PhotonNetwork.LocalPlayer.ActorNumber);
    }
}

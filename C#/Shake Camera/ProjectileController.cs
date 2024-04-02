using UnityEngine;
using Weapon.Model;
using Photon.Pun;

namespace Weapon.Controller
{
    public class ProjectileController : MonoBehaviour
    {
        [field: SerializeField] public int Damage { get; private set; } = 0;
        [field: SerializeField] public float Speed { get; private set; } = 0;
        [field: SerializeField] public Vector2 Direction { get; private set; } = Vector2.zero;

        private Rigidbody2D _rigidbody;
        private PhotonView _photonView;

        private bool _isReady = false;

        protected virtual void Awake()
        {
            _rigidbody = GetComponent<Rigidbody2D>();
            _photonView = GetComponent<PhotonView>();
        }

        private void OnEnable()
        {
            Invoke(nameof(Disapear), 5f);
        }

        private void FixedUpdate()
        {
            if (!_isReady)
                return;

            _rigidbody.velocity = Direction * Speed;
        }

        private void OnTriggerEnter2D(Collider2D collision)
        {
            if (collision.CompareTag("Stage") || collision.CompareTag("FirstBreak") || collision.CompareTag("SecondBreak"))
                Disapear();

            if (!_photonView.IsMine && collision.CompareTag("Player") && collision.GetComponent<PhotonView>().IsMine)
            {
                collision.GetComponent<PlayerStatHandler>().Hit(Damage);
                Vector3 pos = collision.ClosestPoint(transform.position) - Direction * 0.3f;
                pos.y += 0.2f;
                PhotonNetwork.Instantiate("Effects/Blood", pos, Quaternion.identity);
                if(collision.transform.TryGetComponent<Rigidbody2D>(out var com))
                {
                    com.AddForce(Direction * 20, ForceMode2D.Impulse);
                    Camera.main.GetComponent<ShakeCamera>().Shake(ShakeType.Attack);
                }
                Disapear();
            }
        }

        public void Initialize(AttackData data, Vector2 direction)
        {
            _photonView.RPC(nameof(RPCInitial), RpcTarget.All, data.bulletDamage, data.bulletSpeed, direction);
            _photonView.RPC("RPCSetActive", RpcTarget.All, true);
            _isReady = true;
        }

        [PunRPC]
        private void RPCInitial(int damage, float speed, Vector2 direction)
        {
            Damage = damage;
            Speed = speed;
            Direction = direction;
        }

        private void Disapear()
        {
            _isReady = false;
            _photonView.RPC("RPCSetActive", RpcTarget.All, false);
        }
    }
}
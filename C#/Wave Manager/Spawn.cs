using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Spawn : Object_Base
{
    [SerializeField] private float _spawnDelay;
    private float _spawnCoolTime = 0;
    [SerializeField] private bool _instantCreate;

    private Queue<string> _names = new Queue<string>();
    private Queue<Vector3> _vecs = new Queue<Vector3>();
    private Queue<Quaternion> _quats = new Queue<Quaternion>();
    public GameObject _waveManager;

    private void Awake()
    {
        int childCount = transform.childCount;

        for(int i = 0 ; i <  transform.childCount ; ++i)
        {
            Transform t = transform.GetChild(i);
            
            if (t.TryGetComponent<Poolable>(out Poolable p))
                Destroy(p);

            int index = t.name.LastIndexOf(' ');

            if (index > 0)
                _names.Enqueue(t.name.Substring(0 , index));
            else
                _names.Enqueue(t.name);
            _vecs.Enqueue(t.position);
            _quats.Enqueue(t.rotation);
            Monster mon = t.GetComponent<Monster>();
            t.gameObject.SetActive(false);
        }

        //int check = 0;
        //int child = transform.childCount;
        foreach (Transform t in transform)
        {
            //++check;
            Managers.Resource.Destroy(t.gameObject);
        }
        //if (check != child)
        //    Debug.Log($"?{check}");
    }
    private void Update()
    {
        if(_names.Count == 0)
        {
            _waveManager.GetComponent<WaveManager>().ResetNowSpawn();
            Managers.Resource.Destroy(this.gameObject);
            return;
        }

        if (!_instantCreate)
        {
            _spawnCoolTime += Time.deltaTime;
            if (_spawnDelay < _spawnCoolTime)
            {
                _spawnCoolTime = 0;
                Create();
            }
        }
        else
        {
            while(_names.Count != 0)
            {
                Create();
            }
        }
    }

    protected virtual void Create()
    {
        Monster ob = Managers.Resource.Instantiate(_names.Dequeue() , _vecs.Dequeue() , _quats.Dequeue()).GetComponent<Monster>();
    }
}

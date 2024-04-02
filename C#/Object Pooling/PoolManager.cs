using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PoolManager
{
    #region Pool
    class Pool
    {
        public GameObject Original { get; private set; }
        public string Path { get; private set; }
        public Transform Root { get; set; }

        readonly Queue<Poolable> _poolStack = new Queue<Poolable>();

        public void Init(GameObject original , string path , int count = 5)
        {
            Original = original;
            Path = path;
            Root = new GameObject().transform;
            Root.name = $"{Original.name}_Root";

            for (int i = 0 ; i < count ; i++)
            {
                Push(Create());
            }
        }

        Poolable Create()
        {
            GameObject go = Managers.Resource.Load<GameObject>($"Prefabs/{Path}");
            go.SetActive(false);
            go = Object.Instantiate(go);
            go.name = Original.name;
            return go.GetOrAddComponent<Poolable>();
        }

        public void Push(Poolable poolable)
        {
            if (poolable == null)
                return;

            poolable.transform.SetParent(Root);
            poolable.gameObject.SetActive(false);

            _poolStack.Enqueue(poolable);
        }

        public Poolable Pop(Vector3 pos , Quaternion q , Transform parent)
        {
            Poolable poolable;

            if (_poolStack.Count > 0)
                poolable = _poolStack.Dequeue();
            else
                poolable = Create();

            if (parent == null)
                poolable.transform.SetParent(Managers.Scene.CurrentScene.transform);

            poolable.transform.SetParent(parent);
            poolable.transform.position = pos;
            poolable.transform.rotation = q;
            poolable.gameObject.SetActive(true);

            return poolable;
        }
    }
    #endregion

    public void Init()
    {
        if (_root == null)
        {
            _root = new GameObject { name = "@Pool_Root" }.transform;
            Object.DontDestroyOnLoad(_root);
        }
    }

    readonly Dictionary<string , Pool> _pool = new Dictionary<string , Pool>();

    Transform _root;
    
    public Poolable Pop(GameObject original, string path , Vector3 pos , Quaternion q , Transform parent)
    {
        if (!_pool.ContainsKey(original.name))
            CreatePool(original, path);

        return _pool[original.name].Pop(pos , q , parent);
    }
    public void CreatePool(GameObject original, string path , int count = 5)
    {
        Pool pool = new Pool();
        pool.Init(original , path, count);
        pool.Root.parent = _root;

        _pool.Add(original.name , pool);
    }
    public void Push(Poolable poolable)
    {
        string objName = poolable.gameObject.name;
        if (!_pool.ContainsKey(objName))
        {
            GameObject.Destroy(poolable.gameObject);
            return;
        }

        _pool[objName].Push(poolable);
    }

    public GameObject GetOriginal(string objName)
    {
        return _pool.TryGetValue(objName, out var value) ? value.Original : null;
    }

    public void Clear()
    {
        foreach (Transform child in _root)
            GameObject.Destroy(child.gameObject);

        _pool.Clear();
    }
}

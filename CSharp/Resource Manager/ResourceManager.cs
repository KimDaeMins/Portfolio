﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ResourceManager
{
    public T Load<T>(string path) where T : Object
    {
        if (typeof(T) == typeof(GameObject))
        {
            string name = path;
            int index = name.LastIndexOf('/');

            if (index >= 0)
                name = name.Substring(index + 1);

            GameObject go = Managers.Pool.GetOriginal(name);
            if (go != null)
                return go as T;
        }

        return Resources.Load<T>(path);
    }

   public GameObject Instantiate(string path , Vector3 pos)
    {
        return Instantiate(path , pos , Quaternion.identity);
    }
    public GameObject Instantiate(string path , Vector3 pos , Vector3 euler)
    {
        Quaternion q = Quaternion.Euler(euler);

        return Instantiate(path , pos , q);
    }
    public GameObject Instantiate(string path, Transform parent = null)
    {
        GameObject original = Load<GameObject>($"Prefabs/{path}");
        if (original == null)
        {
            Debug.Log($"Failed to load original : {path}");
            return null;
        }

        if (original.TryGetComponent<Poolable>(out _))
        {
            return Managers.Pool.Pop(original, path , original.transform.position , original.transform.rotation , parent).gameObject;
        }

        return Instantiate(original, original.transform.position, original.transform.rotation, parent);
    }
    public GameObject Instantiate(string path, Vector3 position, Quaternion q, Transform parent = null)
    {
        GameObject original = Load<GameObject>($"Prefabs/{path}");
        if (original == null)
        {
            Debug.Log($"Failed to load original : {path}");
            return null;
        }

        if (original.TryGetComponent<Poolable>(out _))
        {
            return Managers.Pool.Pop(original , path , position , q , parent).gameObject;
        }

        return Instantiate(original , position, q, parent);
    }
    public GameObject Instantiate(GameObject prefab , Vector3 position, Quaternion q, Transform parent = null)
    {
        GameObject go = Object.Instantiate(prefab , position, q, parent);
        go.name = prefab.name;
        return go;
    }


    public void Destroy(GameObject go)
    {
        if (go == null)
            return;

        if (!go.activeSelf)
            return;

        if (go.TryGetComponent(out Poolable p))
        {
            Managers.Pool.Push(p);
            go = null;
            return;
        }

        Object.Destroy(go);
        go = null;
    }

    public void Destroy(MonoBehaviour mob)
    {
        if (mob == null)
            return;
        Destroy(mob.gameObject);
        mob = null;
    }
}

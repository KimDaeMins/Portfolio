using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class  ObjectManager
{
    //딱히 생각나는게 없으니 일단 enum값으로 오브젝트만 딱 정리해서 넣어놓자 //도식화
    Dictionary<Define.Object , LinkedList<GameObject>> _objects = new Dictionary<Define.Object , LinkedList<GameObject>>();

    public void Init()
    {
        _objects.Add(Define.Object.Player, new LinkedList<GameObject>());
        _objects.Add(Define.Object.Monster , new LinkedList<GameObject>());
        _objects.Add(Define.Object.PlayerBullet , new LinkedList<GameObject>());
        _objects.Add(Define.Object.MonsterBullet , new LinkedList<GameObject>());
    }
    public void Add(GameObject go, Define.Object type)
    {
        if (type == Define.Object.None)
            return;
        
        if(_objects.ContainsKey(type) == false)
            _objects.Add(type , new LinkedList<GameObject>());

        _objects[type].AddLast(go);
    }
    public void Remove(GameObject go, Define.Object type)
    {
        if (type == Define.Object.None)
            return;

        _objects[type].Remove(go);
    }
    public void Clear()
    {
        foreach (var obj in _objects)
            obj.Value.Clear();
    }
    public void Clear(Define.Object type)
    {
        _objects[type].Clear();
    }
    public int GetObjectCount(Define.Object type)
    {
        return _objects[type].Count;
    }
    public GameObject GetNearObject(GameObject go, Define.Object type)
    {
        float nearDis = 9999;
        GameObject nearObject = null;
        foreach(var data in _objects[type])
        {
            float dis = ( data.transform.position - go.transform.position ).magnitude;
            if(nearDis > dis)
            {
                nearObject = data;
                nearDis = dis;
            }
        }

        return nearObject;
    }
    public LinkedList<GameObject> GetAllObject(Define.Object type)
    {
        return _objects[type];
    }
    public GameObject GetPlayer()
    {
        return _objects[Define.Object.Player].First.Value;
    }
    public GameObject GetNearObjectInAngle(GameObject go, float angle, Define.Object type)
    {
        float nearDis = 9999;
        GameObject nearObject = null;
        foreach (var data in _objects[type])
        {
            float dis = (data.transform.position - go.transform.position).magnitude;
            Vector3 _targetVector = (data.transform.position - go.transform.position).normalized;
            float dot = Vector3.Dot(go.transform.up, _targetVector);
            float checkAngle = Mathf.Acos(dot) * Mathf.Rad2Deg;

            if (nearDis > dis && checkAngle < angle)
            {
                nearObject = data;
                nearDis = dis;
            }
        }

        return nearObject;
    }
}

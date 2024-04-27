using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
[Serializable]
public class Wave
{
    public string spawnName;
    public float waitTime;
}
[Serializable]
public class WaveData
{
    public List<Wave> waves = new List<Wave>();
}

public class WaveManager : MonoBehaviour
{
    private Queue<Wave> _waves = new Queue<Wave>();
    private Wave nowWaveData;
    private GameObject nowSpawn;
    private float nowTime;
    private bool _allSpawn;
    private bool _isStart = false;
    public string dataPath;
    public void StartWave()
    {
        _isStart = true;
    }
    // Start is called before the first frame update
    void Start()
    {
        TextAsset textAsset =  Managers.Resource.Load<TextAsset>($"Data/{dataPath}");
        WaveData data = JsonUtility.FromJson<WaveData>(textAsset.text);

        foreach(Wave wave in data.waves)
        {
            _waves.Enqueue(wave);
        }
        nowWaveData = _waves.Dequeue();
    }

    // Update is called once per frame
    void Update()
    {
        if (!_isStart)
            return;

        if (_allSpawn)
        {
            //if(몬스터 개채수가 0이면)
            // 보상획득 웨이브매니저삭제

            return;
        }
            

        if(nowSpawn == null)
        {
            nowTime += Time.deltaTime;
            if(nowTime > nowWaveData.waitTime && Managers.Object.GetObjectCount(Define.Object.Monster) < 2)
            {
                nowSpawn = Managers.Resource.Instantiate(nowWaveData.spawnName);
                nowSpawn.GetComponent<Spawn>()._waveManager = this.gameObject;
                if (_waves.Count == 0)
                {
                    _allSpawn = true;
                    return;
                }
                nowWaveData = _waves.Dequeue();
                nowTime = 0;
            }
            
        }
    }

    public void ResetNowSpawn()
    {
        nowSpawn = null;
    }
}

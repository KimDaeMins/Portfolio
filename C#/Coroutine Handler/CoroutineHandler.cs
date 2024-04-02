using System.Collections;
using UnityEngine;


//유니티 메인스레드에서만 실행되어야한다
public class CoroutineHandler : MonoBehaviour
{
    private static CoroutineHandler s_instance;
    public static CoroutineHandler Instance
    {
        get { Init();
            return s_instance;
        }
    }
    private static void Init()
    {
        if (s_instance != null) return;
        
        //어차피 따로 만들생각은 없어서.
        GameObject go = new GameObject { name = "@CoroutineHandler" };
        s_instance = go.AddComponent<CoroutineHandler>();
        DontDestroyOnLoad(go);
    }
    public Coroutine Start_Coroutine(IEnumerator method)
    {
        return StartCoroutine(method);
    }
    public void Stop_Coroutine(Coroutine co)
    {
        StopCoroutine(co);
    }
}
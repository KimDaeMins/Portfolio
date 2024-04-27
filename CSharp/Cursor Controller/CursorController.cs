using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CursorController : MonoBehaviour
{
    private readonly int _mask = 1 << (int)Define.Layer.Ground;
    private Camera _mainCamera;
    public Vector3 GroundRayPos { get; private set; }
    void Awake()
    {
        _mainCamera = Camera.main;
    }

    // Update is called once per frame
    void Update()
    {
        Ray ray = _mainCamera.ScreenPointToRay(Input.mousePosition);
        RaycastHit[] hits = Physics.RaycastAll(ray, 100.0f, _mask);
        //레이를 두번쏘고싶지않았어.
        foreach (var hit in hits)
        {
            if (hit.transform.gameObject.layer == 1 << (int)Define.Layer.Ground)
            {
                GroundRayPos = hit.point;
            }
        }
    }
}

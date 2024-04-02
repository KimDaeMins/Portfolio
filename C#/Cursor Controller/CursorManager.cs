using UnityEngine;

public class CursorManager
{
    private int _mask;
    private Camera _mainCamera;
    private Vector3 _groundRayPos;
    private bool _isGroundHit;
    
    private RaycastHit[] _hits;
    public void Init()
    {
        _mainCamera = Camera.main;
        _mask = 1 << (int)Define.Layer.Ground;
        _hits = new RaycastHit[10];
        _isGroundHit = false;
    }

    public void Update()
    {
        Ray ray = _mainCamera.ScreenPointToRay(Input.mousePosition);
        int hitCount = Physics.RaycastNonAlloc(ray, _hits, 100.0f, _mask);
       
        _isGroundHit = false;
        for (int i = 0; i < hitCount; ++i)
        {
            if (_hits[i].transform.gameObject.layer == (int)Define.Layer.Ground)
            {
                _isGroundHit = true;
                _groundRayPos = _hits[i].point;
            }
        }
    }

    public bool GetGroundRayPos(out Vector3 pos)
    {
        if (_isGroundHit)
        {
            pos = _groundRayPos;
            return true;
        }

        pos = Vector3.zero;
        return false;
    }
}
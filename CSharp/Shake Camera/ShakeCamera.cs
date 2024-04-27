using DG.Tweening;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum ShakeType
{
    Hit,
    Attack,

}
public class ShakeCamera : MonoBehaviour
{
    DOTweenAnimation[] _animations;
    void Awake()
    {
        _animations = GetComponents<DOTweenAnimation>();
    }

    public void Shake(ShakeType type)
    {
        if (_animations.Length > (int)type)
        {
            _animations[(int)type].CreateTween();
            _animations[(int)type].DORestart();
        }
    }
}

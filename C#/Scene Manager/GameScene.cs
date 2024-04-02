using System.Collections;
using System.Collections.Generic;
using Unity.AI.Navigation;
using UnityEngine;

public class GameScene : BaseScene
{
    private NavMeshSurface _surface;
    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Game;
        _surface = GetComponent<NavMeshSurface>();
        
        //여기서 맵을 생성해야됨
        Managers.Map.LoadMap();
        ///////////
        
        Screen.SetResolution(640, 480, false);
        _surface.BuildNavMesh();
    }

    public override void Clear()
    {
        
    }
    
}

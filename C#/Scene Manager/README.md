# Scene Manager

## 1.Scene구성

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/0f7582bf-f77d-4ce6-b354-79894f0b6006)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/5c1d3f1e-f0b5-4ec3-bd71-98e13a008083)

enum값으로 구성할 Scene들을 미리 입력합니다.

Scene의 부모가 되는 BaseScene은 SceneType을 각각 들고있습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/211d99f0-b2a4-4558-9404-3ee270bc99c8)

각 Scene의 Init에선 맵을 생성하거나 Bgm을 재생하는 등 스테이지 초기화시 해야할 작업을 입력합니다.

## 2.Scene전환

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/4a2857c1-2f0d-44c0-b7f3-fbd7e3752a5f)

매니저의 LoadScene 함수를 호출합니다 인자로는 enum값을 받습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/ff36741d-8e89-4309-9547-103663b0bde1)

SceneManager에서는 현재씬(객체)을 갖고있습니다. 

씬 교체시 매니저를 초기화합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/16a372b3-20bc-47d7-bd15-7a5a114932b3)

매니저의 초기화 단계에선 씬을 초기화하게됩니다. 이후 씬이 재생성(LoadScene)되는 방식으로 구현했습니다.

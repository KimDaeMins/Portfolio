# Shield

## 귀속 아이템, 방패 전투 매커니즘, 패링을 구현했습니다.

<img width="392" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/51823573-d842-4221-a417-e561691323f4">

## 핵심 코드

<img width="756" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/1d8de54c-e452-49dc-9199-d39a84948286">

최초 생성시 플레이어 왼쪽 손의 Bone을 가져와서 변수로 들고있습니다.

구현위치 - Shield.cpp Line[165-186]

<img width="284" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/c58a87b1-d93d-4137-be1c-d709ba07b5f1">

플레이어 랜더링의 마지막단계에서 마지막으로 사용한 아이템(오른손) 과 방패(왼손) 및 화관(모자) 을 랜더링 하는 작업을 합니다.

-> 플레이어 랜더링 단계에서 플레이어의 뼈의 정확한 위치가 업데이트되고 그곳에 제대로 랜더링하기위해 인벤토리 아이템들은 플레이어가 랜더링하도록 조절했습니다.

<img width="516" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/b44a217b-84be-4cca-ac51-5dd2d27af41a">

위에서 가져온 플레이어의 뼈정보 및 월드정보를 랜더링단계에서 조절합니다.

이쪽에서 실질적으로 방패의 트랜스폼이 움직이니 방패 Collider또한 랜더링단계에 맞춰서 이동합니다.

역할상의 문제가 있긴 하지만 보다 깔끔한 작동을 위하여 이렇게 만들었습니다. 그럼에도 역할상의 문제는 중요하다고 생각하기에 다음에 만들때는 업데이트단계로 조절할 듯 합니다.

구현위치 - Shield.cpp Line[85-91]

<img width="581" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/eb7b1dfe-0c43-45da-932c-bd76e68200c1">

쉴드는 인벤토리의 Geer에서 쉴드가 존재하는지, 플레이어의 스테미너가 일정량 이상인지를 판단 후 전환됩니다.

구현위치 - StateIdle.cpp Line[143-147]

<img width="831" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/e2ac0a32-ab8c-4903-a3c7-ebb380f3e0d2">

쉴드를 들고있는 상태에서 양 다리Bone을 가져와 ControllMatrix를 일정각도 왕복하도록 조절하여 방패를 들고 천천히 걷는 모션을 구현했습니다.

구현위치 - StateShield.cpp Line[132-210]

<img width="457" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/1607a835-7add-4baa-9be0-a880e69c9b7d">

쉴드를 든 상태에서 시간을 체크한 후 키를 빠르게 떼었다면 패링상태로 전환합니다.

구현위치 - StateShield.cpp Line[91-103]

<img width="569" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/108837cb-98a0-4dcd-8d80-f0fa390a2b08">

패링 애니메이션의 특정 구간에서만 패링이 가능하게 조절하며, 쉴드 Collider의 속성을 잠시동안 변경하는 방식을 이용합니다.

구현위치 - StateParry.cpp Line[71-91]

    3-1. 2-3 에서 구한 Tick당 이동량을 Update에서 m_ControlTranslationMatix에 적용합니다.

     구현위치 - FrogTongueInit.cpp 61~115


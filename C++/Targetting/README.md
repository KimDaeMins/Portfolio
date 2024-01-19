# Targetting

## 몬스터를 순회하여 가장 가까운 몬스터를 탐지하고 회전하는 기능을 구현했습니다.

<img width="428" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/bbab013c-dcfb-43f8-ad76-fd3f402afacf">


## 핵심 코드

<img width="477" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/8f5e5dd5-1298-4302-85a4-a2fce20b4377">


## 설명

  몬스터의 매 프레임마다 플레이어와의 거리를 저장한 후 플레이어의 Targgetting시 몬스터를 순회하여 가장 가까운 몬스터를 찾고 저장하는 방식입니다.

  구현위치 - Monster.cpp Line[276-292], Player.cpp Line[975-1073]

### 몬스터를 바라보는 방식

<img width="627" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/2a11da82-cafb-48ca-8e7f-4c87fa2be2ab">

#### 1. 몬스터로의 Dir을 구합니다

    1-1. 몬스터를 바라보는 방향에 맞춰 (Target - MyPos) Vector3를 추출합니다.

#### 2. 플레이어의 Transform을 설정합니다

    2-1. 1-1에서 구한 Dir을 이용해 Look을 구하고 (YLock == true)일시 내 월드매트릭스의 Look의 y값을 가져오도록 합니다.

    2-2. 2-1에서 구한 LookVector와 UpVector를 외적하여 Right를 구합니다.

    2-3. 2-1에서 구한 LookVector와 2-2에서 구한 RightVector를 외적하여 UpVector를 구합니다.

    2-4. 각각 구한 Right, Up, Look을 내 WorldMatrix행렬에 적용합니다.

    구현위치 - Transform.cpp Line[500-530]

## 개선사항

실시간으로 가까운적을 판단하고 언제든 타겟팅을 뽑아쓸 수 있도록 몬스터의 업데이트에서 플레이어를 가져와서 몬스터와 플레이어간의 거리를 저장하는 방식을 이용했는데, 의미없는 몬스터까지 적용되는 문제가 있지 않았나 싶습니다. 다음에 구현한다면 Targetting함수를 호출하는 즉시 몬스터와 플레이어간의 거리를 전부 계산한 후 타겟팅함수를 실행할 것 같습니다.


# PhysX

## PhysX 작동구조, 객체 충돌 관리 및 저장, RigidBody, Collider생성을 구현했습니다.

<img width="268" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/2c09c8e3-d001-4786-9009-a78ab27b4204">

## 핵심 코드


## 설명

  

### 회전값과 이동값 매트릭스를 나눈 이유

  매트릭스의 곱을 적용할 때 크기 -> 자전 -> 이동 -> 공전 -> 부모 의 순으로 곱해주어야 합니다.(행렬의 곱은 역이 성립하지 않기 떄문에)

  이 규칙을 어기고 이동을 한 후 크기조절 매트릭스를 곱하면 이동거리가 크기조절 매트릭스의 크기만큼 커지게 되는 현상들을 볼 수 있습니다.  

  이런 상황에서 정확한 추가 회전값과 이동값을 기존 TramsformationMatrix에 전달하려면 회전과 이동량을 따로 나누어서 전달해야한다고 판단했습니다.

  그래서 현재 Bone의 TransformationMatrix(회전 및 이동값이 들어있는 매트릭스) 의 앞에서 ControlMatrix(회전 행렬) 을 곱해주고 뒤에서 ControllTranslationMatrix(이동 행렬) 을 곱해주어 행렬 계산의 오차를 없앴습니다.

  구현 위치 -  HierarchyNode.cpp - 40~76 Line

### 회전값의 적용 예시

<img width="234" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/29c09d03-fb16-4f43-9305-a2d4b9a97d12">

  플레이어를 바라보는 보스몬스터의 머리

<img width="662" alt="8-2" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/d4f3c328-b5ae-4f74-9f2f-78783f54a3d3">

#### 1. 몬스터의 뼈 -> 플레이어로의 Y축 회전량을 구합니다

    1-1. 몬스터의 방향벡터(MyDir), 플레이어로의 방향벡터(ToTarget)의 Y값을 제거합니다

    1-2. Y값이 제거된 MyDir과 ToTarget을 정규화 시킨 후 내적 합니다 (MyDir·ToTarget)

    1-3. 결과값으로 나온 cosA값의 역코사인값을 가져옵니다 ( 라디안각이 추출됨 )

    1-4. 뼈의 회전범위를 조절합니다.

    1-5. 외적을 통해 좌우를 판단하여 회전각에 적용합니다. (MyDir X ToTarget 의 y값으로 비교)

#### 2. 몬스터의 뼈 -> 플레이어로의 X축 회전량을 구합니다

    2-1. 1-1에서 구한 ToTarget의 Y값을 제거한 Vector와 ToTarget을 정규화시킨 후 내적합니다 (ZeroYToTarget·ToTarget)

    2-2. 플레이어의 현재 위치와 뼈의 위치를 비교하여 상하를 판단하여 회전각에 적용합니다

#### 3. 회전 매트릭스를 만든 후 컨트롤매트릭스에 적용합니다.

    3-1. 회전할 뼈의 매트릭스의 회전값의 역행렬을 구합니다. (원점에서 회전하기 위함)
  
    3-2. 1, 2에서 구한 회전값으로 회전매트릭스를 만듭니다.
  
    3-3. 회전 오차를 조절하기위하여 싱크매트릭스를 만듭니다.

    3-4. 3-2 * 3-3 * 3-1 의 값을 컨트롤 매트릭스에 적용합니다.

    구현위치 - SpiderTank_Idle.cpp 61~115

## 회전값 적용 개선사항

    Unity를 배우며 이 방법이 아닌 쿼터니언을 사용했다면 훨씬 쉽고 빠른 코드가 되지않았을까 싶습니다.

    보스의 회전에서 보스to플레이어까지의 회전을 그냥 적용시키면 되는 부분을 돌아서 갔다고 생각하는데,

    그래도 얻어간 점이라면 행렬에 대한 이해를 확실히 했다고 생각합니다.
  
### 이동값의 적용 예시

<img width="232" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/051233e9-d9b3-467a-b78d-88e13fa2aa67">

    뼈의 길이를 플레이어에게까지 늘려 혓바닥 갈고리가 플레이어에게 닿는 모습
    
<img width="635" alt="8-3p" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/0b83804d-0ae9-4b9f-ad16-1fcbfe01b9ff">

#### 1. 몬스터를 플레이어 방향으로 회전합니다.
    
    1-1. 플레이어 - 몬스터 -> 플레이어로의 방향(dir)

    1-2. dir벡터에 맞춰서 Look, Right, Up 벡터를 설정 후 회전행렬을 구성합니다

#### 2. 플레이어와의 거리 - 최대거리의 크기를 구한 후 1초당 이동량을 구합니다

    2-1. 이미 회전을 시킨 상태이니 z축 양의방향을 바라보는 상태에서 애니메이션상 
    
    뼈의 최대 길이를 뺀 값(고정값 6)을 구합니다. -> 애니메이션에서 뼈가 살짝 늘어나는 부분떄문에 오차를 구했습니다.

    2-2. (플레이어 위치 - 애니메이션상 뼈가 제일 늘어났을때의 위치) 의 길이만큼 z축 양의방향으로 늘린 벡터를 만듭니다.

    2-3. 애니메이션에 따라 서서히 증가해야하기때문에 속도값과 Duration을 구해서 한 Tick당 이동량을 구합니다.

#### 3. 특정 뼈의 ControlTranslationMatrix에 적용합니다

    3-1. 2-3 에서 구한 Tick당 이동량을 Update에서 m_ControlTranslationMatix에 적용합니다.

     구현위치 - FrogTongueInit.cpp 61~115


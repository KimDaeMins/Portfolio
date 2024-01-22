# Grass

## 플레이어 충돌에 따른 객체의 기울어짐 표현

<img width="535" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/ce69c987-8b1b-4dd4-a2f9-b77c824f5962">

## 핵심 코드

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/6ad430e5-6bb7-47a8-8995-a1f00d9f11b3)

캡슐상의 임의의 점과 바닥에 고정된 점을 이용하여 Up벡터를 만들어 풀의 기울기를 구현했습니다.

플레이어에 의해 밀리는 캡슐의 최대 이동량을 제어하여 플레이어가 캡슐을 그대로 밀고 나가는 현상(풀이 플레이어가 전부 지나간 이후에도 누워있는 현상)을 방지했습니다.

캡슐 Collider가 고정된 점을 향해 이동하여 캡슐상의 임의의 점과 고정된 점이 가까워지며 기울기가 90도에 점차 맞춰져 풀이 기울었다가 서는 느낌을 만들었습니다.

<img width="347" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/5868efea-9d74-4f5a-9f76-ec575b989032">

조잡한 그림이지만 일단 이런 형태입니다.

### 과정

#### 1. 객체 생성시 Transform을 두개를 들고있으며 동시에 조작합니다.

    1-1. 한개는 랜더링을 위한 Transform, 한개는 Collider를 다루기 위한 Transform입니다.

    1-2. Collider가 움직이게 되면 Transform이 그에 맞춰 움직이게됩니다 (PhysX구조에 따른 움직임)

#### 2. 움직인 Transform과 랜더링을 위한 Transform과의 연산으로 Up벡터를 만들어 랜더링 Transform에 적용합니다.

    2-1. Collider가 움직인다면 Actor의 포지션이 움직이는것이라서 Actor의 포지션(ActorPos)을 가져옵니다.(World + Local로 중점을 가져옵니다.)
    
    2-2. 2-1에서 구한 ActorPos에 랜더링을 위한 Transform의 WorldPos(MyPos)를 -연산해줍니다. (ActorPos - MyPos)

    2-3. 2-2에서 구한 벡터를 Up벡터로 사용하고 Up벡터와 1,0,0 벡터를 외적하여 Look 벡터를 만들어줍니다.

    2-4. Up벡터와 2-3에서 구한 Look벡터를 외적하여 Right벡터를 만든 후 랜더링용 트랜스폼에 적용시킵니다. (Right, Up, Look순으로 월드를 조절합니다.)

    Grass.cpp Line[117-137]

#### 3. 플레이어에 의해 너무 많이 밀리게 되는 현상을 조절합니다.

    3-1. 2-2의 MyPos와 2-1의 ActorWorldPos의 길이를 비교하여 플레이어의 지름 + 풀의 반지름 을 넘어섰다면 플레이어의 뒤로 조절되게 구현합니다. (플레이어의 지름 + 풀의 반지름정도가 최대로 밀린 상태일때가 가장 자연스러웠습니다)

    구현위치 - Grass.cpp Line[143-155]

#### 4. ActorPos를 MyPos방향으로 천천히 움직입니다.(풀이 되돌아오는 현상을 구현하기위함)

    4-1. 풀의 무게를 0에 가깝게(0.00001) 설정하여 풀에의해 플레이어가 밀리는 현상을 미리 방지합니다.

    4-2. ActorWorldPos에  MyPos로 향하는 방향을(MyPos - ActorWorldPos)더해줍니다.

    4-3. ActorWorldPos의 y를 보정한값이 MyPos와 같다면 Sleep상태로 두어(물리현상계산을 안하는 상태) 렉을 방지합니다.

    4-4. 다르다면 ActorWorldPos를 Collider의 움직임을 위한 Transform에 적용합니다.

    구현위치 - Grass.cpp Line[158-176]


# Bush

## 플레이어 속도에 따른 고정객체의 기울기 및 특정 필터충돌에 맞춰 나무가 잘리는 현상을 구현했습니다.

<img width="230" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/3804f27f-7a88-4700-a340-b4b575f4dbf0">

## 핵심 코드

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/735d15c7-daee-4c20-b3f6-ec25fa5e0f77)

physX라이브러리를 이용 충돌된 모든 오브젝트를 가져오고 필터링합니다.

한 프레임동안 누적된 이동량을 받아온 후 이것을 이용하여 Up벡터를 만들어 객체에 적용하는 방식입니다. 충돌하지 않았다면 기본 상태로 복구합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/512fee89-31a0-4261-9ef5-8c71d6601f3d)

Transform에서 이동량을 누적하는 방식입니다. 플레이어가 움직일때 Set_State를 이용합니다. 이 때 변화한 위치와 현재위치를 계산하여 이동량을 저장합니다.

-> 실제 이동시엔 physX에서 충돌을 감지하여 이동되지는 않습니다.

### 기울기의 적용

#### 1. 기울기를 표현할 Vector를 만듭니다.

    1-1. 최종적인 이동량을 계산하기 위함이니 LateTick에서 작업을 합니다(Tick에서 가속도가 계속 더해지고 그 결과값을 LateTick에서 활용하는 방식)

    1-2. 기울기 표현을 위하여 기울기를 표현할 Vector(현재 나무의 위치벡터)에 y값에 1만큼의 보정을 해줍니다.
    
    1-3. 나무의 위치벡터(MyPos)와 부딫힌 대상의 위치벡터(TargetPos)의 z값, x값을 비교하여 부딫힌 방향을 설정합니다.

    1-4. 부딫힌 방향에 따라 Target의 이동량을 기울기를 표현할 Vector에 적용합니다
    
#### 2. 밀리고있다면 밀리는것에 맞춰 Transform을 수정합니다.

    2-1. 1-4까지 진행된 기울기Vector - 나무의 위치벡터의 계산으로 Up벡터를 만듭니다.

    2-2. Up벡터와 0,0,1벡터를 외적하여 Right벡터를 구합니다.

    2-3. Right벡터와 Up벡터를 외적하여 Look벡터를 구합니다.

    2-4. 각각 구현된 벡터를 랜더링용 WorldTranform에 적용합니다 Right, Up, Look순으로 행렬을 교체합니다.


# Grass, Bush의 필터충돌

## 충돌 필터링을 이용해 칼로 잘리는 나무를 표현했습니다.

<img width="152" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/71270e15-6989-4102-bda4-84e225e88701">

## 핵심코드 

<img width="699" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/292f7aa4-9ebf-432e-b5ee-3764cb41b4ef">

충돌 객체의 타입과 충돌 객체의 데미지타입을 가져와서 NormalAttack(칼공격)이라면 객체를 죽이는 방식을 이용했습니다.

데미지타입은 노말, 스태프, 독, 테크보우, 폭발, 레이저 등 여러가지의 타입이 존재합니다.

객체 사망 시 충돌Collider를 제거하고 객체의 상부와 하부를 나누어 같은위치에 생성이 되게끔 구현하였고 (TreeTop, Stump)

<img width="408" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/17417109-dd34-40a3-b892-64eeb0b0327b">

상부는 physX를 이용하여 Force와 Torque를 주어 칼에 잘리면서 날아가는 현상을 구현했습니다.

<img width="632" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/2aea6cde-fcfc-43c1-ae6d-eb860ec7ae70">

하부는 그대로 남게하고 상부는 일정 시간이 지난 이후에 크기가 점점 작아지게하고 LifeTime을 두어 사라지게 설정했습니다.


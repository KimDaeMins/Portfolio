# Grass(PhysX응용)

## 플레이어 충돌에 따른 객체의 기울어짐 표

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

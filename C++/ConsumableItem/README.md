# 소모성 아이템

## 소모성 아이템의 종류에 따른 아이템 사용 효과 및 애니메이션 변경을 구현했습니다.

<img width="232" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/23e31200-c1a8-49cf-b4b9-15561b6c6914">


## 핵심 코드

<img width="498" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/02c16356-c2a0-4800-8556-d306f644b3e1">

아이템 타입에 따라 사용하는 방법(애니메이션의 모습)이 바뀌는 점을 구분하기 위하여 생성단계에서 사용방법을 미리 정의합니다.

구현위치 - ConsumableItem.cpp Line[28-48]

<img width="513" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/09a667ea-282f-4532-ae74-c432db28a29c">

MotionChangerState에서 위에서 정의한 UsingType을 받아와 각 UsingType에 맞춰 플레이어의 애니메이션을 변경합니다.

구현위치 - StateItemMotionChange.cpp Line[139-168]

<img width="916" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/234df751-ec13-49a6-b542-0b33a32f1278">

아이템을 실질적으로 사용하는곳은 인벤토리이며 Use_Item함수는 각 아이템으로 진입 후 갯수를 1개 줄이는 함수입니다.

아이템의 남은 갯수가 없다면 현재 키슬롯에서 제거하고, 인벤토리 슬롯에서도 제거합니다.

아이템 갯수의 변화에 따라 UI또한 업데이트 해줍니다.


### ThrowedItem

<img width="659" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/3b7fe809-fa8c-4322-b0a2-a4926d4da728">

<img width="861" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/11f4756d-9087-4eda-9ecc-adcc295611a8">

아이템을 던지는 방식에서 m_Time의 증가에 따라 포물선 위의 어느 점에 있는지를 판단한 후 이전프레임에서의 위치를 뺴주어서 나온 값 -> 진행방향 을 RigidBody의 velocity에 직접적용하여 목표물을 향해 날아가는 방식을 구현했습니다.

구현위치 - ThrowItem.cpp Line[180-184]

<img width="928" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/5aef424b-b027-4e45-8298-a6c40eec9b20">

날아가는 도중 벽이나 오브젝트와 충돌했다면 목표물을 향해 천천히 이동하도록(유도) 구현했습니다.

구현위치 - ThrowItem.cpp Line[155-174]

<img width="389" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/f8100429-78d9-458b-a9aa-14f35148d6d8">

던진 후 일정시간이 지나면 몬스터만을 공격하도록 필터링한 AttackZone을 잠시동안 생성 후 객체를 삭제합니다.

구현위치 - ThrowItem.cpp Line[188-220]

### EatItem

<img width="515" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/343dcfc4-88a1-4dd2-9cfc-a7312d18e678">

먹는 형식의 아이템은 각각 아이템의 종류에 따라 체력이나 마나를 증가시키는 방식을 이용하였고, 최대체력,마력을 넘기지 못하도록 제한했습니다.

최대체력을 단순히 증가시키는것이 아닌 작은 30분의 1의 값을 프레임마다 증가시키게 설정하여 체력바가 서서히 늘어나는 모습을 구현했습니다.

구현위치 - StateEatting.cpp Line[70-105]

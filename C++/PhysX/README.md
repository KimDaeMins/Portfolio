<img width="252" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/d3cbc990-1638-49ef-988f-9850f31fc4ac"><img width="561" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/ae3e6141-04cc-43ed-a3c9-d672750dfd1c"><img width="651" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/c1efc7f2-5ba9-4592-859a-96733debd789"><img width="660" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/0d0a954f-78f7-4c97-9ad1-760db5027f53"><img width="798" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/a83486de-c318-42b2-8036-2b683ea07ab2"><img width="731" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/faee4522-39b5-4a53-a2a7-43acf5e377cb"><img width="260" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/bdace07b-561d-4fef-b011-1053d8bf3330"><img width="884" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/7ef82687-e4c4-4726-a235-8a8887073078"># PhysX

## PhysX 작동구조, 객체 충돌 관리 및 저장, RigidBody, Collider생성을 구현했습니다.

<img width="268" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/2c09c8e3-d001-4786-9009-a78ab27b4204">

## 핵심 코드

<img width="691" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/d2a07cad-17c1-4c3f-81e1-58d0d027ab5f">

Collider 생성 과정 (Sphere)

<img width="884" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/5fb30784-62ca-41a1-ba47-f309dd075193">

RigidBody 생성

<img width="608" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/d31d68c8-3fa6-4963-87c5-7bc74a122a97">

RigidBody의 Fixed_Update - 움직임 감지 후 좌표조절

<img width="260" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/6523d8c5-849c-46f7-aae4-1871f1d9c485">

PhysXSystem의 Update - Apply_Tick에선 충돌 이후 객체의 변화를 각각객체에 적용시킵니다.

<img width="731" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/d25296b3-aa7a-4435-b25a-d15e7657e04e">

충돌시 상황에따라 각 리지드바디에 충돌체 저장

<img width="705" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/022b98ee-b81c-4868-a0a3-09fb57b9b8c5">

저장된 충돌체를 한개씩 가져옵니다 . Why? 리지드바디를 컴포넌트로 만들었기떄문 - 함수포인터를 들고있는다음 충돌때 함수를 전달받아서 작동하는게 좋았을듯 합니다.

<img width="660" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/77a803be-ccb7-49b5-8adb-2404335450b0">

하나씩 가져온 충돌체의 태그를 확인 후 그에 맞는 작동을 하는 모습입니다.

필터링

<img width="252" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/6dac1be5-00dc-4eed-aeab-4cbd70b424b5">

<img width="538" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/ab609f88-4064-4d14-9da5-ee6ac355fae7">

<img width="651" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/863fdfb0-e2eb-4a8a-846a-cacc910958f1">

<img width="667" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/420ff4d6-e454-44df-9c74-46e5a1a558d6">

<img width="561" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/56d4b6f5-fe3f-41b7-9394-bef51ffd55f9">

필터 충돌을 그룹화 시키고 한 객체가 충돌할 수 있는 객체분류를 미리 정의합니다.

RigidBody 생성시 필터를 미리 정해줍니다.

충돌 시 필터끼리의 &연산을 이용하여 서로 충돌이 가능한지 여부를 판단한 후 충돌판정합니다.

## 개선사항

    Unity를 배우며 이 방법이 아닌 쿼터니언을 사용했다면 훨씬 쉽고 빠른 코드가 되지않았을까 싶습니다.

    보스의 회전에서 보스to플레이어까지의 회전을 그냥 적용시키면 되는 부분을 돌아서 갔다고 생각하는데,

    그래도 얻어간 점이라면 행렬에 대한 이해를 확실히 했다고 생각합니다.


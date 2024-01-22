# Ladder

## RootMotion을 응용한 사다리 구현 ( 유니티 파싱, RootMotion, Interaction, Animation)

<img width="686" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/979dadf6-3052-4554-8404-1ecb6e892128">

애니메이션 진행에 맞춰 플레이어를 사다리에 딱 맞게 조절한 모습입니다.

## 핵심 코드

<img width="437" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/93d9767d-5c2b-437f-909a-20ea7faad338">

사다리의 상부 Collider인지 하부Collider인지 판단 후 각자 다른 애니메이션이나 상태값으로 전환하고 움직이게 됩니다.

-> 상부에서 사다리를 타는 경우 한바퀴 돌아서 사다리에 오르지만 하부에선 애니메이션이 존재하지않습니다. 바로 사다리에 오르며 진행됩니다.

<img width="435" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/a141e8de-bcbd-441e-9457-60bf5d1eb16d">

상부에서 사다리를 타는 경우 애니메이션 매트릭스를 identity화 시키고(최초회전값을 플레이어에 적용시키지 않기위해 원점으로 설정), 플레이어에게 직접 적용하기 때문에 시뮬레이션 충돌을 비활성화하여 physX에 의해 포지션이 바뀌는것을 방지합니다.

구현위치 - StateLadder.cpp Line[44-53],  Animator.cpp Line[313-318]

<img width="264" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/21fd1332-8291-4ed7-a645-8a0e5711a433">

사다리에서 상하로 움직이는 애니메이션이 있는데, 버튼을 누르지 않을때는 애니메이션이 진행되지않도록 Pause_Animation함수를 불러줍니다.

구현위치 - StateLadder.cpp Line[164-167]

<img width="329" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/74edf0da-f4b1-4588-a7e6-80903cc73147">

Animation이 Pause상태에 들어가게된다면 TimeAcc가 시간값에따라 증가되지않습니다.

<img width="601" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/022606ed-7516-46a1-943f-2c86e0435c09">

그렇게 된다면 TimeAcc의 증가에 따른 Ratio값이 변경되지 않고, 업데이트마다 같은 Scale, Rotation, Position값을 갖게되고

<img width="823" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/327f95cd-031e-46c8-a832-99613efc415c">

각 뼈(Channel)의 Matrix도 업데이트 이전과 같은 값을 가지게되어 업데이트가 되어도 애니메이션이 진행되지 않습니다.

구현위치 - Animation.cpp Line[132-241]

<img width="726" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/67c7153e-80f1-48d4-b53b-b2351c777be3">

사다리를 내려가다가 끝에 다다른 경우(바닥LadderCollider와의 충돌로 판단합니다) 바닥을 향해 Ray를 쏴서 바닥의 유무를 판단한 후 바닥이 없다면 떨어지는상태로, 바닥이 있다면 Idle상태로 전환합니다.

구현위치 - StateLadder.cpp Line[169-191]

<img width="778" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/f7bd0534-a953-47bc-a46c-53a3ca2e5576">

사다리의 상부 하부는 Unity 파싱을 통하여 좌표를 정한 후 맵 생성 시 직접 적용합니다.

구현위치 - Level_Librarian.cpp Line[381-401]

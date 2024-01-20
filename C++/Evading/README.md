# 피격 무적, 누적 데미지

## 무적시간을 구현하여 연속피격을 방지하고 데미지를 누적하여 피격모션을 구분했습니다.

<img width="827" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/e3dd86b1-5d33-4e71-a80f-771cd39a77e5">

## 핵심 코드

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/72c29e16-60b6-4b30-831b-cae10174a09c)

피격시 무적시간을 갱신하고 무적 시간이 갱신되었다면 무적상태로 전환합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/ec2c9c47-c520-44f9-9946-b5006651fde0)

받은 데미지를 누적하여 일정 수치 이상 누적됐을 시(m_AccDmg) 피격 모션이 발동되고 무적 시간이 갱신됩니다.

누적 데미지의 수치에 따라 무경직, 밀리는 모션, 넘어지는 모션 으로 나뉘어집니다. 

구현위치 - Player.cpp Line[449-540]

## 개선 사항

피격 자체를 따로 함수화하여 처리하는것이 좀 더 깔끔했을 것 같습니다. 충돌처리에서 많은걸 처리한 느낌이 있습니다.

충돌 종류에 따라 다른 코드가 진행되는점을 좀 더 가시성 있게 표현하는것이 좋았을 것 같습니다.

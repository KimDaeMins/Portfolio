# Cursor Controller / Navigation

## 1. 업데이트

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/810fafbe-225f-45a7-abd6-677e0475c034)

매 프레임마다 메인카메라 기준 마우스포인터의 방향으로 Ray를 쏩니다.

레이캐스트는 모든 객체를 맞춥니다(바닥 클릭을 이용한 이동을 구현하기위함)

충돌된 모든 객체를 순회하며, 땅에 충돌됐다면 GroundHit를 true상태로 교체하고 좌표를 수정합니다.

## 2. 플레이어의 목적지 설정 및 이동

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/fab8fba6-cea5-4ff7-972d-3e937815380b)

GetGroundRayPos함수를 이용하여 마우스 포인터가 찍힌 땅의 위치를 가져온 후 플레이어의 목적지를 변경합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/bf300042-657a-4b24-b773-273f424a58ee)

목적지가 변경되면 Update상태를 true로 변경합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/031ef928-f39d-495b-b40b-6f641a0abc15)

업데이트가 필요한 상황이면 서버에 move패킷을 보내고 업데이트상태를 false로 변경합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/22dd3bf3-5be5-4e3b-891b-8813afdef9c7)

에코서버를 통해 다시 목적지가 플레이어에게로 전송되며 캐릭터의 SetDestination함수를 부르며 움직임을 시작합니다.

이동중에는 지속적으로 서버와 클라이언트를 동기화합니다.

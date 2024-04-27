# InputAction

## 플레이어를 위한 특정 상황에 맞는 Input Action을 구현했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/fa97a51f-773c-4cce-b52e-343473011727)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/ec476129-1451-4a94-b42f-072a1012732b)

스크립트에서 조절이 가능하도록 Generate C# Class 를 체크했습니다.

각 액션별 연동은 StateMachine에서 처리하는 방법을 이용했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/1642e0ea-0b10-4c7b-a260-775968ca6893)

플레이어 인풋 클래스를 제작하여 PlayerActions에 이벤트를 직접 연동할 수 있게끔 설정하였습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/66dde00b-76f4-41d3-96b5-e393b5f98a84)

실제 StateMachine구동시 이벤트 함수를 직접 제작하여 특정 상황에 맞춰 구독하는 방식을 이용했습니다.

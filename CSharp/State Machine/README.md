# StateMachine

## 0.IState의 구조

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/c1dd253b-0033-4716-82de-115c46360a24)

해당스테이트에 진입 , 퇴장 , 입력 , 업데이트 ,물리업데이트 시 각각 조절이 가능하게 인터페이스로 설정합니다.

## 1.State 추가

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/d66c8b0a-e6ef-4b0c-b8b2-5c80c947a569)

StateMachine의 AddState함수를 통해 추가합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/6da35cf4-29fa-43cc-b77a-49bc96de9462)

StateMachine을 부모로 둔 하위 StateMachine들의 Init에서 직접 추가하게됩니다.

## 2.State교환

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/b8f53ad4-9f5f-4762-a435-a34c591c318a)

현재스테이트가 존재하면 Exit함수를 불러줍니다.

바꿀 스테이트로 현재 스테이트를 변경한 후 키값을 설정하고 바뀐 스테이트의 Enter함수를 불러줍니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/5f79085f-9925-4c76-9a77-54e1eef0a155)

일정 시간이 지난 후 State를 변경하고 싶을때는 Coroutine을 이용하여 스테이트를 교환해줍니다. 중간에 멈출수있습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/5825b480-2264-4a4d-95dc-f15362356de8)

각 스테이트들은 Monobehavior가 존재하지 않습니다. CoroutineHandler를 이용해 Monobehavior를 상속하지 않고도 Coroutine을 쓸 수 있게 만듭니다. -> 가능한 이유는 결국 이 함수를 돌리는 주체는 Monobehavior를 상속받고있기 때문입니다.

## 3.State Update

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/93e63c9a-6e9d-434f-ba23-132f451e9079)

StateMachine에서 Tick을 돌리면 현재 State의 Tick함수를 부릅니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/0a03b6db-a052-40fa-ae75-771193538cfd)

StateMachine의 Tick은 각각 개인 Update에서 부르고있습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/d3fce2ef-26b2-44d2-96f9-79e965b68e50)

각각 내부에서 구현된 State 의 Tick 함수를 실행하게됩니다.

## 4. InputActionEvent

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/2719dded-8b25-42ff-a7ae-b349812cb24a)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/ccf5b843-a0cc-4a40-96e9-b8a0d58b78fd)

각 객체별 State의 부모인 BaseState에서 PlayerInput을 받고 InputActionCallback을 virtual로 생성하여 콜백을 연동하는 작업을 수행합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/52179d21-d2c4-417b-b827-c1bf7abf66b9)

하위 State에서는 virtual함수를 오버라이딩만 하면 자동으로 특정 상황에 이벤트를 호출할 수 있도록 연동됩니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/fb415c3d-7b2f-404b-9b7d-b386ef93ba8e)

각 콜백들은 Enter단계에서 연동되고 Exit단계에서 빠지게됩니다.





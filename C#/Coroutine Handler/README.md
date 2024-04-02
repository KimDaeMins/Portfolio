# Coroutine Handler

## 기본구조

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/9df5de45-e4ce-4697-88b1-42899ce119f1)


![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a55e0bb3-87e3-4b65-b6ef-39eaa060b363)

싱글톤 방식으로 제작했습니다. Monobehavior를 상속하여 Coroutine함수를 사용 가능하게 설정했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/cb986bd6-01fa-498c-b083-e4bef07dd38e)

Start_Coroutine함수를 구현하여 함수를 인자로 전달하게 설정하였으며, 인자로 전달받은 함수를 CoroutineHandler 내부에서 판단 후 실행하도록 즉 함수실행의 주체가 CoroutineHandler가 되도록 설정하였습니다.

진행중인 코루틴을 반환하도록 구현하여 같은 클래스 내에서는 언제든 멈출 수 있도록 변수로 가질 수 있게 구현했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/86ce0c6f-5abd-4944-8c4d-81bc413a4070)

Monobehavior의 상속을 받지 않는 클래스에서 코루틴함수를 제작 후 핸들러를 통해 스타트 코루틴 함수를 불러냅니다. 반환값은 변수로 들고있다가 필요할때 멈추는등의 기능을 수행합니다.

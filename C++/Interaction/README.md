# Interaction

## 객체별로 플레이어와의 상호작용 구간을 만들어 상호작용 UI를 나타내고, 플레이어의 상태값을 전환했습니다.

<img width="369" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/f12fdf49-73e5-4c7b-8111-047e205f6a93">

우물 앞에서 상호작용 버튼이 활성화되는 모습

<img width="311" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/65d1464b-247e-45d0-bb70-0b46185de78a">

상호작용 방식에 따라 우물에 동전을 집어넣는 모습 ( 아이템의 금화를 사용하면 위의 애니메이션이 진행되고 , 단순 스페이스바를 누르면 몇개의 금화를 넣었는지 UI로 나타납니다 )


## 핵심 코드

<img width="672" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/c0e5e1ac-2bb6-4cb7-a5ee-f838cdde62d6">

현재 닿아있는 오브젝트의 타입을 판단하고, 그에 맞춰 플레이어의 상태값을 변경하는 방식을 이용했습니다.

<img width="815" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/4ae079a4-e98a-4a97-a7fc-796ce37899ad">

오브젝트들은 각각 플레이어의 애니메이션이나 상태값을 가져와서 상호작용을 진행합니다.

구현위치 - Well.cpp Line[76-95], StateIdle.cpp Line[76-116]


## 개선사항

Unity를 배우며 physX엔진 구조나 함수포인터, 상호작용을 좀 더 배웠습니다. 다음에 구현하게 된다면 Interaction버튼을 생성하는 과정에서 함수포인터로 어떤 행동이 이루어질지를 전달하고, Interaction객체 내에서 그 함수를 실행하는 방식으로 역할을 분기할 것 같습니다. Unity의 Interation구현에서는 위의 방식으로 구현하여 좀 더 객체지향적이고 깔끔한 코드가 나왔었습니다.
링크 - 

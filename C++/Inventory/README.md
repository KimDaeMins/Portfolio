# Inventory

## 아이템 획득 및 단축키 배치, 아이템슬롯, 아이템 사용, 획득 구현

<img width="887" alt="26-1" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/2281a7f1-8da9-4f38-836d-0bbaf7c66ff9">


## 핵심 코드

<img width="652" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/0ef8fab0-3750-418e-b77d-d3fd6b4be8fc">

아이템의 갯수 및 슬롯의 크기가 변동할 수 있습니다. 그에 맞게 vector컨테이너를 이용하여 아이템 슬롯의 크기를 유동적으로 조절했습니다. 골드 , 포션 등 단순 숫자로 나타낼 수 있는 녀석들은 int형변수를 이용했습니다.

<img width="949" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/b74e19cc-60cf-4651-887d-09d8e5844c06">

획득 : 아이템의 타입, 갯수를 입력받아 배열에 같은 타입이 있다면 갯수를 증가시켜주고 없다면 새로 만들어서 인벤토리 vector 컨테이너에 할당 후 UI로 보여줍니다.

<img width="431" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/a3e3fe05-89e5-4f19-8fd2-d1605950ddaa">

단축키 등록 : vector컨테이너에 할당된 아이템의 슬롯좌표를 받아와서 퀵슬롯에 있는 아이템 위에 덮어씌워줍니다. 실제 데이터를 관리하는 부분은 따로 인벤토리에 존재하고 단순 보여주는 용도의 슬롯입니다.

<img width="926" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/16196aa0-9b30-4603-a2f7-29da15ea488f">

사용 : 아이템의 갯수를 추출하고, 키를 받아 어떤 아이템을 사용할지 정한 후 아이템을 사용합니다. 아이템의 갯수가 0개라면 삭제합니다.

### 부적 아이템

  <img width="749" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/3a7881ef-e397-4244-ba19-9ddbcbeb421f">

부적 아이템의 경우 BitFlag를 이용하여 여러 부적 옵션을 한번에 적용할 수 있게 만들었습니다.

<img width="380" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/74e95552-4bbe-4d71-b4e1-50231ba2bea0">

특정 상황에서 &연산을 하여 부적 효과를 적용하는 경우와 하지않는경우를 나눕니다.

### 플레이어의 아이템별 모션 적용

<img width="441" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/af87f75c-ec7c-4ec7-8b19-fc0a3afd95b9">

ㅤ
<img width="386" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/ca79119f-3894-4426-b99f-7adc1848960a">

슬롯에 있는 아이템의 타입에 맞춰 플레이어의 모션을 바꿀 수 있도록 구현했습니다.
특정 키를 누르면 함수의 인자로 넣은 포인터에 값을 전달하여 아이템타입을 전달받습니다.

#### 인벤토리는 헤더에 함수별 설명을 적어두어서 다른 사람들이 쓸때 보고 쓸 수 있도록 잘 써놓았습니다. 헤더파일을 참고해주세요.


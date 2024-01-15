# Pattern Table

## 몬스터의 패턴을 설정할때 더 세부적인 조건을 따져서 현명한 AI를 만들기 위하여 사용했습니다

<img width="677" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/7c558063-9bef-410f-b65b-1695c8994db3">

<img width="868" alt="1차 스파이더 생각" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/23d377d7-3016-4d89-950a-bbdc5de643a3">

## 핵심 코드

<img width="502" alt="9-1p" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/ab398988-9a15-4743-901b-41e3a4e12f3a">

플레이어와 몬스터 사이의 거리, 각도, 몬스터의 이동가능성을 판단하여 패턴테이블을 구성합니다.

구성된 패턴 테이블의 수치를 확률로 두고 어떤 패턴을 사용할지 결정합니다.

Thread를 이용해서 시간이 걸릴 수 있는 계산을 따로 스레드로 계산했습니다.

### Thread 이용

<img width="704" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/9d5909bb-8ec5-46e8-8d5a-4ca813a7841a">

<img width="504" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/b5ca6e86-0ff6-498c-a88e-641151ad31db">

c++에서 지원하는 _beginthreadex함수를 호출하여 패턴계산에 Thread를 이용했습니다.

<img width="457" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/4af8740f-fe84-4c31-906c-b612c6b26d2e">

스레드 함수 내부에서 멤버변수의 값을 바꾸기 때문에 안정성을 위하여 CriticalSection을 이용했습니다.

구현위치 - SpiderTank_Idle.cpp Line[8-19], [34-46], [182-404]

### Data Parsing

<img width="578" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/57dc1de7-0b20-4e9b-9e86-97db7b6cbdd2">

PARSER::CSVParser를 이용하여 파일 패스 경로를 넣어서 엑셀 데이터를 vector<vector<string>> 타입으로 저장합니다.

<img width="216" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/08f06014-02f4-45cb-8e66-43fde2cb3620">

<img width="670" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/b6f905d6-d794-43ef-9e7c-5f46ce6523fa">

배열을 미리 int형으로 만들어 둔 후 각 상황에 맞는 가중치를 int형으로 가져옵니다.

<img width="316" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/7faad248-fc93-4925-8fb8-0e210c292ba3">

모든 가중치의 합과 1사이의 랜덤값을 추출한 후 인덱스 한개씩 검사하며 어떤 패턴을 사용할지 결정합니다.

구현위치 - SpiderTank_Idle.cpp Line[182~404] DataManager.cpp Line[213~242][446~458] RandomManager.cpp Line[26~51]

## 패턴 테이블 개선사항

    멀티스레드와 관련해서 데이터를 수정하기는 하지만 크리티컬 섹션을 이용해야 할 정도로 위험한 작업이 아니고(외부에서 추가변경될 여지가 없음)
    
    나머지변수는 가져오기만 할 뿐 내부에서 변경하진 않으니 크리티컬 섹션은 불필요한 추가가 아니였을까 싶습니다.

    행동영역을 세분화 하는것은 좋았지만 코드 자체가 읽기 굉장히 난해하다는 생각을 했습니다. (if문이 많고 단순 수치비교가 많았습니다 -> 코드 개선이 어려워지는 단점이 생깁니다.)

    실행 단계에서 조금 느리더라도 조건들을 각각 함수화하든 해서 가시성있는 코드를 짜는게 훨씬 좋았을 것 같습니다.

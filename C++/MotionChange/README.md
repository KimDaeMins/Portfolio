# Motion Change & Combo System

## 아이템 사용에 의한 모션 총괄 - 플레이어의 스테이트를 아이템에 맞춰 변경해주는 스테이트 

<img width="1048" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/c47fc1aa-92bc-4678-919d-c75184831244">

칼을 3번 사용하면 3타 콤보 모션이 나옵니다.

## 핵심 코드

<img width="389" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/0483277f-8fdd-4b25-9677-848a8a995b1d">

최초 공격시 누른 단축키에 맞는 타입을 가져온 후 플레이어의 상태와 애니메이션을 저장합니다.

이전에 누른 닥축키와 같은 단축키라면 콤보카운트를 가져와서 플레이어의 상태와 애니메이션을 전환합니다.

<img width="541" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/d888555d-2f31-4a3e-ab47-200ac397f876">

<img width="495" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/32cceee2-ab38-4b4d-b42a-bebb824a6102">

<img width="484" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/0e93731c-6c3c-44ac-8824-d9be14370dff">

특정 프레임 구간 이후에서 계속 키를 누르고있는지 검사한 후 같은 키를 또 한번 눌렀다면 콤보를 증가시킨 후 모션체인지스테이트에서 다음 공격 스테이트로 전환합니다.

구현위치 StateAttack.cpp Line[91-130] StateItemMotionChange.cpp Line[169-183]

### 복잡하게 구현한 이유

사용 아이템 슬롯에 (사용아이템, 무기들을 구분없이)아이템을 저장하는 기능이 있고, 그 아이템을 사용하는것이 플레이어의 스테이트 변환시키는 구조여서

아이템을 사용할 때 어떤 키를 눌렀는지 그 키에 어떤 아이템이 있는지와 같은 아이템을 사용하고 있는지(콤보중인지)를 검사했어야하는데

이부분을 한곳에서 총괄하는것이 좋다는 판단하에 따로 사용아이템에 따른 모션체인지를 하는 스테이트를 만들어서 구현했습니다.

상당히 괜찮았던 기능인 것 같습니다. 인벤토리의 정보를 받아서 플레이어가 변화하는것이 한군데에서 진행되니 버그찾기나 개선이 상당히 쉬웠습니다. 

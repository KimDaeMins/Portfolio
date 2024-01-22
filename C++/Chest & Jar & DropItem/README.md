# Chest

## 아이템의 종류에 따라 골드 또는 아이템이 나타납니다.

<img width="104" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/c72e475c-7398-4c22-b497-e1b9d4d32159">

골드가 나타나는 모습

<img width="386" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/793e92eb-8bf0-4311-9676-681ed4ca28ea">

아이템이 나타나는 모습

## 핵심 코드

<img width="610" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/cd76dc07-da73-4f17-951c-c4f02bfd5c0e">

플레이어는 상자의 상호작용위치와 충돌했다면 상자오픈 상태로 변경합니다.(이 상태에선 플레이어의 애니메이션이 변경됩니다)

<img width="626" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/349da563-a5b5-4b73-964c-be69fb8e602b">

플레이어의 애니메이션 인덱스를 판단하여 상자를 여는 애니메이션이라면 애니메이션을 같이 바꾸어줍니다.

<img width="405" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/ea5e0c81-156f-4f7c-8364-431a68bd644b">

골드일시 Coin을 좀 더 다양하게 (파랑보석, 보라보석, 금화) 나타내기 위하여 랜덤성을 만들어서 CoinTable을 생성합니다.

Coin의 종류마다 돈이 다르기때문에 가격을 조절하기위하여 각 Coin 의 종류마다 최댓값을 들고있으며 그 갯수를 넘길 순 없습니다.

<img width="802" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/b9a019d1-3060-427c-b674-932ba07c31fd">

코인테이블을 생성했다면 생성방향을 조절하여 360도로 Coin이 퍼트려서 한개씩 생성되게 구현합니다.(코인이 주르르륵 나오게 됩니다)

<img width="873" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/f0f091b8-cd34-42a9-aa51-1547d89d3f5c">

아이템일시 UI를 보여주며 인벤토리에 직접 아이템을 생성하여 넣어줍니다. 

구현위치 chest.cpp Line[67-171], specialChest.cpp Line[71-174]

### 개선사항

아이템 상자와 골드상자의 클래스가 각각 다릅니다. 어떻게보면 같은 작동을 하고있는데도 말입니다. 이부분은 하나로 묶는것이 조금 더 현명한 방법이 아닐까 싶은데, 하나로 묶는다면 인자를 넣는 방식을 조절해야해서 고민을 굉장히 많이 했었습니다.

결국 클래스를 나누는게 제작단계에선 조금 더 편하다고 생각했지만 같은 애니메이션을 하고 생김새나 Collider가 전부 같고 비슷한 코드가 많은점이 거슬렸습니다. 아마 다음에 만들면 이런부분에선 합친 후 파싱단계에서 조절하지않을까 싶습니다.


# Coin

## 땅과의 충돌횟수를 저장하여 통통 튕긴 후 플레이어를 향해 빨려오는 방식을 구현했습니다.

<img width="229" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/59432845-b56a-414b-9ae1-4c60e96b0aac">

## 핵심 코드

<img width="501" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/c968c105-8a91-4788-acb4-cc6170430a80">

땅과의 충돌횟수를 판단합니다 (Stay충돌을 판단한 이유는 Enter로 판단하면서 Force를 높게 주지 않은 경우 통통 튀는 현상이 줄어 바닥과의 충돌판정을 여러번 확인할 수 없기 때문입니다.)

땅과 일정 이상 충돌되었다면 플레이어 흡수 판단 영역을 생성합니다.

<img width="786" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/1be3b5c9-37b4-40cc-9856-8fec918bb049">

플레이어가 흡수 판단 영역 내에 들어간다면 모든 물리적 충돌을 제거한 후 플레이어의 위치로 이동시킵니다.

후에 플레이어와의 거리로 판단하여 Coin의 종류에 따라 플레이어의 돈을 증가시키며 사운드를 재생합니다.

구현위치 - Coin.cpp Line[88-158]

# DropItem

## 플레이어를 따라가기 전 WaitTime을 설정하여 일정 시간 뒤 플레이어에게 흡수되도록 구현했습니다.

<img width="259" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/2c81934b-7ab9-4f8f-a37b-8bafd3f7c13f">

## 핵심 코드

<img width="439" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/71e59ec1-f64e-4106-896b-1a2943c6fc68">

플레이어를 향해 날아가는 방식은 Coin과 같고 DropItem의 속성에 따라 Trail을 적용하거나 이미지가 다른 점 정도의 차이가 있습니다.

DropItem의 회전하는 모습을 좀 더 보여주고 싶다는 생각과 Coin만큼 통통튀는 모습은 그닥 이쁘지 않다는 판단하에 Coin과는 다른 방식을 이용했습니다.

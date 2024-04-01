# UI

## 게임 내 플레이어의 상황을 표현하기위한 UI를 제작했습니다

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/7ec984f1-b617-4be5-aca6-ca38e68a98c3)

체력바 UI입니다 체력감소시 빨갛게 표시됩니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/55c528b2-5e7b-4e9f-9f47-b6609f3e5420)

무기류의 UI입니다 장탄량이 존재한다면 남은 장탄량에 따라 게이지바가 줄어들고 폰트로 나타납니다.

### 0. 로드 및 기본구조

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/56d20c98-4d1a-45e8-8e9c-be7caa2fe115)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/60884f74-6e13-4c71-978a-4a0329d62d6d)

MFC를 이용하여 Tool제작, Tool로 배치한 UI를 Client에서 Load합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/cd95591f-8975-49f1-9376-328901a5a2d5)

mainhub는 default로써 childUI를 생성하지 않습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/bf332c93-c6ad-4374-8cf0-1056852bb2b4)

childUI가 존재하는 UI는 메인 랜더링이 아닌 child만의 랜더링을 수행합니다.

즉 UI를 각각 생성 및 로드,  UI를 개별적으로 둔 후 각각 업데이트 및 랜더링입니다.

### ★ 개선사항

유니티를 배우며 UI의 구조를 만드는법에 대해 많이 배웠으며, SceneUI와 popupUI를 나누고 종속적인 관계를 설정하는 방식을 배웠습니다.
개별적으로 각각 UI를 구성하는것보단 각각 상황에 맞춰 UI를 구성할 수 있도록 조절하는것이 좋았을 것 같고, UI의 전체적인 구조를 미리 짜둔 후 만들어야했을 것 같습니다 비효율적인 코드가 많습니다.

유니티에서의 개선 링크 - 

### 1. 랜더링

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/c2168994-efec-49f1-96b0-588c754a845f)

worldMatrix를 UI형식으로 구현하기 위하여 right값과 up값을 각각 UI의 사이즈
worldMatrix의 _41(x)인자는 x좌표 , _42(y)인자는 y좌표로 월드를 설정한 후.

뷰 매트릭스를 Identity화 worldMatrix만이 UI에 영향이가도록 설정했습니다.
추가로 ROtationMatirx를 구성하여 UI의 z회전을 통해 같은 리소스를 회전을 통해 사용할 수 있도록 조절했습니다.

### 2. 폰트 랜더링

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/ab04cd75-f0c7-4468-8bf1-c11f372fbcb5)

DirectX에 내장된 SpriteBatch, SpriteFont 객체와 내부함수 DrawString 을 이용하여 폰트를 랜더링했습니다.



### 3. child UI
 
#### 3-1. BoosterUI

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/5af9e308-7d62-4bbf-85bd-9473b044b4b3)

랜더링 단계에서 g_Gap으로 부스터의 현재 잔량을 전달하였으며, 부스터의 용량에 따라 색이 변하도록 TexChangeGap을 두어 텍스쳐를 변경했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/bb049f3e-79cd-42a4-a22b-54bb98d8494d)

픽셀 셰이더단계에서 입력받은 g_Gap의 수치에 따라 discard하여 부스터바를 실시간으로 줄였습니다.

#### 3-2. HPUI

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/805f029b-ee34-4db5-aee0-74a185e901d1)

Hp에는 파랑색, 빨강색, 하얀색 에너지칸을 두고. 로직을 구상했습니다.

체력이 줄어든다면 하얀색 바가 줄어들고 시간에따라 빨간색바가 줄어들게끔 표현하고

체력이 늘어난다면 파란색 바가 늘어나고 시간에따라 하얀색 바가 채워지며 UI를 애니메이션화 시켰습니다.

HitDelay와 RecorveDelay를 주어 회복량이 후처리되는듯한 모습을 구현했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/29466b45-5bb5-4dea-adc7-29482eb4237d)

랜더링시에는 3개의 gap수치를 전달받아 discard하여 실시간으로 UI에 적용시켰습니다.

#### 3-1. WeaponUI

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/dea50bcb-873d-4839-8f54-f5d1045e56fc)

무기의 경우 총알을 사용하는것과 사용하지 않는것을 분기하여 renderGap을 적용할지 구성하였습니다.

총알을 사용하는 경우 현재장탄량 / 최대장탄량 으로 게이지바의 비율을 조절하였습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/439d73e8-d922-4979-bad8-a3e766c091ff)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/850f7261-9d12-47b1-bae4-e70a757d68d6)

마지막으로 총알또한 gap수치를 전달받아 discard하여 ui에 적용시켰고, DrawString함수를 이용하여 장탄량, 무기이름 등을 표기했습니다.

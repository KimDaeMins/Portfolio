# DragObject 

## TanslationMatrix 응용하여 객체의 Bone에 다른 객체를 붙여 당기거나 객체를 향해 날아가는 효과를 구현했습니다.

<img width="851" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/c87ccb8b-14fe-44e1-ab94-755d7202d155">

객체를 향해 날아가는 플레이어

## 핵심 코드

<img width="754" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/e4458be6-f3dd-4316-a875-e778e82aac49">

초록색 : 오브젝트의 연결 기준 뼈를 정합니다.

보라색 : 오브젝트 혹은 플레이어를 기준 뼈에 붙입니다.

이후 뼈 사이의 간격을 줄여 날아가거나 끌어오는 효과를 구현했습니다.

빨강색 : DragAble == true ? 끌어온다 : 날아간다.


### Drag의 과정

#### 1. 특정 애니메이션 프레임에서 방향을 정하고 레이캐스트를 쏘고 데이터를 정리합니다.

  1-1. 오른손의 위치를 뽑기위해 PivotMatrix, WorldMatrix, 오른손의 BoneMatrix를 구합니다.
  
  1-2. ((BoneMatrix  * PivotMatrix) * WorldMatrix).Translation으로 위치좌표를 가져옵니다.(Origin)
  
  1-3. 타겟팅상태라면 타겟의 위치 - Origin을 Dir로 설정합니다.

  1-4. 타겟팅 상태가 아니라면 객체의 Look을 Dir로 설정합니다.(Transform.Get_State(Look));

  1-5. Origin과 Dir을 이용하여 레이캐스트를 쏜 후 결과를 Data에 정리합니다.

  1-6. 정리한 데이터를 기반으로 드래그용 객체를 생성합니다.

  구현위치 - StateTetherWand.cpp Line[84-176]


#### 2. 드래그용 객체를 이용하여 당기거나 날아갑니다.

  2-1. 1의 방식으로 플레이어 오른손의 위치를 가져온 후 드래그객체의 위치에 적용시킵니다.

  2-2. 회전은 정확성을 위하여 생성시 인자로 받았던 Dir을 바라보게 조절합니다.

  2-3. 이쁘게 보이기 위하여 8개의 Bone중 첫번째 Bone을 회전시킵니다(위쪽으로 회전시켜서 당길 때 낚시대처럼 휘어보이게 만들었습니다)

  2-4. Pivot으로 인한 보정값과 객체 생성시 받아온 정보(1-5)를 이용한 DragScale과 보정값을  마지막 Bone에 적용시킵니다.

  2-5. 1-5의 레이에 맞은 상대가 당길 수 있는 상대라면 상대의 position을, 당길 수 없는 상대라면 플레이어의 position을 마지막 Bone과 연동합니다.

  2-6. 플레이어의 특정 프레임에서 DragScale을 점점 줄입니다. -> 플레이어나 몬스터가 뼈가 줄어드는것에 맞춰서 날아가게됩니다.

  구현위치 - TetherBream Line[100-199] [282-292], StateTetherWand.cpp Line[189-200]

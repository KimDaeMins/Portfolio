# Animation Blending

## 애니메이션간의 선형보간

## 핵심 코드

<img width="631" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/6d772d25-ea3b-4507-bffe-60479a8a3ace">

애니메이션을 셋업할때 블랜딩여부를 인자로 받아 애니메이션 블랜딩을 진행할지 여부를 정합니다.

<img width="751" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/3a286749-31c6-4a4c-9446-196da57bf10c">

블랜딩을 위한 시간값을 따로 계산하고, 블랜딩상태라면 모든 뼈를 순회하여 적용합니다.

<img width="824" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/d981bdf0-9337-45f2-aeec-2493c0c4f028">

각 뼈에 애니메이션이 존재한다면(특정 애니메이션 동작 시 이 뼈가 움직인다면) 이전 뼈의 형태와 선형보간을 하여 적용하는 방식입니다.

XMVectorLerp로 크기와 위치값을 선형보간했으며, XMQuaternionSlerp로 회전값을 구면선형보간하여 각 뼈에 적용했습니다.

구현위치 - CModel.cpp Line[298-318], CHierarchyNode.cpp Line[55-113]

<img width="648" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/b49b6a24-61de-4359-9ed3-3e9ba4d95b8a">

이후 보간된 뼈는 계층구조방식으로 부모뼈부터 차례차례 뼈의 TransformationMatrix를 설정해줍니다. 자식뼈들은 부모뼈의 영향을 받습니다.

구현위치 - CHierarchyNode.cpp Line[29-38]

### 구현하며 느낀점 및 이후 개선사항

애니메이션 전환이 블랜딩간에 전환될수도 있는것이고, 이전 애니메이션과만 보간이 이루어지는것이 아니며, 각 뼈가 분리되어 적용될 수 있는점 등을 고려했을떄 구조적으로 상당히 아쉬웠습니다.

만약 애니메이션이 변경되기 직전의 Scale, Rotation, Position값을 저장한 후 이것을 이용하는 방식이 훨씬 자연스러웠을 것 같습니다.

또한 각 뼈마다 애니메이션이 다르게도 적용될 수 있는점(유니티 휴머노이드) 구조를 조금 더 세분화하여 이런부분을 미리 설정 후 개별 애니메이션의 적용을 조금 더 자연스럽게 하는것이 좋았을 것 같습니다.

위의 애니메이션이 변경되기 직전의 데이터를 저장한 후 적용하는 방식은 Tunic모작시 새로이 적용했습니다.

다만 부위별 분할 애니메이션이 없는 게임이여서 두번째 개선사항은 따로 적용하지 않았습니다.

#### Tunic 코드

<img width="544" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/2f3654d4-b2e7-4554-9558-3063cabf548a">

<img width="533" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/9bf8a9c1-846e-4fae-b32f-2b6de4240c28">

<img width="838" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/84acccdd-05bb-4ea7-b051-d2bda0e9149b">

애니메이션이 새로 셋업되었다면 기존 진행중이던 뼈의 정보를 미리 저장한 후 저장한 뼈의 정보와 보간하는 방식을 이용했습니다.
  


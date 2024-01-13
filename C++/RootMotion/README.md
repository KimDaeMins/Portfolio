# Control & Transformation Matrix

## 애니메이션 상으로는 조절되지 않은 뼈의 추가적인 회전 및 이동

<img width="343" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/ccaeeecc-2989-46ee-af26-0a4d6210b7bd">


## 핵심 코드

<img width="773" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/ba0105be-5355-4934-8eeb-2de93b25c9f9">

<img width="793" alt="16-1" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/cbcfb1ee-2a72-4293-b781-6443c04a364e">

<img width="659" alt="16-2" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/a2703290-b43e-4f9d-b15f-2f3890b67e3d">

## 설명

애니메이션 업데이트시 루트노드를 Identity화 시켜서 애니메이션상 로컬 이동, 회전을 제거합니다.

이전 RootNode 회전의 역 행렬 * 현재 RootNode매트릭스 * 이전 RootNode 이동의 역행렬 = 한 프레임동안의 이동, 회전 행렬

한 프레임동안의 회전 * 월드상 이동,회전행렬 * 최초 회전값이 적용된 위치행렬 = 애니메이션 로컬 이동, 회전 이 적용된 월드행렬

이 과정을 통해 애니메이션상 이동 및 회전을 프레임마다 월드에 바로바로 적용시킵니다.

### 과정

#### 1. 애니메이션 업데이트시 RootNode라면 이동,회전값을 제거합니다(적용할때만).

    구현위치 - HierarchyNode.cpp Line[51~72]

#### 2. 한 프레임당 애니메이션 이동,회전량을 가져옵니다.

    2-1. 이전 RootNode매트릭스의 행렬을 가져옵니다.

    2-2. 2-1의 행렬의 Rotation과 Postion을 분할해서 각각 행렬로 만듭니다.

    2-3. 2-2의 Rotation행렬의 역행렬 * 현재 RootNode행렬 * 이전 Position행렬의 역행렬 계산으로 한 프레임당 이동,회전행렬을 구합니다.

    구현위치 - Animator.cpp Line[85~106]

#### 3. 월드행렬에 한 프레임당 이동, 회전값을 적용합니다.

    3-1. 애니메이션이 시작할 타이밍에 최초 월드매트릭스를 저장합니다. (방향을 알기위하여)

    3-2. 객체 생성시 최초 설정한 Pivot매트릭스를 가져옵니다(방향을 정하기 위하여).

    3-3. 설정값에 따라 Pivot의 크기를 조절합니다.(피봇의 크기가 100이라면 이동거리가 100 늘어나는데(XMVector3TransformCoord)

    이게 적용되었을 때 정확하게 이동하는 녀석이 있고, 100배 더 이동하는 녀석이 있습니다 이는 애니메이션에 따라 다 달라서 bool값으로 통제했습니다.)

    3-4. 2의 행렬을 가져와서 이동거리에 피봇행렬을 곱합니다.

    3-5. 이동거리(_41x, _43z)에 방향값을 곱해줍니다.(Why? 애니메이션에 따라 로컬상 앞뒤좌우진행방향이 달라서 애니메이션마다 위치값을 다르게 적용하기 위하여)

    3-6. 월드의 최초 회전량(3-1)을 가져와서 애니메이션 이동량에 곱해줍니다.

    3-7. 3-6까지의 과정이 전부 적용된 행렬을 이동행렬과 회전행렬로 나눕니다 (회전 -> 이동 규칙을 지키기 위하여)

    3-8. 3-6의 회전행렬 * 현재월드행렬 * 3-6의 위치행렬로 현재 월드매트릭스를 갱신합니다. 

    구현위치 - Transform.cpp Line[61~115]
  


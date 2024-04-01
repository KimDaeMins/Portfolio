# Trail

## 칼의 잔상 등을 표현하기위하여 사용

<img width="520" alt="32-1" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/638546e3-3ba5-4bca-933f-d1d16548eb35">

## 핵심 코드

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/63aaccde-1615-4dad-9c5a-f534d6301d16)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/e9a5d3a6-6f98-4f88-a34c-d31a26ac0254)


## 설명

버텍스 버퍼와 인덱스 버퍼를 최초 생성할 때 Usage를 Dynamic으로 설정하여

map unmap을 통하여 버텍스와 인덱스를 게임 진행중 변경이 가능하게 설정한 후 

지정된 로직에 따라 버텍스를 생성하여 Trail을 만들었습니다.

### Trail 버텍스 생성

  ![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/d5f58760-d2c2-44da-a69c-a3105dfb7245)

 Trail을 적용할 상부와 하부의 Position을 받아와서 deque에 저장합니다

 deque에 저장하는 이유는 Trail의 랜더링과 삭제의 영향으로

 랜더링시 나중에 생성된것부터 먼저 Vertex를 구성하고, 삭제는 처음 생성된 녀석이 삭제되어야해서 양쪽이 뚫려있는 deque컨테이너를 이용했습니다.

 구현위치 - CVIBuffer_Trail.cpp Line[111~111]
 
### Vertex구성 및 스플라인 곡선 생성

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/050525a0-94a7-4aad-a73c-c4fc36473d16)

Vertex는 위에서 생성한 Data의 Position을 받아온 후 현재 worldMatrix의 역행렬을 곱하여 로컬좌표로 구성합니다. - 쉐이더에서 월드좌표를 곱하기 때문에 이 작업이 수행되어야합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/fe5fe8af-0624-403e-8d00-5f31e1ee4ca8)

Spline 곡선 생성 알고리즘을 구현했습니다. 이전 Position 자신 다음Position 그 다음 Position 총 네개의 Position값과 보간수치를 인자로

XMVectorCatmullRom를 호출하여 보간된 좌표를 가져온 후

위에서 구성한 Vertex다음으로 입력합니다

### Index구성

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/d8e50c08-0913-416d-8d5e-e2ce484a94b6)

Vertex구성 단계에서 이전 Vertex와 방금 만들어진 Vertex간의 길이를 TrailData에 저장합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/528d43dd-c561-4327-9b8f-f8ae1ce57021)

이후 UV구성 단계에서  현재까지의 길이 / Vertex간의 길이의 총 합 의 계산을 통하여 UV가 0~1이 되게끔 설정 후 적용합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/1c5f8209-f077-492f-b50f-8bb955a83bb5)

Trail의 진행에 따라 Vertex의 구성 및 갯수가 바뀌는것에 맞춰 Index를 재구성합니다.


# Teleport

## Physic의 Raycast를 이용 이동가능성을 탐지하고 이동하는 방식을 구현했습니다.

<img width="821" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/a9f14560-8ebc-47d9-bb6d-1da0984e44f2">

## 핵심 코드

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/6bfba503-b93b-48f3-898d-11212e68de51)

PhysX에서 제공하는 Raycast기능을 이용하여 도착지가 지상인지 판단 후 Teleport여부를 결정합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/9543b6af-f949-4b2f-b9eb-e9d5b6e095fc)

막히지 않았다는 전제하에 프레임당 이동을 구하고 실제 Teleport도착까지의 길이를 구합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/d315ce61-de4d-444d-896b-51805fd1e953)

카메라 연출을 위하여 실제로 객체를 움직입니다(거리에따라 속도를 조절하고 충돌을 제거하여 움직입니다)

도착지보다 많이 이동했거나 도착했다면 이동을 멈추고 이펙트를 삭제합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/e77f49aa-e6d0-4780-85d1-5abc4828e8cc)

Raycast를 이용한 도착지 판별의 조건들입니다. 

각 벽과 땅을 collider로 구현하였고, 오르막길과 내리막길이 있어서 텔레포트 도착지를 좀 더 복잡하게 판별한 느낌이 있습니다.


## 개선사항

   Raycast를 남발한 느낌이 있습니다. 버그가 생길 가능성이 높은 기능이라 조금 더 세밀하게 조절하고싶은 생각에 구현과정이 하드코딩 느낌이고 복잡한감이 있습니다. 다음에 구현할때는 텔레포트의 구간을 분할하여 기능을 조금 더 단순화할 것 같습니다

   ex) 구간을 5개로 나누어 위에서 아래로 Raycast -> 가장 먼거리의 Raycast부터 판별 
   5번이 땅이 아니고 4번이 땅이라면 4번 Raycast 기준으로 4번과 5번사이에 벽이 있다는 판단, 벽 앞으로 텔레포트합니다.

   2가지의 과정으로 줄일 수 있지않을까 생각했습니다. (벽을 판단하는 과정은 현재 구현의 마지막 Raycast의 과정을 이용)
  

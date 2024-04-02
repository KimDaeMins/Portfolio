# Resource Manager

## 1. 리소스 로드

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/2ed4b8ad-b5a9-4985-915d-c31bf2505b0c)

유니티의 Resources.Load 기능을 통해 경로를 입력받으면 데이터를 로드합니다

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/7892fffb-7b3e-478a-b9a5-cac8fd21bc4e)

path는 Instantiate과정에서 인자로 받습니다.

제네릭 함수로 구현하였으며 GameObject라면 오브젝트 풀에서 존재하는지(이미 생성한적이 있는지) 판단 후 생성합니다.


## 2. Instantiate

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/b829ab39-8b01-4e0d-8df4-613ad666fc08)

함수 오버로딩을 통해 인자를 넣는 방식을 편하게 할 수 있도록 구현했습니다.

1번의 Load를 통해 Original GameObject를 가져오면 오브젝트의 Prefab내부에 Poolable 스크립트의 존재여부를 판단한 후

오브젝트 풀에서 꺼내거나 Object.Instantiate함수를 부릅니다.

## 3. Destroy

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/93149ac5-bb54-4c7d-8487-b1756f9642c5)

현재 객체가 꺼져있거나 객체가 null상태라면 이미 지워진 상태와 다름없기에 return 해줍니다.

Destroy또한 Poolable스크립트의 존재를 판단한 후 있다면 오브젝트 풀에 다시 집어넣고 없다면 Object.Destory를 이용하여 삭제합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/160c13b0-eb83-412d-a0ed-b9e2cae009c6)# Frustum Culling

## 랜더링 최적화를 위하여 절두체 컬링을 구현했습니다.


## 코드 진행

#### 1. 절두체 컬링을 위한 절두체를 생성합니다.(큐브형태)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/10b6bb3e-52a4-41f9-8184-273c09ec554e)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/0caf0206-f3ab-467f-886e-f93e3b426e39)

절두체의 형태는 투영스페이스 상의 공간으로 제작합니다.

#### 2. 절두체를 월드상태로 옮깁니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/57efd1da-857b-4a76-b928-c4d0e7a8064b)

파이프라인에서 뷰행렬과 투영행렬을 가져온 후 Inverse를 이용하여 역행렬로 만듭니다

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/b0a84df3-3f2d-45a3-ba3e-1f925cac6375)

8가지의 점에 투영행렬의 역행렬 을 곱하여 뷰 공간으로 옮긴 후 다시 뷰행렬의 역행렬을 곱하여 월드공간으로 옮깁니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/96381226-cbdb-4bdd-ae77-20af88a6c68c)

XMPlaneFromPoints 함수를 이용하여 6개의 면을 구성합니다.

면을 구성할때에는 내적을 생각하고 면의 방향을 신경쓴 후 만듭니다.

#### 3. 절두체 내부에 객체가 존재하는지를 판단합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/314c5458-0dab-46fb-879b-07670190e3f2)

is_InWorldSpace가 true라면 랜더링 대기리스트에 저장합니다. 

평면과 점 사이의 거리를 구하는 XMPlaneDotCoord함수를 통해 평면과 각 면 사이의 거리가 Radius보다 클경우 - 즉 절두체 내부에 임의로 설정한 구형태가 포함되지 않을 경우 랜더링 대기리스트에 포함하지 않습니다.

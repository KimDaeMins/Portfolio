# AABB & OBB Collision

## Collision Base Update

<img width="743" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/8d8419d7-ec63-4600-a1a4-069e3d014851">

뼈에 부착된 Collider의 경우 뼈의 정보를 가져와서 TransformMatrix를 재조정한 후 박스의 업데이트의 인자로 보냅니다.

## AABB Collision

### 업데이트

<img width="697" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/9e825af8-31f1-46eb-8053-d39d232dbb03">

업데이트 과정입니다. 인자로 받은 Matrix의 회전값을 제거한 후 최초로 저장한 Center값과 Extents값을 가져옵니다.

XMVectorTransform을 이용하여 각 꼭짓점의 좌표값을 비교하여 최솟값과 최댓값을 가진 꼭짓점을 구합니다.

각 꼭짓점의 정보를 이용하여 Center와 Extent값을 정의합니다.

Radius는 충돌 최적화를 위하여 충돌범위를 Extents값을 이용해 임의로 설정했습니다.

### 충돌처리

#### AABB to AABB Collision

<img width="746" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/11d543e4-73c3-4a34-a25d-11eafdc84b68">

각각 BoundingBox의 Center와 Extents를 가져온 후 각 Box의 최솟값과 최댓값을 구합니다.

XMVectorGreater함수 이용 A박스의 Min값 > B박스의 Max값,  B박스의 Min값 >  A박스의 Max값 의 연산이 한개라도 True라면 False를 반환하도록 구현했습니다.

-> 한 박스의 최솟값좌표가 다른 박스의 최댓값좌표보다 크다면 충돌상태가 아닙니다.

#### AABB to OBB Collision

AABB Collider를 회전값이 0인 OBBCollider로 판단하고 OBB to OBB 충돌검사를 진행합니다.



## OBB COllision

### 업데이트

<img width="669" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/a2750dea-7c99-430b-9b17-df25d582a898">

업데이트 과정입니다. Matrix를 인자로 받으며 최초로 저장한 Center값과 Extents값, Orientation(Quaternion)값을 가져옵니다.

Center값에는 Matrix를 적용해 위치를 조정합니다.

Matrix의 크기, 위치값을 제거한 후 XMQuaternionRotationMatrix함수를 이용하여 Quaternion을 추출후 Orientation에 적용합니다.

Matrix에서 크기값을 추출한 후 Extents에 적용합니다.

Radius는 AABB와 같이 충돌 최적화를 위하여 충돌범위를 Extents값을 이용해 임의로 설정했습니다.

### 충돌처리

#### OBB to OBB Collision

![14](https://github.com/KimDaeMins/Portfolio/assets/68540137/71d56afe-4caf-4f1e-af71-3340e435c8a8)

각 꼭짓점을 미리 생각해둡니다.

<img width="879" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/b3f9a7f7-fa68-4cb8-abbe-e247e330646b">

BoundingBox에 저장된 데이터를 이용하여 8개의 꼭짓점을 구합니다.

구한 꼭짓점으로 Center부터 각 면으로의 Vector를 구하여 저장합니다(3방향).

각 면으로의 방향벡터를 저장합니다.

<img width="1018" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/027bcc21-99a7-4c79-9126-5687743c1e77">

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/b2a4fa4d-085e-47ef-91b4-0bab9a7a512c)

각 면으로의 Vector를 각 Axis와 내적한 결과의 합 -> Axis기준으로 본 Box의 최대길이가 되고,

그 큐브의 Center사이의 거리를 Axis와 내적한값 -> Axis기준으로 본 중점 사이의 길이 가 됩니다.

각 Box의 최대길이가 Center사이의 길이보다 작은부분이 단 한개라도 존재한다면 충돌한 상태가 아닙니다.




## Sphere Collision

### 업데이트

<img width="444" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/1c9ebb0d-d90f-413e-b942-49cdcdcdd811">

Matrix를 인자로 받으며 최초로 저장한 Center값과 Radius값을 가져옵니다.

Center값은 최초로 저장한 Center값에 MAtrix를 곱하여 저장합니다.

각 x값 y값 z값 행렬의 내적을 하여 크기의 제곱을 구합니다.

그중 가장 큰 수의 sqrt값을 최초로 저장한 Radius에 저장하여 Matrix에서 가장 큰 Scale을 가진 녀석을 기준으로 Radius를 조절합니다.

### 충돌

#### Sphere to Sphere Collision

<img width="538" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/c3b5468e-d51f-464c-8f17-0307d11fb92b">

각 BoundingBox의 Center와 Radius를 가져온 후 

Center - Center의 길이의 제곱과 Radius + Radius의 길이의 제곱을 계산하여 앞의값이 더 크다면 충돌하지 않은것이고 뒤의값이 더 크다면 충돌취급합니다.

길이를 구하는 과정에서 sqrt연산을 둘 다 해야하는 과정을 줄이기 위하여 제곱끼리의 계산으로만 사용했습니다.

#### AniBox to Sphere Collision

OBB박스, AABB박스와의 충돌은 DirectX Library에 구현되어있는 Box Collision함수를 가져와 직접 구현한 BoundingBox로 계산이 가능하도록 리팩토링 했습니다



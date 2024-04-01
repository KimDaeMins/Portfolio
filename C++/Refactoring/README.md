# Refactoring 내용입니다.

## 1. smartPointer 사용

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/27cf64e2-5c3d-47ec-8341-69424fc190b8)

DirectX기능은 레퍼런스 카운팅을 하는 pointer형에서 ComPtr로 변경하여 레퍼런스 카운팅을 직접하고 할당 및 해제를 직접해야하는 불편한점을 개선했습니다.

또한 자주 사용하는 Device와 DeviceContext를 전역변수화 하여 객체마다 할당하는 방식을 개선했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/ac772697-a775-4337-bad9-ea14c619720c)

new 를 사용하는 객체들은 전부 스마트포인터화 하여 레퍼런스 카운팅을 체크하여 메모리를 해제하거나 레퍼런스 카운팅을 증가시키는 부분을 제거했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/8ae72fa7-ba8c-463d-86bd-98b9c00761b7)

void의 shared_ptr 배열을 만들었습니다. Deleter를 이용하여 삭제를 관리합니다.

## 2. c++ 20의 기능 사용

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/4de81a4a-b7e0-4887-8db7-7e40ee5a254b)

erase_if 기능을 이용하여 리스트 순회중 삭제코드를 효율적으로 줄였으며,

contains, at을 이용하여 iter를 이용한 탐색이 아닌 바로 객체를 가져올 수 있도록 조절했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/bc0d2ecc-1682-4744-a52b-21d9c607856e)

추가로 Range 기능을 이용하여 객체가 nullptr인지 여부를 판단후 아니라면 업데이트한다 같은 조건부순회의 길이를 효과적으로 줄였습니다.

## 3. Collider, BoundingBox개선

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/d2c91831-3e28-4240-afd4-8678127b7890)

Collider의 구조를 바꾸어 기존에 AABB, OBB, Sphere용으로 따로 구현헀던 BoundingBox및 충돌함수를 한개로 통합했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/96be2884-cd70-4552-b828-57cb592dda1b)

개선 이후에는 자신과 상대 BoundingBox의 속성에 따라 자동으로 Collision을 조절하는 방식으로 구현했습니다.

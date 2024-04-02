# BloodDraw(Object)

## 0. 구성

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a4c22ff8-c36b-454c-bec9-b72865c0d021)

오브젝트 배치시 BloodDraw 스크립트를 적용해주기만 하면 작동됩니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/3dff056f-5a37-42b7-b4f9-12fc17fd3376)

최초 설정시 spriteRenderer들을 가져와서 originalTextures에 sprite형태로 저장합니다.

가져온 sprite의 정보를 토대로 texture2D와 color배열을 구상합니다.

마지막으로 brushSize를 설정하여 피가 지형에 얼만큼 적용될지를 지정합니다.

## 1.진행

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/d8fdf4f6-8b88-4186-98cf-71d893351fc7)

해당 객체와 파티클이 충돌했다면 충돌한 파티클의 정보를 가져옵니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/f68c4e2f-9938-4f57-80a8-5f6b3b00685c)

세부적인 충돌검사를 위하여 충돌된 위치와 진행방향을 추출하여 다시 Raycast작업을 수행합니다 벽쪽으로 파티클이 튀어서 부딪혔다면 부딪힌 순간에만 velocity가 벽을 향해있으므로 충돌판정이 1회 적용되는 점을 이용했습니다.

만약 충돌되었다면 충돌지점을 로컬화 시킨후 collider와 scale 정보를 이용하여 정확한 UV값을 추출합니다.

해당 UV값으로 DrawTexture작업을 수행합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/44cba816-2cb7-4e99-b691-08d8f57b967b)

UV값을 이용해 좌표를 잡고 최초 생성했던 브러시 사이즈를 이용하여 그려질 총 크기를 지정합니다.

int indexX = i % textures[0].width; int indexY = j % textures[0].height;을 이용하여 순회할 픽셀의 인덱스를 찾은 후 

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a429a43f-f499-49ff-9508-bb934b6b83ab)

위의 조건식을 이용하여 부딫힌 지점보다 멀리 갈수록 픽셀을 덜 그리도록 설정하여 픽셀의 정보를 바꿔줍니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/8ad22441-c8d9-478b-9efc-72a0ad7278f7)

픽셀의 정보가 바뀐 이후에는 텍스쳐의 SetPixels함수를 이용하여 픽셀정보를 텍스쳐에 적용합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/24e15f7c-60dd-42c5-90dc-963958a2587b)

이후 서버에 텍스쳐의 정보를 로우데이타로 넘겨서 모든 플레이어가 같은 피터짐을 볼 수 있도록 텍스쳐와 스프라이트랜더러 안에있는 스프라이트를 재설정합니다.


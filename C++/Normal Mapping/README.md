# Normal Mapping

## 모델 데이터의 입체감과 질감 구현(픽셀단위의 빛에의한 음영)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a25cc97d-155c-4b50-8f87-75ac544da3f0)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/8dfd74c4-16a4-4b80-942c-cfc1aac7b01a)

## 코드 진행

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/aa5fc50b-bb03-4402-b89c-0bf9f54b69d9)

최초 mesh 를 불러올 때 각 vertex별로 존재하는 Normal값, Tangent값, Position값, UV을 저장합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/2b06ff76-a8e5-4ecb-83cd-b02a1c729f9a)

랜더링시 각각 부위에 할당된 Texture(Normal, Diffuse)를 바인합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/af45eaa6-221c-497d-ad3b-68580c5a0712)

현재 객체의 World Matrix와 카메라의 ViewMatrix , ProjMatrix를 바인딩합니다

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a3241c07-ee3d-42bc-96e0-7695b72b6842)

각 Vertex별로 저장된 값들과, 그릴 객체의 정보를 이용하여 월드스페이스에의 Vertex의 Tabgent, Normal를 구하고, 투영스페이스에서의 Position을 구합니다.

또한 Vertex의 월드상 Normal값과(Look) Tangent값을(Right)  외적하여 BiNormal을(Up) 만듭니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/34f15d46-d09f-4897-b177-48fa543d61d9)

객체에서 랜더링시 저장한 NormalTexture의 값을 가져온 후 -1~1 범위로 변환합니다 (색으로 되어있어 0~1까지의 범위에서 *2 - 1을 해줍니다)

VertexShader 단계에서 구한 Tangent Binormal Normal값을 각각 Right Up Look 으로 가정하여 WorldMatrix를 구성합니다.

이미지상 Normal과 월드에있는 Vertex의 Normal을 곱하여 월드상 한 픽셀의 Normal값을 구합니다.

구한 정보를 Light연산을 위하여 0~1값으로 재조정 후 그립니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/fe5284d4-4c61-4542-a0a5-252c33fe9d40)

Defferd연산 단계에서 Texture형태로 저장한 Normal값과 Depth값 불러옵니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/e08ac23b-0030-450b-8b3e-29817a7c5b85)

Normal값을 -1~1의 형태로 다시 변환합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/742a4132-328b-4693-a76b-cef3e8a78e70)

저장된 Light의 방향을 반전시킨 후 Normal값과 내적하여 빛의 영향에 따른 그림자의 정도를 구합니다

그림자도 최종 계산시 0~1값으로 들어가기 때문에 saturate를 이용하여 값을 조절하고,

빛의 색을 적용하기 위하여 LightDiffuse 를 곱해줍니다.

(빛의 최솟값을 정해두기 위하여(완전 흑백 방지) LightAmbient와 MtrlAmbient간의 곱을 더해줍니다)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/2e104e57-f87c-436b-9e79-c7784f1673f3)

마지막으로 저장된 Diffuse값과 Shade값을 곱하여 빛에의한 음영을 구현합니다.


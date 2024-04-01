# Rendering

## RenderTarget

텍스쳐에 따로 랜더링을 하고, 랜더링된 텍스쳐의 정보를 이용하여 후처리셰이딩을 하기 위하여 구현했습니다.

#### 1. 생성

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/0a23bc36-ab46-436c-a9d4-3280239de0aa)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/d6bcc9b1-4ff2-474d-95ef-b9af15b28367)

디바이스에서 텍스쳐를 생성 후 텍스쳐를 이용하여 셰이더 리소스 뷰와  랜더 타겟 뷰를 생성합니다.

이 랜더타겟들을 총 관리하는 랜더타겟 매니저에서는 스왑체인을 이용하기위한 백버퍼(RTV)와 DepthStencilView, RenderTarget들을 들고있습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/e0b4be39-f41d-4e79-80a2-99755b544fb4)

MRT는 멀티랜더타겟의 약자로 랜더타겟을 여러개 두어서 한번에 다수의 텍스쳐에 그릴 수 있도록 구성했습니다.

#### 2. 랜더타겟 적용

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/b766ef4a-2e3c-4d06-9b25-dcccca6b3a23)

특정상황에서 그릴 랜더타겟을 설정하기 위한 Begin_MRT 함수입니다.

랜더링 단계에서 태그값을 받아온 후 랜더타겟 리스트를 가져옵니다.

기존에 있던 BackBufferView와 DepthStencilView는 새로운 타겟들을 장치에 바인드하는 과정에서 백버퍼의 유실을 방지하기 위하여 멤버변수로 보관합니다.

랜더타겟뷰의 주소를 배열형태로 받은 후 OMSetRenderTargets함수를 이용하여 랜더타겟을 변경합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/bde0ab0d-3cb9-4f94-a381-593922c27565)

-> 이후 랜더링시에는 픽셀셰이더에서 지정한 Out으로 텍스쳐에 각각 그려지게됩니다.

#### 3. 복구

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/56ebd76f-84bf-4b84-b4b9-e56ea98dd4e1)

셰이더 리소스 뷰를 다시 초기화 한 후 랜더타겟 적용시 멤버변수로 보관했던 BackBufferView와 DepthStencilView를 이용해 랜더타겟을 다시 변경합니다.

#### 4. 사용

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/e7ba90ac-ee9f-4e33-9df2-10f4ebef11bb)

그려진 텍스쳐들은 셰이더리소스뷰를 통해서 셰이더에 Texture2D형태로 보내지고 셰이더 내에서 UV값을 통해 사용할 수 있습니다



## 랜더링 순서

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/87ce637f-e05a-4838-a4b8-aaac5845ba21)

1. 좌표 관계없이 무조건 뒤로 그려져야 하는 부분 ex:뒷배경

2. 알파블랜딩을 하지 않는 객체들 + 그림자 구현을 위한 깊이값 저장

3. 빛과 그림자

4. 블러처리

5. 위에서 그린 텍스쳐들을 종합하여 색 블랜딩처리

6. 빛의 영향을 받지 않는 오브젝트

7. 알파블랜딩이 필요한 오브젝트

8. UI

순서로 그립니다.

## Alpha Testing

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/256eac25-ecf7-499d-b487-227da9a8d74d)

픽셀셰이더 과정에서 DiffuseTexture의 알파값을 확인 후 특정값보다 작으면 discard -> 화면에 그려지지 않게 됩니다.

구현위치 Shader_AnimMesh.hlsl Line[116~117]

## Alpha Blending / Sorting

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/552d8aa9-d067-4040-b7b3-caf93ced7e90)

알파블랜딩에서는 랜더링에서 알파값을 결정하는 덧셈혼합을 이용하였습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a3463a30-bae0-4989-bd38-e1bc9b493a10)

알파블랜딩을 하도록 BlendState를 설정해주었다면 자연스럽게 알파블랜딩이 적용됩니다.

알파블랜딩시에는 주의할점이 있는데 알파블랜딩도 결국엔 그리는것이기 때문에 z값을 저장하고, z값을 저장하게 된다면 뒤쪽에 있는 물체가 그려지지 않게 됩니다. 여기에서 두가지 해결방안이 있습니다.

### 1. z값을 저장하거나 판단하지 않는다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/3a82ec46-e1cb-49d4-8f5f-a1cbee68c1f5)

셰이더에서 Pass 를 정의할 때 DepthStencilState를 zRead와 zWrite전부 하지 않도록 설정하면 깊이값에 따른 보정이 일어나지 않습니다.

### 2. z값에 따른 Sorting을 한다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/408580e8-fefb-47fb-8980-2ef03743a2bb)

각 오브젝트에서 카메라와 오브젝트 사이의 거리를 매 Tick마다 저장하도록 합니다.

저장된 카메라와 오브젝트 사이의 거리를 z값으로 판단 후 알파블랜딩이 필요한 오브젝트들을 Sorting합니다

sorting은 리스트 컨테이너 내부 함수인 sort와 람다식을 이용하여 구현했습니다.


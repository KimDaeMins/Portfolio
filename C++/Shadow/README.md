# 그림자

## 객체들의 그림자를 구현했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/88ab2a5a-a5c6-44de-84e3-8527cf6c5696)

## 코드 진행
#### 0. 카메라 기준에서의 객체를 랜더링 합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/d6c0a75c-4be8-4ff7-b15e-70b6426b3ce0)

랜더링 과정에서 Depth값을 따로 그립니다.

#### 1. Light 기준에서 객체를 바라보는 느낌으로 객체를 랜더링합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/c61ca59b-eeed-456b-b3ac-f0e0042f9b34)

객체 랜더링시 Light기준 ViewMatrix와 ProjMatrix를 따로 전달하여 그립니다.

Diffuse값은 투명한 부분을 처리하기 위하여 따로 전달했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/54f4f760-60b8-4846-a9db-dfb7bb3665b3)

VertexShader 단계에서 Vertex의 좌표들을 카메라 기준이 아닌 Light기준으로 정렬해서 PixeShader로 옮깁니다.

구현위치 - Shader_AnimMesh.hlsl Line[186-208]

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/f8c3cc0b-3c97-43e1-b4b4-dc7dd459c6be)

PixelShader 단계에서 Diffuse에 의한 Discard를 제외한 나머지부분의 Z값을 w로 나눈 후 (0~1이 됩니다) 텍스쳐에 그립니다.

구현위치 Shader_AnimMesh.hlsl Line[222~233]

#### 2. Light연산 

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/8fcf5d16-1566-4308-81e6-18b2e29c71a7)

빛을 관리하는 Light_Manager의 Render에서 

카메라 기준으로 저장된 Target_Depth, Light기준으로 저장된 Target_LightDepth를 각각 바인딩 합니다.

또한 월드상태로의 변환을 위하여 Light기준의 View, Proj Matrix와 카메라상의 Inverse View, Inverse Proj Matrix, CamPosition을 바인딩 하고.

Light의 방향, 좌표, 거리, Diffuse, Ambient, Specular값을 각각 바인딩합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/534513aa-9d3c-4249-8980-df3f4b6fbd94)

이후 DefferedShader단계에서 UV값을 -1, 1 값으로 전환 및 z값을 구하여 투영스페이스상에서의 좌표를 구합니다,

이후 카메라의 뷰스페이스의 역행렬과 투영스페이스의 역행렬을 곱해주어 월드스페이스상의 위치를 구합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/bad47508-b998-4647-b422-8a99ef64be6d)

구한 월드스페이스상의 좌표에 View Matrix와 Proj Matrix 를 곱해주어 빛 기준 투영스페이스에서의 좌표를 구한 후 w나누기를 한 후

-1, 1 사이로 되어있는 화면을 0, 1 값으로 변환합니다(UV좌표로 변환)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/64ac8cd5-55fa-4483-9813-9114a769e107)

마지막으로 월드좌표의 빛기준 z값을 구한 후 똑같은 값일때의 부동소숫점 오차 방지를 위해 작은 값(bias)을 빼줍니다.

빛을 그릴때 저장했던 depthTexture에서 같은 UV값을 추출하여 카메라상 그려진 객체의 빛기준으로 본 z값과 빛 방향에서 그린 z값을 비교합니다.

카메라상 그려진 객체릐 빛기준 z값이 빛에서 그린 z값과 같거나 작다면 빛을 받는 부분이고, 크다면 빛을 받지 않는 부분으로 색 밝기를 줄입니다.

구현위치 Shader_Deffered.hlsl Line[124~181]

#### 참고 자료

![36-2](https://github.com/KimDaeMins/Portfolio/assets/68540137/a5ed4112-9e8e-4b27-b2d5-a606d2235aff)







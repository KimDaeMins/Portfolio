# Blur

## 빛의 번짐을 구현하기 위하여 사용했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/03054a7c-0fcd-465f-aae2-259deb180b40)

초록색 및 빨강색을 나타내는 Texture를 블러처리해서 빛이 번지는듯한 효과를 구현했습니다. 

## 코드 진행

#### 1. 가중치 설정

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/d1d32074-96e3-4c5d-a991-a39919b20b65)

블러처리를 위한 Weight값을 가중치 형식으로 설정한 후 가중치값을 정규화하여 블러처리를 조금 더 부드럽게 할 수 있도록 적용했습니다.

화면 비율을 나누어 UV값의 최소단위를 미리 저장해두었습니다.

구현위치 Renderer.cpp Line[112~135]

#### 2. 오브젝트 랜더링

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/6bac6281-bc90-4a55-b31a-82ba1f8b5940)

(따로 빛을 위한 Texture가 있는 것 같아서 구현당시엔 이 부분을 빛처럼 구현하고 블러처리를 했지만 실제 게임에선 이부분에서 따로 작업을 하기위해 색으로 조절해둔 느낌이였습니다. 텍스쳐를 인게임 내에서 조금 더 활용할 여지가 있을 것 같습니다.)

블러처리를 하고싶은 텍스쳐를 따로 불러와서 Shader에 바인딩해줍니다.

구현위치 Player.cpp Line[215]

#### 3. 다른 텍스쳐에 따로 블러처리될 부분을 랜더링

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/726bd2c6-5004-468e-9730-60d20d908261)

픽셀 셰이더 단계에서 다른(Emissive용) Texture에 그려둔 후 값이 존재하면 Diffuse의 색도 변환합니다.

구현위치 Shader_AnimMesh.hlsl Line[147~177]

#### 4. 가로 및 세로 블러처리

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/5e392c3d-14a5-4693-b78c-7578198daa95)

블러처리가 되어야 할 객체들을 전부 그린 후 그려진 텍스쳐로 후처리를 하기 위하여 순서를 조절했습니다.

구현위치 Renderer.cpp Line[162~194]

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/19558dac-5db8-4c64-98a1-6a204e821227)

가로를 먼저 그리고 세로를 그리는 방식을 이용하였으며, 객체 렌더링시 그려진 Target_Emissive 텍스쳐를 g_EmissiveTexture로 전달하여 가로를 그리고(Target_Bloom_Weith에 저장), 가로의 블러처리가 전부 완료된 Target_Bloom_Weith를 g_EmissiveTexture로 전달하여 세로를 그립니다(Target_Bloom_Height에 저장).

구현위치 Renderer.cpp Line[329~359]

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/bd86c0af-e8c4-4c5f-840b-da5c355c5371)

랜더링시에는 미리 저장해둔 UV값의 최소단위인 TexGap을 이용하여 좌측 7픽셀부터 우츨 7픽셀까지의 가중치를 계산하여 컬러값에 적용했습니다.

이 작업은 가로 블러처리와 세로블러처리가 같습니다.

구현위치 Shader_Deffered.hlsl Line[242~356]

#### 5. 최종 랜더링

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/dc523507-7140-43b7-baad-b4e01127d92a)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/60a93bb4-32de-419b-82cb-f7dc7deb863c)

마지막으로 색과 빛 연산을 한 최종 화면에 Target_Bloom_Height를 불러와서 색을 조합합니다.

구현위치 Renderer.cpp Line[310~328], Shader_AnimMesh.hlsl Line[361~373]

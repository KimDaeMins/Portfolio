# WaveManager

## 몬스터웨이브 레벨전환 및 클리어 보상 구현

<img width="866" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/2dd50815-a031-4215-a7b8-47974619fffa">

## 핵심 코드

<img width="486" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/a4a7d52c-9d36-4173-99ba-0f8c73c9d41e">

<img width="395" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/e5594cd2-c906-4938-9377-58201e9d346e">

석상 오브젝트가 Wave를 시작하며, 미리 저장해둔 몬스터 생선 패턴에 맞춰 몬스터가 생성됩니다.

Wave의 종료조건은 WaveManager에서 업데이트마다 판단하며 (시간 or 남은 몬스터수) Wave가 종료되었다면 보상이나 다음 웨이브를 생성합니다.

### 몬스터 스폰 포인트 생성

<img width="724" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/ffa7a747-44d0-46a8-883d-c36c315ec087">

사용의 간편함을 위하여 몬스터의 데이터, 스폰할 몬스터의 이름, 스폰방식을 설정할 수 있도록 구현하였습니다.

### 생성 좌표 파싱

  <img width="668" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/938f2cd7-4f09-4421-a4f9-d92ad126fa9d">

  포인트들을 유니티에서 미리 자리를 잡은 후(유니티를 툴 용도로 사용했습니다) 엑셀로 전환한 다음 엑셀에서 좌표를 읽어들이는 방식을 이용했습니다. 어느정도의 랜덤성을 위하여 위치들을 섞어주었습니다.

<img width="689" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/baa2aa11-e7e1-456e-b9f0-7634783ba215">

<img width="671" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/feedee1d-ae81-4d76-9cfa-43f29866dd72">

아이템 또한 좌표를 유니티에서 미리 파싱한 후 지정된 위치에 생성 가능합니다.

### Wave 실행

<img width="673" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/13ff35c7-1e41-4638-9f8b-952d231bd6dc">

<img width="515" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/916b7e9c-c8d4-444a-9a08-f39528383d66">

Wave는 석상 오브젝트의 상호작용을 통해 시작되며, 웨이브 시작시 데이터를 웨이브매니저에게 넘겨준 후 웨이브매니저의 Start함수를 불러주면 웨이브가 실행됩니다.

## 개선사항

사용이 쉽게 만들려고 하다보니 범용성이 굉장히 떨어집니다. 딱 이게임만을 위해 만든 느낌이고 추후 코드 리팩토링도 쉽지않습니다.

추후 유니티에서 웨이브를 구현했을 때엔 몬스터가 나오는 방식, 위치, 방향 등 세세한걸 전부 조절할 수 있게 만들었습니다.

개선 링크 - https://github.com/KimDaeMins/Portfolio/tree/main/C%23/Wave%20Manager

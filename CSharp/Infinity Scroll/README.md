# Infinity Scroll

## 1. 최초 구성

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/6a9fd4c1-9772-4bfe-93ca-f0980024be52)

세로형 무한 스크롤을 구현했습니다.

스크롤을 할 인벤토리의 Awake단계에서 Grid의 정보를 가져옵니다.

가져온 Grid정보를 바탕으로 paddingLeft와 paddingTop, Spacing.x와 spacing.y를 저장합니다.

또한 스크롤의 추가적인 트랜스폼이 있다면 제거합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/b6320e0d-98b6-4c70-908c-968daba09d80)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/df298e8a-0bfe-4877-b7ca-555140dcc97a)

다음으로 Item을 생성해줍니다. 아이템은 스크롤의 자연스러움을 위하여 상하에 1~2개의 추가적인 아이템 공간을 생성합니다.

추가로 객체 생성시 받아온 Grid정보를 이용하여 아이템의 위치를 조정한 후 데이터를 받아옵니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a654e7e6-4250-4fe8-830e-5e22851fa622)

무한스크롤이라도 일단 인벤토리이기에 마지막은 있다는 판단하에 스크롤이 가능한 총 길이를 설정합니다.

## 2. 업데이트

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/350c02db-b8b6-4b5f-9e8e-ea334676c8cb)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/afe2e9d4-46b0-46e4-817c-7317667873f8)

업데이트도중 ReLocationItem함수를 이용하여 생성한 아이템리스트의 포지션을 스크롤을 올리는 상태라면 아래로 내리는 상태라면 위로 조절합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/62e096d4-334b-4c1f-b9b2-6c9f9ae5c5d0)

아이템 UI의 위치가 바뀌었다면 아이템슬롯의 인덱스를 재설정하여 스크롤에 맞게 아이템의 인덱스를 조절한 후 아이템을 업데이트합니다.

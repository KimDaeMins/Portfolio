# Interaction / Event

## 0. 구성

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/290228fa-7b63-4573-b789-d0a68e99d3b5)

delegate와 Action을 만든 후 delegate의 리스트를 만들어서 이 리스트를 전부 수행하지 않으면 상호작용이 완료되지 않는 개념으로 구현했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a745cc5d-911a-41cd-9e16-fe73d9d811a5)

Adddelegate함수를 이용하여 외부에서 조건을 입력할 수 있도록 설정했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/f12ec2a6-5b35-4671-b819-04a734aef3ab)

플레이어의 controller에서는 OnInteractionEvent에 OnInteraction함수를 바인딩합니다(e버튼)

## 1. 진행

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/b8ba8f3d-21b2-40bd-a0ba-d7a11a5ac5f3)

플레이어 컨트롤러에서 InteractionEvent가 발생하면 OnInteraction함수가 불려지고 링크드리스트를 전부 순회하며 조건 합격 여부를 판단합니다.

조건에 맞아졌다면 하나씩 제거하며 list의 카운트가 0이 될때에 마지막 보상을 얻고 UI를 제거합니다.

## 2. 외부 예시

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/3d7243ff-1134-4c84-9121-5dc15122e6e5)

KeyInteraction함수를 구현하여 키를 얻기위한 조건을 구현합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/04e3a4f9-a9da-4043-8b8e-628d7ba4b809)

마지막 보상을 구현합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/736a7723-23a0-472c-a3df-469237163e2d)

키의 임의의 트리거와 플레이어 레이어가 닿았다면 PopupUI를 생성하고 조건을 AddDelegate함수를 통해 입력하며, 보상을 OnEndInteraction에 += 연산으로 입력합니다.

이후 1번의 진행이 이루어집니다.


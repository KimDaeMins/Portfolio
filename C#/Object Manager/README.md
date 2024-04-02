# Object Manager

## 0. 기본 구조

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/6b725626-128f-46ec-a886-e59470f3b00c)

Dictionary<enum, LinkedList>로 오브젝트를 관리합니다.

Player찾기의 용이함과 List의 Count를 가져오는 상황에서의 편리함 때문에 LinkedList로 제작했습니다.

## 1. 오브젝트 추가

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/fc7f5a4f-7f53-44b3-8d44-0ae56839ced5)

오브젝트 생성시 Add함수를 Type에 맞춰 불러줍니다.

생성된 오브젝트는 LinkedList에 넣어줍니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/6d758cd1-b042-4c51-b720-ce451c9956d4)

해당기능은 Resource Manager에서 ObjectType을 전달하여 바로 ObjectManager로 전송됩니다.

## 2. 오브젝트 삭제

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/7bb9dd96-74d2-45a0-b582-4f82760e22ab)

오브젝트의 삭제는 특정타입의 특정 객체를 삭제하는 방식으로 구현했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/590f3cbc-bba7-4449-8e23-a6c889dbefab)

이또한 ResourceManager에서 자동으로 적용되게 구현했습니다.

## 3. 가까운 오브젝트 반환

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/7a48fe28-f0b9-439d-9a48-f26c567be49c)

한 오브젝트를 기준으로 가까운 오브젝트를 반환하는 방식의 코드입니다.

magnitude를 구한 후 near값을 비교하여 가장 가까운 오브젝트의 데이터를 저장 후 마지막에 반환합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/699eba4f-98cb-40c4-b7f4-0320360e90ac)

특정 각도 안에 있는 오브젝트만 반환할때에는 normalize후 Dot연산을 이용하여 Angle을 체크한 후 반환합니다.




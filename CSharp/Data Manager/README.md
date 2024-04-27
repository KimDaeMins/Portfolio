# Data Manager

## Data.Contents / Json

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a13e224d-d27c-4c66-a5ce-c26d36ca65a4)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/c89ad521-2641-4f4b-abcd-66c23a2bc5d9)

데이터 입력 방식을 정의하고 Json 문법을 이용하여 데이터를 정리합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a5737c12-8f54-42f0-b4c8-65c1ca147efd)

Data Manager에서는 ILoader템플릿을 만들어 둡니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/be851b4a-371e-40dd-81ae-50b82257b030)

컨텐츠 구현 시 MakeDict 함수를 직접 구현하여 

스텟들을 Dictionary에 입력후 반환합니다. Key값을 id로 설정하였습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/f153732c-4b03-4c1c-b8d6-4af22c7a6805)

데이터 매니저의 Init에서 Json파일을 읽은 후 Dictionary형태로 반환하는 형식을 구현했습니다.

리소스매니저에서 path를 조립 후 TextAsset을 생성한 후  JsonUtility의 FromJson함수를 이용하여 경로의 데이터를 읽었습니다.

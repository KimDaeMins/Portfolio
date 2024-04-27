## 1. Pool 구성

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/ab172698-bcf7-47ff-a05f-53e34de61ae0)

Pop을 이용한 객체 생성 시 현재 Dictionary에 Pool의 생성여부를 판단한 후 없다면 CreatePool 함수를 이용하여 Pool을 만듭니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/e2dcd71c-6c16-4275-b6ab-6e43ecb44959)

만들어지는 Pool은 Original 오브젝트 경로 부모 Queue<Poolable> 를 들고있습니다.

최초 만들어질때의 오브젝트 풀의 크기는 5로 제한합니다.

## 2. 객체 생성

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/4faaf308-c70e-4a93-9296-22ffd2daf043)

이미 Pool이 만들어져있다면 pool의 _poolStack에서 객체를 한개 꺼내옵니다.

인자로 받은 부모가 null이라면 Don't Destory Object 구간에서 빠져나오기 위하여 현재씬을 부모로 둔 후 다시 부모를 재설정합니다.

인자로 받은 position과 rotation값을 적용한 후 SetActive를 이용하여 활성화합니다.

만일 풀에 남아있는 객체가 없을경우 Create() 함수를 이용하여 새로 만들어냅니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/8796036a-e914-4263-853f-0659d9bb8ba0)

Pool을 최초로 만들었을때 저장한 path를 이용하여 객체를 만든 후 이름을 설정하고 Poolable Script를 부착합니다.

생성시에는 Disable상태로 둡니다. pool에서 객체를 꺼내는 행동을 할 때 SetActive를 이용하여 enable시킵니다.

## 3. 객체 삭제

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/085bd76a-4ebe-4c94-9d73-944519d0a40a)

초기화 작업은 객체를 생성시에 따로 하기때문에 객체를 비활성화 해준 후

최상위 부모로 Parent를 재조절합니다.

마지막으로 poolStack에 다시 집어넣습니다 Queue형태이기 때문에 선입선출의 구조를 가져서 안정성이 있습니다. 

# Wave Manager

## 1.DataLoad

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/7ee65e55-1e7d-4647-89ea-bdaf002e8f63)

몬스터 Spawn정보에 대한 Prefab을 미리 구성해둡니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a5fc553c-17e8-4e12-8681-1fdf730611a9)

Json파일을 읽기위한 DataPath를 지정해줍니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/1b6b2070-a95b-4c17-b599-af410fd3fc4a)


![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/adfc2ca1-41b5-4981-ab00-6b0442ac1b5d)

정해진 DataPath에서 WaveData를 읽어온 후 Spawn객체를 생성하여 Queue에 입력합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/f3e688bb-0a9d-4774-b2a5-4599caa02ff4)

생성된 Spawn객체는 자식을 순회하며 정보를 Queue에 전부 저장한 후 자식을 제거합니다.

이후 Wave를 실행합니다.

## 2.Wave Rotine

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/2dacf93f-3894-47cf-b1d1-1ffe28547b33)

Spawn은 지속적으로 Update되며 Queue에 남아있는 몬스터가 0이 될때까지 Spawn Cooltime을 줄이며 객체를 생성합니다.

만약 instantCreate(즉시생성)이 활성화 되어있다면 몬스터를 한번에 생성합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/468ac3a4-11fe-43b3-bb2e-e62ba1e6e089)

현재 맵의 몬스터수가 일정 수 이하거나 몬스터가 전부 생성이 되었는데 웨이브 대기타임이 지나가버리면 WaveData에서 Spawn정보를 꺼낸 후 다음 Spawn객체를 생성합니다.

이 과정을 저장한 웨이브가 끝날때까지 반복합니다.


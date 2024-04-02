# Sound Manager

## 1. Init

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a1d43213-6974-4435-9e9c-04c1d7b805d2)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/0e5b6512-c2a4-4dc2-91c7-9f2dd03b75b0)

최초 생성시 enum으로 정해준 sound의 이름들을 System.Enum.GetName을 이용하여 찾습니다.

배열을 순회하며 각각 AudioSource를 생성하여 Dictionary에 입력합니다.

Bgm이라면 loop기능을 true로 설정합니다.

## 2. 재생

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/2349e32f-04ba-4eb1-95d9-4c5e6496e609)

총괄하는 매니저를 통해 Play함수를 부릅니다. 인자로는 Sound의 경로를 입력합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/e7a8e8c6-85dd-4bef-baa7-37440e69d400)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/cb1e9d20-54bb-4e36-9ad6-469ba23bb193)

플레이 함수를 경로로 부를 시 GetOrAddAudioClip함수를 이용하여 Sound를 찾은 후 AudioClip을 반환합니다.

이펙트사운드의 경우 자주 출력될 여지가 있으니 Dictionary의 경로와 오디오클립을 따로 저장한 후

TryGetValue 함수를 이용하여 객체를 찾아서 바로 반환하는 작업을 수행합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/f6fd6e3d-2d0a-4608-8a1c-ea03aaeec2d3)

마지막으로 audioClip을 받아 재생하는 작업을 수행합니다.

Effect의 경우 한번만 재생이 되어야하니 PlayOneShot함수를 이용합니다.

Bgm의 경우 중복브금을 방지하기 위하여 오디오가 진행중이면 재생을 중지한 후 지정된 오디오를 재생합니다.

<img width="618" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/10a9d7b1-b05d-44e3-8297-0e78d081d5fd"># Animation Event

## 특정 프레임 구간에서에 이벤트 생성

<img width="647" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/ce519744-09b4-4da2-832b-d2c37a54cf9e">


## 핵심 코드

<img width="590" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/5b86dd49-8a24-404e-96f7-b1277888f094">

## 설명

애니메이션 진행에 따른 키프레임을 받아와서 특정 구간에서 이벤트를 발생시킵니다.(사운드 재생, 어택존 생성 등)

### 과정(사운드 예시)

#### 1. 지정한 사운드 재생 구간에 도달하면 사운드를 재생합니다.

#### 2. 지정한 사운드 재생 구간을 넘어가면 다시 사운드를 재생가능한 상태로 전환합니다
    
    구현위치 - Animation.cpp Line[87~125]

## 개선사항

    하드코딩한 느낌입니다. Unity를 배워가며 함수포인터와 이벤트를 더 이해했으며, 애니메이션이벤트를 더 잘 구현할 방법을 알게되었습니다.
    새로이 구현한다면 Animation내부에서 애니메이션별로 특정 구간과 함수포인터를 저장 한 후 구간검사를 하여 특정구간에서 함수를 자동으로 실행하는 코드를 짜지 않을까 싶습니다.
    현재는 각 객체의 특정상황의 Update에서 실행중입니다. 
    툴을 이용해서 특정 구간마다 함수포인터와 범위를 넣으면 알아서 저장하는 시스템을 만드는게 좋을 것 같습니다.(유니티와 비슷하게)
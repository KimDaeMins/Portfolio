# Singletom

## 1. 싱글톤 구조

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a17bcb53-035e-4c8a-afb9-b49482dfe01e)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/93f9404d-061f-47ac-ae64-28f33fbcef6d)

스태틱 형식으로 s_instance를 제작 후 프로퍼티를 이용하여 객체에 접근할때 Init함수를 필수로 호출하게 설정했습니다.

이후 Init함수에서는 s_instnace가 할당되지 않았다면 객체를 할당한 후 DontDestroy화 시켰습니다(객체마다 다름)

이후 싱글톤 내부 변수들의 Init작업을 수행하여 s_instance변수에 할당후 관리했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/a10c7f7c-cfee-4686-adfd-73746f217240)

DontDestory특성상 씬 이동간에 지워지거나 하지 않으니 따로 초기화해야하는 객체들은 Clear함수를 구현하여 적용했습니다.

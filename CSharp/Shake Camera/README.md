# Shake Camera

## 0.구성

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/cbe33835-0cb9-4f8d-9bd6-6aa6812bc311)

각 상황에 맞는 DOTween Animation을 구상합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/49ef8a7e-795a-4f50-a9bc-dc961b7a1b77)

shake Camera 스크립트에서는 각 흔들림마다의 enum값을 가집니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/c932f67b-ae98-4a39-a813-6f2c23bec363)

DOTween내장함수인 CreateTween과 DORestart함수를 이용하여 카메라를 흔드는 효과를 구현합니다.

## 1. 외부사용

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/4d97b889-eb3a-456a-9252-fbec5e9bcfc8)

만약 플레이어와 총알이 부딫혔다면, 플레이어는 Hit상태가 되고 체력이 감소됩니다. 또한 피격자는 AddForce함수로 뒤로 밀려나는효과가 이루어지고 공격자는 Attack만큼의 미리 설정한 카메라 쉐이킹이 적용됩니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/4688609a-09ed-4939-b27c-5aa629e96b88)

피격자는 Hp의 감소에 따라 Hit만큼의 미리 설정한 카메라 쉐이킹이 작동됩니다.

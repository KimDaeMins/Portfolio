# Lerp
## 시간값에따른 객체의 상태변화

<img width="686" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/40eacdb2-4cc0-4585-be18-156e3bcf97e5">

## 핵심 코드

<img width="470" alt="13-6" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/25238cd6-8678-4001-b50c-846ac978108e">

<img width="380" alt="13-7" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/e892acec-445d-485a-b773-3cc44289ca91">

<img width="269" alt="13-8" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/61aaa463-c2e8-4b09-9134-329631297dcc">

Easing그래프의 변화량을 이용했습니다.

<img width="405" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/5804b65a-105e-4bc0-b84f-1631083c1db1">

실제 사용에서는 이펙트의 LifeTime을 주고 Easing그래프를 적용하여 LifeTime 감소에 따라 객체의 크기가 변화하는 방식을 이용했습니다.

구현위치 - Easing.cpp , WandBulletExplosion.cpp Line[62], [93-97]

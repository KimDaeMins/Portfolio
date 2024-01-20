# DragObject 

## TanslationMatrix 응용하여 객체의 Bone에 다른 객체를 붙여 당기거나 객체를 향해 날아가는 효과를 구현했습니다.

<img width="851" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/c87ccb8b-14fe-44e1-ab94-755d7202d155">

객체를 향해 날아가는 플레이어

## 핵심 코드

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/1f1e865d-c7ce-4169-a4a9-270dee4ef36b)

어느 위치(무기) 에서 어느 방향(타겟 혹은 전방) 으로 Ray를 발사할지 정합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/d9c2d3e2-060e-480c-ba15-1ab8ae71ed9a)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/6f2fd224-d780-4b19-8320-d98c01cad91e)

충돌한 객체가 벽인지, 몬스터인지, 당길 수 있는 오브젝트인지 등을 판단하여 데이터를 정리합니다.

정리된 데이터에 따라 아래 코드를 실행합니다.

<img width="754" alt="image" src="https://github.com/KimDaeMins/Portfolio/assets/68540137/e4458be6-f3dd-4316-a875-e778e82aac49">

초록색 : 오브젝트의 연결 기준 뼈를 정합니다.

보라색 : 오브젝트 혹은 플레이어를 기준 뼈에 붙입니다.

이후 뼈 사이의 간격을 줄여 날아가거나 끌어오는 효과를 구현했습니다.

빨강색 : DragAble == true ? 끌어온다 : 날아간다.

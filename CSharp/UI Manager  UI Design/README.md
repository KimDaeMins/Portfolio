# UI Manager / UI Design

## 1. UI Design

### 1-1 UI Base

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/40ef5a29-c139-435c-824a-b671a8af5435)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/99477ead-8574-4494-a692-b9345057ec66)

모든 UI들은 UIBase 를 상속받도록 만들었습니다.

UI_Base에서는 UI 바인딩, UI에 이벤트를 바인딩, Refreash(UI업데이트)의 작업을 수행합니다.

### 1-2 UI Scene

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/ec800cb1-84f8-4bdf-a168-2f8eaa3478c5)

UI Scene은 씬에 하나만 존재하는 꺼지지않는 UI라고 생각하고 제작했습니다.

씬 전환시 단 한번 생성하는 용도로 Popup관리에 포함되지 않도록 설정했습니다.

### 1-3 UI Popup

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/8c50b871-6378-407c-980b-a35c9e206235)

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/4d5f94d7-0f6c-4371-aa71-bd35830320ab)

PopupUI는 생성 삭제 관리가 자주 일어나는 UI들을 분류했습니다. 매니저를 통해 생성, 삭제 , 숨기기, SubItem생성 등 다양한 작업을 수행합니다.

## 2. UIManager

생성된 모든 UI를 관리합니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/8bba1d4a-17cd-4314-b561-ac4836e57fde)

씬과 팝업으로 나눈 후 PopupUI는 순회및 삭제, 위치변경이 용이하도록 LinkedList로 구현했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/aeea0e08-5951-4739-b44a-325338aaac16)

팝업창의 SortOrder를 따로 설정하지 않고 켜고 끄는대로 조절이 가능하도록 자동화시켰습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/44bebe7e-2413-42fa-8dcb-55a21e4098b7)

켰다 껏다 하는 방식이 자유로운 PopUpUI의 경우 같은 버튼으로 Toggle형식의 동작을 수행하기 위하여 함수를 구현했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/397af379-0b6f-4a1c-b215-ecbc09b10847)

PopupUI를 키게된다면 SetCanvas 함수가 불려집니다.

이 구간에선 팝업을 링크드리스트의 제일 마지막으로 옮긴 후 sortOrder를 조절하여 최전방으로 나타나게 조절해줍니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/c72a0481-7fd4-4b18-8d77-16d6ac487838)

열려있는 모든 팝업을 순회하며 Refresh(업데이트)하는 방식을 구현했습니다.

![image](https://github.com/KimDaeMins/Portfolio/assets/68540137/9ed1eb35-f48c-4fad-853a-81c4aee500f3)

팝업을 닫을 시 단순히 비활성화만 시키는 녀석과 아예 삭제시키는 녀석을 구분한 후 잠시 Disable시키거나 아예 팝업리스트에서 제거하는 작업을 수행합니다.





# Reset Protocol

## 🎮 프로젝트 소개 (Introduction)
<img width="2555" height="1463" alt="Image" src="https://github.com/user-attachments/assets/3393d030-077d-4487-8953-3598bc8d7abf" />

**Reset Protocol**은 가상 세계의 게임 스테이지를 직접 유지보수하는 1인칭 협동 멀티플레이 게임입니다. 플레이어들은 팀을 이루어 현실의 게이머가 어지럽힌 게임 스테이지를 복원하는 임무를 수행합니다.

## ✨ 주요 기능 (Features)

|![Image](https://github.com/user-attachments/assets/ab3504c8-dbf3-4e90-9acb-3b596878e7a7)|![Image](https://github.com/user-attachments/assets/dd1126d3-7477-4de0-bb43-5f2db5b4a40a)|![Image](https://github.com/user-attachments/assets/687e07d2-36ca-4246-b17e-f288089525af)|
|:---:|:---:|:---:|
|*동료와 힘을 합치세요*|*어지럽혀진 스테이지를 복원하세요*|*버그 오브젝트는 복원하지 마세요*|

- **협동 멀티플레이**: Steamworks 기반의 멀티플레이를 지원하여 친구와 함께 플레이할 수 있습니다. (세션 생성, 참가, 친구 초대 기능 포함)
  - Advanced Session Plugin
- **동적 상호작용 시스템**: 게임 월드 내 다양한 장비 및 오브젝트(Reset 오브젝트)와 상호작용하여 문제를 해결합니다.
  - 다양한 청소 도구
    - 빗자루, 밀대, 망치 등
  - 각 청소 도구를 사용한 오염 요소 해결
    - 쓰레기 줍기, 수리하기, 얼룩 닦기, 제자리 놓기 등
- **버그 오브젝트와 버그 파인더**: 버그 오브젝트를 복원하게 되면 스테이지 클리어 후 결과에 패널티를 받습니다. 버그 파인더를 사용하여 버그를 식별할 수 있습니다.

## 🕹️ 조작법

| 키 | 동작 |
| :--- | :--- |
| **Tab** | 조작키 토글 |
| **W, A, S, D** | 이동 |
| **마우스 좌클릭** | 줍기 / 얼룩 지우기 |
| **마우스 우클릭** | 줌 확대 |
| **Space Bar** | 점프 |
| **Shift** | 달리기 |
| **C** | 앉기 |
| **달리기 + C** | 슬라이딩 |
| **F** | 청소 도구 줍기 |
| **G** | 수리 |
| **1 ~ 4** | 청소 도구 교체 |
| **Y** | 준비 |
| **U** | 게임 시작 |
| **X** | 도구 장착 해제 |
| **Z** | 도구 던지기 |

## 🧩 Reset 오브젝트 구조
<img width="800" height="750" alt="Reset 오브젝트 UML ver최종" src="https://github.com/user-attachments/assets/5989555d-2473-4b74-aa9b-e34117044da5" />

## 🛠️ 기술 스택 (Tech Stack)

- **Engine**: Unreal Engine 5.6
- **Language**: C++, Blueprint
- **Core Plugins**:
  - `Advanced Sessions` & `Advanced Steam Sessions`: Steam 기반 멀티플레이 세션 관리를 위해 사용합니다.
  - `OnlineSubsystemSteam`: Steamworks API 연동을 위한 핵심 서브시스템입니다.

## 🚀 시작하기 (Getting Started)

>2가지 방법으로 플레이할 수 있습니다.<br>
>
>1.Release 다운로드 후 실행(권장)<br>
>2.리포지토리 클론 후 직접 빌드 : 아래 항목을 따라하세요

1.  **저장소 클론**
    ```bash
    git clone https://github.com/[YourUsername]/ResetProtocol.git
    ```

2.  **필수 프로그램 설치**
    - **Unreal Engine 5.6**: Epic Games Launcher를 통해 설치합니다.
    - **Visual Studio 2022**: 'C++를 사용한 게임 개발' 워크로드를 반드시 포함하여 설치합니다.

3.  **프로젝트 파일 생성**
    - 프로젝트 루트 디렉토리에서 `ResetProtocol.uproject` 파일을 우클릭합니다.
    - `Generate Visual Studio project files` 메뉴를 선택하여 Visual Studio 솔루션(`.sln`) 파일을 생성합니다.

4.  **빌드**
    - 생성된 `ResetProtocol.sln` 파일을 Visual Studio 2022에서 엽니다.
    - `Development Editor` 빌드 구성으로 `ResetProtocol` 프로젝트를 빌드합니다.

5.  **프로젝트 실행**
    - 빌드가 성공하면, Visual Studio에서 `F5` 키를 눌러 디버깅을 시작하거나 언리얼 에디터에서 직접 프로젝트를 열 수 있습니다.
    - 에디터가 열리면 `Content/Maps/LoadInLevel`을 열어 시작할 수 있습니다.




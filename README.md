# Reset Protocol

![header](https://via.placeholder.com/800x200.png?text=Reset+Protocol)

## 🎮 프로젝트 소개 (Introduction)

**Reset Protocol**은 가상 세계의 게임 스테이지를 직접 유지보수하는 1인칭 협동 멀티플레이 게임입니다. 플레이어들은 팀을 이루어 현실의 게이머가 어지럽힌 게임 스테이지를 복원하는 임무를 수행합니다.

## ✨ 주요 기능 (Features)

- **협동 멀티플레이**: Steamworks 기반의 멀티플레이를 지원하여 친구와 함께 플레이할 수 있습니다. (세션 생성, 참가, 친구 초대 기능 포함)
- **동적 상호작용 시스템**: 게임 월드 내 다양한 장비 및 오염 요소와 상호작용하여 문제를 해결합니다.
- **1인칭 캐릭터**: 몰입감 높은 1인칭 시점의 캐릭터 조작 및 액션을 제공합니다.
- **게임 UI 시스템**: 로비, 친구 목록, 인게임 HUD 등 원활한 게임 플레이를 위한 UI가 구현되어 있습니다.

## 🛠️ 기술 스택 (Tech Stack)

- **Engine**: Unreal Engine 5.6
- **Language**: C++, Blueprint
- **Core Plugins**:
  - `Advanced Sessions` & `Advanced Steam Sessions`: Steam 기반 멀티플레이 세션 관리를 위해 사용합니다.
  - `OnlineSubsystemSteam`: Steamworks API 연동을 위한 핵심 서브시스템입니다.
  - `VisualStudioTools`: Visual Studio 연동 및 디버깅 효율을 높입니다.

## 📂 프로젝트 구조 (Project Structure)

- `Source/ResetProtocol/`: 게임의 핵심 로직을 담고 있는 C++ 소스 코드입니다.
- `Content/Blueprint/`: 캐릭터, 아이템, 상호작용 등 주요 게임플레이 로직이 구현된 블루프린트 에셋이 위치합니다.
- `Content/Maps/`: `LobbyLevel`, `GameLevel` 등 실제 게임이 진행되는 레벨 파일들입니다.
- `Content/UI/`: WBP (Widget Blueprint) 파일들로, 게임의 모든 UI 요소들을 포함합니다.
- `Config/`: 게임 엔진, 입력, 게임 규칙 등 프로젝트의 전반적인 설정 파일들이 위치합니다.

## 🚀 시작하기 (Getting Started)

프로젝트를 로컬 환경에서 설정하고 실행하는 방법입니다.

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
    - 에디터가 열리면 `Content/Maps/LobbyLevel`을 열어 시작할 수 있습니다.

## 🤝 기여 방법 (Contributing)

이 프로젝트에 기여하고 싶으시다면 언제든지 환영합니다. 버그 리포트, 기능 제안, 코드 기여 등 모든 형태의 기여가 가능합니다.

1.  이 저장소를 Fork합니다.
2.  새로운 기능에 대한 브랜치를 생성합니다. (`git checkout -b feature/AmazingFeature`)
3.  변경사항을 커밋합니다. (`git commit -m 'Add some AmazingFeature'`)
4.  브랜치에 푸시합니다. (`git push origin feature/AmazingFeature`)
5.  Pull Request를 생성합니다.

## 📄 라이선스 (License)

이 프로젝트는 MIT 라이선스를 따르는 것을 권장합니다. 라이선스 파일을 추가하고 내용을 명시해주세요.

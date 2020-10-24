# CGGroup11_MiniProject
컴퓨터그래픽스 과목 미니프로젝트 11조의 GIT입니다.

## Git 사용법
(Git Desktop 기준)
저장소 불러오기: Clone repository->CGGroup11_MiniProject 선택->다운로드할 Local path 선택->Clone

### 파일 업로드하기(Upload or commit)
프로젝트 폴더 내의 어떤 파일이든 수정하고 나면 Commit으로 저장소에 현재 파일을 업로드해야 합니다.
다음과 같은 방법으로 진행할 수 있습니다.
1. Summary에 요약 적기 (간결하게 적기)
ex) 원그리기 기능 추가 및 버그 수정
또는, Add drawing circle function and bugfix.
2. Description에 자세한 설명 적기
ex) 원그리기 기능을 추가하였습니다.
어떤 버튼을 눌러 원그리기를 실행할 수 있습니다.
DrawCircle(중심, 마우스 커서 위치)를 호출하여 사용할 수 있습니다.
(영어로 적어도 좋아요! 자세하게 적어주면 더 좋습니다.)

### 파일 내려받기(Download)
현재 Repository의 파일로 동기화하고 싶으면 Pull을 해야 합니다.
상단 탭에서 Repository->Pull 클릭

## 주의사항
둘 이상의 사람이 같은 파일을 수정하면 충돌(Conflict) 현상이 발생합니다.
다른 사람의 작업물을 수정할 때에는 반드시 상호 허락 하에 진행합시다.

작업을 할 때에는 반드시 branch를 만들어서 진행해주세요.
branch를 만들면 자기만의 저장소를 복사해서 사용할 수 있게 됩니다.
그러나 이 상태에서는 push를 해도 원래 저장소와 다른 곳에 push됩니다.
하나의 작업이 끝나면 Merge로 원래 branch로 돌아올 수 있습니다.

요약하면,
branch 만들기->열심히 작업하기->기능이 어느 정도 완성되었고, 오류가 나지 않는다->Merge
-> branch 만들기 (REPEAT)

이 과정이라고 할 수 있습니다.

단, merge를 하면 되돌릴 수 없으니 주의하세요. 충돌이 일어났을 때 해결이 되지 않으면 바로 팀원에게 도움을 요청하세요.

버그가 생기지 않고 충돌이 생기지 않는 한도 내에서 자주 자주 merge 해주세요!
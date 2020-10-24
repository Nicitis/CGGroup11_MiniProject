# CGGroup11_MiniProject

컴퓨터그래픽스 과목 미니프로젝트 11조의 GIT입니다.

## Git 사용법

(Git Desktop 기준)

저장소 불러오기: 

1. Clone repository 클릭
2. CGGroup11_MiniProject 선택
3. 다운로드할 Local path 선택
4. Clone 클릭

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

+ 가능하면 자주 commit 해주세요. 매일 매일 commit하려 노력해봅시다. 팀원의 상황을 알기도 좋고, commit한 기록이 남아 포트폴리오로 사용하기도 좋습니다.

### 파일 내려받기(Download)

현재 Repository의 파일로 동기화하고 싶으면 Pull을 해야 합니다.

방법: 상단 탭에서 Repository->Pull 클릭하기

## 주의사항

둘 이상의 사람이 같은 파일을 수정하면 충돌(Conflict) 현상이 발생합니다.

다른 사람의 작업물을 수정할 때에는 반드시 상호 허락 하에 진행합시다.

### Branch

작업을 할 때에는 반드시 branch를 만들어서 진행해주세요.

branch를 만들면 자기만의 저장소를 복사해서 사용할 수 있게 됩니다.

그러나 이 상태에서는 push를 해도 원래 저장소와 다른 곳에 push됩니다.

하나의 작업이 끝나면 Merge로 원래 branch로 돌아올 수 있습니다.

요약:

**branch 만들기->열심히 작업하기->기능이 어느 정도 완성되었고, 오류가 나지 않는다-> Merge -> branch 만들기 (REPEAT)**



+ 단, merge를 하면 되돌릴 수 없으니 주의하세요. 충돌이 일어났을 때 해결이 되지 않으면 바로 팀원에게 도움을 요청하세요.

+ 버그가 생기지 않고 충돌이 생기지 않는 한도 내에서 자주 자주 merge 해주세요!


## 코드 작성 관습

일관성 있고 깔끔한 코드 작성을 위해 다음 원칙을 따라주세요.

1. 카멜(Camel) 표기법 사용

* 함수의 시작: 대문자
* 변수의 시작: 소문자
* 단어마다 대문자로 구분하기
  * ex1) Function: drawcircle -> DrawCircle
  * ex2) Variables: identitymatrix -> identityMatrix

2. 변수, 함수의 이름은 의미있게 지을 것(Meaningful naming for variable and function)

* 무성의하게 이름을 지으면 나중에 읽을 때 힘들 수 있습니다.
* i, j, k는 예외(for, while)
* ex) a, b, c -> grade, height, tempArray

3. 전역 변수는 최대한 자제할 것(Don't use global variables)

4. 가능하면 *주석*을 달 것 (Please write *COMMENTS*)

* 특히, 외부에서 복사해온 코드가 있으면 출처(Source)를 남겨주세요.
* 함수마다 그 함수가 뭘 하는 함수인지 한줄 정도로 간략하게 써주시면 좋습니다!
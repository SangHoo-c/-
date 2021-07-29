### 메모리 관리 

> Main Topic 
>
> - 메모리 구조 
> - 메모리 누수 
> - 동적할당 되는 메모리의 위치 





####   프로세스 vs 쓰레드 



프로세스를 4 줄로 간단하게 정리하자면 이와 같다. 

> 프로세스 : 컴퓨터에서 연속적으로 실행되고 있는 프로그램 
>
> 각각 독립된 메모리 영역 ( Code, Data, Stack, Heap 의 구조 )
>
> 프로세스당 최소 1개의 스레드 (메인 스레드) 를 갖는다. 
>
> 별도의 공간에서 수행, 다른 프로세스에 접근하려면 IPC 통신을 해야한다. 

![img](https://gmlwjd9405.github.io/images/os-process-and-thread/process.png)





쓰레드도 정리해보자 .

> 쓰레드 : 프로세스 내에서 실행되는 여러 흐름 단위 
>
> 프로세스 내에서 Stack 만 각각 할당 받고, Code, Data, Heap 은 공유한다. 
>
> 프로세스 내의 주소공간, 자원 (힙 공간 등,,) 을 공유하며 실행된다. 

![img](https://gmlwjd9405.github.io/images/os-process-and-thread/thread.png)





#### 동적 메모리 할당 



Run-Time 동안 사용할 메모리 공간을 할당하는 것. 

동적 할당은 [프로세스](https://ko.wikipedia.org/wiki/프로세스)의 [힙](https://ko.wikipedia.org/wiki/힙_(프로그래밍))영역에서 할당하므로 프로세스가 종료되면 [운영 체제](https://ko.wikipedia.org/wiki/운영_체제)에 메모리 리소스가 반납되므로 해제된다. 
그러나 프로세스가 계속 실행될 때에는 동적할당 된 영역은 유지되므로 프로그램이 정해진 힙 영역의 크기를 넘는 메모리 할당을 요구하면 할당되지 않는다. 따라서 사용이 완료된 영역은 반납하는 것이 유리한데, 프로그래머가 함수를 사용해서 해제해야 한다.



정적 메모리 할당 : 프로그램 실행 될 때 이미 해당 메모리의 크기가 결정된다. Stack 에 할당된 메모리



![img](https://t1.daumcdn.net/cfile/tistory/99A29B395A93C9B417)



1. CODE : 코드 전체를 저장
2. DATA : 전역변수 라는 변수값을 저장
3. HEAP : malloc 과 같이 runtime 에서 메모리를 할당해준 값들을 저장 
   => 실행되는 도중에 HEAP 부분은 계속 변경된다.
   => **runtime** 시간에 결정
4. STACK : 다양한 지역변수 & 함수 들이 저장 
   => 프로그램을 빌드 & 실행 시키는 과정에서 결정 
   => **compile time** 시간에 결정 

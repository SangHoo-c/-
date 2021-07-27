- [x] : GIL
- [ ] : reference counting
- [ ] : garbage collection



### GIL (global interpreter lock)

GIL : 인터프리터가 한 스레만 하나의 바이트코드를 실행 시킬 수 있도록 해주는 LOCK. 하나의 스레드에 모든 자원을 허락하고 그 후에는 LOCK 을 걸어 다른 스레드는 실행할 수 없게 막어버리는 것. 

#### Python 에서 GIL 을 사용하는 이유 

- python :  *garbage collection* , *reference counting*  통한 메모리 관리. 
- python 의 모든 객체는 *reference count* (해당 변수가 참조된 수) 를 저장 하고 있음 
- 멀티 스레드인 경우, 여러 스레드가 하나의 객체를 사용한다면, *reference count* 를 관리하기 위해 모든 객체에 대한 lock 이 필요
  **=> 엄청난 비효율. **

**비효율을 막기 위해, GIL 을 사용해서 모든 객체들에 대한 *reference count* 의 동기화 문제 해결.**






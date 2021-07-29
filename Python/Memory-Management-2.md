## Python 은 메모리 관리를 어떻게 하는가? 

- Reference Counts
- Automatic Garbage Collection 


-----


### Automatic Garbage Collection 



> **main Topic**
>
> how to solve 순환 참조 problem ?! 





reference count 도 GC 라고 부른다. 순환 참조를 해결하기 위해 구현한 GC 를 'Automatic Garbage Collection' 이라 한다. 

Python 에선  [Cyclic Garbage Collection](https://docs.python.org/3/c-api/gcsupport.html) 을 지원한다. 

이는 순환 참조를 해결하기 위해 존재하며, **참조 주기**를 감지하여 메모리 누수를 예방한다. 



#### Generational Hypothesis



Garbage Collection 은 Generational Hypothesis 를 기반으로 작동한다. 



> ***Generational Hypothesis***
>
> "대부분의 객체는 생성되고 오래 살아남지 못하고 곧바로 버려진다. "
>
> "젊은 객체가 오래된 객체를 참조하는 상황은 드물다. "



이 가설을 기반으로 메모리에 존재하는 객체를 오래된 객체(old)와 젊은 객체(young)로 나눌 수 있는데, 
대부분의 객체는 생성되고 곧바로 버려지기 때문에 젊은 객체에 비교적 더 많이 존재한다고 볼 수 있다. 

즉, 해당 가설기반으로 작동한다는 것은, 
대부분의 객체는 젊은 객체이니, Garbage Collector 가 작동 빈도수를 높여 젊은 객체 위주로 관리해주는 것이다. 

***0 세대 일수록 더 자주 GC 하도록 설계되어 있음!*** 



#### 세대 관리 

Python 에선 객체 관리를 위한 영역 (세대, generation) 을 3 가지로 나눈다. 

세대를 초기화할 때   [_PyGC_Initialize](https://github.com/python/cpython/blob/bf8162c8c45338470bbe487c8769bba20bde66c2/Modules/gcmodule.c#L129) 메소드를 호출하는데 3세대를 초기화하는 걸 확인할 수 있다. 



```c
#define NUM_GENERATIONS 3                 /* 3세대로 관리 */
#define GEN_HEAD(state, n) (&(state)->generations[n].head)

// ...

void
_PyGC_Initialize(struct _gc_runtime_state *state)
{
    state->enabled = 1; /* automatic collection enabled? */

  #define _GEN_HEAD(n) GEN_HEAD(state, n)
    struct gc_generation generations[NUM_GENERATIONS] = {
        /* PyGC_Head,                                           threshold,    count */
        \{\{(uintptr_t)_GEN_HEAD(0), (uintptr_t)_GEN_HEAD(0)\},   700,        0\},      /** 0세대 초기화 */
        \{\{(uintptr_t)_GEN_HEAD(1), (uintptr_t)_GEN_HEAD(1)\},   10,         0\},      /** 1세대 초기화 */
        \{\{(uintptr_t)_GEN_HEAD(2), (uintptr_t)_GEN_HEAD(2)\},   10,         0\},      /** 2세대 초기화 */
    };
    for (int i = 0; i < NUM_GENERATIONS; i++) {
        state->generations[i] = generations[i];
    };
}
```



임계값 (thresold) 를 700 / 10 /10 으로  count 는 0 / 0 / 0 으로 초기화 한다. 



```python
import gc

print(gc.get_threshold())			# 설정된 임계값 확인 
print(gc.get_count())					# 현재 count 확인 

"""
(700, 10, 10)
(248, 2, 1)			# 현재 count 상태를 확인하는 것 
"""
```



세팅한 세대별 임계값 / 할당된 카운트를 비교하여 Garbage Collection 을 결정한다. 



> 임계값 활용 방법 
>
> 1. 객체가 생성될 때 0 세대의 카운트 값이 증가. 
> 2. 0 세대의 카운트와 임계값을 비교한다. 
> 3. 만약 카운트가 임계값보다 클 때, Garbage Collection 실행, 0 세대는 초기화 
> 4. 0 세대의 임계값을 넘은 객체는 다음 1세대로 옮겨진다. 
> 5. 1 세대의 카운트 1 증가 

식으로 정리하자면, 

> 1. `generation[0].count++`, 
> 2. 해제시 `generation[0].count--` 발생
> 3. `generation[0].count > threshold[0]`이면 `genereation[0].count = 0`, `generation[1].count++`이 발생
> 4. `generation[1].count > 10`일 때 0세대, 1세대 count를 0으로 만들고 `generation[2].count++`을 한다는 뜻



***이런 방식으로 젊은 세대에서 임계값이 초과하면 오래된 세대로 위임하는 방식!***



코드로 이를 확인해보자. 

Python 에서 객체가 생성될 때  [_PyObject_GC_Alloc](https://github.com/python/cpython/blob/master/Modules/gcmodule.c#L1934) 메소드를 호출한다. 

```C
static PyObject *
_PyObject_GC_Alloc(int use_calloc, size_t basicsize)
{
    struct _gc_runtime_state *state = &_PyRuntime.gc;
    PyObject *op;
    PyGC_Head *g;
    size_t size;
    if (basicsize > PY_SSIZE_T_MAX - sizeof(PyGC_Head))                         /* 메모리 할당 */
        return PyErr_NoMemory();
    size = sizeof(PyGC_Head) + basicsize;
    if (use_calloc)
        g = (PyGC_Head *)PyObject_Calloc(1, size);
    else
        g = (PyGC_Head *)PyObject_Malloc(size);
    if (g == NULL)
        return PyErr_NoMemory();
    assert(((uintptr_t)g & 3) == 0);  // g must be aligned 4bytes boundary
    g->_gc_next = 0;
    g->_gc_prev = 0;
    state->generations[0].count++; /* number of allocated GC objects */         /* 0세대 증가 */
    if (state->generations[0].count > state->generations[0].threshold &&        /* 임계값 비교 */
        state->enabled &&                                                       /* 사용여부 */
        state->generations[0].threshold &&                                      /* 임계값 설정 여부 */
        !state->collecting &&                                                   /* 수집중 여부 */
        !PyErr_Occurred()) {

        state->collecting = 1;                                                  /* 수집 상태 활성화 */
        collect_generations(state);                                             /* 모든 세대 검사 메소드 */
        state->collecting = 0;
    }
    op = FROM_GC(g);
    return op;
}
```



generations[0].count (0세대)를 증가 이후, 현재 상태를 확인하며  [collect_generations](https://github.com/python/cpython/blob/master/Modules/gcmodule.c#L1229)을 호출 한다. 



#### 라이프 사이클 

이렇듯 가비지 컬렉터는 세대와 임계값을 통해 가비지 컬렉션의 주기를 관리한다. 

이제 가비지 컬렉터가 어떻게 순환 참조를 발견하는지 알아보기에 앞서,  **가비지 컬렉션** 의 실행 과정(라이프 사이클)을 간단하게 알아보자.





> 1. 새로운 객체가 만들어질때 파이썬은 객체를 메모리와 0세대에 할당한다.  
> 2. 만약 0세대의 객체 수가 `threshold 0`보다 크면 `collect_generations()`를 실행한다.
> 3. `collect_generations()` 메서드가 호출되면 가장 오래된 세대(2세대)부터 역으로 확인한다. 
> 4. 해당 세대에 객체가 할당된 횟수가 각 세대에 대응되는 `threshold n`보다 크면 `collect()`를 호출해 가비지 컬렉션을 수행한다.
> 5. `collect()` 메서드는 **순환 참조 탐지 알고리즘**을 수행하고 특정 세대에서 *도달할 수 있는 객체*(reachable)와 *도달할 수 없는 객체*(unreachable)를 구분하고 도달할 수 없는 객체 집합을 찾는다. 
> 6. *도달할 수 있는 객체* 집합은 다음 상위 세대로 합쳐지고(0세대에서 수행되었으면 1세대로 이동), *도달할 수 없는 객체* 집합은 콜백을 수행한 후 메모리에서 해제된다.





3번의 collect_generations() 메서드의 코드는 이와 같다. 

##### collect_generations

```C
static Py_ssize_t
collect_generations(struct _gc_runtime_state *state)
{
    Py_ssize_t n = 0;
    /** 마지막 세대부터 확인 */
    for (int i = NUM_GENERATIONS-1; i >= 0; i--) {                           
        if (state->generations[i].count > state->generations[i].threshold) {
          	/** 새 객체 수가 기존 객체 수의 25%를 초과하면 전체 콜렉션 실행 */
            if (i == NUM_GENERATIONS - 1 && state->long_lived_pending < state->long_lived_total / 4)   
                continue;
            n = collect_with_callback(state, i);	/* 조건문에 만족하는 세대를 인자값으로 전달 */
            break;
        }
    }
    return n;
}
```



오래된 세대 (2) -> 젊은 세대 (1) 로 내려오면서 임계값 검사를 한다. 

이는 가비지 컬렉션 성능 향상을 위한 전략으로 새로 생성된 객체(long_lived_pending)의 수가 기존의 살아남았던 객체(long_lived_total)의 25%를 기준으로 기준치를 초과했을 때 전체 콜렉션이 실행한다. 

조건문에 만족하는 세대를 collect_with_callback에 인자값으로 전달. 



```C
static Py_ssize_t
collect_with_callback(struct _gc_runtime_state *state, int generation)
{
    assert(!PyErr_Occurred());
    Py_ssize_t result, collected, uncollectable;
    invoke_gc_callback(state, "start", generation, 0, 0);
    result = collect(state, generation, &collected, &uncollectable, 0);			// GC 의 핵심, collect 
    invoke_gc_callback(state, "stop", generation, collected, uncollectable);
    assert(!PyErr_Occurred());
    return result;
}
```



**라이프사이클 4번 과정**

[collect_with_callback](https://github.com/python/cpython/blob/master/Modules/gcmodule.c#L1217) 함수에서 GC의 핵심인 [collect](https://github.com/python/cpython/blob/master/Modules/gcmodule.c#L987)를 호출 해서 Garbage Collection 을 수행한다. 

[collect](https://github.com/python/cpython/blob/master/Modules/gcmodule.c#L987)는 내부에서 콜렉션 대상 이하의 세대 카운트를 초기화하고, 도달 가능(reachable)한 객체와 도달할 수 없는(unreachable) 객체를 분류한다. 

그리고 분류된 도달할 수 없는 객체들을 메모리에서 삭제한다.





자, 이제 우리의 main Topic 으로 돌아가서 어떻게 순환 참조를 감지하는지 알아보자 



#### 순환 참조 감지 



순환 참조를 해결하기 위한 아이디어로 모든 컨테이너 객체를 추적한다. 

***컨테이너 객체란 ? (e.g. `tuple`, `list`, `set`, `dict`, `class`) 이며 컨테이너 객체는 다른 객체에 대한 참조를 보유할 수 있다.***



객체 내부의 링크 필드에 더블 링크드 리스트를 사용하는 방법이 가장 좋다. 
이렇게 하면 추가적인 메모리 할당 없이도 **컨테이너 객체 집합**에서 객체를 빠르게 추가하고 제거할 수 있다. 
컨테이너 객체가 생성될 때 이 집합에 추가되고 제거될 때 집합에서 삭제된다.





```C
// PyGC_Head 에 선언된 더블 링크드 리스트 
typedef struct {
    // Pointer to next object in the list.
    // 0 means the object is not tracked
    uintptr_t _gc_next;

    // Pointer to previous object in the list.
    // Lowest two bits are used for flags documented later.
    uintptr_t _gc_prev;
} PyGC_Head;
```



더블 링크드 리스트를 이용해 모든 객체에 접근할 수 있으니, **순환 참조** 를 찾을 수 있어야 한다. 



> 1. 객체에 `gc_refs` 필드를 레퍼런스 카운트와 같게 설정한다.
> 2. 각 객체에서 참조하고 있는 다른 컨테이너 객체를 찾고, 참조되는 컨테이너의 `gc_refs`를 감소시킨다.
> 3. `gc_refs`가 0이면 그 객체는 컨테이너 집합 내부에서 자기들끼리 참조하고 있다는 뜻이다.
> 4. 그 객체를 unreachable 하다고 표시한 뒤 메모리에서 해제한다.





**드디어!** 가비지 컬렉터가 어떻게 순환 참조 객체를 탐지하고 메모리에서 해제하는지 알았다.

**이어서** GC 의 핵심인 collect 가 어떻게 작동하는지 알아보자. 



#### collect 작동 원리 



위의 순환 참조 4 단계를 조금 더 자세하게 정리한다. 



`collect()` 메서드는 현재 세대와 어린 세대를 합쳐 순환 참조를 검사한다. 
이 합쳐진 세대를 `young`으로 이름 붙이고 다음의 과정을 거치며 최종적으로 도달할 수 없는 객체가 모인 unreachable 리스트를 메모리에서 해제하고 young에 남아있는 객체를 다음 세대에 할당한다.



```c
// 1. 모든 객체의 레퍼런스 카운트 사본을 만든다. 
update_refs(young)	
  

// 2. 각 객체 i 에 대해 i 에 의해 참조되는 객체 j 의 gc_refs 를 감소시킨다. 
// (3번 과정) 즉, 다른 컨테이너 객체에 참조되고 있는 수 A 와 현재 gc_refs B 를 빼서 0 이면 unreachable 로 분류한다. 
subtract_refs(young)
  
// 3. young 세대를 스캔하며 gc_refs 가 0 인 객체를 unreachable 리스트로 이동시키고 
// GC_TENTATIVELY_UNREACHABLE로 설정한다. 
// 임시로(Tentatively) 설정하는 이유는 나중에 스캔 될 객체로부터 도달할 수도 있기 때문이다.
gc_init_list(&unreachable)
move_unreachable(young, &unreachable)
```





##### Reference

- [Python 메모리 관리](https://dc7303.github.io/python/2019/08/06/python-memory/)
- [Python GC가 작동하는 원리](https://winterj.me/python-gc/)


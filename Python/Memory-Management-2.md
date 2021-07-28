### Python 은 메모리 관리를 어떻게 하는가? 

- Reference Counts
- Automatic Garbage Collection 









### Automatic Garbage Collection 



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
> 3.  `generation[0].count > threshold[0]`이면 `genereation[0].count = 0`, `generation[1].count++`이 발생
> 4.  `generation[1].count > 10`일 때 0세대, 1세대 count를 0으로 만들고 `generation[2].count++`을 한다는 뜻



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

조건문에 만족하는 세대를 collect_with_callback에 인자값으로 전달. 



```C
static Py_ssize_t
collect_with_callback(struct _gc_runtime_state *state, int generation)
{
    assert(!PyErr_Occurred());
    Py_ssize_t result, collected, uncollectable;
    invoke_gc_callback(state, "start", generation, 0, 0);
    result = collect(state, generation, &collected, &uncollectable, 0);
    invoke_gc_callback(state, "stop", generation, collected, uncollectable);
    assert(!PyErr_Occurred());
    return result;
}
```



[collect_with_callback](https://github.com/python/cpython/blob/master/Modules/gcmodule.c#L1217) 함수에서 GC의 핵심인 [collect](https://github.com/python/cpython/blob/master/Modules/gcmodule.c#L987)를 호출해서 Garbage Collection 을 수행한다. 

[collect](https://github.com/python/cpython/blob/master/Modules/gcmodule.c#L987)는 내부에서 콜렉션 대상 이하의 세대 카운트를 초기화하고, 도달 가능(reachable)한 객체와 도달할 수 없는(unreachable) 객체를 분류한다. 

그리고 분류된 도달할 수 없는 객체들을 메모리에서 삭제한다.



#### collect 작동 원리 

...







ref. https://blog.winterjung.dev/2018/02/18/python-gc
Ref. https://dc7303.github.io/python/2019/08/06/python-memory/







### 

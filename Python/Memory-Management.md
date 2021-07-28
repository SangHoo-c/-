### Python 은 메모리 관리를 어떻게 하는가? 

- Reference Counts
- Automatic Garbage Collection 



#### Reference Counts 

파이썬은 내부적으로 malloc() / free() 를 많이 사용하기 떄문에 메모리 누수의 위험이 있다. 
=> Reference Counts 전략 사용 



**작동방식** 

- Python 의 모든 객체에  Count 를 포함하고, 이 Count 는 객체가 참조될 때 증가 / 참조가 삭제될 때 감소 한다. 
- Count 가 0 이 되면 메모리 할당이 해제 

```python
import sys

class RefTest():
    def __init__(self):
        print('create object')


a = RefTest()
print(f'count {sys.getrefcount(a)}')
b = a
print(f'count {sys.getrefcount(a)}')
c = a
print(f'count {sys.getrefcount(a)}')
c = 0
print(f'count {sys.getrefcount(a)}')
b = 0
print(f'count {sys.getrefcount(a)}')

"""
create object
count 2
count 3
count 4
count 3
count 2
"""
```

([getrefcount](https://docs.python.org/3/library/sys.html#sys.getrefcount)) : getrefcount 의 인자값으로 임시 참조되기 때문에  초기 reference count 가 2 가 된다. 

증가 / 감소 는 파이썬에서 Py_INCREF와 Py_DECREF을 통해 구현되어 있습니다. [*(Python Doc)*](https://docs.python.org/ko/3.6/c-api/refcounting.html#c.Py_XINCREF)



```C
/* 파이썬의 객체 형태 */
typedef struct _object {
    _PyObject_HEAD_EXTRA
    Py_ssize_t ob_refcnt;          /* 레퍼런스 카운트 */
    struct _typeobject *ob_type;
} PyObject;



/* ob_refcnt를 증가시킨다. */
static inline void _Py_INCREF(PyObject *op)
{
    _Py_INC_REFTOTAL;
    op->ob_refcnt++;
}

/* ob_refcnt 0일때 _Py Dealloc(op)을 사용하여 메모리 할당을 제거한다. */
static inline void _Py_DECREF(const char *filename, int lineno, PyObject *op)
{
    (void)filename; /* may be unused, shut up -Wunused-parameter */
    (void)lineno; /* may be unused, shut up -Wunused-parameter */
    _Py_DEC_REFTOTAL;
    if (--op->ob_refcnt != 0) {
#ifdef Py_REF_DEBUG
        if (op->ob_refcnt < 0) {
            _Py_NegativeRefcount(filename, lineno, op);
        }
#endif
    }
    else {
        _Py_Dealloc(op);	/* ob_refcnt 가 0 일때 메모리 할당을 해제한다.*/
    }
}
```

ref,  [cpython/object.h](https://github.com/python/cpython/blob/master/Include/object.h) 



**핵심동작**

1. PyObject (파이썬 객체)에서 ob_refcnt 를 갖고 있다. 
2. _Py_INCREF 에서 증가시킨다. 
3. _Py_DECREF 에서 감소시킨 후, ob_refcnt 가 0 일 때 메모리 할당을 삭제한다. 



> 효율적으로 동작하지만, ref-count 만으로 메모리를 관리했을 때, 허점이 있다. 





#### reference count 의 약점! 순환 참조 



> 순환 참조란 : 컨테이너 객체가 자기 자신을 참조하는 것을 말한다. 
>
> 자기 자신을 참조할 때 레퍼런스 카운트가 0에 도달할 수 없고 할당된 메모리를 삭제할 수 없어 ***메모리 누수*** 가 발생한다.



```python
class RefTest():
    def __init__(self):
        print('create object')

    def __del__(self):
        print(f'destroy {id(self)}')


a = RefTest()
a = 0
print('end .....')

"""
create object
destroy 4528171712
end .....
"""
```



_ _ del _ _ () 은 메모리 할당이 삭제되는 시점에 실행되는 메소드 이다. 

a 에 0 을 재할당할 때, _ _ del _ _ () 이 실행되고 마무리 된다. 하지만, 만약 **순환참조**가 있다면 ?



```python
# me 프로퍼티에 자기 자신을 할당합니다.
class RefTest():
    def __init__(self):
        print('create object')
        self.me = self

    def __del__(self):
        print(f'destroy {id(self)}')


a = RefTest()
a = 0
print('end .....')

"""
create object
end .....
destroy 2110595412432
"""

```



a 변수에 0 을 재할당해도 a.me 에 자기 자신을 참조하고 있기때문에 reference count 가 남아서 발생하는 현상이다. 

이렇게 되면 reference count 는 0 에 도달할 수 없어 할당된 메모리를 삭제할 수 없기에 ***메모리 누수*** 가 발생한다. 

> Python 은 이 문제를 가비지 콜렉션으로 해결한다. 


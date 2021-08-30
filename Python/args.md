### *args, **kargs

`python` 의 함수에 대해 공부하다보면 `*args`, `**kwargs` 라는 내용을 볼 수 있다.

해당내용을 간단하게 정리해보자. 



#### *args

`*args` 는 `*arguments`의 줄임말이다.
줄임말일 뿐 꼭 저 단어를 사용할 필요는 없다. 얼마든지 원하는 단어를 사용해도 좋다.

`*a` 라고 적든 `*qiwefkjqwf`로 적든 상관이 없다. 앞에 `*`만 잘 붙여주자.



>이 지시어는 **여러개의 매개변수**를 함수로 받고싶을 때 사용하는 지시어다.



```python
def name(a1, *args):
    print(a1, args)
    
name("a", "b", "c", "d")
---output---
# tuple 형태로 출력 
a ('b', 'c', 'd')
```

함수 `name`의 매개변수로 `*args` 가 있다는 뜻은 이 함수는 여러개의 매개변수를 받을 수 있다는 의미이다.
그래서 함수를 호출했을 때 인자값으로 몇개의 이름을 주어도 출력이 잘 되는 것을 확인할 수 있다.

결과값은 `tuple` 형태로 출력된다. 

- 수정 x 
- indexing 가능 



#### *kwargs



`**kwargs`는`keyword argument`의 줄임말로 키워드를 제공한다.

> `**kwargs`는 (키워드 = 특정 값) 형태로 함수를 호출할 수 있다.

```python
def name2(**kwargs):
    print(kwargs)


name2(name1="a1", name2="a2")

---output---
# dictionary 형태로 출력 
{'name1': 'a1', 'name2': 'a2'}	
```






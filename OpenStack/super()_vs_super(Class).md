## super() vs super(Class) 

```python
# osc_lib/shell.py
def run(self, argv):
        ret_val = 1
        self.command_options = argv
        try:
            ret_val = super(OpenStackShell, self).run(argv)
            return ret_val
        except Exception as e:
```



opnestack CLI 코드 분석 도중, `super([모델명], self).run(argv)`이라는 코드를 보고 의문이 생겼다. 
`super`가 그 전에는 부모 클래스를 상속받는 것이라고 생각했었는데, 왜 그 인자로 자기 자신의 모델명을 넣는 것인가.



이 [stackoverflow 글](https://stackoverflow.com/questions/14743787/python-superclass-self-method-vs-superparent-self-method)을 통해 이해한 바로는 다음과 같다.

- `super`는 하위 클래스의 이름과 하위 클래스의 object를 파라미터로 받는다.
- 인자로 하위 클래스의 이름을 명시하면, 그 부모로부터 탐색한다.

다음과 같이 코드를 실행해서 `super`가 반환하는 것이 무엇인지 정확히 알아 보자.

```python
class A(object):
    def do_work(self):
        print('A의 do_work')

class B(A):
    def do_work(self):
        print('B의 do_work')
        super(B, self).do_work()

class C(B):
    def do_work(self):
        print(1)
        super(C, self).do_work()
        print(2)
        super(B, self).do_work()
        print(3)
        super(A, self).do_work

# C 인스턴스 생성
c = C()
c.do_work()


---output---
1
B의 do_work
A의 do_work
2
A의 do_work
3
AttributeError: 'super' object has no attribute 'do_work'
```

즉, super는 인자로 받은 클래스의 **부모 클래스**의 object들을 가져 온다. 
리턴된 자기 자신 클래스의 object에서는 인자로 넘긴 클래스의 부모가 갖는 메소드들을 사용할 수 있는 것이다.



리턴하는 것은 `B` 클래스의 인스턴스이지만, 그 인스턴스가 가져 오는 object들은 인자로 받은 클래스의 부모 클래스가 갖는 것들이다. 
위의 코드에서 마지막에 에러가 난 원인은, A의 부모 클래스가 없기 때문이다.



> 그렇다면 `super`에 인자를 넘기지 않는다면 어떻게 될까?



```python
class A(object):
    def do_work(self):
        print('A의 do_work')

class B(A):
    def do_work(self):
        print('B의 do_work')
        super().do_work()

class C(B):
    def do_work(self):
        print(1)
        super().do_work()
        print(2)
        super().do_work()
        print(3)
        super().do_work

# C 인스턴스 생성
c = C()
c.do_work()

---output---
1
B의 do_work
A의 do_work
2
B의 do_work
A의 do_work
3
B의 do_work
A의 do_work
```

결론적으로 인자를 넘기지 않으면 `super`의 할머니까지 탐색해서 object를 가져 오고, 

인자를 넘기면 인자로 명시된 클래스의 부모에서 object를 탐색해서 가져 온다.



#### 결론

```python
# osc_lib/shell.py

class OpenStackShell(app.App):
  
def run(self, argv):
        ret_val = 1
        self.command_options = argv
        try:
            ret_val = super(OpenStackShell, self).run(argv)
            return ret_val
        except Exception as e:
```



OpenStack CLI 에서 orc_lib/shell.py 는 app.App Class 의 run 메서드를 상속받는 것이다. 

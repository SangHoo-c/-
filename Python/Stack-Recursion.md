### 스택과 재귀 (Python)

1. 함수의 호출 과정 
2. 스택 

----



#### 함수의 호출 과정 

```python
def mul(a, b):
    c = a * b
    return c
 
def add(a, b):
    c = a + b
    print(c)
    d = mul(a, b)
    print(d)
 
x = 10
y = 20
add(x, y)
```



- **함수 호출 스택** : (전역 프레임 - add 프레임 - mul 프레임 순으로 쌓아진다. )
- 전역 프레임에 위에서 부터 순서대로 넣는다. 
- add 의 스택 프레임에 매개변수 a, b 를 넣고, c 를 이어서 넣는다. 
- add 에서 mul 을 호출하면 mul 의 스택프레임이 만들어지고, a, b 이후 c 를 넣는다. return c 를 하게 되면 mul 의 스택프레임은 사라진다. 
- add 의 스택 프레임에 반환 받은 d 가 추가된다. 
- Print(d) 까지 함수 add 가 끝나면 add 의 스택프레임도 사라진다. 





#### 번외. factorial 여러 구현법 

```python
# stack : factorial 구현 
def fac(n):
    stack = []
    while n > 0:
        stack.append(n)
        n -= 1
    result = 1
    while stack:
        result *= stack.pop()
    return result


print(fac(4))
```



```python 
# 재귀 :  factorial 구현
def fac(n):
    result = 1
    while n > 0:
        result *= n
        n -= 1
    return result


print(fac(4))
```



```python
# DP & 재귀 : factorial 구현
dp = [0] * 1000
dp[0] = 1
dp[1] = 1


def fac(n):
    if dp[n]:
        return dp[n]
    dp[n] = fac(n-1) * n
    return dp[n]


print(fac(4))
```






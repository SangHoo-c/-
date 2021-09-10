## Hashing 



```
name | number
'kim' : 010-1234-1234
'lee' : 010-1234-1234
'park' : 010-1234-1234
```

과 같은 정보가 있을때, 빠르게 검색하기 위한 방법은 이분탐색이 있을 것이다. 하지만, 새로운 정보가 들어온 경우, 전체 정보를 다시 정렬해줘야하는 불편함이 있을것이다. 확률적으로 절반의 순서를 옮겨야한다. 만약 순서대로 저장하지 않는다면, 검색에 시간이 오래걸릴것이다. 

이런 불편함을 개선하기 위해서 'Hashing' 이 나왔다. 

**넓은 공간을** 사용해 **데이터의 저장과 검색**시 시간을 줄이는 것에 초점을 둔다. 

![img](https://upload.wikimedia.org/wikipedia/commons/thumb/5/58/Hash_table_4_1_1_0_0_1_0_LL.svg/240px-Hash_table_4_1_1_0_0_1_0_LL.svg.png)



해쉬 테이블은 키와 밸류를 기반으로 데이터를 저장한다. 

데이터는 **해쉬 함수**를 거쳐 숫자로 변경된다. 변경된 이 값(해시)를 배열(buckets)의 **키**로 삼아 value 를 저장한다. 따라서 데이터를 서칭하는 과정에서 배열을 순차적으로 탐색할 필요없이 해시 함수를 거쳐 생성된 해시 값으로 데이터를 빠르게 찾을 수 있다. 딕셔너리의 key-value 구조와 유사하다.

> Hash : 인풋 데이터를 고정된 길이의 숫자열로 변환한 결과
> Hash Function : **'키'** 를 해쉬로 변경해주는 함수 

문자열로 들어온 인풋 데이터를 해시 함수를 통해 숫자열로 변경해주고, 이 숫자를 키 값 삼아서 배열(buckets)에 값을 저장하는 구조다. 
(파이썬 해쉬 함수의 경우 환경마다 아웃풋이 달라서 hashlib의 sha1, sha256을 쓰기도 함)



**하지만,**

아무리 해쉬 함수를 잘 짜도 충돌은 피할 수 없다.
따라서 **충돌처리**를 해줘야한다. 

- 오픈해싱 (Open Hashing)
- 클로즈해싱 (Close Hashing)



#### 오픈해싱 

오픈 해싱은 해시 테이블의 충돌 문제를 해결하는 대표적인 방법중 하나로 체이닝(Separate Chaining) 기법이라고도 한다. 
만약 해시 값이 중복되는 경우, 먼저 저장된 데이터에 링크드 리스트를 이용하여 중복 해시 데이터를 연결한다.

#### 클로즈해싱

해시 중복이 발생하면 해당 해시 값부터 순차적으로 빈 공간을 찾기 시작한다. 가장 처음 찾는 빈 공간에 키와 밸류를 저장한다. 


```python

hash_table = [[] for _ in range(10)]
hash_table_2 = [[] for _ in range(10)]


def hash_func(a):
    return a % 8


# index , value
data = [
    [11, "lee"],
    [4, "park"],
    [1, "jung"],
    [1, "chu"]
]

# chaining 기법
for d in data:
    hash_table[hash_func(d[0])].append(d[1])  # collision !

# Linear probing 기법
for d in data:
    key = hash_func(d[0])
    init_key = key
    while hash_table_2[key]:
        key = (key + 1 + 8) % 8
        if key == init_key:
            break
    hash_table_2[key].append(d[1])


print(hash_table)
print(hash_table_2)


# To-Do

## 목적 

- series 와 DataFrame 초기화 과정 코드 분석 

```python
# 분석한 코드 
# pandas/_typing.py
# pandas/_config/config.py
# pandas/core/frame.py
# pandas/core/internal/construction.py
```

## 방법 

- DataFrame 이 선언될때, 실행되는 코드를 찾아보자.
- create_DataFrame_by_Series_test.py 를 실행시키고, tracing 해보았다.

<br/>
<br/>

## 목차 

1. series 구현체
2. **__init__** Series & DataFrame. ( 1-a ) 
3. Manager  ( 1-b, 1-c )

<br/>
<br/>


## series 구현체


```python
print(type(test))
for t in test:
    print(type(t))

"""
<class 'pandas.core.series.Series'>
<class 'int'>
<class 'int'>
<class 'int'>
"""
```

- pandas.core.series.Series 는 for 문으로 조회 가능
    - iterable? iterator?
    - pandas series 구현체 확인
    

<br/>

## 1. __init__ Series & DataFrame

```python
dfs = pd.Series(
    {"A": [4, 5, 6, 7], }
)
dfs2 = pd.Series([4, 5, 6, 7])

... 

print(pd.DataFrame(dfs).head())
"""
0
A  [4, 5, 6, 7]
"""

print(pd.DataFrame(dfs2).head())
"""
   0
0  4
1  5
2  6
3  7
"""

```

- dict 타입으로 DF 를 생성하면, 하나의 열에 모두 포함된다.
- but, index (key) 를 주지 않는다면, 여러개의 열을 거쳐서 생성된다.

> **series 는 행 우선인가? 열 우선인가?**
> 

**A. 행 우선 이다.**  (이는 Series 정의에서 알 수 있다.) 

 `Series` is a 1-dimensional (1D) column of data with index labels. `Series` is value-mutable but size-immutable.

`DataFrame` is a 2-dimensional (2D) table of data with index labels and column labels. Each column in a `DataFrame` is a `Series`. 

[pandas column](https://stackoverflow.com/a/27375049) 

Internally, the values are stored as blocks of the same type. Each column, or collection of columns of the same type is stored in a separate array.

And this indeed implies that appending a row is more expensive. In general, appending multiple single rows is not a good idea: better to eg preallocate an empty dataframe to fill, or put the new rows/columns in a list and concat them all at once.

<br/>

- DataFrame 이 선언될때, 실행되는 코드를 찾아보자.
- create_DataFrame_by_Series_test.py 를 실행시키고, tracing 해보았다. 


<br/>
<br/>

```python
# create_DataFrame_by_Series_test.py
series_1 = pd.Series(
    {0: "Kim", 1: "David", 2: "Lee", "3": "Jym"}
)
series_2 = pd.Series(["ABC", "who is he?", "bye bye", "hello world! "])

frame = {'Author': series_1, 'Article': series_2}

pdf = pd.DataFrame(frame)
print(pdf.head())

"""
Author        Article
0    Kim            ABC
1  David     who is he?
2    Lee        bye bye
3    NaN  hello world!    # default index 는 정수값이기 때문. 
3    Jym            NaN
"""
```

- pandas dataframe 을 만들기 위해선 여러 방식으로 선언이 가능하다. 
[pandas dataframe 선언](https://pandas.pydata.org/docs/reference/api/pandas.DataFrame.html)
- create_DataFrame_by_Series_test.py 에서 선언된 것 처럼, series 를 이용한 Dataframe 선언하는 방식을 알아보고자 한다.
- 전체 적인 흐름은 이와 같다.
    - DataFrame 호출
    - dict_to_mgr()
    - mgr_to_mgr(mgr, typ=manager)
    - NDFrame.__init__(self, mgr)

<br/>

```python
print(pd.__version__)
# 1.4.1
```

- 해당 코드는 pandas 1.4.1 기준이다.

<br/>

```python
# pandas/__init__.py
from pandas.core.api import (
	...
	DataFrame,
)

__all__ = [
	...
	"DataFrame",
]
```

- 특정 디렉토리의 모듈을 `*`을 이용하여 import할 때에는 다음과 같이 해당 디렉터리의 `__init__.py` 파일에 `__all__`이라는 변수를 설정하고 import 할 수 있는 모듈을 정의해 주어야 한다.
- `__all__`로 정의하지 않으면 인식되지 않는다.

<br/>

### **main 코드**

```python
# pandas/core/frame.py
class DataFrame(NDFrame, OpsMixin):
	def __init__(
        self,
        data=None,
        index: Axes | None = None,
        columns: Axes | None = None,
        dtype: Dtype | None = None,
        copy: bool | None = None,
    ):
			manager = get_option("mode.data_manager") # 1-a. 
			... 
			elif isinstance(data, dict):
            mgr = dict_to_mgr
			... 
			# ensure correct Manager type according to settings
      mgr = mgr_to_mgr(mgr, typ=manager) # 1-b
      NDFrame.__init__(self, mgr) # 1-c 
```

- 반환된 `Manager` 객체 (mgr) 를 `mgr_to_mgr` 함수에 넣고 mgr 재선언
    - `Manager` 객체란?
    - `get_option` 함수
- mgr 을 이용한 `NDFrame` 초기화
    - `NDFrame` 이란?

<br/>

### 1-a. Manager

```python
# pandas/_typing.py
# internals
Manager = Union[
    "ArrayManager", "SingleArrayManager", "BlockManager", "SingleBlockManager"
]

Union = _SpecialForm('Union', doc=
    """Union type; Union[X, Y] means either X or Y.
		...
		""")
```

- 어떤 Manager 를 쓰는지는 확실하지 않지만,
- 자료가 있는 `BlockManager` 에 대해서 알아보도록 하자.

> **BlockManger 란?**
> 
- pandas core architectural component.
- has an impact on performance.

[[well-explained] block manager & block consolidation 에 관하여](https://uwekorn.com/2020/05/24/the-one-pandas-internal.html) 

- 완벽하게 이해하지 못했다. 추가적인 학습 필요
- pandas core 에 구현되어 있고, block consolidation 을 통해서 성능상 이점을 가져간다고 이해했다.

<br/>
<br/>

### NDFrame

- a DataFrame is a subclass (i.e. special case) of `NDFrame`
- N-dimensional analogue of DataFrame.
- Store multi-dimensional in a size-mutable, labeled data structure

[NDFrame, 즉 다차원의 dataframe, 잘 안쓴다.](https://stackoverflow.com/a/43002466)

자, 이제 함수 하나씩 분석해보자. 

<br/>

```python
# pandas/_config/config.py
# pat = "mode.data_manager"
def _get_option(pat: str, silent: bool = False):
    key = _get_single_key(pat, silent)  # 1-a-가

    # walk the nested dict
    root, k = _get_root(key)
    return root[k]

...
_get_option_tmpl = """
get_option(pat)

Retrieves the value of the specified option.
"""
... 

get_option = CallableDynamicDoc(_get_option, _get_option_tmpl)
```

- `CallableDynamicDoc` 가 뭘까?
    
    
<br/>    

```python
# pandas/_config/config.py
class CallableDynamicDoc:
    def __init__(self, func, doc_tmpl):
        self.__doc_tmpl__ = doc_tmpl
        self.__func__ = func
```

- 함수와 template (str) 를 인자로 받는다.
- 사용자의 편의성을 위해 정의된 부분 (뇌피셜)
    - 결국, `_get_option` 수행한다.

다시, get_option 으로 돌아오자. 

<br/>

```python
# 1-a-가
# pandas/_config/config.py
def _get_single_key(pat: str, silent: bool) -> str:
    keys = _select_options(pat) # 1-a-나 
    if len(keys) == 0:
        if not silent:
            _warn_if_deprecated(pat)
        raise OptionError(f"No such keys(s): {repr(pat)}")
    if len(keys) > 1:
        raise OptionError("Pattern matched multiple keys")
    key = keys[0]

    if not silent:
        _warn_if_deprecated(key)

    key = _translate_key(key) # 1-a-다 

    return key
```

- ex) `# pat : "mode.data_manager"` 와 같이 전달받은 pattern 을 인자로
- select option 함수를 호출
- `key = keys[0]` 여러개 key 중에 하나만 고른다.
- 여러개의 key 가 있다면, OptionError 발생시킴.
- `_translate_key(key)` 호출

select 와 translate 함수를 보고가자. 

<br/>

```python
# pandas/_config/config.py
# 1-a-나
def _select_options(pat: str) -> list[str]:
    """
    returns a list of keys matching `pat`
    if pat=="all", returns all registered options
    """
```

- pat 에 일치하는 keys 를 반환.

<br/>

```python
# pandas/_config/config.py
# 1-a-다 
def _translate_key(key: str) -> str:
    """
    if key id deprecated and a replacement key defined, will return the
    replacement key, otherwise returns `key` as - is
    """
    d = _get_deprecated_option(key)
    if d:
        return d.rkey or key
    else:
        return key
```

- deprecated 되지 않고, 남아있는 key 반환하기 위한 함수

<br/>

결론 

```python
manager = get_option("mode.data_manager") # 1-a. 
```

- manager 를 선언한다.

<br/>

### 1-b.

```python
# pandas/core/frame.py
elif isinstance(data, dict):
    # GH#38939 de facto copy defaults to False only in non-dict cases
    mgr = dict_to_mgr(data, index, columns, dtype=dtype, copy=copy, typ=manager)
```

- `isinstance` 내장함수를 통해 `dict`타입인지 체크
- dict 타입인 경우, `dict_to_mgr` 호출

<br/>

```python
# pandas/core/internal/construction.py
def dict_to_mgr(
	data: dict,
    index,
    ...
    typ: str = "block",
    copy: bool = True,
) -> Manager:
...
return arrays_to_mgr(arrays, columns, index, dtype=dtype, typ=typ, consolidate=copy)
```

- `dict_to_mgr` 구현부
- `arrays_to_mgr` 함수 반환

<br/>

```python
# pandas/core/internal/construction.py
def arrays_to_mgr(
    arrays,
    columns: Index,
    ...
) -> Manager:

	...
	# from BlockManager perspective
  axes = [columns, index]

  if typ == "block":
      return create_block_manager_from_column_arrays(
          arrays, axes, consolidate=consolidate
      )
  elif typ == "array":
      return ArrayManager(arrays, [index, columns])
  else:
      raise ValueError(f"'typ' needs to be one of {{'block', 'array'}}, got '{typ}'")
```

- `typ` 에 따라서 입력받은 `array` 에 맞는 `manager` 생성
- 여기가 핵심, 여기를 통해 Manager 구현부 확인 가능 ⇒ 추후에 다시 학습
    - `create_block_manager_from_column_arrays`
    - `ArrayManager`

<br/>

결론

```python
# pandas/core/frame.py
elif isinstance(data, dict):
    # GH#38939 de facto copy defaults to False only in non-dict cases
    mgr = dict_to_mgr(data, index, columns, dtype=dtype, copy=copy, typ=manager)
```

- dict 타입 데이터를 Manager 로 변경

<br/>

### 1-c.

```python
# pandas/core/frame.py
NDFrame.__init__(self, mgr) # 1-c 
```

- Manager 로 DF 초기화 한다.

<br/>

```python
# pandas/core/frame.py
class DataFrame(NDFrame, OpsMixin):
			... 
			# ensure correct Manager type according to settings
      mgr = mgr_to_mgr(mgr, typ=manager)  # 1-c-가 
      NDFrame.__init__(self, mgr)
```

- manager 체크 후, init

<br/>

```python
# pandas/core/internals/construction.py
# 1-c-가 
def mgr_to_mgr(mgr, typ: str, copy: bool = True):
		new_mgr: Manager
    if typ == "block":
				... 
    elif typ == "array":
				... 
    return new_mgr
```

- Convert to specific type of Manager.

<br/>
<br/>

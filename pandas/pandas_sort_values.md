# sort_values

`sort_values` 구현방식을 알아보자. 

```python
# pandas/core/frame.py
from pandas.util._decorators import (
    Appender,
    Substitution,
    deprecate_nonkeyword_arguments,
		...
)
...

class DataFrame(NDFrame, OpsMixin):
		...
		# sorting 
    @deprecate_nonkeyword_arguments(version=None, allowed_args=["self", "by"])
    @Substitution(**_shared_doc_kwargs)
    @Appender(NDFrame.sort_values.__doc__)
    def sort_values(  # type: ignore[override]
        self,
        by,
        axis: Axis = 0,
        ascending=True,
        inplace: bool = False,
        kind: str = "quicksort",
        na_position: str = "last",
        ignore_index: bool = False,
        key: ValueKeyFunc = None,
    ):
```

- `pandas.util._decorators` 에서 import 된 decorator
- 기본 sorting mechanism 은 quick sort
- `self` : sort 를 할 대상, 여긴 Dataframe class 이니까 Dataframe 이겠지.
- `by` : sort 의 기준이 되는 값
    - by = {str} 'Starting Median Salary’

<br/>

## 로직 1.

```python
inplace = validate_bool_kwarg(inplace, "inplace")  # inplace = {bool} False
axis = self._get_axis_number(axis)
ascending = validate_ascending(ascending)
```

- 한개씩 살펴보자.

<br/>

### 1-a

```python
# pandas/util/_validator.py
def validate_bool_kwarg(
	value, arg_name, none_allowed=True, int_allowed=False):

```

- *Ensure that argument passed in arg_name can be interpreted as boolean.*
- return *The same value as input.*

<br/>

### 1-b

```python
# pandas/core/generic.py
@final
@classmethod
def _get_axis_number(cls, axis: Axis) -> int:
    try:
        return cls._AXIS_TO_AXIS_NUMBER[axis]
    except KeyError:
        raise ValueError(f"No axis named {axis} for object type {cls.__name__}")
```

- decorator : `final`, `classmethod`
- `cls = {type} <class 'pandas.core.frame.DataFrame'>`
- axis = 0 이란? 열 단위 적용 ... 말이 어렵다. 아래 설명 참고.

<br/>

> `axis` 매개 변수란? 
특정 메소드 또는 함수가 DataFrame에 적용되는 방향을 지정
> 

[사용예시](https://www.delftstack.com/ko/howto/python-pandas/pandas-axis-meaning/) 

- `axis = 0` 함수가 DataFrame에서 열 단위로 적용
- `axis = 1` 함수가 DataFrame에서 행 단위로 적용

<br/>

### 1-c

```python
# pandas/util/_validators.py
def validate_ascending(
    ascending: bool | int | Sequence[bool | int] = True,
):
    """Validate ``ascending`` kwargs for ``sort_index`` method."""
    kwargs = {"none_allowed": False, "int_allowed": True}
    if not isinstance(ascending, (list, tuple)):
        return validate_bool_kwarg(ascending, "ascending", **kwargs)

    return [validate_bool_kwarg(item, "ascending", **kwargs) for item in ascending]
```

- `validate_bool_kwarg` 호출
    - 1-a 의 함수와 같다.
- `ascending = validate_ascending(ascending)` 을 통해, boolean 값이 맞게 들어왔는지 체크

<br/>

# 2 편에서 이어집니다. 

## 로직 2

```python
# pandas/core/frame.py
if len(by) > 1:
	keys = [self._get_label_or_level_values(x, axis=axis) for x in by]
	...
	indexer = lexsort_indexer(
		keys, orders=ascending, na_position=na_position, key=key
	)

elif len(by):
	k = self._get_label_or_level_values(by, axis=axis)
	indexer = nargsort(
	  k, kind=kind, ascending=ascending, na_position=na_position, key=key
	)
```

- sort 의 기준이 되는 `column` 의 개수에 따라 다르다.
- `_get_label_or_level_values` 는 공통적으로 포함된다.

<br/>

### 2-a : key 여러개

```python
# pandas/core/sorting.py
def lexsort_indexer(
    keys, orders=None, na_position: str = "last", key: Callable | None = None
) -> npt.NDArray[np.intp]:

		...
		keys = [ensure_key_mapped(k, key) for k in keys] # check resulting value has the same shape
		...
		
		for k, order in zip(keys, orders):
        with warnings.catch_warnings():
						...
            cat = Categorical(k, ordered=True)  # ndarray -> Categorical
```

- lexical sort 수행

> what is lexical sort & how it implement?
> 

 `... to be continued ...`

<br/>

### 2-b : key 한개

```python
# pandas/core/sorting.py
def nargsort(
    items,
    kind: str = "quicksort",
    ascending: bool = True,
    na_position: str = "last",
    key: Callable | None = None,
    mask: np.ndarray | None = None,
) -> npt.NDArray[np.intp]:
```

- nargosrt 수행

> also... what is nargsosrt & how it implement?
> 

 `... to be continued 2...`


<br/>


### 공통 내용

```python
# pandas/core/generic.py
@final
def _get_label_or_level_values(self, key: str, axis: int = 0) -> np.ndarray:
	...
	if self._is_label_reference(key, axis=axis):
            self._check_label_or_level_ambiguity(key, axis=axis)
            values = self.xs(key, axis=other_axes[0])._values
```

- Return a 1-D array of values associated with `key` (a label or level from the given `axis` )
- `_check_label_or_level_ambiguity` : Check whether `key` is ambiguous.
    - 즉, 정렬의 기준이 되는 key 값의 자격을 검증
    

```python
# pandas/core/generic.py
@final
def _check_label_or_level_ambiguity(self, key, axis: int = 0) -> None:
	if (
        key is not None
        and is_hashable(key) # hash 적용 체크 
        and key in self.axes[axis].names # 
        and any(key in self.axes[ax] for ax in other_axes) # 다른 column 에 겹치는게 있는지
    ):
```

- 만약  해당 조건에 걸린다면, *`ValueError` 발생.*

<br/> 
 

### 마무리

```python
# pandas/core/frame.py
# self = {DataFrmae} 
@property
def _constructor(self) -> type[DataFrame]:
		return DataFrame
...

new_data = self._mgr.take(
            indexer, axis=self._get_block_manager_axis(axis), verify=False
        )
...
result = self._constructor(new_data) 
... 

if inplace:
	      return self._update_inplace(result)
    else:
        return result.__finalize__(self, method="sort_values")

```

- 각 기준에 맞게 정렬된 이후, *`BlockManager` 를 반환 받는다.*
    - `pandas/core/internals/manager.py`
    - def take(self: T ... ) → T
- new_data  = {BlockManager}
- `_constructor` 를 통해 DataFrame 을 생성한다.

 
 
<br/>

<br/>

- **번외 (*args, **kwargs)**

```python
def test(*args, **kwargs):
    print(type(args), type(kwargs))
    print(args)

    for k, v in kwargs.items():
        print(k, v)

test("abcd", "efgs", myname="hoo", age="27")

"""
<class 'tuple'> <class 'dict'>
('abcd', 'efgs')
myname hoo
age 27
"""
```

- args : tuple 반환
- kwargs : dict 반환

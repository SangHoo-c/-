# BlockManager

*The BlockManager keeps columns of the exact same dtype in a single continuous memory region.*

https://uwekorn.com/images/pd-df-perception.002.png

- data stored in blocks as managed by the `BlockManager`
- columns are blocked by their data type
- block also needs to store the position of its columns in the DataFrame

<br/>

```python
import pandas as pd
import numpy as np

df = pd.DataFrame({
    'int64_1': np.array([1, 2, 3], dtype=np.int64),
    'int64_2': np.array([10, 20, 30], dtype=np.int64),
    'int32_1': np.array([9, 8, 7], dtype=np.int32),
    'float64_1': np.array([.1, .5, .7], dtype=np.float64)
})

# <class 'pandas.core.series.Series'>
print(type(df['int64_1']))

# <class 'numpy.ndarray'>
print(type(df['int64_1'].values))

# [[ 1  2  3]
#  [10 20 30]]
print(df['int64_1'].values.base)

# check how dataframe is stored. 
# internal storage object
print(df._data)

"""
BlockManager
Items: Index(['int64_1', 'int64_2', 'int32_1', 'float64_1'], dtype='object')
Axis 1: RangeIndex(start=0, stop=3, step=1)
NumericBlock: slice(0, 2, 1), 2 x 3, dtype: int64
NumericBlock: slice(2, 3, 1), 1 x 3, dtype: int32
NumericBlock: slice(3, 4, 1), 1 x 3, dtype: float64
"""

```

- BlockManager 가 어떻게 구성되어 있는지 알 수 있다.
    - `int64` 로 2 x 3 배열이 생성되어 있다.

 
<br/>



## **BlockManager 의 장단점**

**장점** 

- columnar store
    - significant performance boosts for `column-oriented operations` on many different columns
- column-oriented operation 에 성능향상
    - vector-like operations (summing two columns together)

  
<br/>
  
**단점** 

- Code complexity
- Loss of user visibility to memory use
    - memory-doubling effect that can lead to memory error
    - [apache arrow 사용장점](https://wesmckinney.com/blog/apache-arrow-pandas-internals/)
- data copying
    - If you add a column of the same type though and `consolidation` is triggered, a new block of the size of `n + 1` is created.
- Unavoidable `consolidation`
    - DataFrame 이 점진적으로 커지는 경우, 매번 `consolidation` 이 호출되진 않지만, 특정 시점에 꽤 많은 자원을 소모하면서 (시간이 오래걸림) `consolidation` 수행
    - block `consolidation` only happens on operations that benefit from it.
    - As the `BlockManager` is an internal implementation detail, it is not visible from the outside which operations trigger the `consolidation`

    
<br/>
    
```python
# consolidation 테스트 
import pandas as pd
import numpy as np

df = pd.DataFrame({
    'int64': np.arange(1024 * 1024, dtype=np.int64),
    'float64': np.arange(1024 * 1024, dtype=np.float64),
})
for i in range(97):
    df[f'new_{i}'] = df['int64'].values
```

- 목적 : consolidation test 하기
- 100 개가 trigger 기준 (?)
    - 출처 : [when does consolidation happen?](https://uwekorn.com/2020/05/24/the-one-pandas-internal.html)

<br/>    

```python
# df._data.nblocks = 99
%time df['c'] = df['int64'].values
# CPU times: user 6.46 ms, sys: 6.22 ms, total: 12.7 ms
# Wall time: 9.54 ms
```

- column 하나 더 추가

<br/>

```python
# df._data.nblocks = 100 
%time df['d'] = df['int64'].values
# CPU times: user 6.42 ms, sys: 5.93 ms, total: 12.4 ms
# Wall time: 9.36 ms
```

- warning 발생
- `df._data.nblocks` 이 100 을 넘어가는 순간부터 해당 경고문 출력됨.

> `timed exec>:1: PerformanceWarning: DataFrame is highly fragmented.  This is usually the result of calling `frame.insert` many times, which has poor performance.  Consider joining all columns at once using pd.concat(axis=1) instead.  To get a de-fragmented frame, use `newframe = frame.copy()``
> 
- 구글링을 해보니, 비슷한 이슈는 대량의 append 시에 발생 했다.
- 대부분의 솔루션은 append 대신 concat 을 사용하라였다.
- 그렇다면, append vs concat 비교를 통해 consolidation 여부의 차이를 알 수 있을까?

<br/>

**다음 주제**

- append 와 concat 내부 동작 원리 파악하기
- consolidation 이 자동으로 수행되는지 여부 파악

ref.

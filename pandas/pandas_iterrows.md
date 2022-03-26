# pandas iterrow

```python
import pandas as pd

dic = {
    'students': ['Hoo', 'Juri', 'David'],
    'scores': ['A', 'A+', 'B+']
}

pdf = pd.DataFrame(dic)
for index, value in pdf.iterrows():
    # print(type(value)) # <class 'pandas.core.series.Series'>
		if value.scores == 'A': 
        print(value.students) 

# output 
Hoo 
```

- pandas 는 행 기반 자료구조
- 행으로 되어 있는 Series 들이 모여서 하나의 Dataframe 을 이룬다고 생각했었다.
- **But,** iterrows 메서드를 사용해서 하나의 열을 확인해보니, value 값이 Series 를 가진다.
- 각 row 를 나타내는 Series 인 value 는 index 로 ‘students’, ‘scores’ 를 갖는다.
- 이를 통해 조건문도 적용 가능하다.

> iterrows 를 하게 되면 Series 는 어떻게 생성되는가? 
iterrows 매서드 내부에, 새로운 Series 를 만들어주는 구조가 있는건가?
> 

<br/>

```python
#pandas/core/frame.py

class DataFrame(NDFrame, OpsMixin):
		... 
		def iterrows(self) -> Iterable[tuple[Hashable, Series]]:
				columns = self.columns
		    klass = self._constructor_sliced # pandas.core.series
		    for k, v in zip(self.index, self.values):
			    s = klass(v, index=columns, name=k)
				  yield k, s
```

- 함수 안에서 yield를 사용하면 함수는 `제너레이터`가 되며 yield 에는 값(변수)을 지정
- `제너레이터`는 `제너레이터` 객체에서 **`next`** 호출할 때마다 함수 안의 yield 까지 코드를 실행하며 yield에서 값을 발생시킵니다(generate). 그래서 이름이 `제너레이터(generator)`
- 즉, yield 를 이용해 iterator 구현 가능하다.

[yield 와 generator 에 대한 개념](https://dojang.io/mod/page/view.php?id=2412) 

- `klass` 메서드는 어떤 내용인가?  ⇒ `_constructor_sliced`

<br/>

```python
#pandas/core/frame.py
class DataFrame(NDFrame, OpsMixin):
		... 
		_constructor_sliced: type[Series] = Series
```

- Series 를 반환한다.
- klass 는 `klass(v, index=columns, name=k)` 이와 같이 Series 를 초기화 하는 함수이다.


<br/>


### `iterrows` 의 내용을 정리하자면

- *Iterate over DataFrame rows as (index, Series) pairs.*

사용시에 type 이 변하는 문제가 있을 수 있다. 

- *```iterrows``` returns a Series for each row*
- it does not preserve dtypes across the rows 
(dtypes are preserved across columns for DataFrames.)

```python
df = pd.DataFrame([[1, 1.5], [4, 4.5]], columns=['int', 'float'])
print(df.head(5))

row = next(df.iterrows())[1]
"""
int  float
0    1    1.5
1    4    4.5

int      1.0  # type 변화함을 알 수 있다. 
float    1.5
Name: 0, dtype: float64
"""
```

- int 컬럼의 데이터 값의 type 에 변화가 있음을 알 수 있다.
    - int ⇒ float64

<br/>

번외) tuple 을 unpack 하는 방법

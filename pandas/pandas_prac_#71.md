# data set 준비
## 데이터 프레임 다루기 실습

```python
# check nan value in dataframe -> return boolean
df.isna()

# delete the row which contains nan value 
clean_df = df.dropna()

pd.options.display.float_format = '{:,.2f}'.format
```

- 결측치 체크
- display format 설정

<br/>

```python
# 1. 
df.loc[df['Mid-Career Median Salary'].idxmax(), 'Undergraduate Major']

# 2.
df.loc[df['Starting Median Salary'].idxmin(), 'Undergraduate Major']

# 3. 
df.loc[df['Mid-Career Median Salary'].idxmin(), 'Mid-Career 10th Percentile Salary']
```

- `idxmax`, `idxmin` 을 통해 조건에 해당되는 col 의 index 값 구하기.
- The largest and smallest values, as well as their positions, can be found with methods like `.max()`, `.min()`, `.idxmax()` and `.idxmin()`
- You can sort the DataFrame with `.sort_values()` and add new columns with `.insert()`

<br/>

```python
# subtract : return series 
test = df['Mid-Career 10th Percentile Salary'].subtract(df['Mid-Career 90th Percentile Salary'])
# test = df['Mid-Career 10th Percentile Salary'] - df['Mid-Career 90th Percentile Salary']

# insert : dataframe 에 column 추가 
df.insert(1, 'spread', test)

# 정렬 
df.sort_values('spread', ascending=False)
```

- 각 `series` 에 대해서 뺄셈
- 새로운 `column` 추가 가능
- shift + tab 으로 도움말 보기

> sort_values 구현 방식이 궁금하다!!! 
반환값???
> 

```python
# 1. 
ddf = df.sort_values('Mid-Career 90th Percentile Salary', ascending=False).head(5)
ddf[['Undergraduate Major', 'Mid-Career 90th Percentile Salary']]

# 2. 
test =  df.sort_values('spread')

test[['Undergraduate Major', 'spread']].head(1) # 방법1.
test.loc[test['spread'].idxmin(), ['Undergraduate Major', 'spread']] # 방법2.
```

- dataframe 에서 원하는 column 여러개 추출하기.

<br/>

```python
## 집계 
# 개수 세기 
clean_df.groupby('Group').count()

# 평균 
clean_df.groupby('Group').mean()

# 통계
clean_df.groupby('Group').describe()
```

- To create an Excel Style Pivot Table by grouping entries that belong to a particular category use the `.groupby()` method

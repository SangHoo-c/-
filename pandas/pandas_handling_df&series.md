# python with

```python
# 1. 
with open('textfile.txt', 'r') as file:
    contents = file.read()

# 2. 
file = open('textfile.txt', 'r')
contents = file.read()
file.close()
```

- 1, 2 같은 결과
- 2 의 `read` 과정에서 오류가 나면 `close` 를 하지 못하는 에러를 1번에서 방지 가능
- 객체가 `__enter__` 와 `__exit__` 를 가지면 `with` 사용 가능

<br/>

# reading CSV

```python
with open('weather_data.csv') as csvfile:
    data = csv.reader(csvfile)
    for row in data:
        print(row)
```

- `csv` 모듈 사용
- `readlines` 메서드 사용하는 것 보다 편하다.
- But, 행이 많아진다면? 다루기 힘들다.
    - **pandas 필요함!**
    

<br/>

```python
import pandas as pd

pdf = pd.read_csv("weather_data.csv")
pdf # <class 'pandas.core.frame.DataFrame'>
```

- 행 / 열에 맞게 잘 나온다.
- `read_csv` : 첫번째 line 이 column 의 이름임을 스스로 체크한다.

<br/>

## Series & DataFrame

The two primary data structures of pandas

- **`[Series](https://pandas.pydata.org/docs/reference/api/pandas.Series.html#pandas.Series)`** (1-dimensional)
- **`[DataFrame](https://pandas.pydata.org/docs/reference/api/pandas.DataFrame.html#pandas.DataFrame)`** (2-dimensional)

```python
pdf = pd.read_csv("weather_data.csv")

data_dict = pdf.to_dict() # DF -> dict 
temp_list = pdf['temp'].to_list() # Series -> list 
```

- 다양한 Conversion 지원

<br/>

```python
sum(temp_list) / len(temp_list)
pdf['temp'].mean()
```

- 편리한 통계 방식 지원

<br/>

```python
pdf['temp']
pdf.temp
```

- 같은 결과이다.
- pandas 의 column 값은 attribute 로 갖기 때문

<br/>

```python
# get max temp rows  
a = pdf[pdf.temp == pdf.temp.max()]
```

- 특정 `column` 이 `max` 인 행 **모두** 고르기

<br/>

```python
pdf["Primary Fur Color"].value_counts()
```

- 특정 기준으로 `count` 집계하기

<br/>

```python
pdf = pd.read_csv("Data.csv")
ndf = pdf["Primary Fur Color"].value_counts()

# 방법 1. 
nndf = pd.DataFrame(ndf, columns=['Primary Fur Color', 'count'])

# 방법 2. 
nndf2 = pd.DataFrame({
    'Primary Fur Color': ndf.index,
    'count': ndf.values
})
```

- DF 에서 특정 `column`의 빈도수를 CSV 로 변환하는 과정
- `방법 1` 사용시 index 설정으로 데이터가 한칸씩 밀리는 오류 발생
    - index=None 등, 여러 시도를 해보았으나 해결 못함.
    - Series 로 DF 을 만드는 경우, 이러한 이슈가 있음을 인지. dict 사용을 해야겠다.
    
    ```python
    # 방법 1. nndf 결과 
    Primary Fur Color count
    Gray                   2473   NaN
    Cinnamon                392   NaN
    Black                   103   NaN
    ```
    
- `방법 2` 를 사용하면, 깔끔하게 잘 됨.
    - `column` 원하는 이름으로 변경 가능

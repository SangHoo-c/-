# spark conf 

목적 : 업무에 사용되는 spark conf 를 파악하기 위함 

방법 : 설정값 하나씩 문서로 정리 







```bash
# 깨진 PARQUET 파일 처리 
conf.set("spark.sql.files.ignoreCorruptFiles","true")

# Timeout in seconds for the broadcast wait time in broadcast joins, default 300 
conf.set("spark.sql.broadcastTimeout", "36000")

# dynamic partition 설정  
conf.set("spark_sql.sources.partitionOverwriteMode", "dynamic")
conf.set("hive.exec.dynamic.partition", "true")
conf.set("hive.exec.dynamic.partition.mode", "nonstrict")

# shuffle partition 
conf.set("spark.sql.shuffle.partitions", "200")

# 교차조인 
conf.set("spark.sql.crossJoin.enabled", "true")
```

<br/>

<br/>


## sql.files.ignoreCorruptFiles

- 하나의 parquet파일의 footer가 손상되어 파일을 읽어오지 못할경우, 전체 과정이 멈춤
- 데이터를 사용중에 parquet file 의 손상여부를 알 수 없다. 
  - `path`중 첫 번째 파일의 footer가 정상이라면 `path` 중 한 파일이 문제가 있다고 가정.  
  - Spark의 lazy loading, lazy computing으로 인해 `.show()`나 `.count()`와 같이 실제 데이터가 필요한 코드를 실행하기 전까지는 데이터를 불러오지 않고 메타게이터만 연결된 DataFrame 객체를 사용
  - 코드들이 정상적으로 작동하더라도 실제 parquet파일이 깨졌다는 사실을 알 수가 없음 
- 손상된 parquet파일 무시
  - 근본적인 해결책? 

```python
path = [
    's3a://some-bucket/normal1.parquet', # 정상
    's3a://some-bucket/normal2.parquet', 
    's3a://some-bucket/normal3.parquet', 
    's3a://some-bucket/brokenfile.parquet', # Broken!
    's3a://some-bucket/normal4.parquet', 
    # ...
]

df = spark.read.parquet(*path) # 정상적으로 실행된다.
```

참고자료 : https://docs.microsoft.com/ko-kr/azure/databricks/kb/data/match-parquet-schema



<br/>

## spark_sql.sources.partitionOverwriteMode

- spark 2.3 이후 

- Spark 에서는 다음의 옵션을 통해 Hive 의 Dynamic Partition 을 활성화 할 수 있다.
- partitioned dataset 에서 `원하는 partition` 만 overwrite 하려고 한다면, 이 옵션을 사용해야한다. 
  - 기본값이 static. 
  - static 설정이라면, 생성되는 파티션 외에는 삭제 ...
  - 따라서, 나머지 전체 파티션이 삭제되는 것을 막기 위해 꼭! 설정 해야함. 

```python
spark.conf.set("spark.sql.sources.partitionOverwriteMode","dynamic")
data.write.insertInto("partitioned_table", overwrite=True)
```

<br/>


#### Hive Partition 

partition 이 왜 필요할까? 

> PARTITION 이란 일종의 디렉토리, 즉 데이터의 구획을 나누는 역할 

- 대량의 데이터가 존재하는 테이블에서 필요한 부분만 (= 요청한 구획, 즉 요청한 파티션만) 탐색 가능 
- 파티션이 없다면 원하는 데이터를 찾기 위해 전체 구획내 데이터를 하나씩 다 읽어야,,, 



Hive partition 의 종류 

- Dynamic Partition 
  - spark / hive 가 데이터를 기반으로 파티션을 판단 
  - partitionBy("dt") 
- Static Partition 
  - 사용자가 직접 파티션 지정 
  - dt = '20211101' 

참고자료 

- https://stackoverflow.com/q/50006526
- nice docs : https://1ambda.blog/2021/12/27/practical-spark-sql-table-8/ 


<br/>




## hive.exec.dynamic.partition & hive.exec.dynamic.partition.mode 

```python
hiveContext.setConf("hive.exec.dynamic.partition", "true")
hiveContext.setConf("hive.exec.dynamic.partition.mode", "nonstrict")

df.write().mode(SaveMode.Append).partitionBy("colname").saveAsTable("Table")
```

- dynamic partition the hive table 
- 기존 컬럼 기준으로 PARTITION 
- dynamic partition the dataframe and store it to hive warehouse 하기 위해서 해당 설정 해줘야한다. 

<br/>



## spark.sql.crossJoin.enabled

- spark >= 3.0 

  - `spark.sql.crossJoin.enable` is true by default.

- spark >= 2.1 

  ```
  df1.crossJoin(df2)
  ```

- spark 2.0 

  ```python
  spark.conf.set("spark.sql.crossJoin.enabled", true)
  ```

  <br/>



**Cartesian product 란 ?** 

-  From절에 2개 이상의 Table이 있을때,
- 두 Table 사이에 **유효 join 조건을 적지 않았을때**,
- 해당 테이블에 대한 모든 데이터를 전부 결합하여 
  - Table에 존재하는 행 갯수를 곱한 만큼의 결과값이 반환되는 것 
- join 쿼리 중에 WHERE 절에 기술하는 join 조건이 `잘못` 기술되었거나 아예 없을 경우 발생하는 현상

<br/>

**crossjoin 이란 ?** 

- 모든 행을 단순히 join 하는 방법 
- 테이블에서 나올 수 있는 행의 모든 조합을 다 만들어낼 수 있다. 
  - cartesian product 라 한다. 
- 모든 경우를 고려할때! 사용 



참고자료 : https://stackoverflow.com/a/39000050





